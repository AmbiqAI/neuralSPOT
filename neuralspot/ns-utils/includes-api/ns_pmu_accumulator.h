/**
 * @file    ns_pmu_acc_matrix.h
 * @author  NeuralSPOT
 * @brief   Compact 2‑D PMU accumulator: (op × event) → uint32_t
 * @version 1.0.0
 *
 * USAGE (one model, collected across several invocations):
 *
 *     #define OPS      128                         // layers / operators
 *     #define EVENTS   NS_PMU_MAP_SIZE             // 71 on Apollo5B
 *     static uint32_t backing[OPS * EVENTS];
 *
 *     ns_pmu_accm_t h = ns_pmu_accm_create(OPS, EVENTS, backing);
 *
 *     while (!ns_pmu_accm_complete(h)) {
 *         ns_pmu_accm_inference_begin(h);
 *         ...
 *         for (uint16_t op = 0; op < OPS_THIS_RUN; ++op) {
 *             ns_pmu_accm_op_begin(h, op);
 *             run_layer(op);
 *             ns_pmu_accm_op_end  (h, op);
 *         }
 *         ns_pmu_accm_inference_end(h);
 *     }
 *
 *     uint32_t *m; ns_pmu_accm_get(h, &m);     // m[op * EVENTS + event]
 */

 #ifndef NS_PMU_ACC_MATRIX_H
 #define NS_PMU_ACC_MATRIX_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 /* ---------- Build‑time limits (override in project‑level flags) ---------- */
 #ifndef NS_PMU_MAX_ACTIVE_MATRICES
 #define NS_PMU_MAX_ACTIVE_MATRICES 1            /* simultaneous matrices   */
 #endif
 
 #ifndef NS_PMU_MAX_OPS
    // Match TFLM kMaxEvents
    #if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L)
        #define NS_PMU_MAX_OPS 4096 /* rows / operators        */
    #else
        #define NS_PMU_MAX_OPS 2048
    #endif
#endif
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /* ---------- Public types ------------------------------------------------- */
 typedef struct { uint8_t id; } ns_pmu_accm_t;       /* opaque handle        */
 
 /* ---------- Mandatory external symbols ---------------------------------- */
 #ifndef NS_PMU_MAP_SIZE                   /* comes from ns_pmu_map.h        */
 #define NS_PMU_MAP_SIZE 70
 #endif
 
 /* ---------- Helper macro ------------------------------------------------- */
 #define NS_PMU_MATRIX_BYTES(_ops,_evt) ( (_ops) * (_evt) * sizeof(uint32_t) )
 
 /* ---------- Life‑cycle --------------------------------------------------- */
 ns_pmu_accm_t ns_pmu_accm_create(uint16_t ops,
                                  uint16_t events,
                                  void    *backing_buf);
 
 void ns_pmu_accm_destroy(ns_pmu_accm_t h);
 
 /* ---------- Inference‑level framing ------------------------------------- */
 void ns_pmu_accm_inference_begin(ns_pmu_accm_t h);
 void ns_pmu_accm_inference_end  (ns_pmu_accm_t h);
 
 /* ---------- Op / layer framing ------------------------------------------ */
 void ns_pmu_accm_op_begin(ns_pmu_accm_t h, uint16_t op_idx);
 void ns_pmu_accm_op_end  (ns_pmu_accm_t h, uint16_t op_idx);
 
 /* ---------- Tag resolution helper (for TFLM) ---------------------------- */
 uint16_t ns_pmu_accm_resolve_tag(ns_pmu_accm_t h, const char *tag);
 uint16_t ns_pmu_accm_find_tag   (ns_pmu_accm_t h, const char *tag);
 
 /* ---------- Query -------------------------------------------------------- */
 bool     ns_pmu_accm_complete(ns_pmu_accm_t h);
 void     ns_pmu_accm_get     (ns_pmu_accm_t h, uint32_t **matrix);
 void     ns_pmu_accmprint_matrix(ns_pmu_accm_t h, uint32_t *matrix, uint16_t ops, uint16_t events);
 
 /* Return a single layer (row) worth of counters into out[].
  * Copies min(events, caller_capacity) entries starting at the given layer.
  */
 void     ns_pmu_accm_get_layer(ns_pmu_accm_t h,
                                uint16_t     layer,
                                uint32_t*    out,
                                uint16_t
                                     caller_capacity);
 #ifdef __cplusplus
 }
 #endif
 #endif /* NS_PMU_ACC_MATRIX_H */
 