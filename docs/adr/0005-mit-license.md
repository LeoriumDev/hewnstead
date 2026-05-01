# ADR-0005: Source license = MIT

## Status

**Accepted** — 2026-05-01

## Context

Hewnstead is intended as a long-running open-source project with potential modding ecosystem and possible future commercial release. License choice affects:

- **Mod ecosystem freedom**: GPL would force mods to also be GPL.
- **Commercial flexibility**: ability to dual-license, sell on Steam, retain closed-source variant.
- **Dependency compatibility**: most permissive C++ libraries are MIT/BSD; GPL projects can absorb these but the reverse is not true.
- **Contributor barrier**: GPL deters some contributors (especially those at companies with GPL bans); MIT does not.

The developer is a solo author with no contributors yet, primary motivation is learning and craftsmanship, secondary motivation is potential future commercial release.

## Decision

**All Hewnstead source code is licensed under the MIT License. The `LICENSE` file at repo root contains the full MIT text. Every source file is implicitly covered (no per-file headers required, but acceptable if developer prefers).**

A separate `THIRD_PARTY_LICENSES.md` lists every dependency with its license (see ADR-0006).

## Alternatives considered

- **GPL v3.** Rejected: forces all mods, plugins, derivative works to be GPL. Closes the door on commercial mods. Incompatible with App Store distribution norms (some history of GPL/App Store conflicts). Solo developer doesn't need GPL's anti-corporate-fork protection.
- **AGPL v3.** Rejected: same as GPL plus extends to network use; too restrictive for a game with possible hosted server scenarios.
- **Apache 2.0.** Considered: similar to MIT but with explicit patent grant. Rejected for simplicity; MIT is shorter, more familiar to most developers, and patent risk for a voxel game is negligible.
- **BSD 2-Clause / 3-Clause.** Considered: nearly identical to MIT. Rejected purely on convention; MIT is more universally recognized.
- **Custom proprietary / source-available.** Rejected: contradicts learning-and-portfolio motivation; closes off community contribution.
- **Dual-license (MIT + commercial).** Considered for the future. Currently unnecessary (no commercial product); easy to add later if developer retains copyright via DCO or CLA.

## Consequences

**Positive:**

- Maximum flexibility for future direction (commercial, free, dual-license).
- Compatible with all permissive dependencies (MIT, BSD, Zlib, Apache, Public Domain).
- Lowers barrier to community contribution.
- Mod authors can release closed-source or differently-licensed mods.
- Steam / App Store distribution is unencumbered.

**Negative:**

- Anyone can fork Hewnstead and release a closed-source competing product without contributing back. Solo developer accepts this risk.
- No patent grant from contributors (acceptable; patent risk negligible).
- Switching to GPL later is possible (MIT → GPL allowed); switching from GPL to MIT is not (would require all contributors to agree). MIT is the safer "starting" license.

## Revisit

Re-examine if:

- A pattern emerges of corporate forks profiting without contributing back (very rare; AGPL would be the response).
- Dual-licensing becomes commercially attractive (additive — does not require changing MIT base).
