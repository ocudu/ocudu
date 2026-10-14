# GTP-U PDU Fuzzer

`gtpu_pdu_fuzzer` accepts one raw GTP-U packet per input, with no harness prefix. Inputs are capped at 65,535 payload
bytes plus the eight-byte mandatory header. It calls `gtpu_dissect_pdu` and extracts the message only after successful
dissection. Packet storage and dissection state are local to each invocation. Fallback allocation follows the existing
NGAP harness's buffer ownership model.

The target covers OCUDU's GTP-U header, optional-field, and extension-chain parsing without sockets, tunnel state, or an
encode/decode round trip. Logs are disabled in the same way as the existing stateless fuzz targets. Production
validation and assertions are unchanged.

`python3 tests/fuzz/gtpu/gen_corpus.py` generates seven GTP-U seeds in this directory's `corpus/gtpu/` from textual
fixtures matching `tests/unittests/gtpu/gtpu_test_shared.h`: `gtpu_ping_vec_teid_1`, `gtpu_ping_ext_vec`,
`gtpu_ping_two_ext_vec`, `gpdu_tpdu_1_teid_1_long_pdcp_sn_0`, `gtpu_echo_request_sn_0`, `gtpu_echo_response_sn_0`, and
`gtpu_error_indication`. They provide ordinary data, optional-header, extension, and control-message forms. No
dictionary or additional runtime options are needed for this raw binary format. Pass `--zip PATH` to write the OSS-Fuzz
seed archive directly, for example
`python3 tests/fuzz/gtpu/gen_corpus.py --zip gtpu_pdu_fuzzer_seed_corpus.zip`.

The target follows the existing `ENABLE_FUZZTESTS` and `fuzz_targets` build conventions documented in
`tests/fuzz/README.md`. OSS-Fuzz must use the matching source checkout until the upstream target is merged. Build,
runtime, and coverage validation remain pending.
