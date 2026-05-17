# ADR-0014: Aesthetic lineage — voxel realism over voxel cartoon

## Status

**Accepted** — 2026-05-17

## Context

A voxel game can occupy a wide range of visual styles, even with the same underlying technology (cubic block grid, hand-painted textures, simple shaders). The same engine that produces Minecraft can produce Vintage Story; the visual gap between them is enormous despite shared foundations.

The space splits along a rough axis:

- **Cartoon end** (Minecraft, Cube World, Eco): saturated palettes, recognizable color identity per block, simple flat lighting, intentionally "gamey" feel. Blocks look like icons.
- **Realistic end** (Vintage Story, Valheim, 7 Days to Die): muted earth tones, weathered textures, dynamic lighting and shadows, ambient occlusion, fog. Blocks look like materials.

Every render-pipeline and asset-pipeline decision pulls toward one end of this axis:

- Texture filtering (`GL_NEAREST` for pixel-art crispness vs `GL_LINEAR` for smooth surfaces — though both ends typically use `NEAREST` for the texel-art aesthetic itself).
- Texture resolution (16×16 / 32×32 voxel-art vs 256×256+ semi-realistic detail).
- Texture style (high-contrast saturated colors vs muted naturalistic colors with subtle variation).
- Lighting (single-channel block-light + sky-light at the cartoon end; colored RGB lighting, sun cycle, dynamic shadows at the realistic end).
- Foliage (cross-quad billboards with hard alpha-cutout at the cartoon end; multi-layered subsurface scattering at the realistic end).
- Audio (8-bit synth at the cartoon end; recorded foley with environmental reverb at the realistic end).
- Time scale (Minecraft's 20-minute day; Vintage Story's hour-long day).

Without committing to an end, individual decisions get made inconsistently — some pulling toward cartoon, some toward realism — and the result is visual incoherence.

## Decision

**The engine's aesthetic target is the realistic end of the voxel axis: Vintage Story and Valheim, not Minecraft.**

Concrete commitments:

- **Texture filtering**: `GL_NEAREST` for both magnification and minification. The voxel-art identity is preserved (texels are visible as crisp blocks); the choice of `NEAREST` is shared with the cartoon end but for different reasons (realism preserves artisan texel decisions, cartoon preserves icon-like clarity).
- **Texture resolution**: 32×32 per block face. Higher than Minecraft (16×16) for material expression; lower than semi-realistic engines for voxel-game identity.
- **Texture style**: muted naturalistic palettes with subtle per-texel variation. Hand-painted dirt looks like dirt, not like brown plastic.
- **Lighting model**: RGB colored lighting (Vintage Story precedent). Sky cycle with sun and moon. Ambient occlusion baked into vertex attributes.
- **Foliage and water**: future-stage decisions, but committed to the realistic end — multi-layered foliage, water as a translucent volume rather than a solid blue block.
- **Time scale**: 1 real-time hour = 1 in-game day. Long enough for meaningful day-night planning, short enough for visible progression in a play session.
- **Audio**: recorded foley and environmental ambience. Footstep sounds vary by surface material. Wind, water, fire have continuous textures rather than triggered samples.

## Alternatives considered

- **Minecraft-style cartoon end.** Rejected on creative grounds. The project's identity goal (forever-playable sandbox, civilization rebuilding, sense of awe at hand-built scale) is harder to achieve when blocks look like icons. The realistic end carries emotional weight that supports the design intent.
- **Even more realistic — Teardown / Minecraft RTX direction.** Rejected on technical grounds. Path-tracing or detailed PBR pipelines require modern OpenGL features (4.6+) or move to Vulkan, conflicting with macOS 4.1 cross-platform constraint (see ADR-0003). Also requires asset pipeline investment (PBR material authoring) that isn't justified for the project's solo development timeline.
- **No aesthetic commitment — let each artist choice decide independently.** Rejected: results in incoherence. The aesthetic decision is foundational because every downstream system (lighting, audio, time scale, mob design) refers back to it.
- **Hybrid: realistic terrain, cartoon characters.** Considered but not chosen — the visual seam between the two styles is more jarring than picking one and staying with it. Cube World ships this hybrid and many players find it tonally inconsistent.

## Consequences

**Positive:**

- Every downstream design decision has a clear arbiter: "does this pull toward Vintage Story or Minecraft?" The realistic end is the answer.
- Asset pipeline can commit to specific tools and styles consistent with the realistic end: muted hand-painted textures, recorded audio, hand-rigged characters (not heavily-stylized procedural mobs).
- The project's emotional design intent (awe at scale, organic discovery, slow time) is supported by the visual treatment rather than fighting it.

**Negative:**

- Realistic-end voxel games typically demand more art investment per asset than cartoon-end games. A Minecraft dirt block can be 16 pixels of obvious brown; a Vintage Story dirt block requires hand-painted variation across 32 × 32 texels. Solo development must budget accordingly.
- Newcomers familiar with Minecraft may find the realistic end slower-to-learn — less visual contrast between materials, less immediate "videogame" affordance. Acceptable tradeoff for the intended audience.
- Some technical decisions (ambient occlusion, colored lighting) are non-trivial to implement and would be skippable on the cartoon end.

**Future implications:**

- Layer 1+ character animation chooses pipelines consistent with the realistic end (skeletal animation with weight painting; not procedural blocky characters).
- Audio sourcing prioritizes recorded foley over synthesized 8-bit samples.
- Modding API, when introduced, exposes the realistic-end primitives (texel-art block faces with optional per-face variation, multi-layered foliage definitions) rather than restricted cartoon-end primitives.

## Revisit

Permanent in spirit, revisable in detail. The "realistic end" commitment is foundational; specific choices within it (32×32 vs 16×16 textures, colored lighting vs single-channel) can be adjusted with profile or art evaluation. A move to the cartoon end would require rewriting most subsequent ADRs.

## References

- ADR-0003 (OpenGL 4.1 Core for cross-platform support) — constrains how realistic the rendering pipeline can get.
- ADR-0002 (Texture array over atlas) — independent technical choice, but compatible with this aesthetic.
- Vintage Story art direction documentation: https://www.vintagestory.at/
- Valheim art direction interviews (Iron Gate Studio) — relevant for understanding the realistic-end approach in indie scope.
