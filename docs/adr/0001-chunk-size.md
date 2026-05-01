# ADR-0001: Chunk size = 32³

## Status

**Accepted** — 2026-05-01

## Context

Hewnstead must divide its world into chunks for memory bounds, streaming, and per-region update granularity. The choice of chunk size affects:

- **Cache fit**: smaller chunks fit in L1; larger chunks spill to L2/L3.
- **Bit math**: power-of-2 sizes allow shift/mask instead of div/mod.
- **Negative coordinates**: integer div/mod misbehave for negatives in C++; bitwise ops on power-of-2 sizes are correct uniformly.
- **Mesher work per chunk**: larger chunks take longer to re-mesh on edits.
- **Chunk count overhead**: more chunks means more `unordered_map` entries, more draw calls, more bookkeeping per chunk.
- **Chunk-boundary face culling**: more chunks means more boundary faces needing neighbor-chunk lookups during meshing.

The primary development hardware is Apple Silicon M3 Max. P-core L1 D-cache is 128 KB per core. Cross-platform targets (Linux/Windows on x86 Zen 4, Intel) typically have 32-48 KB L1 D-cache.

## Decision

**Chunks are 32 × 32 × 32 blocks. Block IDs are `uint16`. One chunk's block array is therefore 64 KB.**

This stores in `std::array<uint16_t, 32*32*32>` (or equivalent flat allocation), with index computed as `(y << 10) | (z << 5) | x`.

Y-major layout chosen because:

1. Mesher inner loop is over x (`for x = 0 to 31`); x must be the fastest-varying dimension for cache locality. `(y << 10) | (z << 5) | x` achieves this.
2. Vertical column iteration (flood-fill lighting) accesses a chunk's blocks across full y-range; chunk-internal cache pattern is acceptable (1024-stride within a 64 KB chunk still fits L1).
3. Same-y "floor" of blocks is contiguous in memory, useful for operations like rendering a single layer or applying y-bounded edits.

```text
Index 0       (x=0,  y=0,  z=0)   ← chunk start
Index 31      (x=31, y=0,  z=0)
Index 32      (x=0,  y=0,  z=1)   ← z+1 jumps 32
...
Index 1023    (x=31, y=0,  z=31)
Index 1024    (x=0,  y=1,  z=0)   ← y+1 jumps 1024
...
Index 32767   (x=31, y=31, z=31)  ← chunk end (64 KB total)
```

## Alternatives considered

- **16³ chunks (8 KB each).** Fits in L1 trivially on every platform.
  Rejected: chunk count grows by 8× for the same world volume, making hash-map lookups, per-chunk bookkeeping, and draw call counts dominate the savings. Streaming and worldgen overhead per chunk also grows.
- **64³ chunks (512 KB each).** Halves chunk count compared to 32³.
  Rejected: 512 KB does not fit in P-core L1 (128 KB) and starts to pressure L2 on x86. Also, re-meshing one 64³ chunk on a single block edit takes ~8× longer than 32³, which becomes user-visible lag.
- **32 × 32 × 256 (Minecraft Java vertical column).** Rejected: vertical column model assumes a fixed vertical range. Hewnstead wants Layer 4+ to support arbitrary-height building (towers, skybridges) without special-case "extra column" handling. Cubic chunks are more uniform.
- **Non-power-of-2 (e.g., 30³ or 50³).** Rejected: forces div/mod for every world→chunk coordinate transformation, wasting a few cycles on every block lookup. Bit-shift correctness on negative coordinates is also lost.

## Consequences

**Positive:**

- 64 KB per chunk fits entirely in M3 Max P-core L1, making meshing and iteration cache-friendly.
- Power-of-2 size: world→chunk coordinate is `>> 5`, local-in-chunk is `& 31`, both correct for negative coordinates.
- Vintage Story precedent (also 32³) gives community knowledge to draw on.
- Re-mesh time per chunk on a single edit is 1-3 ms on M3 Max — invisible to the player, comfortably below 16 ms frame budget.

**Negative:**

- On x86 platforms with smaller L1 (32-48 KB), one chunk does not fully fit in L1; meshing depends on L2 hits. Still fast (10-15 cycles per miss) but not optimal. Profile and revisit if x86 perf becomes a problem.
- Chunk count for a 2 km × 256 m × 2 km world is ~256,000 chunks — large but tractable.
- Boundary face culling requires neighbor-chunk lookups during meshing. Acceptable; standard pattern.

## Revisit

Re-examine if any of the following occurs:

- Profiler (Tracy) shows mesh worker threads dominated by L1/L2 cache misses on x86 platforms.
- World volume targets grow past current expectations and chunk-count overhead becomes a bottleneck.
- A future GPU-driven meshing path (compute shader) would benefit from a different chunk granularity.
