# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(ocudu_ran
        beamforming/beam_identifier_helpers.cpp
        beamforming/beam_weights_codebook_generator.cpp
        cause/ngap_cause_converters.cpp
        cause/f1ap_cause_converters.cpp
        cause/e1ap_cause_converters.cpp
        csi_report/csi_report_config_helpers.cpp
        csi_report/csi_report_on_pucch_helpers.cpp
        csi_report/csi_report_on_pusch_helpers.cpp
        csi_report/csi_report_on_puxch_helpers.cpp
        csi_rs/csi_rs_pattern.cpp
        csi_rs/csi_rs_config_helpers.cpp
        csi_rs/frequency_allocation_type.cpp
        pdcch/cce_to_prb_mapping.cpp
        pdcch/coreset.cpp
        pdcch/dci_packing.cpp
        pdcch/pdcch_candidates.cpp
        pdcch/pdcch_type0_css_coreset_config.cpp
        pdcch/pdcch_type0_css_occasions.cpp
        pdcch/search_space.cpp
        pdsch/dlsch_info.cpp
        pdsch/pdsch_mcs.cpp
        pdsch/pdsch_antenna_port_mapping.cpp
        prach/prach_configuration.cpp
        prach/prach_cyclic_shifts.cpp
        prach/prach_frequency_mapping.cpp
        prach/prach_preamble_information.cpp
        prach/prach_helper.cpp
        prach/prach_time_mapping.cpp
        precoding/precoding_codebook_configuration.cpp
        precoding/precoding_codebooks.cpp
        precoding/two_port/precoding_codebooks.cpp
        precoding/type1_sp/precoding_codebook_helpers.cpp
        precoding/type1_sp/precoding_codebooks.cpp
        ptrs/ptrs_pattern.cpp
        pucch/pucch_info.cpp
        pusch/pusch_antenna_port_mapping.cpp
        pusch/pusch_mcs.cpp
        pusch/pusch_tpmi_select.cpp
        pusch/pusch_uci_beta_offset.cpp
        pusch/ulsch_info.cpp
        qos/five_qi_qos_mapping.cpp
        uci/uci_part2_size_calculator.cpp
        band_helper.cpp
        sch/tbs_calculator.cpp
        resource_allocation/resource_allocation_frequency.cpp
        resource_allocation/vrb_to_prb.cpp
        sch/sch_segmentation.cpp
        sib/sib_helper.cpp
        srs/srs_bandwidth_configuration.cpp
        srs/srs_information.cpp
        ssb/pbch_mib_pack.cpp
        ssb/ssb_freq_position_generator.cpp
        ssb/ssb_gscn.cpp
        ssb/ssb_helper.cpp
        ssb/ssb_mapping.cpp
        tdd_ul_dl_config.cpp
        drx_config.cpp)
target_link_libraries(ocudu_ran ocudulog ocuduvec)

add_to_exported_libs(ocudu_ran)
