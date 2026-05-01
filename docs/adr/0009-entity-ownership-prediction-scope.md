# ADR-0009: Entity ownership and prediction scope

## Status

**Accepted** — 2026-05-01

## Context

ADR-0008 establishes optimistic prediction for player actions. Not all actions are safe to predict: actions that depend on shared world state (item pickup when multiple players compete) or that trigger transactional side effects (crafting consumes materials and produces output) have prediction-rollback hazards.

In particular:

- **Item pickup**: Two players running toward the same dropped item; server arbitrates "who gets it." If both clients predict pickup, both see the item disappear, but server confirms only one — the loser sees the item reappear, awkwardly.
- **Container open / inventory transfers**: Container contents are authoritative; client cannot know contents until server sends them.
- **Crafting**: Consumes materials and produces output atomically; rollback is complex if any step fails.
- **Respawn**: Server determines respawn location based on save state, spawn rules, and possibly other players' presence; client cannot predict where the player will reappear.
- **Chat messages, voice, social actions**: not state-changing in the world simulation sense; latency is acceptable.

Conversely, **movement and block edits** are safe to predict: each player has their own movement state, and block edits are local operations whose rollback (revert one block) is trivial.

## Decision

**Prediction is enabled for the following actions: player movement, block place, block break, tool use (attacks, animations).**

**Prediction is DISABLED for the following actions: item pickup, container open / inventory transfer, crafting / smelting / processing recipes, social actions (chat, etc.), respawn.**

Item entities are server-owned; client mirrors them via interpolation of position updates received from server. Client never decides "I picked up X" — server does, based on physics proximity, and informs the client via inventory-update message. There is **no Client→Server "pickup" message** in the protocol.

State transitions on the server (especially inventory changes) are implemented atomically — either all related state changes succeed, or none do.

## Alternatives considered

- **Predict everything.** Rejected: pickup races and crafting rollback are visible failure modes that hurt UX more than the saved latency helps.
- **Predict nothing.** Rejected: movement and block edits feel terrible without prediction.
- **Predict everything except item pickup.** Considered. Rejected: container and crafting also have transactional issues. Drawing the line at "actions that affect only the predicting player's local view" is cleaner.

## Consequences

**Positive:**

- No race conditions on shared resources (item pickup is server-arbitrated).
- Crafting is safe: client UI shows "in progress" → server confirms → client updates inventory.
- Interpolated entity rendering provides smooth visuals without prediction risk.

**Negative:**

- Item pickup has visible latency (tens of ms). Acceptable: pickup isn't a snap-judgment action like movement.
- Container open has loading-screen feel for a single round trip. Acceptable.
- Slightly more message types than a fully-predicted protocol (extra Server→Client messages for entity state).

## Revisit

Re-examine if:

- A specific non-predicted action's latency becomes a player complaint.
- Layer 4 introduces a real-time competitive mode where pickup races matter (very unlikely given Hewnstead's settlement-game tone).
