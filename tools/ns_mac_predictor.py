#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Cortex-M55 Helium PMU-theory counters for MAC-heavy kernels with fused activations.
and **elementwise** operators (using Helium vector ALU).

Events modeled:
- MVE_INT_MAC_RETIRED     : integer multiply/MAC instructions retired (MAC/mul only)
- MVE_VREDUCE_INT_RETIRED : integer vector **reduction** instructions retired
- MVE_PRED                : cycles with predicated beats
    (Modeled as 2 cycles per predicated MVE instruction on Cortex-M55.)

Supported ops:
- fully_connected
- conv2d
- depthwise_conv2d
- elementwise (ADD/SUB/MUL/DIV/MAXIMUM/MINIMUM/ABS/NEG/SQRT/RSQRT/EXP/LOG/SIN/COS/
               TANH/LOGISTIC/RELU/RELU6/RELU_N1_TO_1/LEAKY_RELU/HARD_SWISH/
               SQUARE/POWER/FLOOR/CEIL/ROUND/CLAMP/PAD/SOFTMAX*)
- max_pool_2d
- mean   (ReduceMean / AveragePool)

Supported element widths (activations & weights):
- 4, 8, 16, 32

Fused activations:
- none      : default (no change)
- relu      : per-vector VMAX with zero (1 vector op per chunk; no INT_MAC/REDUCE)
- relu6     : VMAX(0), then VMIN(6) (2 vector ops)
- clamp     : configurable clamp_min / clamp_max -> up to 2 vector ops
  (All above only affect MVE_PRED via tail predication; no INT_MAC/REDUCE deltas.)

Assumptions:
- Helium vector width = 128 bits. Lanes = 16/8/4 for 8/16/32-bit respectively.
- 4-bit is modeled as widened to 8-bit lanes for lane math (unpack overhead not modeled).
- 8/16-bit MACs use reduction-MAC (e.g., VMLADAVA); 32-bit uses per-lane VMLA/VMUL + 1 scalar reduce.
- Activations are applied **across channels (Co)** per output spatial location; tail predication occurs once
  per (N, Ho, Wo) position if `Co % L != 0` (or Co < L).
- To model a different activation vectorization axis, adjust `_activation_tail_instances_*`.

You can tune constants/policies below to match your kernels.
"""

from dataclasses import dataclass
from typing import Optional, Tuple, Dict, Any, Union
import math


# ---- Tunables -----------------------------------------------------------------

# PRED cycles contributed by a single **predicated** MVE instruction on Cortex-M55.
PRED_CYCLES_PER_PREDICATED_INSTR = 2

# For int4, model as widened to 8-bit (i.e., use 16 lanes effective).
INT4_STRATEGY = "widen_to_8bit"  # choices: "widen_to_8bit"


# ---- Data structures -----------------------------------------------------------

@dataclass(frozen=True)
class FCSpec:
    # Fully-connected
    batch: int          # N
    cin: int            # input length
    cout: int           # outputs

@dataclass(frozen=True)
class Conv2DSpec:
    # Standard conv2d: N,H,W,Cin with filters Kh,Kw,Cin,Cout
    n: int
    h: int
    w: int
    cin: int
    kh: int
    kw: int
    cout: int
    stride: Tuple[int, int] = (1, 1)
    padding: str = "same"          # "same" or "valid"
    dilation: Tuple[int, int] = (1, 1)
    out_shape: Optional[Tuple[int, int, int, int]] = None  # (N, Ho, Wo, Co)

@dataclass(frozen=True)
class DepthwiseConv2DSpec:
    # Depthwise conv2d: N,H,W,Cin with filters Kh,Kw,Cin,DepthMultiplier
    n: int
    h: int
    w: int
    cin: int
    kh: int
    kw: int
    depth_multiplier: int = 1
    stride: Tuple[int, int] = (1, 1)
    padding: str = "same"          # "same" or "valid"
    dilation: Tuple[int, int] = (1, 1)
    out_shape: Optional[Tuple[int, int, int, int]] = None  # (N, Ho, Wo, Co)

@dataclass(frozen=True)
class ActivationSpec:
    """
    Fused activation description.

    type: "none" | "relu" | "relu6" | "clamp"
    bits: element width used for activation vector ops; defaults to effective compute width
    clamp_min / clamp_max: integers (interpreted in the chosen 'bits' range); None means unbounded.
    """
    type: str = "none"
    bits: Optional[int] = None
    clamp_min: Optional[int] = None
    clamp_max: Optional[int] = None

@dataclass(frozen=True)
class ElementwiseSpec:
    """
    Elementwise operation description, intended to be fed by analyzer hints.
      - elems: total number of output elements (analyzer Output_Magnitude)
      - op_name: TFLite op name (e.g., 'RELU', 'ADD', 'SOFTMAX', ...)
      - out_shape: optional (N,Ho,Wo,Co) for ops that require a per-position reduction (SOFTMAX)
      - vec_chunks_hint: optional analyzer 'ElemVecChunks'
      - predicated_hint: optional analyzer 'ElemPredicatedInstrs' (0/1)
      - clamp_min/max: for CLAMP, to determine 1 vs 2 vector ops per chunk
      - out_dtype: optional ('int8','int16','int32','float32',...) used to decide requant multiply in MUL
      - window_elems / reduce_axis_len: optional sizes for pooling / mean if known
    """
    elems: int
    op_name: str = "GENERIC"
    out_shape: Optional[Tuple[int, int, int, int]] = None
    vec_chunks_hint: Optional[int] = None
    predicated_hint: Optional[int] = None
    clamp_min: Optional[int] = None
    clamp_max: Optional[int] = None
    out_dtype: Optional[str] = None
    window_elems: Optional[int] = None
    reduce_axis_len: Optional[int] = None

# ---- Helpers ------------------------------------------------------------------

def _effective_elem_bits(act_bits: int, w_bits: int) -> int:
    a = 8 if act_bits == 4 else act_bits
    w = 8 if w_bits == 4 else w_bits
    eff = max(a, w)
    if eff not in (8, 16, 32):
        raise ValueError(f"Unsupported element width: act_bits={act_bits}, w_bits={w_bits}")
    return eff

def _activation_elem_bits(act_bits: int, w_bits: int, activation: Optional[ActivationSpec]) -> int:
    if activation and activation.bits:
        b = 8 if activation.bits == 4 else activation.bits
        if b not in (8, 16, 32):
            raise ValueError(f"Unsupported activation bits: {activation.bits}")
        return b
    return _effective_elem_bits(act_bits, w_bits)

def _lanes_for_width(bits: int) -> int:
    return 128 // bits

def _ceil_div(a: int, b: int) -> int:
    return (a + b - 1) // b

def _conv_out_dim(in_size: int, k: int, stride: int, dilation: int, padding: str) -> int:
    eff_k = dilation * (k - 1) + 1
    if padding.lower() == "same":
        return math.ceil(in_size / stride)
    elif padding.lower() == "valid":
        return max(0, (in_size - eff_k) // stride + 1)
    else:
        raise ValueError(f"Unsupported padding: {padding}")

def _conv2d_output_shape(spec: Conv2DSpec) -> Tuple[int, int, int, int]:
    if spec.out_shape is not None:
        return spec.out_shape
    Ho = _conv_out_dim(spec.h, spec.kh, spec.stride[0], spec.dilation[0], spec.padding)
    Wo = _conv_out_dim(spec.w, spec.kw, spec.stride[1], spec.dilation[1], spec.padding)
    return (spec.n, Ho, Wo, spec.cout)

def _dw_output_shape(spec: DepthwiseConv2DSpec) -> Tuple[int, int, int, int]:
    if spec.out_shape is not None:
        return spec.out_shape
    Ho = _conv_out_dim(spec.h, spec.kh, spec.stride[0], spec.dilation[0], spec.padding)
    Wo = _conv_out_dim(spec.w, spec.kw, spec.stride[1], spec.dilation[1], spec.padding)
    Co = spec.cin * spec.depth_multiplier
    return (spec.n, Ho, Wo, Co)

def _activation_ops_per_vector(actv: Optional[ActivationSpec]) -> int:
    """Number of vector ALU ops per activation vector chunk (diagnostic)."""
    if actv is None or actv.type == "none":
        return 0
    t = actv.type.lower()
    if t == "relu":
        return 1
    if t == "relu6":
        return 2
    if t == "clamp":
        # 1 op for min bound (VMAX), 1 for max bound (VMIN)
        ops = 0
        if actv.clamp_min is not None:
            ops += 1
        if actv.clamp_max is not None:
            ops += 1
        return ops
    raise ValueError(f"Unsupported activation type: {actv.type}")

def _activation_tail_instances_fc(N: int, Co: int, L: int) -> int:
    """Assume activation vectorizes across channels => tail once per batch item if Co%L!=0."""
    return N if (Co % L != 0 and Co > 0) else 0

def _activation_tail_instances_conv(N: int, Ho: int, Wo: int, Co: int, L: int) -> int:
    """Tail once per (n,y,x) position if Co%L!=0."""
    positions = N * Ho * Wo
    return positions if (Co % L != 0 and Co > 0) else 0


# ---- Core MAC/reduction modeling ----------------------------------------------

def _counts_for_output_scalar(K: int, elem_bits: int) -> Dict[str, int]:
    """
    Counts for producing ONE scalar output (dot product length K) at given element width:
      - 8/16-bit: use reduction-MAC (VMLADAVA*):
            reduce_insts = ceil(K / L)
            INT_MAC      = reduce_insts
            VREDUCE      = reduce_insts
            PRED         = 2 if tail (K%L!=0 or K<L) else 0
      - 32-bit: per-lane VMLA/VMUL, then 1 scalar reduce:
            INT_MAC      = ceil(K / L)
            VREDUCE      = 1 if K>0 else 0
            PRED         = 2 if tail else 0
    """
    L = _lanes_for_width(elem_bits)

    if elem_bits in (8, 16):
        reduce_insts = _ceil_div(K, L) if K > 0 else 0
        predicated_reductions = 1 if (K > 0 and (K % L != 0 or K < L)) else 0
        return {
            "MVE_INT_MAC_RETIRED": reduce_insts,
            "MVE_VREDUCE_INT_RETIRED": reduce_insts,
            "MVE_PRED": predicated_reductions * PRED_CYCLES_PER_PREDICATED_INSTR,
            "lanes": L,
            "vector_chunks": reduce_insts,
            "predicated_instrs": predicated_reductions,
        }

    elif elem_bits == 32:
        vmla_insts = _ceil_div(K, L) if K > 0 else 0
        predicated_vmla = 1 if (K > 0 and (K % L != 0 or K < L)) else 0
        vreduce = 1 if vmla_insts > 0 else 0
        return {
            "MVE_INT_MAC_RETIRED": vmla_insts,
            "MVE_VREDUCE_INT_RETIRED": vreduce,
            "MVE_PRED": predicated_vmla * PRED_CYCLES_PER_PREDICATED_INSTR,
            "lanes": L,
            "vector_chunks": vmla_insts,
            "predicated_instrs": predicated_vmla,
        }

    else:
        raise ValueError(f"Unexpected elem_bits: {elem_bits}")

def _is_quantized_out_dtype(out_dtype: Optional[str]) -> bool:
    """True if the layer's output dtype implies an int requantization step."""
    if not out_dtype:
        return False
    od = str(out_dtype).strip().lower()
    return od in ("int8", "uint8", "int16")

def _requant_vec_chunks_for_outputs(N: int, Ho: int, Wo: int, Co: int) -> int:
    """
    Requantization (post-accum) typically runs on 32-bit lanes (e.g., VQRDMULH).
    Model this as 1 vector multiply per output vector chunk across channels.
    """
    if N <= 0 or Ho < 0 or Wo < 0 or Co <= 0:
        return 0
    L_post = _lanes_for_width(32)  # 32-bit lanes for requant math
    positions = max(1, N * max(1, Ho) * max(1, Wo))
    return positions * _ceil_div(Co, L_post)

# ---- Op-specific counts + fused activation ------------------------------------

def fc_counts(spec: FCSpec, act_bits: int, w_bits: int, activation: Optional[ActivationSpec]=None, out_dtype: Optional[str]=None) -> Dict[str, Any]:
    elem_bits = _effective_elem_bits(act_bits, w_bits)
    total_outputs = spec.batch * spec.cout
    K = spec.cin

    per_out = _counts_for_output_scalar(K, elem_bits)

    # Base (MAC/reduction) scaled by outputs
    counts = {
        "MVE_INT_MAC_RETIRED": per_out["MVE_INT_MAC_RETIRED"] * total_outputs,
        "MVE_VREDUCE_INT_RETIRED": per_out["MVE_VREDUCE_INT_RETIRED"] * total_outputs,
        "MVE_PRED": per_out["MVE_PRED"] * total_outputs,
        "MACs": total_outputs * K,
        "effective_bits": elem_bits,
        "lanes": per_out["lanes"],
        "vector_chunks_total": per_out["vector_chunks"] * total_outputs,
        "predicated_instrs_total": per_out["predicated_instrs"] * total_outputs,
        "outputs": total_outputs,
        "K_per_output": K,
    }

    # Post-op requantization (vector int multiply), if quantized output.
    if _is_quantized_out_dtype(out_dtype):
        # Vectorize across Cout per batch; requant uses 32-bit lanes.
        rq_vec_chunks = spec.batch * _ceil_div(spec.cout, _lanes_for_width(32))
        counts["MVE_INT_MAC_RETIRED"] += rq_vec_chunks
        counts.setdefault("_diag_requant_vec_chunks", rq_vec_chunks)

    # Activation contribution (PRED only, from tail across Cout per batch)
    ops_per_vec = _activation_ops_per_vector(activation)
    if ops_per_vec > 0:
        act_bits_eff = _activation_elem_bits(act_bits, w_bits, activation)
        L_act = _lanes_for_width(act_bits_eff)
        tails = _activation_tail_instances_fc(spec.batch, spec.cout, L_act)
        counts["MVE_PRED"] += tails * PRED_CYCLES_PER_PREDICATED_INSTR * ops_per_vec
        # Diagnostics
        vec_chunks = spec.batch * _ceil_div(spec.cout, L_act)
        counts["_diag_activation_vec_ops_total"] = vec_chunks * ops_per_vec
        counts["_diag_activation_predicated_instrs_total"] = tails * ops_per_vec
        counts["_diag_activation_bits"] = act_bits_eff
        counts["_diag_activation_lanes"] = L_act

    return counts


def conv2d_counts(spec: Conv2DSpec, act_bits: int, w_bits: int, activation: Optional[ActivationSpec]=None, out_dtype: Optional[str]=None) -> Dict[str, Any]:
    elem_bits = _effective_elem_bits(act_bits, w_bits)
    N, Ho, Wo, Co = _conv2d_output_shape(spec)
    total_outputs = N * Ho * Wo * Co
    K = spec.kh * spec.kw * spec.cin

    per_out = _counts_for_output_scalar(K, elem_bits)

    counts = {
        "MVE_INT_MAC_RETIRED": per_out["MVE_INT_MAC_RETIRED"] * total_outputs,
        "MVE_VREDUCE_INT_RETIRED": per_out["MVE_VREDUCE_INT_RETIRED"] * total_outputs,
        "MVE_PRED": per_out["MVE_PRED"] * total_outputs,
        "MACs": total_outputs * K,
        "effective_bits": elem_bits,
        "lanes": per_out["lanes"],
        "vector_chunks_total": per_out["vector_chunks"] * total_outputs,
        "predicated_instrs_total": per_out["predicated_instrs"] * total_outputs,
        "outputs": total_outputs,
        "K_per_output": K,
        "output_shape": (N, Ho, Wo, Co),
    }

    # Post-op requantization (vector int multiply), if quantized output.
    if _is_quantized_out_dtype(out_dtype):
        rq_vec_chunks = _requant_vec_chunks_for_outputs(N, Ho, Wo, Co)
        counts["MVE_INT_MAC_RETIRED"] += rq_vec_chunks
        counts.setdefault("_diag_requant_vec_chunks", rq_vec_chunks)

    ops_per_vec = _activation_ops_per_vector(activation)
    if ops_per_vec > 0:
        act_bits_eff = _activation_elem_bits(act_bits, w_bits, activation)
        L_act = _lanes_for_width(act_bits_eff)
        tails = _activation_tail_instances_conv(N, Ho, Wo, Co, L_act)
        counts["MVE_PRED"] += tails * PRED_CYCLES_PER_PREDICATED_INSTR * ops_per_vec
        vec_chunks = N * Ho * Wo * _ceil_div(Co, L_act)
        counts["_diag_activation_vec_ops_total"] = vec_chunks * ops_per_vec
        counts["_diag_activation_predicated_instrs_total"] = tails * ops_per_vec
        counts["_diag_activation_bits"] = act_bits_eff
        counts["_diag_activation_lanes"] = L_act

    return counts


def depthwise_conv2d_counts(spec: DepthwiseConv2DSpec, act_bits: int, w_bits: int, activation: Optional[ActivationSpec]=None, out_dtype: Optional[str]=None) -> Dict[str, Any]:
    elem_bits = _effective_elem_bits(act_bits, w_bits)
    N, Ho, Wo, Co = _dw_output_shape(spec)
    total_outputs = N * Ho * Wo * Co
    K = spec.kh * spec.kw * 1  # depthwise per-channel

    per_out = _counts_for_output_scalar(K, elem_bits)

    counts = {
        "MVE_INT_MAC_RETIRED": per_out["MVE_INT_MAC_RETIRED"] * total_outputs,
        "MVE_VREDUCE_INT_RETIRED": per_out["MVE_VREDUCE_INT_RETIRED"] * total_outputs,
        "MVE_PRED": per_out["MVE_PRED"] * total_outputs,
        "MACs": total_outputs * K,
        "effective_bits": elem_bits,
        "lanes": per_out["lanes"],
        "vector_chunks_total": per_out["vector_chunks"] * total_outputs,
        "predicated_instrs_total": per_out["predicated_instrs"] * total_outputs,
        "outputs": total_outputs,
        "K_per_output": K,
        "output_shape": (N, Ho, Wo, Co),
    }

    # Post-op requantization (vector int multiply), if quantized output.
    if _is_quantized_out_dtype(out_dtype):
        rq_vec_chunks = _requant_vec_chunks_for_outputs(N, Ho, Wo, Co)
        counts["MVE_INT_MAC_RETIRED"] += rq_vec_chunks
        counts.setdefault("_diag_requant_vec_chunks", rq_vec_chunks)

    ops_per_vec = _activation_ops_per_vector(activation)
    if ops_per_vec > 0:
        act_bits_eff = _activation_elem_bits(act_bits, w_bits, activation)
        L_act = _lanes_for_width(act_bits_eff)
        tails = _activation_tail_instances_conv(N, Ho, Wo, Co, L_act)
        counts["MVE_PRED"] += tails * PRED_CYCLES_PER_PREDICATED_INSTR * ops_per_vec
        vec_chunks = N * Ho * Wo * _ceil_div(Co, L_act)
        counts["_diag_activation_vec_ops_total"] = vec_chunks * ops_per_vec
        counts["_diag_activation_predicated_instrs_total"] = tails * ops_per_vec
        counts["_diag_activation_bits"] = act_bits_eff
        counts["_diag_activation_lanes"] = L_act

    return counts

# ---- Elementwise modeling -----------------------------------------------------

_ELTWISE_ONE_OP = {
    "ADD","SUB","MUL","DIV","MAXIMUM","MINIMUM","ABS","NEG",
    "SQRT","RSQRT","EXP","LOG","SIN","COS","TANH","LOGISTIC",
    "RELU","RELU_N1_TO_1","SQUARE","POWER","FLOOR","CEIL","ROUND"
}
_ELTWISE_TWO_OPS = {
    "RELU6","LEAKY_RELU","HARD_SWISH"
}
# CLAMP: 1 or 2 depending on bounds; PAD: 0 (modeled as memory movement)

def _ops_per_elementwise_vector(op: str, clamp_min: Optional[int], clamp_max: Optional[int]) -> int:
    t = (op or "GENERIC").upper()
    if t in _ELTWISE_ONE_OP:
        return 1
    if t in _ELTWISE_TWO_OPS:
        return 2
    if t == "CLAMP":
        ops = 0
        if clamp_min is not None: ops += 1
        if clamp_max is not None: ops += 1
        return ops
    if t == "PAD":
        return 0
    if t == "SOFTMAX":
        # Elementwise phase (exp/scale): assume 1 vector op per chunk.
        # Reduction is handled separately below.
        return 1
    # Default conservative assumption: 1 vector op per chunk
    return 1

def _intmacs_per_vec_for_mul(elem_bits: int, out_dtype: Optional[str]) -> int:
    """
    Heuristic per-vector INT multiply count for MUL kernels commonly used in quantized TFLM:
      - For int8/int16 paths: VMULLB + VMULLT => 2 muls/vec
        If output is quantized (int8/int16), requant often multiplies again on each half => +2.
      - For int32 paths: VMULQ.S32 => 1 mul/vec; add +1 if requant multiply present (rare).
    """
    od = (out_dtype or "").lower()
    quant_out = od in ("int8","uint8","int16")
    if elem_bits in (8,16):
        return 2 + (2 if quant_out else 0)
    elif elem_bits == 32:
        return 1 + (1 if quant_out else 0)
    return 0


def elementwise_counts(spec: ElementwiseSpec, elem_bits: int) -> Dict[str, Any]:
    """
    Predict counters for elementwise operators.
      - INT_MAC: 0
      - VREDUCE: 0 except for SOFTMAX (1 reduction pass per (N,Ho,Wo))
      - PRED cycles: predicated tail beats × ops_per_vec × PRED_CYCLES_PER_PREDICATED_INSTR
    """
    if elem_bits not in (8,16,32):
        raise ValueError(f"Unsupported elementwise width: {elem_bits}")
    L = _lanes_for_width(elem_bits)
    vec_chunks = spec.vec_chunks_hint if (spec.vec_chunks_hint is not None) else (_ceil_div(spec.elems, L) if spec.elems>0 else 0)
    predicated_instrs = spec.predicated_hint if (spec.predicated_hint is not None) else (1 if (spec.elems>0 and (spec.elems % L != 0)) else 0)


    op_upper = (spec.op_name or "").upper()
    ops_per_vec = _ops_per_elementwise_vector(op_upper, spec.clamp_min, spec.clamp_max)
    pred_cycles = predicated_instrs * ops_per_vec * PRED_CYCLES_PER_PREDICATED_INSTR

    vreduce = 0
    intmacs = 0
    # Some elementwise ops do use multiplies (tick INT_MAC):
    if op_upper in ("MUL","SQUARE","LEAKY_RELU","HARD_SWISH","POWER"):
        # Multiply beats per vector:
        if op_upper == "MUL":
            intmacs = (_intmacs_per_vec_for_mul(elem_bits, spec.out_dtype) * max(0, vec_chunks))
        elif op_upper == "SQUARE":
            # x*x → 1 mul/vec (requant multiply if quantized)
            od = (spec.out_dtype or "").lower()
            intmacs = (1 + (1 if od in ("int8","uint8","int16") else 0)) * max(0, vec_chunks)
        elif op_upper == "LEAKY_RELU":
            # y = x for x>=0 else a*x  → ≈ 1 mul/vec
            intmacs = 1 * max(0, vec_chunks)
        elif op_upper == "HARD_SWISH":
            # y = x * relu6(x+3)/6  → ≈ 2 mul/vec
            intmacs = 2 * max(0, vec_chunks)
        elif op_upper == "POWER":
            # Heuristic: at least 1 multiply per vec (more if exponent not power-of-two)
            intmacs = 1 * max(0, vec_chunks)

    softmax_reduce_chunks = 0
    softmax_predicated_instrs = 0
    if op_upper == "SOFTMAX":
        # One "bounds" reduction per position across last dimension (Co)
        if spec.out_shape is not None and len(spec.out_shape) == 4:
            N, Ho, Wo, Co = spec.out_shape
            positions = max(1, N*Ho*Wo)
            softmax_reduce_chunks = positions * _ceil_div(Co, L) if Co>0 else 0
            vreduce += softmax_reduce_chunks
            # Tail predication once per position if Co%L!=0
            softmax_predicated_instrs = positions if (Co>0 and (Co % L != 0)) else 0
            pred_cycles += softmax_predicated_instrs * PRED_CYCLES_PER_PREDICATED_INSTR
        else:
            # Fallback if we don't know Co: reuse vector chunk/tail hints
            softmax_reduce_chunks = vec_chunks
            vreduce += softmax_reduce_chunks
            softmax_predicated_instrs = predicated_instrs
            pred_cycles += softmax_predicated_instrs * PRED_CYCLES_PER_PREDICATED_INSTR

    return {
        "MVE_INT_MAC_RETIRED": int(intmacs),
        "MVE_VREDUCE_INT_RETIRED": vreduce,
        "MVE_PRED": pred_cycles,
        "MACs": 0,
        "effective_bits": elem_bits,
        "lanes": L,
        "vector_chunks_total": vec_chunks * max(1, ops_per_vec),
        "predicated_instrs_total": predicated_instrs * max(1, ops_per_vec) + softmax_predicated_instrs,
        "outputs": spec.elems,
        "K_per_output": 0,
        "_diag_elementwise_op": (spec.op_name or "").upper(),
        "_diag_ops_per_vec": ops_per_vec,
        "_diag_softmax_reduce_chunks": softmax_reduce_chunks,
        "_diag_softmax_predicated_instrs": softmax_predicated_instrs,
    }


# ---- Pooling / Mean modeling --------------------------------------------------
def maxpool2d_counts(elems: int, elem_bits: int,
                     vec_chunks_hint: Optional[int], predicated_hint: Optional[int]) -> Dict[str, Any]:
    """
    MAX_POOL_2D: modeled as compare-only path: no INT_MAC, no VREDUCE.
    PRED cycles follow tail beats. ops_per_vec ~ 1 (vmax) per chunk; window-size compare depth not modeled (missing).
    """
    if elem_bits not in (8,16,32):
        raise ValueError(f"Unsupported width: {elem_bits}")
    L = _lanes_for_width(elem_bits)
    vec_chunks = vec_chunks_hint if (vec_chunks_hint is not None) else (_ceil_div(elems, L) if elems>0 else 0)
    predicated_instrs = predicated_hint if (predicated_hint is not None) else (1 if (elems>0 and (elems % L != 0)) else 0)
    pred_cycles = predicated_instrs * 1 * PRED_CYCLES_PER_PREDICATED_INSTR
    return {
        "MVE_INT_MAC_RETIRED": 0,
        "MVE_VREDUCE_INT_RETIRED": 0,
        "MVE_PRED": pred_cycles,
        "MACs": 0,
        "effective_bits": elem_bits,
        "lanes": L,
        "vector_chunks_total": vec_chunks,
        "predicated_instrs_total": predicated_instrs,
        "outputs": elems,
        "K_per_output": 0,
        "_diag_note": "MAX_POOL compares only; window compare depth not modeled (needs Kh, Kw).",
    }

def mean_counts(elems: int, elem_bits: int,
                vec_chunks_hint: Optional[int],
                predicated_hint: Optional[int],
                reduce_axis_len: Optional[int],
                out_shape: Optional[Tuple[int,int,int,int]]) -> Dict[str, Any]:
    """
    MEAN / AveragePool: counts reductions if we know the reduce extent:
      - VREDUCE_INT_RETIRED ≈ (#positions) * ceil(K/L)  where K is the reduced length (e.g., Kh*Kw or Co)
    Otherwise: conservative fallback with no reductions.
    """
    if elem_bits not in (8,16,32):
        raise ValueError(f"Unsupported width: {elem_bits}")
    L = _lanes_for_width(elem_bits)
    vec_chunks = vec_chunks_hint if (vec_chunks_hint is not None) else (_ceil_div(elems, L) if elems>0 else 0)
    predicated_instrs = predicated_hint if (predicated_hint is not None) else (1 if (elems>0 and (elems % L != 0)) else 0)
    pred_cycles = predicated_instrs * 1 * PRED_CYCLES_PER_PREDICATED_INSTR

    vreduce = 0
    if reduce_axis_len and out_shape and len(out_shape)==4:
        N, Ho, Wo, Co = out_shape
        positions = max(1, N*Ho*Wo)  # assuming reduction over channels; adjust if spatial reduction
        K = max(1, reduce_axis_len)
        vreduce = positions * _ceil_div(K, L)
    # else: unknown K → leave vreduce=0 (lower bound)

    return {
        "MVE_INT_MAC_RETIRED": 0,
        "MVE_VREDUCE_INT_RETIRED": int(vreduce),
        "MVE_PRED": pred_cycles,
        "MACs": 0,
        "effective_bits": elem_bits,
        "lanes": L,
        "vector_chunks_total": vec_chunks,
        "predicated_instrs_total": predicated_instrs,
        "outputs": elems,
        "K_per_output": int(reduce_axis_len or 0),
        "_diag_note": "MEAN reduction uses reduce_axis_len when provided; otherwise lower-bound.",
    }


# ---- Dispatcher ----------------------------------------------------------------

def compute_counts(
    op_type: str,
    act_bits: int,
    w_bits: int,
    *,
    fc: Optional[FCSpec] = None,
    conv2d: Optional[Conv2DSpec] = None,
    dw: Optional[DepthwiseConv2DSpec] = None,
    activation: Optional[ActivationSpec] = None,
    elementwise: Optional[ElementwiseSpec] = None,
    out_dtype: Optional[str] = None,
    pool_kind: Optional[str] = None,
    pool_output_elems: Optional[int] = None,
    pool_vec_chunks_hint: Optional[int] = None,
    pool_predicated_hint: Optional[int] = None,
    mean_reduce_axis_len: Optional[int] = None,
    mean_out_shape: Optional[Tuple[int,int,int,int]] = None,
) -> Dict[str, Any]:
    """
    op_type in {"fully_connected", "conv2d", "depthwise_conv2d", "elementwise"}
    """
    op_type = op_type.lower()

    if op_type == "fully_connected":
        if fc is None:
            raise ValueError("FCSpec required for fully_connected")
        return fc_counts(fc, act_bits, w_bits, activation, out_dtype)
    elif op_type == "conv2d":
        if conv2d is None:
            raise ValueError("Conv2DSpec required for conv2d")
        return conv2d_counts(conv2d, act_bits, w_bits, activation, out_dtype)
    elif op_type == "depthwise_conv2d":
        if dw is None:
            raise ValueError("DepthwiseConv2DSpec required for depthwise_conv2d")
        return depthwise_conv2d_counts(dw, act_bits, w_bits, activation, out_dtype)
    elif op_type == "elementwise":
        if elementwise is None:
            raise ValueError("ElementwiseSpec required for elementwise")
        # Elementwise uses only the activation/datapath width.
        elem_bits = 8 if act_bits == 4 else act_bits
        if elem_bits not in (8,16,32):
            raise ValueError(f"Unsupported elementwise bits: {act_bits}")
        return elementwise_counts(elementwise, elem_bits)
    elif op_type == "max_pool_2d":
        elem_bits = 8 if act_bits == 4 else act_bits
        return maxpool2d_counts(
            elems=int(pool_output_elems or 0),
            elem_bits=elem_bits,
            vec_chunks_hint=pool_vec_chunks_hint,
            predicated_hint=pool_predicated_hint
        )
    elif op_type == "mean":
        elem_bits = 8 if act_bits == 4 else act_bits
        return mean_counts(
            elems=int(pool_output_elems or 0),
            elem_bits=elem_bits,
            vec_chunks_hint=pool_vec_chunks_hint,
            predicated_hint=pool_predicated_hint,
            reduce_axis_len=mean_reduce_axis_len,
            out_shape=mean_out_shape
        )
    
    else:
        raise ValueError(f"Unsupported op_type: {op_type}")


# ---- Pretty-print --------------------------------------------------------------

def _fmt_counts(tag: str, d: Dict[str, Any]) -> None:
    print(f"\n[{tag}]")
    print(f"  MACs (scalar):              {d['MACs']}")
    print(f"  MVE_INT_MAC_RETIRED:        {d['MVE_INT_MAC_RETIRED']}")
    print(f"  MVE_VREDUCE_INT_RETIRED:    {d['MVE_VREDUCE_INT_RETIRED']}")
    print(f"  MVE_PRED (cycles):          {d['MVE_PRED']}")
    print(f"  outputs:                    {d['outputs']}")
    print(f"  K_per_output:               {d['K_per_output']}")
    print(f"  effective_bits / lanes:     {d['effective_bits']} / {d['lanes']}")
    print(f"  vector_chunks_total:        {d['vector_chunks_total']}")
    print(f"  predicated_instrs_total:    {d['predicated_instrs_total']}")
    if 'output_shape' in d:
        print(f"  output_shape:               {d['output_shape']}")
    # diagnostics for activation
    if "_diag_activation_vec_ops_total" in d:
        print(f"  act.vec_ops_total (diag):   {d['_diag_activation_vec_ops_total']}")
        print(f"  act.pred_instrs_total:      {d['_diag_activation_predicated_instrs_total']}")
        print(f"  act.bits / lanes:           {d['_diag_activation_bits']} / {d['_diag_activation_lanes']}")


# ---- Main (self-test scenarios) -----------------------------------------------

def main() -> None:
    # 1) FC, int8, no activation
    fc_spec = FCSpec(batch=1, cin=96, cout=2)
    d = compute_counts("fully_connected", act_bits=8, w_bits=8, fc=fc_spec)
    _fmt_counts("FC int8 (N=1, Cin=96, Cout=2) + no activation", d)

    # 2) FC, int8, ReLU fused (vectorized across Cout)
    d = compute_counts("fully_connected", act_bits=8, w_bits=8, fc=fc_spec,
                       activation=ActivationSpec(type="relu"))
    _fmt_counts("FC int8 + ReLU", d)

    # 3) Conv2D int8 example: N=1,H=1,W=500,Cin=1,Kh=7,Kw=1,Co=24,SAME
    conv_spec = Conv2DSpec(n=1, h=1, w=500, cin=1, kh=7, kw=1, cout=24, stride=(1,1), padding="same")
    d = compute_counts("conv2d", act_bits=8, w_bits=8, conv2d=conv_spec, out_dtype="int8")
    _fmt_counts("Conv2D int8 + no activation", d)

    # 4) Same Conv2D with ReLU
    d = compute_counts("conv2d", act_bits=8, w_bits=8, conv2d=conv_spec,
                       activation=ActivationSpec(type="relu"))
    _fmt_counts("Conv2D int8 + ReLU", d)

    # 5) Depthwise Conv2D int8, SAME, ReLU6
    dw_spec = DepthwiseConv2DSpec(n=1, h=1, w=500, cin=8, kh=5, kw=1, depth_multiplier=1, stride=(1,1), padding="same")
    d = compute_counts("depthwise_conv2d", act_bits=8, w_bits=8, dw=dw_spec,
                       activation=ActivationSpec(type="relu6"))
    _fmt_counts("Depthwise Conv2D int8 + ReLU6", d)

    # 5.5)) DW Conv with out_shape 1*1*500*24
    dw_spec = DepthwiseConv2DSpec(n=1, h=1, w=500, cin=8, kh=5, kw=1, depth_multiplier=1, stride=(1,1), padding="same", out_shape=(1,1,500,24))
    d = compute_counts("depthwise_conv2d", act_bits=8, w_bits=8, dw=dw_spec, out_dtype="int8",
                       activation=ActivationSpec(type="relu6"))
    _fmt_counts("Depthwise Conv2D outshape int8 + ReLU6", d)

    # 6) Conv2D int16, VALID, clamp [-128, 127] applied with 16-bit ops
    conv16 = Conv2DSpec(n=1, h=32, w=32, cin=8, kh=3, kw=3, cout=16, stride=(1,1), padding="valid")
    d = compute_counts("conv2d", act_bits=16, w_bits=16, conv2d=conv16,
                       activation=ActivationSpec(type="clamp", clamp_min=-32768, clamp_max=32767))
    _fmt_counts("Conv2D int16 + clamp", d)

    # 7) Conv2D int32 output path, apply ReLU at 32-bit (override activation bits)
    conv32 = Conv2DSpec(n=1, h=16, w=16, cin=16, kh=3, kw=3, cout=8, stride=(1,1), padding="valid")
    d = compute_counts("conv2d", act_bits=32, w_bits=32, conv2d=conv32,
                       activation=ActivationSpec(type="relu", bits=32))
    _fmt_counts("Conv2D int32 + ReLU@32b", d)

    # 8) Depthwise Conv2D mixed (act8, wt4->8), VALID, ReLU
    dw_mixed = DepthwiseConv2DSpec(n=1, h=28, w=28, cin=16, kh=5, kw=5, depth_multiplier=2, stride=(1,1), padding="valid")
    d = compute_counts("depthwise_conv2d", act_bits=8, w_bits=4, dw=dw_mixed,
                       activation=ActivationSpec(type="relu"))
    _fmt_counts("Depthwise Conv2D (act8, wt4→8) + ReLU", d)

    # 9) Elementwise ReLU on 1x32x32x24 (int8)
    elems = 1*32*32*24
    elt = ElementwiseSpec(elems=elems, op_name="RELU")
    d = compute_counts("elementwise", act_bits=8, w_bits=8, elementwise=elt)
    _fmt_counts("Eltwise ReLU int8 (1x32x32x24)", d)

    # 10) Elementwise SOFTMAX on (N=1, Co=10) with one bounds reduction
    elt_sm = ElementwiseSpec(elems=10, op_name="SOFTMAX", out_shape=(1,1,1,10))
    d = compute_counts("elementwise", act_bits=8, w_bits=8, elementwise=elt_sm)
    _fmt_counts("Eltwise Softmax int8 (1x1x1x10) + bounds reduce", d)

    # 11) Elemmentwise MUL on (N=1, H=  250, W=24) with 8-bit inputs and 8-bit outputs
    elt_mul = ElementwiseSpec(elems=1*250*24, op_name="MUL", out_shape=(1,250,1,24))
    d = compute_counts("elementwise", act_bits=8, w_bits=8, elementwise=elt_mul)
    _fmt_counts("Eltwise Mul int8 (250x24) + bounds reduce", d)

if __name__ == "__main__":
    main()
