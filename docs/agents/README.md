# Agent reference documentation

Reference material written for AI coding agents working on OCUDU, rather than for
people. Pages here describe conventions, architecture and workflows an agent needs to
produce changes that fit the project.

The developer guidelines are documented in full at
<https://docs.ocudu.org/dev_guide/>. Pages here must stay aligned with that guide and
carry only what an agent cannot derive from the sources and gets wrong in practice.
A page earns its place by fixing a mistake agents actually repeat, not by restating a
rule they already follow.

Agent instruction files at the repository root (`CLAUDE.md`, `AGENTS.md`) stay where
their tooling expects them. Point to this directory from there instead of duplicating
its content.

## Guidelines

- One topic per file, named after the topic.
- Document a convention or invariant once agents have tripped on it, not in
  anticipation.
- Keep pages current. A stale page misleads an agent more than a missing one.
