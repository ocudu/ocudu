// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/asn1_codec.h"
#include "ocudu/asn1/e2sm_ccc/e2sm_ccc.h"
#include <algorithm>
#include <cstddef>
#include <gtest/gtest.h>
#include <set>
#include <string>
#include <vector>

using namespace asn1;
using namespace asn1::e2sm_ccc;

namespace {

/// Number of objects and of enumerated types that the schema of the specification holds.
constexpr size_t NOF_SCHEMA_OBJECTS    = 103;
constexpr size_t NOF_SCHEMA_ENUMERATED = 26;

/// Member names that a SEQUENCE writes, as per O-RAN.WG3.TS.E2SM-CCC clause 9.4.2 of v08.00.
struct sequence_names {
  const char*                  schema;
  const asn1::table::seq_desc* table;
  std::vector<std::string>     names;
};

const std::vector<sequence_names> expected_sequences = {
    {"AsmCapabilityInfo",
     &asm_cap_info_s::desc,
     {"definedDurationSleepSupported", "sleepModes", "undefinedDurationSleepSupported"}},
    {"AttributeName", &attribute_name_s::desc, {"attributeName"}},
    {"Attribute", &attribute_s::desc, {"attributeName", "supportedServices"}},
    {"CellConfigurationToBeReportedForADF",
     &cell_cfg_to_be_reported_for_adf_s::desc,
     {"cellGlobalId", "listOfCellLevelRANConfigurationStructuresForADF"}},
    {"CellControlOutcome",
     &cell_ctrl_outcome_s::desc,
     {"cellGlobalId", "ranConfigurationStructuresAcceptedList", "ranConfigurationStructuresFailedList"}},
    {"CellControlled", &cell_ctrl_s::desc, {"cellGlobalId", "listOfConfigurationStructures"}},
    {"CellForRANFunctionDefinition",
     &cell_for_ran_function_definition_s::desc,
     {"cellGlobalID", "listOfSupportedCellLevelRANConfigurationStructures"}},
    {"CellLevelConfigurationStructureForEventTrigger",
     &cell_level_cfg_structure_for_event_trigger_s::desc,
     {"cellGlobalId", "listOfRANConfigurationStructuresForEventTrigger"}},
    {"CellReported", &cell_reported_s::desc, {"cellGlobalId", "listOfConfigurationStructuresReported"}},
    {"CelllevelConfigurationStructuresForQuery",
     &celllevel_cfg_structures_for_query_s::desc,
     {"listOfAttributes", "ranConfigurationStructureName"}},
    {"CellsToBeQueried",
     &cells_to_be_queried_s::desc,
     {"cellGlobalId", "listOfCelllevelConfigurationStructuresForQuery"}},
    {"CellsToBeReportedForQuery",
     &cells_to_be_reported_for_query_s::desc,
     {"cellGlobalId", "listOfConfigurationStructuresToBeReportedForQuery"}},
    {"ConfigurationStructureAccepted",
     &cfg_structure_accepted_s::desc,
     {"appliedTimestamp", "currentValuesOfAttributes", "oldValuesOfAttributes", "ranConfigurationStructureName"}},
    {"ConfigurationStructureFailed",
     &cfg_structure_failed_s::desc,
     {"cause", "oldValuesOfAttributes", "ranConfigurationStructureName", "requestedValuesOfAttributes"}},
    {"ConfigurationStructure",
     &cfg_structure_s::desc,
     {"changeType", "oldValuesOfAttributes", "ranConfigurationStructureName", "valuesOfAttributes"}},
    {"ConfigurationStructureWrite",
     &cfg_structure_write_s::desc,
     {"newValuesOfAttributes", "oldValuesOfAttributes", "ranConfigurationStructureName"}},
    {"ConfigurationStructuresToBeReportedForQuery",
     &cfg_structures_to_be_reported_for_query_s::desc,
     {"ranConfigurationStructureName", "valuesOfAttributes"}},
    {"ControlService", &ctrl_service_s::desc, {"listOfSupportedControlStyles"}},
    {"ControlStyle",
     &ctrl_style_s::desc,
     {"controlServiceControlOutcomeFormatType",
      "controlServiceHeaderFormatType",
      "controlServiceMessageFormatType",
      "controlServiceStyleName",
      "controlServiceStyleType",
      "ricCallProcessIDFormatType"}},
    {"CycleStartOffset", &cycle_start_offset_s::desc, {"offset", "periodicity"}},
    {"DeepHibernateCapabilityInfo", &deep_hibernate_cap_info_s::desc, {"maxHibernateDuration", "minHibernateDuration"}},
    {"E2SM-CCC-ActionDefinitionFormat1",
     &e2sm_ccc_action_definition_format1_s::desc,
     {"listOfNodeLevelRANConfigurationStructuresForADF"}},
    {"E2SM-CCC-ActionDefinitionFormat2",
     &e2sm_ccc_action_definition_format2_s::desc,
     {"listOfCellConfigurationsToBeReportedForADF"}},
    {"E2SM-CCC-ControlHeaderFormat1", &e2sm_ccc_ctrl_hdr_format1_s::desc, {"ricStyleType"}},
    {"E2SM-CCC-ControlMessageFormat1", &e2sm_ccc_ctrl_msg_format1_s::desc, {"listOfConfigurationStructures"}},
    {"E2SM-CCC-ControlMessageFormat2", &e2sm_ccc_ctrl_msg_format2_s::desc, {"listOfCellsControlled"}},
    {"E2SM-CCC-ControlOutcomeFormat1",
     &e2sm_ccc_ctrl_outcome_format1_s::desc,
     {"ranConfigurationStructuresAcceptedList", "ranConfigurationStructuresFailedList", "receivedTimestamp"}},
    {"E2SM-CCC-ControlOutcomeFormat2",
     &e2sm_ccc_ctrl_outcome_format2_s::desc,
     {"listOfCellsForControlOutcome", "receivedTimestamp"}},
    {"E2SM-CCC-EventTriggerDefinition-Format1",
     &e2sm_ccc_event_trigger_definition_format1_s::desc,
     {"listOfNodeLevelConfigurationStructuresForEventTrigger"}},
    {"E2SM-CCC-EventTriggerDefinition-Format2",
     &e2sm_ccc_event_trigger_definition_format2_s::desc,
     {"listOfCellLevelConfigurationStructuresForEventTrigger"}},
    {"E2SM-CCC-EventTriggerDefinition-Format3", &e2sm_ccc_event_trigger_definition_format3_s::desc, {"period"}},
    {"E2SM-CCC-IndicationHeaderFormat1", &e2sm_ccc_ind_hdr_format1_s::desc, {"eventTime", "indicationReason"}},
    {"E2SM-CCC-IndicationMessageFormat1", &e2sm_ccc_ind_msg_format1_s::desc, {"listOfConfigurationStructuresReported"}},
    {"E2SM-CCC-IndicationMessageFormat2", &e2sm_ccc_ind_msg_format2_s::desc, {"listOfCellsReported"}},
    {"E2SM-CCC-QueryDefinitionFormat1",
     &e2sm_ccc_query_definition_format1_s::desc,
     {"listOfNodelevelRanConfigurationStructuresForQuery"}},
    {"E2SM-CCC-QueryDefinitionFormat2", &e2sm_ccc_query_definition_format2_s::desc, {"listOfCellsToBeQueriedForQuery"}},
    {"E2SM-CCC-QueryHeaderFormat1", &e2sm_ccc_query_hdr_format1_s::desc, {"ricStyleType"}},
    {"E2SM-CCC-QueryOutcomeFormat1",
     &e2sm_ccc_query_outcome_format1_s::desc,
     {"listOfConfigurationStructuresToBeReportedForQuery"}},
    {"E2SM-CCC-QueryOutcomeFormat2", &e2sm_ccc_query_outcome_format2_s::desc, {"listOfCellsToBeReportedForQuery"}},
    {"EnergySavingCapabilityCommonInfo",
     &energy_saving_cap_common_info_s::desc,
     {"ST8-ready-message-supported", "emergency-wake-up-command-supported", "sleep-duration-extension-supported"}},
    {"EsObjective|alt1", &es_objive_c::es_percentage_s_::desc, {"esPercentage"}},
    {"EsObjective|alt0", &es_objive_c::target_ec_s_::desc, {"targetEc"}},
    {"EUTRA-CGI", &eutra_cgi_s::desc, {"eUTRACellIdentity", "plmnIdentity"}},
    {"EventTrigger", &event_trigger_s::desc, {"listOfSupportedEventTriggerStyles"}},
    {"EventTriggerStyle",
     &event_trigger_style_s::desc,
     {"eventTriggerFormatType", "eventTriggerStyleName", "eventTriggerStyleType"}},
    {"EventTriggerStyleType", &event_trigger_style_type_s::desc, {"eventTriggerStyleType"}},
    {"NodelevelRanConfigurationStructuresForQuery",
     &nodelevel_ran_cfg_structures_for_query_s::desc,
     {"listOfAttributes", "ranConfigurationStructureName"}},
    {"NR-CGI", &nr_cgi_s::desc, {"nRCellIdentity", "plmnIdentity"}},
    {"O-Bwp",
     &o_bwp_s::desc,
     {"bwpContext", "cyclicPrefix", "isInitialBwp", "numberOfRBs", "startRB", "subCarrierSpacing"}},
    {"O-CellDTXDRXConfig",
     &o_cell_dtxdrx_cfg_s::desc,
     {"activationStatus", "configType", "cycleStartOffset", "l1Activation", "onDurationTimer", "slotOffset"}},
    {"O-CESManagementFunction",
     &o_ces_management_function_s::desc,
     {"cesSwitch", "energySavingControl", "energySavingState"}},
    {"O-GnbCuCpFunction",
     &o_gnb_cu_cp_function_s::desc,
     {"gnbCuName",
      "gnbId",
      "gnbIdLength",
      "plmnId",
      "x2AllowList",
      "x2BlockList",
      "x2HOBlockList",
      "xnAllowList",
      "xnBlockList",
      "xnHOBlockList"}},
    {"O-GnbCuUpFunction", &o_gnb_cu_up_function_s::desc, {"gnbCuUpId", "gnbId", "gnbIdLength", "plmnInfoList"}},
    {"O-GnbDuFunction", &o_gnb_du_function_s::desc, {"gnbDuId", "gnbDuName", "gnbId", "gnbIdLength"}},
    {"O-NESPolicy",
     &o_nes_policy_s::desc,
     {"antennaMask",
      "antennaMaskName",
      "dataDir",
      "esObjective",
      "hibernateDuration",
      "perfObjectiveList",
      "policyType",
      "sleepMode",
      "slotMask",
      "symbolMask"}},
    {"O-NrCellCu", &o_nr_cell_cu_s::desc, {"cellLocalId", "plmnInfoList"}},
    {"O-NrCellDu",
     &o_nr_cell_du_s::desc,
     {"administrativeState", "arfcnDL",        "arfcnSUL",       "arfcnUL",
      "bSChannelBwDL",       "bSChannelBwSUL", "bSChannelBwUL",  "bwpList",
      "cellLocalId",         "cellState",      "nrPci",          "nrTac",
      "operationalState",    "partitionList",  "plmnInfoList",   "ssbDuration",
      "ssbFrequency",        "ssbOffset",      "ssbPeriodicity", "ssbSubCarrierSpacing"}},
    {"O-PRBBlankingPolicy",
     &o_prb_blanking_policy_s::desc,
     {"endTime", "linkDir", "policyCategory", "prbBlankingConfigurationList", "prbBlankingObj", "startTime"}},
    {"O-RRMPolicyRatio",
     &o_rrm_policy_ratio_s::desc,
     {"rRMPolicyDedicatedRatio", "rRMPolicyMaxRatio", "rRMPolicyMemberList", "rRMPolicyMinRatio", "resourceType"}},
    {"O-RUInfo", &o_ru_info_s::desc, {"oruCapabilities", "oruUserPlaneConfiguration"}},
    {"OruCapabilities",
     &oru_cap_s::desc,
     {"asmCapabilityInfo",
      "deepHibernateCapabilityInfo",
      "energySavingCapabilityCommonInfo",
      "trxControlCapabilityInfo"}},
    {"OruUserPlaneConfiguration", &oru_user_plane_cfg_s::desc, {"rxArrayList", "txArrayList"}},
    {"PartitionFlowList[]", &partition_flow_list_item_s_::desc, {"5qiList", "plmnId", "snssai"}},
    {"PartitionList[]", &partition_list_item_s_::desc, {"pNumberOfRBs", "pOffsetToPointA", "partitionFlowList"}},
    {"PerfObjective",
     &perf_objive_s::desc,
     {"fiveQIValue", "flowBrAvgWindow", "maxNgbrFlowBr", "maxPd", "plmnInfoList", "targetPd"}},
    {"PlmnId", &plmn_id_s::desc, {"mcc", "mnc"}},
    {"PlmnInfo", &plmn_info_s::desc, {"plmnId", "snssai"}},
    {"PrbBlankingConfigItem", &prb_blanking_cfg_item_s::desc, {"bwpIndex", "frameNumber", "prbIndex", "slotNumber"}},
    {"QueryService", &query_service_s::desc, {"listOfSupportedQueryStyles"}},
    {"QueryStyle",
     &query_style_s::desc,
     {"queryServiceDefinitionFormatType",
      "queryServiceHeaderFormatType",
      "queryServiceOutcomeFormatType",
      "queryServiceStyleName",
      "queryServiceStyleType"}},
    {"rPolarizationItem", &r_polarization_item_s::desc, {"rPolarizationIndex", "rPolarizationType"}},
    {"RANConfigurationStructureForADF",
     &ran_cfg_structure_for_adf_s::desc,
     {"listOfAttributes", "ranConfigurationStructureName", "reportType"}},
    {"RANConfigurationStructureForEventTrigger",
     &ran_cfg_structure_for_event_trigger_s::desc,
     {"listOfAttributes", "ranConfigurationStructureName"}},
    {"RANConfigurationStructure",
     &ran_cfg_structure_s::desc,
     {"listOfSupportedAttributes", "ranConfigurationStructureName"}},
    {"RANFunctionDefinition",
     &ran_function_definition_s::desc,
     {"listOfCellsForRANFunctionDefinition", "listOfSupportedNodeLevelConfigurationStructures", "ranFunctionName"}},
    {"RANFunctionName",
     &ran_function_name_s::desc,
     {"ranFunctionDescription", "ranFunctionInstance", "ranFunctionServiceModelOID", "ranFunctionShortName"}},
    {"ReportService", &report_service_s::desc, {"listOfSupportedReportStyles"}},
    {"ReportStyle",
     &report_style_s::desc,
     {"listOfSupportedEventTriggerStylesForReportStyle",
      "reportServiceActionDefinitionFormatType",
      "reportServiceIndicationHeaderFormatType",
      "reportServiceIndicationMessageFormatType",
      "reportServiceStyleName",
      "reportServiceStyleType"}},
    {"RICActionDefinition", &ric_action_definition_s::desc, {"actionDefinitionFormat", "ricStyleType"}},
    {"RIC-Control-Header", &ric_ctrl_hdr_s::desc, {"controlHeaderFormat"}},
    {"RIC-Control-Message", &ric_ctrl_msg_s::desc, {"controlMessageFormat"}},
    {"RIC-Control-Outcome", &ric_ctrl_outcome_s::desc, {"controlOutcomeFormat"}},
    {"RICEventTriggerDefinition", &ric_event_trigger_definition_s::desc, {"eventTriggerDefinitionFormat"}},
    {"RIC-Indication-Header", &ric_ind_hdr_s::desc, {"indicationHeaderFormat"}},
    {"RIC-Indication-Message", &ric_ind_msg_s::desc, {"indicationMessageFormat"}},
    {"RIC-Query-Definition", &ric_query_definition_s::desc, {"queryDefinitionFormat"}},
    {"RIC-Query-Header", &ric_query_hdr_s::desc, {"queryHeaderFormat"}},
    {"RIC-Query-Outcome", &ric_query_outcome_s::desc, {"queryOutcomeFormat"}},
    {"RICServices",
     &ric_services_s::desc,
     {"controlService", "eventTrigger", "insertService", "policyService", "queryService", "reportService"}},
    {"RrmPolicyMember", &rrm_policy_member_s::desc, {"plmnId", "snssai"}},
    {"RxArrayItem",
     &rx_array_item_s::desc,
     {"rBandNumber",
      "rGainCorrectionRange",
      "rHorizontalSpacing",
      "rLeftmostBottomArrayElementPosition",
      "rName",
      "rNormalVectorDirection",
      "rNumberOfArrayLayers",
      "rNumberOfColumns",
      "rNumberOfRows",
      "rPolarizationList",
      "rVerticalSpacing"}},
    {"RxArrayItem.rGainCorrectionRange", &rx_array_item_s::rgain_correction_range_s_::desc, {"rMax", "rMin"}},
    {"RxArrayItem.rLeftmostBottomArrayElementPosition",
     &rx_array_item_s::rleftmost_bottom_array_elem_position_s_::desc,
     {"rXAxis", "rYAxis", "rZAxis"}},
    {"RxArrayItem.rNormalVectorDirection",
     &rx_array_item_s::rnormal_vector_direction_s_::desc,
     {"rAzimuthAngle", "rZenithAngle"}},
    {"SleepMode", &sleep_mode_s::desc, {"sleepModeType", "wakeupDuration", "wakeupDurationGuaranteed"}},
    {"Snssai", &snssai_s::desc, {"sd", "sst"}},
    {"SupportedTrxControlMask", &supported_trx_ctrl_mask_s::desc, {"antennaMask", "maskName"}},
    {"tPolarizationItem", &t_polarization_item_s::desc, {"tPolarizationIndex", "tPolarizationType"}},
    {"TrxControlCapabilityInfo",
     &trx_ctrl_cap_info_s::desc,
     {"definedDurationSleepSupported", "sleepModes", "supportedTrxControlMasks", "undefinedDurationSleepSupported"}},
    {"TxArrayItem",
     &tx_array_item_s::desc,
     {"tBandNumber",
      "tHorizontalSpacing",
      "tIndependentPowerBudget",
      "tLeftmostBottomArrayElementPosition",
      "tMaxGain",
      "tMinGain",
      "tName",
      "tNormalVectorDirection",
      "tNumberOfArrayLayers",
      "tNumberOfColumns",
      "tNumberOfRows",
      "tPolarizationList",
      "tVerticalSpacing"}},
    {"TxArrayItem.tLeftmostBottomArrayElementPosition",
     &tx_array_item_s::tleftmost_bottom_array_elem_position_s_::desc,
     {"tXAxis", "tYAxis", "tZAxis"}},
    {"TxArrayItem.tNormalVectorDirection",
     &tx_array_item_s::tnormal_vector_direction_s_::desc,
     {"tAzimuthAngle", "tZenithAngle"}},
    {"ValuesOfAttributes", &values_of_attributes_s::desc, {"ranConfigurationStructure"}},
};

/// Values that an ENUMERATED writes, as per O-RAN.WG3.TS.E2SM-CCC clause 9.4.2 of v08.00.
struct enumerated_values {
  const char*                   schema;
  const asn1::table::enum_desc* table;
  std::vector<std::string>      values;
};

const std::vector<enumerated_values> expected_enumerated = {
    {"AdministrativeState", &administrative_state_opts::desc, {"LOCKED", "SHUTTINGDOWN", "UNLOCKED"}},
    {"BwpContext", &bwp_context_opts::desc, {"DL", "UL", "SUL"}},
    {"Cause",
     &cause_opts::desc,
     {"NotSupported", "NotAvailable", "IncompatibleState", "JsonError", "SemanticError", "Unspecified"}},
    {"CellState", &cell_state_opts::desc, {"IDLE", "INACTIVE", "ACTIVE"}},
    {"ConfigurationStructure.changeType",
     &cfg_structure_s::change_type_opts::desc,
     {"none", "modification", "addition", "deletion"}},
    {"CyclicPrefix", &cp_opts::desc, {"15", "30", "60", "120"}},
    {"CycleStartOffset.periodicity",
     &cycle_start_offset_s::periodicity_opts::desc,
     {"ms10",  "ms20",  "ms32",  "ms40",  "ms60",   "ms64",   "ms70",   "ms80",   "ms128",  "ms160",
      "ms256", "ms320", "ms512", "ms640", "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"}},
    {"E2SM-CCC-IndicationHeaderFormat1.indicationReason",
     &e2sm_ccc_ind_hdr_format1_s::ind_reason_opts::desc,
     {"uponSubscription", "uponChange", "periodic"}},
    {"IsInitialBwp", &is_init_bwp_opts::desc, {"INITIAL", "OTHER", "SUL"}},
    {"O-CellDTXDRXConfig.activationStatus",
     &o_cell_dtxdrx_cfg_s::activation_status_opts::desc,
     {"ACTIVATED", "DEACTIVATED"}},
    {"O-CellDTXDRXConfig.configType", &o_cell_dtxdrx_cfg_s::cfg_type_opts::desc, {"DTX", "DRX", "DTXDRX"}},
    {"O-CellDTXDRXConfig.l1Activation", &o_cell_dtxdrx_cfg_s::l1_activation_opts::desc, {"ENABLED", "DISABLED"}},
    {"O-CESManagementFunction.energySavingControl",
     &o_ces_management_function_s::energy_saving_ctrl_opts::desc,
     {"toBeEnergySaving", "toBeNotEnergySaving"}},
    {"O-CESManagementFunction.energySavingState",
     &o_ces_management_function_s::energy_saving_state_opts::desc,
     {"isNotEnergySaving", "isEnergySaving"}},
    {"O-NESPolicy.dataDir", &o_nes_policy_s::data_dir_opts::desc, {"DL", "UL"}},
    {"O-NESPolicy.policyType", &o_nes_policy_s::policy_type_opts::desc, {"ASM", "TRX_CONTROL", "DEEP_HIBERNATE"}},
    {"O-NESPolicy.sleepMode",
     &o_nes_policy_s::sleep_mode_opts::desc,
     {"SLEEP_MODE0", "SLEEP_MODE1", "SLEEP_MODE2", "SLEEP_MODE3"}},
    {"O-PRBBlankingPolicy.linkDir", &o_prb_blanking_policy_s::link_dir_opts::desc, {"DL", "UL"}},
    {"O-PRBBlankingPolicy.policyCategory",
     &o_prb_blanking_policy_s::policy_category_opts::desc,
     {"prbBlankingConfiguration", "prbBlankingObjective"}},
    {"OnDurationTimer|alt1",
     &on_dur_timer_c::timeout_opts::desc,
     {"ms1",  "ms2",   "ms3",   "ms4",   "ms5",   "ms6",   "ms8",   "ms10",  "ms20",   "ms40",   "ms50",  "ms60",
      "ms80", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms800", "ms1000", "ms1200", "ms1600"}},
    {"OperationalState", &operational_state_opts::desc, {"ENABLED", "DISABLED"}},
    {"rPolarizationItem.rPolarizationType",
     &r_polarization_item_s::rpolarization_type_opts::desc,
     {"MINUS_45", "ZERO", "PLUS_45", "PLUS_90"}},
    {"RANConfigurationStructureForADF.reportType",
     &ran_cfg_structure_for_adf_s::report_type_opts::desc,
     {"all", "change"}},
    {"ResourceType", &res_type_opts::desc, {"PRB_DL", "PRB_UL", "DRB", "RRC"}},
    {"SleepMode.sleepModeType",
     &sleep_mode_s::sleep_mode_type_opts::desc,
     {"SLEEP_MODE0", "SLEEP_MODE1", "SLEEP_MODE2", "SLEEP_MODE3"}},
    {"tPolarizationItem.tPolarizationType",
     &t_polarization_item_s::tpolarization_type_opts::desc,
     {"MINUS_45", "ZERO", "PLUS_45", "PLUS_90"}},
};

} // namespace

TEST(e2sm_ccc_schema_conformance_test, every_sequence_writes_the_member_names_of_its_schema)
{
  for (const auto& expected : expected_sequences) {
    ASSERT_NE(expected.table->root.names, nullptr) << expected.schema;
    std::vector<std::string> names;
    for (uint32_t i = 0; i != expected.table->root.nof_fields; ++i) {
      names.emplace_back(expected.table->root.names[i].name);
    }
    std::sort(names.begin(), names.end());
    ASSERT_EQ(names, expected.names) << "the members of " << expected.schema << " do not match the schema";
  }
}

TEST(e2sm_ccc_schema_conformance_test, every_object_and_enumerated_of_the_schema_has_a_table)
{
  // A type that drifts from the schema no longer matches it, so it drops out of the lists above rather than
  // failing a comparison. These counts are what the schema holds. The names are counted rather than the entries,
  // because two entries that name one object of the schema leave another with no table and still count as two.
  std::set<std::string> sequence_names;
  for (const auto& expected : expected_sequences) {
    ASSERT_TRUE(sequence_names.emplace(expected.schema).second) << expected.schema << " is named by two tables";
  }
  std::set<std::string> enumerated_names;
  for (const auto& expected : expected_enumerated) {
    ASSERT_TRUE(enumerated_names.emplace(expected.schema).second) << expected.schema << " is named by two tables";
  }
  ASSERT_EQ(sequence_names.size(), NOF_SCHEMA_OBJECTS);
  ASSERT_EQ(enumerated_names.size(), NOF_SCHEMA_ENUMERATED);
}

TEST(e2sm_ccc_schema_conformance_test, every_enumerated_writes_the_values_of_its_schema)
{
  for (const auto& expected : expected_enumerated) {
    ASSERT_NE(expected.table->names, nullptr) << expected.schema;
    std::vector<std::string> values;
    for (uint32_t i = 0; i != expected.table->nof_types; ++i) {
      values.emplace_back(expected.table->names[i]);
    }
    ASSERT_EQ(values, expected.values) << "the values of " << expected.schema << " do not match the schema";
  }
}
