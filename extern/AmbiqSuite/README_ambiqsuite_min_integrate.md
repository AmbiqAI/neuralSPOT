
# ambiqsuite_min_integrate.py

This tool helps you rebase your neuralSPOT **minimal AmbiqSuite** onto a **new** AmbiqSuite release.

## What it does

1. (Optionally) copies the **new upstream** tree into `--out`.
2. Recreates your `lib/` layout using libraries from the **new** upstream (falls back to old libs if not found).
3. For each file under **`old-port/src/**`**, finds the **closest match** in the new upstream (and the old upstream if provided) and performs a conservative merge:
   - 3‑way merge when `--old-upstream` is supplied (base=old upstream, local=your src, remote=new upstream).
   - 2‑way fallback (with conflict markers) if no base is available.
4. Ensures **`third_party/cordio/**`** in the output mirrors the new upstream (Cordio sources kept).
5. Deletes `.c`/`.cpp` everywhere in the output **except** under `src/**` and `third_party/cordio/**`.
6. Writes a detailed report to `OUT/_integration_report/`:
   - `decisions.json` + `decisions.md`: per‑file results
   - `diffs/*.diff`: diffs of merged results
   - `src_map/*.json`: which upstream files were considered
   - `libs_mapping.json`: where each `lib/` entry came from

## CLI

```bash
# Basic (no old upstream available)
python ambiqsuite_min_integrate.py   --old-port      /Users/carlosmorales/dev/clean/neuralSPOT/extern/AmbiqSuite/R5.3.0   --new-upstream  /path/to/full/AmbiqSuite/RNEW   --out           /path/to/neuralSPOT/extern/AmbiqSuite/RNEW

# With old upstream (enables true 3-way merges)
python ambiqsuite_min_integrate.py   --old-port      /Users/carlosmorales/dev/clean/neuralSPOT/extern/AmbiqSuite/R5.3.0   --old-upstream  /path/to/full/AmbiqSuite/R5.3.0   --new-upstream  /path/to/full/AmbiqSuite/RNEW   --out           /path/to/neuralSPOT/extern/AmbiqSuite/RNEW
```

### Useful options

- `--hints hints.json`
  Map from `src` file to preferred upstream path (relative to the upstream root). Example:

  ```json
  {
    "usb/dcd_apollo5.c": "src/usb/dcd_apollo5.c",
    "am_hal_uart.c": "mcu/apollo510/hal/am_hal_uart.c"
  }
  ```

- `--keep-list keep.json`
  List of **output‑relative** paths you want to protect from pruning.

- `--copy-upstream all|empty`  
  `all`: copy the entire upstream into `--out` first (default).  
  `empty`: start with an empty `--out` and only write merged files, libs, and cordio.

- `--similarity 0.0–1.0`  
  Minimum similarity for accepting a "closest match" candidate (default 0.55).

## Notes / Caveats

- The 3‑way merge is conservative: when both upstream and local change the **same** region differently,
  conflict markers are inserted for manual review (`<<<<<<< LOCAL` / `=======` / `>>>>>>> REMOTE`).
- The tool indexes upstream by **filename** first. If your `src` file name is unique, matching is fast and reliable.
  If not, it computes content similarity and picks the best candidate.
- Library mapping is by filename; if a library is renamed or moved, provide a hint or replace it manually.
- Everything the tool changes is summarized in the report folder so you can audit quickly.

Good luck, and may your diffs be small! 🧰
