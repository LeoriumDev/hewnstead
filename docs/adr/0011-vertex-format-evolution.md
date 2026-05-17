# ADR-0011: Vertex format evolves with engine milestones, not designed forward-compatible

## Status

**Accepted** — 2026-05-17

## Context

The `ChunkVertex` struct is the per-vertex data layout the mesher emits and the GPU consumes. It defines:

- What attributes each vertex carries (position, color, UV, layer, lighting, AO, normal, ...).
- The byte offsets the OpenGL attribute pointers reference.
- The `in` declarations in the vertex shader.

The mesher emits ~36,864 vertices for a fully-dirt 32³ chunk with face culling. Larger scenes will emit millions across all loaded chunks. Vertex size therefore directly affects:

- **GPU memory** for static chunk meshes.
- **CPU → GPU upload bandwidth** on every chunk re-mesh.
- **Cache line utilization** during vertex shading.

Across the engine's development arc, `ChunkVertex` needs to carry different things at different points in the project's maturity:

- Initial mesh integration: position + per-face debug color (six colors for the six face directions, no textures yet).
- Texture array integration: position + UV + texture array layer (debug color replaced by sampled texture).
- Lighting + ambient occlusion integration: position + UV + layer + baked light + ambient occlusion + face normal hint.

A naive encoding of the lighting-stage vertex with seven attributes is ~32 bytes. With bit-packing into a denser representation (position into 16-bit fixed-point, layer + face direction into a single byte, light + AO into another byte), it fits in 8 bytes.

A common temptation is to design `ChunkVertex` once with all future attributes already present, leaving unused fields blank until the milestone that needs them. This was rejected.

## Decision

**`ChunkVertex` is rewritten at each milestone that requires new vertex attributes. The struct, the mesher's emit code, the `ChunkMesh` attribute pointers, and the vertex/fragment shaders are all updated together as one coherent change.** No effort is made to design earlier formats to be forward-compatible with later needs.

Confirmed format evolution path:

- **Initial mesh / culling milestones**: `{ vec3 position, vec3 color }` — 24 bytes.
- **Texture array integration**: `{ vec3 position, vec2 uv, float layer }` — 24 bytes.
- **Lighting + AO integration**: bit-packed 8 bytes (exact layout to be decided when that milestone is implemented).

The `ChunkMesh` class API (`upload()`, `draw()`) stays stable across these rewrites. The format change is contained within `ChunkMesh`'s internals, the mesher, and the shaders. Application code does not see `ChunkVertex` directly.

## Alternatives considered

- **Forward-compatible format from the first mesh.** Design `ChunkVertex` with all eventual fields already present; ignore unused fields in earlier milestones. Rejected: requires predicting the complete attribute set before any milestone has shipped, which guarantees getting it wrong. Also wastes GPU memory and upload bandwidth for several months until lighting fields are actually used.
- **Variadic / template `ChunkVertex`.** `template <typename Attribs> struct Vertex<Attribs>...` parameterized over which attributes are present. Rejected: forces every consumer (`ChunkMesh`, mesher, shader) to be generic, which is significant complexity for one client of the type. Same reasoning as ADR-0012 (mesh class specificity).
- **Reserved padding bytes for future expansion.** Use a 32-byte struct with 8 bytes of `uint8_t reserved[8]` ready for later use. Rejected: pays the cost (33% larger vertex, 33% more upload bandwidth) for months without benefit, and the lighting milestone still wants bit-packed 8 bytes — the reserved field's location wouldn't match the eventual bit-packed layout. Worst of both worlds.
- **Two parallel vertex streams.** Static stream (position, never changes) + dynamic stream (everything else). Rejected: doubles draw call complexity, doubles VAO binding work, and no profile data has justified it. Revisit if bandwidth becomes a measured bottleneck under streaming workloads.

## Consequences

**Positive:**

- Every vertex format is exactly as small as it needs to be for the current capabilities.
- Each milestone's vertex layout is self-documenting; reading the struct tells you what attributes that capability tier uses.
- The bit-packing rewrite at the lighting milestone is not constrained by a months-old decision made before bit-packing requirements were known.
- Format rewrites are mechanical: struct, attribute pointers, shader inputs. All three must change together; no place to forget.

**Negative:**

- Each milestone that introduces new attributes pays a small refactor cost (~30 minutes for format swap + verification). Multiple rewrites add up to a few hours of churn over the engine's development.
- Anyone reading old git history sees `ChunkVertex` change shape; not a problem for solo development but worth noting if collaborators ever join.

**Future implications:**

- The pattern of "rewrite per milestone, don't predict" carries over to other internal data structures (mesher tables, save format versioning, network message layouts). Premature flexibility is a recurring trap; this ADR sets the policy for resisting it.

## Revisit

Permanent — foundational policy. Specific format choices at each milestone are decided at that milestone's design discussion; this ADR is the meta-policy that they should be allowed to differ.

## References

- ADR-0012 (Mesh class specificity over genericity) — sibling decision applying the same "concrete over generic" reasoning.
- ADR-0021 (`ChunkMesh` evolution policy) — describes how `ChunkMesh`'s API stays stable across these format swaps.
