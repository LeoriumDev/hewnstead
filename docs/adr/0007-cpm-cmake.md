# ADR-0007: Dependency management = CPM.cmake

## Status

**Accepted** — 2026-05-01

## Context

C++ has multiple competing dependency management approaches in 2026:

- **CMake `FetchContent`** — built-in, low-level, verbose.
- **CPM.cmake** — single-file wrapper over `FetchContent`, more ergonomic.
- **vcpkg** — Microsoft-backed, central registry, manifest mode, integrates via toolchain file.
- **Conan** — Python-based, enterprise feature set, profile management.
- **git submodule** — manual, painful to maintain.
- **Vendored copies** — copy-paste source into `external/`, manual updates.

The developer is solo, on a hobby-grade time budget, on a single machine (Apple Silicon Mac). No CI cache, no team-wide build farm, no binary distribution infrastructure yet.

## Decision

**Use CPM.cmake as primary dependency manager. Vendored as `cmake/CPM.cmake` (single file, ~1200 lines). Set `CPM_SOURCE_CACHE=$HOME/.cache/CPM` globally for offline-friendly builds. Each dependency added via `CPMAddPackage(...)` in `CMakeLists.txt`.**

If a dependency's `CPMAddPackage` becomes problematic (build options not exposed, version conflicts unresolved), fall back to manual `FetchContent_Declare` for that dependency. vcpkg manifest is held in reserve as a future option (not currently used).

## Alternatives considered

- **vcpkg.** Rejected for Layer 0: requires separate toolchain setup, binary cache infrastructure, and is heavier than needed for solo development. Strong candidate if Hewnstead grows to team scale.
- **Conan.** Rejected: adds Python toolchain and Conan CLI; enterprise-scale tooling overkill for solo project.
- **Pure `FetchContent`.** Considered. Rejected on ergonomics: CPM's one-line dependency syntax (`"gh:glfw/glfw#3.4"`) reads better than multi-line `FetchContent_Declare` blocks. CPM also provides better version-conflict warnings.
- **Git submodules.** Rejected: maintenance burden, painful for solo dev, doesn't solve transitive dependencies.
- **Vendored source copies.** Rejected: manual update friction; silently outdated dependencies; no version metadata.

## Consequences

**Positive:**

- One-line dependency declarations.
- Source-based, debuggable: dependency code is right there in the CPM cache, can be `step into`-ed in the debugger.
- Global cache (`$CPM_SOURCE_CACHE`) makes second-and-later builds fully offline.
- Single-file (`cmake/CPM.cmake`) committed to repo; no developer setup required beyond CMake itself.
- Compatible with all CMake-friendly libraries (most of the C++ ecosystem).

**Negative:**

- Source-based: every dependency builds from source, which is slower on first configure than vcpkg's binary cache. Mitigated by ccache.
- Version conflicts across dependencies are warned but require manual resolution.
- Not all libraries are CMake-friendly; non-CMake libraries require shim CMakeLists or manual `add_library` work.
- Vendored CPM file becomes outdated; should be refreshed periodically (annually, or when CPM features are needed).

## Revisit

Re-examine if:

- Hewnstead grows to multi-developer team (vcpkg becomes attractive for shared binary cache).
- Dependency-build time becomes a CI bottleneck (binary caching via vcpkg or sccache).
- A required dependency cannot be packaged via CPM.
