# ADR-0002: Texture array over texture atlas

## Status

**Accepted** — 2026-05-01

## Context

Voxel engines need to render many block types using different textures in a single draw call. Three approaches exist:

1. **Texture atlas** — pack all block textures into one big PNG; UV coordinates point at sub-rectangles. (Classic Minecraft Java < 1.13.)
2. **Individual textures, one per block** — bind each texture per draw call.
3. **Texture 2D array** — pack textures into a single GPU `GL_TEXTURE_2D_ARRAY`; sample with `(uv, layer)`. (OpenGL 3.0+.)

Two known problems with atlas:

- **Mipmap bleeding**: at distant LODs, neighboring atlas tiles bleed color across boundaries. A stone block 100 m away picks up green from the grass tile next to it in the atlas.
- **No texture wrap**: atlas UVs are sub-ranges of `[0,1]`, so `GL_REPEAT` doesn't work correctly within a tile; manual `fract()` simulation needed in shader.

Hewnstead's art direction is Vintage-Story-like smoothed voxel; many distinct block textures will exist (stone variants, wood variants, ore variants), all 32×32 RGBA.

## Decision

**Use `GL_TEXTURE_2D_ARRAY` for all block textures. Each block face's texture is one layer in the array. Vertex format includes a `uint8 layer` attribute. Fragment shader samples with `texture(blockTextures, vec3(uv, layer))`.**

All block textures are stored as separate PNG files on disk (`assets/blocks/stone.png`, `assets/blocks/grass_top.png`, etc.) and uploaded to GPU at startup as a single texture array. A `blocks.json` manifest defines block-ID → face → layer-index mapping.

## Alternatives considered

- **Texture atlas.** Rejected: mipmap bleeding and UV-range confusion are tractable but never go away. Each is a class of bugs the developer will hit repeatedly. Atlas saves a few KB at high cost in correctness.
- **Individual textures, one bind per draw call.** Rejected: with hundreds of distinct block types, draw-call count explodes. Each chunk would need either dozens of binds or a sort-by-texture step.
- **Bindless textures (OpenGL 4.4+).** Rejected: not available in OpenGL 4.1 Core (Apple's ceiling).
- **Sparse texture arrays (OpenGL 4.4+).** Same — not available on Mac.

## Consequences

**Positive:**

- Single bind for all block textures; one draw call can render arbitrary block-type mixtures.
- Each layer has its own mipmap chain; no cross-bleeding.
- UVs are clean `[0, 1]`; `GL_REPEAT` works as expected for tiling textures.
- Asset workflow is one-PNG-per-block, friendly for art iteration.
- Adding a new block type = adding a layer; no atlas re-pack.

**Negative:**

- All layers must share the same dimensions. Forces a project-wide texture resolution (32×32 chosen for Layer 0; revisitable). Some artists may want some textures higher-res; that requires a separate texture array.
- Slightly more code than atlas in the loader (per-layer upload).
- Maximum array layer count is implementation-defined but typically ≥ 2048; not a practical limit for Hewnstead.

## Revisit

Re-examine if:

- A subset of blocks needs distinctly higher resolution than the rest (introduce a second texture array, not a swap).
- OpenGL is dropped in favor of Vulkan/Metal/wgpu (Layer 3+); these all support equivalent or better texture-array primitives.
