# Hewnstead ADRs

This directory contains Architecture Decision Records — short documents
recording significant design decisions and the reasoning behind them.

## How to add an ADR

1. Copy `0000-template.md` to `NNNN-short-title.md`
   (NNNN = next available number, padded to 4 digits)
2. Fill in the sections below
3. Commit with message: `docs: ADR-NNNN <short title>`

## Section guidelines

### Status
Newly proposed: `Proposed`. After confirmation: `Accepted` with date.
If overturned later, update to `Superseded by ADR-NNNN`.

### Context
Keep this factual, not argumentative. The arguments belong in
"Decision" and "Alternatives considered."

### Decision
The core decision in one or two sentences. Avoid burying it in prose.

### Alternatives considered
The most valuable section for future-you. Without it, the team will
propose rejected alternatives again because the reasoning is lost.

Distinguish "rejected" (clearly worse) from "considered but not chosen"
(reasonable, but X wasn't the right fit).

### Consequences
Be honest about tradeoffs — every decision has downsides. If you can't
think of a "Negative" consequence, you haven't analyzed enough.

### Revisit
Some decisions are one-way doors (mark as "permanent"). Others should
be reconsidered when conditions change. Specify the trigger.

### References
External resources only. Freeform thoughts go in Context or Alternatives.

## ADR list

| Number   | Title                                                                         | Status   |
| -------- | ----------------------------------------------------------------------------- | -------- |
| ADR-0001 | Chunk size = 32³                                                              | Accepted |
| ADR-0002 | Texture array over texture atlas                                              | Accepted |
| ADR-0003 | Graphics API = Apple-native OpenGL 4.1 Core                                   | Accepted |
| ADR-0004 | Server tick rate = 20 TPS                                                     | Accepted |
| ADR-0005 | Source license = MIT                                                          | Accepted |
| ADR-0006 | Third-party license management = permissive only, GPL banned                  | Accepted |
| ADR-0007 | Dependency management = CPM.cmake                                             | Accepted |
| ADR-0008 | Client-server architecture with optimistic prediction                         | Accepted |
| ADR-0009 | Entity ownership and prediction scope                                         | Accepted |
| ADR-0010 | Anti-cheat strategy = design-level mitigation, no kernel anti-cheat           | Accepted |
| ADR-0011 | Vertex format evolves with engine milestones, not designed forward-compatible | Accepted |
| ADR-0012 | Mesh class specificity over genericity                                        | Accepted |
| ...      | ...                                                                           | ...      |