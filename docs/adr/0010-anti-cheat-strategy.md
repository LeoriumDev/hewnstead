# ADR-0010: Anti-cheat strategy = design-level mitigation, no kernel anti-cheat

## Status

**Accepted** — 2026-05-01

## Context

Open-source client-renderable voxel games are fundamentally vulnerable to certain cheats:

- **X-ray / wall-hack**: client renders blocks the server has sent, so a modified client can render through walls.
- **Aim assistance / auto-mine**: client controls input, so a modified client can issue actions faster than a human.
- **Resource scanners**: client knows where chunks are, so a modified client can find ore distributions.

Server-authoritative architecture (ADR-0008) prevents cheats that modify world state, but cannot prevent cheats that only affect the cheater's own client. Industry approaches range from kernel-level anti-cheat (Vanguard, EAC, BattlEye) to obfuscation (Paper anti-xray) to passive behavior detection.

Hewnstead is single-developer, open-source (MIT), and primarily co-operative or lightly competitive. Most cheats hurt the cheater's own experience more than other players'.

## Decision

**Anti-cheat strategy is design-level mitigation, not technical prevention.** Specifically:

1. **Resource design**: rare ores are not so rare that finding them defines progression. Most progression depends on knowledge, building, social/villager mechanics, not lottery-ticket mining.
2. **Cooperative-first design**: PvP is opt-in faction-based, not default. Cheaters in singleplayer or co-op affect only themselves.
3. **No kernel-level anti-cheat**: incompatible with FOSS philosophy, privacy-invasive, platform-restrictive (Linux), maintenance burden far exceeds solo developer capacity.
4. **Layer 3+: chunk distance culling**: server only sends chunks within client view distance. (This is also a perf optimization; anti-cheat is a side effect.)
5. **Layer 4+ (optional): chunk obfuscation**: similar to PaperMC's anti-xray engine, mask block types in chunks the player hasn't physically explored. Adds CPU and bandwidth cost; only enable on public servers if cheating becomes a community problem.
6. **Layer 5+ (optional): behavior analysis**: log mining patterns and detect statistical anomalies (e.g., direct-line ore-mining paths). Public-server feature; not for solo or small-group play.

## Alternatives considered

- **Kernel-level anti-cheat.** Rejected explicitly: incompatible with open-source codebase (cheaters would read the source), privacy concerns, Linux unsupported, maintenance impossible at solo scale.
- **Aggressive client validation (server replays client actions).** Rejected: doubles server CPU cost to catch a small fraction of cheaters.
- **Encrypted asset streaming.** Rejected: any client must eventually decrypt to render; the developer cannot "prevent" rendering attacks.
- **Obfuscation as primary defense.** Considered for Layer 3 default; rejected. Obfuscation is incomplete (cheaters who explore-then-cheat bypass it) and adds non-trivial overhead; defer to Layer 4+ optional.

## Consequences

**Positive:**

- Solo developer is not on the hook for ongoing anti-cheat arms race.
- FOSS philosophy preserved.
- Co-op and singleplayer experience is not degraded by anti-cheat overhead.

**Negative:**

- Public-PvP servers (if hosted) will have cheaters; the design accepts this and relies on social moderation (server-admin bans) rather than technical prevention.
- Resource design constraint: cannot rely on rarity-as-difficulty for progression. Forces gameplay design toward knowledge-and-building as primary progression axes.

## Revisit

Re-examine if:

- A community emerges around competitive PvP servers and cheating becomes an active complaint.
- Specific cheat (e.g., a public X-ray client targeting Hewnstead) meaningfully degrades small-group cooperative experience.
- Hewnstead transitions to a hosted-server commercial model where cheating affects revenue.
