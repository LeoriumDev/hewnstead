# ADR-0006: Third-party license management = permissive only, GPL banned

## Status

**Accepted** — 2026-05-01

## Context

Because Hewnstead is MIT-licensed (ADR-0005), every dependency must be license-compatible with MIT. Common license categories:

- **Permissive** (MIT, BSD, Zlib, Apache 2.0, Public Domain, CC0, Unlicense, Boost): compatible. Require attribution but no source release.
- **LGPL**: marginally compatible — can dynamic-link without forcing GPL on whole project, but static linking or modifications trigger copyleft. Risky and inconvenient for game projects.
- **GPL / AGPL**: incompatible — forces whole project to GPL. Cannot be used in MIT project.

Layer 0's expected dependencies (CPM-managed) are all permissive: GLFW (Zlib), GLAD (multi-license), GLM (MIT), {fmt} (MIT), spdlog (MIT), nlohmann/json (MIT), zstd (BSD 3-Clause selected from BSD/GPLv2 dual), SQLite (Public Domain), FastNoise2 (MIT), ImGui (MIT), Tracy (BSD 3-Clause), doctest (MIT), EnTT (MIT), Jolt (MIT). Layer 1+ adds more but the policy is the same.

## Decision

**Hewnstead may depend only on permissive-licensed dependencies (MIT, BSD 2/3-Clause, Zlib, Apache 2.0, Public Domain, CC0, Unlicense, Boost). LGPL requires explicit case-by-case approval. GPL and AGPL are banned.**

Every dependency is recorded in `THIRD_PARTY_LICENSES.md` with its license, copyright holder, and a path to the license text. CPM downloads each dependency's `LICENSE` file into `external/<dep>/`; these files must remain in the repository.

For dependencies with multi-license options (zstd: BSD 3-Clause / GPLv2; GLAD: MIT / Apache 2.0 / Public Domain), Hewnstead explicitly selects the most permissive option (BSD 3-Clause; MIT respectively). The selection is documented in `THIRD_PARTY_LICENSES.md`.

For binary distribution (Steam release, GitHub release zip), a `licenses.txt` bundling all third-party license texts is included alongside the executable. Layer 4+ adds an in-game "Credits" screen displaying the same.

## Alternatives considered

- **Allow GPL dependencies via dual-licensing.** Rejected: would force Hewnstead itself to GPL, contradicting ADR-0005.
- **Allow LGPL freely.** Rejected for Layer 0: dynamic-linking requirements add packaging complexity. Acceptable case-by-case if a strong need arises (e.g., a critical LGPL library with no permissive equivalent).
- **No formal license management policy.** Rejected: license violations are silent and accumulate; one GPL dependency contaminates retroactively. Active policy is cheap insurance.

## Consequences

**Positive:**

- Clean legal story for any future commercial release.
- No surprise GPL contamination.
- `THIRD_PARTY_LICENSES.md` is a single source of truth.
- All dependencies' license texts preserved in `external/`, satisfying attribution obligations.

**Negative:**

- Some otherwise-attractive libraries (mostly server / DRM / niche audio codecs) are GPL and unavailable. Acceptable: permissive alternatives exist for everything Hewnstead needs.
- Manual maintenance of `THIRD_PARTY_LICENSES.md` per added dependency. Mitigated by CI script (Layer 3+) that diffs the dependency list against the document.

## Revisit

Re-examine if:

- A critical-path dependency exists only as GPL/LGPL (so far, none).
- Hewnstead's overall license changes (then ADR-0005 supersedes; this ADR's scope auto-adjusts).
- Distribution model changes (e.g., source-only releases versus binary releases — requirements differ slightly).
