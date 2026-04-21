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

| Number | Title | Status |
| --- | --- | --- |
| ADR-0001 | Chunk size 32³ | Accepted |
| ADR-0002 | Texture array over atlas | Accepted |
| ... | ... | ... |