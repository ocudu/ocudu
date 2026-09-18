// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_processor_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/upper/pucch_formats3_4_helpers.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/pucch/pucch_info.h"
#include "ocudu/ran/uci/uci_info.h"
#include "ocudu/ran/uci/uci_part2_size_calculator.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/transform_optional.h"

using namespace ocudu;

/// \brief Looks at the output of the validator and, if unsuccessful, fills \c msg with the error message.
///
/// This is used to call the validator inside the process methods only if asserts are active.
[[maybe_unused]] static bool handle_validation(std::string& msg, const error_type<std::string>& err)
{
  bool is_success = err.has_value();
  if (!is_success) {
    msg = err.error();
  }
  return is_success;
}

/// Compute the number of soft bits assigned to UCI Part 1 as per TS38.212 Table 6.3.1.4.1-1.
static unsigned
get_nof_uci_part1_softbits(unsigned nof_bits_part1, unsigned nof_softbits, float max_code_rate, unsigned mod_order)
{
  ocudu_assert(nof_bits_part1 != 0, "The number of bits of UCI Part 1 cannot be zero.");
  ocudu_assert(nof_softbits != 0, "The total number of softbits of UCI payload cannot be zero.");
  ocudu_assert(max_code_rate > 0.0F, "The maximum code rate cannot be zero.");
  ocudu_assert(mod_order != 0, "The modulation order cannot be zero.");

  // Get the number of bits for UCI Part 1 after codeblock segmentation and CRC attachment.
  unsigned nof_bits_part1_with_crc = nof_bits_part1 + get_uci_nof_crc_bits(nof_bits_part1, nof_softbits);

  // Get the number of rate matched output bits for UCI Part 1.
  unsigned result =
      std::ceil(static_cast<float>(nof_bits_part1_with_crc) / max_code_rate / static_cast<float>(mod_order)) *
      mod_order;

  return std::min(result, nof_softbits);
}

/// Return the maximum possible CSI Part 2 payload size described by the size description.
static unsigned get_max_nof_csi_part2_bits(const uci_part2_size_description& csi_part2_size)
{
  unsigned max_nof_bits = 0;
  for (const uci_part2_size_description::entry& entry : csi_part2_size.entries) {
    if (!entry.map.empty()) {
      max_nof_bits += *std::max_element(entry.map.begin(), entry.map.end());
    }
  }
  return max_nof_bits;
}

/// Decode UCI payloads containing CSI Part 1 and CSI Part 2 reports.
static uci_status decode_uci_payload(uci_decoder&                      decoder,
                                     pucch_uci_message&                message,
                                     span<const log_likelihood_ratio>  llr,
                                     float                             max_code_rate,
                                     const uci_decoder::configuration& decoder_config,
                                     const uci_part2_size_description& csi_part2_size)
{
  // Directly decode the UCI payload if only one part is present.
  if (csi_part2_size.entries.empty()) {
    return decoder.decode(message.get_full_payload(), llr, decoder_config);
  }

  // Compute the UCI Part 1 size from the number of bits for SR, HARQ-ACK and CSI Part 1.
  unsigned nof_bits_uci_part1 = message.get_expected_nof_sr_bits() + message.get_expected_nof_harq_ack_bits() +
                                message.get_expected_nof_csi_part1_bits();

  // Calculate the number of rate matched output bits for UCI Part 1 according to TS38.212 Table 6.3.1.4.1-1.
  unsigned nof_softbits_uci_part1 = get_nof_uci_part1_softbits(
      nof_bits_uci_part1, llr.size(), max_code_rate, get_bits_per_symbol(decoder_config.modulation));

  // The UCI Part 2 rate matched output bits are the remaining after UCI Part 1.
  unsigned nof_softbits_uci_part2 = llr.size() - nof_softbits_uci_part1;

  // Decode first part. Skip second part if the first part was not decoded successfully or there are no soft bits for
  // Part 2.
  uci_status status = decoder.decode(
      message.get_full_payload().first(nof_bits_uci_part1), llr.first(nof_softbits_uci_part1), decoder_config);
  if ((status != uci_status::valid) || (nof_softbits_uci_part2 == 0)) {
    return status;
  }

  // The CSI Part 2 size depends on the decoded CSI Part 1.
  unsigned nof_bits_part2 =
      uci_part2_get_size(uci_payload_type(message.get_csi_part1_bits().begin(), message.get_csi_part1_bits().end()),
                         csi_part2_size)
          .value();

  message.set_expected_nof_csi_part2_bits(nof_bits_part2);
  if (nof_bits_part2 == 0) {
    return uci_status::valid;
  }

  if (nof_bits_part2 >= 3) {
    return decoder.decode(message.get_csi_part2_bits(), llr.last(nof_softbits_uci_part2), decoder_config);
  }

  // TS38.212 Section 6.3.1.1.3 requires CSI Part 2 bit sequences shorter than 3 bits to be zero-padded to length 3
  // before channel coding.
  std::array<uint8_t, 3> csi_part2_with_padding = {};
  status = decoder.decode(span<uint8_t>(csi_part2_with_padding), llr.last(nof_softbits_uci_part2), decoder_config);
  if (status == uci_status::valid) {
    ocuduvec::copy(message.get_csi_part2_bits(), span<const uint8_t>(csi_part2_with_padding).first(nof_bits_part2));
  }

  return status;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&                   grid,
                                                     const pucch_processor::format0_configuration& config)
{
  [[maybe_unused]] std::string msg;
  ocudu_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

  // Calculate actual PRB.
  std::optional<unsigned> second_hop_prb;
  if (config.second_hop_prb.has_value()) {
    second_hop_prb.emplace(*config.second_hop_prb + config.bwp_start_rb);
  }

  pucch_detector::format0_configuration detector_config;
  detector_config.slot                                                     = config.slot - config.slot_offset;
  detector_config.starting_prb                                             = config.starting_prb + config.bwp_start_rb;
  detector_config.second_hop_prb                                           = second_hop_prb;
  detector_config.start_symbol_index                                       = config.start_symbol_index;
  detector_config.nof_symbols                                              = config.nof_symbols;
  detector_config.initial_cyclic_shift                                     = config.initial_cyclic_shift;
  detector_config.n_id                                                     = config.n_id;
  detector_config.nof_harq_ack                                             = config.nof_harq_ack;
  detector_config.sr_opportunity                                           = config.sr_opportunity;
  detector_config.ports                                                    = config.ports;
  std::pair<pucch_uci_message, channel_state_information> detection_result = detector->detect(grid, detector_config);

  pucch_processor_result result;
  result.message = detection_result.first;
  result.csi     = detection_result.second;

  return result;
}

pucch_format1_map<pucch_processor_result> pucch_processor_impl::process(const resource_grid_reader&        grid,
                                                                        const format1_batch_configuration& batch_config)
{
  const format1_common_configuration& common_config = batch_config.common_config;
  format1_configuration               proc_config   = {.context              = {},
                                                       .slot                 = common_config.slot,
                                                       .slot_offset          = common_config.slot_offset,
                                                       .bwp_size_rb          = common_config.bwp_size_rb,
                                                       .bwp_start_rb         = common_config.bwp_start_rb,
                                                       .cp                   = common_config.cp,
                                                       .starting_prb         = common_config.starting_prb,
                                                       .second_hop_prb       = common_config.second_hop_prb,
                                                       .n_id                 = common_config.n_id,
                                                       .nof_harq_ack         = 0,
                                                       .ports                = common_config.ports,
                                                       .initial_cyclic_shift = 0,
                                                       .nof_symbols          = common_config.nof_symbols,
                                                       .start_symbol_index   = common_config.start_symbol_index,
                                                       .time_domain_occ      = 0};

  pucch_format1_map<unsigned> mux_harq_size;

  for (const auto& this_pucch : batch_config.entries) {
    unsigned initial_cyclic_shift = this_pucch.initial_cyclic_shift;
    unsigned time_domain_occ      = this_pucch.time_domain_occ;
    unsigned nof_harq_ack         = this_pucch.value.nof_harq_ack;

    proc_config.context              = this_pucch.value.context;
    proc_config.initial_cyclic_shift = initial_cyclic_shift;
    proc_config.time_domain_occ      = time_domain_occ;
    proc_config.nof_harq_ack         = nof_harq_ack;

    [[maybe_unused]] std::string msg;
    ocudu_assert(handle_validation(msg, pdu_validator->is_valid(proc_config)), "{}", msg);

    mux_harq_size.insert(initial_cyclic_shift, time_domain_occ, nof_harq_ack);
  }

  // Fill the detector configuration - recall that time_domain_occ, initial_cyclic_shift and nof_harq_ack are set
  // via mux_harq_size.
  pucch_detector::format1_configuration detector_config = {
      .slot               = common_config.slot - common_config.slot_offset,
      .cp                 = common_config.cp,
      .starting_prb       = common_config.starting_prb + common_config.bwp_start_rb,
      .second_hop_prb     = transform_optional(common_config.second_hop_prb, std::plus(), common_config.bwp_start_rb),
      .start_symbol_index = common_config.start_symbol_index,
      .nof_symbols        = common_config.nof_symbols,
      .group_hopping      = pucch_group_hopping::NEITHER,
      .ports              = common_config.ports,
      .beta_pucch         = 1.0F,
      .n_id               = common_config.n_id};

  const pucch_format1_map<pucch_detector::pucch_detection_result_csi>& detection_results =
      detector->detect(grid, detector_config, mux_harq_size);

  // Create the detection results for this detection batch.
  pucch_format1_map<pucch_processor_result> batch_results;
  for (const auto& this_result : detection_results) {
    batch_results.insert(this_result.initial_cyclic_shift,
                         this_result.time_domain_occ,
                         {.csi              = this_result.value.csi,
                          .message          = this_result.value.detection_result.uci_message,
                          .detection_metric = this_result.value.detection_result.detection_metric});
  }
  return batch_results;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format2_configuration& config)
{
  // Check that the PUCCH Format 2 configuration is valid.
  [[maybe_unused]] std::string msg;
  ocudu_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

  pucch_processor_result result;

  // PUCCH UCI message configuration.
  pucch_uci_message::configuration pucch_uci_message_config;
  pucch_uci_message_config.nof_sr        = config.nof_sr;
  pucch_uci_message_config.nof_harq_ack  = config.nof_harq_ack;
  pucch_uci_message_config.nof_csi_part1 = config.nof_csi_part1;
  pucch_uci_message_config.nof_csi_part2 = 0;

  result.message = pucch_uci_message(pucch_uci_message_config);

  // Channel estimator configuration.
  dmrs_pucch_estimator::format2_configuration estimator_config;
  estimator_config.slot               = config.slot - config.slot_offset;
  estimator_config.cp                 = config.cp;
  estimator_config.group_hopping      = pucch_group_hopping::NEITHER;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.prbs.start();
  estimator_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  estimator_config.nof_prb            = config.prbs.length();
  estimator_config.n_id_0             = config.n_id_0;
  estimator_config.ports.assign(config.ports.begin(), config.ports.end());

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = config.bwp_start_rb + config.bwp_size_rb;
  dims.nof_symbols   = get_nsymb_per_slot(config.cp);
  dims.nof_rx_ports  = config.ports.size();
  dims.nof_tx_layers = pucch_constants::NOF_LAYERS;

  estimates.resize(dims);

  // Perform channel estimation.
  channel_estimator->estimate(estimates, grid, estimator_config);

  estimates.get_channel_state_information(result.csi);

  span<log_likelihood_ratio> llr =
      span<log_likelihood_ratio>(temp_llr).first(pucch_constants::f2::NOF_DATA_SUBC_PER_RB * config.prbs.length() *
                                                 config.nof_symbols * get_bits_per_symbol(modulation_scheme::QPSK));

  // PUCCH Format 2 demodulator configuration.
  pucch_demodulator::format2_configuration demod_config;
  demod_config.rx_ports           = config.ports;
  demod_config.first_prb          = config.bwp_start_rb + config.prbs.start();
  demod_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  demod_config.nof_prb            = config.prbs.length();
  demod_config.start_symbol_index = config.start_symbol_index;
  demod_config.nof_symbols        = config.nof_symbols;
  demod_config.rnti               = config.rnti;
  demod_config.n_id               = config.n_id;

  // Perform demodulation.
  demodulator->demodulate(llr, grid, estimates, demod_config);

  // UCI decoder configuration.
  uci_decoder::configuration decoder_config;
  decoder_config.modulation = modulation_scheme::QPSK;

  // Decode UCI payload.
  result.message.set_status(
      decode_uci_payload(*decoder, result.message, llr, config.max_code_rate, decoder_config, config.csi_part2_size));

  // Expected UCI payload length in number of bits.
  unsigned expected_nof_uci_bits =
      config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + result.message.get_expected_nof_csi_part2_bits();

  // Assert that the decoded UCI payload has the expected number of bits.
  ocudu_assert(result.message.get_full_payload().size() == expected_nof_uci_bits,
               "Decoded UCI payload length, i.e., {}, does not match expected number of UCI bits, i.e., {}.",
               result.message.get_full_payload().size(),
               expected_nof_uci_bits);

  return result;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format3_configuration& config)
{
  // Check that the PUCCH Format 3 configuration is valid.
  [[maybe_unused]] std::string msg;
  ocudu_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

  pucch_processor_result result;

  // PUCCH UCI message configuration.
  pucch_uci_message::configuration pucch_uci_message_config;
  pucch_uci_message_config.nof_sr        = config.nof_sr;
  pucch_uci_message_config.nof_harq_ack  = config.nof_harq_ack;
  pucch_uci_message_config.nof_csi_part1 = config.nof_csi_part1;
  pucch_uci_message_config.nof_csi_part2 = 0;

  result.message = pucch_uci_message(pucch_uci_message_config);

  // Channel estimator configuration.
  dmrs_pucch_estimator::format3_configuration estimator_config;
  estimator_config.slot               = config.slot - config.slot_offset;
  estimator_config.cp                 = config.cp;
  estimator_config.group_hopping      = pucch_group_hopping::NEITHER;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.prbs.start();
  estimator_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  estimator_config.nof_prb            = config.prbs.length();
  estimator_config.n_id               = config.n_id_hopping;
  estimator_config.ports.assign(config.ports.begin(), config.ports.end());
  estimator_config.additional_dmrs = config.additional_dmrs;

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = config.bwp_start_rb + config.bwp_size_rb;
  dims.nof_symbols   = get_nsymb_per_slot(config.cp);
  dims.nof_rx_ports  = config.ports.size();
  dims.nof_tx_layers = pucch_constants::NOF_LAYERS;

  estimates.resize(dims);

  // Perform channel estimation.
  channel_estimator->estimate(estimates, grid, estimator_config);

  estimates.get_channel_state_information(result.csi);

  const symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  const modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  span<log_likelihood_ratio> llr = span<log_likelihood_ratio>(temp_llr).first(
      NOF_SUBCARRIERS_PER_RB * config.prbs.length() * (config.nof_symbols - dmrs_symb_mask.count()) *
      get_bits_per_symbol(mod_scheme));

  // PUCCH Format 3 demodulator configuration.
  pucch_demodulator::format3_configuration demod_config;
  demod_config.rx_ports           = config.ports;
  demod_config.first_prb          = config.bwp_start_rb + config.prbs.start();
  demod_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  demod_config.nof_prb            = config.prbs.length();
  demod_config.start_symbol_index = config.start_symbol_index;
  demod_config.nof_symbols        = config.nof_symbols;
  demod_config.rnti               = config.rnti;
  demod_config.n_id               = config.n_id_scrambling;
  demod_config.additional_dmrs    = config.additional_dmrs;
  demod_config.pi2_bpsk           = config.pi2_bpsk;

  // Perform demodulation.
  demodulator->demodulate(llr, grid, estimates, demod_config);

  // UCI decoder configuration.
  uci_decoder::configuration decoder_config;
  decoder_config.modulation = mod_scheme;

  // Decode UCI payload.
  result.message.set_status(
      decode_uci_payload(*decoder, result.message, llr, config.max_code_rate, decoder_config, config.csi_part2_size));

  // Expected UCI payload length in number of bits.
  unsigned expected_nof_uci_bits =
      config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + result.message.get_expected_nof_csi_part2_bits();

  // Assert that the decoded UCI payload has the expected number of bits.
  ocudu_assert(result.message.get_full_payload().size() == expected_nof_uci_bits,
               "Decoded UCI payload length, i.e., {}, does not match expected number of UCI bits, i.e., {}.",
               result.message.get_full_payload().size(),
               expected_nof_uci_bits);

  return result;
}

pucch_processor_result pucch_processor_impl::process(const resource_grid_reader&  grid,
                                                     const format4_configuration& config)
{
  // Check that the PUCCH Format 4 configuration is valid.
  [[maybe_unused]] std::string msg;
  ocudu_assert(handle_validation(msg, pdu_validator->is_valid(config)), "{}", msg);

  pucch_processor_result result;

  // PUCCH UCI message configuration.
  pucch_uci_message::configuration pucch_uci_message_config;
  pucch_uci_message_config.nof_sr        = config.nof_sr;
  pucch_uci_message_config.nof_harq_ack  = config.nof_harq_ack;
  pucch_uci_message_config.nof_csi_part1 = config.nof_csi_part1;
  pucch_uci_message_config.nof_csi_part2 = 0;

  result.message = pucch_uci_message(pucch_uci_message_config);

  // Channel estimator configuration.
  dmrs_pucch_estimator::format4_configuration estimator_config;
  estimator_config.slot               = config.slot - config.slot_offset;
  estimator_config.cp                 = config.cp;
  estimator_config.group_hopping      = pucch_group_hopping::NEITHER;
  estimator_config.start_symbol_index = config.start_symbol_index;
  estimator_config.nof_symbols        = config.nof_symbols;
  estimator_config.starting_prb       = config.bwp_start_rb + config.starting_prb;
  estimator_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  estimator_config.n_id               = config.n_id_hopping;
  estimator_config.ports.assign(config.ports.begin(), config.ports.end());
  estimator_config.additional_dmrs = config.additional_dmrs;
  estimator_config.occ_index       = config.occ_index;

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = config.bwp_start_rb + config.bwp_size_rb;
  dims.nof_symbols   = get_nsymb_per_slot(config.cp);
  dims.nof_rx_ports  = config.ports.size();
  dims.nof_tx_layers = pucch_constants::NOF_LAYERS;

  estimates.resize(dims);

  // Perform channel estimation.
  channel_estimator->estimate(estimates, grid, estimator_config);

  estimates.get_channel_state_information(result.csi);

  const symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  const modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  span<log_likelihood_ratio> llr = span<log_likelihood_ratio>(temp_llr).first(
      NOF_SUBCARRIERS_PER_RB * (config.nof_symbols - dmrs_symb_mask.count()) * get_bits_per_symbol(mod_scheme) /
      config.occ_length);

  // PUCCH Format 4 demodulator configuration.
  pucch_demodulator::format4_configuration demod_config;
  demod_config.rx_ports           = config.ports;
  demod_config.first_prb          = config.bwp_start_rb + config.starting_prb;
  demod_config.second_hop_prb     = transform_optional(config.second_hop_prb, std::plus(), config.bwp_start_rb);
  demod_config.start_symbol_index = config.start_symbol_index;
  demod_config.nof_symbols        = config.nof_symbols;
  demod_config.rnti               = config.rnti;
  demod_config.n_id               = config.n_id_scrambling;
  demod_config.additional_dmrs    = config.additional_dmrs;
  demod_config.pi2_bpsk           = config.pi2_bpsk;
  demod_config.occ_index          = config.occ_index;
  demod_config.occ_length         = config.occ_length;

  // Perform demodulation.
  demodulator->demodulate(llr, grid, estimates, demod_config);

  // UCI decoder configuration.
  uci_decoder::configuration decoder_config;
  decoder_config.modulation = mod_scheme;

  // Decode UCI payload.
  result.message.set_status(
      decode_uci_payload(*decoder, result.message, llr, config.max_code_rate, decoder_config, config.csi_part2_size));

  // Expected UCI payload length in number of bits.
  unsigned expected_nof_uci_bits =
      config.nof_harq_ack + config.nof_sr + config.nof_csi_part1 + result.message.get_expected_nof_csi_part2_bits();

  // Assert that the decoded UCI payload has the expected number of bits.
  ocudu_assert(result.message.get_full_payload().size() == expected_nof_uci_bits,
               "Decoded UCI payload length, i.e., {}, does not match expected number of UCI bits, i.e., {}.",
               result.message.get_full_payload().size(),
               expected_nof_uci_bits);

  return result;
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format0_configuration& config) const
{
  // BWP PRB shall not exceed the maximum.
  if (config.bwp_start_rb + config.bwp_size_rb > MAX_NOF_PRBS) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    MAX_NOF_PRBS));
  }

  // PRB allocation goes beyond the BWP. Recall that PUCCH Format 0 occupies a single PRB.
  if (config.starting_prb >= config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.starting_prb + 1,
                                       config.bwp_size_rb));
  }

  // Second hop PRB allocation goes beyond the BWP.
  if (config.second_hop_prb.has_value()) {
    if (config.second_hop_prb >= config.bwp_size_rb) {
      return make_unexpected(
          fmt::format("Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                      *config.second_hop_prb + 1,
                      config.bwp_size_rb));
    }
  }

  // The number of symbols shall be in the range.
  static constexpr auto nof_symbols_range =
      interval<unsigned, true>(pucch_constants::f0::MIN_NOF_SYMS, pucch_constants::f0::MAX_NOF_SYMS);
  if (!nof_symbols_range.contains(config.nof_symbols)) {
    return make_unexpected(
        fmt::format("Number of symbols (i.e., {}) is out of the range {}.", config.nof_symbols, nof_symbols_range));
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }

  // Initial cyclic shift must be in range.
  static constexpr auto ics_range = interval<unsigned, false>(0, pucch_constants::f0::NOF_ICS);
  if (!ics_range.contains(config.initial_cyclic_shift)) {
    return make_unexpected(fmt::format(
        "The initial cyclic shift (i.e., {}) is out of the range {}.", config.initial_cyclic_shift, ics_range));
  }

  // Hopping identifier must be in range.
  if (!pucch_constants::N_ID.contains(config.n_id)) {
    return make_unexpected(fmt::format(
        "The sequence hopping identifier (i.e., {}) is out of the range {}.", config.n_id, pucch_constants::N_ID));
  }

  // No payload detected.
  if ((config.nof_harq_ack == 0) && !config.sr_opportunity) {
    return make_unexpected(fmt::format("No payload."));
  }

  // Number of HARQ-ACK exceeds maximum.
  static constexpr auto nof_harq_ack_range =
      interval<unsigned, true>(pucch_constants::f0::MIN_NOF_HARQ_ACK_BITS, pucch_constants::f0::MAX_NOF_HARQ_ACK_BITS);
  if (!nof_harq_ack_range.contains(config.nof_harq_ack)) {
    return make_unexpected(fmt::format(
        "The number of HARQ-ACK bits (i.e., {}) is out of the range {}.", config.nof_harq_ack, nof_harq_ack_range));
  }

  // The number of receive ports must not be empty.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format1_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    ce_dims.nof_prb));
  }

  // PRB allocation goes beyond the BWP. Recall that PUCCH Format 1 occupies a single PRB.
  if (config.starting_prb >= config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.starting_prb + 1,
                                       config.bwp_size_rb));
  }

  // Second hop PRB allocation goes beyond the BWP.
  if (config.second_hop_prb.has_value()) {
    if (config.second_hop_prb >= config.bwp_size_rb) {
      return make_unexpected(
          fmt::format("Second hop PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                      *config.second_hop_prb + 1,
                      config.bwp_size_rb));
    }
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }
  if (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols) {
    return make_unexpected(fmt::format("OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum "
                                       "number of slot symbols, i.e., {}.",
                                       config.start_symbol_index + config.nof_symbols,
                                       ce_dims.nof_symbols));
  }

  // Number of HARQ-ACK exceeds maximum.
  static constexpr auto nof_harq_ack_range =
      interval<unsigned, true>(pucch_constants::f1::MIN_NOF_HARQ_ACK_BITS, pucch_constants::f1::MAX_NOF_HARQ_ACK_BITS);
  if (!nof_harq_ack_range.contains(config.nof_harq_ack)) {
    return make_unexpected(fmt::format(
        "The number of HARQ-ACK bits (i.e., {}) is out of the range {}.", config.nof_harq_ack, nof_harq_ack_range));
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }
  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(fmt::format(
        "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
        config.ports.size(),
        ce_dims.nof_rx_ports));
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format2_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    ce_dims.nof_prb));
  }

  // None of the occupied PRB within the BWP can exceed the BWP dimensions.
  if (config.prbs.stop() > config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.prbs.stop(),
                                       config.bwp_size_rb));
  }

  // If needed, check the PRB allocation for the second hop.
  if (config.second_hop_prb.has_value() && (*config.second_hop_prb + config.prbs.length() > config.bwp_size_rb)) {
    return make_unexpected(
        fmt::format("PRB allocation within the BWP goes up to PRB {} in the second hop, exceeding BWP size, i.e., {}.",
                    *config.second_hop_prb + config.prbs.length(),
                    config.bwp_size_rb));
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }

  if (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols) {
    return make_unexpected(fmt::format("OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum "
                                       "number of slot symbols, i.e., {}.",
                                       config.start_symbol_index + config.nof_symbols,
                                       ce_dims.nof_symbols));
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }

  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(fmt::format(
        "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
        config.ports.size(),
        ce_dims.nof_rx_ports));
  }

  // Get the maximum number of CSI Part 2 bits.
  unsigned max_nof_csi_part2 = get_max_nof_csi_part2_bits(config.csi_part2_size);

  // Count the total number of payload bits for UCI Part 1.
  unsigned nof_uci_part1_bits = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1;

  // The UCI Part 1 payload must not be empty.
  if (nof_uci_part1_bits == 0) {
    return make_unexpected("The UCI Part 1 payload must not be empty.");
  }

  // Check that the CSI Part 2 size description is valid for the given number of bits for CSI Part 1.
  if (!config.csi_part2_size.is_valid(config.nof_csi_part1)) {
    return make_unexpected("CSI Part 2 size description does not match CSI Part 1 payload size.");
  }

  // The maximum code rate shall not exceed the format maximum.
  if (config.max_code_rate > pucch_constants::f2::MAX_CODE_RATE) {
    return make_unexpected(fmt::format("The maximum code rate (i.e., {}) exceeds the format maximum (i.e., {}).",
                                       config.max_code_rate,
                                       static_cast<float>(pucch_constants::f2::MAX_CODE_RATE)));
  }

  // Calculate effective code rate.
  float effective_code_rate = pucch_format2_code_rate(config.prbs.length(), config.nof_symbols, nof_uci_part1_bits);

  // The code rate shall not exceed the maximum.
  if (effective_code_rate > config.max_code_rate) {
    return make_unexpected(fmt::format("The effective code rate (i.e., {}) exceeds the maximum allowed {}.",
                                       effective_code_rate,
                                       config.max_code_rate));
  }

  // UCI payload exceeds the UCI payload size boundaries.
  static constexpr auto nof_uci_bits_range =
      interval<unsigned, true>(pucch_constants::f2::MIN_NOF_DATA_BITS, pucch_constants::f2::MAX_NOF_DATA_BITS);
  if (!nof_uci_bits_range.contains(nof_uci_part1_bits + max_nof_csi_part2)) {
    return make_unexpected(fmt::format("UCI Payload length (i.e., {}) is outside the supported range (i.e., {}).",
                                       nof_uci_part1_bits + max_nof_csi_part2,
                                       nof_uci_bits_range));
  }

  // If there are any CSI Part 2 bits, check that there are rate matching output bits remaining for UCI Part 2.
  if (max_nof_csi_part2 > 0) {
    // Calculate the total number of rate matching output bits.
    unsigned e_tot = get_pucch_format2_E_total(config.prbs.length(), config.nof_symbols);
    // Calculate the number of rate matching output bits for UCI Part 1.
    unsigned e_uci_part1 = get_nof_uci_part1_softbits(
        nof_uci_part1_bits, e_tot, config.max_code_rate, get_bits_per_symbol(modulation_scheme::QPSK));
    if (e_tot == e_uci_part1) {
      return make_unexpected("There are no rate matching output bits remaining for UCI Part 2.");
    }
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format3_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    ce_dims.nof_prb));
  }

  // None of the occupied PRB within the BWP can exceed the BWP dimensions.
  if (config.prbs.stop() > config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.prbs.stop(),
                                       config.bwp_size_rb));
  }

  // If needed, check the PRB allocation for the second hop.
  if (config.second_hop_prb.has_value() && (*config.second_hop_prb + config.prbs.length() > config.bwp_size_rb)) {
    return make_unexpected(
        fmt::format("PRB allocation within the BWP goes up to PRB {} in the second hop, exceeding BWP size, i.e., {}.",
                    *config.second_hop_prb + config.prbs.length(),
                    config.bwp_size_rb));
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }

  if (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols) {
    return make_unexpected(fmt::format("OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum "
                                       "number of slot symbols, i.e., {}.",
                                       config.start_symbol_index + config.nof_symbols,
                                       ce_dims.nof_symbols));
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }

  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(fmt::format(
        "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
        config.ports.size(),
        ce_dims.nof_rx_ports));
  }

  // Get the maximum number of CSI Part 2 bits.
  unsigned max_nof_csi_part2 = get_max_nof_csi_part2_bits(config.csi_part2_size);

  // Count the total number of payload bits for UCI Part 1.
  unsigned nof_uci_part1_bits = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1;

  // The UCI Part 1 payload must not be empty.
  if (nof_uci_part1_bits == 0) {
    return make_unexpected("The UCI Part 1 payload must not be empty.");
  }

  // Check that the CSI Part 2 size description is valid for the given number of bits for CSI Part 1.
  if (!config.csi_part2_size.is_valid(config.nof_csi_part1)) {
    return make_unexpected("CSI Part 2 size description does not match CSI Part 1 payload size.");
  }

  // The maximum code rate shall not exceed the format maximum.
  if (config.max_code_rate > pucch_constants::f3::MAX_CODE_RATE) {
    return make_unexpected(fmt::format("The maximum code rate (i.e., {}) exceeds the format maximum (i.e., {}).",
                                       config.max_code_rate,
                                       static_cast<float>(pucch_constants::f3::MAX_CODE_RATE)));
  }

  // Calculate effective code rate.
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  float effective_code_rate = pucch_format3_code_rate(
      config.prbs.length(), config.nof_symbols - dmrs_symb_mask.count(), config.pi2_bpsk, nof_uci_part1_bits);

  // The code rate shall not exceed the maximum.
  if (effective_code_rate > config.max_code_rate) {
    return make_unexpected(fmt::format("The effective code rate (i.e., {}) exceeds the maximum allowed {}.",
                                       effective_code_rate,
                                       config.max_code_rate));
  }

  // UCI payload exceeds the UCI payload size boundaries.
  static constexpr interval<unsigned, true> nof_uci_bits_range(pucch_constants::f3::MIN_NOF_DATA_BITS,
                                                               pucch_constants::f3::MAX_NOF_DATA_BITS);
  if (!nof_uci_bits_range.contains(nof_uci_part1_bits + max_nof_csi_part2)) {
    return make_unexpected(fmt::format("UCI Payload length (i.e., {}) is outside the supported range (i.e., {}).",
                                       nof_uci_part1_bits + max_nof_csi_part2,
                                       nof_uci_bits_range));
  }

  // The number of allocated PRBs is outside the allowed range.
  static constexpr interval<unsigned, true> nof_prb_range(1, 16);
  if (!nof_prb_range.contains(config.prbs.length())) {
    return make_unexpected(
        fmt::format("Number of PRBs (i.e., {}) is outside the allowed range for PUCCH Format 3 (i.e., {}).",
                    config.prbs.length(),
                    nof_prb_range));
  }

  // If there are any CSI Part 2 bits, check that there are rate matching output bits remaining for UCI Part 2.
  if (max_nof_csi_part2 > 0) {
    const modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;
    // Calculate the total number of rate matching output bits.
    unsigned e_tot =
        get_pucch_format3_E_total(config.prbs.length(), config.nof_symbols - dmrs_symb_mask.count(), config.pi2_bpsk);
    // Calculate the number of rate matching output bits for UCI Part 1.
    unsigned e_uci_part1 =
        get_nof_uci_part1_softbits(nof_uci_part1_bits, e_tot, config.max_code_rate, get_bits_per_symbol(mod_scheme));
    if (e_tot == e_uci_part1) {
      return make_unexpected("There are no rate matching output bits remaining for UCI Part 2.");
    }
  }

  return default_success_t();
}

error_type<std::string> pucch_pdu_validator_impl::is_valid(const pucch_processor::format4_configuration& config) const
{
  // The BWP size exceeds the grid dimensions.
  if ((config.bwp_start_rb + config.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(
        fmt::format("BWP allocation goes up to PRB {}, exceeding the configured maximum grid RB size, i.e., {}.",
                    config.bwp_start_rb + config.bwp_size_rb,
                    ce_dims.nof_prb));
  }

  // None of the occupied PRB within the BWP can exceed the BWP dimensions.
  if (config.starting_prb + 1 > config.bwp_size_rb) {
    return make_unexpected(fmt::format("PRB allocation within the BWP goes up to PRB {}, exceeding BWP size, i.e., {}.",
                                       config.starting_prb + 1,
                                       config.bwp_size_rb));
  }

  // None of the occupied symbols can exceed the configured maximum slot size, or the slot size given by the CP.
  if (config.start_symbol_index + config.nof_symbols > get_nsymb_per_slot(config.cp)) {
    return make_unexpected(fmt::format(
        "OFDM symbol allocation goes up to symbol {}, exceeding the number of symbols in the given slot with "
        "{} CP, i.e., {}.",
        config.start_symbol_index + config.nof_symbols,
        config.cp.to_string(),
        get_nsymb_per_slot(config.cp)));
  }

  if (config.start_symbol_index + config.nof_symbols > ce_dims.nof_symbols) {
    return make_unexpected(fmt::format("OFDM symbol allocation goes up to symbol {}, exceeding the configured maximum "
                                       "number of slot symbols, i.e., {}.",
                                       config.start_symbol_index + config.nof_symbols,
                                       ce_dims.nof_symbols));
  }

  // The number of receive ports is either zero or exceeds the configured maximum number of receive ports.
  if (config.ports.empty()) {
    return make_unexpected(fmt::format("The number of receive ports cannot be zero."));
  }

  if (config.ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(fmt::format(
        "The number of receive ports, i.e. {}, exceeds the configured maximum number of receive ports, i.e., {}.",
        config.ports.size(),
        ce_dims.nof_rx_ports));
  }

  // Get the maximum number of CSI Part 2 bits.
  unsigned max_nof_csi_part2 = get_max_nof_csi_part2_bits(config.csi_part2_size);

  // Count the total number of payload bits for UCI Part 1.
  unsigned nof_uci_part1_bits = config.nof_harq_ack + config.nof_sr + config.nof_csi_part1;

  // The UCI Part 1 payload must not be empty.
  if (nof_uci_part1_bits == 0) {
    return make_unexpected("The UCI Part 1 payload must not be empty.");
  }

  // Check that the CSI Part 2 size description is valid for the given number of bits for CSI Part 1.
  if (!config.csi_part2_size.is_valid(config.nof_csi_part1)) {
    return make_unexpected("CSI Part 2 size description does not match CSI Part 1 payload size.");
  }

  // The maximum code rate shall not exceed the format maximum.
  if (config.max_code_rate > pucch_constants::f4::MAX_CODE_RATE) {
    return make_unexpected(fmt::format("The maximum code rate (i.e., {}) exceeds the format maximum (i.e., {}).",
                                       config.max_code_rate,
                                       static_cast<float>(pucch_constants::f4::MAX_CODE_RATE)));
  }

  // Calculate effective code rate.
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  float effective_code_rate = pucch_format4_code_rate(
      config.occ_length, config.nof_symbols - dmrs_symb_mask.count(), config.pi2_bpsk, nof_uci_part1_bits);

  // The code rate shall not exceed the maximum.
  if (effective_code_rate > config.max_code_rate) {
    return make_unexpected(fmt::format("The effective code rate (i.e., {}) exceeds the maximum allowed {}.",
                                       effective_code_rate,
                                       config.max_code_rate));
  }

  // UCI payload exceeds the UCI payload size boundaries.
  static constexpr interval<unsigned, true> nof_uci_bits_range(pucch_constants::f4::MIN_NOF_DATA_BITS,
                                                               pucch_constants::f4::MAX_NOF_DATA_BITS);
  if (!nof_uci_bits_range.contains(nof_uci_part1_bits + max_nof_csi_part2)) {
    return make_unexpected(fmt::format("UCI Payload length (i.e., {}) is outside the supported range (i.e., {}).",
                                       nof_uci_part1_bits + max_nof_csi_part2,
                                       nof_uci_bits_range));
  }

  // The OCC length is invalid.
  if ((config.occ_length != 2) && (config.occ_length != 4)) {
    return make_unexpected(
        fmt::format("Invalid OCC length value (i.e., {}). Valid values are 2 and 4.", config.occ_length));
  }

  // If there are any CSI Part 2 bits, check that there are rate matching output bits remaining for UCI Part 2.
  if (max_nof_csi_part2 > 0) {
    const modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;
    // Calculate the total number of rate matching output bits.
    unsigned e_tot =
        get_pucch_format4_E_total(config.occ_length, config.nof_symbols - dmrs_symb_mask.count(), config.pi2_bpsk);
    // Calculate the number of rate matching output bits for UCI Part 1.
    unsigned e_uci_part1 =
        get_nof_uci_part1_softbits(nof_uci_part1_bits, e_tot, config.max_code_rate, get_bits_per_symbol(mod_scheme));
    if (e_tot == e_uci_part1) {
      return make_unexpected("There are no rate matching output bits remaining for UCI Part 2.");
    }
  }

  return default_success_t();
}
