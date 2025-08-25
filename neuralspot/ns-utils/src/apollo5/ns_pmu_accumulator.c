/**
 * @file    ns_pmu_acc_matrix.c
 * @author  NeuralSPOT
 * @brief   Implementation of 2‑D PMU accumulator.
 */


 #include "ns_pmu_utils.h"          /* ns_pmu_event_create, _init, … */
 #include "ns_pmu_map.h"
 #include <string.h>
 #include "ns_pmu_accumulator.h"
 
 #ifndef EVENTS_PER_RUN
 #define EVENTS_PER_RUN 4           /* Apollo5B can sample four 32‑bit counters */
 #endif
 
 typedef struct {
     const char *tag;
     uint16_t    row;
 } tag_map_t;
 
 /* One control block per active matrix ----------------------------------- */
 typedef struct {
     bool            in_use;
     bool            complete;
     uint16_t        ops, events;
     uint16_t        next_event;            /* next PMU event index to sample */
     uint16_t        next_row;              /* next unused row id             */
 
     uint32_t       *sum;                   /* caller‑supplied [ops*events]   */
     tag_map_t       tag_map[NS_PMU_MAX_OPS];
 
     ns_pmu_config_t cfg;                   /* scratch per‑run PMU setup      */
 } acc_t;
 
 static acc_t g_acc[NS_PMU_MAX_ACTIVE_MATRICES] = {0};
 
 /* ---------- internal helpers ------------------------------------------- */
 
 static acc_t *handle_to_acc(ns_pmu_accm_t h)
 {
     return (h.id < NS_PMU_MAX_ACTIVE_MATRICES && g_acc[h.id].in_use)
            ? &g_acc[h.id] : NULL;
 }
 
 static void cfg_select_events(acc_t *a)
 {
     a->cfg.api = &ns_pmu_V1_0_0;
     ns_pmu_reset_config(&a->cfg);
 
     for (uint8_t i = 0; i < EVENTS_PER_RUN; ++i) {
         uint16_t ev = a->next_event + i;
         if (ev >= a->events) break;
         ns_pmu_event_create(&a->cfg.events[i],
                             ns_pmu_map[ev].eventId,
                             NS_PMU_EVENT_COUNTER_SIZE_32);
     }
     ns_pmu_init(&a->cfg);
 }
 
 /* ---------- API implementation ----------------------------------------- */
 
 ns_pmu_accm_t ns_pmu_accm_create(uint16_t ops,
                                  uint16_t events,
                                  void    *backing_buf)
 {
     ns_pmu_accm_t h = { .id = 0xFF };
 
     if (!backing_buf || ops == 0 || ops > NS_PMU_MAX_OPS ||
         events == 0 || events > NS_PMU_MAP_SIZE)
         return h;
 
     /* Find free slot */
     uint8_t idx;
     for (idx = 0; idx < NS_PMU_MAX_ACTIVE_MATRICES; ++idx)
         if (!g_acc[idx].in_use) break;
     if (idx == NS_PMU_MAX_ACTIVE_MATRICES) return h;
 
     acc_t *a = &g_acc[idx];
     memset(a, 0, sizeof(*a));
 
     a->in_use    = true;
     a->ops       = ops;
     a->events    = events;
     a->sum       = (uint32_t *)backing_buf;
     a->next_event= 0;
 
     memset(a->sum, 0, NS_PMU_MATRIX_BYTES(ops,events));
     for (uint16_t i = 0; i < NS_PMU_MAX_OPS; ++i) a->tag_map[i].tag = NULL;
 
     cfg_select_events(a);                /* arm PMU for first run */
 
     h.id = idx;
     return h;
 }
 
 void ns_pmu_accm_destroy(ns_pmu_accm_t h)
 {
     acc_t *a = handle_to_acc(h);
     if (a) memset(a, 0, sizeof(*a));
 }
 
 void ns_pmu_accm_inference_begin(ns_pmu_accm_t h)
 {
     /* Nothing to do – cfg is already set by create() or previous end()      */
     (void)h;
 }
 
 void ns_pmu_accm_inference_end(ns_pmu_accm_t h)
 {
     acc_t *a = handle_to_acc(h);
     if (!a || a->complete) return;
 
     a->next_event += EVENTS_PER_RUN;
     if (a->next_event >= a->events) {
         a->complete = true;
         return;
     }
     cfg_select_events(a);                /* prepare for next run             */
 }
 
 void ns_pmu_accm_op_begin(ns_pmu_accm_t h, uint16_t op_idx)
 {
     acc_t *a = handle_to_acc(h);
     if (!a || op_idx >= a->ops) return;
 
     ns_pmu_reset_counters();
     (void)a;
 }
 
 void ns_pmu_accm_op_end(ns_pmu_accm_t h, uint16_t op_idx)
 {
     acc_t *a = handle_to_acc(h);
     if (!a || op_idx >= a->ops) return;
 
     ns_pmu_get_counters(&a->cfg);
 
     for (uint8_t i = 0; i < EVENTS_PER_RUN; ++i) {
         uint16_t ev = a->next_event + i;
         if (ev >= a->events) break;
         uint32_t *cell = &a->sum[op_idx * a->events + ev];
         *cell += a->cfg.counter[i].counterValue;
     }
 }
 
 /* ------ Tag resolution -------------------------------------------------- */
 static uint16_t tag_lookup(acc_t *a, const char *tag, bool create_if_missing)
 {
     /* pointer equality first (fast path) */
     for (uint16_t i = 0; i < a->next_row; ++i)
         if (a->tag_map[i].tag == tag) return a->tag_map[i].row;
 
     /* fall back to strcmp in case compiler duplicated string literals */
     for (uint16_t i = 0; i < a->next_row; ++i)
         if (strcmp(a->tag_map[i].tag, tag) == 0) return a->tag_map[i].row;
 
     if (!create_if_missing || a->next_row >= a->ops) return UINT16_MAX;
 
     /* new tag */
     uint16_t row = a->next_row++;
     a->tag_map[row].tag = tag;
     a->tag_map[row].row = row;
     return row;
 }
 
 uint16_t ns_pmu_accm_resolve_tag(ns_pmu_accm_t h, const char *tag)
 {
     acc_t *a = handle_to_acc(h);
     if (!a || !tag) return UINT16_MAX;
     return tag_lookup(a, tag, true);
 }
 
 uint16_t ns_pmu_accm_find_tag(ns_pmu_accm_t h, const char *tag)
 {
     acc_t *a = handle_to_acc(h);
     if (!a || !tag) return UINT16_MAX;
     return tag_lookup(a, tag, false);
 }
 
 /* ------ Query ----------------------------------------------------------- */
 bool ns_pmu_accm_complete(ns_pmu_accm_t h)
 {
     acc_t *a = handle_to_acc(h);
     return a ? a->complete : false;
 }
 
 void ns_pmu_accm_get(ns_pmu_accm_t h, uint32_t **matrix)
 {
     acc_t *a = handle_to_acc(h);
     if (a && matrix) *matrix = a->sum;
 }

 void ns_pmu_accmprint_matrix(ns_pmu_accm_t h, uint32_t *matrix, uint16_t ops, uint16_t events)
 {
     acc_t *a = handle_to_acc(h);

     // Print the header with the event names
     ns_lp_printf("Op: ");
     for (uint16_t j = 0; j < events; ++j) {
         ns_lp_printf("%s ", ns_pmu_map[j].regname);
     }
     ns_lp_printf("\n");

     if (a && matrix) {
         for (uint16_t i = 0; i < ops; ++i) {
            ns_lp_printf("%d, ", i);
             for (uint16_t j = 0; j < events; ++j) {
                 ns_lp_printf("%d, ", matrix[i * events + j]);
             }
             ns_lp_printf("\n");
         }
     }
 }