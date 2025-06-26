#!/usr/bin/env python
import sys
import numpy as np
import tensorflow as tf
import math

def generate_c_array(name, array, dtype_str):
    """
    Generate a C declaration for a given numpy array.
    """
    flat = array.flatten()
    c_str = f"const {dtype_str} {name}[] = {{\n  "
    for i, val in enumerate(flat):
        if dtype_str == "int8_t":
            c_str += f"{int(val)}, "
        elif dtype_str == "int32_t":
            c_str += f"{int(val)}, "
        elif dtype_str == "float":
            c_str += f"{float(val):.8f}f, "
        else:
            c_str += f"{val}, "
        if (i+1) % 16 == 0:
            c_str += "\n  "
    c_str += "\n};\n\n"
    return c_str

def quantize_multiplier(real_multiplier):
    """
    Given a real multiplier, compute a quantized multiplier and shift.
    This mimics TFLite’s QuantizeMultiplierSmallerThanOneExp.
    """
    if real_multiplier == 0:
        return 0, 0
    significand, exponent = math.frexp(real_multiplier)
    quantized_multiplier = int(round(significand * (1 << 31)))
    if quantized_multiplier == (1 << 31):
        quantized_multiplier //= 2
        exponent += 1
    shift = -exponent
    return quantized_multiplier, shift

def quantize_list(scales):
    """
    Given a list (or array) of scales, compute arrays of int32 multipliers and shifts.
    """
    multipliers = []
    shifts = []
    for scale in scales:
        m, s = quantize_multiplier(scale)
        multipliers.append(m)
        shifts.append(s)
    return np.array(multipliers, dtype=np.int32), np.array(shifts, dtype=np.int32)

def find_tensor_by_pattern(tensor_details, pattern):
    """
    Search through tensor_details for a tensor whose 'name' contains the given pattern.
    Returns a tuple (name, tensor, detail) if found; otherwise, None.
    """
    for detail in tensor_details:
        if pattern in detail['name']:
            try:
                tensor = interpreter.get_tensor(detail['index'])
                if tensor is not None:
                    return detail['name'], tensor, detail
            except Exception as e:
                print(f"Skipping tensor {detail['name']} due to error: {e}")
    return None

def main():
    if len(sys.argv) < 2:
        print("Usage: generate_ds_cnn_data.py <tflite_model_path>")
        sys.exit(1)
    tflite_path = sys.argv[1]

    # Load and allocate the TFLite model.
    global interpreter
    interpreter = tf.lite.Interpreter(model_path=tflite_path)
    interpreter.allocate_tensors()

    tensor_details = interpreter.get_tensor_details()
    print("Found tensors:")
    for detail in tensor_details:
        print("  " + detail['name'])

    # Mapping for weights and biases.
    mapping = {
        "conv1_weights": "functional_1/conv2d/Conv2D",  # conv1 layer weights.
        "conv1_bias":    "functional_1/conv2d/BiasAdd/ReadVariableOp",  # conv1 bias.
        "block1_dw_weights": "functional_1/depthwise_conv2d/depthwise",
        "block1_dw_bias":    "functional_1/depthwise_conv2d/BiasAdd/ReadVariableOp",
        "block1_pw_weights": "functional_1/conv2d_1/Conv2D",
        "block1_pw_bias":    "functional_1/conv2d_1/BiasAdd",
        "block2_dw_weights": "functional_1/depthwise_conv2d_1/depthwise",
        "block2_dw_bias":    "functional_1/depthwise_conv2d_1/BiasAdd/ReadVariableOp",
        "block2_pw_weights": "functional_1/conv2d_2/Conv2D",
        "block2_pw_bias":    "functional_1/conv2d_2/BiasAdd",
        "block3_dw_weights": "functional_1/depthwise_conv2d_2/depthwise",
        "block3_dw_bias":    "functional_1/depthwise_conv2d_2/BiasAdd/ReadVariableOp",
        "block3_pw_weights": "functional_1/conv2d_3/Conv2D",
        "block3_pw_bias":    "functional_1/conv2d_3/BiasAdd",
        "block4_dw_weights": "functional_1/depthwise_conv2d_3/depthwise",
        "block4_dw_bias":    "functional_1/depthwise_conv2d_3/BiasAdd/ReadVariableOp",
        "block4_pw_weights": "functional_1/conv2d_4/Conv2D",
        "block4_pw_bias":    "functional_1/conv2d_4/BiasAdd",
        # Updated fully connected mappings: assuming final dense layer uses "dense" prefix.
        "fc_weights": "dense/MatMul",
        "fc_bias":    "dense/BiasAdd",
    }

    # Mapping for batch normalization parameters.
    bn_mapping = {
        "conv1_bn_multiplier": "functional_1/batch_normalization/gamma",
        "conv1_bn_shift":      "functional_1/batch_normalization/beta",
        "block1_dw_bn_multiplier": "functional_1/batch_normalization_1/gamma",
        "block1_dw_bn_shift":      "functional_1/batch_normalization_1/beta",
        "block1_pw_bn_multiplier": "functional_1/batch_normalization_1/gamma",
        "block1_pw_bn_shift":      "functional_1/batch_normalization_1/beta",
        "block2_dw_bn_multiplier": "functional_1/batch_normalization_2/gamma",
        "block2_dw_bn_shift":      "functional_1/batch_normalization_2/beta",
        "block2_pw_bn_multiplier": "functional_1/batch_normalization_2/gamma",
        "block2_pw_bn_shift":      "functional_1/batch_normalization_2/beta",
        "block3_dw_bn_multiplier": "functional_1/batch_normalization_3/gamma",
        "block3_dw_bn_shift":      "functional_1/batch_normalization_3/beta",
        "block3_pw_bn_multiplier": "functional_1/batch_normalization_3/gamma",
        "block3_pw_bn_shift":      "functional_1/batch_normalization_3/beta",
        "block4_dw_bn_multiplier": "functional_1/batch_normalization_4/gamma",
        "block4_dw_bn_shift":      "functional_1/batch_normalization_4/beta",
        "block4_pw_bn_multiplier": "functional_1/batch_normalization_4/gamma",
        "block4_pw_bn_shift":      "functional_1/batch_normalization_4/beta",
    }

    # Updated mapping for quantization parameters.
    quant_mapping = {
        "conv1_multiplier": "functional_1/conv2d/Conv2D",
        "conv1_shift":      "functional_1/conv2d/Conv2D",
        "dw_conv_multiplier": "depthwise_conv2d",  # matches any tensor with "depthwise_conv2d"
        "dw_conv_shift":      "depthwise_conv2d",
        "pw_conv_multiplier": "conv2d_1/Conv2D",    # use block1's pointwise conv tensor.
        "pw_conv_shift":      "conv2d_1/Conv2D",
        "fc_multiplier": "dense/MatMul",
        "fc_shift":      "dense/MatMul",
    }

    c_arrays = ""

    # Process weights and biases.
    for cname, pattern in mapping.items():
        result = find_tensor_by_pattern(tensor_details, pattern)
        if result is not None:
            tname, tensor, detail = result
            print(f"Found tensor for {cname}: {tname}, shape {tensor.shape}, dtype {tensor.dtype}")
            # For weight arrays, force conversion to int8.
            if "weights" in cname:
                arr = np.array(tensor)
                if arr.dtype != np.int8:
                    # Clip values to int8 range and cast.
                    arr = np.clip(arr, -128, 127).astype(np.int8)
                dtype_str = "int8_t"
            else:
                # For biases assume int32 is desired.
                dtype_str = "int32_t" if tensor.dtype == np.int32 else "int8_t"
                arr = tensor
            c_arrays += generate_c_array(cname, arr, dtype_str)
        else:
            print(f"Warning: Could not find tensor for {cname} with pattern '{pattern}'")

    # Process batch normalization parameters.
    for cname, pattern in bn_mapping.items():
        result = find_tensor_by_pattern(tensor_details, pattern)
        if result is not None:
            tname, tensor, detail = result
            print(f"Found BN tensor for {cname}: {tname}, shape {tensor.shape}, dtype {tensor.dtype}")
            dtype_str = "int32_t"  # assuming BN parameters are quantized to int32_t
            c_arrays += generate_c_array(cname, tensor, dtype_str)
        else:
            print(f"Warning: Could not find BN tensor for {cname} with pattern '{pattern}'")

    # Process quantization parameters.
    for cname, pattern in quant_mapping.items():
        result = find_tensor_by_pattern(tensor_details, pattern)
        if result is not None:
            tname, tensor, detail = result
            quant = detail.get("quantization")
            if quant is not None:
                if isinstance(quant[0], (list, tuple, np.ndarray)) and len(quant[0]) > 1:
                    scales = quant[0]
                else:
                    scales = [quant[0]]
                multipliers, shifts = quantize_list(scales)
                if "multiplier" in cname:
                    c_arrays += generate_c_array(cname, multipliers, "int32_t")
                elif "shift" in cname:
                    c_arrays += generate_c_array(cname, shifts, "int32_t")
            else:
                print(f"Warning: No quantization info for tensor {tname} for {cname}")
        else:
            print(f"Warning: Could not find tensor for quantization parameter {cname} with pattern '{pattern}'")

    # Process unit test arrays.
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    input_shape = input_details[0]['shape']
    output_shape = output_details[0]['shape']
    test_input = np.zeros(tuple(input_shape[1:]), dtype=np.int8)
    expected_output = np.zeros(tuple(output_shape[1:]), dtype=np.int8)
    c_arrays += generate_c_array("test_input", test_input, "int8_t")
    c_arrays += generate_c_array("expected_output", expected_output, "int8_t")

    # For the fully connected layer quantization, output the structure.
    fc_result = find_tensor_by_pattern(tensor_details, quant_mapping["fc_multiplier"])
    if fc_result is not None:
        tname, tensor, detail = fc_result
        quant = detail.get("quantization")
        if quant is not None:
            if isinstance(quant[0], (list, tuple, np.ndarray)) and len(quant[0]) > 1:
                scales = quant[0]
            else:
                scales = [quant[0]]
            fc_mult, fc_sh = quantize_list(scales)
            fc_n = len(fc_mult)
            fc_quant_struct = (
                f"const cmsis_nn_per_channel_quant_params fc_quant_params = {{\n"
                f"  .multiplier = fc_multiplier,\n"
                f"  .shift = fc_shift,\n"
                f"  .n = {fc_n}\n"
                f"}};\n\n"
            )
            c_arrays += fc_quant_struct
        else:
            print("Warning: No quantization info for fully connected layer.")
    else:
        print("Warning: Could not find fc layer quantization info for fc_quant_params.")

    # Write the generated arrays and structures to ds_cnn_data.c.
    with open("ds_cnn_data.c", "w") as f:
        f.write("// Auto-generated C data for DS-CNN model\n\n")
        f.write('#include "ds_cnn_data.h"\n\n')
        f.write(c_arrays)

    print("Generated ds_cnn_data.c. Please ensure that ds_cnn_data.h matches the declarations.")

if __name__ == "__main__":
    main()

