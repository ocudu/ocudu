# Code comment conventions

Advisory. Full developer guidelines: <https://docs.ocudu.org/dev_guide/>.

Apply to comments you write or are already modifying. Never rewrite a comment you were
not otherwise touching.

## Shape

- Declaration takes a noun phrase: `/// NR duplex mode.`
- Function takes a third-person verb phrase: `/// Returns the active UE count.`
- Omit `\brief` unless the comment runs to more than one paragraph.
- Comment goes above the code, never at the end of the line.

## Language

- Present tense.
- Active voice.
- No `This function`, `This class` or `This method` openers.
- One idea per sentence.
- Cut filler: `in order to` to `to`, `is used to hold` to `holds`. Drop `simply`,
  `just`, `obviously`.
- Facts only. No `added`, `changed`, `now handles`, `should be`, `for now`.
- No em dashes.

## What to say

- Public interface: terse, usually one line. Never why. Never name callers.
- Private member: one line. What it holds, not how it is used.
- Function body: why, not what.

## Specifications

- Cite the clause: `as per TS 38.331, Section 6.3.2`.
- Prefix a public constant with `[Implementation-defined]` when OCUDU chooses the
  value rather than a 3GPP, O-RAN or SCF specification. Otherwise cite the clause.
- Never name another vendor or implementation.
