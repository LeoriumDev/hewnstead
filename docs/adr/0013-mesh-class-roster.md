# ADR-0013: Complete mesh class roster

## Status

**Accepted** — 2026-05-17

## Context

ADR-0012 established that the engine uses concrete mesh classes rather than a generic `Mesh<Vertex>` template. That ADR named five mesh kinds (chunk, skeletal, billboard, line, UI) but didn't commit to specifics: what each class is named, what API each presents, when each is introduced.

A naming and integration roster is needed for two reasons:

1. **Cross-system references.** Other ADRs and design discussions need to reference the mesh classes by stable names. Without a roster, "the skeletal one" and "the line one" stay informal, and naming drifts over time.

2. **Premature class introduction is a real risk.** Without a roster, the temptation is to introduce all five class scaffolds together as part of an "engine foundation" pass, leaving four of them empty for months. This contradicts the "specific to current need" reasoning of ADR-0012 — empty scaffold classes are genericity by another name (deferred to runtime instead of templates).

The roster commits the engine to introducing each class only when its specific need arrives.

## Decision

**The engine's complete mesh class roster, ordered by integration trigger:**

| Class | Purpose | Integrated when |
|---|---|---|
| `ChunkMesh` | Voxel chunk geometry. Triangles, large vertex counts, persistent VAO+VBO with full reuploads. | First chunk-rendering integration (currently the only mesh class implemented). |
| `LineMesh` | Debug lines, block selection outlines, gizmos. `GL_LINES` primitive, tiny vertex counts, frequent rebuild. | First interactive editing (block selection highlight). |
| `UIMesh` | 2D HUD, menus, inventory grids. Orthographic projection, alpha-blended, screen-space coordinates. | First player UI surface (main menu). |
| `BillboardMesh` | Camera-facing quads for foliage, particles, world-space labels. Oriented per-camera at upload time. | First foliage or particle effect. |
| `SkeletalMesh` | Animated characters and creatures. Per-vertex bone weights, uniform buffer of bone transforms. | First animated character. |

Each class:

- Lives in `include/hewnstead/<class_name>.hpp` and `src/<class_name>.cpp`.
- Manages its own GPU resources (VAO, VBO, plus any class-specific resources like the bone uniform buffer for `SkeletalMesh`).
- Follows Rule of Five: copy = delete, move = noexcept-enabled. Same pattern as `ChunkMesh`.
- Exposes whatever methods make sense for its purpose. No forced uniform API across classes.

The roster is **closed** — these five cover every mesh need anticipated in the engine's design. A sixth class is added only if a genuinely new mesh kind appears that none of the five can handle naturally; in that case, this ADR is updated.

## Alternatives considered

- **Open-ended roster: add mesh classes as needed without committing to a list.** Considered but not chosen — the value of the roster is forcing the question "do we really need a new class for this, or does an existing one fit?" upfront. Without it, mesh class proliferation is easy.
- **Combine `LineMesh` and `BillboardMesh` into a generic "DynamicMesh".** Both have small vertex counts and frequent rebuilds. Rejected: line meshes use `GL_LINES` primitive with no triangle culling; billboards use `GL_TRIANGLES` with camera-orientation logic. The shared properties (small, dynamic) are coincidental, not structural.
- **Combine `ChunkMesh` and `SkeletalMesh` under a "WorldMesh" umbrella.** Both are triangle meshes drawn in the main 3D pass. Rejected: chunk meshes are static with millions of vertices; skeletal meshes are dynamically posed with thousands of vertices and bone-transform uniforms. The differences dominate the similarities.
- **Defer `BillboardMesh` and `SkeletalMesh` permanently until the engine has a real need.** Considered but not chosen — including them in the roster commits the engine to the design space they cover (foliage, characters) being supported in principle. Removing them from the roster would commit to a different scope (no characters, no foliage), which is incompatible with project goals.

## Consequences

**Positive:**

- Mesh class names are stable from day one of the roster. ADRs and design discussions can reference `BillboardMesh` and `SkeletalMesh` before they exist as code, with no naming drift later.
- Integration order is explicit; each class arrives when its purpose arrives, not before.
- The five classes' scopes are well-defined and mutually exclusive — disputes about "where does this geometry go" are resolvable by checking the roster.
- Engine design discussions can plan for future classes (e.g., "the lighting system will eventually need to consider `SkeletalMesh` shadows") without those classes existing.

**Negative:**

- Five classes is more documentation surface than a generic alternative would be. Mitigated by classes being small (~80-150 lines each).
- Adding a sixth class requires updating this ADR, which adds friction. By design — the friction is the feature.

**Future implications:**

- Each class's internal evolution is governed by its own ADR or by the meta-policy of ADR-0011 (rewrite per milestone). `ChunkMesh` is governed by ADR-0021.
- If skeletal-mesh design reveals subtleties not anticipated here (bone weight precision, multi-rig support, vertex morphing), those decisions belong in their own ADR, not as edits to this roster.

## Revisit

Stable but not foundational. Revisit if:

- A sixth distinct mesh kind appears that genuinely doesn't fit any of the five. Likely candidates: GPU-driven mesh (compute-shader generated), tessellated mesh (hardware tessellation), point cloud. None anticipated for the engine's current scope.
- Two of the five turn out to share more than expected at integration time — for example, `BillboardMesh` and `LineMesh` might naturally collapse if their GPU resource management ends up identical in practice. Address with empirical evidence, not speculation.

## References

- ADR-0012 (Mesh class specificity over genericity) — the meta-decision this ADR makes concrete.
- ADR-0021 (`ChunkMesh` evolution policy) — governs how `ChunkMesh`'s internals evolve without affecting the roster.
