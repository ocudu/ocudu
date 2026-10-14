# E1AP Receive Decoder Fuzzer

`e1ap_pdu_decoder_fuzzer` passes one raw E1AP ASN.1 PER PDU to the production `e1ap_asn1_packer::handle_packed_pdu`
receive path. Inputs larger than the 64 KiB resource budget are skipped. A fallback-allocation buffer owns each input,
and local gateway/message sinks outlive the local packer. No socket, executor, UE state, or persistent message state is
created.

The production unpacker handles rejection and forwards only successful decodes to the no-op message sink. The harness
never calls the transmit packer and does not encode, round-trip, suppress exceptions, or alter production checks. It
covers decoding, not E1AP procedure dispatch or bearer state machines.

`python3 tests/fuzz/e1ap/gen_corpus.py` generates two E1AP seeds in this directory's `corpus/` from textual fixtures
matching the `rx_msg` arrays in `tests/unittests/asn1/asn1_e1ap_test.cpp`:

- `when_bearer_context_setup_request_correct_then_unpacking_successful` (109 bytes).
- `when_bearer_context_setup_response_correct_then_unpacking_successful` (59 bytes).

They need no harness prefix, dictionary, or runtime options. Both request and response directions are useful to this
direction-neutral decoder. They do not assert successful stateful procedure handling. Generated files are ignored, not
checked in. Pass `--zip PATH` to write the OSS-Fuzz seed archive directly, for example
`python3 tests/fuzz/e1ap/gen_corpus.py --zip e1ap_pdu_decoder_fuzzer_seed_corpus.zip`.

The 2026-10-13 OSS-Fuzz report records 8.68% line coverage in `lib/asn1/e1ap/e1ap.cpp`, 0.11% in `e1ap_ies.cpp`, and 0%
in `e1ap_pdu_contents.cpp`. The receive packer itself is absent from that report. These measurements motivate a distinct
raw decoder target; they are not a claim of coverage gained by this untested target.

The target follows `ENABLE_FUZZTESTS` and `fuzz_targets` as documented in `tests/fuzz/README.md`. The matching OSS-Fuzz
branch runs the generator; it requires the upstream target until merged. Build, replay, sanitizer, and coverage checks
remain pending.
