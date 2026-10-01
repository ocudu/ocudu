# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_dmrs_helper STATIC dmrs_helper.cpp)

add_library(ocudu_channel_estimator STATIC
    channel_estimator/factories.cpp
    channel_estimator/port_channel_estimator_average_impl.cpp
    channel_estimator/port_channel_estimator_helpers.cpp
)
target_link_libraries(ocudu_channel_estimator ocuduvec ocudu_phy_support)

add_library(ocudu_nzp_csi_rs_generator STATIC
    nzp_csi_rs/factories.cpp
    nzp_csi_rs/nzp_csi_rs_generator_impl.cpp
)
target_link_libraries(ocudu_nzp_csi_rs_generator ocudu_ran)

add_library(ocudu_dmrs_pdcch STATIC
    pdcch/factories.cpp
    pdcch/dmrs_pdcch_processor_impl.cpp
)
target_link_libraries(ocudu_dmrs_pdcch ocudu_dmrs_helper)

add_library(ocudu_dmrs_pdsch STATIC
    pdsch/factories.cpp
    pdsch/dmrs_pdsch_processor_impl.cpp
)
target_link_libraries(ocudu_dmrs_pdsch ocudu_dmrs_helper)

add_library(ocudu_dmrs_pucch STATIC
    pucch/factories.cpp
    pucch/dmrs_pucch_estimator_format2.cpp
    pucch/dmrs_pucch_estimator_formats3_4.cpp
)

add_library(ocudu_dmrs_pusch STATIC
    pusch/factories.cpp
    pusch/dmrs_pusch_estimator_impl.cpp
)
target_link_libraries(ocudu_dmrs_pusch ocudu_dmrs_helper)

add_library(ocudu_prs_generator STATIC
    prs/factories.cpp
    prs/prs_generator_impl.cpp
)
target_link_libraries(ocudu_prs_generator ocudu_ran)

add_library(ocudu_ptrs_generator STATIC
    ptrs/ptrs_pdsch_generator_factory.cpp
    ptrs/ptrs_pdsch_generator_impl.cpp
)

add_library(ocudu_srs_estimator STATIC
    srs/srs_estimator_factory.cpp
    srs/srs_estimator_generic_impl.cpp
    srs/srs_validator_generic_impl.cpp
)
target_link_libraries(ocudu_srs_estimator ocudu_ran)

add_library(ocudu_ssb_generators STATIC
    ssb/factories.cpp
    ssb/dmrs_pbch_processor_impl.cpp
    ssb/pss_processor_impl.cpp
    ssb/sss_processor_impl.cpp
)
target_link_libraries(ocudu_ssb_generators ocudu_dft)

add_library(ocudu_signal_processors INTERFACE)
target_link_libraries(ocudu_signal_processors INTERFACE
    ocudu_dmrs_helper
    ocudu_channel_estimator
    ocudu_nzp_csi_rs_generator
    ocudu_dmrs_pdcch
    ocudu_dmrs_pdsch
    ocudu_dmrs_pucch
    ocudu_dmrs_pusch
    ocudu_prs_generator
    ocudu_ptrs_generator
    ocudu_srs_estimator
    ocudu_ssb_generators
)

add_to_exported_libs(
    ocudu_dmrs_helper
    ocudu_signal_processors
    ocudu_channel_estimator
    ocudu_nzp_csi_rs_generator
    ocudu_dmrs_pdcch
    ocudu_dmrs_pdsch
    ocudu_dmrs_pucch
    ocudu_dmrs_pusch
    ocudu_prs_generator
    ocudu_ptrs_generator
    ocudu_srs_estimator
    ocudu_ssb_generators
)

if (TARGET Eigen3::Eigen)
    add_library(ocudu_doa_estimator STATIC srs/doa_estimator_eigen.cpp srs/doa_estimator_eigen_factory.cpp)
    target_link_libraries(ocudu_signal_processors INTERFACE ocudu_doa_estimator)
    add_to_exported_libs(ocudu_doa_estimator)
else (TARGET Eigen3::Eigen)
    add_library(doa_estimator_dummy STATIC srs/doa_estimator_dummy_factory.cpp)
    target_link_libraries(ocudu_signal_processors INTERFACE doa_estimator_dummy)
    add_to_exported_libs(doa_estimator_dummy)
endif (TARGET Eigen3::Eigen)
