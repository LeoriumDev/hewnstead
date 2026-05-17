# ADR-0012: Mesh class specificity over genericity

## Status

**Accepted** — 2026-05-17

## Context

The renderer needs to draw multiple distinct kinds of geometry, each with substantially different requirements:

- **Voxel chunk meshes**: huge vertex counts (~37,000 to ~1,200,000 vertices per chunk depending on culling strategy), per-face attributes (UV, layer, lighting, AO), `GL_STATIC_DRAW` upload pattern, persistent VAO+VBO with full-buffer reuploads on rebuild.
- **Skeletal meshes** (characters, animals, NPCs): tens of thousands of vertices, per-vertex bone weights (4 bone indices + 4 weights), uniform buffer of bone transformation matrices, animation update per frame.
- **Billboard meshes** (foliage, particles, UI text in world space): tiny vertex counts, oriented per-camera, frequently rebuilt as the player rotates, possibly `GL_DYNAMIC_DRAW`.
- **Line meshes** (debug overlays, block selection outlines, gizmos): trivial vertex counts (4-12 vertices), `GL_LINES` primitive instead of `GL_TRIANGLES`, no culling, often immediate-mode rebuild.
- **UI meshes** (HUD, menus, inventory): 2D in screen-space, orthographic projection, alpha-blended, rebuilt per UI state change.

Looking at these five categories side by side, the temptation is to design a single generic template:

```cpp
template <typename Vertex>
class Mesh {
    void upload(std::span<const Vertex> verts);
    void draw() const;
    // ...
};
```

with usage like `Mesh<ChunkVertex> chunkMesh; Mesh<SkeletalVertex> playerMesh;`. This was rejected.

## Decision

**Each distinct mesh kind is its own concrete class with its own name and its own API surface.** No generic `Mesh<Vertex>` template, no inheritance hierarchy with a base `IMesh` interface.

Confirmed class roster:

- `ChunkMesh` — voxel chunk geometry (the only mesh class currently implemented).
- `SkeletalMesh` — animated characters (introduced when animation work begins).
- `BillboardMesh` — camera-facing quads (introduced when foliage or particles are needed).
- `LineMesh` — debug lines, selection outlines (introduced when interactive editing begins).
- `UIMesh` — 2D HUD/menu geometry (introduced when the player UI library is integrated).

Each class manages its own VAO+VBO (and additional GPU resources as needed — e.g., `SkeletalMesh` will also own a uniform buffer for bone matrices). Each class follows Rule of Five with copy = delete, move = noexcept-enabled. Each class exposes whatever methods make sense for its kind: `ChunkMesh::upload(span<ChunkVertex>)`, `SkeletalMesh::pose(const Skeleton&, float t)`, `LineMesh::addSegment(vec3 a, vec3 b)`, etc.

## Alternatives considered

- **Single generic `template <Vertex> Mesh<V>`.** Rejected on three grounds. (1) The five mesh kinds have non-trivially different APIs — a `SkeletalMesh` needs `pose()`, a `LineMesh` needs `addSegment()`, a `BillboardMesh` needs `orientToCamera()`. Forcing all of them through a uniform `upload(span)` API hides the kind-specific methods or pushes them outside the class. (2) The GL state setup is also non-trivially different (texture binding, depth test on/off, primitive type, blend mode); a generic template would either need to be parameterized on GL state strategy (more template parameters, more complexity) or restrict each instantiation to share state choices it doesn't all want. (3) Generic-class shape suggests interchangeability that the actual kinds don't have — you cannot meaningfully store skeletal meshes and chunk meshes in the same container or pass them to the same renderer pipeline.

- **Inheritance hierarchy with `IMesh` base.** `class IMesh { virtual void draw() = 0; }; class ChunkMesh : public IMesh { ... };`. Rejected: virtual dispatch on `draw()` adds a vtable lookup per call, and future chunk meshes draw potentially hundreds of times per frame. More importantly, the polymorphic-mesh-container assumption is wrong — chunks are drawn in a sorted pass with their texture array bound; skeletal meshes are drawn in a separate pass with their bone uniforms updated; line meshes are drawn last with depth-testing relaxed. They don't share a generic "draw" step; pretending they do via inheritance hides the pipeline structure.

- **Single `Mesh` class with discriminator enum.** `class Mesh { MeshKind kind; ... }; Mesh::draw()` switches on `kind`. Rejected: collapses all five kinds' specific data members into one class, bloating it; switch-on-kind dispatch is just slower-and-more-fragile vtable; adding a kind requires modifying the central class instead of adding a new concrete class.

- **Defer the decision until the second mesh kind exists.** Wait until skeletal meshes are needed, then look at `ChunkMesh` and the new code side by side and decide. Considered but not chosen: the decision is foundational enough that having it made and documented before the second kind arrives avoids hasty design under pressure. Also, the same reasoning applies in advance as it would apply later.

## Consequences

**Positive:**

- Each mesh kind's class is exactly as expressive as it needs to be — no compromises to fit a generic mold.
- Adding a new mesh kind is a self-contained operation: write the class, integrate at its specific call sites. Doesn't ripple through other mesh kinds.
- Code reading is direct: `ChunkMesh::upload` clearly means "upload chunk vertices" without parametric typing.
- Compilation cost is bounded: no template instantiation churn when one mesh kind changes.
- Each class can have its own internal evolution policy (see ADR-0021 for `ChunkMesh`'s) without affecting other classes.

**Negative:**

- Some boilerplate is duplicated across classes: each one redeclares Rule of Five members, each one has its own VAO+VBO management. ~30-50 lines of repetition per class. Acceptable cost.
- Five classes is more code to read than one templated class. Tradeoff against the better fit per class.
- A reader looking for "the mesh class" finds five answers and must learn which is which. Mitigated by clear naming (`ChunkMesh` ↔ voxel chunks, `SkeletalMesh` ↔ characters, etc.).

**Future implications:**

- The "concrete over generic" reasoning generalizes to other system-design points where genericity is tempting. For example, save format serializers: separate `ChunkSerializer`, `EntitySerializer`, `PlayerSerializer` classes rather than a templated `Serializer<T>`. Decisions made consistent with this ADR.
- If a future need genuinely requires polymorphic mesh containers (e.g., a render queue), the answer is composition — a separate `RenderItem` struct with type-erasure or `std::variant`-style dispatch — not retroactive `IMesh` inheritance on the mesh classes themselves.

## Revisit

Permanent — foundational policy. Specific mesh classes' APIs evolve independently; this ADR only commits to the "specific not generic" stance.

## References

- ADR-0011 (Vertex format evolves with engine milestones) — same "concrete over generic" reasoning applied to `ChunkVertex`.
- ADR-0013 (Complete mesh class roster) — enumerates the five mesh classes and their integration order.
- ADR-0021 (`ChunkMesh` evolution policy) — describes how `ChunkMesh`'s API stays stable while its internals (vertex format, GL state, optimization tricks) churn.
