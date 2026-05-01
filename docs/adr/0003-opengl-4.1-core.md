# ADR-0003: Graphics API = Apple-native OpenGL 4.1 Core

## Status

**Accepted** — 2026-05-01

## Context

Hewnstead targets macOS (primary), Linux, and Windows. The graphics API choice affects every rendering decision for the next several years. Major options in 2026:

- **OpenGL 4.1 Core** — Apple's deprecated-but-functional ceiling on macOS; supported natively on Linux/Windows.
- **OpenGL 4.6** — modern OpenGL; not supported on macOS at all.
- **Vulkan** — modern, low-level, cross-platform via MoltenVK on macOS.
- **Metal** — Apple-native, Apple-only.
- **wgpu / wgpu-native** — modern WebGPU-derived API with cross-platform backends.
- **bgfx** — high-level abstraction over multiple backends.
- **MGL (openglonmetal/MGL)** — third-party "OpenGL 4.6 on Metal" translator.

The developer is an OpenGL beginner with limited time (~12 hr/week). Tutorial ecosystem (LearnOpenGL, 0fps.net) overwhelmingly assumes OpenGL 3.3 / 4.1.

## Decision

**Use Apple-native OpenGL 4.1 Core for all of Layer 0 through Layer 4. Defer any change of graphics API until Layer 5 or until Apple drops OpenGL support entirely (whichever comes first).**

Rendering code is structured so that draw-call abstraction sits behind an interface, making a future swap to Vulkan, Metal, or wgpu a Layer-3-or-later project, not a Layer-0 concern.

## Alternatives considered

- **OpenGL 4.6.** Rejected: macOS doesn't support it. Pinning to 4.6 would mean Mac is a second-class platform for the developer's primary hardware. Compute shaders, SSBO, bindless textures — none available on Mac OpenGL.
- **Vulkan + MoltenVK.** Rejected for Layer 0: Hello Triangle in Vulkan is 800+ lines of boilerplate. The first six months would be spent fighting the API, not building voxels. Tutorial ecosystem assumes comfort with graphics first. Revisit at Layer 3+ when developer is past the initial learning curve.
- **Metal.** Rejected: Apple-only. Linux and Windows targets disappear.
- **wgpu-native.** Considered: modern, cross-platform, future-proof. Rejected for Layer 0: tutorials are sparse, the developer has already begun reading LearnOpenGL, and the project would lose weeks of momentum switching now. Strong candidate for Layer 4+ port if needed.
- **bgfx.** Considered: well-maintained abstraction. Rejected for the same momentum reason; also adds a layer of magic between developer and the underlying API while still learning.
- **MGL (openglonmetal/MGL).** Rejected explicitly: it is a partial implementation of OpenGL 4.6 by a single contributor, Mac-only, unstable for production. A multi-year project cannot rest on a hobby fork.

## Consequences

**Positive:**

- Fastest path to first rendered voxel.
- Vast tutorial ecosystem (LearnOpenGL, 0fps.net, Reddit r/GraphicsProgramming).
- Concepts (vertex buffers, shaders, MVP, depth test) are transferable to Vulkan/Metal/wgpu when porting later.
- Apple's OpenGL implementation, despite deprecation, has been stable since 2018 and shows no signs of imminent removal as of April 2026.

**Negative:**

- macOS deprecated OpenGL in 2018; Apple could remove the compatibility layer in a future macOS release. Risk is real but slow-moving (Apple typically gives 1-2 year deprecation windows).
- No compute shaders on Mac. GPU-driven meshing is off the table for Layer 0-4. Acceptable: Hewnstead's mesher runs fine on CPU worker threads.
- No SSBO, no bindless textures, no mesh shaders. None of these are needed for Layer 0-4.

## Revisit

Re-examine when any of the following occurs:

- Apple announces removal of OpenGL compatibility in a macOS release.
- Hewnstead's rendering performance becomes GPU-bound and a feature available only in Vulkan/Metal/wgpu would unblock further progress.
- Layer 5+ porting work begins (e.g., porting to mobile, console, or high-end RTX raytracing experiments).

The current "checkpoint" is 2028, or the next macOS release that deprecates further (whichever first).
