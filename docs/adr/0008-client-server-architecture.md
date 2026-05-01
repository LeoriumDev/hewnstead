# ADR-0008: Client-server architecture with optimistic prediction

## Status

**Accepted** — 2026-05-01

## Context

Hewnstead is server-authoritative from day zero (singleplayer is a 1-player localhost server; multiplayer is the same code with a network transport). The naive design — client sends action, blocks until server confirms — is correct but feels laggy at any non-zero latency. Modern multiplayer games solve this with **client-side prediction + server reconciliation**:

- Client immediately performs the action locally and renders the predicted result.
- Client sends the action message to server.
- Server validates and either confirms (no-op for the client; the prediction was right) or rejects (client rolls back).

This is the design used by Minecraft, CS:GO, Overwatch, Valheim, Rocket League. Glenn Fiedler's "What every programmer needs to know about game networking" is the canonical reference.

For Layer 0, the server runs in-process (localhost, zero latency), so prediction is invisible — direct execution is indistinguishable from prediction-then-confirm. But the **architecture** must support prediction from day zero, because Layer 3 introduces real network latency and forcing prediction in late requires touching every action handler.

## Decision

**All player actions cross a message boundary between client and server. The client-side prediction layer is built into the client's action-handling code from Layer 0. In Layer 0 the prediction is trivial (server confirms immediately because it's localhost), but the code path is the real one.**

For each predictable action (movement, block place, block break, tool use), the client:

1. Predicts the action locally (updates local world copy, plays sound, etc.).
2. Sends a typed message to the server.
3. Tracks the prediction in a "pending actions" list.

The server:

1. Receives the message.
2. Validates (distance, permissions, resource availability, etc.).
3. Either commits the change to authoritative state and broadcasts a confirmation, or rejects and sends a rollback message.

The client:

1. On confirmation, removes the pending action.
2. On rejection, rolls back the local prediction and removes the pending action.

## Alternatives considered

- **Synchronous server-authoritative (no prediction).** Rejected: introduces visible latency at any non-zero ping. Layer 0 would feel fine (localhost) but Layer 3 would feel terrible without rebuilding half the input handling.
- **Client-authoritative.** Rejected: cheating becomes trivial (a modified client can change world state directly).
- **Lock-step deterministic simulation (RTS-style).** Rejected: requires cross-platform deterministic floating-point behavior, which is infeasible across Apple Silicon, x86, and various GPUs. Also conflicts with observer-dependent simulation in Layer 2.
- **Hybrid (some client-authoritative for cosmetics, server for important state).** Rejected: complicates the model; clean server-authoritative is simpler and has no real downside given prediction.

## Consequences

**Positive:**

- Layer 0 architecture supports Layer 3 multiplayer without rewriting action handling.
- Cheating resistance: world state cannot change without server approval.
- Deterministic save/load: server's authoritative state is the source of truth.
- Replayable: messages are the only input to server, so message log replay reproduces world state.

**Negative:**

- Every player action requires a message struct, a handler on each side, and rollback logic. More boilerplate per action than direct function calls.
- Rollback logic is non-trivial for some actions (especially when an action triggers chain effects, e.g., breaking a block that then causes neighboring blocks to fall).
- Some actions cannot be predicted safely (item pickup, container open, crafting); these have explicit synchronous-server-confirms flow. See ADR-0009.

## Revisit

Re-examine if:

- A specific action's prediction-rollback logic becomes unmanageable (consider making that action non-predicted).
- A different determinism model is desired for replays (extremely unlikely; requires lock-step which conflicts with other ADRs).
