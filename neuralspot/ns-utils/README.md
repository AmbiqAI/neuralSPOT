// Variables
ns_cache_config_t cc;
ns_cache_dump_t start;
ns_cache_dump_t end;

// Init and enable the DMON
cc.enable = true;
ns_cache_profiler_init(&cc);

// Capture the start values
ns_capture_cache_stats(&start);

// Code you're interested in watching
// ...

// Capture the end values
ns_capture_cache_stats(&end);

// Print out the difference between start and end
ns_print_cache_stats_delta(&start, &end);
