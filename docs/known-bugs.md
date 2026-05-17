## Known bugs

### Stuck Shift after macOS screenshot shortcut

**Symptom**: Pressing Cmd+Shift+5 to take a screenshot, then returning to
Hewnstead, causes the camera to descend continuously (because LEFT_SHIFT is
treated as held). Cursor lock also breaks during the screenshot capture mode.

**Root cause**: macOS screenshot system intercepts keyboard input system-wide.
Modifier RELEASE events (Shift, Cmd) that occur during screenshot capture
mode are swallowed by the OS — GLFW receives neither callback nor polling
notification. Both key callback and `glfwGetKey` polling get stuck reporting
PRESS even after the user physically releases.

This is a known limitation of GLFW's Cocoa backend, not a Hewnstead bug.

**Workaround (dev environment)**:

1. Disable macOS system screenshot shortcuts:
   System Settings → Keyboard → Keyboard Shortcuts → Screenshots → uncheck all.
2. Use a third-party screenshot tool (Shottr, CleanShot X) with a non-Shift
   hotkey.

**Possible code-level fixes (deferred)**:

- macOS native `CGEventSourceKeyState` to query HID-level modifier state
  every frame and reconcile against callback state. Requires `#ifdef __APPLE__`,
  ApplicationServices.framework link.
- Block-level `CGEventTap` to intercept Cmd+Shift+4 before it reaches the
  screenshot system. Requires Accessibility permission, macOS-only.

Both deferred — for now doesn't justify either.

**Filed**: 2026-05-08

### Cross-chunk seam emits duplicate boundary faces

**Symptom**: When Stage 5 places adjacent chunks side by side, the shared
boundary between them will emit two coplanar faces — one from each chunk —
instead of culling both. Back-face culling makes the result visually correct,
but the GPU pays for vertex shading on both faces plus fragment shading on
the visible one (the back-facing one is discarded after rasterization,
wasting fillrate).

Not yet observable: Stage 3C ships with one chunk, so no seams exist. Will
appear the moment Stage 5 spawns a second chunk adjacent to the first.

**Root cause**: Stage 3C mesher treats out-of-bounds neighbor lookups as Air
via `Chunk::getOrAir`. From inside each chunk, the boundary faces appear to
face empty space — so both chunks emit them. The mesher has no awareness of
neighboring chunks; the OOB-as-air policy is the deliberate Stage 3C
boundary contract (PROJECT_INDEX §15, Stage 3C guide §2.4).

This is a known-deferred bug, not an oversight. Single-chunk Stage 3C is
correct as-is; cross-chunk awareness is Stage 5 territory.

**Cost when it manifests**: ~6,144 wasted vertex per chunk-pair sharing a
full 32×32 boundary (one boundary surface × 6 vertex per face × 2 chunks).
For N chunks arranged in a contiguous region, roughly 3N × 6,144 wasted
vertex (each interior chunk has neighbors on all six sides; edge chunks
fewer). Acceptable while chunk count is small; problematic at Stage 5 view
distance.

**Workaround**: None needed for Stage 3C / 3D / 4 (single chunk).

**Fix (Stage 5)**: Cross-chunk mesher queries neighbor chunks for OOB
lookups instead of returning Air. Probable shapes:

- `World::getBlock(worldCoords)` global accessor that resolves chunk +
  in-chunk offset and delegates to the right `Chunk::get`.
- `ChunkNeighbors` struct (6 `Chunk*`, one per face direction) passed to
  `buildMesh` alongside the central chunk. Mesher checks the neighbor pointer
  for boundary lookups, falls back to Air if the neighbor isn't loaded
  (chunk at world edge or not-yet-streamed).

Decision deferred to Stage 5; ADR worth writing once the streaming model
is clearer.

**Filed**: 2026-05-17