# ADR-0004: Server tick rate = 20 TPS

## Status

**Accepted** — 2026-05-01

## Context

Hewnstead's server-authoritative architecture requires a fixed simulation tick rate, decoupled from the client's render rate. This rate determines:

- **Simulation responsiveness**: how quickly world updates, AI decisions, and physics propagate.
- **CPU budget per tick**: lower TPS means more CPU available per tick for villager AI, world simulation, etc.
- **Network bandwidth (Layer 3)**: state updates broadcast at tick rate; doubling tick rate doubles bandwidth.
- **Determinism**: fixed-rate ticks are required for deterministic replay, save/load consistency, and multiplayer synchronization.

Hewnstead is a settlement / strategy / building game. Combat exists (Layer 2+) but is not action-FPS-tier. Villager AI, world simulation, and physics dominate per-tick CPU cost as Layer 2 fills in.

## Decision

**Server runs at 20 ticks per second (50 ms per tick interval). Render runs at display refresh rate (60-144+ FPS). Render uses interpolation between server tick states for smooth visual playback at higher rates.**

Tick rate is defined as a single constant `TICK_INTERVAL_SEC = 1.0 / 20.0` in server core. Client receives state updates at this rate and interpolates between received states for rendering.

## Alternatives considered

- **10 TPS (100 ms).** Rejected: too sluggish; block updates and villager actions feel laggy.
- **30 TPS (33 ms).** Considered: marginally smoother. Rejected: most players cannot perceive the difference, and the 50% CPU overhead is not justified for Hewnstead's gameplay tempo.
- **50-60 TPS.** Considered for combat-feel reasons; modeled by Valheim (50 TPS) and Terraria (60 TPS). Rejected for Layer 0: combat system is not yet designed and CPU budget for Layer 2 villager AI is a bigger concern than slight combat-feel gains. Revisit when combat prototyping happens.
- **64-128 TPS.** Rejected: required only for competitive FPS hit registration; Hewnstead is not that genre.

## Consequences

**Positive:**

- Matches Minecraft (20 TPS) → leverages community knowledge, modding precedent, "ticks per second" as a familiar unit.
- Generous CPU budget per tick (50 ms) for Layer 2 villager AI and world simulation.
- Lower network bandwidth at Layer 3 multiplayer.
- Effects/potions/timers can be expressed as integer tick counts (e.g., "lasts 60 seconds = 1200 ticks"), avoiding floating-point duration drift.

**Negative:**

- 50 ms simulation latency for player actions before server confirms. Mitigated by client-side prediction (see ADR-0008).
- If Hewnstead's combat system later requires sub-50ms reaction windows (parry timing, dodge frames), 20 TPS may feel "stiff." Revisit trigger.
- Render-tick interpolation is mandatory; cannot ship "render at tick rate" as a fallback without flickery motion.

## Revisit

Re-examine when:

- Combat system prototyping begins (Layer 2+) and player feedback indicates "stiff" or "laggy" combat feel.
- Layer 3 networking testing reveals tick-rate-related sync issues (rare, but possible for very fast vehicles or projectiles).
- Layer 2 villager AI consistently exceeds 50 ms per tick at expected population (consider opposite: lower TPS to give AI more budget).
