// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/dl_ccch_msg_ies.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// TimeToTrigger ::= ENUMERATED
static const char*           enum_names_time_to_trigger_e_values[] = {"ms0",
                                                                      "ms40",
                                                                      "ms64",
                                                                      "ms80",
                                                                      "ms100",
                                                                      "ms128",
                                                                      "ms160",
                                                                      "ms256",
                                                                      "ms320",
                                                                      "ms480",
                                                                      "ms512",
                                                                      "ms640",
                                                                      "ms1024",
                                                                      "ms1280",
                                                                      "ms2560",
                                                                      "ms5120"};
const asn1::table::enum_desc time_to_trigger_opts::desc            = {enum_names_time_to_trigger_e_values,
                                                                      16,
                                                                      "time_to_trigger_e"};
uint16_t                     time_to_trigger_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};
  return map_enum_number(numbers, 16, value, "time_to_trigger_e");
}

// ReportInterval ::= ENUMERATED
static const char*           enum_names_report_interv_e_values[] = {"ms120",
                                                                    "ms240",
                                                                    "ms480",
                                                                    "ms640",
                                                                    "ms1024",
                                                                    "ms2048",
                                                                    "ms5120",
                                                                    "ms10240",
                                                                    "ms20480",
                                                                    "ms40960",
                                                                    "min1",
                                                                    "min6",
                                                                    "min12",
                                                                    "min30"};
const asn1::table::enum_desc report_interv_opts::desc = {enum_names_report_interv_e_values, 14, "report_interv_e"};
uint16_t                     report_interv_opts::to_number() const
{
  static const uint16_t numbers[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 1, 6, 12, 30};
  return map_enum_number(numbers, 14, value, "report_interv_e");
}

// SL-MeasReportQuantity-r16 ::= CHOICE
static const asn1::table::seq_field choice_table_sl_meas_report_quant_r16_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::boolean, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_sl_meas_report_quant_r16_c_names[] = {{"sl-RSRP-r16", nullptr}};
const asn1::table::choice_desc       sl_meas_report_quant_r16_c::desc                = {
    asn1::table::enum_int_type<sl_meas_report_quant_r16_c::types>(),
    sl_meas_report_quant_r16_c::types::nof_types,
    sl_meas_report_quant_r16_c::types::nof_exts,
    sl_meas_report_quant_r16_c::types::has_ext,
    false,
    choice_table_sl_meas_report_quant_r16_c_alternatives,
    nullptr,
    choice_table_sl_meas_report_quant_r16_c_names,
    "sl_meas_report_quant_r16_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_meas_report_quant_r16_c, c),
    false};

static const char*           enum_names_sl_meas_report_quant_r16_c_types_values[] = {"sl-RSRP-r16"};
const asn1::table::enum_desc sl_meas_report_quant_r16_c::types_opts::desc         = {
    enum_names_sl_meas_report_quant_r16_c_types_values,
    1,
    "sl_meas_report_quant_r16_c::types"};

// SL-MeasTriggerQuantity-r16 ::= CHOICE
static const asn1::table::seq_field choice_table_sl_meas_trigger_quant_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr}};
static const asn1::table::field_name choice_table_sl_meas_trigger_quant_r16_c_names[] = {{"sl-RSRP-r16", nullptr}};
const asn1::table::choice_desc       sl_meas_trigger_quant_r16_c::desc                = {
    asn1::table::enum_int_type<sl_meas_trigger_quant_r16_c::types>(),
    sl_meas_trigger_quant_r16_c::types::nof_types,
    sl_meas_trigger_quant_r16_c::types::nof_exts,
    sl_meas_trigger_quant_r16_c::types::has_ext,
    false,
    choice_table_sl_meas_trigger_quant_r16_c_alternatives,
    nullptr,
    choice_table_sl_meas_trigger_quant_r16_c_names,
    "sl_meas_trigger_quant_r16_c",
    asn1::table::choice_desc::no_member,
    offsetof(sl_meas_trigger_quant_r16_c, c),
    false};

static const char*           enum_names_sl_meas_trigger_quant_r16_c_types_values[] = {"sl-RSRP-r16"};
const asn1::table::enum_desc sl_meas_trigger_quant_r16_c::types_opts::desc         = {
    enum_names_sl_meas_trigger_quant_r16_c_types_values,
    1,
    "sl_meas_trigger_quant_r16_c::types"};

// SN-FieldLengthAM ::= ENUMERATED
static const char*           enum_names_sn_field_len_am_e_values[] = {"size12", "size18"};
const asn1::table::enum_desc sn_field_len_am_opts::desc = {enum_names_sn_field_len_am_e_values, 2, "sn_field_len_am_e"};
uint8_t                      sn_field_len_am_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "sn_field_len_am_e");
}

// T-PollRetransmit ::= ENUMERATED
static const char* enum_names_t_poll_retx_e_values[] = {
    "ms5",       "ms10",      "ms15",      "ms20",  "ms25",  "ms30",  "ms35",   "ms40",   "ms45",   "ms50",
    "ms55",      "ms60",      "ms65",      "ms70",  "ms75",  "ms80",  "ms85",   "ms90",   "ms95",   "ms100",
    "ms105",     "ms110",     "ms115",     "ms120", "ms125", "ms130", "ms135",  "ms140",  "ms145",  "ms150",
    "ms155",     "ms160",     "ms165",     "ms170", "ms175", "ms180", "ms185",  "ms190",  "ms195",  "ms200",
    "ms205",     "ms210",     "ms215",     "ms220", "ms225", "ms230", "ms235",  "ms240",  "ms245",  "ms250",
    "ms300",     "ms350",     "ms400",     "ms450", "ms500", "ms800", "ms1000", "ms2000", "ms4000", "ms1-v1610",
    "ms2-v1610", "ms3-v1610", "ms4-v1610", "spare1"};
const asn1::table::enum_desc t_poll_retx_opts::desc = {enum_names_t_poll_retx_e_values, 64, "t_poll_retx_e"};
uint16_t                     t_poll_retx_opts::to_number() const
{
  static const uint16_t numbers[] = {5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,  65,  70,  75,  80,
                                     85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140, 145, 150, 155, 160,
                                     165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220, 225, 230, 235, 240,
                                     245, 250, 300, 350, 400, 450, 500, 800, 1000, 2000, 4000, 1,   2,   3,   4};
  return map_enum_number(numbers, 63, value, "t_poll_retx_e");
}

// PollPDU ::= ENUMERATED
static const char* enum_names_poll_pdu_e_values[] = {
    "p4",     "p8",       "p16",    "p32",    "p64",    "p128",   "p256",   "p512",   "p1024",  "p2048",  "p4096",
    "p6144",  "p8192",    "p12288", "p16384", "p20480", "p24576", "p28672", "p32768", "p40960", "p49152", "p57344",
    "p65536", "infinity", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc poll_pdu_opts::desc = {enum_names_poll_pdu_e_values, 32, "poll_pdu_e"};
int32_t                      poll_pdu_opts::to_number() const
{
  static const int32_t numbers[] = {4,    8,     16,    32,    64,    128,   256,   512,   1024,  2048,  4096,  6144,
                                    8192, 12288, 16384, 20480, 24576, 28672, 32768, 40960, 49152, 57344, 65536, -1};
  return map_enum_number(numbers, 24, value, "poll_pdu_e");
}

// PollByte ::= ENUMERATED
static const char* enum_names_poll_byte_e_values[] = {
    "kB1",     "kB2",     "kB5",     "kB8",      "kB10",    "kB15",    "kB25",    "kB50",    "kB75",    "kB100",
    "kB125",   "kB250",   "kB375",   "kB500",    "kB750",   "kB1000",  "kB1250",  "kB1500",  "kB2000",  "kB3000",
    "kB4000",  "kB4500",  "kB5000",  "kB5500",   "kB6000",  "kB6500",  "kB7000",  "kB7500",  "mB8",     "mB9",
    "mB10",    "mB11",    "mB12",    "mB13",     "mB14",    "mB15",    "mB16",    "mB17",    "mB18",    "mB20",
    "mB25",    "mB30",    "mB40",    "infinity", "spare20", "spare19", "spare18", "spare17", "spare16", "spare15",
    "spare14", "spare13", "spare12", "spare11",  "spare10", "spare9",  "spare8",  "spare7",  "spare6",  "spare5",
    "spare4",  "spare3",  "spare2",  "spare1"};
const asn1::table::enum_desc poll_byte_opts::desc = {enum_names_poll_byte_e_values, 64, "poll_byte_e"};
int32_t                      poll_byte_opts::to_number() const
{
  static const int32_t numbers[] = {1,     2,     5,     8,     10,    15,    25,    50,    75,    100,   125,
                                    250,   375,   500,   750,   1000,  1250,  1500,  2000,  3000,  4000,  4500,
                                    5000,  5500,  6000,  6500,  7000,  7500,  8000,  9000,  10000, 11000, 12000,
                                    13000, 14000, 15000, 16000, 17000, 18000, 20000, 25000, 30000, 40000, -1};
  return map_enum_number(numbers, 44, value, "poll_byte_e");
}

// SN-FieldLengthUM ::= ENUMERATED
static const char*           enum_names_sn_field_len_um_e_values[] = {"size6", "size12"};
const asn1::table::enum_desc sn_field_len_um_opts::desc = {enum_names_sn_field_len_um_e_values, 2, "sn_field_len_um_e"};
uint8_t                      sn_field_len_um_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12};
  return map_enum_number(numbers, 2, value, "sn_field_len_um_e");
}

// BSR-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_bsr_cfg_s_fields[] = {
    {offsetof(bsr_cfg_s, periodic_bsr_timer),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(bsr_cfg_s::periodic_bsr_timer)>(),
     decltype(bsr_cfg_s::periodic_bsr_timer)::has_ext,
     false,
     decltype(bsr_cfg_s::periodic_bsr_timer)::nof_types,
     decltype(bsr_cfg_s::periodic_bsr_timer)::nof_exts,
     nullptr,
     &bsr_cfg_s::periodic_bsr_timer_opts::desc},
    {offsetof(bsr_cfg_s, retx_bsr_timer),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(bsr_cfg_s::retx_bsr_timer)>(),
     decltype(bsr_cfg_s::retx_bsr_timer)::has_ext,
     false,
     decltype(bsr_cfg_s::retx_bsr_timer)::nof_types,
     decltype(bsr_cfg_s::retx_bsr_timer)::nof_exts,
     nullptr,
     &bsr_cfg_s::retx_bsr_timer_opts::desc},
    {offsetof(bsr_cfg_s, lc_ch_sr_delay_timer),
     (int32_t)offsetof(bsr_cfg_s, lc_ch_sr_delay_timer_present),
     asn1::table::enum_op<decltype(bsr_cfg_s::lc_ch_sr_delay_timer)>(),
     decltype(bsr_cfg_s::lc_ch_sr_delay_timer)::has_ext,
     false,
     decltype(bsr_cfg_s::lc_ch_sr_delay_timer)::nof_types,
     decltype(bsr_cfg_s::lc_ch_sr_delay_timer)::nof_exts,
     nullptr,
     &bsr_cfg_s::lc_ch_sr_delay_timer_opts::desc}};
static const asn1::table::field_name seq_table_bsr_cfg_s_names[] = {{"periodicBSR-Timer", nullptr},
                                                                    {"retxBSR-Timer", nullptr},
                                                                    {"logicalChannelSR-DelayTimer", nullptr}};
static const asn1::table::seq_opt    seq_table_bsr_cfg_s_opts[]  = {
    {2, (int32_t)offsetof(bsr_cfg_s, lc_ch_sr_delay_timer_present)}};
const asn1::table::seq_desc bsr_cfg_s::desc = {
    {seq_table_bsr_cfg_s_fields, uint64_t{0x3}, seq_table_bsr_cfg_s_opts, seq_table_bsr_cfg_s_names, 3, 1},
    true,
    false,
    nullptr,
    0};

static const char*           enum_names_bsr_cfg_s_periodic_bsr_timer_e__values[] = {"sf1",
                                                                                    "sf5",
                                                                                    "sf10",
                                                                                    "sf16",
                                                                                    "sf20",
                                                                                    "sf32",
                                                                                    "sf40",
                                                                                    "sf64",
                                                                                    "sf80",
                                                                                    "sf128",
                                                                                    "sf160",
                                                                                    "sf320",
                                                                                    "sf640",
                                                                                    "sf1280",
                                                                                    "sf2560",
                                                                                    "infinity"};
const asn1::table::enum_desc bsr_cfg_s::periodic_bsr_timer_opts::desc            = {
    enum_names_bsr_cfg_s_periodic_bsr_timer_e__values,
    16,
    "bsr_cfg_s::periodic_bsr_timer_e_"};
int16_t bsr_cfg_s::periodic_bsr_timer_opts::to_number() const
{
  static const int16_t numbers[] = {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return map_enum_number(numbers, 16, value, "bsr_cfg_s::periodic_bsr_timer_e_");
}

static const char*           enum_names_bsr_cfg_s_retx_bsr_timer_e__values[] = {"sf10",
                                                                                "sf20",
                                                                                "sf40",
                                                                                "sf80",
                                                                                "sf160",
                                                                                "sf320",
                                                                                "sf640",
                                                                                "sf1280",
                                                                                "sf2560",
                                                                                "sf5120",
                                                                                "sf10240",
                                                                                "spare5",
                                                                                "spare4",
                                                                                "spare3",
                                                                                "spare2",
                                                                                "spare1"};
const asn1::table::enum_desc bsr_cfg_s::retx_bsr_timer_opts::desc = {enum_names_bsr_cfg_s_retx_bsr_timer_e__values,
                                                                     16,
                                                                     "bsr_cfg_s::retx_bsr_timer_e_"};
uint16_t                     bsr_cfg_s::retx_bsr_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(numbers, 11, value, "bsr_cfg_s::retx_bsr_timer_e_");
}

static const char* enum_names_bsr_cfg_s_lc_ch_sr_delay_timer_e__values[] =
    {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
const asn1::table::enum_desc bsr_cfg_s::lc_ch_sr_delay_timer_opts::desc = {
    enum_names_bsr_cfg_s_lc_ch_sr_delay_timer_e__values,
    8,
    "bsr_cfg_s::lc_ch_sr_delay_timer_e_"};
uint16_t bsr_cfg_s::lc_ch_sr_delay_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(numbers, 7, value, "bsr_cfg_s::lc_ch_sr_delay_timer_e_");
}

// RRCReject-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_reject_ies_s_fields[] = {
    {offsetof(rrc_reject_ies_s, wait_time),
     (int32_t)offsetof(rrc_reject_ies_s, wait_time_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(rrc_reject_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_reject_ies_s::late_non_crit_ext), asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(rrc_reject_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_rrc_reject_ies_s_names[] = {{"waitTime", nullptr},
                                                                           {"lateNonCriticalExtension", nullptr},
                                                                           {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_rrc_reject_ies_s_opts[]  = {
    {0, (int32_t)offsetof(rrc_reject_ies_s, wait_time_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(rrc_reject_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_reject_ies_s::desc = {{seq_table_rrc_reject_ies_s_fields,
                                                       uint64_t{0x0},
                                                       seq_table_rrc_reject_ies_s_opts,
                                                       seq_table_rrc_reject_ies_s_names,
                                                       3,
                                                       3},
                                                      false,
                                                      false,
                                                      nullptr,
                                                      0};

// RRCReject ::= SEQUENCE
static const asn1::table::seq_field  seq_table_rrc_reject_s_fields[] = {{offsetof(rrc_reject_s, crit_exts),
                                                                         asn1::table::seq_field::mandatory,
                                                                         asn1::table::field_op::choice,
                                                                         false,
                                                                         false,
                                                                         0,
                                                                         0,
                                                                         nullptr,
                                                                         &rrc_reject_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_rrc_reject_s_names[]  = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          rrc_reject_s::desc              = {
    {seq_table_rrc_reject_s_fields, uint64_t{0x1}, nullptr, seq_table_rrc_reject_s_names, 1, 0},
    false,
    false,
    nullptr,
    0};

void rrc_reject_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_reject_ies_s& rrc_reject_s::crit_exts_c_::set_rrc_reject()
{
  set(types::rrc_reject);
  return c;
}
void rrc_reject_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_rrc_reject_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_reject_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_rrc_reject_s_crit_exts_c__names[] = {
    {"rrcReject", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc rrc_reject_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<rrc_reject_s::crit_exts_c_::types>(),
    rrc_reject_s::crit_exts_c_::types::nof_types,
    rrc_reject_s::crit_exts_c_::types::nof_exts,
    rrc_reject_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_rrc_reject_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<rrc_reject_s::crit_exts_c_>,
    choice_table_rrc_reject_s_crit_exts_c__names,
    "rrc_reject_s::crit_exts_c_",
    offsetof(rrc_reject_s::crit_exts_c_, type_),
    offsetof(rrc_reject_s::crit_exts_c_, c),
    false};

static const char* enum_names_rrc_reject_s_crit_exts_c__types_values[]    = {"rrcReject", "criticalExtensionsFuture"};
const asn1::table::enum_desc rrc_reject_s::crit_exts_c_::types_opts::desc = {
    enum_names_rrc_reject_s_crit_exts_c__types_values,
    2,
    "rrc_reject_s::crit_exts_c_::types"};

// MAC-MainConfigSL-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mac_main_cfg_sl_r16_s_fields[] = {
    {offsetof(mac_main_cfg_sl_r16_s, sl_bsr_cfg_r16),
     (int32_t)offsetof(mac_main_cfg_sl_r16_s, sl_bsr_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bsr_cfg_s::desc},
    {offsetof(mac_main_cfg_sl_r16_s, ul_prioritization_thres_r16),
     (int32_t)offsetof(mac_main_cfg_sl_r16_s, ul_prioritization_thres_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(mac_main_cfg_sl_r16_s, sl_prioritization_thres_r16),
     (int32_t)offsetof(mac_main_cfg_sl_r16_s, sl_prioritization_thres_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr}};
static const asn1::table::field_name seq_table_mac_main_cfg_sl_r16_s_names[] = {
    {"sl-BSR-Config-r16", nullptr},
    {"ul-PrioritizationThres-r16", nullptr},
    {"sl-PrioritizationThres-r16", nullptr}};
static const asn1::table::seq_opt seq_table_mac_main_cfg_sl_r16_s_opts[] = {
    {0, (int32_t)offsetof(mac_main_cfg_sl_r16_s, sl_bsr_cfg_r16_present)},
    {1, (int32_t)offsetof(mac_main_cfg_sl_r16_s, ul_prioritization_thres_r16_present)},
    {2, (int32_t)offsetof(mac_main_cfg_sl_r16_s, sl_prioritization_thres_r16_present)}};
const asn1::table::seq_desc mac_main_cfg_sl_r16_s::desc = {{seq_table_mac_main_cfg_sl_r16_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_mac_main_cfg_sl_r16_s_opts,
                                                            seq_table_mac_main_cfg_sl_r16_s_names,
                                                            3,
                                                            3},
                                                           true,
                                                           false,
                                                           nullptr,
                                                           0};

// SL-PeriodCG-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sl_period_cg_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<sl_period_cg_r16_c::sl_period_cg1_r16_e_, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc sl_period_cg_r16_c::storage_desc = {
    choice_storage_sl_period_cg_r16_c_alternatives,
    2,
    "sl_period_cg_r16_c"};

sl_period_cg_r16_c::sl_period_cg1_r16_e_& sl_period_cg_r16_c::set_sl_period_cg1_r16()
{
  set(types::sl_period_cg1_r16);
  return c.get<sl_period_cg1_r16_e_>();
}
uint8_t& sl_period_cg_r16_c::set_sl_period_cg2_r16()
{
  set(types::sl_period_cg2_r16);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_sl_period_cg_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<sl_period_cg_r16_c::sl_period_cg1_r16_e_>(),
     sl_period_cg_r16_c::sl_period_cg1_r16_e_::has_ext,
     false,
     sl_period_cg_r16_c::sl_period_cg1_r16_e_::nof_types,
     sl_period_cg_r16_c::sl_period_cg1_r16_e_::nof_exts,
     nullptr,
     &sl_period_cg_r16_c::sl_period_cg1_r16_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{98},
     nullptr}};
static const asn1::table::field_name choice_table_sl_period_cg_r16_c_names[] = {{"sl-PeriodCG1-r16", nullptr},
                                                                                {"sl-PeriodCG2-r16", nullptr}};
const asn1::table::choice_desc sl_period_cg_r16_c::desc = {asn1::table::enum_int_type<sl_period_cg_r16_c::types>(),
                                                           sl_period_cg_r16_c::types::nof_types,
                                                           sl_period_cg_r16_c::types::nof_exts,
                                                           sl_period_cg_r16_c::types::has_ext,
                                                           false,
                                                           choice_table_sl_period_cg_r16_c_alternatives,
                                                           nullptr,
                                                           choice_table_sl_period_cg_r16_c_names,
                                                           "sl_period_cg_r16_c",
                                                           offsetof(sl_period_cg_r16_c, c.type),
                                                           offsetof(sl_period_cg_r16_c, c.value),
                                                           false,
                                                           &sl_period_cg_r16_c::storage_desc};

static const char*           enum_names_sl_period_cg_r16_c_sl_period_cg1_r16_e__values[] = {"ms100",
                                                                                            "ms200",
                                                                                            "ms300",
                                                                                            "ms400",
                                                                                            "ms500",
                                                                                            "ms600",
                                                                                            "ms700",
                                                                                            "ms800",
                                                                                            "ms900",
                                                                                            "ms1000",
                                                                                            "spare6",
                                                                                            "spare5",
                                                                                            "spare4",
                                                                                            "spare3",
                                                                                            "spare2",
                                                                                            "spare1"};
const asn1::table::enum_desc sl_period_cg_r16_c::sl_period_cg1_r16_opts::desc            = {
    enum_names_sl_period_cg_r16_c_sl_period_cg1_r16_e__values,
    16,
    "sl_period_cg_r16_c::sl_period_cg1_r16_e_"};
uint16_t sl_period_cg_r16_c::sl_period_cg1_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(numbers, 10, value, "sl_period_cg_r16_c::sl_period_cg1_r16_e_");
}

static const char*           enum_names_sl_period_cg_r16_c_types_values[] = {"sl-PeriodCG1-r16", "sl-PeriodCG2-r16"};
const asn1::table::enum_desc sl_period_cg_r16_c::types_opts::desc         = {enum_names_sl_period_cg_r16_c_types_values,
                                                                             2,
                                                                             "sl_period_cg_r16_c::types"};
uint8_t                      sl_period_cg_r16_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_period_cg_r16_c::types");
}

// SL-CG-MaxTransNum-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cg_max_trans_num_r16_s_fields[] = {
    {offsetof(sl_cg_max_trans_num_r16_s, sl_prio_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_cg_max_trans_num_r16_s, sl_max_trans_num_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_sl_cg_max_trans_num_r16_s_names[] = {{"sl-Priority-r16", nullptr},
                                                                                    {"sl-MaxTransNum-r16", nullptr}};
const asn1::table::seq_desc          sl_cg_max_trans_num_r16_s::desc = {{seq_table_sl_cg_max_trans_num_r16_s_fields,
                                                                         uint64_t{0x3},
                                                                         nullptr,
                                                                         seq_table_sl_cg_max_trans_num_r16_s_names,
                                                                         2,
                                                                         0},
                                                                        false,
                                                                        false,
                                                                        nullptr,
                                                                        0};

// SL-ConfiguredGrantConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s__fields[] = {
    {offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_res_cg_type1_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_res_cg_type1_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{496},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_start_subch_cg_type1_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_start_subch_cg_type1_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{26},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_freq_res_cg_type1_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_freq_res_cg_type1_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{6929},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_offset_cg_type1_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_offset_cg_type1_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{7999},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_n1_pucch_an_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_n1_pucch_an_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_psfch_to_pucch_cg_type1_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_psfch_to_pucch_cg_type1_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_res_pool_id_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_res_pool_id_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {0,
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_ref_sfn_type1_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s__names[] = {
    {"sl-TimeResourceCG-Type1-r16", nullptr},
    {"sl-StartSubchannelCG-Type1-r16", nullptr},
    {"sl-FreqResourceCG-Type1-r16", nullptr},
    {"sl-TimeOffsetCG-Type1-r16", nullptr},
    {"sl-N1PUCCH-AN-r16", nullptr},
    {"sl-PSFCH-ToPUCCH-CG-Type1-r16", nullptr},
    {"sl-ResourcePoolID-r16", nullptr},
    {"sl-TimeReferenceSFN-Type1-r16", "sfn512"}};
static const asn1::table::seq_opt seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_res_cg_type1_r16_present)},
    {1, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_start_subch_cg_type1_r16_present)},
    {2, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_freq_res_cg_type1_r16_present)},
    {3, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_offset_cg_type1_r16_present)},
    {4, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_n1_pucch_an_r16_present)},
    {5,
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_psfch_to_pucch_cg_type1_r16_present)},
    {6, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_res_pool_id_r16_present)},
    {7, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s::rrc_cfg_sidelink_grant_r16_s_, sl_time_ref_sfn_type1_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s_ = {
    {seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s__fields,
     uint64_t{0x0},
     seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s__opts,
     seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s__names,
     8,
     8},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_sl_cfg_grant_cfg_r16_s_fields[] = {
    {offsetof(sl_cfg_grant_cfg_r16_s, sl_cfg_idx_cg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s, sl_period_cg_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_period_cg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_period_cg_r16_c::desc},
    {offsetof(sl_cfg_grant_cfg_r16_s, sl_nr_of_harq_processes_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_nr_of_harq_processes_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s, sl_harq_proc_id_offset_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_harq_proc_id_offset_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_r16_s, sl_cg_max_trans_num_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_cfg_grant_cfg_r16_s::sl_cg_max_trans_num_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_cg_max_trans_num_r16_s::desc},
    {offsetof(sl_cfg_grant_cfg_r16_s, rrc_cfg_sidelink_grant_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, rrc_cfg_sidelink_grant_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_cfg_grant_cfg_r16_s_rrc_cfg_sidelink_grant_r16_s_}};
static const asn1::table::field_name seq_table_sl_cfg_grant_cfg_r16_s_names[] = {
    {"sl-ConfigIndexCG-r16", nullptr},
    {"sl-PeriodCG-r16", nullptr},
    {"sl-NrOfHARQ-Processes-r16", nullptr},
    {"sl-HARQ-ProcID-offset-r16", nullptr},
    {"sl-CG-MaxTransNumList-r16", nullptr},
    {"rrc-ConfiguredSidelinkGrant-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_grant_cfg_r16_s_opts[] = {
    {1, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_period_cg_r16_present)},
    {2, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_nr_of_harq_processes_r16_present)},
    {3, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_harq_proc_id_offset_r16_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, rrc_cfg_sidelink_grant_r16_present)}};
static const asn1::table::seq_field seq_table_sl_cfg_grant_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_cfg_grant_cfg_r16_s, sl_n1_pucch_an_type2_r16),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_n1_pucch_an_type2_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr}};
static const asn1::table::field_name seq_table_sl_cfg_grant_cfg_r16_s_g0_names[] = {
    {"sl-N1PUCCH-AN-Type2-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_grant_cfg_r16_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_n1_pucch_an_type2_r16_present)}};
static const asn1::table::seq_field seq_table_sl_cfg_grant_cfg_r16_s_g1_fields[] = {
    {offsetof(sl_cfg_grant_cfg_r16_s, sl_start_r_bset_cg_type1_r18),
     (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_start_r_bset_cg_type1_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{4},
     nullptr}};
static const asn1::table::field_name seq_table_sl_cfg_grant_cfg_r16_s_g1_names[] = {
    {"sl-StartRBsetCG-Type1-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_grant_cfg_r16_s_g1_opts[] = {
    {0, (int32_t)offsetof(sl_cfg_grant_cfg_r16_s, sl_start_r_bset_cg_type1_r18_present)}};
static const asn1::table::seq_group seq_table_sl_cfg_grant_cfg_r16_s_groups[] = {
    {{seq_table_sl_cfg_grant_cfg_r16_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_cfg_grant_cfg_r16_s_g0_opts,
      seq_table_sl_cfg_grant_cfg_r16_s_g0_names,
      1,
      1},
     false},
    {{seq_table_sl_cfg_grant_cfg_r16_s_g1_fields,
      uint64_t{0x0},
      seq_table_sl_cfg_grant_cfg_r16_s_g1_opts,
      seq_table_sl_cfg_grant_cfg_r16_s_g1_names,
      1,
      1},
     false}};
const asn1::table::seq_desc sl_cfg_grant_cfg_r16_s::desc = {{seq_table_sl_cfg_grant_cfg_r16_s_fields,
                                                             uint64_t{0x1},
                                                             seq_table_sl_cfg_grant_cfg_r16_s_opts,
                                                             seq_table_sl_cfg_grant_cfg_r16_s_names,
                                                             6,
                                                             5},
                                                            true,
                                                            false,
                                                            seq_table_sl_cfg_grant_cfg_r16_s_groups,
                                                            2};

// SL-ConfiguredGrantConfigList-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cfg_grant_cfg_list_r16_s_fields[] = {
    {offsetof(sl_cfg_grant_cfg_list_r16_s, sl_cfg_grant_cfg_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_cfg_grant_cfg_list_r16_s::sl_cfg_grant_cfg_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{7}, false, false>>::value},
    {offsetof(sl_cfg_grant_cfg_list_r16_s, sl_cfg_grant_cfg_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_cfg_grant_cfg_list_r16_s::sl_cfg_grant_cfg_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_cfg_grant_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_grant_cfg_list_r16_s_names[] = {
    {"sl-ConfiguredGrantConfigToReleaseList-r16", nullptr},
    {"sl-ConfiguredGrantConfigToAddModList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_grant_cfg_list_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_cfg_grant_cfg_list_r16_s::desc = {{seq_table_sl_cfg_grant_cfg_list_r16_s_fields,
                                                                  uint64_t{0x0},
                                                                  seq_table_sl_cfg_grant_cfg_list_r16_s_opts,
                                                                  seq_table_sl_cfg_grant_cfg_list_r16_s_names,
                                                                  2,
                                                                  2},
                                                                 false,
                                                                 false,
                                                                 nullptr,
                                                                 0};

// SL-ConfiguredGrantConfigDedicatedSL-PRS-RP-r18 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s__fields[] = {
        {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                  sl_time_offset_cg_type1_r18),
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_time_offset_cg_type1_r18_present),
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{7999},
         nullptr},
        {0,
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_time_ref_sfn_type1_r18_present),
         asn1::table::field_op::none,
         false,
         false,
         0,
         0,
         nullptr},
        {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                  sl_time_res_cg_type1_r18),
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_time_res_cg_type1_r18_present),
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{496},
         nullptr},
        {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                  sl_prs_res_ind_first_type1_r18),
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_prs_res_ind_first_type1_r18_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{11},
         nullptr},
        {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                  sl_prs_res_ind_future_type1_r18),
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_prs_res_ind_future_type1_r18_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{143},
         nullptr}};
static const asn1::table::field_name
    seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s__names[] = {
        {"sl-TimeOffsetCG-Type1-r18", nullptr},
        {"sl-TimeReferenceSFN-Type1-r18", "sfn512"},
        {"sl-TimeResourceCG-Type1-r18", nullptr},
        {"sl-PRS-ResourceIndicationFirstType1-r18", nullptr},
        {"sl-PRS-ResourceIndicationFutureType1-r18", nullptr}};
static const asn1::table::seq_opt
    seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s__opts[] = {
        {0,
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_time_offset_cg_type1_r18_present)},
        {1,
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_time_ref_sfn_type1_r18_present)},
        {2,
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_time_res_cg_type1_r18_present)},
        {3,
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_prs_res_ind_first_type1_r18_present)},
        {4,
         (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_,
                           sl_prs_res_ind_future_type1_r18_present)}};
static const asn1::table::seq_desc
    seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_ = {
        {seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s__fields,
         uint64_t{0x0},
         seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s__opts,
         seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s__names,
         5,
         5},
        false,
        false,
        nullptr,
        0};
static const asn1::table::seq_field seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_fields[] = {
    {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, sl_prs_cfg_idx_cg_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, sl_prs_period_cg_r18),
     (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, sl_prs_period_cg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_period_cg_r16_c::desc},
    {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, sl_prs_res_pool_id_r18),
     (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, sl_prs_res_pool_id_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_rrc_cfg_sidelink_grant_ded_sl_prs_rp_r18_s_}};
static const asn1::table::field_name seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_names[] = {
    {"sl-PRS-ConfigIndexCG-r18", nullptr},
    {"sl-PRS-PeriodCG-r18", nullptr},
    {"sl-PRS-ResourcePoolID-r18", nullptr},
    {"rrc-ConfiguredSidelinkGrantDedicatedSL-PRS-RP-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_opts[] = {
    {1, (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, sl_prs_period_cg_r18_present)},
    {2, (int32_t)offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s, sl_prs_res_pool_id_r18_present)}};
const asn1::table::seq_desc sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::desc = {
    {seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_fields,
     uint64_t{0x9},
     seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_opts,
     seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s_names,
     4,
     2},
    false,
    false,
    nullptr,
    0};

// SL-ConfiguredGrantConfigDedicatedSL-PRS-RP-List-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s_fields[] = {
    {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s, sl_cfg_grant_cfg_ded_sl_prs_rp_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<
         decltype(sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s::sl_cfg_grant_cfg_ded_sl_prs_rp_to_release_list_r18),
         asn1::table::int_json,
         asn1::table::int_items<false, int64_t{0}, uint64_t{7}, false, false>>::value},
    {offsetof(sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s, sl_cfg_grant_cfg_ded_sl_prs_rp_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<
         decltype(sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s::sl_cfg_grant_cfg_ded_sl_prs_rp_to_add_mod_list_r18),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &sl_cfg_grant_cfg_ded_sl_prs_rp_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s_names[] = {
    {"sl-ConfiguredGrantConfigDedicatedSL-PRS-RP-ToReleaseList-r18", nullptr},
    {"sl-ConfiguredGrantConfigDedicatedSL-PRS-RP-ToAddModList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s::desc = {
    {seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s_fields,
     uint64_t{0x0},
     seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s_opts,
     seq_table_sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s_names,
     2,
     2},
    false,
    false,
    nullptr,
    0};

// SL-ScheduledConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sched_cfg_r16_s_fields[] = {
    {offsetof(sl_sched_cfg_r16_s, sl_rnti_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(sl_sched_cfg_r16_s, mac_main_cfg_sl_r16),
     (int32_t)offsetof(sl_sched_cfg_r16_s, mac_main_cfg_sl_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mac_main_cfg_sl_r16_s::desc},
    {offsetof(sl_sched_cfg_r16_s, sl_cs_rnti_r16),
     (int32_t)offsetof(sl_sched_cfg_r16_s, sl_cs_rnti_r16_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(sl_sched_cfg_r16_s, sl_psfch_to_pucch_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_sched_cfg_r16_s::sl_psfch_to_pucch_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{15}, false, false>>::value},
    {offsetof(sl_sched_cfg_r16_s, sl_cfg_grant_cfg_list_r16),
     (int32_t)offsetof(sl_sched_cfg_r16_s, sl_cfg_grant_cfg_list_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_cfg_grant_cfg_list_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_sched_cfg_r16_s_names[] = {
    {"sl-RNTI-r16", nullptr},
    {"mac-MainConfigSL-r16", nullptr},
    {"sl-CS-RNTI-r16", nullptr},
    {"sl-PSFCH-ToPUCCH-r16", nullptr},
    {"sl-ConfiguredGrantConfigList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_sched_cfg_r16_s_opts[] = {
    {1, (int32_t)offsetof(sl_sched_cfg_r16_s, mac_main_cfg_sl_r16_present)},
    {2, (int32_t)offsetof(sl_sched_cfg_r16_s, sl_cs_rnti_r16_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(sl_sched_cfg_r16_s, sl_cfg_grant_cfg_list_r16_present)}};
static const asn1::table::seq_field seq_table_sl_sched_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_sched_cfg_r16_s, sl_dci_to_sl_trans_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_list_codec<decltype(sl_sched_cfg_r16_s::sl_dci_to_sl_trans_r16),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{1}, uint64_t{31}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_sched_cfg_r16_s_g0_names[] = {{"sl-DCI-ToSL-Trans-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_sched_cfg_r16_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_sched_cfg_r16_s_g1_fields[] = {
    {offsetof(sl_sched_cfg_r16_s, sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_sched_cfg_r16_s::sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18)>::value,
     &sl_cfg_grant_cfg_ded_sl_prs_rp_list_r18_s::desc},
    {offsetof(sl_sched_cfg_r16_s, sl_prs_rnti_r18),
     (int32_t)offsetof(sl_sched_cfg_r16_s, sl_prs_rnti_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(sl_sched_cfg_r16_s, sl_prs_cs_rnti_r18),
     (int32_t)offsetof(sl_sched_cfg_r16_s, sl_prs_cs_rnti_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_sl_sched_cfg_r16_s_g1_names[] = {
    {"sl-ConfiguredGrantConfigDedicatedSL-PRS-RP-List-r18", nullptr},
    {"sl-PRS-RNTI-r18", nullptr},
    {"sl-PRS-CS-RNTI-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_sched_cfg_r16_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(sl_sched_cfg_r16_s, sl_prs_rnti_r18_present)},
    {2, (int32_t)offsetof(sl_sched_cfg_r16_s, sl_prs_cs_rnti_r18_present)}};
static const asn1::table::seq_group seq_table_sl_sched_cfg_r16_s_groups[] = {{{seq_table_sl_sched_cfg_r16_s_g0_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_sl_sched_cfg_r16_s_g0_opts,
                                                                               seq_table_sl_sched_cfg_r16_s_g0_names,
                                                                               1,
                                                                               1},
                                                                              false},
                                                                             {{seq_table_sl_sched_cfg_r16_s_g1_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_sl_sched_cfg_r16_s_g1_opts,
                                                                               seq_table_sl_sched_cfg_r16_s_g1_names,
                                                                               3,
                                                                               3},
                                                                              false}};
const asn1::table::seq_desc         sl_sched_cfg_r16_s::desc              = {{seq_table_sl_sched_cfg_r16_s_fields,
                                                                              uint64_t{0x1},
                                                                              seq_table_sl_sched_cfg_r16_s_opts,
                                                                              seq_table_sl_sched_cfg_r16_s_names,
                                                                              5,
                                                                              4},
                                                                             true,
                                                                             false,
                                                                             seq_table_sl_sched_cfg_r16_s_groups,
                                                                             2};

// SL-TypeTxSync-r16 ::= ENUMERATED
static const char*           enum_names_sl_type_tx_sync_r16_e_values[] = {"gnss", "gnbEnb", "ue"};
const asn1::table::enum_desc sl_type_tx_sync_r16_opts::desc            = {enum_names_sl_type_tx_sync_r16_e_values,
                                                                          3,
                                                                          "sl_type_tx_sync_r16_e"};

// SL-TxPower-r16 ::= CHOICE
void sl_tx_pwr_r16_c::set(types::options e)
{
  type_ = e;
}
void sl_tx_pwr_r16_c::set_minusinfinity_r16()
{
  set(types::minusinfinity_r16);
}
int8_t& sl_tx_pwr_r16_c::set_tx_pwr_r16()
{
  set(types::tx_pwr_r16);
  return c;
}
static const asn1::table::seq_field choice_table_sl_tx_pwr_r16_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr}};
static const asn1::table::field_name choice_table_sl_tx_pwr_r16_c_names[] = {{"minusinfinity-r16", nullptr},
                                                                             {"txPower-r16", nullptr}};
const asn1::table::choice_desc       sl_tx_pwr_r16_c::desc = {asn1::table::enum_int_type<sl_tx_pwr_r16_c::types>(),
                                                              sl_tx_pwr_r16_c::types::nof_types,
                                                              sl_tx_pwr_r16_c::types::nof_exts,
                                                              sl_tx_pwr_r16_c::types::has_ext,
                                                              false,
                                                              choice_table_sl_tx_pwr_r16_c_alternatives,
                                                              &asn1::table::choice_set<sl_tx_pwr_r16_c>,
                                                              choice_table_sl_tx_pwr_r16_c_names,
                                                              "sl_tx_pwr_r16_c",
                                                              offsetof(sl_tx_pwr_r16_c, type_),
                                                              offsetof(sl_tx_pwr_r16_c, c),
                                                              false};

static const char*           enum_names_sl_tx_pwr_r16_c_types_values[] = {"minusinfinity-r16", "txPower-r16"};
const asn1::table::enum_desc sl_tx_pwr_r16_c::types_opts::desc         = {enum_names_sl_tx_pwr_r16_c_types_values,
                                                                          2,
                                                                          "sl_tx_pwr_r16_c::types"};
int8_t                       sl_tx_pwr_r16_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-1};
  return map_enum_number(numbers, 1, value, "sl_tx_pwr_r16_c::types");
}

// SL-PSSCH-TxParameters-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pssch_tx_params_r16_s_fields[] = {
    {offsetof(sl_pssch_tx_params_r16_s, sl_min_mcs_pssch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{27},
     nullptr},
    {offsetof(sl_pssch_tx_params_r16_s, sl_max_mcs_pssch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {offsetof(sl_pssch_tx_params_r16_s, sl_min_sub_ch_num_pssch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{26},
     nullptr},
    {offsetof(sl_pssch_tx_params_r16_s, sl_max_subch_num_pssch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{26},
     nullptr},
    {offsetof(sl_pssch_tx_params_r16_s, sl_max_tx_trans_num_pssch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(sl_pssch_tx_params_r16_s, sl_max_tx_pwr_r16),
     (int32_t)offsetof(sl_pssch_tx_params_r16_s, sl_max_tx_pwr_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_tx_pwr_r16_c::desc}};
static const asn1::table::field_name seq_table_sl_pssch_tx_params_r16_s_names[] = {
    {"sl-MinMCS-PSSCH-r16", nullptr},
    {"sl-MaxMCS-PSSCH-r16", nullptr},
    {"sl-MinSubChannelNumPSSCH-r16", nullptr},
    {"sl-MaxSubchannelNumPSSCH-r16", nullptr},
    {"sl-MaxTxTransNumPSSCH-r16", nullptr},
    {"sl-MaxTxPower-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pssch_tx_params_r16_s_opts[] = {
    {5, (int32_t)offsetof(sl_pssch_tx_params_r16_s, sl_max_tx_pwr_r16_present)}};
const asn1::table::seq_desc sl_pssch_tx_params_r16_s::desc = {{seq_table_sl_pssch_tx_params_r16_s_fields,
                                                               uint64_t{0x1f},
                                                               seq_table_sl_pssch_tx_params_r16_s_opts,
                                                               seq_table_sl_pssch_tx_params_r16_s_names,
                                                               6,
                                                               1},
                                                              false,
                                                              false,
                                                              nullptr,
                                                              0};

// SL-MinMaxMCS-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_min_max_mcs_cfg_r16_s_fields[] = {
    {offsetof(sl_min_max_mcs_cfg_r16_s, sl_mcs_table_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16)>(),
     decltype(sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16)::has_ext,
     false,
     decltype(sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16)::nof_types,
     decltype(sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16)::nof_exts,
     nullptr,
     &sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16_opts::desc},
    {offsetof(sl_min_max_mcs_cfg_r16_s, sl_min_mcs_pssch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{27},
     nullptr},
    {offsetof(sl_min_max_mcs_cfg_r16_s, sl_max_mcs_pssch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_sl_min_max_mcs_cfg_r16_s_names[] = {{"sl-MCS-Table-r16", nullptr},
                                                                                   {"sl-MinMCS-PSSCH-r16", nullptr},
                                                                                   {"sl-MaxMCS-PSSCH-r16", nullptr}};
const asn1::table::seq_desc          sl_min_max_mcs_cfg_r16_s::desc             = {
    {seq_table_sl_min_max_mcs_cfg_r16_s_fields, uint64_t{0x7}, nullptr, seq_table_sl_min_max_mcs_cfg_r16_s_names, 3, 0},
    false,
    false,
    nullptr,
    0};

static const char* enum_names_sl_min_max_mcs_cfg_r16_s_sl_mcs_table_r16_e__values[] = {"qam64", "qam256", "qam64LowSE"};
const asn1::table::enum_desc sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16_opts::desc  = {
    enum_names_sl_min_max_mcs_cfg_r16_s_sl_mcs_table_r16_e__values,
    3,
    "sl_min_max_mcs_cfg_r16_s::sl_mcs_table_r16_e_"};

// SL-PSSCH-TxConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pssch_tx_cfg_r16_s_fields[] = {
    {offsetof(sl_pssch_tx_cfg_r16_s, sl_type_tx_sync_r16),
     (int32_t)offsetof(sl_pssch_tx_cfg_r16_s, sl_type_tx_sync_r16_present),
     asn1::table::enum_op<decltype(sl_pssch_tx_cfg_r16_s::sl_type_tx_sync_r16)>(),
     decltype(sl_pssch_tx_cfg_r16_s::sl_type_tx_sync_r16)::has_ext,
     false,
     decltype(sl_pssch_tx_cfg_r16_s::sl_type_tx_sync_r16)::nof_types,
     decltype(sl_pssch_tx_cfg_r16_s::sl_type_tx_sync_r16)::nof_exts,
     nullptr,
     &sl_type_tx_sync_r16_opts::desc},
    {offsetof(sl_pssch_tx_cfg_r16_s, sl_thres_ue_speed_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16)>(),
     decltype(sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16)::has_ext,
     false,
     decltype(sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16)::nof_types,
     decltype(sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16)::nof_exts,
     nullptr,
     &sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_opts::desc},
    {offsetof(sl_pssch_tx_cfg_r16_s, sl_params_above_thres_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pssch_tx_params_r16_s::desc},
    {offsetof(sl_pssch_tx_cfg_r16_s, sl_params_below_thres_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pssch_tx_params_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_pssch_tx_cfg_r16_s_names[] = {
    {"sl-TypeTxSync-r16", nullptr},
    {"sl-ThresUE-Speed-r16", nullptr},
    {"sl-ParametersAboveThres-r16", nullptr},
    {"sl-ParametersBelowThres-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pssch_tx_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_pssch_tx_cfg_r16_s, sl_type_tx_sync_r16_present)}};
static const asn1::table::seq_field seq_table_sl_pssch_tx_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_pssch_tx_cfg_r16_s, sl_params_above_thres_v1650),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::ptr_item_list_codec<decltype(sl_pssch_tx_cfg_r16_s::sl_params_above_thres_v1650),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_min_max_mcs_cfg_r16_s::desc},
    {offsetof(sl_pssch_tx_cfg_r16_s, sl_params_below_thres_v1650),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::ptr_item_list_codec<decltype(sl_pssch_tx_cfg_r16_s::sl_params_below_thres_v1650),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_min_max_mcs_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_pssch_tx_cfg_r16_s_g0_names[] = {
    {"sl-ParametersAboveThres-v1650", nullptr},
    {"sl-ParametersBelowThres-v1650", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pssch_tx_cfg_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_pssch_tx_cfg_r16_s_groups[] = {
    {{seq_table_sl_pssch_tx_cfg_r16_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_pssch_tx_cfg_r16_s_g0_opts,
      seq_table_sl_pssch_tx_cfg_r16_s_g0_names,
      2,
      2},
     false}};
const asn1::table::seq_desc sl_pssch_tx_cfg_r16_s::desc = {{seq_table_sl_pssch_tx_cfg_r16_s_fields,
                                                            uint64_t{0xe},
                                                            seq_table_sl_pssch_tx_cfg_r16_s_opts,
                                                            seq_table_sl_pssch_tx_cfg_r16_s_names,
                                                            4,
                                                            1},
                                                           true,
                                                           false,
                                                           seq_table_sl_pssch_tx_cfg_r16_s_groups,
                                                           1};

static const char* enum_names_sl_pssch_tx_cfg_r16_s_sl_thres_ue_speed_r16_e__values[] =
    {"kmph60", "kmph80", "kmph100", "kmph120", "kmph140", "kmph160", "kmph180", "kmph200"};
const asn1::table::enum_desc sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_opts::desc = {
    enum_names_sl_pssch_tx_cfg_r16_s_sl_thres_ue_speed_r16_e__values,
    8,
    "sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_e_"};
uint8_t sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {60, 80, 100, 120, 140, 160, 180, 200};
  return map_enum_number(numbers, 8, value, "sl_pssch_tx_cfg_r16_s::sl_thres_ue_speed_r16_e_");
}

// SL-CBR-PSSCH-TxConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cbr_pssch_tx_cfg_r16_s_fields[] = {
    {offsetof(sl_cbr_pssch_tx_cfg_r16_s, sl_cr_limit_r16),
     (int32_t)offsetof(sl_cbr_pssch_tx_cfg_r16_s, sl_cr_limit_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{10000},
     nullptr},
    {offsetof(sl_cbr_pssch_tx_cfg_r16_s, sl_tx_params_r16),
     (int32_t)offsetof(sl_cbr_pssch_tx_cfg_r16_s, sl_tx_params_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pssch_tx_params_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_cbr_pssch_tx_cfg_r16_s_names[] = {{"sl-CR-Limit-r16", nullptr},
                                                                                    {"sl-TxParameters-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_cbr_pssch_tx_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_cbr_pssch_tx_cfg_r16_s, sl_cr_limit_r16_present)},
    {1, (int32_t)offsetof(sl_cbr_pssch_tx_cfg_r16_s, sl_tx_params_r16_present)}};
const asn1::table::seq_desc sl_cbr_pssch_tx_cfg_r16_s::desc = {{seq_table_sl_cbr_pssch_tx_cfg_r16_s_fields,
                                                                uint64_t{0x0},
                                                                seq_table_sl_cbr_pssch_tx_cfg_r16_s_opts,
                                                                seq_table_sl_cbr_pssch_tx_cfg_r16_s_names,
                                                                2,
                                                                2},
                                                               false,
                                                               false,
                                                               nullptr,
                                                               0};

// SL-CBR-CommonTxConfigList-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cbr_common_tx_cfg_list_r16_s_fields[] = {
    {offsetof(sl_cbr_common_tx_cfg_list_r16_s, sl_cbr_range_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<
         decltype(sl_cbr_common_tx_cfg_list_r16_s::sl_cbr_range_cfg_list_r16),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 1, 16, false, asn1::table::int_items<false, int64_t{0}, uint64_t{100}, false, false>>>::
         value},
    {offsetof(sl_cbr_common_tx_cfg_list_r16_s, sl_cbr_pssch_tx_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(sl_cbr_common_tx_cfg_list_r16_s::sl_cbr_pssch_tx_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_cbr_pssch_tx_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_cbr_common_tx_cfg_list_r16_s_names[] = {
    {"sl-CBR-RangeConfigList-r16", nullptr},
    {"sl-CBR-PSSCH-TxConfigList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cbr_common_tx_cfg_list_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_cbr_common_tx_cfg_list_r16_s::desc = {{seq_table_sl_cbr_common_tx_cfg_list_r16_s_fields,
                                                                      uint64_t{0x0},
                                                                      seq_table_sl_cbr_common_tx_cfg_list_r16_s_opts,
                                                                      seq_table_sl_cbr_common_tx_cfg_list_r16_s_names,
                                                                      2,
                                                                      2},
                                                                     false,
                                                                     false,
                                                                     nullptr,
                                                                     0};

// SL-CBR-SL-PRS-TxConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cbr_sl_prs_tx_cfg_r18_s_fields[] = {
    {offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_cr_limit_r18),
     (int32_t)offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_cr_limit_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{10000},
     nullptr},
    {offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_max_tx_pwr_r18),
     (int32_t)offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_max_tx_pwr_r18_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_max_num_txs_r18),
     (int32_t)offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_max_num_txs_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_sl_cbr_sl_prs_tx_cfg_r18_s_names[] = {
    {"sl-PRS-CR-Limit-r18", nullptr},
    {"sl-PRS-MaxTx-power-r18", nullptr},
    {"sl-PRS-MaxNumTransmissions-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cbr_sl_prs_tx_cfg_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_cr_limit_r18_present)},
    {1, (int32_t)offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_max_tx_pwr_r18_present)},
    {2, (int32_t)offsetof(sl_cbr_sl_prs_tx_cfg_r18_s, sl_prs_max_num_txs_r18_present)}};
const asn1::table::seq_desc sl_cbr_sl_prs_tx_cfg_r18_s::desc = {{seq_table_sl_cbr_sl_prs_tx_cfg_r18_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_sl_cbr_sl_prs_tx_cfg_r18_s_opts,
                                                                 seq_table_sl_cbr_sl_prs_tx_cfg_r18_s_names,
                                                                 3,
                                                                 3},
                                                                false,
                                                                false,
                                                                nullptr,
                                                                0};

// SL-CBR-CommonTxDedicatedSL-PRS-RP-List-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s_fields[] = {
    {offsetof(sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s, sl_cbr_range_ded_sl_prs_rp_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     6,
     &asn1::table::list_codec<
         decltype(sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s::sl_cbr_range_ded_sl_prs_rp_list_r18),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 0, 15, false, asn1::table::int_items<false, int64_t{0}, uint64_t{100}, false, false>>>::
         value},
    {offsetof(sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s, sl_cbr_sl_prs_tx_cfg_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s::sl_cbr_sl_prs_tx_cfg_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_cbr_sl_prs_tx_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s_names[] = {
    {"sl-CBR-RangeDedicatedSL-PRS-RP-List-r18", nullptr},
    {"sl-CBR-SL-PRS-TxConfigList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s::desc = {
    {seq_table_sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s_fields,
     uint64_t{0x0},
     seq_table_sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s_opts,
     seq_table_sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s_names,
     2,
     2},
    false,
    false,
    nullptr,
    0};

// SL-UE-SelectedConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_ue_sel_cfg_r16_s_fields[] = {
    {offsetof(sl_ue_sel_cfg_r16_s, sl_pssch_tx_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_ue_sel_cfg_r16_s::sl_pssch_tx_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_pssch_tx_cfg_r16_s::desc},
    {offsetof(sl_ue_sel_cfg_r16_s, sl_prob_res_keep_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_prob_res_keep_r16_present),
     asn1::table::enum_op<decltype(sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16)>(),
     decltype(sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16)::has_ext,
     false,
     decltype(sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16)::nof_types,
     decltype(sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16)::nof_exts,
     nullptr,
     &sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_opts::desc},
    {offsetof(sl_ue_sel_cfg_r16_s, sl_reselect_after_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_reselect_after_r16_present),
     asn1::table::enum_op<decltype(sl_ue_sel_cfg_r16_s::sl_reselect_after_r16)>(),
     decltype(sl_ue_sel_cfg_r16_s::sl_reselect_after_r16)::has_ext,
     false,
     decltype(sl_ue_sel_cfg_r16_s::sl_reselect_after_r16)::nof_types,
     decltype(sl_ue_sel_cfg_r16_s::sl_reselect_after_r16)::nof_exts,
     nullptr,
     &sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_opts::desc},
    {offsetof(sl_ue_sel_cfg_r16_s, sl_cbr_common_tx_cfg_list_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_cbr_common_tx_cfg_list_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_cbr_common_tx_cfg_list_r16_s::desc},
    {offsetof(sl_ue_sel_cfg_r16_s, ul_prioritization_thres_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_r16_s, ul_prioritization_thres_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(sl_ue_sel_cfg_r16_s, sl_prioritization_thres_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_prioritization_thres_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr}};
static const asn1::table::field_name seq_table_sl_ue_sel_cfg_r16_s_names[] = {
    {"sl-PSSCH-TxConfigList-r16", nullptr},
    {"sl-ProbResourceKeep-r16", nullptr},
    {"sl-ReselectAfter-r16", nullptr},
    {"sl-CBR-CommonTxConfigList-r16", nullptr},
    {"ul-PrioritizationThres-r16", nullptr},
    {"sl-PrioritizationThres-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_ue_sel_cfg_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_prob_res_keep_r16_present)},
    {2, (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_reselect_after_r16_present)},
    {3, (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_cbr_common_tx_cfg_list_r16_present)},
    {4, (int32_t)offsetof(sl_ue_sel_cfg_r16_s, ul_prioritization_thres_r16_present)},
    {5, (int32_t)offsetof(sl_ue_sel_cfg_r16_s, sl_prioritization_thres_r16_present)}};
static const asn1::table::seq_field seq_table_sl_ue_sel_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_ue_sel_cfg_r16_s, sl_cbr_common_tx_ded_sl_prs_rp_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_ue_sel_cfg_r16_s::sl_cbr_common_tx_ded_sl_prs_rp_list_r18)>::value,
     &sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_ue_sel_cfg_r16_s_g0_names[] = {
    {"sl-CBR-CommonTxDedicatedSL-PRS-RP-List-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_ue_sel_cfg_r16_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_ue_sel_cfg_r16_s_groups[] = {{{seq_table_sl_ue_sel_cfg_r16_s_g0_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_ue_sel_cfg_r16_s_g0_opts,
                                                                                seq_table_sl_ue_sel_cfg_r16_s_g0_names,
                                                                                1,
                                                                                1},
                                                                               false}};
const asn1::table::seq_desc         sl_ue_sel_cfg_r16_s::desc              = {{seq_table_sl_ue_sel_cfg_r16_s_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_sl_ue_sel_cfg_r16_s_opts,
                                                                               seq_table_sl_ue_sel_cfg_r16_s_names,
                                                                               6,
                                                                               6},
                                                                              true,
                                                                              false,
                                                                              seq_table_sl_ue_sel_cfg_r16_s_groups,
                                                                              1};

static const char*           enum_names_sl_ue_sel_cfg_r16_s_sl_prob_res_keep_r16_e__values[] = {"v0",
                                                                                                "v0dot2",
                                                                                                "v0dot4",
                                                                                                "v0dot6",
                                                                                                "v0dot8"};
const asn1::table::enum_desc sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_opts::desc            = {
    enum_names_sl_ue_sel_cfg_r16_s_sl_prob_res_keep_r16_e__values,
    5,
    "sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_e_"};
float sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.2, 0.4, 0.6, 0.8};
  return map_enum_number(numbers, 5, value, "sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_e_");
}
const char* sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(number_strs, 5, value, "sl_ue_sel_cfg_r16_s::sl_prob_res_keep_r16_e_");
}

static const char* enum_names_sl_ue_sel_cfg_r16_s_sl_reselect_after_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n5", "n6", "n7", "n8", "n9"};
const asn1::table::enum_desc sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_opts::desc = {
    enum_names_sl_ue_sel_cfg_r16_s_sl_reselect_after_r16_e__values,
    9,
    "sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_e_"};
uint8_t sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  return map_enum_number(numbers, 9, value, "sl_ue_sel_cfg_r16_s::sl_reselect_after_r16_e_");
}

// SL-PSBCH-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_ps_bch_cfg_r16_s_fields[] = {
    {offsetof(sl_ps_bch_cfg_r16_s, dl_p0_ps_bch_r16),
     (int32_t)offsetof(sl_ps_bch_cfg_r16_s, dl_p0_ps_bch_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-16},
     uint64_t{31},
     nullptr},
    {offsetof(sl_ps_bch_cfg_r16_s, dl_alpha_ps_bch_r16),
     (int32_t)offsetof(sl_ps_bch_cfg_r16_s, dl_alpha_ps_bch_r16_present),
     asn1::table::enum_op<decltype(sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16)>(),
     decltype(sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16)::has_ext,
     false,
     decltype(sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16)::nof_types,
     decltype(sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16)::nof_exts,
     nullptr,
     &sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_ps_bch_cfg_r16_s_names[] = {{"dl-P0-PSBCH-r16", nullptr},
                                                                              {"dl-Alpha-PSBCH-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_ps_bch_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_ps_bch_cfg_r16_s, dl_p0_ps_bch_r16_present)},
    {1, (int32_t)offsetof(sl_ps_bch_cfg_r16_s, dl_alpha_ps_bch_r16_present)}};
static const asn1::table::seq_field seq_table_sl_ps_bch_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_ps_bch_cfg_r16_s, dl_p0_ps_bch_r17),
     (int32_t)offsetof(sl_ps_bch_cfg_r16_s, dl_p0_ps_bch_r17_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     int64_t{-202},
     uint64_t{226},
     nullptr}};
static const asn1::table::field_name seq_table_sl_ps_bch_cfg_r16_s_g0_names[] = {{"dl-P0-PSBCH-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_ps_bch_cfg_r16_s_g0_opts[]  = {
    {0, (int32_t)offsetof(sl_ps_bch_cfg_r16_s, dl_p0_ps_bch_r17_present)}};
static const asn1::table::seq_group seq_table_sl_ps_bch_cfg_r16_s_groups[] = {{{seq_table_sl_ps_bch_cfg_r16_s_g0_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_ps_bch_cfg_r16_s_g0_opts,
                                                                                seq_table_sl_ps_bch_cfg_r16_s_g0_names,
                                                                                1,
                                                                                1},
                                                                               false}};
const asn1::table::seq_desc         sl_ps_bch_cfg_r16_s::desc              = {{seq_table_sl_ps_bch_cfg_r16_s_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_sl_ps_bch_cfg_r16_s_opts,
                                                                               seq_table_sl_ps_bch_cfg_r16_s_names,
                                                                               2,
                                                                               2},
                                                                              true,
                                                                              false,
                                                                              seq_table_sl_ps_bch_cfg_r16_s_groups,
                                                                              1};

static const char* enum_names_sl_ps_bch_cfg_r16_s_dl_alpha_ps_bch_r16_e__values[] =
    {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
const asn1::table::enum_desc sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_opts::desc = {
    enum_names_sl_ps_bch_cfg_r16_s_dl_alpha_ps_bch_r16_e__values,
    8,
    "sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_e_"};
float sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_e_");
}
const char* sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_ps_bch_cfg_r16_s::dl_alpha_ps_bch_r16_e_");
}

// SL-LBT-FailureRecoveryConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_lbt_fail_recovery_cfg_r18_s_fields[] = {
    {offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_fail_instance_max_count_r18),
     (int32_t)offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_fail_instance_max_count_r18_present),
     asn1::table::enum_op<decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18)>(),
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18)::has_ext,
     false,
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18)::nof_types,
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18)::nof_exts,
     nullptr,
     &sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18_opts::desc},
    {offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_fail_detection_timer_r18),
     (int32_t)offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_fail_detection_timer_r18_present),
     asn1::table::enum_op<decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18)>(),
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18)::has_ext,
     false,
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18)::nof_types,
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18)::nof_exts,
     nullptr,
     &sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18_opts::desc},
    {offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_recovery_timer_r18),
     (int32_t)offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_recovery_timer_r18_present),
     asn1::table::enum_op<decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18)>(),
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18)::has_ext,
     false,
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18)::nof_types,
     decltype(sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18)::nof_exts,
     nullptr,
     &sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18_opts::desc}};
static const asn1::table::field_name seq_table_sl_lbt_fail_recovery_cfg_r18_s_names[] = {
    {"sl-LBT-FailureInstanceMaxCount-r18", nullptr},
    {"sl-LBT-FailureDetectionTimer-r18", nullptr},
    {"sl-LBT-RecoveryTimer-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_lbt_fail_recovery_cfg_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_fail_instance_max_count_r18_present)},
    {1, (int32_t)offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_fail_detection_timer_r18_present)},
    {2, (int32_t)offsetof(sl_lbt_fail_recovery_cfg_r18_s, sl_lbt_recovery_timer_r18_present)}};
const asn1::table::seq_desc sl_lbt_fail_recovery_cfg_r18_s::desc = {{seq_table_sl_lbt_fail_recovery_cfg_r18_s_fields,
                                                                     uint64_t{0x0},
                                                                     seq_table_sl_lbt_fail_recovery_cfg_r18_s_opts,
                                                                     seq_table_sl_lbt_fail_recovery_cfg_r18_s_names,
                                                                     3,
                                                                     3},
                                                                    true,
                                                                    false,
                                                                    nullptr,
                                                                    0};

static const char* enum_names_sl_lbt_fail_recovery_cfg_r18_s_sl_lbt_fail_instance_max_count_r18_e__values[] =
    {"n4", "n8", "n16", "n32", "n64", "n128", "spare2", "spare1"};
const asn1::table::enum_desc sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18_opts::desc = {
    enum_names_sl_lbt_fail_recovery_cfg_r18_s_sl_lbt_fail_instance_max_count_r18_e__values,
    8,
    "sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18_e_"};
uint8_t sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16, 32, 64, 128};
  return map_enum_number(numbers, 6, value, "sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_instance_max_count_r18_e_");
}

static const char* enum_names_sl_lbt_fail_recovery_cfg_r18_s_sl_lbt_fail_detection_timer_r18_e__values[] =
    {"ms10", "ms20", "ms40", "ms80", "ms160", "ms320", "spare2", "spare1"};
const asn1::table::enum_desc sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18_opts::desc = {
    enum_names_sl_lbt_fail_recovery_cfg_r18_s_sl_lbt_fail_detection_timer_r18_e__values,
    8,
    "sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18_e_"};
uint16_t sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 40, 80, 160, 320};
  return map_enum_number(numbers, 6, value, "sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_fail_detection_timer_r18_e_");
}

static const char* enum_names_sl_lbt_fail_recovery_cfg_r18_s_sl_lbt_recovery_timer_r18_e__values[] =
    {"ms10", "ms20", "ms40", "ms80", "ms160", "ms320", "spare2", "spare1"};
const asn1::table::enum_desc sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18_opts::desc = {
    enum_names_sl_lbt_fail_recovery_cfg_r18_s_sl_lbt_recovery_timer_r18_e__values,
    8,
    "sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18_e_"};
uint16_t sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 40, 80, 160, 320};
  return map_enum_number(numbers, 6, value, "sl_lbt_fail_recovery_cfg_r18_s::sl_lbt_recovery_timer_r18_e_");
}

// SL-RBSetConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_rb_set_cfg_r18_s_fields[] = {
    {offsetof(sl_rb_set_cfg_r18_s, sl_rb_set_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{4},
     nullptr},
    {offsetof(sl_rb_set_cfg_r18_s, sl_num_of_s_ssb_repeat_r18),
     (int32_t)offsetof(sl_rb_set_cfg_r18_s, sl_num_of_s_ssb_repeat_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{2},
     uint64_t{7},
     nullptr},
    {offsetof(sl_rb_set_cfg_r18_s, sl_gap_between_s_ssb_repeat_r18),
     (int32_t)offsetof(sl_rb_set_cfg_r18_s, sl_gap_between_s_ssb_repeat_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{83},
     nullptr}};
static const asn1::table::field_name seq_table_sl_rb_set_cfg_r18_s_names[] = {
    {"sl-RBSetIndex-r18", nullptr},
    {"sl-NumOfSSSBRepetition-r18", nullptr},
    {"sl-GapBetweenSSSBRepetition-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rb_set_cfg_r18_s_opts[] = {
    {1, (int32_t)offsetof(sl_rb_set_cfg_r18_s, sl_num_of_s_ssb_repeat_r18_present)},
    {2, (int32_t)offsetof(sl_rb_set_cfg_r18_s, sl_gap_between_s_ssb_repeat_r18_present)}};
const asn1::table::seq_desc sl_rb_set_cfg_r18_s::desc = {{seq_table_sl_rb_set_cfg_r18_s_fields,
                                                          uint64_t{0x1},
                                                          seq_table_sl_rb_set_cfg_r18_s_opts,
                                                          seq_table_sl_rb_set_cfg_r18_s_names,
                                                          3,
                                                          2},
                                                         false,
                                                         false,
                                                         nullptr,
                                                         0};

// SL-Unlicensed-r18 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sl_unlicensed_r18_s_sl_lbt_fail_recovery_cfg_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_lbt_fail_recovery_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_unlicensed_r18_s_sl_lbt_fail_recovery_cfg_r18_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_unlicensed_r18_s_sl_lbt_fail_recovery_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_unlicensed_r18_s_sl_lbt_fail_recovery_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_unlicensed_r18_s_sl_lbt_fail_recovery_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_lbt_fail_recovery_cfg_r18_s>::type_offset(),
    setup_release_c<sl_lbt_fail_recovery_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_unlicensed_r18_s_fields[] = {
    {offsetof(sl_unlicensed_r18_s, sl_lbt_fail_recovery_cfg_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_lbt_fail_recovery_cfg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_unlicensed_r18_s_sl_lbt_fail_recovery_cfg_r18_setup_release},
    {offsetof(sl_unlicensed_r18_s, sl_start_symbol_first_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_start_symbol_first_r18_present),
     asn1::table::enum_op<decltype(sl_unlicensed_r18_s::sl_start_symbol_first_r18)>(),
     decltype(sl_unlicensed_r18_s::sl_start_symbol_first_r18)::has_ext,
     false,
     decltype(sl_unlicensed_r18_s::sl_start_symbol_first_r18)::nof_types,
     decltype(sl_unlicensed_r18_s::sl_start_symbol_first_r18)::nof_exts,
     nullptr,
     &sl_unlicensed_r18_s::sl_start_symbol_first_r18_opts::desc},
    {offsetof(sl_unlicensed_r18_s, sl_start_symbol_second_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_start_symbol_second_r18_present),
     asn1::table::enum_op<decltype(sl_unlicensed_r18_s::sl_start_symbol_second_r18)>(),
     decltype(sl_unlicensed_r18_s::sl_start_symbol_second_r18)::has_ext,
     false,
     decltype(sl_unlicensed_r18_s::sl_start_symbol_second_r18)::nof_types,
     decltype(sl_unlicensed_r18_s::sl_start_symbol_second_r18)::nof_exts,
     nullptr,
     &sl_unlicensed_r18_s::sl_start_symbol_second_r18_opts::desc},
    {offsetof(sl_unlicensed_r18_s, sl_tx_structure_for_pscch_and_pssch_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_tx_structure_for_pscch_and_pssch_r18_present),
     asn1::table::enum_op<decltype(sl_unlicensed_r18_s::sl_tx_structure_for_pscch_and_pssch_r18)>(),
     decltype(sl_unlicensed_r18_s::sl_tx_structure_for_pscch_and_pssch_r18)::has_ext,
     false,
     decltype(sl_unlicensed_r18_s::sl_tx_structure_for_pscch_and_pssch_r18)::nof_types,
     decltype(sl_unlicensed_r18_s::sl_tx_structure_for_pscch_and_pssch_r18)::nof_exts,
     nullptr,
     &sl_unlicensed_r18_s::sl_tx_structure_for_pscch_and_pssch_r18_opts::desc},
    {offsetof(sl_unlicensed_r18_s, sl_gap_of_add_s_ssb_occasion_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_gap_of_add_s_ssb_occasion_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{639},
     nullptr},
    {offsetof(sl_unlicensed_r18_s, sl_absolute_freq_ssb_non_anchor_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::list_codec<decltype(sl_unlicensed_r18_s::sl_absolute_freq_ssb_non_anchor_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{3279165}, false, false>>::value},
    {offsetof(sl_unlicensed_r18_s, sl_cp_e_start_position_s_ssb_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_cp_e_start_position_s_ssb_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_unlicensed_r18_s, sl_cws_for_pssch_without_harq_ack_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_cws_for_pssch_without_harq_ack_r18_present),
     asn1::table::enum_op<decltype(sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18)>(),
     decltype(sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18)::has_ext,
     false,
     decltype(sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18)::nof_types,
     decltype(sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18)::nof_exts,
     nullptr,
     &sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18_opts::desc},
    {offsetof(sl_unlicensed_r18_s, sl_num_of_add_s_ssb_occasion_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_num_of_add_s_ssb_occasion_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{4},
     nullptr},
    {offsetof(sl_unlicensed_r18_s, sl_s_ssb_pwr_offset_of_anchor_rb_set_r18),
     (int32_t)offsetof(sl_unlicensed_r18_s, sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_present),
     asn1::table::enum_op<decltype(sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18)>(),
     decltype(sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18)::has_ext,
     false,
     decltype(sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18)::nof_types,
     decltype(sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18)::nof_exts,
     nullptr,
     &sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_opts::desc},
    {offsetof(sl_unlicensed_r18_s, sl_rb_set_cfg_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(sl_unlicensed_r18_s::sl_rb_set_cfg_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_rb_set_cfg_r18_s::desc},
    {offsetof(sl_unlicensed_r18_s, sl_intra_cell_guard_bands_sl_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(sl_unlicensed_r18_s::sl_intra_cell_guard_bands_sl_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &intra_cell_guard_bands_per_scs_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_unlicensed_r18_s_names[] = {
    {"sl-LBT-FailureRecoveryConfig-r18", nullptr},
    {"sl-StartingSymbolFirst-r18", nullptr},
    {"sl-StartingSymbolSecond-r18", nullptr},
    {"sl-TransmissionStructureForPSCCHandPSSCH-r18", nullptr},
    {"sl-GapOfAdditionalSSSB-Occasion-r18", nullptr},
    {"sl-AbsoluteFrequencySSB-NonAnchorList-r18", nullptr},
    {"sl-CPE-StartingPositionS-SSB-r18", nullptr},
    {"sl-CWS-ForPsschWithoutHarqAck-r18", nullptr},
    {"sl-NumOfAdditionalSSSBOccasion-r18", nullptr},
    {"sl-SSSBPowerOffsetOfAnchorRBSet-r18", nullptr},
    {"sl-RBSetConfigList-r18", nullptr},
    {"sl-IntraCellGuardBandsSL-List-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_unlicensed_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_unlicensed_r18_s, sl_lbt_fail_recovery_cfg_r18_present)},
    {1, (int32_t)offsetof(sl_unlicensed_r18_s, sl_start_symbol_first_r18_present)},
    {2, (int32_t)offsetof(sl_unlicensed_r18_s, sl_start_symbol_second_r18_present)},
    {3, (int32_t)offsetof(sl_unlicensed_r18_s, sl_tx_structure_for_pscch_and_pssch_r18_present)},
    {4, (int32_t)offsetof(sl_unlicensed_r18_s, sl_gap_of_add_s_ssb_occasion_r18_present)},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(sl_unlicensed_r18_s, sl_cp_e_start_position_s_ssb_r18_present)},
    {7, (int32_t)offsetof(sl_unlicensed_r18_s, sl_cws_for_pssch_without_harq_ack_r18_present)},
    {8, (int32_t)offsetof(sl_unlicensed_r18_s, sl_num_of_add_s_ssb_occasion_r18_present)},
    {9, (int32_t)offsetof(sl_unlicensed_r18_s, sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_present)},
    {10, asn1::table::seq_field::list_presence},
    {11, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_unlicensed_r18_s::desc = {{seq_table_sl_unlicensed_r18_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_sl_unlicensed_r18_s_opts,
                                                          seq_table_sl_unlicensed_r18_s_names,
                                                          12,
                                                          12},
                                                         false,
                                                         false,
                                                         nullptr,
                                                         0};

static const char* enum_names_sl_unlicensed_r18_s_sl_start_symbol_first_r18_e__values[] =
    {"sym0", "sym1", "sym2", "sym3", "sym4", "sym5", "sym6"};
const asn1::table::enum_desc sl_unlicensed_r18_s::sl_start_symbol_first_r18_opts::desc = {
    enum_names_sl_unlicensed_r18_s_sl_start_symbol_first_r18_e__values,
    7,
    "sl_unlicensed_r18_s::sl_start_symbol_first_r18_e_"};
uint8_t sl_unlicensed_r18_s::sl_start_symbol_first_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(numbers, 7, value, "sl_unlicensed_r18_s::sl_start_symbol_first_r18_e_");
}

static const char*           enum_names_sl_unlicensed_r18_s_sl_start_symbol_second_r18_e__values[] = {"sym3",
                                                                                                      "sym4",
                                                                                                      "sym5",
                                                                                                      "sym6",
                                                                                                      "sym7"};
const asn1::table::enum_desc sl_unlicensed_r18_s::sl_start_symbol_second_r18_opts::desc            = {
    enum_names_sl_unlicensed_r18_s_sl_start_symbol_second_r18_e__values,
    5,
    "sl_unlicensed_r18_s::sl_start_symbol_second_r18_e_"};
uint8_t sl_unlicensed_r18_s::sl_start_symbol_second_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4, 5, 6, 7};
  return map_enum_number(numbers, 5, value, "sl_unlicensed_r18_s::sl_start_symbol_second_r18_e_");
}

static const char* enum_names_sl_unlicensed_r18_s_sl_tx_structure_for_pscch_and_pssch_r18_e__values[] = {"contiguousRB",
                                                                                                         "interlaceRB"};
const asn1::table::enum_desc sl_unlicensed_r18_s::sl_tx_structure_for_pscch_and_pssch_r18_opts::desc  = {
    enum_names_sl_unlicensed_r18_s_sl_tx_structure_for_pscch_and_pssch_r18_e__values,
    2,
    "sl_unlicensed_r18_s::sl_tx_structure_for_pscch_and_pssch_r18_e_"};

static const char*           enum_names_sl_unlicensed_r18_s_sl_cws_for_pssch_without_harq_ack_r18_e__values[] = {"t1",
                                                                                                                 "t8",
                                                                                                                 "t16",
                                                                                                                 "t32",
                                                                                                                 "infinity"};
const asn1::table::enum_desc sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18_opts::desc            = {
    enum_names_sl_unlicensed_r18_s_sl_cws_for_pssch_without_harq_ack_r18_e__values,
    5,
    "sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18_e_"};
int8_t sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18_opts::to_number() const
{
  static const int8_t numbers[] = {1, 8, 16, 32, -1};
  return map_enum_number(numbers, 5, value, "sl_unlicensed_r18_s::sl_cws_for_pssch_without_harq_ack_r18_e_");
}

static const char* enum_names_sl_unlicensed_r18_s_sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_e__values[] = {"value1",
                                                                                                          "value2"};
const asn1::table::enum_desc sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_opts::desc  = {
    enum_names_sl_unlicensed_r18_s_sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_e__values,
    2,
    "sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_e_"};
uint8_t sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_unlicensed_r18_s::sl_s_ssb_pwr_offset_of_anchor_rb_set_r18_e_");
}

// SL-BWP-Generic-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_generic_r16_s_sl_ps_bch_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ps_bch_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_generic_r16_s_sl_ps_bch_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_bwp_generic_r16_s_sl_ps_bch_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_bwp_generic_r16_s_sl_ps_bch_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_sl_bwp_generic_r16_s_sl_ps_bch_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_ps_bch_cfg_r16_s>::type_offset(),
    setup_release_c<sl_ps_bch_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_bwp_generic_r16_s_sl_unlicensed_r18_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_unlicensed_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_generic_r16_s_sl_unlicensed_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_bwp_generic_r16_s_sl_unlicensed_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_bwp_generic_r16_s_sl_unlicensed_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_bwp_generic_r16_s_sl_unlicensed_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_unlicensed_r18_s>::type_offset(),
    setup_release_c<sl_unlicensed_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_bwp_generic_r16_s_fields[] = {
    {offsetof(sl_bwp_generic_r16_s, sl_bwp_r16),
     (int32_t)offsetof(sl_bwp_generic_r16_s, sl_bwp_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc},
    {offsetof(sl_bwp_generic_r16_s, sl_len_symbols_r16),
     (int32_t)offsetof(sl_bwp_generic_r16_s, sl_len_symbols_r16_present),
     asn1::table::enum_op<decltype(sl_bwp_generic_r16_s::sl_len_symbols_r16)>(),
     decltype(sl_bwp_generic_r16_s::sl_len_symbols_r16)::has_ext,
     false,
     decltype(sl_bwp_generic_r16_s::sl_len_symbols_r16)::nof_types,
     decltype(sl_bwp_generic_r16_s::sl_len_symbols_r16)::nof_exts,
     nullptr,
     &sl_bwp_generic_r16_s::sl_len_symbols_r16_opts::desc},
    {offsetof(sl_bwp_generic_r16_s, sl_start_symbol_r16),
     (int32_t)offsetof(sl_bwp_generic_r16_s, sl_start_symbol_r16_present),
     asn1::table::enum_op<decltype(sl_bwp_generic_r16_s::sl_start_symbol_r16)>(),
     decltype(sl_bwp_generic_r16_s::sl_start_symbol_r16)::has_ext,
     false,
     decltype(sl_bwp_generic_r16_s::sl_start_symbol_r16)::nof_types,
     decltype(sl_bwp_generic_r16_s::sl_start_symbol_r16)::nof_exts,
     nullptr,
     &sl_bwp_generic_r16_s::sl_start_symbol_r16_opts::desc},
    {offsetof(sl_bwp_generic_r16_s, sl_ps_bch_cfg_r16),
     (int32_t)offsetof(sl_bwp_generic_r16_s, sl_ps_bch_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_bwp_generic_r16_s_sl_ps_bch_cfg_r16_setup_release},
    {offsetof(sl_bwp_generic_r16_s, sl_tx_direct_current_location_r16),
     (int32_t)offsetof(sl_bwp_generic_r16_s, sl_tx_direct_current_location_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{3301},
     nullptr}};
static const asn1::table::field_name seq_table_sl_bwp_generic_r16_s_names[] = {
    {"sl-BWP-r16", nullptr},
    {"sl-LengthSymbols-r16", nullptr},
    {"sl-StartSymbol-r16", nullptr},
    {"sl-PSBCH-Config-r16", nullptr},
    {"sl-TxDirectCurrentLocation-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_generic_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_bwp_generic_r16_s, sl_bwp_r16_present)},
    {1, (int32_t)offsetof(sl_bwp_generic_r16_s, sl_len_symbols_r16_present)},
    {2, (int32_t)offsetof(sl_bwp_generic_r16_s, sl_start_symbol_r16_present)},
    {3, (int32_t)offsetof(sl_bwp_generic_r16_s, sl_ps_bch_cfg_r16_present)},
    {4, (int32_t)offsetof(sl_bwp_generic_r16_s, sl_tx_direct_current_location_r16_present)}};
static const asn1::table::seq_field seq_table_sl_bwp_generic_r16_s_g0_fields[] = {
    {offsetof(sl_bwp_generic_r16_s, sl_unlicensed_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_bwp_generic_r16_s::sl_unlicensed_r18)>::value,
     &seq_table_sl_bwp_generic_r16_s_sl_unlicensed_r18_setup_release}};
static const asn1::table::field_name seq_table_sl_bwp_generic_r16_s_g0_names[] = {{"sl-Unlicensed-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_bwp_generic_r16_s_g0_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_bwp_generic_r16_s_groups[] = {
    {{seq_table_sl_bwp_generic_r16_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_bwp_generic_r16_s_g0_opts,
      seq_table_sl_bwp_generic_r16_s_g0_names,
      1,
      1},
     false}};
const asn1::table::seq_desc sl_bwp_generic_r16_s::desc = {{seq_table_sl_bwp_generic_r16_s_fields,
                                                           uint64_t{0x0},
                                                           seq_table_sl_bwp_generic_r16_s_opts,
                                                           seq_table_sl_bwp_generic_r16_s_names,
                                                           5,
                                                           5},
                                                          true,
                                                          false,
                                                          seq_table_sl_bwp_generic_r16_s_groups,
                                                          1};

static const char* enum_names_sl_bwp_generic_r16_s_sl_len_symbols_r16_e__values[] =
    {"sym7", "sym8", "sym9", "sym10", "sym11", "sym12", "sym13", "sym14"};
const asn1::table::enum_desc sl_bwp_generic_r16_s::sl_len_symbols_r16_opts::desc = {
    enum_names_sl_bwp_generic_r16_s_sl_len_symbols_r16_e__values,
    8,
    "sl_bwp_generic_r16_s::sl_len_symbols_r16_e_"};
uint8_t sl_bwp_generic_r16_s::sl_len_symbols_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 8, 9, 10, 11, 12, 13, 14};
  return map_enum_number(numbers, 8, value, "sl_bwp_generic_r16_s::sl_len_symbols_r16_e_");
}

static const char* enum_names_sl_bwp_generic_r16_s_sl_start_symbol_r16_e__values[] =
    {"sym0", "sym1", "sym2", "sym3", "sym4", "sym5", "sym6", "sym7"};
const asn1::table::enum_desc sl_bwp_generic_r16_s::sl_start_symbol_r16_opts::desc = {
    enum_names_sl_bwp_generic_r16_s_sl_start_symbol_r16_e__values,
    8,
    "sl_bwp_generic_r16_s::sl_start_symbol_r16_e_"};
uint8_t sl_bwp_generic_r16_s::sl_start_symbol_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(numbers, 8, value, "sl_bwp_generic_r16_s::sl_start_symbol_r16_e_");
}

// SL-PSCCH-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pscch_cfg_r16_s_fields[] = {
    {offsetof(sl_pscch_cfg_r16_s, sl_time_res_pscch_r16),
     (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_time_res_pscch_r16_present),
     asn1::table::enum_op<decltype(sl_pscch_cfg_r16_s::sl_time_res_pscch_r16)>(),
     decltype(sl_pscch_cfg_r16_s::sl_time_res_pscch_r16)::has_ext,
     false,
     decltype(sl_pscch_cfg_r16_s::sl_time_res_pscch_r16)::nof_types,
     decltype(sl_pscch_cfg_r16_s::sl_time_res_pscch_r16)::nof_exts,
     nullptr,
     &sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_opts::desc},
    {offsetof(sl_pscch_cfg_r16_s, sl_freq_res_pscch_r16),
     (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_freq_res_pscch_r16_present),
     asn1::table::enum_op<decltype(sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16)>(),
     decltype(sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16)::has_ext,
     false,
     decltype(sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16)::nof_types,
     decltype(sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16)::nof_exts,
     nullptr,
     &sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_opts::desc},
    {offsetof(sl_pscch_cfg_r16_s, sl_dmrs_scramble_id_r16),
     (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_dmrs_scramble_id_r16_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(sl_pscch_cfg_r16_s, sl_num_reserved_bits_r16),
     (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_num_reserved_bits_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{2},
     uint64_t{2},
     nullptr}};
static const asn1::table::field_name seq_table_sl_pscch_cfg_r16_s_names[] = {{"sl-TimeResourcePSCCH-r16", nullptr},
                                                                             {"sl-FreqResourcePSCCH-r16", nullptr},
                                                                             {"sl-DMRS-ScrambleID-r16", nullptr},
                                                                             {"sl-NumReservedBits-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_pscch_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_time_res_pscch_r16_present)},
    {1, (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_freq_res_pscch_r16_present)},
    {2, (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_dmrs_scramble_id_r16_present)},
    {3, (int32_t)offsetof(sl_pscch_cfg_r16_s, sl_num_reserved_bits_r16_present)}};
const asn1::table::seq_desc sl_pscch_cfg_r16_s::desc = {{seq_table_sl_pscch_cfg_r16_s_fields,
                                                         uint64_t{0x0},
                                                         seq_table_sl_pscch_cfg_r16_s_opts,
                                                         seq_table_sl_pscch_cfg_r16_s_names,
                                                         4,
                                                         4},
                                                        true,
                                                        false,
                                                        nullptr,
                                                        0};

static const char*           enum_names_sl_pscch_cfg_r16_s_sl_time_res_pscch_r16_e__values[] = {"n2", "n3"};
const asn1::table::enum_desc sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_opts::desc            = {
    enum_names_sl_pscch_cfg_r16_s_sl_time_res_pscch_r16_e__values,
    2,
    "sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_e_"};
uint8_t sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_pscch_cfg_r16_s::sl_time_res_pscch_r16_e_");
}

static const char*           enum_names_sl_pscch_cfg_r16_s_sl_freq_res_pscch_r16_e__values[] = {"n10",
                                                                                                "n12",
                                                                                                "n15",
                                                                                                "n20",
                                                                                                "n25"};
const asn1::table::enum_desc sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_opts::desc            = {
    enum_names_sl_pscch_cfg_r16_s_sl_freq_res_pscch_r16_e__values,
    5,
    "sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_e_"};
uint8_t sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 12, 15, 20, 25};
  return map_enum_number(numbers, 5, value, "sl_pscch_cfg_r16_s::sl_freq_res_pscch_r16_e_");
}

// SL-PSSCH-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pssch_cfg_r16_s_fields[] = {
    {offsetof(sl_pssch_cfg_r16_s, sl_pssch_dmrs_time_pattern_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::list_codec<decltype(sl_pssch_cfg_r16_s::sl_pssch_dmrs_time_pattern_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{2}, uint64_t{2}, false, false>>::value},
    {offsetof(sl_pssch_cfg_r16_s, sl_beta_offsets2nd_sci_r16),
     (int32_t)offsetof(sl_pssch_cfg_r16_s, sl_beta_offsets2nd_sci_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     4,
     0,
     &asn1::table::list_codec<decltype(sl_pssch_cfg_r16_s::sl_beta_offsets2nd_sci_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<true, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(sl_pssch_cfg_r16_s, sl_scaling_r16),
     (int32_t)offsetof(sl_pssch_cfg_r16_s, sl_scaling_r16_present),
     asn1::table::enum_op<decltype(sl_pssch_cfg_r16_s::sl_scaling_r16)>(),
     decltype(sl_pssch_cfg_r16_s::sl_scaling_r16)::has_ext,
     false,
     decltype(sl_pssch_cfg_r16_s::sl_scaling_r16)::nof_types,
     decltype(sl_pssch_cfg_r16_s::sl_scaling_r16)::nof_exts,
     nullptr,
     &sl_pssch_cfg_r16_s::sl_scaling_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_pssch_cfg_r16_s_names[] = {
    {"sl-PSSCH-DMRS-TimePatternList-r16", nullptr},
    {"sl-BetaOffsets2ndSCI-r16", nullptr},
    {"sl-Scaling-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pssch_cfg_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_pssch_cfg_r16_s, sl_beta_offsets2nd_sci_r16_present)},
    {2, (int32_t)offsetof(sl_pssch_cfg_r16_s, sl_scaling_r16_present)}};
const asn1::table::seq_desc sl_pssch_cfg_r16_s::desc = {{seq_table_sl_pssch_cfg_r16_s_fields,
                                                         uint64_t{0x0},
                                                         seq_table_sl_pssch_cfg_r16_s_opts,
                                                         seq_table_sl_pssch_cfg_r16_s_names,
                                                         3,
                                                         3},
                                                        true,
                                                        false,
                                                        nullptr,
                                                        0};

static const char*           enum_names_sl_pssch_cfg_r16_s_sl_scaling_r16_e__values[] = {"f0p5", "f0p65", "f0p8", "f1"};
const asn1::table::enum_desc sl_pssch_cfg_r16_s::sl_scaling_r16_opts::desc            = {
    enum_names_sl_pssch_cfg_r16_s_sl_scaling_r16_e__values,
    4,
    "sl_pssch_cfg_r16_s::sl_scaling_r16_e_"};

// SL-PSFCH-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_psfch_cfg_r16_s_fields[] = {
    {offsetof(sl_psfch_cfg_r16_s, sl_psfch_period_r16),
     (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_period_r16_present),
     asn1::table::enum_op<decltype(sl_psfch_cfg_r16_s::sl_psfch_period_r16)>(),
     decltype(sl_psfch_cfg_r16_s::sl_psfch_period_r16)::has_ext,
     false,
     decltype(sl_psfch_cfg_r16_s::sl_psfch_period_r16)::nof_types,
     decltype(sl_psfch_cfg_r16_s::sl_psfch_period_r16)::nof_exts,
     nullptr,
     &sl_psfch_cfg_r16_s::sl_psfch_period_r16_opts::desc},
    {offsetof(sl_psfch_cfg_r16_s, sl_psfch_rb_set_r16),
     (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_rb_set_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_psfch_cfg_r16_s::sl_psfch_rb_set_r16), asn1::table::struct_json>::value},
    {offsetof(sl_psfch_cfg_r16_s, sl_num_mux_cs_pair_r16),
     (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_num_mux_cs_pair_r16_present),
     asn1::table::enum_op<decltype(sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16)>(),
     decltype(sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16)::has_ext,
     false,
     decltype(sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16)::nof_types,
     decltype(sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16)::nof_exts,
     nullptr,
     &sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_opts::desc},
    {offsetof(sl_psfch_cfg_r16_s, sl_min_time_gap_psfch_r16),
     (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_min_time_gap_psfch_r16_present),
     asn1::table::enum_op<decltype(sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16)>(),
     decltype(sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16)::has_ext,
     false,
     decltype(sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16)::nof_types,
     decltype(sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16)::nof_exts,
     nullptr,
     &sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_opts::desc},
    {offsetof(sl_psfch_cfg_r16_s, sl_psfch_hop_id_r16),
     (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_hop_id_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {offsetof(sl_psfch_cfg_r16_s, sl_psfch_candidate_res_type_r16),
     (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_candidate_res_type_r16_present),
     asn1::table::enum_op<decltype(sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16)>(),
     decltype(sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16)::has_ext,
     false,
     decltype(sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16)::nof_types,
     decltype(sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16)::nof_exts,
     nullptr,
     &sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_psfch_cfg_r16_s_names[] = {
    {"sl-PSFCH-Period-r16", nullptr},
    {"sl-PSFCH-RB-Set-r16", nullptr},
    {"sl-NumMuxCS-Pair-r16", nullptr},
    {"sl-MinTimeGapPSFCH-r16", nullptr},
    {"sl-PSFCH-HopID-r16", nullptr},
    {"sl-PSFCH-CandidateResourceType-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_psfch_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_period_r16_present)},
    {1, (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_rb_set_r16_present)},
    {2, (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_num_mux_cs_pair_r16_present)},
    {3, (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_min_time_gap_psfch_r16_present)},
    {4, (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_hop_id_r16_present)},
    {5, (int32_t)offsetof(sl_psfch_cfg_r16_s, sl_psfch_candidate_res_type_r16_present)}};
const asn1::table::seq_desc sl_psfch_cfg_r16_s::desc = {{seq_table_sl_psfch_cfg_r16_s_fields,
                                                         uint64_t{0x0},
                                                         seq_table_sl_psfch_cfg_r16_s_opts,
                                                         seq_table_sl_psfch_cfg_r16_s_names,
                                                         6,
                                                         6},
                                                        true,
                                                        false,
                                                        nullptr,
                                                        0};

static const char* enum_names_sl_psfch_cfg_r16_s_sl_psfch_period_r16_e__values[] = {"sl0", "sl1", "sl2", "sl4"};
const asn1::table::enum_desc sl_psfch_cfg_r16_s::sl_psfch_period_r16_opts::desc  = {
    enum_names_sl_psfch_cfg_r16_s_sl_psfch_period_r16_e__values,
    4,
    "sl_psfch_cfg_r16_s::sl_psfch_period_r16_e_"};
uint8_t sl_psfch_cfg_r16_s::sl_psfch_period_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_psfch_cfg_r16_s::sl_psfch_period_r16_e_");
}

static const char* enum_names_sl_psfch_cfg_r16_s_sl_num_mux_cs_pair_r16_e__values[] = {"n1", "n2", "n3", "n6"};
const asn1::table::enum_desc sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_opts::desc  = {
    enum_names_sl_psfch_cfg_r16_s_sl_num_mux_cs_pair_r16_e__values,
    4,
    "sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_e_"};
uint8_t sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 6};
  return map_enum_number(numbers, 4, value, "sl_psfch_cfg_r16_s::sl_num_mux_cs_pair_r16_e_");
}

static const char*           enum_names_sl_psfch_cfg_r16_s_sl_min_time_gap_psfch_r16_e__values[] = {"sl2", "sl3"};
const asn1::table::enum_desc sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_opts::desc            = {
    enum_names_sl_psfch_cfg_r16_s_sl_min_time_gap_psfch_r16_e__values,
    2,
    "sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_e_"};
uint8_t sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_psfch_cfg_r16_s::sl_min_time_gap_psfch_r16_e_");
}

static const char*           enum_names_sl_psfch_cfg_r16_s_sl_psfch_candidate_res_type_r16_e__values[] = {"startSubCH",
                                                                                                          "allocSubCH"};
const asn1::table::enum_desc sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16_opts::desc            = {
    enum_names_sl_psfch_cfg_r16_s_sl_psfch_candidate_res_type_r16_e__values,
    2,
    "sl_psfch_cfg_r16_s::sl_psfch_candidate_res_type_r16_e_"};

// SL-SyncAllowed-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sync_allowed_r16_s_fields[] = {
    {0,
     (int32_t)offsetof(sl_sync_allowed_r16_s, gnss_sync_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sl_sync_allowed_r16_s, gnb_enb_sync_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sl_sync_allowed_r16_s, ue_sync_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_sync_allowed_r16_s_names[] = {{"gnss-Sync-r16", "true"},
                                                                                {"gnbEnb-Sync-r16", "true"},
                                                                                {"ue-Sync-r16", "true"}};
static const asn1::table::seq_opt    seq_table_sl_sync_allowed_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_sync_allowed_r16_s, gnss_sync_r16_present)},
    {1, (int32_t)offsetof(sl_sync_allowed_r16_s, gnb_enb_sync_r16_present)},
    {2, (int32_t)offsetof(sl_sync_allowed_r16_s, ue_sync_r16_present)}};
const asn1::table::seq_desc sl_sync_allowed_r16_s::desc = {{seq_table_sl_sync_allowed_r16_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_sl_sync_allowed_r16_s_opts,
                                                            seq_table_sl_sync_allowed_r16_s_names,
                                                            3,
                                                            3},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// SL-PTRS-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_ptrs_cfg_r16_s_fields[] = {
    {offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_freq_density_r16),
     (int32_t)offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_freq_density_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     2,
     0,
     &asn1::table::list_codec<decltype(sl_ptrs_cfg_r16_s::sl_ptrs_freq_density_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<true, int64_t{1}, uint64_t{275}, false, false>>::value},
    {offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_time_density_r16),
     (int32_t)offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_time_density_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     3,
     0,
     &asn1::table::list_codec<decltype(sl_ptrs_cfg_r16_s::sl_ptrs_time_density_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<true, int64_t{0}, uint64_t{29}, false, false>>::value},
    {offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_re_offset_r16),
     (int32_t)offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_re_offset_r16_present),
     asn1::table::enum_op<decltype(sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16)>(),
     decltype(sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16)::has_ext,
     false,
     decltype(sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16)::nof_types,
     decltype(sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16)::nof_exts,
     nullptr,
     &sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_ptrs_cfg_r16_s_names[] = {{"sl-PTRS-FreqDensity-r16", nullptr},
                                                                            {"sl-PTRS-TimeDensity-r16", nullptr},
                                                                            {"sl-PTRS-RE-Offset-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_ptrs_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_freq_density_r16_present)},
    {1, (int32_t)offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_time_density_r16_present)},
    {2, (int32_t)offsetof(sl_ptrs_cfg_r16_s, sl_ptrs_re_offset_r16_present)}};
const asn1::table::seq_desc sl_ptrs_cfg_r16_s::desc = {{seq_table_sl_ptrs_cfg_r16_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_sl_ptrs_cfg_r16_s_opts,
                                                        seq_table_sl_ptrs_cfg_r16_s_names,
                                                        3,
                                                        3},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

static const char*           enum_names_sl_ptrs_cfg_r16_s_sl_ptrs_re_offset_r16_e__values[] = {"offset01",
                                                                                               "offset10",
                                                                                               "offset11"};
const asn1::table::enum_desc sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_opts::desc            = {
    enum_names_sl_ptrs_cfg_r16_s_sl_ptrs_re_offset_r16_e__values,
    3,
    "sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_e_"};
float sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_opts::to_number() const
{
  static const float numbers[] = {0.1, 1.0, 1.1};
  return map_enum_number(numbers, 3, value, "sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_e_");
}
const char* sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.1", "1.0", "1.1"};
  return convert_enum_idx(number_strs, 3, value, "sl_ptrs_cfg_r16_s::sl_ptrs_re_offset_r16_e_");
}

// SL-PriorityTxConfigIndex-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prio_tx_cfg_idx_r16_s_fields[] = {
    {offsetof(sl_prio_tx_cfg_idx_r16_s, sl_prio_thres_r16),
     (int32_t)offsetof(sl_prio_tx_cfg_idx_r16_s, sl_prio_thres_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_prio_tx_cfg_idx_r16_s, sl_default_tx_cfg_idx_r16),
     (int32_t)offsetof(sl_prio_tx_cfg_idx_r16_s, sl_default_tx_cfg_idx_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(sl_prio_tx_cfg_idx_r16_s, sl_cbr_cfg_idx_r16),
     (int32_t)offsetof(sl_prio_tx_cfg_idx_r16_s, sl_cbr_cfg_idx_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(sl_prio_tx_cfg_idx_r16_s, sl_tx_cfg_idx_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<decltype(sl_prio_tx_cfg_idx_r16_s::sl_tx_cfg_idx_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{63}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_prio_tx_cfg_idx_r16_s_names[] = {
    {"sl-PriorityThreshold-r16", nullptr},
    {"sl-DefaultTxConfigIndex-r16", nullptr},
    {"sl-CBR-ConfigIndex-r16", nullptr},
    {"sl-Tx-ConfigIndexList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_prio_tx_cfg_idx_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_prio_tx_cfg_idx_r16_s, sl_prio_thres_r16_present)},
    {1, (int32_t)offsetof(sl_prio_tx_cfg_idx_r16_s, sl_default_tx_cfg_idx_r16_present)},
    {2, (int32_t)offsetof(sl_prio_tx_cfg_idx_r16_s, sl_cbr_cfg_idx_r16_present)},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_prio_tx_cfg_idx_r16_s::desc = {{seq_table_sl_prio_tx_cfg_idx_r16_s_fields,
                                                               uint64_t{0x0},
                                                               seq_table_sl_prio_tx_cfg_idx_r16_s_opts,
                                                               seq_table_sl_prio_tx_cfg_idx_r16_s_names,
                                                               4,
                                                               4},
                                                              false,
                                                              false,
                                                              nullptr,
                                                              0};

// SL-SelectionWindowConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sel_win_cfg_r16_s_fields[] = {
    {offsetof(sl_sel_win_cfg_r16_s, sl_prio_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_sel_win_cfg_r16_s, sl_sel_win_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_sel_win_cfg_r16_s::sl_sel_win_r16)>(),
     decltype(sl_sel_win_cfg_r16_s::sl_sel_win_r16)::has_ext,
     false,
     decltype(sl_sel_win_cfg_r16_s::sl_sel_win_r16)::nof_types,
     decltype(sl_sel_win_cfg_r16_s::sl_sel_win_r16)::nof_exts,
     nullptr,
     &sl_sel_win_cfg_r16_s::sl_sel_win_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_sel_win_cfg_r16_s_names[] = {{"sl-Priority-r16", nullptr},
                                                                               {"sl-SelectionWindow-r16", nullptr}};
const asn1::table::seq_desc          sl_sel_win_cfg_r16_s::desc             = {
    {seq_table_sl_sel_win_cfg_r16_s_fields, uint64_t{0x3}, nullptr, seq_table_sl_sel_win_cfg_r16_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

static const char*           enum_names_sl_sel_win_cfg_r16_s_sl_sel_win_r16_e__values[] = {"n1", "n5", "n10", "n20"};
const asn1::table::enum_desc sl_sel_win_cfg_r16_s::sl_sel_win_r16_opts::desc            = {
    enum_names_sl_sel_win_cfg_r16_s_sl_sel_win_r16_e__values,
    4,
    "sl_sel_win_cfg_r16_s::sl_sel_win_r16_e_"};
uint8_t sl_sel_win_cfg_r16_s::sl_sel_win_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 5, 10, 20};
  return map_enum_number(numbers, 4, value, "sl_sel_win_cfg_r16_s::sl_sel_win_r16_e_");
}

// SL-ResourceReservePeriod-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sl_res_reserve_period_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc sl_res_reserve_period_r16_c::storage_desc = {
    choice_storage_sl_res_reserve_period_r16_c_alternatives,
    2,
    "sl_res_reserve_period_r16_c"};

sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_&
sl_res_reserve_period_r16_c::set_sl_res_reserve_period1_r16()
{
  set(types::sl_res_reserve_period1_r16);
  return c.get<sl_res_reserve_period1_r16_e_>();
}
uint8_t& sl_res_reserve_period_r16_c::set_sl_res_reserve_period2_r16()
{
  set(types::sl_res_reserve_period2_r16);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_sl_res_reserve_period_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_>(),
     sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_::has_ext,
     false,
     sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_::nof_types,
     sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_::nof_exts,
     nullptr,
     &sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{98},
     nullptr}};
static const asn1::table::field_name choice_table_sl_res_reserve_period_r16_c_names[] = {
    {"sl-ResourceReservePeriod1-r16", nullptr},
    {"sl-ResourceReservePeriod2-r16", nullptr}};
const asn1::table::choice_desc sl_res_reserve_period_r16_c::desc = {
    asn1::table::enum_int_type<sl_res_reserve_period_r16_c::types>(),
    sl_res_reserve_period_r16_c::types::nof_types,
    sl_res_reserve_period_r16_c::types::nof_exts,
    sl_res_reserve_period_r16_c::types::has_ext,
    false,
    choice_table_sl_res_reserve_period_r16_c_alternatives,
    nullptr,
    choice_table_sl_res_reserve_period_r16_c_names,
    "sl_res_reserve_period_r16_c",
    offsetof(sl_res_reserve_period_r16_c, c.type),
    offsetof(sl_res_reserve_period_r16_c, c.value),
    false,
    &sl_res_reserve_period_r16_c::storage_desc};

static const char* enum_names_sl_res_reserve_period_r16_c_sl_res_reserve_period1_r16_e__values[] =
    {"ms0", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms700", "ms800", "ms900", "ms1000"};
const asn1::table::enum_desc sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_opts::desc = {
    enum_names_sl_res_reserve_period_r16_c_sl_res_reserve_period1_r16_e__values,
    11,
    "sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_"};
uint16_t sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(numbers, 11, value, "sl_res_reserve_period_r16_c::sl_res_reserve_period1_r16_e_");
}

static const char*           enum_names_sl_res_reserve_period_r16_c_types_values[] = {"sl-ResourceReservePeriod1-r16",
                                                                                      "sl-ResourceReservePeriod2-r16"};
const asn1::table::enum_desc sl_res_reserve_period_r16_c::types_opts::desc         = {
    enum_names_sl_res_reserve_period_r16_c_types_values,
    2,
    "sl_res_reserve_period_r16_c::types"};
uint8_t sl_res_reserve_period_r16_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_res_reserve_period_r16_c::types");
}

// SL-PriorityTxConfigIndex-v1650 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prio_tx_cfg_idx_v1650_s_fields[] = {
    {offsetof(sl_prio_tx_cfg_idx_v1650_s, sl_mcs_range_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<decltype(sl_prio_tx_cfg_idx_v1650_s::sl_mcs_range_list_r16),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 1, 3, false, asn1::table::object_items<false>>>::value}};
static const asn1::table::field_name seq_table_sl_prio_tx_cfg_idx_v1650_s_names[] = {{"sl-MCS-RangeList-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_prio_tx_cfg_idx_v1650_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_prio_tx_cfg_idx_v1650_s::desc = {{seq_table_sl_prio_tx_cfg_idx_v1650_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_sl_prio_tx_cfg_idx_v1650_s_opts,
                                                                 seq_table_sl_prio_tx_cfg_idx_v1650_s_names,
                                                                 1,
                                                                 1},
                                                                false,
                                                                false,
                                                                nullptr,
                                                                0};

// SL-UE-SelectedConfigRP-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_ue_sel_cfg_rp_r16_s_fields[] = {
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_cbr_prio_tx_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_ue_sel_cfg_rp_r16_s::sl_cbr_prio_tx_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_prio_tx_cfg_idx_r16_s::desc},
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_thres_rsrp_list_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_thres_rsrp_list_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     64,
     0,
     &asn1::table::list_codec<decltype(sl_ue_sel_cfg_rp_r16_s::sl_thres_rsrp_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<true, int64_t{0}, uint64_t{66}, false, false>>::value},
    {0,
     (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_multi_reserve_res_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_max_num_per_reserve_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_max_num_per_reserve_r16_present),
     asn1::table::enum_op<decltype(sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16)>(),
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16)::has_ext,
     false,
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16)::nof_types,
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16)::nof_exts,
     nullptr,
     &sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_opts::desc},
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_sensing_win_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_sensing_win_r16_present),
     asn1::table::enum_op<decltype(sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16)>(),
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16)::has_ext,
     false,
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16)::nof_types,
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16)::nof_exts,
     nullptr,
     &sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_opts::desc},
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_sel_win_list_r16),
     (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_sel_win_list_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     8,
     0,
     &asn1::table::item_list_codec<decltype(sl_ue_sel_cfg_rp_r16_s::sl_sel_win_list_r16),
                                   asn1::table::sequence_item_ops,
                                   true,
                                   true>::value,
     &sl_sel_win_cfg_r16_s::desc},
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_res_reserve_period_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_ue_sel_cfg_rp_r16_s::sl_res_reserve_period_list_r16),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &sl_res_reserve_period_r16_c::desc},
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_rs_for_sensing_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16)>(),
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16)::has_ext,
     false,
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16)::nof_types,
     decltype(sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16)::nof_exts,
     nullptr,
     &sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_ue_sel_cfg_rp_r16_s_names[] = {
    {"sl-CBR-PriorityTxConfigList-r16", nullptr},
    {"sl-Thres-RSRP-List-r16", nullptr},
    {"sl-MultiReserveResource-r16", "enabled"},
    {"sl-MaxNumPerReserve-r16", nullptr},
    {"sl-SensingWindow-r16", nullptr},
    {"sl-SelectionWindowList-r16", nullptr},
    {"sl-ResourceReservePeriodList-r16", nullptr},
    {"sl-RS-ForSensing-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_ue_sel_cfg_rp_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_thres_rsrp_list_r16_present)},
    {2, (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_multi_reserve_res_r16_present)},
    {3, (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_max_num_per_reserve_r16_present)},
    {4, (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_sensing_win_r16_present)},
    {5, (int32_t)offsetof(sl_ue_sel_cfg_rp_r16_s, sl_sel_win_list_r16_present)},
    {6, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_sl_ue_sel_cfg_rp_r16_s_g0_fields[] = {
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_cbr_prio_tx_cfg_list_v1650),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<decltype(sl_ue_sel_cfg_rp_r16_s::sl_cbr_prio_tx_cfg_list_v1650),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_prio_tx_cfg_idx_v1650_s::desc}};
static const asn1::table::field_name seq_table_sl_ue_sel_cfg_rp_r16_s_g0_names[] = {
    {"sl-CBR-PriorityTxConfigList-v1650", nullptr}};
static const asn1::table::seq_opt seq_table_sl_ue_sel_cfg_rp_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_ue_sel_cfg_rp_r16_s_g1_fields[] = {
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_nr_pssch_eutra_thres_rsrp_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     64,
     0,
     &asn1::table::ptr_list_codec<decltype(sl_ue_sel_cfg_rp_r16_s::sl_nr_pssch_eutra_thres_rsrp_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<true, int64_t{0}, uint64_t{66}, false, false>>::value},
    {offsetof(sl_ue_sel_cfg_rp_r16_s, sl_nr_psfch_eutra_thres_rsrp_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     64,
     0,
     &asn1::table::ptr_list_codec<decltype(sl_ue_sel_cfg_rp_r16_s::sl_nr_psfch_eutra_thres_rsrp_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<true, int64_t{0}, uint64_t{66}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_ue_sel_cfg_rp_r16_s_g1_names[] = {
    {"sl-NRPSSCH-EUTRA-ThresRSRP-List-r18", nullptr},
    {"sl-NRPSFCH-EUTRA-ThresRSRP-List-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_ue_sel_cfg_rp_r16_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_ue_sel_cfg_rp_r16_s_groups[] = {
    {{seq_table_sl_ue_sel_cfg_rp_r16_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_ue_sel_cfg_rp_r16_s_g0_opts,
      seq_table_sl_ue_sel_cfg_rp_r16_s_g0_names,
      1,
      1},
     false},
    {{seq_table_sl_ue_sel_cfg_rp_r16_s_g1_fields,
      uint64_t{0x0},
      seq_table_sl_ue_sel_cfg_rp_r16_s_g1_opts,
      seq_table_sl_ue_sel_cfg_rp_r16_s_g1_names,
      2,
      2},
     false}};
const asn1::table::seq_desc sl_ue_sel_cfg_rp_r16_s::desc = {{seq_table_sl_ue_sel_cfg_rp_r16_s_fields,
                                                             uint64_t{0x80},
                                                             seq_table_sl_ue_sel_cfg_rp_r16_s_opts,
                                                             seq_table_sl_ue_sel_cfg_rp_r16_s_names,
                                                             8,
                                                             7},
                                                            true,
                                                            false,
                                                            seq_table_sl_ue_sel_cfg_rp_r16_s_groups,
                                                            2};

static const char*           enum_names_sl_ue_sel_cfg_rp_r16_s_sl_max_num_per_reserve_r16_e__values[] = {"n2", "n3"};
const asn1::table::enum_desc sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_opts::desc            = {
    enum_names_sl_ue_sel_cfg_rp_r16_s_sl_max_num_per_reserve_r16_e__values,
    2,
    "sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_e_"};
uint8_t sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_ue_sel_cfg_rp_r16_s::sl_max_num_per_reserve_r16_e_");
}

static const char*           enum_names_sl_ue_sel_cfg_rp_r16_s_sl_sensing_win_r16_e__values[] = {"ms100", "ms1100"};
const asn1::table::enum_desc sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_opts::desc            = {
    enum_names_sl_ue_sel_cfg_rp_r16_s_sl_sensing_win_r16_e__values,
    2,
    "sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_e_"};
uint16_t sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 1100};
  return map_enum_number(numbers, 2, value, "sl_ue_sel_cfg_rp_r16_s::sl_sensing_win_r16_e_");
}

static const char*           enum_names_sl_ue_sel_cfg_rp_r16_s_sl_rs_for_sensing_r16_e__values[] = {"pscch", "pssch"};
const asn1::table::enum_desc sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16_opts::desc            = {
    enum_names_sl_ue_sel_cfg_rp_r16_s_sl_rs_for_sensing_r16_e__values,
    2,
    "sl_ue_sel_cfg_rp_r16_s::sl_rs_for_sensing_r16_e_"};

// SL-ZoneConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_zone_cfg_r16_s_fields[] = {
    {offsetof(sl_zone_cfg_r16_s, sl_zone_len_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_zone_cfg_r16_s::sl_zone_len_r16)>(),
     decltype(sl_zone_cfg_r16_s::sl_zone_len_r16)::has_ext,
     false,
     decltype(sl_zone_cfg_r16_s::sl_zone_len_r16)::nof_types,
     decltype(sl_zone_cfg_r16_s::sl_zone_len_r16)::nof_exts,
     nullptr,
     &sl_zone_cfg_r16_s::sl_zone_len_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_zone_cfg_r16_s_names[] = {{"sl-ZoneLength-r16", nullptr}};
const asn1::table::seq_desc          sl_zone_cfg_r16_s::desc             = {
    {seq_table_sl_zone_cfg_r16_s_fields, uint64_t{0x1}, nullptr, seq_table_sl_zone_cfg_r16_s_names, 1, 0},
    true,
    false,
    nullptr,
    0};

static const char* enum_names_sl_zone_cfg_r16_s_sl_zone_len_r16_e__values[] =
    {"m5", "m10", "m20", "m30", "m40", "m50", "spare2", "spare1"};
const asn1::table::enum_desc sl_zone_cfg_r16_s::sl_zone_len_r16_opts::desc = {
    enum_names_sl_zone_cfg_r16_s_sl_zone_len_r16_e__values,
    8,
    "sl_zone_cfg_r16_s::sl_zone_len_r16_e_"};
uint8_t sl_zone_cfg_r16_s::sl_zone_len_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 30, 40, 50};
  return map_enum_number(numbers, 6, value, "sl_zone_cfg_r16_s::sl_zone_len_r16_e_");
}

// SL-ZoneConfigMCR-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_zone_cfg_mcr_r16_s_fields[] = {
    {offsetof(sl_zone_cfg_mcr_r16_s, sl_zone_cfg_mcr_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(sl_zone_cfg_mcr_r16_s, sl_trans_range_r16),
     (int32_t)offsetof(sl_zone_cfg_mcr_r16_s, sl_trans_range_r16_present),
     asn1::table::enum_op<decltype(sl_zone_cfg_mcr_r16_s::sl_trans_range_r16)>(),
     decltype(sl_zone_cfg_mcr_r16_s::sl_trans_range_r16)::has_ext,
     false,
     decltype(sl_zone_cfg_mcr_r16_s::sl_trans_range_r16)::nof_types,
     decltype(sl_zone_cfg_mcr_r16_s::sl_trans_range_r16)::nof_exts,
     nullptr,
     &sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_opts::desc},
    {offsetof(sl_zone_cfg_mcr_r16_s, sl_zone_cfg_r16),
     (int32_t)offsetof(sl_zone_cfg_mcr_r16_s, sl_zone_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_zone_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_zone_cfg_mcr_r16_s_names[] = {{"sl-ZoneConfigMCR-Index-r16", nullptr},
                                                                                {"sl-TransRange-r16", nullptr},
                                                                                {"sl-ZoneConfig-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_zone_cfg_mcr_r16_s_opts[]  = {
    {1, (int32_t)offsetof(sl_zone_cfg_mcr_r16_s, sl_trans_range_r16_present)},
    {2, (int32_t)offsetof(sl_zone_cfg_mcr_r16_s, sl_zone_cfg_r16_present)}};
const asn1::table::seq_desc sl_zone_cfg_mcr_r16_s::desc = {{seq_table_sl_zone_cfg_mcr_r16_s_fields,
                                                            uint64_t{0x1},
                                                            seq_table_sl_zone_cfg_mcr_r16_s_opts,
                                                            seq_table_sl_zone_cfg_mcr_r16_s_names,
                                                            3,
                                                            2},
                                                           true,
                                                           false,
                                                           nullptr,
                                                           0};

static const char* enum_names_sl_zone_cfg_mcr_r16_s_sl_trans_range_r16_e__values[] = {
    "m20",   "m50",    "m80",    "m100",   "m120",   "m150",   "m180",   "m200",   "m220",   "m250",  "m270",
    "m300",  "m350",   "m370",   "m400",   "m420",   "m450",   "m480",   "m500",   "m550",   "m600",  "m700",
    "m1000", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_opts::desc = {
    enum_names_sl_zone_cfg_mcr_r16_s_sl_trans_range_r16_e__values,
    32,
    "sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_e_"};
uint16_t sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {20,  50,  80,  100, 120, 150, 180, 200, 220, 250, 270, 300,
                                     350, 370, 400, 420, 450, 480, 500, 550, 600, 700, 1000};
  return map_enum_number(numbers, 23, value, "sl_zone_cfg_mcr_r16_s::sl_trans_range_r16_e_");
}

// FilterCoefficient ::= ENUMERATED
static const char*           enum_names_filt_coef_e_values[] = {"fc0",
                                                                "fc1",
                                                                "fc2",
                                                                "fc3",
                                                                "fc4",
                                                                "fc5",
                                                                "fc6",
                                                                "fc7",
                                                                "fc8",
                                                                "fc9",
                                                                "fc11",
                                                                "fc13",
                                                                "fc15",
                                                                "fc17",
                                                                "fc19",
                                                                "spare1"};
const asn1::table::enum_desc filt_coef_opts::desc            = {enum_names_filt_coef_e_values, 16, "filt_coef_e"};
uint8_t                      filt_coef_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19};
  return map_enum_number(numbers, 15, value, "filt_coef_e");
}

// SL-PowerControl-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pwr_ctrl_r16_s_fields[] = {
    {offsetof(sl_pwr_ctrl_r16_s, sl_max_trans_pwr_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(sl_pwr_ctrl_r16_s, sl_alpha_pssch_pscch_r16),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, sl_alpha_pssch_pscch_r16_present),
     asn1::table::enum_op<decltype(sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16)>(),
     decltype(sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16)::has_ext,
     false,
     decltype(sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16)::nof_types,
     decltype(sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16)::nof_exts,
     nullptr,
     &sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_opts::desc},
    {offsetof(sl_pwr_ctrl_r16_s, dl_alpha_pssch_pscch_r16),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_alpha_pssch_pscch_r16_present),
     asn1::table::enum_op<decltype(sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16)>(),
     decltype(sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16)::has_ext,
     false,
     decltype(sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16)::nof_types,
     decltype(sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16)::nof_exts,
     nullptr,
     &sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_opts::desc},
    {offsetof(sl_pwr_ctrl_r16_s, sl_p0_pssch_pscch_r16),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, sl_p0_pssch_pscch_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-16},
     uint64_t{31},
     nullptr},
    {offsetof(sl_pwr_ctrl_r16_s, dl_p0_pssch_pscch_r16),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_pssch_pscch_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-16},
     uint64_t{31},
     nullptr},
    {offsetof(sl_pwr_ctrl_r16_s, dl_alpha_psfch_r16),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_alpha_psfch_r16_present),
     asn1::table::enum_op<decltype(sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16)>(),
     decltype(sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16)::has_ext,
     false,
     decltype(sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16)::nof_types,
     decltype(sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16)::nof_exts,
     nullptr,
     &sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_opts::desc},
    {offsetof(sl_pwr_ctrl_r16_s, dl_p0_psfch_r16),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_psfch_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-16},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_sl_pwr_ctrl_r16_s_names[] = {{"sl-MaxTransPower-r16", nullptr},
                                                                            {"sl-Alpha-PSSCH-PSCCH-r16", nullptr},
                                                                            {"dl-Alpha-PSSCH-PSCCH-r16", nullptr},
                                                                            {"sl-P0-PSSCH-PSCCH-r16", nullptr},
                                                                            {"dl-P0-PSSCH-PSCCH-r16", nullptr},
                                                                            {"dl-Alpha-PSFCH-r16", nullptr},
                                                                            {"dl-P0-PSFCH-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_pwr_ctrl_r16_s_opts[]  = {
    {1, (int32_t)offsetof(sl_pwr_ctrl_r16_s, sl_alpha_pssch_pscch_r16_present)},
    {2, (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_alpha_pssch_pscch_r16_present)},
    {3, (int32_t)offsetof(sl_pwr_ctrl_r16_s, sl_p0_pssch_pscch_r16_present)},
    {4, (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_pssch_pscch_r16_present)},
    {5, (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_alpha_psfch_r16_present)},
    {6, (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_psfch_r16_present)}};
static const asn1::table::seq_field seq_table_sl_pwr_ctrl_r16_s_g0_fields[] = {
    {offsetof(sl_pwr_ctrl_r16_s, dl_p0_pssch_pscch_r17),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_pssch_pscch_r17_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     int64_t{-202},
     uint64_t{226},
     nullptr},
    {offsetof(sl_pwr_ctrl_r16_s, sl_p0_pssch_pscch_r17),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, sl_p0_pssch_pscch_r17_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     int64_t{-202},
     uint64_t{226},
     nullptr},
    {offsetof(sl_pwr_ctrl_r16_s, dl_p0_psfch_r17),
     (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_psfch_r17_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     int64_t{-202},
     uint64_t{226},
     nullptr}};
static const asn1::table::field_name seq_table_sl_pwr_ctrl_r16_s_g0_names[] = {{"dl-P0-PSSCH-PSCCH-r17", nullptr},
                                                                               {"sl-P0-PSSCH-PSCCH-r17", nullptr},
                                                                               {"dl-P0-PSFCH-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_pwr_ctrl_r16_s_g0_opts[]  = {
    {0, (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_pssch_pscch_r17_present)},
    {1, (int32_t)offsetof(sl_pwr_ctrl_r16_s, sl_p0_pssch_pscch_r17_present)},
    {2, (int32_t)offsetof(sl_pwr_ctrl_r16_s, dl_p0_psfch_r17_present)}};
static const asn1::table::seq_group seq_table_sl_pwr_ctrl_r16_s_groups[] = {{{seq_table_sl_pwr_ctrl_r16_s_g0_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_sl_pwr_ctrl_r16_s_g0_opts,
                                                                              seq_table_sl_pwr_ctrl_r16_s_g0_names,
                                                                              3,
                                                                              3},
                                                                             false}};
const asn1::table::seq_desc         sl_pwr_ctrl_r16_s::desc              = {{seq_table_sl_pwr_ctrl_r16_s_fields,
                                                                             uint64_t{0x1},
                                                                             seq_table_sl_pwr_ctrl_r16_s_opts,
                                                                             seq_table_sl_pwr_ctrl_r16_s_names,
                                                                             7,
                                                                             6},
                                                                            true,
                                                                            false,
                                                                            seq_table_sl_pwr_ctrl_r16_s_groups,
                                                                            1};

static const char* enum_names_sl_pwr_ctrl_r16_s_sl_alpha_pssch_pscch_r16_e__values[] =
    {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
const asn1::table::enum_desc sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_opts::desc = {
    enum_names_sl_pwr_ctrl_r16_s_sl_alpha_pssch_pscch_r16_e__values,
    8,
    "sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_e_"};
float sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_e_");
}
const char* sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_pwr_ctrl_r16_s::sl_alpha_pssch_pscch_r16_e_");
}

static const char* enum_names_sl_pwr_ctrl_r16_s_dl_alpha_pssch_pscch_r16_e__values[] =
    {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
const asn1::table::enum_desc sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_opts::desc = {
    enum_names_sl_pwr_ctrl_r16_s_dl_alpha_pssch_pscch_r16_e__values,
    8,
    "sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_e_"};
float sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_e_");
}
const char* sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_pssch_pscch_r16_e_");
}

static const char* enum_names_sl_pwr_ctrl_r16_s_dl_alpha_psfch_r16_e__values[] =
    {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
const asn1::table::enum_desc sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_opts::desc = {
    enum_names_sl_pwr_ctrl_r16_s_dl_alpha_psfch_r16_e__values,
    8,
    "sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_e_"};
float sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_e_");
}
const char* sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_pwr_ctrl_r16_s::dl_alpha_psfch_r16_e_");
}

// SL-TxPercentageConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_tx_percentage_cfg_r16_s_fields[] = {
    {offsetof(sl_tx_percentage_cfg_r16_s, sl_prio_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_tx_percentage_cfg_r16_s, sl_tx_percentage_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16)>(),
     decltype(sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16)::has_ext,
     false,
     decltype(sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16)::nof_types,
     decltype(sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16)::nof_exts,
     nullptr,
     &sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_tx_percentage_cfg_r16_s_names[] = {{"sl-Priority-r16", nullptr},
                                                                                     {"sl-TxPercentage-r16", nullptr}};
const asn1::table::seq_desc          sl_tx_percentage_cfg_r16_s::desc = {{seq_table_sl_tx_percentage_cfg_r16_s_fields,
                                                                          uint64_t{0x3},
                                                                          nullptr,
                                                                          seq_table_sl_tx_percentage_cfg_r16_s_names,
                                                                          2,
                                                                          0},
                                                                         false,
                                                                         false,
                                                                         nullptr,
                                                                         0};

static const char* enum_names_sl_tx_percentage_cfg_r16_s_sl_tx_percentage_r16_e__values[] = {"p20", "p35", "p50"};
const asn1::table::enum_desc sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_opts::desc  = {
    enum_names_sl_tx_percentage_cfg_r16_s_sl_tx_percentage_r16_e__values,
    3,
    "sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_e_"};
uint8_t sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 35, 50};
  return map_enum_number(numbers, 3, value, "sl_tx_percentage_cfg_r16_s::sl_tx_percentage_r16_e_");
}

// SL-PBPS-CPS-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pbps_cp_s_cfg_r17_s_fields[] = {
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_allowed_res_sel_cfg_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_allowed_res_sel_cfg_r17_present),
     asn1::table::enum_op<decltype(sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17)>(),
     decltype(sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17)::has_ext,
     false,
     decltype(sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17)::nof_types,
     decltype(sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17)::nof_exts,
     nullptr,
     &sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_opts::desc},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_candidate_slots_periodic_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_candidate_slots_periodic_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_pbps_occasion_reserve_period_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<decltype(sl_pbps_cp_s_cfg_r17_s::sl_pbps_occasion_reserve_period_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{15}, false, false>>::value},
    {0,
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_add_pbps_occasion_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_cp_s_win_periodic_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_cp_s_win_periodic_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{5},
     uint64_t{25},
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_candidate_slots_aperiodic_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_candidate_slots_aperiodic_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_rssi_meas_slots_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_rssi_meas_slots_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{799},
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_default_cbr_random_sel_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_default_cbr_random_sel_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{100},
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_default_cbr_partial_sensing_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_default_cbr_partial_sensing_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{100},
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_cp_s_win_aperiodic_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_cp_s_win_aperiodic_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr},
    {offsetof(sl_pbps_cp_s_cfg_r17_s, sl_partial_sensing_inactive_time_r17),
     (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_partial_sensing_inactive_time_r17_present),
     asn1::table::enum_op<decltype(sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17)>(),
     decltype(sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17)::has_ext,
     false,
     decltype(sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17)::nof_types,
     decltype(sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17)::nof_exts,
     nullptr,
     &sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17_opts::desc}};
static const asn1::table::field_name seq_table_sl_pbps_cp_s_cfg_r17_s_names[] = {
    {"sl-AllowedResourceSelectionConfig-r17", nullptr},
    {"sl-MinNumCandidateSlotsPeriodic-r17", nullptr},
    {"sl-PBPS-OccasionReservePeriodList-r17", nullptr},
    {"sl-Additional-PBPS-Occasion-r17", "monitored"},
    {"sl-CPS-WindowPeriodic-r17", nullptr},
    {"sl-MinNumCandidateSlotsAperiodic-r17", nullptr},
    {"sl-MinNumRssiMeasurementSlots-r17", nullptr},
    {"sl-DefaultCBR-RandomSelection-r17", nullptr},
    {"sl-DefaultCBR-PartialSensing-r17", nullptr},
    {"sl-CPS-WindowAperiodic-r17", nullptr},
    {"sl-PartialSensingInactiveTime-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pbps_cp_s_cfg_r17_s_opts[] = {
    {0, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_allowed_res_sel_cfg_r17_present)},
    {1, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_candidate_slots_periodic_r17_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_add_pbps_occasion_r17_present)},
    {4, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_cp_s_win_periodic_r17_present)},
    {5, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_candidate_slots_aperiodic_r17_present)},
    {6, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_min_num_rssi_meas_slots_r17_present)},
    {7, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_default_cbr_random_sel_r17_present)},
    {8, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_default_cbr_partial_sensing_r17_present)},
    {9, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_cp_s_win_aperiodic_r17_present)},
    {10, (int32_t)offsetof(sl_pbps_cp_s_cfg_r17_s, sl_partial_sensing_inactive_time_r17_present)}};
const asn1::table::seq_desc sl_pbps_cp_s_cfg_r17_s::desc = {{seq_table_sl_pbps_cp_s_cfg_r17_s_fields,
                                                             uint64_t{0x0},
                                                             seq_table_sl_pbps_cp_s_cfg_r17_s_opts,
                                                             seq_table_sl_pbps_cp_s_cfg_r17_s_names,
                                                             11,
                                                             11},
                                                            true,
                                                            false,
                                                            nullptr,
                                                            0};

static const char* enum_names_sl_pbps_cp_s_cfg_r17_s_sl_allowed_res_sel_cfg_r17_e__values[] =
    {"c1", "c2", "c3", "c4", "c5", "c6", "c7"};
const asn1::table::enum_desc sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_opts::desc = {
    enum_names_sl_pbps_cp_s_cfg_r17_s_sl_allowed_res_sel_cfg_r17_e__values,
    7,
    "sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_e_"};
uint8_t sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(numbers, 7, value, "sl_pbps_cp_s_cfg_r17_s::sl_allowed_res_sel_cfg_r17_e_");
}

static const char* enum_names_sl_pbps_cp_s_cfg_r17_s_sl_partial_sensing_inactive_time_r17_e__values[] = {"enabled",
                                                                                                         "disabled"};
const asn1::table::enum_desc sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17_opts::desc  = {
    enum_names_sl_pbps_cp_s_cfg_r17_s_sl_partial_sensing_inactive_time_r17_e__values,
    2,
    "sl_pbps_cp_s_cfg_r17_s::sl_partial_sensing_inactive_time_r17_e_"};

// SL-ThresholdRSRP-Condition1-B-1-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_thres_rsrp_condition1_b_1_r17_s_fields[] = {
    {offsetof(sl_thres_rsrp_condition1_b_1_r17_s, sl_prio_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_thres_rsrp_condition1_b_1_r17_s, sl_thres_rsrp_condition1_b_1_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{66},
     nullptr}};
static const asn1::table::field_name seq_table_sl_thres_rsrp_condition1_b_1_r17_s_names[] = {
    {"sl-Priority-r17", nullptr},
    {"sl-ThresholdRSRP-Condition1-B-1-r17", nullptr}};
const asn1::table::seq_desc sl_thres_rsrp_condition1_b_1_r17_s::desc = {
    {seq_table_sl_thres_rsrp_condition1_b_1_r17_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_sl_thres_rsrp_condition1_b_1_r17_s_names,
     2,
     0},
    false,
    false,
    nullptr,
    0};

// SL-InterUE-CoordinationScheme1-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_inter_ue_coordination_scheme1_r17_s_fields[] = {
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_iuc_explicit_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_iuc_explicit_r17_present),
     asn1::table::enum_op<decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17)>(),
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17)::has_ext,
     false,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17)::nof_types,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17)::nof_exts,
     nullptr,
     &sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17_opts::desc},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_iuc_condition_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_iuc_condition_r17_present),
     asn1::table::enum_op<decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17)>(),
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17)::has_ext,
     false,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17)::nof_types,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17)::nof_exts,
     nullptr,
     &sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17_opts::desc},
    {0,
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_condition1_a_2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_thres_rsrp_condition1_b_1_option1_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<
         decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_thres_rsrp_condition1_b_1_option1_list_r17),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &sl_thres_rsrp_condition1_b_1_r17_s::desc},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_thres_rsrp_condition1_b_1_option2_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<
         decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_thres_rsrp_condition1_b_1_option2_list_r17),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &sl_thres_rsrp_condition1_b_1_r17_s::desc},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_container_coord_info_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_container_coord_info_r17_present),
     asn1::table::enum_op<decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17)>(),
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17)::has_ext,
     false,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17)::nof_types,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17)::nof_exts,
     nullptr,
     &sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17_opts::desc},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_container_request_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_container_request_r17_present),
     asn1::table::enum_op<decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17)>(),
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17)::has_ext,
     false,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17)::nof_types,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17)::nof_exts,
     nullptr,
     &sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17_opts::desc},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_trigger_condition_coord_info_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_trigger_condition_coord_info_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{1},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_trigger_condition_request_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_trigger_condition_request_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{1},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_coord_info_explicit_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_coord_info_explicit_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_coord_info_condition_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_coord_info_condition_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_request_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_request_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_preferred_res_set_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_preferred_res_set_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_max_slot_offset_triv_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_max_slot_offset_triv_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{7999},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_num_sub_ch_preferred_res_set_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_num_sub_ch_preferred_res_set_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{26},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_reserved_period_preferred_res_set_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_reserved_period_preferred_res_set_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_determine_res_type_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_determine_res_type_r17_present),
     asn1::table::enum_op<decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17)>(),
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17)::has_ext,
     false,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17)::nof_types,
     decltype(sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17)::nof_exts,
     nullptr,
     &sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17_opts::desc}};
static const asn1::table::field_name seq_table_sl_inter_ue_coordination_scheme1_r17_s_names[] = {
    {"sl-IUC-Explicit-r17", nullptr},
    {"sl-IUC-Condition-r17", nullptr},
    {"sl-Condition1-A-2-r17", "disabled"},
    {"sl-ThresholdRSRP-Condition1-B-1-Option1List-r17", nullptr},
    {"sl-ThresholdRSRP-Condition1-B-1-Option2List-r17", nullptr},
    {"sl-ContainerCoordInfo-r17", nullptr},
    {"sl-ContainerRequest-r17", nullptr},
    {"sl-TriggerConditionCoordInfo-r17", nullptr},
    {"sl-TriggerConditionRequest-r17", nullptr},
    {"sl-PriorityCoordInfoExplicit-r17", nullptr},
    {"sl-PriorityCoordInfoCondition-r17", nullptr},
    {"sl-PriorityRequest-r17", nullptr},
    {"sl-PriorityPreferredResourceSet-r17", nullptr},
    {"sl-MaxSlotOffsetTRIV-r17", nullptr},
    {"sl-NumSubCH-PreferredResourceSet-r17", nullptr},
    {"sl-ReservedPeriodPreferredResourceSet-r17", nullptr},
    {"sl-DetermineResourceType-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_inter_ue_coordination_scheme1_r17_s_opts[] = {
    {0, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_iuc_explicit_r17_present)},
    {1, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_iuc_condition_r17_present)},
    {2, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_condition1_a_2_r17_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_container_coord_info_r17_present)},
    {6, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_container_request_r17_present)},
    {7, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_trigger_condition_coord_info_r17_present)},
    {8, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_trigger_condition_request_r17_present)},
    {9, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_coord_info_explicit_r17_present)},
    {10, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_coord_info_condition_r17_present)},
    {11, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_request_r17_present)},
    {12, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_prio_preferred_res_set_r17_present)},
    {13, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_max_slot_offset_triv_r17_present)},
    {14, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_num_sub_ch_preferred_res_set_r17_present)},
    {15, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_reserved_period_preferred_res_set_r17_present)},
    {16, (int32_t)offsetof(sl_inter_ue_coordination_scheme1_r17_s, sl_determine_res_type_r17_present)}};
const asn1::table::seq_desc sl_inter_ue_coordination_scheme1_r17_s::desc = {
    {seq_table_sl_inter_ue_coordination_scheme1_r17_s_fields,
     uint64_t{0x0},
     seq_table_sl_inter_ue_coordination_scheme1_r17_s_opts,
     seq_table_sl_inter_ue_coordination_scheme1_r17_s_names,
     17,
     17},
    true,
    false,
    nullptr,
    0};

static const char* enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_iuc_explicit_r17_e__values[] = {"enabled",
                                                                                                        "disabled"};
const asn1::table::enum_desc sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17_opts::desc  = {
    enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_iuc_explicit_r17_e__values,
    2,
    "sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_explicit_r17_e_"};

static const char* enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_iuc_condition_r17_e__values[] = {"enabled",
                                                                                                         "disabled"};
const asn1::table::enum_desc sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17_opts::desc  = {
    enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_iuc_condition_r17_e__values,
    2,
    "sl_inter_ue_coordination_scheme1_r17_s::sl_iuc_condition_r17_e_"};

static const char* enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_container_coord_info_r17_e__values[] = {
    "enabled",
    "disabled"};
const asn1::table::enum_desc sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17_opts::desc = {
    enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_container_coord_info_r17_e__values,
    2,
    "sl_inter_ue_coordination_scheme1_r17_s::sl_container_coord_info_r17_e_"};

static const char* enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_container_request_r17_e__values[] = {
    "enabled",
    "disabled"};
const asn1::table::enum_desc sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17_opts::desc = {
    enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_container_request_r17_e__values,
    2,
    "sl_inter_ue_coordination_scheme1_r17_s::sl_container_request_r17_e_"};

static const char* enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_determine_res_type_r17_e__values[] = {"uea",
                                                                                                              "ueb"};
const asn1::table::enum_desc sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17_opts::desc  = {
    enum_names_sl_inter_ue_coordination_scheme1_r17_s_sl_determine_res_type_r17_e__values,
    2,
    "sl_inter_ue_coordination_scheme1_r17_s::sl_determine_res_type_r17_e_"};

// SL-InterUE-CoordinationScheme2-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_inter_ue_coordination_scheme2_r17_s_fields[] = {
    {0,
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_iuc_scheme2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_rb_set_psfch_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_rb_set_psfch_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_inter_ue_coordination_scheme2_r17_s::sl_rb_set_psfch_r17),
                                asn1::table::struct_json>::value},
    {0,
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_type_ue_a_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_psfch_occasion_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_psfch_occasion_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{1},
     nullptr},
    {0,
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_slot_level_res_exclusion_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_option_for_condition2_a_1_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_option_for_condition2_a_1_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{1},
     nullptr},
    {offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_ind_ue_b_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_ind_ue_b_r17_present),
     asn1::table::enum_op<decltype(sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17)>(),
     decltype(sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17)::has_ext,
     false,
     decltype(sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17)::nof_types,
     decltype(sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17)::nof_exts,
     nullptr,
     &sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17_opts::desc}};
static const asn1::table::field_name seq_table_sl_inter_ue_coordination_scheme2_r17_s_names[] = {
    {"sl-IUC-Scheme2-r17", "enabled"},
    {"sl-RB-SetPSFCH-r17", nullptr},
    {"sl-TypeUE-A-r17", "enabled"},
    {"sl-PSFCH-Occasion-r17", nullptr},
    {"sl-SlotLevelResourceExclusion-r17", "enabled"},
    {"sl-OptionForCondition2-A-1-r17", nullptr},
    {"sl-IndicationUE-B-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_inter_ue_coordination_scheme2_r17_s_opts[] = {
    {0, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_iuc_scheme2_r17_present)},
    {1, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_rb_set_psfch_r17_present)},
    {2, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_type_ue_a_r17_present)},
    {3, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_psfch_occasion_r17_present)},
    {4, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_slot_level_res_exclusion_r17_present)},
    {5, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_option_for_condition2_a_1_r17_present)},
    {6, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_ind_ue_b_r17_present)}};
static const asn1::table::seq_field seq_table_sl_inter_ue_coordination_scheme2_r17_s_g0_fields[] = {
    {offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_delta_rsrp_thresh_v1720),
     (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_delta_rsrp_thresh_v1720_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{60},
     nullptr}};
static const asn1::table::field_name seq_table_sl_inter_ue_coordination_scheme2_r17_s_g0_names[] = {
    {"sl-DeltaRSRP-Thresh-v1720", nullptr}};
static const asn1::table::seq_opt seq_table_sl_inter_ue_coordination_scheme2_r17_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_inter_ue_coordination_scheme2_r17_s, sl_delta_rsrp_thresh_v1720_present)}};
static const asn1::table::seq_group seq_table_sl_inter_ue_coordination_scheme2_r17_s_groups[] = {
    {{seq_table_sl_inter_ue_coordination_scheme2_r17_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_inter_ue_coordination_scheme2_r17_s_g0_opts,
      seq_table_sl_inter_ue_coordination_scheme2_r17_s_g0_names,
      1,
      1},
     false}};
const asn1::table::seq_desc sl_inter_ue_coordination_scheme2_r17_s::desc = {
    {seq_table_sl_inter_ue_coordination_scheme2_r17_s_fields,
     uint64_t{0x0},
     seq_table_sl_inter_ue_coordination_scheme2_r17_s_opts,
     seq_table_sl_inter_ue_coordination_scheme2_r17_s_names,
     7,
     7},
    true,
    false,
    seq_table_sl_inter_ue_coordination_scheme2_r17_s_groups,
    1};

static const char*           enum_names_sl_inter_ue_coordination_scheme2_r17_s_sl_ind_ue_b_r17_e__values[] = {"enabled",
                                                                                                              "disabled"};
const asn1::table::enum_desc sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17_opts::desc            = {
    enum_names_sl_inter_ue_coordination_scheme2_r17_s_sl_ind_ue_b_r17_e__values,
    2,
    "sl_inter_ue_coordination_scheme2_r17_s::sl_ind_ue_b_r17_e_"};

// SL-InterUE-CoordinationConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_inter_ue_coordination_cfg_r17_s_fields[] = {
    {offsetof(sl_inter_ue_coordination_cfg_r17_s, sl_inter_ue_coordination_scheme1_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_cfg_r17_s, sl_inter_ue_coordination_scheme1_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_inter_ue_coordination_scheme1_r17_s::desc},
    {offsetof(sl_inter_ue_coordination_cfg_r17_s, sl_inter_ue_coordination_scheme2_r17),
     (int32_t)offsetof(sl_inter_ue_coordination_cfg_r17_s, sl_inter_ue_coordination_scheme2_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_inter_ue_coordination_scheme2_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_inter_ue_coordination_cfg_r17_s_names[] = {
    {"sl-InterUE-CoordinationScheme1-r17", nullptr},
    {"sl-InterUE-CoordinationScheme2-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_inter_ue_coordination_cfg_r17_s_opts[] = {
    {0, (int32_t)offsetof(sl_inter_ue_coordination_cfg_r17_s, sl_inter_ue_coordination_scheme1_r17_present)},
    {1, (int32_t)offsetof(sl_inter_ue_coordination_cfg_r17_s, sl_inter_ue_coordination_scheme2_r17_present)}};
const asn1::table::seq_desc sl_inter_ue_coordination_cfg_r17_s::desc = {
    {seq_table_sl_inter_ue_coordination_cfg_r17_s_fields,
     uint64_t{0x0},
     seq_table_sl_inter_ue_coordination_cfg_r17_s_opts,
     seq_table_sl_inter_ue_coordination_cfg_r17_s_names,
     2,
     2},
    true,
    false,
    nullptr,
    0};

// SL-CPE-StartingPositionsPSCCH-PSSCH-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cp_e_start_positions_pscch_pssch_r18_s_fields[] = {
    {offsetof(sl_cp_e_start_positions_pscch_pssch_r18_s, sl_prio_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_cp_e_start_positions_pscch_pssch_r18_s, sl_cp_e_start_positions_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(sl_cp_e_start_positions_pscch_pssch_r18_s::sl_cp_e_start_positions_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{8}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_cp_e_start_positions_pscch_pssch_r18_s_names[] = {
    {"sl-Priority-r18", nullptr},
    {"sl-CPE-StartingPositions-r18", nullptr}};
const asn1::table::seq_desc sl_cp_e_start_positions_pscch_pssch_r18_s::desc = {
    {seq_table_sl_cp_e_start_positions_pscch_pssch_r18_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_sl_cp_e_start_positions_pscch_pssch_r18_s_names,
     2,
     0},
    false,
    false,
    nullptr,
    0};

// SL-PRS-ResourceSharedSL-PRS-RP-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prs_res_shared_sl_prs_rp_r18_s_fields[] = {
    {offsetof(sl_prs_res_shared_sl_prs_rp_r18_s, sl_prs_res_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{16},
     nullptr},
    {offsetof(sl_prs_res_shared_sl_prs_rp_r18_s, mnof_symbols_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_prs_res_shared_sl_prs_rp_r18_s, sl_prs_comb_size_n_and_re_offset_r18),
     (int32_t)offsetof(sl_prs_res_shared_sl_prs_rp_r18_s, sl_prs_comb_size_n_and_re_offset_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::desc}};
static const asn1::table::field_name seq_table_sl_prs_res_shared_sl_prs_rp_r18_s_names[] = {
    {"sl-PRS-ResourceID-r18", nullptr},
    {"mNumberOfSymbols-r18", nullptr},
    {"sl-PRS-CombSizeN-AndReOffset-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_prs_res_shared_sl_prs_rp_r18_s_opts[] = {
    {2, (int32_t)offsetof(sl_prs_res_shared_sl_prs_rp_r18_s, sl_prs_comb_size_n_and_re_offset_r18_present)}};
const asn1::table::seq_desc sl_prs_res_shared_sl_prs_rp_r18_s::desc = {
    {seq_table_sl_prs_res_shared_sl_prs_rp_r18_s_fields,
     uint64_t{0x3},
     seq_table_sl_prs_res_shared_sl_prs_rp_r18_s_opts,
     seq_table_sl_prs_res_shared_sl_prs_rp_r18_s_names,
     3,
     1},
    false,
    false,
    nullptr,
    0};

static const asn1::table::storage_ops* const
    choice_storage_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc
    sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::storage_desc = {
        choice_storage_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__alternatives,
        3,
        "sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_"};

uint8_t& sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::set_n2_r18()
{
  set(types::n2_r18);
  return c.get<uint8_t>();
}
uint8_t& sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::set_n4_r18()
{
  set(types::n4_r18);
  return c.get<uint8_t>();
}
uint8_t& sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::set_dummy1()
{
  set(types::dummy1);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field
    choice_table_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{1},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{3},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{5},
         nullptr}};
static const asn1::table::field_name
    choice_table_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__names[] = {
        {"n2-r18", nullptr},
        {"n4-r18", nullptr},
        {"dummy1", nullptr}};
const asn1::table::choice_desc sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::desc = {
    asn1::table::enum_int_type<sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types>(),
    sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types::nof_types,
    sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types::nof_exts,
    sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types::has_ext,
    false,
    choice_table_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__alternatives,
    nullptr,
    choice_table_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__names,
    "sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_",
    offsetof(sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_, c.type),
    offsetof(sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_, c.value),
    false,
    &sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::storage_desc};

static const char* enum_names_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__types_values[] =
    {"n2-r18", "n4-r18", "dummy1"};
const asn1::table::enum_desc
    sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types_opts::desc = {
        enum_names_sl_prs_res_shared_sl_prs_rp_r18_s_sl_prs_comb_size_n_and_re_offset_r18_c__types_values,
        3,
        "sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types"};
uint8_t sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 1};
  return map_enum_number(
      numbers, 3, value, "sl_prs_res_shared_sl_prs_rp_r18_s::sl_prs_comb_size_n_and_re_offset_r18_c_::types");
}

// SL-ResourcePool-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s__fields[] = {
    {offsetof(sl_res_pool_r16_s::sl_rx_params_ncell_r16_s_, sl_tdd_cfg_r16),
     (int32_t)offsetof(sl_res_pool_r16_s::sl_rx_params_ncell_r16_s_, sl_tdd_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_cfg_common_s::desc},
    {offsetof(sl_res_pool_r16_s::sl_rx_params_ncell_r16_s_, sl_sync_cfg_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s__names[] = {
    {"sl-TDD-Configuration-r16", nullptr},
    {"sl-SyncConfigIndex-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_res_pool_r16_s::sl_rx_params_ncell_r16_s_, sl_tdd_cfg_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s_ = {
    {seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s__fields,
     uint64_t{0x2},
     seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s__opts,
     seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s__names,
     2,
     1},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_sl_pscch_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pscch_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_sl_pscch_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_res_pool_r16_s_sl_pscch_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_res_pool_r16_s_sl_pscch_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_sl_res_pool_r16_s_sl_pscch_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_pscch_cfg_r16_s>::type_offset(),
    setup_release_c<sl_pscch_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_sl_pssch_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pssch_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_sl_pssch_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_res_pool_r16_s_sl_pssch_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_res_pool_r16_s_sl_pssch_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_sl_res_pool_r16_s_sl_pssch_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_pssch_cfg_r16_s>::type_offset(),
    setup_release_c<sl_pssch_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_sl_psfch_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_psfch_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_sl_psfch_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_res_pool_r16_s_sl_psfch_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_res_pool_r16_s_sl_psfch_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_sl_res_pool_r16_s_sl_psfch_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_psfch_cfg_r16_s>::type_offset(),
    setup_release_c<sl_psfch_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_sl_pbps_cp_s_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pbps_cp_s_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_sl_pbps_cp_s_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_res_pool_r16_s_sl_pbps_cp_s_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_res_pool_r16_s_sl_pbps_cp_s_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sl_res_pool_r16_s_sl_pbps_cp_s_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_pbps_cp_s_cfg_r17_s>::type_offset(),
    setup_release_c<sl_pbps_cp_s_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_sl_res_pool_r16_s_sl_inter_ue_coordination_cfg_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_inter_ue_coordination_cfg_r17_s::desc}};
static const asn1::table::field_name
    seq_table_sl_res_pool_r16_s_sl_inter_ue_coordination_cfg_r17_setup_release_names[] = {{"release", nullptr},
                                                                                          {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_res_pool_r16_s_sl_inter_ue_coordination_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_res_pool_r16_s_sl_inter_ue_coordination_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sl_res_pool_r16_s_sl_inter_ue_coordination_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_inter_ue_coordination_cfg_r17_s>::type_offset(),
    setup_release_c<sl_inter_ue_coordination_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18_setup_release_alternatives[] =
        {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
         {0,
          asn1::table::seq_field::mandatory,
          asn1::table::field_op::codec,
          false,
          false,
          8,
          0,
          &asn1::table::item_list_codec<setup_release_c<sl_cp_e_start_positions_pscch_pssch_list_r18_l>::value_type,
                                        asn1::table::sequence_item_ops,
                                        true,
                                        true>::value,
          &sl_cp_e_start_positions_pscch_pssch_r18_s::desc}};
static const asn1::table::field_name
    seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18_setup_release_names[] = {
        {"release", nullptr},
        {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18_setup_release_alternatives,
        nullptr,
        seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<sl_cp_e_start_positions_pscch_pssch_list_r18_l>::type_offset(),
        setup_release_c<sl_cp_e_start_positions_pscch_pssch_list_r18_l>::value_offset()};
static const asn1::table::seq_field
    seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         8,
         0,
         &asn1::table::item_list_codec<setup_release_c<sl_cp_e_start_positions_pscch_pssch_list_r18_l>::value_type,
                                       asn1::table::sequence_item_ops,
                                       true,
                                       true>::value,
         &sl_cp_e_start_positions_pscch_pssch_r18_s::desc}};
static const asn1::table::field_name
    seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18_setup_release_names[] = {
        {"release", nullptr},
        {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18_setup_release_alternatives,
        nullptr,
        seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<sl_cp_e_start_positions_pscch_pssch_list_r18_l>::type_offset(),
        setup_release_c<sl_cp_e_start_positions_pscch_pssch_list_r18_l>::value_offset()};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_fields[] = {
    {offsetof(sl_res_pool_r16_s, sl_pscch_cfg_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_pscch_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_res_pool_r16_s_sl_pscch_cfg_r16_setup_release},
    {offsetof(sl_res_pool_r16_s, sl_pssch_cfg_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_pssch_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_res_pool_r16_s_sl_pssch_cfg_r16_setup_release},
    {offsetof(sl_res_pool_r16_s, sl_psfch_cfg_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_psfch_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_res_pool_r16_s_sl_psfch_cfg_r16_setup_release},
    {offsetof(sl_res_pool_r16_s, sl_sync_allowed_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_sync_allowed_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_sync_allowed_r16_s::desc},
    {offsetof(sl_res_pool_r16_s, sl_subch_size_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_subch_size_r16_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_subch_size_r16)>(),
     decltype(sl_res_pool_r16_s::sl_subch_size_r16)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_subch_size_r16)::nof_types,
     decltype(sl_res_pool_r16_s::sl_subch_size_r16)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_subch_size_r16_opts::desc},
    {offsetof(sl_res_pool_r16_s, dummy),
     (int32_t)offsetof(sl_res_pool_r16_s, dummy_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{10},
     uint64_t{150},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_start_rb_subch_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_start_rb_subch_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{265},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_num_subch_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_num_subch_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{26},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_add_mcs_table_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_add_mcs_table_r16_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_add_mcs_table_r16)>(),
     decltype(sl_res_pool_r16_s::sl_add_mcs_table_r16)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_add_mcs_table_r16)::nof_types,
     decltype(sl_res_pool_r16_s::sl_add_mcs_table_r16)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_add_mcs_table_r16_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_thresh_s_rssi_cbr_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_thresh_s_rssi_cbr_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{45},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_time_win_size_cbr_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_time_win_size_cbr_r16_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_time_win_size_cbr_r16)>(),
     decltype(sl_res_pool_r16_s::sl_time_win_size_cbr_r16)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_time_win_size_cbr_r16)::nof_types,
     decltype(sl_res_pool_r16_s::sl_time_win_size_cbr_r16)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_time_win_size_cbr_r16_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_time_win_size_cr_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_time_win_size_cr_r16_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_time_win_size_cr_r16)>(),
     decltype(sl_res_pool_r16_s::sl_time_win_size_cr_r16)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_time_win_size_cr_r16)::nof_types,
     decltype(sl_res_pool_r16_s::sl_time_win_size_cr_r16)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_time_win_size_cr_r16_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_ptrs_cfg_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_ptrs_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ptrs_cfg_r16_s::desc},
    {offsetof(sl_res_pool_r16_s, sl_ue_sel_cfg_rp_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_ue_sel_cfg_rp_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ue_sel_cfg_rp_r16_s::desc},
    {offsetof(sl_res_pool_r16_s, sl_rx_params_ncell_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_rx_params_ncell_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_res_pool_r16_s_sl_rx_params_ncell_r16_s_},
    {offsetof(sl_res_pool_r16_s, sl_zone_cfg_mcr_list_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_zone_cfg_mcr_list_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     16,
     0,
     &asn1::table::item_list_codec<decltype(sl_res_pool_r16_s::sl_zone_cfg_mcr_list_r16),
                                   asn1::table::sequence_item_ops,
                                   true,
                                   true>::value,
     &sl_zone_cfg_mcr_r16_s::desc},
    {offsetof(sl_res_pool_r16_s, sl_filt_coef_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_filt_coef_r16_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_filt_coef_r16)>(),
     decltype(sl_res_pool_r16_s::sl_filt_coef_r16)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_filt_coef_r16)::nof_types,
     decltype(sl_res_pool_r16_s::sl_filt_coef_r16)::nof_exts,
     nullptr,
     &filt_coef_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_rb_num_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_rb_num_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{10},
     uint64_t{265},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_preemption_enable_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_preemption_enable_r16_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_preemption_enable_r16)>(),
     decltype(sl_res_pool_r16_s::sl_preemption_enable_r16)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_preemption_enable_r16)::nof_types,
     decltype(sl_res_pool_r16_s::sl_preemption_enable_r16)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_preemption_enable_r16_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_prio_thres_ul_urllc_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_prio_thres_ul_urllc_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_prio_thres_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_prio_thres_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_x_overhead_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_x_overhead_r16_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_x_overhead_r16)>(),
     decltype(sl_res_pool_r16_s::sl_x_overhead_r16)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_x_overhead_r16)::nof_types,
     decltype(sl_res_pool_r16_s::sl_x_overhead_r16)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_x_overhead_r16_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_pwr_ctrl_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_pwr_ctrl_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pwr_ctrl_r16_s::desc},
    {offsetof(sl_res_pool_r16_s, sl_tx_percentage_list_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_tx_percentage_list_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     8,
     0,
     &asn1::table::item_list_codec<decltype(sl_res_pool_r16_s::sl_tx_percentage_list_r16),
                                   asn1::table::sequence_item_ops,
                                   true,
                                   true>::value,
     &sl_tx_percentage_cfg_r16_s::desc},
    {offsetof(sl_res_pool_r16_s, sl_min_max_mcs_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::item_list_codec<decltype(sl_res_pool_r16_s::sl_min_max_mcs_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_min_max_mcs_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_names[] = {
    {"sl-PSCCH-Config-r16", nullptr},         {"sl-PSSCH-Config-r16", nullptr},
    {"sl-PSFCH-Config-r16", nullptr},         {"sl-SyncAllowed-r16", nullptr},
    {"sl-SubchannelSize-r16", nullptr},       {"dummy", nullptr},
    {"sl-StartRB-Subchannel-r16", nullptr},   {"sl-NumSubchannel-r16", nullptr},
    {"sl-Additional-MCS-Table-r16", nullptr}, {"sl-ThreshS-RSSI-CBR-r16", nullptr},
    {"sl-TimeWindowSizeCBR-r16", nullptr},    {"sl-TimeWindowSizeCR-r16", nullptr},
    {"sl-PTRS-Config-r16", nullptr},          {"sl-UE-SelectedConfigRP-r16", nullptr},
    {"sl-RxParametersNcell-r16", nullptr},    {"sl-ZoneConfigMCR-List-r16", nullptr},
    {"sl-FilterCoefficient-r16", nullptr},    {"sl-RB-Number-r16", nullptr},
    {"sl-PreemptionEnable-r16", nullptr},     {"sl-PriorityThreshold-UL-URLLC-r16", nullptr},
    {"sl-PriorityThreshold-r16", nullptr},    {"sl-X-Overhead-r16", nullptr},
    {"sl-PowerControl-r16", nullptr},         {"sl-TxPercentageList-r16", nullptr},
    {"sl-MinMaxMCS-List-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_res_pool_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_res_pool_r16_s, sl_pscch_cfg_r16_present)},
    {1, (int32_t)offsetof(sl_res_pool_r16_s, sl_pssch_cfg_r16_present)},
    {2, (int32_t)offsetof(sl_res_pool_r16_s, sl_psfch_cfg_r16_present)},
    {3, (int32_t)offsetof(sl_res_pool_r16_s, sl_sync_allowed_r16_present)},
    {4, (int32_t)offsetof(sl_res_pool_r16_s, sl_subch_size_r16_present)},
    {5, (int32_t)offsetof(sl_res_pool_r16_s, dummy_present)},
    {6, (int32_t)offsetof(sl_res_pool_r16_s, sl_start_rb_subch_r16_present)},
    {7, (int32_t)offsetof(sl_res_pool_r16_s, sl_num_subch_r16_present)},
    {8, (int32_t)offsetof(sl_res_pool_r16_s, sl_add_mcs_table_r16_present)},
    {9, (int32_t)offsetof(sl_res_pool_r16_s, sl_thresh_s_rssi_cbr_r16_present)},
    {10, (int32_t)offsetof(sl_res_pool_r16_s, sl_time_win_size_cbr_r16_present)},
    {11, (int32_t)offsetof(sl_res_pool_r16_s, sl_time_win_size_cr_r16_present)},
    {12, (int32_t)offsetof(sl_res_pool_r16_s, sl_ptrs_cfg_r16_present)},
    {13, (int32_t)offsetof(sl_res_pool_r16_s, sl_ue_sel_cfg_rp_r16_present)},
    {14, (int32_t)offsetof(sl_res_pool_r16_s, sl_rx_params_ncell_r16_present)},
    {15, (int32_t)offsetof(sl_res_pool_r16_s, sl_zone_cfg_mcr_list_r16_present)},
    {16, (int32_t)offsetof(sl_res_pool_r16_s, sl_filt_coef_r16_present)},
    {17, (int32_t)offsetof(sl_res_pool_r16_s, sl_rb_num_r16_present)},
    {18, (int32_t)offsetof(sl_res_pool_r16_s, sl_preemption_enable_r16_present)},
    {19, (int32_t)offsetof(sl_res_pool_r16_s, sl_prio_thres_ul_urllc_r16_present)},
    {20, (int32_t)offsetof(sl_res_pool_r16_s, sl_prio_thres_r16_present)},
    {21, (int32_t)offsetof(sl_res_pool_r16_s, sl_x_overhead_r16_present)},
    {22, (int32_t)offsetof(sl_res_pool_r16_s, sl_pwr_ctrl_r16_present)},
    {23, (int32_t)offsetof(sl_res_pool_r16_s, sl_tx_percentage_list_r16_present)},
    {24, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_g0_fields[] = {
    {offsetof(sl_res_pool_r16_s, sl_time_res_r16),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_time_res_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_res_pool_r16_s::sl_time_res_r16), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_g0_names[] = {{"sl-TimeResource-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_res_pool_r16_s_g0_opts[]  = {
    {0, (int32_t)offsetof(sl_res_pool_r16_s, sl_time_res_r16_present)}};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_g1_fields[] = {
    {offsetof(sl_res_pool_r16_s, sl_pbps_cp_s_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_res_pool_r16_s::sl_pbps_cp_s_cfg_r17)>::value,
     &seq_table_sl_res_pool_r16_s_sl_pbps_cp_s_cfg_r17_setup_release},
    {offsetof(sl_res_pool_r16_s, sl_inter_ue_coordination_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_res_pool_r16_s::sl_inter_ue_coordination_cfg_r17)>::value,
     &seq_table_sl_res_pool_r16_s_sl_inter_ue_coordination_cfg_r17_setup_release}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_g1_names[] = {
    {"sl-PBPS-CPS-Config-r17", nullptr},
    {"sl-InterUE-CoordinationConfig-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_sl_res_pool_r16_s_g1_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                             {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_res_pool_r16_s_g2_fields[] = {
    {offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<
         decltype(sl_res_pool_r16_s::sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18)>::value,
     &seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_initiate_cot_list_r18_setup_release},
    {offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_initiate_cot_default_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_initiate_cot_default_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<
         decltype(sl_res_pool_r16_s::sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18)>::value,
     &seq_table_sl_res_pool_r16_s_sl_cp_e_start_positions_pscch_pssch_within_cot_list_r18_setup_release},
    {offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_within_cot_default_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_within_cot_default_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {0,
     (int32_t)offsetof(sl_res_pool_r16_s, sl_type1_lbt_blocking_option1_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sl_res_pool_r16_s, sl_type1_lbt_blocking_option2_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_num_interlace_per_subch_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_num_interlace_per_subch_r18_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_num_interlace_per_subch_r18)>(),
     decltype(sl_res_pool_r16_s::sl_num_interlace_per_subch_r18)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_num_interlace_per_subch_r18)::nof_types,
     decltype(sl_res_pool_r16_s::sl_num_interlace_per_subch_r18)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_num_interlace_per_subch_r18_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_num_ref_prbs_of_interlace_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_num_ref_prbs_of_interlace_r18_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18)>(),
     decltype(sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18)::nof_types,
     decltype(sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_tx_structure_for_psfch_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_tx_structure_for_psfch_r18_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_tx_structure_for_psfch_r18)>(),
     decltype(sl_res_pool_r16_s::sl_tx_structure_for_psfch_r18)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_tx_structure_for_psfch_r18)::nof_types,
     decltype(sl_res_pool_r16_s::sl_tx_structure_for_psfch_r18)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_tx_structure_for_psfch_r18_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_num_ded_prbs_for_psfch_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_num_ded_prbs_for_psfch_r18_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18)>(),
     decltype(sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18)::nof_types,
     decltype(sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_num_psfch_occasions_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_num_psfch_occasions_r18_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_num_psfch_occasions_r18)>(),
     decltype(sl_res_pool_r16_s::sl_num_psfch_occasions_r18)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_num_psfch_occasions_r18)::nof_types,
     decltype(sl_res_pool_r16_s::sl_num_psfch_occasions_r18)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_num_psfch_occasions_r18_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_psfch_common_interlace_idx_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_psfch_common_interlace_idx_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{9},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_cp_e_start_position_psfch_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_cp_e_start_position_psfch_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_num_ref_symbol_len_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_num_ref_symbol_len_r18_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_num_ref_symbol_len_r18)>(),
     decltype(sl_res_pool_r16_s::sl_num_ref_symbol_len_r18)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_num_ref_symbol_len_r18)::nof_types,
     decltype(sl_res_pool_r16_s::sl_num_ref_symbol_len_r18)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_num_ref_symbol_len_r18_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_psfch_rb_set_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::ptr_list_codec<decltype(sl_res_pool_r16_s::sl_psfch_rb_set_list_r18),
                                  asn1::table::struct_json>::value},
    {offsetof(sl_res_pool_r16_s, sl_iuc_rb_set_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::ptr_list_codec<decltype(sl_res_pool_r16_s::sl_iuc_rb_set_list_r18),
                                  asn1::table::struct_json>::value},
    {offsetof(sl_res_pool_r16_s, sl_psfch_pwr_offset_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_psfch_pwr_offset_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{10},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_rb_set_idx_of_res_pool_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::ptr_list_codec<decltype(sl_res_pool_r16_s::sl_rb_set_idx_of_res_pool_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{4}, false, false>>::value},
    {offsetof(sl_res_pool_r16_s, sl_a2_x_service_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_a2_x_service_r18_present),
     asn1::table::enum_op<decltype(sl_res_pool_r16_s::sl_a2_x_service_r18)>(),
     decltype(sl_res_pool_r16_s::sl_a2_x_service_r18)::has_ext,
     false,
     decltype(sl_res_pool_r16_s::sl_a2_x_service_r18)::nof_types,
     decltype(sl_res_pool_r16_s::sl_a2_x_service_r18)::nof_exts,
     nullptr,
     &sl_res_pool_r16_s::sl_a2_x_service_r18_opts::desc},
    {offsetof(sl_res_pool_r16_s, sl_prs_res_shared_sl_prs_rp_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::ptr_item_list_codec<decltype(sl_res_pool_r16_s::sl_prs_res_shared_sl_prs_rp_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_prs_res_shared_sl_prs_rp_r18_s::desc},
    {offsetof(sl_res_pool_r16_s, num_sym_sl_prs_2nd_stage_sci_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, num_sym_sl_prs_2nd_stage_sci_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{3},
     nullptr},
    {offsetof(sl_res_pool_r16_s, sl_sci_based_sl_prs_tx_trigger_sci2_d_r18),
     (int32_t)offsetof(sl_res_pool_r16_s, sl_sci_based_sl_prs_tx_trigger_sci2_d_r18_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_res_pool_r16_s_g2_names[] = {
    {"sl-CPE-StartingPositionsPSCCH-PSSCH-InitiateCOT-List-r18", nullptr},
    {"sl-CPE-StartingPositionsPSCCH-PSSCH-InitiateCOT-Default-r18", nullptr},
    {"sl-CPE-StartingPositionsPSCCH-PSSCH-WithinCOT-List-r18", nullptr},
    {"sl-CPE-StartingPositionsPSCCH-PSSCH-WithinCOT-Default-r18", nullptr},
    {"sl-Type1-LBT-BlockingOption1-r18", "enabled"},
    {"sl-Type1-LBT-BlockingOption2-r18", "enabled"},
    {"sl-NumInterlacePerSubchannel-r18", nullptr},
    {"sl-NumReferencePRBs-OfInterlace-r18", nullptr},
    {"sl-TransmissionStructureForPSFCH-r18", nullptr},
    {"sl-NumDedicatedPRBs-ForPSFCH-r18", nullptr},
    {"sl-NumPSFCH-Occasions-r18", nullptr},
    {"sl-PSFCH-CommonInterlaceIndex-r18", nullptr},
    {"sl-CPE-StartingPositionPSFCH-r18", nullptr},
    {"sl-NumRefSymbolLength-r18", nullptr},
    {"sl-PSFCH-RB-SetList-r18", nullptr},
    {"sl-IUC-RB-SetList-r18", nullptr},
    {"sl-PSFCH-PowerOffset-r18", nullptr},
    {"sl-RBSetIndexOfResourcePool-r18", nullptr},
    {"sl-A2X-Service-r18", nullptr},
    {"sl-PRS-ResourcesSharedSL-PRS-RP-r18", nullptr},
    {"numSym-SL-PRS-2ndStageSCI-r18", nullptr},
    {"sl-SCI-basedSL-PRS-TxTriggerSCI2-D-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_res_pool_r16_s_g2_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_initiate_cot_default_r18_present)},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(sl_res_pool_r16_s, sl_cp_e_start_positions_pscch_pssch_within_cot_default_r18_present)},
    {4, (int32_t)offsetof(sl_res_pool_r16_s, sl_type1_lbt_blocking_option1_r18_present)},
    {5, (int32_t)offsetof(sl_res_pool_r16_s, sl_type1_lbt_blocking_option2_r18_present)},
    {6, (int32_t)offsetof(sl_res_pool_r16_s, sl_num_interlace_per_subch_r18_present)},
    {7, (int32_t)offsetof(sl_res_pool_r16_s, sl_num_ref_prbs_of_interlace_r18_present)},
    {8, (int32_t)offsetof(sl_res_pool_r16_s, sl_tx_structure_for_psfch_r18_present)},
    {9, (int32_t)offsetof(sl_res_pool_r16_s, sl_num_ded_prbs_for_psfch_r18_present)},
    {10, (int32_t)offsetof(sl_res_pool_r16_s, sl_num_psfch_occasions_r18_present)},
    {11, (int32_t)offsetof(sl_res_pool_r16_s, sl_psfch_common_interlace_idx_r18_present)},
    {12, (int32_t)offsetof(sl_res_pool_r16_s, sl_cp_e_start_position_psfch_r18_present)},
    {13, (int32_t)offsetof(sl_res_pool_r16_s, sl_num_ref_symbol_len_r18_present)},
    {14, asn1::table::seq_field::ptr_presence},
    {15, asn1::table::seq_field::ptr_presence},
    {16, (int32_t)offsetof(sl_res_pool_r16_s, sl_psfch_pwr_offset_r18_present)},
    {17, asn1::table::seq_field::ptr_presence},
    {18, (int32_t)offsetof(sl_res_pool_r16_s, sl_a2_x_service_r18_present)},
    {19, asn1::table::seq_field::ptr_presence},
    {20, (int32_t)offsetof(sl_res_pool_r16_s, num_sym_sl_prs_2nd_stage_sci_r18_present)},
    {21, (int32_t)offsetof(sl_res_pool_r16_s, sl_sci_based_sl_prs_tx_trigger_sci2_d_r18_present)}};
static const asn1::table::seq_group seq_table_sl_res_pool_r16_s_groups[] = {{{seq_table_sl_res_pool_r16_s_g0_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_sl_res_pool_r16_s_g0_opts,
                                                                              seq_table_sl_res_pool_r16_s_g0_names,
                                                                              1,
                                                                              1},
                                                                             false},
                                                                            {{seq_table_sl_res_pool_r16_s_g1_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_sl_res_pool_r16_s_g1_opts,
                                                                              seq_table_sl_res_pool_r16_s_g1_names,
                                                                              2,
                                                                              2},
                                                                             false},
                                                                            {{seq_table_sl_res_pool_r16_s_g2_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_sl_res_pool_r16_s_g2_opts,
                                                                              seq_table_sl_res_pool_r16_s_g2_names,
                                                                              22,
                                                                              22},
                                                                             false}};
const asn1::table::seq_desc         sl_res_pool_r16_s::desc              = {{seq_table_sl_res_pool_r16_s_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_sl_res_pool_r16_s_opts,
                                                                             seq_table_sl_res_pool_r16_s_names,
                                                                             25,
                                                                             25},
                                                                            true,
                                                                            false,
                                                                            seq_table_sl_res_pool_r16_s_groups,
                                                                            3};

static const char* enum_names_sl_res_pool_r16_s_sl_subch_size_r16_e__values[] =
    {"n10", "n12", "n15", "n20", "n25", "n50", "n75", "n100"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_subch_size_r16_opts::desc = {
    enum_names_sl_res_pool_r16_s_sl_subch_size_r16_e__values,
    8,
    "sl_res_pool_r16_s::sl_subch_size_r16_e_"};
uint8_t sl_res_pool_r16_s::sl_subch_size_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 12, 15, 20, 25, 50, 75, 100};
  return map_enum_number(numbers, 8, value, "sl_res_pool_r16_s::sl_subch_size_r16_e_");
}

static const char*           enum_names_sl_res_pool_r16_s_sl_add_mcs_table_r16_e__values[] = {"qam256",
                                                                                              "qam64LowSE",
                                                                                              "qam256-qam64LowSE"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_add_mcs_table_r16_opts::desc            = {
    enum_names_sl_res_pool_r16_s_sl_add_mcs_table_r16_e__values,
    3,
    "sl_res_pool_r16_s::sl_add_mcs_table_r16_e_"};

static const char*           enum_names_sl_res_pool_r16_s_sl_time_win_size_cbr_r16_e__values[] = {"ms100", "slot100"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_time_win_size_cbr_r16_opts::desc            = {
    enum_names_sl_res_pool_r16_s_sl_time_win_size_cbr_r16_e__values,
    2,
    "sl_res_pool_r16_s::sl_time_win_size_cbr_r16_e_"};

static const char*           enum_names_sl_res_pool_r16_s_sl_time_win_size_cr_r16_e__values[] = {"ms1000", "slot1000"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_time_win_size_cr_r16_opts::desc            = {
    enum_names_sl_res_pool_r16_s_sl_time_win_size_cr_r16_e__values,
    2,
    "sl_res_pool_r16_s::sl_time_win_size_cr_r16_e_"};

static const char* enum_names_sl_res_pool_r16_s_sl_preemption_enable_r16_e__values[] =
    {"enabled", "pl1", "pl2", "pl3", "pl4", "pl5", "pl6", "pl7", "pl8"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_preemption_enable_r16_opts::desc = {
    enum_names_sl_res_pool_r16_s_sl_preemption_enable_r16_e__values,
    9,
    "sl_res_pool_r16_s::sl_preemption_enable_r16_e_"};
uint8_t sl_res_pool_r16_s::sl_preemption_enable_r16_opts::to_number() const
{
  switch (value) {
    case pl1:
      return 1;
    case pl2:
      return 2;
    case pl3:
      return 3;
    case pl4:
      return 4;
    case pl5:
      return 5;
    case pl6:
      return 6;
    case pl7:
      return 7;
    case pl8:
      return 8;
    default:
      invalid_enum_number(value, "sl_res_pool_r16_s::sl_preemption_enable_r16_e_");
  }
  return 0;
}

static const char*           enum_names_sl_res_pool_r16_s_sl_x_overhead_r16_e__values[] = {"n0", "n3", "n6", "n9"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_x_overhead_r16_opts::desc            = {
    enum_names_sl_res_pool_r16_s_sl_x_overhead_r16_e__values,
    4,
    "sl_res_pool_r16_s::sl_x_overhead_r16_e_"};
uint8_t sl_res_pool_r16_s::sl_x_overhead_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 3, 6, 9};
  return map_enum_number(numbers, 4, value, "sl_res_pool_r16_s::sl_x_overhead_r16_e_");
}

static const char*           enum_names_sl_res_pool_r16_s_sl_num_interlace_per_subch_r18_e__values[] = {"sc1", "sc2"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_num_interlace_per_subch_r18_opts::desc            = {
    enum_names_sl_res_pool_r16_s_sl_num_interlace_per_subch_r18_e__values,
    2,
    "sl_res_pool_r16_s::sl_num_interlace_per_subch_r18_e_"};
uint8_t sl_res_pool_r16_s::sl_num_interlace_per_subch_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_res_pool_r16_s::sl_num_interlace_per_subch_r18_e_");
}

static const char* enum_names_sl_res_pool_r16_s_sl_num_ref_prbs_of_interlace_r18_e__values[] = {"prb10", "prb11"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18_opts::desc  = {
    enum_names_sl_res_pool_r16_s_sl_num_ref_prbs_of_interlace_r18_e__values,
    2,
    "sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18_e_"};
uint8_t sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 11};
  return map_enum_number(numbers, 2, value, "sl_res_pool_r16_s::sl_num_ref_prbs_of_interlace_r18_e_");
}

static const char* enum_names_sl_res_pool_r16_s_sl_tx_structure_for_psfch_r18_e__values[] = {"commonInterlace",
                                                                                             "dedicatedInterlace"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_tx_structure_for_psfch_r18_opts::desc  = {
    enum_names_sl_res_pool_r16_s_sl_tx_structure_for_psfch_r18_e__values,
    2,
    "sl_res_pool_r16_s::sl_tx_structure_for_psfch_r18_e_"};

static const char* enum_names_sl_res_pool_r16_s_sl_num_ded_prbs_for_psfch_r18_e__values[] = {"prb1", "prb2", "prb5"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18_opts::desc  = {
    enum_names_sl_res_pool_r16_s_sl_num_ded_prbs_for_psfch_r18_e__values,
    3,
    "sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18_e_"};
uint8_t sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 5};
  return map_enum_number(numbers, 3, value, "sl_res_pool_r16_s::sl_num_ded_prbs_for_psfch_r18_e_");
}

static const char* enum_names_sl_res_pool_r16_s_sl_num_psfch_occasions_r18_e__values[] = {"o1", "o2", "o3", "o4"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_num_psfch_occasions_r18_opts::desc  = {
    enum_names_sl_res_pool_r16_s_sl_num_psfch_occasions_r18_e__values,
    4,
    "sl_res_pool_r16_s::sl_num_psfch_occasions_r18_e_"};
uint8_t sl_res_pool_r16_s::sl_num_psfch_occasions_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "sl_res_pool_r16_s::sl_num_psfch_occasions_r18_e_");
}

static const char* enum_names_sl_res_pool_r16_s_sl_num_ref_symbol_len_r18_e__values[] =
    {"sym7", "sym8", "sym9", "sym10", "sym11", "sym12", "sym13", "sym14"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_num_ref_symbol_len_r18_opts::desc = {
    enum_names_sl_res_pool_r16_s_sl_num_ref_symbol_len_r18_e__values,
    8,
    "sl_res_pool_r16_s::sl_num_ref_symbol_len_r18_e_"};
uint8_t sl_res_pool_r16_s::sl_num_ref_symbol_len_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 8, 9, 10, 11, 12, 13, 14};
  return map_enum_number(numbers, 8, value, "sl_res_pool_r16_s::sl_num_ref_symbol_len_r18_e_");
}

static const char*           enum_names_sl_res_pool_r16_s_sl_a2_x_service_r18_e__values[] = {"brid",
                                                                                             "daa",
                                                                                             "bridAndDAA",
                                                                                             "spare1"};
const asn1::table::enum_desc sl_res_pool_r16_s::sl_a2_x_service_r18_opts::desc            = {
    enum_names_sl_res_pool_r16_s_sl_a2_x_service_r18_e__values,
    4,
    "sl_res_pool_r16_s::sl_a2_x_service_r18_e_"};

// SL-ResourcePoolConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_res_pool_cfg_r16_s_fields[] = {
    {offsetof(sl_res_pool_cfg_r16_s, sl_res_pool_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(sl_res_pool_cfg_r16_s, sl_res_pool_r16),
     (int32_t)offsetof(sl_res_pool_cfg_r16_s, sl_res_pool_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_res_pool_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_res_pool_cfg_r16_s_names[] = {{"sl-ResourcePoolID-r16", nullptr},
                                                                                {"sl-ResourcePool-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_res_pool_cfg_r16_s_opts[]  = {
    {1, (int32_t)offsetof(sl_res_pool_cfg_r16_s, sl_res_pool_r16_present)}};
const asn1::table::seq_desc sl_res_pool_cfg_r16_s::desc = {{seq_table_sl_res_pool_cfg_r16_s_fields,
                                                            uint64_t{0x1},
                                                            seq_table_sl_res_pool_cfg_r16_s_opts,
                                                            seq_table_sl_res_pool_cfg_r16_s_names,
                                                            2,
                                                            1},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// SL-TxPoolDedicated-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_tx_pool_ded_r16_s_fields[] = {
    {offsetof(sl_tx_pool_ded_r16_s, sl_pool_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_tx_pool_ded_r16_s::sl_pool_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{15}, false, false>>::value},
    {offsetof(sl_tx_pool_ded_r16_s, sl_pool_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_tx_pool_ded_r16_s::sl_pool_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_res_pool_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_tx_pool_ded_r16_s_names[] = {{"sl-PoolToReleaseList-r16", nullptr},
                                                                               {"sl-PoolToAddModList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_tx_pool_ded_r16_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                           {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       sl_tx_pool_ded_r16_s::desc            = {{seq_table_sl_tx_pool_ded_r16_s_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_sl_tx_pool_ded_r16_s_opts,
                                                                            seq_table_sl_tx_pool_ded_r16_s_names,
                                                                            2,
                                                                            2},
                                                                           false,
                                                                           false,
                                                                           nullptr,
                                                                           0};

// SL-BWP-PoolConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_pool_cfg_r16_s_fields[] = {
    {offsetof(sl_bwp_pool_cfg_r16_s, sl_rx_pool_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_bwp_pool_cfg_r16_s::sl_rx_pool_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_res_pool_r16_s::desc},
    {offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_sel_normal_r16),
     (int32_t)offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_sel_normal_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_tx_pool_ded_r16_s::desc},
    {offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_sched_r16),
     (int32_t)offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_sched_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_tx_pool_ded_r16_s::desc},
    {offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_exceptional_r16),
     (int32_t)offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_exceptional_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_res_pool_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_pool_cfg_r16_s_names[] = {
    {"sl-RxPool-r16", nullptr},
    {"sl-TxPoolSelectedNormal-r16", nullptr},
    {"sl-TxPoolScheduling-r16", nullptr},
    {"sl-TxPoolExceptional-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_pool_cfg_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_sel_normal_r16_present)},
    {2, (int32_t)offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_sched_r16_present)},
    {3, (int32_t)offsetof(sl_bwp_pool_cfg_r16_s, sl_tx_pool_exceptional_r16_present)}};
const asn1::table::seq_desc sl_bwp_pool_cfg_r16_s::desc = {{seq_table_sl_bwp_pool_cfg_r16_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_sl_bwp_pool_cfg_r16_s_opts,
                                                            seq_table_sl_bwp_pool_cfg_r16_s_names,
                                                            4,
                                                            4},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// SL-BWP-DiscPoolConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_disc_pool_cfg_r17_s_fields[] = {
    {offsetof(sl_bwp_disc_pool_cfg_r17_s, sl_disc_rx_pool_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_bwp_disc_pool_cfg_r17_s::sl_disc_rx_pool_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_res_pool_r16_s::desc},
    {offsetof(sl_bwp_disc_pool_cfg_r17_s, sl_disc_tx_pool_sel_r17),
     (int32_t)offsetof(sl_bwp_disc_pool_cfg_r17_s, sl_disc_tx_pool_sel_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_tx_pool_ded_r16_s::desc},
    {offsetof(sl_bwp_disc_pool_cfg_r17_s, sl_disc_tx_pool_sched_r17),
     (int32_t)offsetof(sl_bwp_disc_pool_cfg_r17_s, sl_disc_tx_pool_sched_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_tx_pool_ded_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_disc_pool_cfg_r17_s_names[] = {
    {"sl-DiscRxPool-r17", nullptr},
    {"sl-DiscTxPoolSelected-r17", nullptr},
    {"sl-DiscTxPoolScheduling-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_disc_pool_cfg_r17_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_bwp_disc_pool_cfg_r17_s, sl_disc_tx_pool_sel_r17_present)},
    {2, (int32_t)offsetof(sl_bwp_disc_pool_cfg_r17_s, sl_disc_tx_pool_sched_r17_present)}};
const asn1::table::seq_desc sl_bwp_disc_pool_cfg_r17_s::desc = {{seq_table_sl_bwp_disc_pool_cfg_r17_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_sl_bwp_disc_pool_cfg_r17_s_opts,
                                                                 seq_table_sl_bwp_disc_pool_cfg_r17_s_names,
                                                                 3,
                                                                 3},
                                                                false,
                                                                false,
                                                                nullptr,
                                                                0};

// SL-PSCCH-ConfigDedicatedSL-PRS-RP-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_fields[] = {
    {offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_time_res_pscch_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_time_res_pscch_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18)>(),
     decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18_opts::desc},
    {offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_freq_res_pscch_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_freq_res_pscch_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18)>(),
     decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18_opts::desc}};
static const asn1::table::field_name seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_names[] = {
    {"sl-TimeResourcePSCCH-DedicatedSL-PRS-RP-r18", nullptr},
    {"sl-FreqResourcePSCCH-DedicatedSL-PRS-RP-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_time_res_pscch_ded_sl_prs_rp_r18_present)},
    {1, (int32_t)offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_freq_res_pscch_ded_sl_prs_rp_r18_present)}};
static const asn1::table::seq_field seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_g0_fields[] = {
    {offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_dmrs_scramble_id_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_dmrs_scramble_id_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_g0_names[] = {
    {"sl-DMRS-ScrambleID-DedicatedSL-PRS-RP-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_pscch_cfg_ded_sl_prs_rp_r18_s, sl_dmrs_scramble_id_ded_sl_prs_rp_r18_present)}};
static const asn1::table::seq_group seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_groups[] = {
    {{seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_g0_opts,
      seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_g0_names,
      1,
      1},
     false}};
const asn1::table::seq_desc sl_pscch_cfg_ded_sl_prs_rp_r18_s::desc = {
    {seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_fields,
     uint64_t{0x0},
     seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_opts,
     seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_names,
     2,
     2},
    true,
    false,
    seq_table_sl_pscch_cfg_ded_sl_prs_rp_r18_s_groups,
    1};

static const char* enum_names_sl_pscch_cfg_ded_sl_prs_rp_r18_s_sl_time_res_pscch_ded_sl_prs_rp_r18_e__values[] = {"n2",
                                                                                                                  "n3"};
const asn1::table::enum_desc sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18_opts::desc  = {
    enum_names_sl_pscch_cfg_ded_sl_prs_rp_r18_s_sl_time_res_pscch_ded_sl_prs_rp_r18_e__values,
    2,
    "sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18_e_"};
uint8_t sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_time_res_pscch_ded_sl_prs_rp_r18_e_");
}

static const char* enum_names_sl_pscch_cfg_ded_sl_prs_rp_r18_s_sl_freq_res_pscch_ded_sl_prs_rp_r18_e__values[] =
    {"n10", "n12", "n15", "n20", "n25"};
const asn1::table::enum_desc sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18_opts::desc = {
    enum_names_sl_pscch_cfg_ded_sl_prs_rp_r18_s_sl_freq_res_pscch_ded_sl_prs_rp_r18_e__values,
    5,
    "sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18_e_"};
uint8_t sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 12, 15, 20, 25};
  return map_enum_number(numbers, 5, value, "sl_pscch_cfg_ded_sl_prs_rp_r18_s::sl_freq_res_pscch_ded_sl_prs_rp_r18_e_");
}

// SL-ReservationPeriodAllowedDedicatedSL-PRS-RP-r18 ::= CHOICE
static const asn1::table::storage_ops* const
    choice_storage_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_alternatives[] = {
        &asn1::table::alternative_ops<sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_,
                                      true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::storage_desc = {
    choice_storage_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_alternatives,
    2,
    "sl_reserv_period_allowed_ded_sl_prs_rp_r18_c"};

sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_&
sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::set_sl_res_reserve_period1_r18()
{
  set(types::sl_res_reserve_period1_r18);
  return c.get<sl_res_reserve_period1_r18_e_>();
}
uint8_t& sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::set_sl_res_reserve_period2_r18()
{
  set(types::sl_res_reserve_period2_r18);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_>(),
     sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_::has_ext,
     false,
     sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_::nof_types,
     sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_::nof_exts,
     nullptr,
     &sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{98},
     nullptr}};
static const asn1::table::field_name choice_table_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_names[] = {
    {"sl-ResourceReservePeriod1-r18", nullptr},
    {"sl-ResourceReservePeriod2-r18", nullptr}};
const asn1::table::choice_desc sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::desc = {
    asn1::table::enum_int_type<sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types>(),
    sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types::nof_types,
    sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types::nof_exts,
    sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types::has_ext,
    false,
    choice_table_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_alternatives,
    nullptr,
    choice_table_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_names,
    "sl_reserv_period_allowed_ded_sl_prs_rp_r18_c",
    offsetof(sl_reserv_period_allowed_ded_sl_prs_rp_r18_c, c.type),
    offsetof(sl_reserv_period_allowed_ded_sl_prs_rp_r18_c, c.value),
    false,
    &sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::storage_desc};

static const char* enum_names_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_sl_res_reserve_period1_r18_e__values[] = {
    "ms0",
    "ms100",
    "ms160",
    "ms200",
    "ms300",
    "ms320",
    "ms400",
    "ms500",
    "ms600",
    "ms640",
    "ms700",
    "ms800",
    "ms900",
    "ms1000",
    "ms1280",
    "ms2560",
    "ms5120",
    "ms10240"};
const asn1::table::enum_desc sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_opts::desc = {
    enum_names_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_sl_res_reserve_period1_r18_e__values,
    18,
    "sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_"};
uint16_t sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {
      0, 100, 160, 200, 300, 320, 400, 500, 600, 640, 700, 800, 900, 1000, 1280, 2560, 5120, 10240};
  return map_enum_number(
      numbers, 18, value, "sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::sl_res_reserve_period1_r18_e_");
}

static const char* enum_names_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_types_values[] = {
    "sl-ResourceReservePeriod1-r18",
    "sl-ResourceReservePeriod2-r18"};
const asn1::table::enum_desc sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types_opts::desc = {
    enum_names_sl_reserv_period_allowed_ded_sl_prs_rp_r18_c_types_values,
    2,
    "sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types"};
uint8_t sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::types");
}

// SL-PRS-ResourceDedicatedSL-PRS-RP-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prs_res_ded_sl_prs_rp_r18_s_fields[] = {
    {offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_res_id_r18),
     (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_res_id_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{11},
     nullptr},
    {offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_nof_symbols_r18),
     (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_nof_symbols_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_comb_size_r18),
     (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_comb_size_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18)>(),
     decltype(sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18)::has_ext,
     false,
     decltype(sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18)::nof_types,
     decltype(sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18)::nof_exts,
     nullptr,
     &sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18_opts::desc},
    {offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_start_symbol_r18),
     (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_start_symbol_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{4},
     uint64_t{8},
     nullptr},
    {offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_comb_offset_r18),
     (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_comb_offset_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{4},
     nullptr}};
static const asn1::table::field_name seq_table_sl_prs_res_ded_sl_prs_rp_r18_s_names[] = {
    {"sl-PRS-ResourceID-r18", nullptr},
    {"sl-NumberOfSymbols-r18", nullptr},
    {"sl-CombSize-r18", nullptr},
    {"sl-PRS-starting-symbol-r18", nullptr},
    {"sl-PRS-comb-offset-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_prs_res_ded_sl_prs_rp_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_res_id_r18_present)},
    {1, (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_nof_symbols_r18_present)},
    {2, (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_comb_size_r18_present)},
    {3, (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_start_symbol_r18_present)},
    {4, (int32_t)offsetof(sl_prs_res_ded_sl_prs_rp_r18_s, sl_prs_comb_offset_r18_present)}};
const asn1::table::seq_desc sl_prs_res_ded_sl_prs_rp_r18_s::desc = {{seq_table_sl_prs_res_ded_sl_prs_rp_r18_s_fields,
                                                                     uint64_t{0x0},
                                                                     seq_table_sl_prs_res_ded_sl_prs_rp_r18_s_opts,
                                                                     seq_table_sl_prs_res_ded_sl_prs_rp_r18_s_names,
                                                                     5,
                                                                     5},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0};

static const char* enum_names_sl_prs_res_ded_sl_prs_rp_r18_s_sl_comb_size_r18_e__values[] = {"n2", "n4", "n6"};
const asn1::table::enum_desc sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18_opts::desc  = {
    enum_names_sl_prs_res_ded_sl_prs_rp_r18_s_sl_comb_size_r18_e__values,
    3,
    "sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18_e_"};
uint8_t sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6};
  return map_enum_number(numbers, 3, value, "sl_prs_res_ded_sl_prs_rp_r18_s::sl_comb_size_r18_e_");
}

// SL-PRS-PowerControl-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prs_pwr_ctrl_r18_s_fields[] = {
    {offsetof(sl_prs_pwr_ctrl_r18_s, dl_p0_sl_prs_r18),
     (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, dl_p0_sl_prs_r18_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     int64_t{-202},
     uint64_t{226},
     nullptr},
    {offsetof(sl_prs_pwr_ctrl_r18_s, dl_alpha_sl_prs_r18),
     (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, dl_alpha_sl_prs_r18_present),
     asn1::table::enum_op<decltype(sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18)>(),
     decltype(sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18)::has_ext,
     false,
     decltype(sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18)::nof_types,
     decltype(sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18)::nof_exts,
     nullptr,
     &sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18_opts::desc},
    {offsetof(sl_prs_pwr_ctrl_r18_s, sl_p0_sl_prs_r18),
     (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, sl_p0_sl_prs_r18_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     int64_t{-202},
     uint64_t{226},
     nullptr},
    {offsetof(sl_prs_pwr_ctrl_r18_s, sl_alpha_sl_prs_r18),
     (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, sl_alpha_sl_prs_r18_present),
     asn1::table::enum_op<decltype(sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18)>(),
     decltype(sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18)::has_ext,
     false,
     decltype(sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18)::nof_types,
     decltype(sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18)::nof_exts,
     nullptr,
     &sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18_opts::desc}};
static const asn1::table::field_name seq_table_sl_prs_pwr_ctrl_r18_s_names[] = {{"dl-P0-SL-PRS-r18", nullptr},
                                                                                {"dl-Alpha-SL-PRS-r18", nullptr},
                                                                                {"sl-P0-SL-PRS-r18", nullptr},
                                                                                {"sl-Alpha-SL-PRS-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_prs_pwr_ctrl_r18_s_opts[]  = {
    {0, (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, dl_p0_sl_prs_r18_present)},
    {1, (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, dl_alpha_sl_prs_r18_present)},
    {2, (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, sl_p0_sl_prs_r18_present)},
    {3, (int32_t)offsetof(sl_prs_pwr_ctrl_r18_s, sl_alpha_sl_prs_r18_present)}};
const asn1::table::seq_desc sl_prs_pwr_ctrl_r18_s::desc = {{seq_table_sl_prs_pwr_ctrl_r18_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_sl_prs_pwr_ctrl_r18_s_opts,
                                                            seq_table_sl_prs_pwr_ctrl_r18_s_names,
                                                            4,
                                                            4},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

static const char* enum_names_sl_prs_pwr_ctrl_r18_s_dl_alpha_sl_prs_r18_e__values[] =
    {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
const asn1::table::enum_desc sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18_opts::desc = {
    enum_names_sl_prs_pwr_ctrl_r18_s_dl_alpha_sl_prs_r18_e__values,
    8,
    "sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18_e_"};
float sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18_e_");
}
const char* sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_prs_pwr_ctrl_r18_s::dl_alpha_sl_prs_r18_e_");
}

static const char* enum_names_sl_prs_pwr_ctrl_r18_s_sl_alpha_sl_prs_r18_e__values[] =
    {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
const asn1::table::enum_desc sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18_opts::desc = {
    enum_names_sl_prs_pwr_ctrl_r18_s_sl_alpha_sl_prs_r18_e__values,
    8,
    "sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18_e_"};
float sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18_e_");
}
const char* sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "sl_prs_pwr_ctrl_r18_s::sl_alpha_sl_prs_r18_e_");
}

// SL-TxPercentageDedicatedSL-PRS-RP-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_fields[] = {
    {offsetof(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s, sl_tx_percentage_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s, sl_tx_percentage_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s, sl_prio_ded_sl_prs_rp),
     (int32_t)offsetof(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s, sl_prio_ded_sl_prs_rp_present),
     asn1::table::enum_op<decltype(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp)>(),
     decltype(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp)::has_ext,
     false,
     decltype(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp)::nof_types,
     decltype(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp)::nof_exts,
     nullptr,
     &sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp_opts::desc}};
static const asn1::table::field_name seq_table_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_names[] = {
    {"sl-TxPercentageDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-Priority-DedicatedSL-PRS-RP", nullptr}};
static const asn1::table::seq_opt seq_table_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s, sl_tx_percentage_ded_sl_prs_rp_r18_present)},
    {1, (int32_t)offsetof(sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s, sl_prio_ded_sl_prs_rp_present)}};
const asn1::table::seq_desc sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::desc = {
    {seq_table_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_fields,
     uint64_t{0x0},
     seq_table_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_opts,
     seq_table_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_names,
     2,
     2},
    false,
    false,
    nullptr,
    0};

static const char* enum_names_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_sl_prio_ded_sl_prs_rp_e__values[] = {"p20",
                                                                                                            "p35",
                                                                                                            "p50"};
const asn1::table::enum_desc sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp_opts::desc  = {
    enum_names_sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s_sl_prio_ded_sl_prs_rp_e__values,
    3,
    "sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp_e_"};
uint8_t sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 35, 50};
  return map_enum_number(numbers, 3, value, "sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::sl_prio_ded_sl_prs_rp_e_");
}

// SL-PriorityTxConfigIndexDedicatedSL-PRS-RP-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s_fields[] = {
    {offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_prio_thres_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_prio_thres_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_default_tx_cfg_idx_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_default_tx_cfg_idx_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_cbr_cfg_idx_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_cbr_cfg_idx_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_prs_tx_cfg_idx_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     14,
     &asn1::table::list_codec<decltype(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s::sl_prs_tx_cfg_idx_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{64}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s_names[] = {
    {"sl-PriorityThresholdDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-DefaultTxConfigIndexDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-CBR-ConfigIndexDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-PRS-TxConfigIndexList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_prio_thres_ded_sl_prs_rp_r18_present)},
    {1, (int32_t)offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_default_tx_cfg_idx_ded_sl_prs_rp_r18_present)},
    {2, (int32_t)offsetof(sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s, sl_cbr_cfg_idx_ded_sl_prs_rp_r18_present)},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s::desc = {
    {seq_table_sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s_fields,
     uint64_t{0x0},
     seq_table_sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s_opts,
     seq_table_sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s_names,
     4,
     4},
    false,
    false,
    nullptr,
    0};

// SL-SelectionWindowConfigDedicatedSL-PRS-RP-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sel_win_cfg_ded_sl_prs_rp_r18_s_fields[] = {
    {offsetof(sl_sel_win_cfg_ded_sl_prs_rp_r18_s, sl_prs_prio_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_sel_win_cfg_ded_sl_prs_rp_r18_s, sl_prs_sel_win_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18)>(),
     decltype(sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18)::has_ext,
     false,
     decltype(sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18)::nof_types,
     decltype(sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18)::nof_exts,
     nullptr,
     &sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18_opts::desc}};
static const asn1::table::field_name seq_table_sl_sel_win_cfg_ded_sl_prs_rp_r18_s_names[] = {
    {"sl-PRS-Priority-r18", nullptr},
    {"sl-PRS-SelectionWindow-r18", nullptr}};
const asn1::table::seq_desc sl_sel_win_cfg_ded_sl_prs_rp_r18_s::desc = {
    {seq_table_sl_sel_win_cfg_ded_sl_prs_rp_r18_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_sl_sel_win_cfg_ded_sl_prs_rp_r18_s_names,
     2,
     0},
    false,
    false,
    nullptr,
    0};

static const char*           enum_names_sl_sel_win_cfg_ded_sl_prs_rp_r18_s_sl_prs_sel_win_r18_e__values[] = {"n1",
                                                                                                             "n5",
                                                                                                             "n10",
                                                                                                             "n20"};
const asn1::table::enum_desc sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18_opts::desc            = {
    enum_names_sl_sel_win_cfg_ded_sl_prs_rp_r18_s_sl_prs_sel_win_r18_e__values,
    4,
    "sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18_e_"};
uint8_t sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 5, 10, 20};
  return map_enum_number(numbers, 4, value, "sl_sel_win_cfg_ded_sl_prs_rp_r18_s::sl_prs_sel_win_r18_e_");
}

// SL-PRS-ResourcePool-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prs_res_pool_r18_s_sl_prs_pscch_cfg_r18_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &sl_pscch_cfg_ded_sl_prs_rp_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_prs_res_pool_r18_s_sl_prs_pscch_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_prs_res_pool_r18_s_sl_prs_pscch_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_prs_res_pool_r18_s_sl_prs_pscch_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_prs_res_pool_r18_s_sl_prs_pscch_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_pscch_cfg_ded_sl_prs_rp_r18_s>::type_offset(),
    setup_release_c<sl_pscch_cfg_ded_sl_prs_rp_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_prs_res_pool_r18_s_fields[] = {
    {offsetof(sl_prs_res_pool_r18_s, sl_prs_pscch_cfg_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prs_pscch_cfg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_prs_res_pool_r18_s_sl_prs_pscch_cfg_r18_setup_release},
    {offsetof(sl_prs_res_pool_r18_s, sl_start_rb_subch_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_start_rb_subch_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{265},
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_filt_coef_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_filt_coef_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_filt_coef_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_filt_coef_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_filt_coef_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_filt_coef_r18)::nof_exts,
     nullptr,
     &filt_coef_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_thresh_s_rssi_prs_cbr_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_thresh_s_rssi_prs_cbr_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{45},
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_rb_num_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_rb_num_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{10},
     uint64_t{265},
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_time_res_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_time_res_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_prs_res_pool_r18_s::sl_time_res_r18), asn1::table::struct_json>::value},
    {offsetof(sl_prs_res_pool_r18_s, sl_pos_allowed_res_sel_cfg_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_pos_allowed_res_sel_cfg_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_prs_res_reserve_period_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_prs_res_pool_r18_s::sl_prs_res_reserve_period_list_r18),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &sl_reserv_period_allowed_ded_sl_prs_rp_r18_c::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_prs_res_ded_sl_prs_rp_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     11,
     &asn1::table::item_list_codec<decltype(sl_prs_res_pool_r18_s::sl_prs_res_ded_sl_prs_rp_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_prs_res_ded_sl_prs_rp_r18_s::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_prs_pwr_ctrl_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prs_pwr_ctrl_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_prs_pwr_ctrl_r18_s::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_sensing_win_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_sensing_win_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_tx_percentage_ded_sl_prs_rp_list_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_tx_percentage_ded_sl_prs_rp_list_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     8,
     0,
     &asn1::table::item_list_codec<decltype(sl_prs_res_pool_r18_s::sl_tx_percentage_ded_sl_prs_rp_list_r18),
                                   asn1::table::sequence_item_ops,
                                   true,
                                   true>::value,
     &sl_tx_percentage_ded_sl_prs_rp_cfg_r18_s::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_sci_based_sl_prs_tx_trigger_sci1_b_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_sci_based_sl_prs_tx_trigger_sci1_b_r18_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_num_subch_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_num_subch_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{26},
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_subch_size_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_subch_size_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_max_num_per_reserve_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_max_num_per_reserve_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_num_reserved_bits_sci1_b_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_num_reserved_bits_sci1_b_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{20},
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_src_id_len_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_src_id_len_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_cbr_prio_tx_cfg_ded_sl_prs_rp_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_prs_res_pool_r18_s::sl_cbr_prio_tx_cfg_ded_sl_prs_rp_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_prio_tx_cfg_idx_ded_sl_prs_rp_r18_s::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_time_win_size_cbr_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_time_win_size_cbr_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cbr_ded_sl_prs_rp_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cbr_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cbr_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cbr_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_time_win_size_cbr_ded_sl_prs_rp_r18_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_time_win_size_cr_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_time_win_size_cr_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cr_ded_sl_prs_rp_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cr_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cr_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_time_win_size_cr_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_time_win_size_cr_ded_sl_prs_rp_r18_opts::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_cbr_common_tx_ded_sl_prs_rp_list_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_cbr_common_tx_ded_sl_prs_rp_list_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_cbr_common_tx_ded_sl_prs_rp_list_r18_s::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_prio_thres_ul_urllc_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prio_thres_ul_urllc_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_prio_thres_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prio_thres_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr},
    {offsetof(sl_prs_res_pool_r18_s, sl_sel_win_list_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_sel_win_list_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     8,
     0,
     &asn1::table::item_list_codec<decltype(sl_prs_res_pool_r18_s::sl_sel_win_list_ded_sl_prs_rp_r18),
                                   asn1::table::sequence_item_ops,
                                   true,
                                   true>::value,
     &sl_sel_win_cfg_ded_sl_prs_rp_r18_s::desc},
    {offsetof(sl_prs_res_pool_r18_s, sl_thres_rsrp_list_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_thres_rsrp_list_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     64,
     0,
     &asn1::table::list_codec<decltype(sl_prs_res_pool_r18_s::sl_thres_rsrp_list_ded_sl_prs_rp_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<true, int64_t{0}, uint64_t{66}, false, false>>::value},
    {offsetof(sl_prs_res_pool_r18_s, sl_preemption_enable_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_preemption_enable_ded_sl_prs_rp_r18_present),
     asn1::table::enum_op<decltype(sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18)>(),
     decltype(sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18)::has_ext,
     false,
     decltype(sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18)::nof_types,
     decltype(sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18)::nof_exts,
     nullptr,
     &sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18_opts::desc}};
static const asn1::table::field_name seq_table_sl_prs_res_pool_r18_s_names[] = {
    {"sl-PRS-PSCCH-Config-r18", nullptr},
    {"sl-StartRB-SubchannelDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-FilterCoefficient-r18", nullptr},
    {"sl-ThreshS-RSSI-PRS-CBR-r18", nullptr},
    {"sl-RB-Number-r18", nullptr},
    {"sl-TimeResource-r18", nullptr},
    {"sl-PosAllowedResourceSelectionConfig-r18", nullptr},
    {"sl-PRS-ResourceReservePeriodList-r18", nullptr},
    {"sl-PRS-ResourcesDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-PRS-PowerControl-r18", nullptr},
    {"sl-SensingWindowDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-TxPercentageDedicatedSL-PRS-RP-List-r18", nullptr},
    {"sl-SCI-basedSL-PRS-TxTriggerSCI1-B-r18", nullptr},
    {"sl-NumSubchannelDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-SubchannelSizeDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-MaxNumPerReserveDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-NumReservedBitsSCI1B-DedicatedSL-PRS-RP-r18", nullptr},
    {"sl-SRC-ID-LenDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-CBR-PriorityTxConfigDedicatedSL-PRS-RP-List-r18", nullptr},
    {"sl-TimeWindowSizeCBR-DedicatedSL-PRS-RP-r18", nullptr},
    {"sl-TimeWindowSizeCR-DedicatedSL-PRS-RP-r18", nullptr},
    {"sl-CBR-CommonTxDedicatedSL-PRS-RP-List-r18", nullptr},
    {"sl-PriorityThreshold-UL-URLLC-r18", nullptr},
    {"sl-PriorityThreshold-r18", nullptr},
    {"sl-SelectionWindowListDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-Thres-RSRP-ListDedicatedSL-PRS-RP-r18", nullptr},
    {"sl-PreemptionEnableDedicatedSL-PRS-RP-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_prs_res_pool_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prs_pscch_cfg_r18_present)},
    {1, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_start_rb_subch_ded_sl_prs_rp_r18_present)},
    {2, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_filt_coef_r18_present)},
    {3, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_thresh_s_rssi_prs_cbr_r18_present)},
    {4, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_rb_num_r18_present)},
    {5, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_time_res_r18_present)},
    {6, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_pos_allowed_res_sel_cfg_r18_present)},
    {7, asn1::table::seq_field::list_presence},
    {8, asn1::table::seq_field::list_presence},
    {9, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prs_pwr_ctrl_r18_present)},
    {10, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_sensing_win_ded_sl_prs_rp_r18_present)},
    {11, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_tx_percentage_ded_sl_prs_rp_list_r18_present)},
    {12, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_sci_based_sl_prs_tx_trigger_sci1_b_r18_present)},
    {13, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_num_subch_ded_sl_prs_rp_r18_present)},
    {14, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_subch_size_ded_sl_prs_rp_r18_present)},
    {15, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_max_num_per_reserve_ded_sl_prs_rp_r18_present)},
    {16, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_num_reserved_bits_sci1_b_ded_sl_prs_rp_r18_present)},
    {17, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_src_id_len_ded_sl_prs_rp_r18_present)},
    {18, asn1::table::seq_field::list_presence},
    {19, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_time_win_size_cbr_ded_sl_prs_rp_r18_present)},
    {20, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_time_win_size_cr_ded_sl_prs_rp_r18_present)},
    {21, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_cbr_common_tx_ded_sl_prs_rp_list_r18_present)},
    {22, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prio_thres_ul_urllc_r18_present)},
    {23, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_prio_thres_r18_present)},
    {24, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_sel_win_list_ded_sl_prs_rp_r18_present)},
    {25, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_thres_rsrp_list_ded_sl_prs_rp_r18_present)},
    {26, (int32_t)offsetof(sl_prs_res_pool_r18_s, sl_preemption_enable_ded_sl_prs_rp_r18_present)}};
const asn1::table::seq_desc sl_prs_res_pool_r18_s::desc = {{seq_table_sl_prs_res_pool_r18_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_sl_prs_res_pool_r18_s_opts,
                                                            seq_table_sl_prs_res_pool_r18_s_names,
                                                            27,
                                                            27},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

static const char* enum_names_sl_prs_res_pool_r18_s_sl_pos_allowed_res_sel_cfg_r18_e__values[] = {"c1", "c2", "c3"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18_opts::desc  = {
    enum_names_sl_prs_res_pool_r18_s_sl_pos_allowed_res_sel_cfg_r18_e__values,
    3,
    "sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18_e_"};
uint8_t sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "sl_prs_res_pool_r18_s::sl_pos_allowed_res_sel_cfg_r18_e_");
}

static const char* enum_names_sl_prs_res_pool_r18_s_sl_sensing_win_ded_sl_prs_rp_r18_e__values[] = {"ms100", "ms1100"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18_opts::desc  = {
    enum_names_sl_prs_res_pool_r18_s_sl_sensing_win_ded_sl_prs_rp_r18_e__values,
    2,
    "sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18_e_"};
uint16_t sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 1100};
  return map_enum_number(numbers, 2, value, "sl_prs_res_pool_r18_s::sl_sensing_win_ded_sl_prs_rp_r18_e_");
}

static const char* enum_names_sl_prs_res_pool_r18_s_sl_subch_size_ded_sl_prs_rp_r18_e__values[] =
    {"n10", "n12", "n15", "n20", "n25", "n50", "n75", "n100"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18_opts::desc = {
    enum_names_sl_prs_res_pool_r18_s_sl_subch_size_ded_sl_prs_rp_r18_e__values,
    8,
    "sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18_e_"};
uint8_t sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 12, 15, 20, 25, 50, 75, 100};
  return map_enum_number(numbers, 8, value, "sl_prs_res_pool_r18_s::sl_subch_size_ded_sl_prs_rp_r18_e_");
}

static const char* enum_names_sl_prs_res_pool_r18_s_sl_max_num_per_reserve_ded_sl_prs_rp_r18_e__values[] = {"n2", "n3"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18_opts::desc  = {
    enum_names_sl_prs_res_pool_r18_s_sl_max_num_per_reserve_ded_sl_prs_rp_r18_e__values,
    2,
    "sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18_e_"};
uint8_t sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_prs_res_pool_r18_s::sl_max_num_per_reserve_ded_sl_prs_rp_r18_e_");
}

static const char* enum_names_sl_prs_res_pool_r18_s_sl_src_id_len_ded_sl_prs_rp_r18_e__values[] = {"n12", "n24"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18_opts::desc  = {
    enum_names_sl_prs_res_pool_r18_s_sl_src_id_len_ded_sl_prs_rp_r18_e__values,
    2,
    "sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18_e_"};
uint8_t sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 24};
  return map_enum_number(numbers, 2, value, "sl_prs_res_pool_r18_s::sl_src_id_len_ded_sl_prs_rp_r18_e_");
}

static const char* enum_names_sl_prs_res_pool_r18_s_sl_time_win_size_cbr_ded_sl_prs_rp_r18_e__values[] = {"ms100",
                                                                                                          "slot100"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_time_win_size_cbr_ded_sl_prs_rp_r18_opts::desc  = {
    enum_names_sl_prs_res_pool_r18_s_sl_time_win_size_cbr_ded_sl_prs_rp_r18_e__values,
    2,
    "sl_prs_res_pool_r18_s::sl_time_win_size_cbr_ded_sl_prs_rp_r18_e_"};

static const char* enum_names_sl_prs_res_pool_r18_s_sl_time_win_size_cr_ded_sl_prs_rp_r18_e__values[] = {"ms1000",
                                                                                                         "slot1000"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_time_win_size_cr_ded_sl_prs_rp_r18_opts::desc  = {
    enum_names_sl_prs_res_pool_r18_s_sl_time_win_size_cr_ded_sl_prs_rp_r18_e__values,
    2,
    "sl_prs_res_pool_r18_s::sl_time_win_size_cr_ded_sl_prs_rp_r18_e_"};

static const char* enum_names_sl_prs_res_pool_r18_s_sl_preemption_enable_ded_sl_prs_rp_r18_e__values[] =
    {"enabled", "pl1", "pl2", "pl3", "pl4", "pl5", "pl6", "pl7", "pl8"};
const asn1::table::enum_desc sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18_opts::desc = {
    enum_names_sl_prs_res_pool_r18_s_sl_preemption_enable_ded_sl_prs_rp_r18_e__values,
    9,
    "sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18_e_"};
uint8_t sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18_opts::to_number() const
{
  switch (value) {
    case pl1:
      return 1;
    case pl2:
      return 2;
    case pl3:
      return 3;
    case pl4:
      return 4;
    case pl5:
      return 5;
    case pl6:
      return 6;
    case pl7:
      return 7;
    case pl8:
      return 8;
    default:
      invalid_enum_number(value, "sl_prs_res_pool_r18_s::sl_preemption_enable_ded_sl_prs_rp_r18_e_");
  }
  return 0;
}

// SL-PRS-ResourcePoolConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prs_res_pool_cfg_r18_s_fields[] = {
    {offsetof(sl_prs_res_pool_cfg_r18_s, sl_prs_res_pool_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_prs_res_pool_cfg_r18_s, sl_prs_res_pool_r18),
     (int32_t)offsetof(sl_prs_res_pool_cfg_r18_s, sl_prs_res_pool_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_prs_res_pool_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_prs_res_pool_cfg_r18_s_names[] = {
    {"sl-PRS-ResourcePoolID-r18", nullptr},
    {"sl-PRS-ResourcePool-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_prs_res_pool_cfg_r18_s_opts[] = {
    {1, (int32_t)offsetof(sl_prs_res_pool_cfg_r18_s, sl_prs_res_pool_r18_present)}};
const asn1::table::seq_desc sl_prs_res_pool_cfg_r18_s::desc = {{seq_table_sl_prs_res_pool_cfg_r18_s_fields,
                                                                uint64_t{0x1},
                                                                seq_table_sl_prs_res_pool_cfg_r18_s_opts,
                                                                seq_table_sl_prs_res_pool_cfg_r18_s_names,
                                                                2,
                                                                1},
                                                               false,
                                                               false,
                                                               nullptr,
                                                               0};

// SL-PRS-TxPoolDedicated-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_prs_tx_pool_ded_r18_s_fields[] = {
    {offsetof(sl_prs_tx_pool_ded_r18_s, sl_prs_pool_to_release_list_r1),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_prs_tx_pool_ded_r18_s::sl_prs_pool_to_release_list_r1),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{7}, false, false>>::value},
    {offsetof(sl_prs_tx_pool_ded_r18_s, sl_prs_pool_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_prs_tx_pool_ded_r18_s::sl_prs_pool_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_prs_res_pool_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_prs_tx_pool_ded_r18_s_names[] = {
    {"sl-PRS-PoolToReleaseList-r1", nullptr},
    {"sl-PRS-PoolToAddModList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_prs_tx_pool_ded_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_prs_tx_pool_ded_r18_s::desc = {{seq_table_sl_prs_tx_pool_ded_r18_s_fields,
                                                               uint64_t{0x0},
                                                               seq_table_sl_prs_tx_pool_ded_r18_s_opts,
                                                               seq_table_sl_prs_tx_pool_ded_r18_s_names,
                                                               2,
                                                               2},
                                                              false,
                                                              false,
                                                              nullptr,
                                                              0};

// SL-BWP-PRS-PoolConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_prs_pool_cfg_r18_s_fields[] = {
    {offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_rx_pool_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_bwp_prs_pool_cfg_r18_s::sl_prs_rx_pool_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_prs_res_pool_r18_s::desc},
    {offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_sel_normal_r18),
     (int32_t)offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_sel_normal_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_prs_tx_pool_ded_r18_s::desc},
    {offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_sched_r18),
     (int32_t)offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_sched_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_prs_tx_pool_ded_r18_s::desc},
    {offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_exceptional_r18),
     (int32_t)offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_exceptional_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_prs_res_pool_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_prs_pool_cfg_r18_s_names[] = {
    {"sl-PRS-RxPool-r18", nullptr},
    {"sl-PRS-TxPoolSelectedNormal-r18", nullptr},
    {"sl-PRS-TxPoolScheduling-r18", nullptr},
    {"sl-PRS-TxPoolExceptional-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_bwp_prs_pool_cfg_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_sel_normal_r18_present)},
    {2, (int32_t)offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_sched_r18_present)},
    {3, (int32_t)offsetof(sl_bwp_prs_pool_cfg_r18_s, sl_prs_tx_pool_exceptional_r18_present)}};
const asn1::table::seq_desc sl_bwp_prs_pool_cfg_r18_s::desc = {{seq_table_sl_bwp_prs_pool_cfg_r18_s_fields,
                                                                uint64_t{0x0},
                                                                seq_table_sl_bwp_prs_pool_cfg_r18_s_opts,
                                                                seq_table_sl_bwp_prs_pool_cfg_r18_s_names,
                                                                4,
                                                                4},
                                                               false,
                                                               false,
                                                               nullptr,
                                                               0};

// SL-BWP-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_ps_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_pool_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_ps_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_ps_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_ps_r17_setup_release_alternatives,
    nullptr,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_ps_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_bwp_pool_cfg_r16_s>::type_offset(),
    setup_release_c<sl_bwp_pool_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_bwp_cfg_r16_s_sl_bwp_disc_pool_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_disc_pool_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_r16_s_sl_bwp_disc_pool_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_bwp_cfg_r16_s_sl_bwp_disc_pool_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_disc_pool_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_disc_pool_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_bwp_disc_pool_cfg_r17_s>::type_offset(),
    setup_release_c<sl_bwp_disc_pool_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_a2_x_r18_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_pool_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_a2_x_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_a2_x_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_a2_x_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_a2_x_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_bwp_pool_cfg_r16_s>::type_offset(),
    setup_release_c<sl_bwp_pool_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_bwp_cfg_r16_s_sl_bwp_prs_pool_cfg_r18_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_prs_pool_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_r16_s_sl_bwp_prs_pool_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_bwp_cfg_r16_s_sl_bwp_prs_pool_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_prs_pool_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_bwp_cfg_r16_s_sl_bwp_prs_pool_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_bwp_prs_pool_cfg_r18_s>::type_offset(),
    setup_release_c<sl_bwp_prs_pool_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_bwp_cfg_r16_s_fields[] = {
    {offsetof(sl_bwp_cfg_r16_s, sl_bwp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{4},
     nullptr},
    {offsetof(sl_bwp_cfg_r16_s, sl_bwp_generic_r16),
     (int32_t)offsetof(sl_bwp_cfg_r16_s, sl_bwp_generic_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_generic_r16_s::desc},
    {offsetof(sl_bwp_cfg_r16_s, sl_bwp_pool_cfg_r16),
     (int32_t)offsetof(sl_bwp_cfg_r16_s, sl_bwp_pool_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_bwp_pool_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_r16_s_names[] = {{"sl-BWP-Id", nullptr},
                                                                           {"sl-BWP-Generic-r16", nullptr},
                                                                           {"sl-BWP-PoolConfig-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_bwp_cfg_r16_s_opts[]  = {
    {1, (int32_t)offsetof(sl_bwp_cfg_r16_s, sl_bwp_generic_r16_present)},
    {2, (int32_t)offsetof(sl_bwp_cfg_r16_s, sl_bwp_pool_cfg_r16_present)}};
static const asn1::table::seq_field seq_table_sl_bwp_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_bwp_cfg_r16_s, sl_bwp_pool_cfg_ps_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_bwp_cfg_r16_s::sl_bwp_pool_cfg_ps_r17)>::value,
     &seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_ps_r17_setup_release},
    {offsetof(sl_bwp_cfg_r16_s, sl_bwp_disc_pool_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_bwp_cfg_r16_s::sl_bwp_disc_pool_cfg_r17)>::value,
     &seq_table_sl_bwp_cfg_r16_s_sl_bwp_disc_pool_cfg_r17_setup_release}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_r16_s_g0_names[] = {{"sl-BWP-PoolConfigPS-r17", nullptr},
                                                                              {"sl-BWP-DiscPoolConfig-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_bwp_cfg_r16_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                              {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sl_bwp_cfg_r16_s_g1_fields[] = {
    {offsetof(sl_bwp_cfg_r16_s, sl_bwp_pool_cfg_a2_x_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sl_bwp_cfg_r16_s::sl_bwp_pool_cfg_a2_x_r18)>::value,
      &seq_table_sl_bwp_cfg_r16_s_sl_bwp_pool_cfg_a2_x_r18_setup_release},
    {offsetof(sl_bwp_cfg_r16_s, sl_bwp_prs_pool_cfg_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sl_bwp_cfg_r16_s::sl_bwp_prs_pool_cfg_r18)>::value,
      &seq_table_sl_bwp_cfg_r16_s_sl_bwp_prs_pool_cfg_r18_setup_release}};
static const asn1::table::field_name seq_table_sl_bwp_cfg_r16_s_g1_names[] = {{"sl-BWP-PoolConfigA2X-r18", nullptr},
                                                                              {"sl-BWP-PRS-PoolConfig-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_bwp_cfg_r16_s_g1_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                              {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_sl_bwp_cfg_r16_s_groups[]   = {{{seq_table_sl_bwp_cfg_r16_s_g0_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_bwp_cfg_r16_s_g0_opts,
                                                                                seq_table_sl_bwp_cfg_r16_s_g0_names,
                                                                                2,
                                                                                2},
                                                                               false},
                                                                              {{seq_table_sl_bwp_cfg_r16_s_g1_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_bwp_cfg_r16_s_g1_opts,
                                                                                seq_table_sl_bwp_cfg_r16_s_g1_names,
                                                                                2,
                                                                                2},
                                                                               false}};
const asn1::table::seq_desc          sl_bwp_cfg_r16_s::desc                = {{seq_table_sl_bwp_cfg_r16_s_fields,
                                                                               uint64_t{0x1},
                                                                               seq_table_sl_bwp_cfg_r16_s_opts,
                                                                               seq_table_sl_bwp_cfg_r16_s_names,
                                                                               3,
                                                                               2},
                                                                              true,
                                                                              false,
                                                                              seq_table_sl_bwp_cfg_r16_s_groups,
                                                                              2};

// SL-SSB-TimeAllocation-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_ssb_time_alloc_r16_s_fields[] = {
    {offsetof(sl_ssb_time_alloc_r16_s, sl_num_ssb_within_period_r16),
     (int32_t)offsetof(sl_ssb_time_alloc_r16_s, sl_num_ssb_within_period_r16_present),
     asn1::table::enum_op<decltype(sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16)>(),
     decltype(sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16)::has_ext,
     false,
     decltype(sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16)::nof_types,
     decltype(sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16)::nof_exts,
     nullptr,
     &sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_opts::desc},
    {offsetof(sl_ssb_time_alloc_r16_s, sl_time_offset_ssb_r16),
     (int32_t)offsetof(sl_ssb_time_alloc_r16_s, sl_time_offset_ssb_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1279},
     nullptr},
    {offsetof(sl_ssb_time_alloc_r16_s, sl_time_interv_r16),
     (int32_t)offsetof(sl_ssb_time_alloc_r16_s, sl_time_interv_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{639},
     nullptr}};
static const asn1::table::field_name seq_table_sl_ssb_time_alloc_r16_s_names[] = {
    {"sl-NumSSB-WithinPeriod-r16", nullptr},
    {"sl-TimeOffsetSSB-r16", nullptr},
    {"sl-TimeInterval-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_ssb_time_alloc_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_ssb_time_alloc_r16_s, sl_num_ssb_within_period_r16_present)},
    {1, (int32_t)offsetof(sl_ssb_time_alloc_r16_s, sl_time_offset_ssb_r16_present)},
    {2, (int32_t)offsetof(sl_ssb_time_alloc_r16_s, sl_time_interv_r16_present)}};
const asn1::table::seq_desc sl_ssb_time_alloc_r16_s::desc = {{seq_table_sl_ssb_time_alloc_r16_s_fields,
                                                              uint64_t{0x0},
                                                              seq_table_sl_ssb_time_alloc_r16_s_opts,
                                                              seq_table_sl_ssb_time_alloc_r16_s_names,
                                                              3,
                                                              3},
                                                             false,
                                                             false,
                                                             nullptr,
                                                             0};

static const char* enum_names_sl_ssb_time_alloc_r16_s_sl_num_ssb_within_period_r16_e__values[] =
    {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
const asn1::table::enum_desc sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_opts::desc = {
    enum_names_sl_ssb_time_alloc_r16_s_sl_num_ssb_within_period_r16_e__values,
    7,
    "sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_e_"};
uint8_t sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "sl_ssb_time_alloc_r16_s::sl_num_ssb_within_period_r16_e_");
}

// SL-SyncConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sync_cfg_r16_s_tx_params_r16_s__fields[] = {
    {offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_tx_thresh_ic_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_tx_thresh_ic_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_tx_thresh_oo_c_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_tx_thresh_oo_c_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_info_reserved_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_info_reserved_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_sync_cfg_r16_s::tx_params_r16_s_::sync_info_reserved_r16),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sl_sync_cfg_r16_s_tx_params_r16_s__names[] = {
    {"syncTxThreshIC-r16", nullptr},
    {"syncTxThreshOoC-r16", nullptr},
    {"syncInfoReserved-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_sync_cfg_r16_s_tx_params_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_tx_thresh_ic_r16_present)},
    {1, (int32_t)offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_tx_thresh_oo_c_r16_present)},
    {2, (int32_t)offsetof(sl_sync_cfg_r16_s::tx_params_r16_s_, sync_info_reserved_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_sync_cfg_r16_s_tx_params_r16_s_ = {
    {seq_table_sl_sync_cfg_r16_s_tx_params_r16_s__fields,
     uint64_t{0x0},
     seq_table_sl_sync_cfg_r16_s_tx_params_r16_s__opts,
     seq_table_sl_sync_cfg_r16_s_tx_params_r16_s__names,
     3,
     3},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_sl_sync_cfg_r16_s_fields[] = {
    {offsetof(sl_sync_cfg_r16_s, sl_sync_ref_min_hyst_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s, sl_sync_ref_min_hyst_r16_present),
     asn1::table::enum_op<decltype(sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16)>(),
     decltype(sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16)::has_ext,
     false,
     decltype(sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16)::nof_types,
     decltype(sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16)::nof_exts,
     nullptr,
     &sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_opts::desc},
    {offsetof(sl_sync_cfg_r16_s, sl_sync_ref_diff_hyst_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s, sl_sync_ref_diff_hyst_r16_present),
     asn1::table::enum_op<decltype(sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16)>(),
     decltype(sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16)::has_ext,
     false,
     decltype(sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16)::nof_types,
     decltype(sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16)::nof_exts,
     nullptr,
     &sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_opts::desc},
    {offsetof(sl_sync_cfg_r16_s, sl_filt_coef_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s, sl_filt_coef_r16_present),
     asn1::table::enum_op<decltype(sl_sync_cfg_r16_s::sl_filt_coef_r16)>(),
     decltype(sl_sync_cfg_r16_s::sl_filt_coef_r16)::has_ext,
     false,
     decltype(sl_sync_cfg_r16_s::sl_filt_coef_r16)::nof_types,
     decltype(sl_sync_cfg_r16_s::sl_filt_coef_r16)::nof_exts,
     nullptr,
     &filt_coef_opts::desc},
    {offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc1_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc1_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ssb_time_alloc_r16_s::desc},
    {offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc2_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc2_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ssb_time_alloc_r16_s::desc},
    {offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc3_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc3_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ssb_time_alloc_r16_s::desc},
    {offsetof(sl_sync_cfg_r16_s, sl_s_si_d_r16),
     (int32_t)offsetof(sl_sync_cfg_r16_s, sl_s_si_d_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{671},
     nullptr},
    {offsetof(sl_sync_cfg_r16_s, tx_params_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_sync_cfg_r16_s_tx_params_r16_s_},
    {0,
     (int32_t)offsetof(sl_sync_cfg_r16_s, gnss_sync_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_sync_cfg_r16_s_names[] = {{"sl-SyncRefMinHyst-r16", nullptr},
                                                                            {"sl-SyncRefDiffHyst-r16", nullptr},
                                                                            {"sl-FilterCoefficient-r16", nullptr},
                                                                            {"sl-SSB-TimeAllocation1-r16", nullptr},
                                                                            {"sl-SSB-TimeAllocation2-r16", nullptr},
                                                                            {"sl-SSB-TimeAllocation3-r16", nullptr},
                                                                            {"sl-SSID-r16", nullptr},
                                                                            {"txParameters-r16", nullptr},
                                                                            {"gnss-Sync-r16", "true"}};
static const asn1::table::seq_opt    seq_table_sl_sync_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_sync_cfg_r16_s, sl_sync_ref_min_hyst_r16_present)},
    {1, (int32_t)offsetof(sl_sync_cfg_r16_s, sl_sync_ref_diff_hyst_r16_present)},
    {2, (int32_t)offsetof(sl_sync_cfg_r16_s, sl_filt_coef_r16_present)},
    {3, (int32_t)offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc1_r16_present)},
    {4, (int32_t)offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc2_r16_present)},
    {5, (int32_t)offsetof(sl_sync_cfg_r16_s, sl_ssb_time_alloc3_r16_present)},
    {6, (int32_t)offsetof(sl_sync_cfg_r16_s, sl_s_si_d_r16_present)},
    {8, (int32_t)offsetof(sl_sync_cfg_r16_s, gnss_sync_r16_present)}};
const asn1::table::seq_desc sl_sync_cfg_r16_s::desc = {{seq_table_sl_sync_cfg_r16_s_fields,
                                                        uint64_t{0x80},
                                                        seq_table_sl_sync_cfg_r16_s_opts,
                                                        seq_table_sl_sync_cfg_r16_s_names,
                                                        9,
                                                        8},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

static const char*           enum_names_sl_sync_cfg_r16_s_sl_sync_ref_min_hyst_r16_e__values[] = {"dB0",
                                                                                                  "dB3",
                                                                                                  "dB6",
                                                                                                  "dB9",
                                                                                                  "dB12"};
const asn1::table::enum_desc sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_opts::desc            = {
    enum_names_sl_sync_cfg_r16_s_sl_sync_ref_min_hyst_r16_e__values,
    5,
    "sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_e_"};
uint8_t sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 3, 6, 9, 12};
  return map_enum_number(numbers, 5, value, "sl_sync_cfg_r16_s::sl_sync_ref_min_hyst_r16_e_");
}

static const char* enum_names_sl_sync_cfg_r16_s_sl_sync_ref_diff_hyst_r16_e__values[] =
    {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
const asn1::table::enum_desc sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_opts::desc = {
    enum_names_sl_sync_cfg_r16_s_sl_sync_ref_diff_hyst_r16_e__values,
    6,
    "sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_e_"};
uint8_t sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 3, 6, 9, 12};
  return map_enum_number(numbers, 5, value, "sl_sync_cfg_r16_s::sl_sync_ref_diff_hyst_r16_e_");
}

// SL-FreqConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_freq_cfg_r16_s_fields[] = {
    {offsetof(sl_freq_cfg_r16_s, sl_freq_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_freq_cfg_r16_s, sl_scs_specific_carrier_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(sl_freq_cfg_r16_s::sl_scs_specific_carrier_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scs_specific_carrier_s::desc},
    {offsetof(sl_freq_cfg_r16_s, sl_absolute_freq_point_a_r16),
     (int32_t)offsetof(sl_freq_cfg_r16_s, sl_absolute_freq_point_a_r16_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{3279165},
     nullptr},
    {offsetof(sl_freq_cfg_r16_s, sl_absolute_freq_ssb_r16),
     (int32_t)offsetof(sl_freq_cfg_r16_s, sl_absolute_freq_ssb_r16_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{3279165},
     nullptr},
    {0,
     (int32_t)offsetof(sl_freq_cfg_r16_s, freq_shift7p5khz_sl_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_freq_cfg_r16_s, value_n_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-1},
     uint64_t{2},
     nullptr},
    {offsetof(sl_freq_cfg_r16_s, sl_bwp_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::list_codec<decltype(sl_freq_cfg_r16_s::sl_bwp_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{4}, false, false>>::value},
    {offsetof(sl_freq_cfg_r16_s, sl_bwp_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::item_list_codec<decltype(sl_freq_cfg_r16_s::sl_bwp_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_bwp_cfg_r16_s::desc},
    {offsetof(sl_freq_cfg_r16_s, sl_sync_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_freq_cfg_r16_s::sl_sync_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_sync_cfg_r16_s::desc},
    {offsetof(sl_freq_cfg_r16_s, sl_sync_prio_r16),
     (int32_t)offsetof(sl_freq_cfg_r16_s, sl_sync_prio_r16_present),
     asn1::table::enum_op<decltype(sl_freq_cfg_r16_s::sl_sync_prio_r16)>(),
     decltype(sl_freq_cfg_r16_s::sl_sync_prio_r16)::has_ext,
     false,
     decltype(sl_freq_cfg_r16_s::sl_sync_prio_r16)::nof_types,
     decltype(sl_freq_cfg_r16_s::sl_sync_prio_r16)::nof_exts,
     nullptr,
     &sl_freq_cfg_r16_s::sl_sync_prio_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_freq_cfg_r16_s_names[] = {{"sl-Freq-Id-r16", nullptr},
                                                                            {"sl-SCS-SpecificCarrierList-r16", nullptr},
                                                                            {"sl-AbsoluteFrequencyPointA-r16", nullptr},
                                                                            {"sl-AbsoluteFrequencySSB-r16", nullptr},
                                                                            {"frequencyShift7p5khzSL-r16", "true"},
                                                                            {"valueN-r16", nullptr},
                                                                            {"sl-BWP-ToReleaseList-r16", nullptr},
                                                                            {"sl-BWP-ToAddModList-r16", nullptr},
                                                                            {"sl-SyncConfigList-r16", nullptr},
                                                                            {"sl-SyncPriority-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_freq_cfg_r16_s_opts[]  = {
    {2, (int32_t)offsetof(sl_freq_cfg_r16_s, sl_absolute_freq_point_a_r16_present)},
    {3, (int32_t)offsetof(sl_freq_cfg_r16_s, sl_absolute_freq_ssb_r16_present)},
    {4, (int32_t)offsetof(sl_freq_cfg_r16_s, freq_shift7p5khz_sl_r16_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence},
    {8, asn1::table::seq_field::list_presence},
    {9, (int32_t)offsetof(sl_freq_cfg_r16_s, sl_sync_prio_r16_present)}};
const asn1::table::seq_desc sl_freq_cfg_r16_s::desc = {{seq_table_sl_freq_cfg_r16_s_fields,
                                                        uint64_t{0x23},
                                                        seq_table_sl_freq_cfg_r16_s_opts,
                                                        seq_table_sl_freq_cfg_r16_s_names,
                                                        10,
                                                        7},
                                                       false,
                                                       false,
                                                       nullptr,
                                                       0};

static const char*           enum_names_sl_freq_cfg_r16_s_sl_sync_prio_r16_e__values[] = {"gnss", "gnbEnb"};
const asn1::table::enum_desc sl_freq_cfg_r16_s::sl_sync_prio_r16_opts::desc            = {
    enum_names_sl_freq_cfg_r16_s_sl_sync_prio_r16_e__values,
    2,
    "sl_freq_cfg_r16_s::sl_sync_prio_r16_e_"};

// SL-RLC-Config-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sl_rlc_cfg_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, false>::value,
    &asn1::table::alternative_ops<sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_, false>::value};
const asn1::table::choice_storage_desc sl_rlc_cfg_r16_c::storage_desc = {choice_storage_sl_rlc_cfg_r16_c_alternatives,
                                                                         2,
                                                                         "sl_rlc_cfg_r16_c"};

sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_& sl_rlc_cfg_r16_c::set_sl_am_rlc_r16()
{
  set(types::sl_am_rlc_r16);
  return c.get<sl_am_rlc_r16_s_>();
}
sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_& sl_rlc_cfg_r16_c::set_sl_um_rlc_r16()
{
  set(types::sl_um_rlc_r16);
  return c.get<sl_um_rlc_r16_s_>();
}
static const asn1::table::seq_field seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__fields[] = {
    {offsetof(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, sl_sn_field_len_am_r16),
     (int32_t)offsetof(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, sl_sn_field_len_am_r16_present),
     asn1::table::enum_op<decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)>(),
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)::nof_types,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)::nof_exts,
     nullptr,
     &sn_field_len_am_opts::desc},
    {offsetof(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, sl_t_poll_retx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_t_poll_retx_r16)>(),
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_t_poll_retx_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_t_poll_retx_r16)::nof_types,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_t_poll_retx_r16)::nof_exts,
     nullptr,
     &t_poll_retx_opts::desc},
    {offsetof(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, sl_poll_pdu_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_pdu_r16)>(),
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_pdu_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_pdu_r16)::nof_types,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_pdu_r16)::nof_exts,
     nullptr,
     &poll_pdu_opts::desc},
    {offsetof(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, sl_poll_byte_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_byte_r16)>(),
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_byte_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_byte_r16)::nof_types,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_poll_byte_r16)::nof_exts,
     nullptr,
     &poll_byte_opts::desc},
    {offsetof(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, sl_max_retx_thres_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16)>(),
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16)::nof_types,
     decltype(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16)::nof_exts,
     nullptr,
     &sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__names[] = {
    {"sl-SN-FieldLengthAM-r16", nullptr},
    {"sl-T-PollRetransmit-r16", nullptr},
    {"sl-PollPDU-r16", nullptr},
    {"sl-PollByte-r16", nullptr},
    {"sl-MaxRetxThreshold-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_, sl_sn_field_len_am_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s_ = {
    {seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__fields,
     uint64_t{0x1e},
     seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__opts,
     seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__names,
     5,
     1},
    true,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s__fields[] = {
    {offsetof(sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_, sl_sn_field_len_um_r16),
     (int32_t)offsetof(sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_, sl_sn_field_len_um_r16_present),
     asn1::table::enum_op<decltype(sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_::sl_sn_field_len_um_r16)>(),
     decltype(sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_::sl_sn_field_len_um_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_::sl_sn_field_len_um_r16)::nof_types,
     decltype(sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_::sl_sn_field_len_um_r16)::nof_exts,
     nullptr,
     &sn_field_len_um_opts::desc}};
static const asn1::table::field_name seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s__names[] = {
    {"sl-SN-FieldLengthUM-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_rlc_cfg_r16_c::sl_um_rlc_r16_s_, sl_sn_field_len_um_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s_ = {
    {seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s__fields,
     uint64_t{0x0},
     seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s__opts,
     seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s__names,
     1,
     1},
    true,
    false,
    nullptr,
    0};
static const asn1::table::seq_field choice_table_sl_rlc_cfg_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_rlc_cfg_r16_c_sl_um_rlc_r16_s_}};
static const asn1::table::field_name choice_table_sl_rlc_cfg_r16_c_names[] = {{"sl-AM-RLC-r16", nullptr},
                                                                              {"sl-UM-RLC-r16", nullptr}};
const asn1::table::choice_desc       sl_rlc_cfg_r16_c::desc = {asn1::table::enum_int_type<sl_rlc_cfg_r16_c::types>(),
                                                               sl_rlc_cfg_r16_c::types::nof_types,
                                                               sl_rlc_cfg_r16_c::types::nof_exts,
                                                               sl_rlc_cfg_r16_c::types::has_ext,
                                                               false,
                                                               choice_table_sl_rlc_cfg_r16_c_alternatives,
                                                               nullptr,
                                                               choice_table_sl_rlc_cfg_r16_c_names,
                                                               "sl_rlc_cfg_r16_c",
                                                               offsetof(sl_rlc_cfg_r16_c, c.type),
                                                               offsetof(sl_rlc_cfg_r16_c, c.value),
                                                               false,
                                                               &sl_rlc_cfg_r16_c::storage_desc};

static const char* enum_names_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__sl_max_retx_thres_r16_e__values[] =
    {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
const asn1::table::enum_desc sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_opts::desc = {
    enum_names_sl_rlc_cfg_r16_c_sl_am_rlc_r16_s__sl_max_retx_thres_r16_e__values,
    8,
    "sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_e_"};
uint8_t sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sl_rlc_cfg_r16_c::sl_am_rlc_r16_s_::sl_max_retx_thres_r16_e_");
}

static const char*           enum_names_sl_rlc_cfg_r16_c_types_values[] = {"sl-AM-RLC-r16", "sl-UM-RLC-r16"};
const asn1::table::enum_desc sl_rlc_cfg_r16_c::types_opts::desc         = {enum_names_sl_rlc_cfg_r16_c_types_values,
                                                                           2,
                                                                           "sl_rlc_cfg_r16_c::types"};

// SL-LogicalChannelConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_lc_ch_cfg_r16_s_fields[] = {
    {offsetof(sl_lc_ch_cfg_r16_s, sl_prio_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_prioritised_bit_rate_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16)>(),
     decltype(sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16)::has_ext,
     false,
     decltype(sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16)::nof_types,
     decltype(sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16)::nof_exts,
     nullptr,
     &sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_opts::desc},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_bucket_size_dur_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16)>(),
     decltype(sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16)::has_ext,
     false,
     decltype(sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16)::nof_types,
     decltype(sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16)::nof_exts,
     nullptr,
     &sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_opts::desc},
    {0,
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_cfg_grant_type1_allowed_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_harq_feedback_enabled_r16),
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_harq_feedback_enabled_r16_present),
     asn1::table::enum_op<decltype(sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16)>(),
     decltype(sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16)::has_ext,
     false,
     decltype(sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16)::nof_types,
     decltype(sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16)::nof_exts,
     nullptr,
     &sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16_opts::desc},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_allowed_cg_list_r16),
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_allowed_cg_list_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     7,
     &asn1::table::list_codec<decltype(sl_lc_ch_cfg_r16_s::sl_allowed_cg_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{7}, false, false>>::value},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_allowed_scs_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<decltype(sl_lc_ch_cfg_r16_s::sl_allowed_scs_list_r16), asn1::table::string_json>::value},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_max_pusch_dur_r16),
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_max_pusch_dur_r16_present),
     asn1::table::enum_op<decltype(sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16)>(),
     decltype(sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16)::has_ext,
     false,
     decltype(sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16)::nof_types,
     decltype(sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16)::nof_exts,
     nullptr,
     &sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16_opts::desc},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_lc_ch_group_r16),
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_lc_ch_group_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_sched_request_id_r16),
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_sched_request_id_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_lc_ch_sr_delay_timer_applied_r16),
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_lc_ch_sr_delay_timer_applied_r16_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_lc_ch_cfg_r16_s_names[] = {
    {"sl-Priority-r16", nullptr},
    {"sl-PrioritisedBitRate-r16", nullptr},
    {"sl-BucketSizeDuration-r16", nullptr},
    {"sl-ConfiguredGrantType1Allowed-r16", "true"},
    {"sl-HARQ-FeedbackEnabled-r16", nullptr},
    {"sl-AllowedCG-List-r16", nullptr},
    {"sl-AllowedSCS-List-r16", nullptr},
    {"sl-MaxPUSCH-Duration-r16", nullptr},
    {"sl-LogicalChannelGroup-r16", nullptr},
    {"sl-SchedulingRequestId-r16", nullptr},
    {"sl-LogicalChannelSR-DelayTimerApplied-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_lc_ch_cfg_r16_s_opts[] = {
    {3, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_cfg_grant_type1_allowed_r16_present)},
    {4, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_harq_feedback_enabled_r16_present)},
    {5, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_allowed_cg_list_r16_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_max_pusch_dur_r16_present)},
    {8, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_lc_ch_group_r16_present)},
    {9, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_sched_request_id_r16_present)},
    {10, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_lc_ch_sr_delay_timer_applied_r16_present)}};
static const asn1::table::seq_field seq_table_sl_lc_ch_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_lc_ch_cfg_r16_s, sl_ch_access_prio_r18),
     (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_ch_access_prio_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{3},
     nullptr},
    {offsetof(sl_lc_ch_cfg_r16_s, sl_allowed_carriers_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_list_codec<decltype(sl_lc_ch_cfg_r16_s::sl_allowed_carriers_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{1}, uint64_t{7}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_lc_ch_cfg_r16_s_g0_names[] = {
    {"sl-ChannelAccessPriority-r18", nullptr},
    {"sl-AllowedCarriers-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_lc_ch_cfg_r16_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_lc_ch_cfg_r16_s, sl_ch_access_prio_r18_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_lc_ch_cfg_r16_s_groups[] = {{{seq_table_sl_lc_ch_cfg_r16_s_g0_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_sl_lc_ch_cfg_r16_s_g0_opts,
                                                                               seq_table_sl_lc_ch_cfg_r16_s_g0_names,
                                                                               2,
                                                                               2},
                                                                              false}};
const asn1::table::seq_desc         sl_lc_ch_cfg_r16_s::desc              = {{seq_table_sl_lc_ch_cfg_r16_s_fields,
                                                                              uint64_t{0x7},
                                                                              seq_table_sl_lc_ch_cfg_r16_s_opts,
                                                                              seq_table_sl_lc_ch_cfg_r16_s_names,
                                                                              11,
                                                                              8},
                                                                             true,
                                                                             false,
                                                                             seq_table_sl_lc_ch_cfg_r16_s_groups,
                                                                             1};

static const char*           enum_names_sl_lc_ch_cfg_r16_s_sl_prioritised_bit_rate_r16_e__values[] = {"kBps0",
                                                                                                      "kBps8",
                                                                                                      "kBps16",
                                                                                                      "kBps32",
                                                                                                      "kBps64",
                                                                                                      "kBps128",
                                                                                                      "kBps256",
                                                                                                      "kBps512",
                                                                                                      "kBps1024",
                                                                                                      "kBps2048",
                                                                                                      "kBps4096",
                                                                                                      "kBps8192",
                                                                                                      "kBps16384",
                                                                                                      "kBps32768",
                                                                                                      "kBps65536",
                                                                                                      "infinity"};
const asn1::table::enum_desc sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_opts::desc            = {
    enum_names_sl_lc_ch_cfg_r16_s_sl_prioritised_bit_rate_r16_e__values,
    16,
    "sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_e_"};
int32_t sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_opts::to_number() const
{
  static const int32_t numbers[] = {0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, -1};
  return map_enum_number(numbers, 16, value, "sl_lc_ch_cfg_r16_s::sl_prioritised_bit_rate_r16_e_");
}

static const char*           enum_names_sl_lc_ch_cfg_r16_s_sl_bucket_size_dur_r16_e__values[] = {"ms5",
                                                                                                 "ms10",
                                                                                                 "ms20",
                                                                                                 "ms50",
                                                                                                 "ms100",
                                                                                                 "ms150",
                                                                                                 "ms300",
                                                                                                 "ms500",
                                                                                                 "ms1000",
                                                                                                 "spare7",
                                                                                                 "spare6",
                                                                                                 "spare5",
                                                                                                 "spare4",
                                                                                                 "spare3",
                                                                                                 "spare2",
                                                                                                 "spare1"};
const asn1::table::enum_desc sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_opts::desc            = {
    enum_names_sl_lc_ch_cfg_r16_s_sl_bucket_size_dur_r16_e__values,
    16,
    "sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_e_"};
uint16_t sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 50, 100, 150, 300, 500, 1000};
  return map_enum_number(numbers, 9, value, "sl_lc_ch_cfg_r16_s::sl_bucket_size_dur_r16_e_");
}

static const char* enum_names_sl_lc_ch_cfg_r16_s_sl_harq_feedback_enabled_r16_e__values[] = {"enabled", "disabled"};
const asn1::table::enum_desc sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16_opts::desc  = {
    enum_names_sl_lc_ch_cfg_r16_s_sl_harq_feedback_enabled_r16_e__values,
    2,
    "sl_lc_ch_cfg_r16_s::sl_harq_feedback_enabled_r16_e_"};

static const char* enum_names_sl_lc_ch_cfg_r16_s_sl_max_pusch_dur_r16_e__values[] =
    {"ms0p02", "ms0p04", "ms0p0625", "ms0p125", "ms0p25", "ms0p5", "spare2", "spare1"};
const asn1::table::enum_desc sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16_opts::desc = {
    enum_names_sl_lc_ch_cfg_r16_s_sl_max_pusch_dur_r16_e__values,
    8,
    "sl_lc_ch_cfg_r16_s::sl_max_pusch_dur_r16_e_"};

// SL-RLC-BearerConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_rlc_bearer_cfg_r16_s_fields[] = {
    {offsetof(sl_rlc_bearer_cfg_r16_s, sl_rlc_bearer_cfg_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr},
    {offsetof(sl_rlc_bearer_cfg_r16_s, sl_served_radio_bearer_r16),
     (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_served_radio_bearer_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr},
    {offsetof(sl_rlc_bearer_cfg_r16_s, sl_rlc_cfg_r16),
     (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_rlc_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_rlc_cfg_r16_c::desc},
    {offsetof(sl_rlc_bearer_cfg_r16_s, sl_mac_lc_ch_cfg_r16),
     (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_mac_lc_ch_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_lc_ch_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_rlc_bearer_cfg_r16_s_names[] = {
    {"sl-RLC-BearerConfigIndex-r16", nullptr},
    {"sl-ServedRadioBearer-r16", nullptr},
    {"sl-RLC-Config-r16", nullptr},
    {"sl-MAC-LogicalChannelConfig-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_bearer_cfg_r16_s_opts[] = {
    {1, (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_served_radio_bearer_r16_present)},
    {2, (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_rlc_cfg_r16_present)},
    {3, (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_mac_lc_ch_cfg_r16_present)}};
static const asn1::table::seq_field seq_table_sl_rlc_bearer_cfg_r16_s_g0_fields[] = {
    {offsetof(sl_rlc_bearer_cfg_r16_s, sl_rlc_bearer_cfg_idx_v1800),
     (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_rlc_bearer_cfg_idx_v1800_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{513},
     uint64_t{511},
     nullptr}};
static const asn1::table::field_name seq_table_sl_rlc_bearer_cfg_r16_s_g0_names[] = {
    {"sl-RLC-BearerConfigIndex-v1800", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_bearer_cfg_r16_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_rlc_bearer_cfg_r16_s, sl_rlc_bearer_cfg_idx_v1800_present)}};
static const asn1::table::seq_group seq_table_sl_rlc_bearer_cfg_r16_s_groups[] = {
    {{seq_table_sl_rlc_bearer_cfg_r16_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_rlc_bearer_cfg_r16_s_g0_opts,
      seq_table_sl_rlc_bearer_cfg_r16_s_g0_names,
      1,
      1},
     false}};
const asn1::table::seq_desc sl_rlc_bearer_cfg_r16_s::desc = {{seq_table_sl_rlc_bearer_cfg_r16_s_fields,
                                                              uint64_t{0x1},
                                                              seq_table_sl_rlc_bearer_cfg_r16_s_opts,
                                                              seq_table_sl_rlc_bearer_cfg_r16_s_names,
                                                              4,
                                                              3},
                                                             true,
                                                             false,
                                                             seq_table_sl_rlc_bearer_cfg_r16_s_groups,
                                                             1};

// SL-PHY-MAC-RLC-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_sched_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_sched_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_sched_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_sched_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_sched_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_sched_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_sched_cfg_r16_s>::type_offset(),
    setup_release_c<sl_sched_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_ue_sel_cfg_r16_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &sl_ue_sel_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_ue_sel_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_ue_sel_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_ue_sel_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_ue_sel_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_ue_sel_cfg_r16_s>::type_offset(),
    setup_release_c<sl_ue_sel_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_csi_sched_request_id_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{7},
         nullptr}};
static const asn1::table::field_name
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_csi_sched_request_id_r16_setup_release_names[] = {{"release", nullptr},
                                                                                            {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_csi_sched_request_id_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_csi_sched_request_id_r16_setup_release_alternatives,
    nullptr,
    seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_csi_sched_request_id_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 0, 7>>::type_offset(),
    setup_release_c<integer<uint8_t, 0, 7>>::value_offset()};
static const asn1::table::seq_field seq_table_sl_phy_mac_rlc_cfg_r16_s_fields[] = {
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_sched_cfg_r16),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_sched_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_sched_cfg_r16_setup_release},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_ue_sel_cfg_r16),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_ue_sel_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_ue_sel_cfg_r16_setup_release},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_freq_info_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_phy_mac_rlc_cfg_r16_s::sl_freq_info_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{7}, false, false>>::value},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_freq_info_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_phy_mac_rlc_cfg_r16_s::sl_freq_info_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_cfg_r16_s::desc},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_rlc_bearer_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::list_codec<decltype(sl_phy_mac_rlc_cfg_r16_s::sl_rlc_bearer_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{511}, false, false>>::value},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_rlc_bearer_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::item_list_codec<decltype(sl_phy_mac_rlc_cfg_r16_s::sl_rlc_bearer_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_rlc_bearer_cfg_r16_s::desc},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_max_num_consecutive_dtx_r16),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_max_num_consecutive_dtx_r16_present),
     asn1::table::enum_op<decltype(sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16)>(),
     decltype(sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16)::has_ext,
     false,
     decltype(sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16)::nof_types,
     decltype(sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16)::nof_exts,
     nullptr,
     &sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_opts::desc},
    {0,
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_csi_acquisition_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_csi_sched_request_id_r16),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_csi_sched_request_id_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_phy_mac_rlc_cfg_r16_s_sl_csi_sched_request_id_r16_setup_release},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_ssb_prio_nr_r16),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_ssb_prio_nr_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_phy_mac_rlc_cfg_r16_s, network_ctrl_sync_tx_r16),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, network_ctrl_sync_tx_r16_present),
     asn1::table::enum_op<decltype(sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16)>(),
     decltype(sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16)::has_ext,
     false,
     decltype(sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16)::nof_types,
     decltype(sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16)::nof_exts,
     nullptr,
     &sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_phy_mac_rlc_cfg_r16_s_names[] = {
    {"sl-ScheduledConfig-r16", nullptr},
    {"sl-UE-SelectedConfig-r16", nullptr},
    {"sl-FreqInfoToReleaseList-r16", nullptr},
    {"sl-FreqInfoToAddModList-r16", nullptr},
    {"sl-RLC-BearerToReleaseList-r16", nullptr},
    {"sl-RLC-BearerToAddModList-r16", nullptr},
    {"sl-MaxNumConsecutiveDTX-r16", nullptr},
    {"sl-CSI-Acquisition-r16", "enabled"},
    {"sl-CSI-SchedulingRequestId-r16", nullptr},
    {"sl-SSB-PriorityNR-r16", nullptr},
    {"networkControlledSyncTx-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_phy_mac_rlc_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_sched_cfg_r16_present)},
    {1, (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_ue_sel_cfg_r16_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_max_num_consecutive_dtx_r16_present)},
    {7, (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_csi_acquisition_r16_present)},
    {8, (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_csi_sched_request_id_r16_present)},
    {9, (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, sl_ssb_prio_nr_r16_present)},
    {10, (int32_t)offsetof(sl_phy_mac_rlc_cfg_r16_s, network_ctrl_sync_tx_r16_present)}};
const asn1::table::seq_desc sl_phy_mac_rlc_cfg_r16_s::desc = {{seq_table_sl_phy_mac_rlc_cfg_r16_s_fields,
                                                               uint64_t{0x0},
                                                               seq_table_sl_phy_mac_rlc_cfg_r16_s_opts,
                                                               seq_table_sl_phy_mac_rlc_cfg_r16_s_names,
                                                               11,
                                                               11},
                                                              false,
                                                              false,
                                                              nullptr,
                                                              0};

static const char* enum_names_sl_phy_mac_rlc_cfg_r16_s_sl_max_num_consecutive_dtx_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n6", "n8", "n16", "n32"};
const asn1::table::enum_desc sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_opts::desc = {
    enum_names_sl_phy_mac_rlc_cfg_r16_s_sl_max_num_consecutive_dtx_r16_e__values,
    8,
    "sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_e_"};
uint8_t sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sl_phy_mac_rlc_cfg_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}

static const char*           enum_names_sl_phy_mac_rlc_cfg_r16_s_network_ctrl_sync_tx_r16_e__values[] = {"on", "off"};
const asn1::table::enum_desc sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16_opts::desc            = {
    enum_names_sl_phy_mac_rlc_cfg_r16_s_network_ctrl_sync_tx_r16_e__values,
    2,
    "sl_phy_mac_rlc_cfg_r16_s::network_ctrl_sync_tx_r16_e_"};

// SL-PQI-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sl_pqi_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, false>::value};
const asn1::table::choice_storage_desc sl_pqi_r16_c::storage_desc = {choice_storage_sl_pqi_r16_c_alternatives,
                                                                     2,
                                                                     "sl_pqi_r16_c"};

uint16_t& sl_pqi_r16_c::set_sl_standardized_pqi_r16()
{
  set(types::sl_standardized_pqi_r16);
  return c.get<uint16_t>();
}
sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_& sl_pqi_r16_c::set_sl_non_standardized_pqi_r16()
{
  set(types::sl_non_standardized_pqi_r16);
  return c.get<sl_non_standardized_pqi_r16_s_>();
}
static const asn1::table::seq_field seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__fields[] = {
    {offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_res_type_r16),
     (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_res_type_r16_present),
     asn1::table::enum_op<decltype(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16)>(),
     decltype(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16)::has_ext,
     false,
     decltype(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16)::nof_types,
     decltype(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16)::nof_exts,
     nullptr,
     &sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16_opts::desc},
    {offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_prio_level_r16),
     (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_prio_level_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{7},
     nullptr},
    {offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_packet_delay_budget_r16),
     (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_packet_delay_budget_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_packet_error_rate_r16),
     (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_packet_error_rate_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{9},
     nullptr},
    {offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_averaging_win_r16),
     (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_averaging_win_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_max_data_burst_volume_r16),
     (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_max_data_burst_volume_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{4095},
     nullptr}};
static const asn1::table::field_name seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__names[] = {
    {"sl-ResourceType-r16", nullptr},
    {"sl-PriorityLevel-r16", nullptr},
    {"sl-PacketDelayBudget-r16", nullptr},
    {"sl-PacketErrorRate-r16", nullptr},
    {"sl-AveragingWindow-r16", nullptr},
    {"sl-MaxDataBurstVolume-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_res_type_r16_present)},
    {1, (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_prio_level_r16_present)},
    {2, (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_packet_delay_budget_r16_present)},
    {3, (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_packet_error_rate_r16_present)},
    {4, (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_averaging_win_r16_present)},
    {5, (int32_t)offsetof(sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_, sl_max_data_burst_volume_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s_ = {
    {seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__fields,
     uint64_t{0x0},
     seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__opts,
     seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__names,
     6,
     6},
    true,
    false,
    nullptr,
    0};
static const asn1::table::seq_field choice_table_sl_pqi_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s_}};
static const asn1::table::field_name choice_table_sl_pqi_r16_c_names[] = {{"sl-StandardizedPQI-r16", nullptr},
                                                                          {"sl-Non-StandardizedPQI-r16", nullptr}};
const asn1::table::choice_desc       sl_pqi_r16_c::desc = {asn1::table::enum_int_type<sl_pqi_r16_c::types>(),
                                                           sl_pqi_r16_c::types::nof_types,
                                                           sl_pqi_r16_c::types::nof_exts,
                                                           sl_pqi_r16_c::types::has_ext,
                                                           false,
                                                           choice_table_sl_pqi_r16_c_alternatives,
                                                           nullptr,
                                                           choice_table_sl_pqi_r16_c_names,
                                                           "sl_pqi_r16_c",
                                                           offsetof(sl_pqi_r16_c, c.type),
                                                           offsetof(sl_pqi_r16_c, c.value),
                                                           false,
                                                           &sl_pqi_r16_c::storage_desc};

static const char* enum_names_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__sl_res_type_r16_e__values[] =
    {"gbr", "non-GBR", "delayCriticalGBR", "spare1"};
const asn1::table::enum_desc sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16_opts::desc = {
    enum_names_sl_pqi_r16_c_sl_non_standardized_pqi_r16_s__sl_res_type_r16_e__values,
    4,
    "sl_pqi_r16_c::sl_non_standardized_pqi_r16_s_::sl_res_type_r16_e_"};

static const char* enum_names_sl_pqi_r16_c_types_values[]   = {"sl-StandardizedPQI-r16", "sl-Non-StandardizedPQI-r16"};
const asn1::table::enum_desc sl_pqi_r16_c::types_opts::desc = {enum_names_sl_pqi_r16_c_types_values,
                                                               2,
                                                               "sl_pqi_r16_c::types"};

// SL-QoS-Profile-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_qos_profile_r16_s_fields[] = {
    {offsetof(sl_qos_profile_r16_s, sl_pqi_r16),
     (int32_t)offsetof(sl_qos_profile_r16_s, sl_pqi_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pqi_r16_c::desc},
    {offsetof(sl_qos_profile_r16_s, sl_gfbr_r16),
     (int32_t)offsetof(sl_qos_profile_r16_s, sl_gfbr_r16_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{4000000000},
     nullptr},
    {offsetof(sl_qos_profile_r16_s, sl_mfbr_r16),
     (int32_t)offsetof(sl_qos_profile_r16_s, sl_mfbr_r16_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{4000000000},
     nullptr},
    {offsetof(sl_qos_profile_r16_s, sl_range_r16),
     (int32_t)offsetof(sl_qos_profile_r16_s, sl_range_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{999},
     nullptr}};
static const asn1::table::field_name seq_table_sl_qos_profile_r16_s_names[] = {{"sl-PQI-r16", nullptr},
                                                                               {"sl-GFBR-r16", nullptr},
                                                                               {"sl-MFBR-r16", nullptr},
                                                                               {"sl-Range-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_qos_profile_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_qos_profile_r16_s, sl_pqi_r16_present)},
    {1, (int32_t)offsetof(sl_qos_profile_r16_s, sl_gfbr_r16_present)},
    {2, (int32_t)offsetof(sl_qos_profile_r16_s, sl_mfbr_r16_present)},
    {3, (int32_t)offsetof(sl_qos_profile_r16_s, sl_range_r16_present)}};
const asn1::table::seq_desc sl_qos_profile_r16_s::desc = {{seq_table_sl_qos_profile_r16_s_fields,
                                                           uint64_t{0x0},
                                                           seq_table_sl_qos_profile_r16_s_opts,
                                                           seq_table_sl_qos_profile_r16_s_names,
                                                           4,
                                                           4},
                                                          true,
                                                          false,
                                                          nullptr,
                                                          0};

// SL-MappedQoS-FlowsListDedicated-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_mapped_qos_flows_list_ded_r16_s_fields[] = {
    {offsetof(sl_mapped_qos_flows_list_ded_r16_s, sl_mapped_qos_flows_to_add_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2047,
     &asn1::table::list_codec<decltype(sl_mapped_qos_flows_list_ded_r16_s::sl_mapped_qos_flows_to_add_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{2047}, false, false>>::value},
    {offsetof(sl_mapped_qos_flows_list_ded_r16_s, sl_mapped_qos_flows_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2047,
     &asn1::table::list_codec<decltype(sl_mapped_qos_flows_list_ded_r16_s::sl_mapped_qos_flows_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{2047}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_mapped_qos_flows_list_ded_r16_s_names[] = {
    {"sl-MappedQoS-FlowsToAddList-r16", nullptr},
    {"sl-MappedQoS-FlowsToReleaseList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_mapped_qos_flows_list_ded_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_mapped_qos_flows_list_ded_r16_s::desc = {
    {seq_table_sl_mapped_qos_flows_list_ded_r16_s_fields,
     uint64_t{0x0},
     seq_table_sl_mapped_qos_flows_list_ded_r16_s_opts,
     seq_table_sl_mapped_qos_flows_list_ded_r16_s_names,
     2,
     2},
    false,
    false,
    nullptr,
    0};

// SL-SDAP-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sdap_cfg_r16_s_fields[] = {
    {offsetof(sl_sdap_cfg_r16_s, sl_sdap_hdr_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_sdap_cfg_r16_s::sl_sdap_hdr_r16)>(),
     decltype(sl_sdap_cfg_r16_s::sl_sdap_hdr_r16)::has_ext,
     false,
     decltype(sl_sdap_cfg_r16_s::sl_sdap_hdr_r16)::nof_types,
     decltype(sl_sdap_cfg_r16_s::sl_sdap_hdr_r16)::nof_exts,
     nullptr,
     &sl_sdap_cfg_r16_s::sl_sdap_hdr_r16_opts::desc},
    {offsetof(sl_sdap_cfg_r16_s, sl_default_rb_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_sdap_cfg_r16_s, sl_mapped_qos_flows_r16),
     (int32_t)offsetof(sl_sdap_cfg_r16_s, sl_mapped_qos_flows_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::desc},
    {offsetof(sl_sdap_cfg_r16_s, sl_cast_type_r16),
     (int32_t)offsetof(sl_sdap_cfg_r16_s, sl_cast_type_r16_present),
     asn1::table::enum_op<decltype(sl_sdap_cfg_r16_s::sl_cast_type_r16)>(),
     decltype(sl_sdap_cfg_r16_s::sl_cast_type_r16)::has_ext,
     false,
     decltype(sl_sdap_cfg_r16_s::sl_cast_type_r16)::nof_types,
     decltype(sl_sdap_cfg_r16_s::sl_cast_type_r16)::nof_exts,
     nullptr,
     &sl_sdap_cfg_r16_s::sl_cast_type_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_sdap_cfg_r16_s_names[] = {{"sl-SDAP-Header-r16", nullptr},
                                                                            {"sl-DefaultRB-r16", nullptr},
                                                                            {"sl-MappedQoS-Flows-r16", nullptr},
                                                                            {"sl-CastType-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_sdap_cfg_r16_s_opts[]  = {
    {2, (int32_t)offsetof(sl_sdap_cfg_r16_s, sl_mapped_qos_flows_r16_present)},
    {3, (int32_t)offsetof(sl_sdap_cfg_r16_s, sl_cast_type_r16_present)}};
const asn1::table::seq_desc sl_sdap_cfg_r16_s::desc = {{seq_table_sl_sdap_cfg_r16_s_fields,
                                                        uint64_t{0x3},
                                                        seq_table_sl_sdap_cfg_r16_s_opts,
                                                        seq_table_sl_sdap_cfg_r16_s_names,
                                                        4,
                                                        2},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

static const char*           enum_names_sl_sdap_cfg_r16_s_sl_sdap_hdr_r16_e__values[] = {"present", "absent"};
const asn1::table::enum_desc sl_sdap_cfg_r16_s::sl_sdap_hdr_r16_opts::desc            = {
    enum_names_sl_sdap_cfg_r16_s_sl_sdap_hdr_r16_e__values,
    2,
    "sl_sdap_cfg_r16_s::sl_sdap_hdr_r16_e_"};

static const asn1::table::storage_ops* const
    choice_storage_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__alternatives[] = {
        &asn1::table::alternative_ops<sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::sl_mapped_qos_flows_list_r16_l_,
                                      false>::value,
        &asn1::table::alternative_ops<sl_mapped_qos_flows_list_ded_r16_s, false>::value};
const asn1::table::choice_storage_desc sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::storage_desc = {
    choice_storage_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__alternatives,
    2,
    "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_"};

sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::sl_mapped_qos_flows_list_r16_l_&
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::set_sl_mapped_qos_flows_list_r16()
{
  set(types::sl_mapped_qos_flows_list_r16);
  return c.get<sl_mapped_qos_flows_list_r16_l_>();
}
sl_mapped_qos_flows_list_ded_r16_s&
sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::set_sl_mapped_qos_flows_list_ded_r16()
{
  set(types::sl_mapped_qos_flows_list_ded_r16);
  return c.get<sl_mapped_qos_flows_list_ded_r16_s>();
}
static const asn1::table::seq_field choice_table_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2047,
     &asn1::table::list_codec<sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::sl_mapped_qos_flows_list_r16_l_,
                              asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_mapped_qos_flows_list_ded_r16_s::desc}};
static const asn1::table::field_name choice_table_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__names[] = {
    {"sl-MappedQoS-FlowsList-r16", nullptr},
    {"sl-MappedQoS-FlowsListDedicated-r16", nullptr}};
const asn1::table::choice_desc sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::desc = {
    asn1::table::enum_int_type<sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types>(),
    sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types::nof_types,
    sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types::nof_exts,
    sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types::has_ext,
    false,
    choice_table_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__alternatives,
    nullptr,
    choice_table_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__names,
    "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_",
    offsetof(sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_, c.type),
    offsetof(sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_, c.value),
    false,
    &sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::storage_desc};

static const char* enum_names_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__types_values[] = {
    "sl-MappedQoS-FlowsList-r16",
    "sl-MappedQoS-FlowsListDedicated-r16"};
const asn1::table::enum_desc sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types_opts::desc = {
    enum_names_sl_sdap_cfg_r16_s_sl_mapped_qos_flows_r16_c__types_values,
    2,
    "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types"};

static const char*           enum_names_sl_sdap_cfg_r16_s_sl_cast_type_r16_e__values[] = {"broadcast",
                                                                                          "groupcast",
                                                                                          "unicast",
                                                                                          "spare1"};
const asn1::table::enum_desc sl_sdap_cfg_r16_s::sl_cast_type_r16_opts::desc            = {
    enum_names_sl_sdap_cfg_r16_s_sl_cast_type_r16_e__values,
    4,
    "sl_sdap_cfg_r16_s::sl_cast_type_r16_e_"};

// SL-PDCP-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pdcp_cfg_r16_s_fields[] = {
    {offsetof(sl_pdcp_cfg_r16_s, sl_discard_timer_r16),
     (int32_t)offsetof(sl_pdcp_cfg_r16_s, sl_discard_timer_r16_present),
     asn1::table::enum_op<decltype(sl_pdcp_cfg_r16_s::sl_discard_timer_r16)>(),
     decltype(sl_pdcp_cfg_r16_s::sl_discard_timer_r16)::has_ext,
     false,
     decltype(sl_pdcp_cfg_r16_s::sl_discard_timer_r16)::nof_types,
     decltype(sl_pdcp_cfg_r16_s::sl_discard_timer_r16)::nof_exts,
     nullptr,
     &sl_pdcp_cfg_r16_s::sl_discard_timer_r16_opts::desc},
    {offsetof(sl_pdcp_cfg_r16_s, sl_pdcp_sn_size_r16),
     (int32_t)offsetof(sl_pdcp_cfg_r16_s, sl_pdcp_sn_size_r16_present),
     asn1::table::enum_op<decltype(sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16)>(),
     decltype(sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16)::has_ext,
     false,
     decltype(sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16)::nof_types,
     decltype(sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16)::nof_exts,
     nullptr,
     &sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_opts::desc},
    {0,
     (int32_t)offsetof(sl_pdcp_cfg_r16_s, sl_out_of_order_delivery_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_pdcp_cfg_r16_s_names[] = {{"sl-DiscardTimer-r16", nullptr},
                                                                            {"sl-PDCP-SN-Size-r16", nullptr},
                                                                            {"sl-OutOfOrderDelivery", "true"}};
static const asn1::table::seq_opt    seq_table_sl_pdcp_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_pdcp_cfg_r16_s, sl_discard_timer_r16_present)},
    {1, (int32_t)offsetof(sl_pdcp_cfg_r16_s, sl_pdcp_sn_size_r16_present)},
    {2, (int32_t)offsetof(sl_pdcp_cfg_r16_s, sl_out_of_order_delivery_present)}};
const asn1::table::seq_desc sl_pdcp_cfg_r16_s::desc = {{seq_table_sl_pdcp_cfg_r16_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_sl_pdcp_cfg_r16_s_opts,
                                                        seq_table_sl_pdcp_cfg_r16_s_names,
                                                        3,
                                                        3},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

static const char*           enum_names_sl_pdcp_cfg_r16_s_sl_discard_timer_r16_e__values[] = {"ms3",
                                                                                              "ms10",
                                                                                              "ms20",
                                                                                              "ms25",
                                                                                              "ms30",
                                                                                              "ms40",
                                                                                              "ms50",
                                                                                              "ms60",
                                                                                              "ms75",
                                                                                              "ms100",
                                                                                              "ms150",
                                                                                              "ms200",
                                                                                              "ms250",
                                                                                              "ms300",
                                                                                              "ms500",
                                                                                              "ms750",
                                                                                              "ms1500",
                                                                                              "infinity"};
const asn1::table::enum_desc sl_pdcp_cfg_r16_s::sl_discard_timer_r16_opts::desc            = {
    enum_names_sl_pdcp_cfg_r16_s_sl_discard_timer_r16_e__values,
    18,
    "sl_pdcp_cfg_r16_s::sl_discard_timer_r16_e_"};
int16_t sl_pdcp_cfg_r16_s::sl_discard_timer_r16_opts::to_number() const
{
  static const int16_t numbers[] = {3, 10, 20, 25, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(numbers, 18, value, "sl_pdcp_cfg_r16_s::sl_discard_timer_r16_e_");
}

static const char*           enum_names_sl_pdcp_cfg_r16_s_sl_pdcp_sn_size_r16_e__values[] = {"len12bits", "len18bits"};
const asn1::table::enum_desc sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_opts::desc            = {
    enum_names_sl_pdcp_cfg_r16_s_sl_pdcp_sn_size_r16_e__values,
    2,
    "sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_e_"};
uint8_t sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "sl_pdcp_cfg_r16_s::sl_pdcp_sn_size_r16_e_");
}

// SL-RadioBearerConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_radio_bearer_cfg_r16_s_fields[] = {
    {offsetof(sl_radio_bearer_cfg_r16_s, slrb_uu_cfg_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr},
    {offsetof(sl_radio_bearer_cfg_r16_s, sl_sdap_cfg_r16),
     (int32_t)offsetof(sl_radio_bearer_cfg_r16_s, sl_sdap_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_sdap_cfg_r16_s::desc},
    {offsetof(sl_radio_bearer_cfg_r16_s, sl_pdcp_cfg_r16),
     (int32_t)offsetof(sl_radio_bearer_cfg_r16_s, sl_pdcp_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pdcp_cfg_r16_s::desc},
    {offsetof(sl_radio_bearer_cfg_r16_s, sl_trans_range_r16),
     (int32_t)offsetof(sl_radio_bearer_cfg_r16_s, sl_trans_range_r16_present),
     asn1::table::enum_op<decltype(sl_radio_bearer_cfg_r16_s::sl_trans_range_r16)>(),
     decltype(sl_radio_bearer_cfg_r16_s::sl_trans_range_r16)::has_ext,
     false,
     decltype(sl_radio_bearer_cfg_r16_s::sl_trans_range_r16)::nof_types,
     decltype(sl_radio_bearer_cfg_r16_s::sl_trans_range_r16)::nof_exts,
     nullptr,
     &sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_radio_bearer_cfg_r16_s_names[] = {
    {"slrb-Uu-ConfigIndex-r16", nullptr},
    {"sl-SDAP-Config-r16", nullptr},
    {"sl-PDCP-Config-r16", nullptr},
    {"sl-TransRange-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_radio_bearer_cfg_r16_s_opts[] = {
    {1, (int32_t)offsetof(sl_radio_bearer_cfg_r16_s, sl_sdap_cfg_r16_present)},
    {2, (int32_t)offsetof(sl_radio_bearer_cfg_r16_s, sl_pdcp_cfg_r16_present)},
    {3, (int32_t)offsetof(sl_radio_bearer_cfg_r16_s, sl_trans_range_r16_present)}};
const asn1::table::seq_desc sl_radio_bearer_cfg_r16_s::desc = {{seq_table_sl_radio_bearer_cfg_r16_s_fields,
                                                                uint64_t{0x1},
                                                                seq_table_sl_radio_bearer_cfg_r16_s_opts,
                                                                seq_table_sl_radio_bearer_cfg_r16_s_names,
                                                                4,
                                                                3},
                                                               true,
                                                               false,
                                                               nullptr,
                                                               0};

static const char* enum_names_sl_radio_bearer_cfg_r16_s_sl_trans_range_r16_e__values[] = {
    "m20",   "m50",    "m80",    "m100",   "m120",   "m150",   "m180",   "m200",   "m220",   "m250",  "m270",
    "m300",  "m350",   "m370",   "m400",   "m420",   "m450",   "m480",   "m500",   "m550",   "m600",  "m700",
    "m1000", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_opts::desc = {
    enum_names_sl_radio_bearer_cfg_r16_s_sl_trans_range_r16_e__values,
    32,
    "sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_e_"};
uint16_t sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {20,  50,  80,  100, 120, 150, 180, 200, 220, 250, 270, 300,
                                     350, 370, 400, 420, 450, 480, 500, 550, 600, 700, 1000};
  return map_enum_number(numbers, 23, value, "sl_radio_bearer_cfg_r16_s::sl_trans_range_r16_e_");
}

// SL-MeasObject-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_obj_r16_s_fields[] = {
    {offsetof(sl_meas_obj_r16_s, freq_info_sl_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{3279165},
     nullptr}};
static const asn1::table::field_name seq_table_sl_meas_obj_r16_s_names[] = {{"frequencyInfoSL-r16", nullptr}};
const asn1::table::seq_desc          sl_meas_obj_r16_s::desc             = {
    {seq_table_sl_meas_obj_r16_s_fields, uint64_t{0x1}, nullptr, seq_table_sl_meas_obj_r16_s_names, 1, 0},
    true,
    false,
    nullptr,
    0};

// SL-MeasObjectInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_obj_info_r16_s_fields[] = {
    {offsetof(sl_meas_obj_info_r16_s, sl_meas_obj_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr},
    {offsetof(sl_meas_obj_info_r16_s, sl_meas_obj_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_obj_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_obj_info_r16_s_names[] = {{"sl-MeasObjectId-r16", nullptr},
                                                                                 {"sl-MeasObject-r16", nullptr}};
const asn1::table::seq_desc          sl_meas_obj_info_r16_s::desc             = {
    {seq_table_sl_meas_obj_info_r16_s_fields, uint64_t{0x3}, nullptr, seq_table_sl_meas_obj_info_r16_s_names, 2, 0},
    true,
    false,
    nullptr,
    0};

// SL-RS-Type-r16 ::= ENUMERATED
static const char*           enum_names_sl_rs_type_r16_e_values[] = {"dmrs", "sl-prs", "spare2", "spare1"};
const asn1::table::enum_desc sl_rs_type_r16_opts::desc = {enum_names_sl_rs_type_r16_e_values, 4, "sl_rs_type_r16_e"};

// SL-PeriodicalReportConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_periodical_report_cfg_r16_s_fields[] = {
    {offsetof(sl_periodical_report_cfg_r16_s, sl_report_interv_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_periodical_report_cfg_r16_s::sl_report_interv_r16)>(),
     decltype(sl_periodical_report_cfg_r16_s::sl_report_interv_r16)::has_ext,
     false,
     decltype(sl_periodical_report_cfg_r16_s::sl_report_interv_r16)::nof_types,
     decltype(sl_periodical_report_cfg_r16_s::sl_report_interv_r16)::nof_exts,
     nullptr,
     &report_interv_opts::desc},
    {offsetof(sl_periodical_report_cfg_r16_s, sl_report_amount_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_periodical_report_cfg_r16_s::sl_report_amount_r16)>(),
     decltype(sl_periodical_report_cfg_r16_s::sl_report_amount_r16)::has_ext,
     false,
     decltype(sl_periodical_report_cfg_r16_s::sl_report_amount_r16)::nof_types,
     decltype(sl_periodical_report_cfg_r16_s::sl_report_amount_r16)::nof_exts,
     nullptr,
     &sl_periodical_report_cfg_r16_s::sl_report_amount_r16_opts::desc},
    {offsetof(sl_periodical_report_cfg_r16_s, sl_report_quant_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_report_quant_r16_c::desc},
    {offsetof(sl_periodical_report_cfg_r16_s, sl_rs_type_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_periodical_report_cfg_r16_s::sl_rs_type_r16)>(),
     decltype(sl_periodical_report_cfg_r16_s::sl_rs_type_r16)::has_ext,
     false,
     decltype(sl_periodical_report_cfg_r16_s::sl_rs_type_r16)::nof_types,
     decltype(sl_periodical_report_cfg_r16_s::sl_rs_type_r16)::nof_exts,
     nullptr,
     &sl_rs_type_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_periodical_report_cfg_r16_s_names[] = {
    {"sl-ReportInterval-r16", nullptr},
    {"sl-ReportAmount-r16", nullptr},
    {"sl-ReportQuantity-r16", nullptr},
    {"sl-RS-Type-r16", nullptr}};
const asn1::table::seq_desc sl_periodical_report_cfg_r16_s::desc = {{seq_table_sl_periodical_report_cfg_r16_s_fields,
                                                                     uint64_t{0xf},
                                                                     nullptr,
                                                                     seq_table_sl_periodical_report_cfg_r16_s_names,
                                                                     4,
                                                                     0},
                                                                    true,
                                                                    false,
                                                                    nullptr,
                                                                    0};

static const char* enum_names_sl_periodical_report_cfg_r16_s_sl_report_amount_r16_e__values[] =
    {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
const asn1::table::enum_desc sl_periodical_report_cfg_r16_s::sl_report_amount_r16_opts::desc = {
    enum_names_sl_periodical_report_cfg_r16_s_sl_report_amount_r16_e__values,
    8,
    "sl_periodical_report_cfg_r16_s::sl_report_amount_r16_e_"};
int8_t sl_periodical_report_cfg_r16_s::sl_report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "sl_periodical_report_cfg_r16_s::sl_report_amount_r16_e_");
}

// SL-EventTriggerConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_event_trigger_cfg_r16_s_fields[] = {
    {offsetof(sl_event_trigger_cfg_r16_s, sl_event_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::desc},
    {offsetof(sl_event_trigger_cfg_r16_s, sl_report_interv_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_event_trigger_cfg_r16_s::sl_report_interv_r16)>(),
     decltype(sl_event_trigger_cfg_r16_s::sl_report_interv_r16)::has_ext,
     false,
     decltype(sl_event_trigger_cfg_r16_s::sl_report_interv_r16)::nof_types,
     decltype(sl_event_trigger_cfg_r16_s::sl_report_interv_r16)::nof_exts,
     nullptr,
     &report_interv_opts::desc},
    {offsetof(sl_event_trigger_cfg_r16_s, sl_report_amount_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_event_trigger_cfg_r16_s::sl_report_amount_r16)>(),
     decltype(sl_event_trigger_cfg_r16_s::sl_report_amount_r16)::has_ext,
     false,
     decltype(sl_event_trigger_cfg_r16_s::sl_report_amount_r16)::nof_types,
     decltype(sl_event_trigger_cfg_r16_s::sl_report_amount_r16)::nof_exts,
     nullptr,
     &sl_event_trigger_cfg_r16_s::sl_report_amount_r16_opts::desc},
    {offsetof(sl_event_trigger_cfg_r16_s, sl_report_quant_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_report_quant_r16_c::desc},
    {offsetof(sl_event_trigger_cfg_r16_s, sl_rs_type_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_event_trigger_cfg_r16_s::sl_rs_type_r16)>(),
     decltype(sl_event_trigger_cfg_r16_s::sl_rs_type_r16)::has_ext,
     false,
     decltype(sl_event_trigger_cfg_r16_s::sl_rs_type_r16)::nof_types,
     decltype(sl_event_trigger_cfg_r16_s::sl_rs_type_r16)::nof_exts,
     nullptr,
     &sl_rs_type_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_event_trigger_cfg_r16_s_names[] = {{"sl-EventId-r16", nullptr},
                                                                                     {"sl-ReportInterval-r16", nullptr},
                                                                                     {"sl-ReportAmount-r16", nullptr},
                                                                                     {"sl-ReportQuantity-r16", nullptr},
                                                                                     {"sl-RS-Type-r16", nullptr}};
const asn1::table::seq_desc          sl_event_trigger_cfg_r16_s::desc = {{seq_table_sl_event_trigger_cfg_r16_s_fields,
                                                                          uint64_t{0x1f},
                                                                          nullptr,
                                                                          seq_table_sl_event_trigger_cfg_r16_s_names,
                                                                          5,
                                                                          0},
                                                                         true,
                                                                         false,
                                                                         nullptr,
                                                                         0};

static const asn1::table::storage_ops* const
    choice_storage_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__alternatives[] = {
        &asn1::table::alternative_ops<sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_, false>::value,
        &asn1::table::alternative_ops<sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_, false>::value};
const asn1::table::choice_storage_desc sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::storage_desc = {
    choice_storage_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__alternatives,
    2,
    "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_"};

sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_&
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::set_event_s1_r16()
{
  set(types::event_s1_r16);
  return c.get<event_s1_r16_s_>();
}
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_&
sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::set_event_s2_r16()
{
  set(types::event_s2_r16);
  return c.get<event_s2_r16_s_>();
}
static const asn1::table::seq_field seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s1_r16_s__fields[] = {
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_, s1_thres_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_trigger_quant_r16_c::desc},
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_, sl_report_on_leave_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_, sl_hysteresis_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr},
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_, sl_time_to_trigger_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_::sl_time_to_trigger_r16)>(),
     decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_::sl_time_to_trigger_r16)::has_ext,
     false,
     decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_::sl_time_to_trigger_r16)::nof_types,
     decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s1_r16_s_::sl_time_to_trigger_r16)::nof_exts,
     nullptr,
     &time_to_trigger_opts::desc}};
static const asn1::table::field_name seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s1_r16_s__names[] = {
    {"s1-Threshold-r16", nullptr},
    {"sl-ReportOnLeave-r16", nullptr},
    {"sl-Hysteresis-r16", nullptr},
    {"sl-TimeToTrigger-r16", nullptr}};
static const asn1::table::seq_desc seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s1_r16_s_ = {
    {seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s1_r16_s__fields,
     uint64_t{0xf},
     nullptr,
     seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s1_r16_s__names,
     4,
     0},
    true,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s2_r16_s__fields[] = {
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_, s2_thres_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_trigger_quant_r16_c::desc},
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_, sl_report_on_leave_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_, sl_hysteresis_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr},
    {offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_, sl_time_to_trigger_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_::sl_time_to_trigger_r16)>(),
     decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_::sl_time_to_trigger_r16)::has_ext,
     false,
     decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_::sl_time_to_trigger_r16)::nof_types,
     decltype(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::event_s2_r16_s_::sl_time_to_trigger_r16)::nof_exts,
     nullptr,
     &time_to_trigger_opts::desc}};
static const asn1::table::field_name seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s2_r16_s__names[] = {
    {"s2-Threshold-r16", nullptr},
    {"sl-ReportOnLeave-r16", nullptr},
    {"sl-Hysteresis-r16", nullptr},
    {"sl-TimeToTrigger-r16", nullptr}};
static const asn1::table::seq_desc seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s2_r16_s_ = {
    {seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s2_r16_s__fields,
     uint64_t{0xf},
     nullptr,
     seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s2_r16_s__names,
     4,
     0},
    true,
    false,
    nullptr,
    0};
static const asn1::table::seq_field choice_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s1_r16_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__event_s2_r16_s_}};
static const asn1::table::field_name choice_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__names[] = {
    {"eventS1-r16", nullptr},
    {"eventS2-r16", nullptr}};
const asn1::table::choice_desc sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::desc = {
    asn1::table::enum_int_type<sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types>(),
    sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types::nof_types,
    sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types::nof_exts,
    sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types::has_ext,
    false,
    choice_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__alternatives,
    nullptr,
    choice_table_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__names,
    "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_",
    offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_, c.type),
    offsetof(sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_, c.value),
    false,
    &sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::storage_desc};

static const char*           enum_names_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__types_values[] = {"eventS1-r16",
                                                                                                        "eventS2-r16"};
const asn1::table::enum_desc sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types_opts::desc        = {
    enum_names_sl_event_trigger_cfg_r16_s_sl_event_id_r16_c__types_values,
    2,
    "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types"};
uint8_t sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types");
}

static const char* enum_names_sl_event_trigger_cfg_r16_s_sl_report_amount_r16_e__values[] =
    {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
const asn1::table::enum_desc sl_event_trigger_cfg_r16_s::sl_report_amount_r16_opts::desc = {
    enum_names_sl_event_trigger_cfg_r16_s_sl_report_amount_r16_e__values,
    8,
    "sl_event_trigger_cfg_r16_s::sl_report_amount_r16_e_"};
int8_t sl_event_trigger_cfg_r16_s::sl_report_amount_r16_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "sl_event_trigger_cfg_r16_s::sl_report_amount_r16_e_");
}

// SL-ReportConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_report_cfg_r16_s_fields[] = {
    {offsetof(sl_report_cfg_r16_s, sl_report_type_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_report_cfg_r16_s::sl_report_type_r16_c_::desc}};
static const asn1::table::field_name seq_table_sl_report_cfg_r16_s_names[] = {{"sl-ReportType-r16", nullptr}};
const asn1::table::seq_desc          sl_report_cfg_r16_s::desc             = {
    {seq_table_sl_report_cfg_r16_s_fields, uint64_t{0x1}, nullptr, seq_table_sl_report_cfg_r16_s_names, 1, 0},
    true,
    false,
    nullptr,
    0};

static const asn1::table::storage_ops* const choice_storage_sl_report_cfg_r16_s_sl_report_type_r16_c__alternatives[] = {
    &asn1::table::alternative_ops<sl_periodical_report_cfg_r16_s, false>::value,
    &asn1::table::alternative_ops<sl_event_trigger_cfg_r16_s, false>::value};
const asn1::table::choice_storage_desc sl_report_cfg_r16_s::sl_report_type_r16_c_::storage_desc = {
    choice_storage_sl_report_cfg_r16_s_sl_report_type_r16_c__alternatives,
    2,
    "sl_report_cfg_r16_s::sl_report_type_r16_c_"};

sl_periodical_report_cfg_r16_s& sl_report_cfg_r16_s::sl_report_type_r16_c_::set_sl_periodical_r16()
{
  set(types::sl_periodical_r16);
  return c.get<sl_periodical_report_cfg_r16_s>();
}
sl_event_trigger_cfg_r16_s& sl_report_cfg_r16_s::sl_report_type_r16_c_::set_sl_event_triggered_r16()
{
  set(types::sl_event_triggered_r16);
  return c.get<sl_event_trigger_cfg_r16_s>();
}
static const asn1::table::seq_field choice_table_sl_report_cfg_r16_s_sl_report_type_r16_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_periodical_report_cfg_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_event_trigger_cfg_r16_s::desc}};
static const asn1::table::field_name choice_table_sl_report_cfg_r16_s_sl_report_type_r16_c__names[] = {
    {"sl-Periodical-r16", nullptr},
    {"sl-EventTriggered-r16", nullptr}};
const asn1::table::choice_desc sl_report_cfg_r16_s::sl_report_type_r16_c_::desc = {
    asn1::table::enum_int_type<sl_report_cfg_r16_s::sl_report_type_r16_c_::types>(),
    sl_report_cfg_r16_s::sl_report_type_r16_c_::types::nof_types,
    sl_report_cfg_r16_s::sl_report_type_r16_c_::types::nof_exts,
    sl_report_cfg_r16_s::sl_report_type_r16_c_::types::has_ext,
    false,
    choice_table_sl_report_cfg_r16_s_sl_report_type_r16_c__alternatives,
    nullptr,
    choice_table_sl_report_cfg_r16_s_sl_report_type_r16_c__names,
    "sl_report_cfg_r16_s::sl_report_type_r16_c_",
    offsetof(sl_report_cfg_r16_s::sl_report_type_r16_c_, c.type),
    offsetof(sl_report_cfg_r16_s::sl_report_type_r16_c_, c.value),
    false,
    &sl_report_cfg_r16_s::sl_report_type_r16_c_::storage_desc};

static const char*           enum_names_sl_report_cfg_r16_s_sl_report_type_r16_c__types_values[] = {"sl-Periodical-r16",
                                                                                                    "sl-EventTriggered-r16"};
const asn1::table::enum_desc sl_report_cfg_r16_s::sl_report_type_r16_c_::types_opts::desc        = {
    enum_names_sl_report_cfg_r16_s_sl_report_type_r16_c__types_values,
    2,
    "sl_report_cfg_r16_s::sl_report_type_r16_c_::types"};

// SL-ReportConfigInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_report_cfg_info_r16_s_fields[] = {
    {offsetof(sl_report_cfg_info_r16_s, sl_report_cfg_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr},
    {offsetof(sl_report_cfg_info_r16_s, sl_report_cfg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_report_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_report_cfg_info_r16_s_names[] = {{"sl-ReportConfigId-r16", nullptr},
                                                                                   {"sl-ReportConfig-r16", nullptr}};
const asn1::table::seq_desc          sl_report_cfg_info_r16_s::desc             = {
    {seq_table_sl_report_cfg_info_r16_s_fields, uint64_t{0x3}, nullptr, seq_table_sl_report_cfg_info_r16_s_names, 2, 0},
    true,
    false,
    nullptr,
    0};

// SL-MeasIdInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_id_info_r16_s_fields[] = {
    {offsetof(sl_meas_id_info_r16_s, sl_meas_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr},
    {offsetof(sl_meas_id_info_r16_s, sl_meas_obj_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr},
    {offsetof(sl_meas_id_info_r16_s, sl_report_cfg_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr}};
static const asn1::table::field_name seq_table_sl_meas_id_info_r16_s_names[] = {{"sl-MeasId-r16", nullptr},
                                                                                {"sl-MeasObjectId-r16", nullptr},
                                                                                {"sl-ReportConfigId-r16", nullptr}};
const asn1::table::seq_desc          sl_meas_id_info_r16_s::desc             = {
    {seq_table_sl_meas_id_info_r16_s_fields, uint64_t{0x7}, nullptr, seq_table_sl_meas_id_info_r16_s_names, 3, 0},
    true,
    false,
    nullptr,
    0};

// SL-QuantityConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_quant_cfg_r16_s_fields[] = {
    {offsetof(sl_quant_cfg_r16_s, sl_filt_coef_dmrs_r16),
     (int32_t)offsetof(sl_quant_cfg_r16_s, sl_filt_coef_dmrs_r16_present),
     asn1::table::enum_op<decltype(sl_quant_cfg_r16_s::sl_filt_coef_dmrs_r16)>(),
     decltype(sl_quant_cfg_r16_s::sl_filt_coef_dmrs_r16)::has_ext,
     false,
     decltype(sl_quant_cfg_r16_s::sl_filt_coef_dmrs_r16)::nof_types,
     decltype(sl_quant_cfg_r16_s::sl_filt_coef_dmrs_r16)::nof_exts,
     nullptr,
     &filt_coef_opts::desc}};
static const asn1::table::field_name seq_table_sl_quant_cfg_r16_s_names[] = {{"sl-FilterCoefficientDMRS-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_quant_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_quant_cfg_r16_s, sl_filt_coef_dmrs_r16_present)}};
const asn1::table::seq_desc sl_quant_cfg_r16_s::desc = {{seq_table_sl_quant_cfg_r16_s_fields,
                                                         uint64_t{0x0},
                                                         seq_table_sl_quant_cfg_r16_s_opts,
                                                         seq_table_sl_quant_cfg_r16_s_names,
                                                         1,
                                                         1},
                                                        true,
                                                        false,
                                                        nullptr,
                                                        0};

// SL-MeasConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_cfg_r16_s_fields[] = {
    {offsetof(sl_meas_cfg_r16_s, sl_meas_obj_to_rem_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::list_codec<decltype(sl_meas_cfg_r16_s::sl_meas_obj_to_rem_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{63}, false, false>>::value},
    {offsetof(sl_meas_cfg_r16_s, sl_meas_obj_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(sl_meas_cfg_r16_s::sl_meas_obj_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_meas_obj_info_r16_s::desc},
    {offsetof(sl_meas_cfg_r16_s, sl_report_cfg_to_rem_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::list_codec<decltype(sl_meas_cfg_r16_s::sl_report_cfg_to_rem_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{63}, false, false>>::value},
    {offsetof(sl_meas_cfg_r16_s, sl_report_cfg_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(sl_meas_cfg_r16_s::sl_report_cfg_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_report_cfg_info_r16_s::desc},
    {offsetof(sl_meas_cfg_r16_s, sl_meas_id_to_rem_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::list_codec<decltype(sl_meas_cfg_r16_s::sl_meas_id_to_rem_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{63}, false, false>>::value},
    {offsetof(sl_meas_cfg_r16_s, sl_meas_id_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(sl_meas_cfg_r16_s::sl_meas_id_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_meas_id_info_r16_s::desc},
    {offsetof(sl_meas_cfg_r16_s, sl_quant_cfg_r16),
     (int32_t)offsetof(sl_meas_cfg_r16_s, sl_quant_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_quant_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_cfg_r16_s_names[] = {
    {"sl-MeasObjectToRemoveList-r16", nullptr},
    {"sl-MeasObjectToAddModList-r16", nullptr},
    {"sl-ReportConfigToRemoveList-r16", nullptr},
    {"sl-ReportConfigToAddModList-r16", nullptr},
    {"sl-MeasIdToRemoveList-r16", nullptr},
    {"sl-MeasIdToAddModList-r16", nullptr},
    {"sl-QuantityConfig-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_meas_cfg_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(sl_meas_cfg_r16_s, sl_quant_cfg_r16_present)}};
const asn1::table::seq_desc sl_meas_cfg_r16_s::desc = {{seq_table_sl_meas_cfg_r16_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_sl_meas_cfg_r16_s_opts,
                                                        seq_table_sl_meas_cfg_r16_s_names,
                                                        7,
                                                        7},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

// SL-MeasConfigInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_cfg_info_r16_s_fields[] = {
    {offsetof(sl_meas_cfg_info_r16_s, sl_dest_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {offsetof(sl_meas_cfg_info_r16_s, sl_meas_cfg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_cfg_info_r16_s_names[] = {{"sl-DestinationIndex-r16", nullptr},
                                                                                 {"sl-MeasConfig-r16", nullptr}};
const asn1::table::seq_desc          sl_meas_cfg_info_r16_s::desc             = {
    {seq_table_sl_meas_cfg_info_r16_s_fields, uint64_t{0x3}, nullptr, seq_table_sl_meas_cfg_info_r16_s_names, 2, 0},
    true,
    false,
    nullptr,
    0};

// SL-DRX-GC-BC-QoS-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_drx_gc_bc_qos_r17_s_fields[] = {
    {offsetof(sl_drx_gc_bc_qos_r17_s, sl_drx_gc_bc_mapped_qos_flow_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2047,
     &asn1::table::item_list_codec<decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_mapped_qos_flow_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_qos_profile_r16_s::desc},
    {offsetof(sl_drx_gc_bc_qos_r17_s, sl_drx_gc_bc_on_dur_timer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::desc},
    {offsetof(sl_drx_gc_bc_qos_r17_s, sl_drx_gc_inactivity_timer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17)>(),
     decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17)::has_ext,
     false,
     decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17)::nof_types,
     decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17)::nof_exts,
     nullptr,
     &sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_opts::desc},
    {offsetof(sl_drx_gc_bc_qos_r17_s, sl_drx_gc_bc_cycle_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17)>(),
     decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17)::has_ext,
     false,
     decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17)::nof_types,
     decltype(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17)::nof_exts,
     nullptr,
     &sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_opts::desc}};
static const asn1::table::field_name seq_table_sl_drx_gc_bc_qos_r17_s_names[] = {
    {"sl-DRX-GC-BC-MappedQoS-FlowList-r17", nullptr},
    {"sl-DRX-GC-BC-OnDurationTimer-r17", nullptr},
    {"sl-DRX-GC-InactivityTimer-r17", nullptr},
    {"sl-DRX-GC-BC-Cycle-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_drx_gc_bc_qos_r17_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_drx_gc_bc_qos_r17_s::desc = {{seq_table_sl_drx_gc_bc_qos_r17_s_fields,
                                                             uint64_t{0xe},
                                                             seq_table_sl_drx_gc_bc_qos_r17_s_opts,
                                                             seq_table_sl_drx_gc_bc_qos_r17_s_names,
                                                             4,
                                                             1},
                                                            true,
                                                            false,
                                                            nullptr,
                                                            0};

static const asn1::table::storage_ops* const
    choice_storage_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_,
                                      true>::value};
const asn1::table::choice_storage_desc sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::storage_desc = {
    choice_storage_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__alternatives,
    2,
    "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_"};

uint8_t& sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_&
sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
static const asn1::table::seq_field
    choice_table_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{1},
         uint64_t{30},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_>(),
         sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_::has_ext,
         false,
         sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_::nof_types,
         sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_::nof_exts,
         nullptr,
         &sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_opts::desc}};
static const asn1::table::field_name choice_table_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__names[] = {
    {"subMilliSeconds", nullptr},
    {"milliSeconds", nullptr}};
const asn1::table::choice_desc sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::desc = {
    asn1::table::enum_int_type<sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types>(),
    sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types::nof_types,
    sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types::nof_exts,
    sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types::has_ext,
    false,
    choice_table_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__alternatives,
    nullptr,
    choice_table_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__names,
    "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_",
    offsetof(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_, c.type),
    offsetof(sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_, c.value),
    false,
    &sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::storage_desc};

static const char* enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__milli_seconds_e__values[] = {
    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",  "ms40",
    "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms400",  "ms500",  "ms600",  "ms800", "ms1000",
    "ms1200", "ms1600", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_opts::desc = {
    enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__milli_seconds_e__values,
    32,
    "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_"};
uint16_t sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(
      numbers, 24, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::milli_seconds_e_");
}

static const char* enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__types_values[] = {
    "subMilliSeconds",
    "milliSeconds"};
const asn1::table::enum_desc sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types_opts::desc = {
    enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_on_dur_timer_r17_c__types_values,
    2,
    "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types"};

static const char* enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_inactivity_timer_r17_e__values[] = {
    "ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",
    "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920",
    "ms2560", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_opts::desc = {
    enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_inactivity_timer_r17_e__values,
    32,
    "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_e_"};
uint16_t sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_inactivity_timer_r17_e_");
}

static const char* enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_cycle_r17_e__values[] = {
    "ms10",    "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",   "ms80",   "ms128",   "ms160",   "ms256",
    "ms320",   "ms512",  "ms640",  "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240", "spare12", "spare11",
    "spare10", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2",  "spare1"};
const asn1::table::enum_desc sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_opts::desc = {
    enum_names_sl_drx_gc_bc_qos_r17_s_sl_drx_gc_bc_cycle_r17_e__values,
    32,
    "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_e_"};
uint16_t sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_cycle_r17_e_");
}

// SL-DRX-GC-Generic-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_drx_gc_generic_r17_s_fields[] = {
    {offsetof(sl_drx_gc_generic_r17_s, sl_drx_gc_harq_rtt_timer1_r17),
     (int32_t)offsetof(sl_drx_gc_generic_r17_s, sl_drx_gc_harq_rtt_timer1_r17_present),
     asn1::table::enum_op<decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17)>(),
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17)::has_ext,
     false,
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17)::nof_types,
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17)::nof_exts,
     nullptr,
     &sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_opts::desc},
    {offsetof(sl_drx_gc_generic_r17_s, sl_drx_gc_harq_rtt_timer2_r17),
     (int32_t)offsetof(sl_drx_gc_generic_r17_s, sl_drx_gc_harq_rtt_timer2_r17_present),
     asn1::table::enum_op<decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17)>(),
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17)::has_ext,
     false,
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17)::nof_types,
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17)::nof_exts,
     nullptr,
     &sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_opts::desc},
    {offsetof(sl_drx_gc_generic_r17_s, sl_drx_gc_retx_timer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17)>(),
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17)::has_ext,
     false,
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17)::nof_types,
     decltype(sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17)::nof_exts,
     nullptr,
     &sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_opts::desc}};
static const asn1::table::field_name seq_table_sl_drx_gc_generic_r17_s_names[] = {
    {"sl-DRX-GC-HARQ-RTT-Timer1-r17", nullptr},
    {"sl-DRX-GC-HARQ-RTT-Timer2-r17", nullptr},
    {"sl-DRX-GC-RetransmissionTimer-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_drx_gc_generic_r17_s_opts[] = {
    {0, (int32_t)offsetof(sl_drx_gc_generic_r17_s, sl_drx_gc_harq_rtt_timer1_r17_present)},
    {1, (int32_t)offsetof(sl_drx_gc_generic_r17_s, sl_drx_gc_harq_rtt_timer2_r17_present)}};
const asn1::table::seq_desc sl_drx_gc_generic_r17_s::desc = {{seq_table_sl_drx_gc_generic_r17_s_fields,
                                                              uint64_t{0x4},
                                                              seq_table_sl_drx_gc_generic_r17_s_opts,
                                                              seq_table_sl_drx_gc_generic_r17_s_names,
                                                              3,
                                                              2},
                                                             false,
                                                             false,
                                                             nullptr,
                                                             0};

static const char* enum_names_sl_drx_gc_generic_r17_s_sl_drx_gc_harq_rtt_timer1_r17_e__values[] =
    {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_opts::desc = {
    enum_names_sl_drx_gc_generic_r17_s_sl_drx_gc_harq_rtt_timer1_r17_e__values,
    8,
    "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_e_"};
uint8_t sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer1_r17_e_");
}

static const char* enum_names_sl_drx_gc_generic_r17_s_sl_drx_gc_harq_rtt_timer2_r17_e__values[] =
    {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_opts::desc = {
    enum_names_sl_drx_gc_generic_r17_s_sl_drx_gc_harq_rtt_timer2_r17_e__values,
    8,
    "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_e_"};
uint8_t sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_harq_rtt_timer2_r17_e_");
}

static const char* enum_names_sl_drx_gc_generic_r17_s_sl_drx_gc_retx_timer_r17_e__values[] = {
    "sl0",     "sl1",    "sl2",    "sl4",    "sl6",    "sl8",    "sl16",    "sl24",    "sl33",    "sl40",    "sl64",
    "sl80",    "sl96",   "sl112",  "sl128",  "sl160",  "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11",
    "spare10", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4",  "spare3",  "spare2",  "spare1"};
const asn1::table::enum_desc sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_opts::desc = {
    enum_names_sl_drx_gc_generic_r17_s_sl_drx_gc_retx_timer_r17_e__values,
    32,
    "sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_e_"};
uint16_t sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "sl_drx_gc_generic_r17_s::sl_drx_gc_retx_timer_r17_e_");
}

// SL-DRX-ConfigGC-BC-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_drx_cfg_gc_bc_r17_s_fields[] = {
    {offsetof(sl_drx_cfg_gc_bc_r17_s, sl_drx_gc_bc_per_qos_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(sl_drx_cfg_gc_bc_r17_s::sl_drx_gc_bc_per_qos_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_drx_gc_bc_qos_r17_s::desc},
    {offsetof(sl_drx_cfg_gc_bc_r17_s, sl_drx_gc_generic_r17),
     (int32_t)offsetof(sl_drx_cfg_gc_bc_r17_s, sl_drx_gc_generic_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_gc_generic_r17_s::desc},
    {offsetof(sl_drx_cfg_gc_bc_r17_s, sl_default_drx_gc_bc_r17),
     (int32_t)offsetof(sl_drx_cfg_gc_bc_r17_s, sl_default_drx_gc_bc_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_gc_bc_qos_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_drx_cfg_gc_bc_r17_s_names[] = {
    {"sl-DRX-GC-BC-PerQoS-List-r17", nullptr},
    {"sl-DRX-GC-generic-r17", nullptr},
    {"sl-DefaultDRX-GC-BC-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_drx_cfg_gc_bc_r17_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_drx_cfg_gc_bc_r17_s, sl_drx_gc_generic_r17_present)},
    {2, (int32_t)offsetof(sl_drx_cfg_gc_bc_r17_s, sl_default_drx_gc_bc_r17_present)}};
const asn1::table::seq_desc sl_drx_cfg_gc_bc_r17_s::desc = {{seq_table_sl_drx_cfg_gc_bc_r17_s_fields,
                                                             uint64_t{0x0},
                                                             seq_table_sl_drx_cfg_gc_bc_r17_s_opts,
                                                             seq_table_sl_drx_cfg_gc_bc_r17_s_names,
                                                             3,
                                                             3},
                                                            true,
                                                            false,
                                                            nullptr,
                                                            0};

// SL-DRX-ConfigUC-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_drx_cfg_uc_r17_s_fields[] = {
    {offsetof(sl_drx_cfg_uc_r17_s, sl_drx_on_dur_timer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::desc},
    {offsetof(sl_drx_cfg_uc_r17_s, sl_drx_inactivity_timer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17)>(),
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17)::has_ext,
     false,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17)::nof_types,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17)::nof_exts,
     nullptr,
     &sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_opts::desc},
    {offsetof(sl_drx_cfg_uc_r17_s, sl_drx_harq_rtt_timer1_r17),
     (int32_t)offsetof(sl_drx_cfg_uc_r17_s, sl_drx_harq_rtt_timer1_r17_present),
     asn1::table::enum_op<decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17)>(),
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17)::has_ext,
     false,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17)::nof_types,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17)::nof_exts,
     nullptr,
     &sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_opts::desc},
    {offsetof(sl_drx_cfg_uc_r17_s, sl_drx_harq_rtt_timer2_r17),
     (int32_t)offsetof(sl_drx_cfg_uc_r17_s, sl_drx_harq_rtt_timer2_r17_present),
     asn1::table::enum_op<decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17)>(),
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17)::has_ext,
     false,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17)::nof_types,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17)::nof_exts,
     nullptr,
     &sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_opts::desc},
    {offsetof(sl_drx_cfg_uc_r17_s, sl_drx_retx_timer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17)>(),
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17)::has_ext,
     false,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17)::nof_types,
     decltype(sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17)::nof_exts,
     nullptr,
     &sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_opts::desc},
    {offsetof(sl_drx_cfg_uc_r17_s, sl_drx_cycle_start_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::desc},
    {offsetof(sl_drx_cfg_uc_r17_s, sl_drx_slot_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_sl_drx_cfg_uc_r17_s_names[] = {
    {"sl-drx-onDurationTimer-r17", nullptr},
    {"sl-drx-InactivityTimer-r17", nullptr},
    {"sl-drx-HARQ-RTT-Timer1-r17", nullptr},
    {"sl-drx-HARQ-RTT-Timer2-r17", nullptr},
    {"sl-drx-RetransmissionTimer-r17", nullptr},
    {"sl-drx-CycleStartOffset-r17", nullptr},
    {"sl-drx-SlotOffset", nullptr}};
static const asn1::table::seq_opt seq_table_sl_drx_cfg_uc_r17_s_opts[] = {
    {2, (int32_t)offsetof(sl_drx_cfg_uc_r17_s, sl_drx_harq_rtt_timer1_r17_present)},
    {3, (int32_t)offsetof(sl_drx_cfg_uc_r17_s, sl_drx_harq_rtt_timer2_r17_present)}};
const asn1::table::seq_desc sl_drx_cfg_uc_r17_s::desc = {{seq_table_sl_drx_cfg_uc_r17_s_fields,
                                                          uint64_t{0x73},
                                                          seq_table_sl_drx_cfg_uc_r17_s_opts,
                                                          seq_table_sl_drx_cfg_uc_r17_s_names,
                                                          7,
                                                          2},
                                                         false,
                                                         false,
                                                         nullptr,
                                                         0};

static const asn1::table::storage_ops* const
    choice_storage_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_, true>::value};
const asn1::table::choice_storage_desc sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::storage_desc = {
    choice_storage_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__alternatives,
    2,
    "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_"};

uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_&
sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
static const asn1::table::seq_field choice_table_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{30},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_>(),
     sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_::has_ext,
     false,
     sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_::nof_types,
     sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_::nof_exts,
     nullptr,
     &sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_opts::desc}};
static const asn1::table::field_name choice_table_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__names[] = {
    {"subMilliSeconds", nullptr},
    {"milliSeconds", nullptr}};
const asn1::table::choice_desc sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::desc = {
    asn1::table::enum_int_type<sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types>(),
    sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types::nof_types,
    sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types::nof_exts,
    sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types::has_ext,
    false,
    choice_table_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__alternatives,
    nullptr,
    choice_table_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__names,
    "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_",
    offsetof(sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_, c.type),
    offsetof(sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_, c.value),
    false,
    &sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::storage_desc};

static const char* enum_names_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__milli_seconds_e__values[] = {
    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",  "ms40",
    "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms400",  "ms500",  "ms600",  "ms800", "ms1000",
    "ms1200", "ms1600", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_opts::desc = {
    enum_names_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__milli_seconds_e__values,
    32,
    "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_"};
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 24, value, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::milli_seconds_e_");
}

static const char* enum_names_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__types_values[]    = {"subMilliSeconds",
                                                                                                  "milliSeconds"};
const asn1::table::enum_desc sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types_opts::desc = {
    enum_names_sl_drx_cfg_uc_r17_s_sl_drx_on_dur_timer_r17_c__types_values,
    2,
    "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types"};

static const char* enum_names_sl_drx_cfg_uc_r17_s_sl_drx_inactivity_timer_r17_e__values[] = {
    "ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",
    "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920",
    "ms2560", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_opts::desc = {
    enum_names_sl_drx_cfg_uc_r17_s_sl_drx_inactivity_timer_r17_e__values,
    32,
    "sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_e_"};
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "sl_drx_cfg_uc_r17_s::sl_drx_inactivity_timer_r17_e_");
}

static const char* enum_names_sl_drx_cfg_uc_r17_s_sl_drx_harq_rtt_timer1_r17_e__values[] =
    {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_opts::desc = {
    enum_names_sl_drx_cfg_uc_r17_s_sl_drx_harq_rtt_timer1_r17_e__values,
    8,
    "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_e_"};
uint8_t sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer1_r17_e_");
}

static const char* enum_names_sl_drx_cfg_uc_r17_s_sl_drx_harq_rtt_timer2_r17_e__values[] =
    {"sl0", "sl1", "sl2", "sl4", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_opts::desc = {
    enum_names_sl_drx_cfg_uc_r17_s_sl_drx_harq_rtt_timer2_r17_e__values,
    8,
    "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_e_"};
uint8_t sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "sl_drx_cfg_uc_r17_s::sl_drx_harq_rtt_timer2_r17_e_");
}

static const char* enum_names_sl_drx_cfg_uc_r17_s_sl_drx_retx_timer_r17_e__values[] = {
    "sl0",     "sl1",    "sl2",    "sl4",    "sl6",    "sl8",    "sl16",    "sl24",    "sl33",    "sl40",    "sl64",
    "sl80",    "sl96",   "sl112",  "sl128",  "sl160",  "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11",
    "spare10", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4",  "spare3",  "spare2",  "spare1"};
const asn1::table::enum_desc sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_opts::desc = {
    enum_names_sl_drx_cfg_uc_r17_s_sl_drx_retx_timer_r17_e__values,
    32,
    "sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_e_"};
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "sl_drx_cfg_uc_r17_s::sl_drx_retx_timer_r17_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::storage_desc = {
    choice_storage_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__alternatives,
    20,
    "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_"};

uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{9},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{19},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{39},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{59},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{69},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{79},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{159},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{319},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{511},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{639},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1279},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{2047},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{2559},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{5119},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{10239},
     nullptr}};
static const asn1::table::field_name choice_table_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__names[] = {
    {"ms10", nullptr},   {"ms20", nullptr},   {"ms32", nullptr},   {"ms40", nullptr},   {"ms60", nullptr},
    {"ms64", nullptr},   {"ms70", nullptr},   {"ms80", nullptr},   {"ms128", nullptr},  {"ms160", nullptr},
    {"ms256", nullptr},  {"ms320", nullptr},  {"ms512", nullptr},  {"ms640", nullptr},  {"ms1024", nullptr},
    {"ms1280", nullptr}, {"ms2048", nullptr}, {"ms2560", nullptr}, {"ms5120", nullptr}, {"ms10240", nullptr}};
const asn1::table::choice_desc sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::desc = {
    asn1::table::enum_int_type<sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types>(),
    sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types::nof_types,
    sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types::nof_exts,
    sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types::has_ext,
    false,
    choice_table_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__alternatives,
    nullptr,
    choice_table_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__names,
    "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_",
    offsetof(sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_, c.type),
    offsetof(sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_, c.value),
    false,
    &sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::storage_desc};

static const char* enum_names_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__types_values[] = {
    "ms10",  "ms20",  "ms32",  "ms40",  "ms60",   "ms64",   "ms70",   "ms80",   "ms128",  "ms160",
    "ms256", "ms320", "ms512", "ms640", "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
const asn1::table::enum_desc sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::desc = {
    enum_names_sl_drx_cfg_uc_r17_s_sl_drx_cycle_start_offset_r17_c__types_values,
    20,
    "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types"};
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types");
}

// SL-DRX-ConfigUC-Info-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_drx_cfg_uc_info_r17_s_fields[] = {
    {offsetof(sl_drx_cfg_uc_info_r17_s, sl_dest_idx_r17),
     (int32_t)offsetof(sl_drx_cfg_uc_info_r17_s, sl_dest_idx_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {offsetof(sl_drx_cfg_uc_info_r17_s, sl_drx_cfg_uc_r17),
     (int32_t)offsetof(sl_drx_cfg_uc_info_r17_s, sl_drx_cfg_uc_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_cfg_uc_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_drx_cfg_uc_info_r17_s_names[] = {{"sl-DestinationIndex-r17", nullptr},
                                                                                   {"sl-DRX-ConfigUC-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_drx_cfg_uc_info_r17_s_opts[]  = {
    {0, (int32_t)offsetof(sl_drx_cfg_uc_info_r17_s, sl_dest_idx_r17_present)},
    {1, (int32_t)offsetof(sl_drx_cfg_uc_info_r17_s, sl_drx_cfg_uc_r17_present)}};
const asn1::table::seq_desc sl_drx_cfg_uc_info_r17_s::desc = {{seq_table_sl_drx_cfg_uc_info_r17_s_fields,
                                                               uint64_t{0x0},
                                                               seq_table_sl_drx_cfg_uc_info_r17_s_opts,
                                                               seq_table_sl_drx_cfg_uc_info_r17_s_names,
                                                               2,
                                                               2},
                                                              true,
                                                              false,
                                                              nullptr,
                                                              0};

// SL-DRX-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_drx_cfg_r17_s_fields[] = {
    {offsetof(sl_drx_cfg_r17_s, sl_drx_cfg_gc_bc_r17),
     (int32_t)offsetof(sl_drx_cfg_r17_s, sl_drx_cfg_gc_bc_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_cfg_gc_bc_r17_s::desc},
    {offsetof(sl_drx_cfg_r17_s, sl_drx_cfg_uc_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(sl_drx_cfg_r17_s::sl_drx_cfg_uc_to_release_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(sl_drx_cfg_r17_s, sl_drx_cfg_uc_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(sl_drx_cfg_r17_s::sl_drx_cfg_uc_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_drx_cfg_uc_info_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_drx_cfg_r17_s_names[] = {
    {"sl-DRX-ConfigGC-BC-r17", nullptr},
    {"sl-DRX-ConfigUC-ToReleaseList-r17", nullptr},
    {"sl-DRX-ConfigUC-ToAddModList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_drx_cfg_r17_s_opts[] = {
    {0, (int32_t)offsetof(sl_drx_cfg_r17_s, sl_drx_cfg_gc_bc_r17_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_drx_cfg_r17_s::desc = {{seq_table_sl_drx_cfg_r17_s_fields,
                                                       uint64_t{0x0},
                                                       seq_table_sl_drx_cfg_r17_s_opts,
                                                       seq_table_sl_drx_cfg_r17_s_names,
                                                       3,
                                                       3},
                                                      true,
                                                      false,
                                                      nullptr,
                                                      0};

// SL-RLC-ChannelConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_rlc_ch_cfg_r17_s_fields[] = {
    {offsetof(sl_rlc_ch_cfg_r17_s, sl_rlc_ch_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr},
    {offsetof(sl_rlc_ch_cfg_r17_s, sl_rlc_cfg_r17),
     (int32_t)offsetof(sl_rlc_ch_cfg_r17_s, sl_rlc_cfg_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_rlc_cfg_r16_c::desc},
    {offsetof(sl_rlc_ch_cfg_r17_s, sl_mac_lc_ch_cfg_r17),
     (int32_t)offsetof(sl_rlc_ch_cfg_r17_s, sl_mac_lc_ch_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_lc_ch_cfg_r16_s::desc},
    {offsetof(sl_rlc_ch_cfg_r17_s, sl_packet_delay_budget_r17),
     (int32_t)offsetof(sl_rlc_ch_cfg_r17_s, sl_packet_delay_budget_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1023},
     nullptr}};
static const asn1::table::field_name seq_table_sl_rlc_ch_cfg_r17_s_names[] = {
    {"sl-RLC-ChannelID-r17", nullptr},
    {"sl-RLC-Config-r17", nullptr},
    {"sl-MAC-LogicalChannelConfig-r17", nullptr},
    {"sl-PacketDelayBudget-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_ch_cfg_r17_s_opts[] = {
    {1, (int32_t)offsetof(sl_rlc_ch_cfg_r17_s, sl_rlc_cfg_r17_present)},
    {2, (int32_t)offsetof(sl_rlc_ch_cfg_r17_s, sl_mac_lc_ch_cfg_r17_present)},
    {3, (int32_t)offsetof(sl_rlc_ch_cfg_r17_s, sl_packet_delay_budget_r17_present)}};
const asn1::table::seq_desc sl_rlc_ch_cfg_r17_s::desc = {{seq_table_sl_rlc_ch_cfg_r17_s_fields,
                                                          uint64_t{0x1},
                                                          seq_table_sl_rlc_ch_cfg_r17_s_opts,
                                                          seq_table_sl_rlc_ch_cfg_r17_s_names,
                                                          4,
                                                          3},
                                                         true,
                                                         false,
                                                         nullptr,
                                                         0};

// SL-FreqSelectionConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_freq_sel_cfg_r18_s_fields[] = {
    {offsetof(sl_freq_sel_cfg_r18_s, sl_prio_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_freq_sel_cfg_r18_s::sl_prio_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{7}, false, false>>::value},
    {offsetof(sl_freq_sel_cfg_r18_s, sl_thresh_cbr_freq_resel_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{100},
     nullptr},
    {offsetof(sl_freq_sel_cfg_r18_s, sl_thresh_cbr_freq_keeping_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{100},
     nullptr}};
static const asn1::table::field_name seq_table_sl_freq_sel_cfg_r18_s_names[] = {
    {"sl-priorityList-r18", nullptr},
    {"sl-threshCBR-FreqReselection-r18", nullptr},
    {"sl-threshCBR-FreqKeeping-r18", nullptr}};
const asn1::table::seq_desc sl_freq_sel_cfg_r18_s::desc = {
    {seq_table_sl_freq_sel_cfg_r18_s_fields, uint64_t{0x7}, nullptr, seq_table_sl_freq_sel_cfg_r18_s_names, 3, 0},
    false,
    false,
    nullptr,
    0};

// SL-FreqConfigExt-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_freq_cfg_ext_v1800_s_fields[] = {
    {0,
     (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, absence_of_any_other_technology_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_freq_cfg_ext_v1800_s, sl_freq_sel_cfg_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sl_freq_cfg_ext_v1800_s::sl_freq_sel_cfg_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_sel_cfg_r18_s::desc},
    {0,
     (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, sl_sync_tx_disabled_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_freq_cfg_ext_v1800_s, sl_energy_detection_cfg_r18),
     (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, sl_energy_detection_cfg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::desc},
    {offsetof(sl_freq_cfg_ext_v1800_s, ue_to_ue_cot_sharing_ed_thres_r18),
     (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, ue_to_ue_cot_sharing_ed_thres_r18_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-85},
     uint64_t{33},
     nullptr},
    {offsetof(sl_freq_cfg_ext_v1800_s, harq_ack_feedback_ratiofor_cw_adjustment_gc_option2_r18),
     (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, harq_ack_feedback_ratiofor_cw_adjustment_gc_option2_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{10},
     uint64_t{90},
     nullptr}};
static const asn1::table::field_name seq_table_sl_freq_cfg_ext_v1800_s_names[] = {
    {"absenceOfAnyOtherTechnology-r18", "true"},
    {"sl-FreqSelectionConfigList-r18", nullptr},
    {"sl-SyncTxDisabled-r18", "true"},
    {"sl-EnergyDetectionConfig-r18", nullptr},
    {"ue-ToUE-COT-SharingED-Threshold-r18", nullptr},
    {"harq-ACK-FeedbackRatioforCW-AdjustmentGC-Option2-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_ext_v1800_s_opts[] = {
    {0, (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, absence_of_any_other_technology_r18_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, sl_sync_tx_disabled_r18_present)},
    {3, (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, sl_energy_detection_cfg_r18_present)},
    {4, (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, ue_to_ue_cot_sharing_ed_thres_r18_present)},
    {5, (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, harq_ack_feedback_ratiofor_cw_adjustment_gc_option2_r18_present)}};
static const asn1::table::seq_field seq_table_sl_freq_cfg_ext_v1800_s_g0_fields[] = {
    {offsetof(sl_freq_cfg_ext_v1800_s, add_spec_emission_v1860),
     (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, add_spec_emission_v1860_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{8},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_sl_freq_cfg_ext_v1800_s_g0_names[] = {
    {"additionalSpectrumEmission-v1860", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_ext_v1800_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_freq_cfg_ext_v1800_s, add_spec_emission_v1860_present)}};
static const asn1::table::seq_group seq_table_sl_freq_cfg_ext_v1800_s_groups[] = {
    {{seq_table_sl_freq_cfg_ext_v1800_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_freq_cfg_ext_v1800_s_g0_opts,
      seq_table_sl_freq_cfg_ext_v1800_s_g0_names,
      1,
      1},
     false}};
const asn1::table::seq_desc sl_freq_cfg_ext_v1800_s::desc = {{seq_table_sl_freq_cfg_ext_v1800_s_fields,
                                                              uint64_t{0x0},
                                                              seq_table_sl_freq_cfg_ext_v1800_s_opts,
                                                              seq_table_sl_freq_cfg_ext_v1800_s_names,
                                                              6,
                                                              6},
                                                             true,
                                                             false,
                                                             seq_table_sl_freq_cfg_ext_v1800_s_groups,
                                                             1};

static const asn1::table::storage_ops* const
    choice_storage_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<int8_t, true>::value,
        &asn1::table::alternative_ops<int8_t, true>::value};
const asn1::table::choice_storage_desc sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::storage_desc = {
    choice_storage_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__alternatives,
    2,
    "sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_"};

int8_t& sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::set_sl_max_energy_detection_thres_r18()
{
  set(types::sl_max_energy_detection_thres_r18);
  return c.get<int8_t>();
}
int8_t& sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::set_sl_energy_detection_thres_offset_r18()
{
  set(types::sl_energy_detection_thres_offset_r18);
  return c.get<int8_t>();
}
static const asn1::table::seq_field choice_table_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_i8,
      false,
      false,
      int64_t{-85},
      uint64_t{33},
      nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_i8,
      false,
      false,
      int64_t{-13},
      uint64_t{33},
      nullptr}};
static const asn1::table::field_name choice_table_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__names[] = {
    {"sl-MaxEnergyDetectionThreshold-r18", nullptr},
    {"sl-EnergyDetectionThresholdOffset-r18", nullptr}};
const asn1::table::choice_desc sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::desc = {
    asn1::table::enum_int_type<sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::types>(),
    sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::types::nof_types,
    sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::types::nof_exts,
    sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::types::has_ext,
    false,
    choice_table_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__alternatives,
    nullptr,
    choice_table_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__names,
    "sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_",
    offsetof(sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_, c.type),
    offsetof(sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_, c.value),
    false,
    &sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::storage_desc};

static const char* enum_names_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__types_values[] = {
    "sl-MaxEnergyDetectionThreshold-r18",
    "sl-EnergyDetectionThresholdOffset-r18"};
const asn1::table::enum_desc sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::types_opts::desc = {
    enum_names_sl_freq_cfg_ext_v1800_s_sl_energy_detection_cfg_r18_c__types_values,
    2,
    "sl_freq_cfg_ext_v1800_s::sl_energy_detection_cfg_r18_c_::types"};

// SL-SCCH-CarrierSetConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_s_cch_carrier_set_cfg_r18_s_fields[] = {
    {offsetof(sl_s_cch_carrier_set_cfg_r18_s, sl_dest_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(sl_s_cch_carrier_set_cfg_r18_s::sl_dest_list_r18),
                              asn1::table::struct_json>::value},
    {offsetof(sl_s_cch_carrier_set_cfg_r18_s, sl_srb_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::list_codec<decltype(sl_s_cch_carrier_set_cfg_r18_s::sl_srb_id_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{2}, false, false>>::value},
    {offsetof(sl_s_cch_carrier_set_cfg_r18_s, sl_allowed_carrier_freq_set1_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_s_cch_carrier_set_cfg_r18_s::sl_allowed_carrier_freq_set1_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{7}, false, false>>::value},
    {offsetof(sl_s_cch_carrier_set_cfg_r18_s, sl_allowed_carrier_freq_set2_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sl_s_cch_carrier_set_cfg_r18_s::sl_allowed_carrier_freq_set2_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{7}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_s_cch_carrier_set_cfg_r18_s_names[] = {
    {"sl-DestinationList-r18", nullptr},
    {"sl-SRB-Identity-r18", nullptr},
    {"sl-AllowedCarrierFreqSet1-r18", nullptr},
    {"sl-AllowedCarrierFreqSet2-r18", nullptr}};
const asn1::table::seq_desc sl_s_cch_carrier_set_cfg_r18_s::desc = {{seq_table_sl_s_cch_carrier_set_cfg_r18_s_fields,
                                                                     uint64_t{0xf},
                                                                     nullptr,
                                                                     seq_table_sl_s_cch_carrier_set_cfg_r18_s_names,
                                                                     4,
                                                                     0},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0};

// SL-PHY-MAC-RLC-Config-v1700 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_lbt_sched_request_id_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{7},
         nullptr}};
static const asn1::table::field_name
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_lbt_sched_request_id_r18_setup_release_names[] = {{"release", nullptr},
                                                                                              {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_lbt_sched_request_id_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_lbt_sched_request_id_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_lbt_sched_request_id_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 0, 7>>::type_offset(),
    setup_release_c<integer<uint8_t, 0, 7>>::value_offset()};
static const asn1::table::seq_field
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_s_cch_carrier_set_cfg_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         95,
         &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<sl_s_cch_carrier_set_cfg_r18_s, 1, 96>>::value_type,
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &sl_s_cch_carrier_set_cfg_r18_s::desc}};
static const asn1::table::field_name
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_s_cch_carrier_set_cfg_r18_setup_release_names[] = {{"release", nullptr},
                                                                                               {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_s_cch_carrier_set_cfg_r18_setup_release =
    {asn1::table::enum_int_type<asn1::setup_release_e>(),
     2,
     0,
     false,
     false,
     seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_s_cch_carrier_set_cfg_r18_setup_release_alternatives,
     nullptr,
     seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_s_cch_carrier_set_cfg_r18_setup_release_names,
     "setup_release_c",
     setup_release_c<dyn_seq_of<sl_s_cch_carrier_set_cfg_r18_s, 1, 96>>::type_offset(),
     setup_release_c<dyn_seq_of<sl_s_cch_carrier_set_cfg_r18_s, 1, 96>>::value_offset()};
static const asn1::table::seq_field
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_prs_sched_request_id_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{7},
         nullptr}};
static const asn1::table::field_name
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_prs_sched_request_id_r18_setup_release_names[] = {{"release", nullptr},
                                                                                              {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_prs_sched_request_id_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_prs_sched_request_id_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_prs_sched_request_id_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 0, 7>>::type_offset(),
    setup_release_c<integer<uint8_t, 0, 7>>::value_offset()};
static const asn1::table::seq_field seq_table_sl_phy_mac_rlc_cfg_v1700_s_fields[] = {
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_drx_cfg_r17),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_drx_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_drx_cfg_r17_s::desc},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_rlc_ch_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::list_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_rlc_ch_to_release_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{511}, false, false>>::value},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_rlc_ch_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::item_list_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_rlc_ch_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_rlc_ch_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_phy_mac_rlc_cfg_v1700_s_names[] = {
    {"sl-DRX-Config-r17", nullptr},
    {"sl-RLC-ChannelToReleaseList-r17", nullptr},
    {"sl-RLC-ChannelToAddModList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_phy_mac_rlc_cfg_v1700_s_opts[] = {
    {0, (int32_t)offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_drx_cfg_r17_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_sl_phy_mac_rlc_cfg_v1700_s_g0_fields[] = {
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_rlc_bearer_to_add_mod_list_size_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::ptr_item_list_codec<
         decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_rlc_bearer_to_add_mod_list_size_ext_v1800),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &sl_rlc_bearer_cfg_r16_s::desc},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_rlc_bearer_to_release_list_size_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::ptr_list_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_rlc_bearer_to_release_list_size_ext_v1800),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{513}, uint64_t{511}, false, false>>::value},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_freq_info_to_add_mod_list_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_freq_info_to_add_mod_list_ext_v1800),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_freq_cfg_ext_v1800_s::desc},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_lbt_sched_request_id_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_lbt_sched_request_id_r18)>::value,
     &seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_lbt_sched_request_id_r18_setup_release},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_sync_freq_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_list_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_sync_freq_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{1}, uint64_t{7}, false, false>>::value},
    {0,
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_sync_tx_multi_freq_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_max_trans_pwr_ca_r18),
     (int32_t)offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_max_trans_pwr_ca_r18_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_s_cch_carrier_set_cfg_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_s_cch_carrier_set_cfg_r18)>::value,
     &seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_s_cch_carrier_set_cfg_r18_setup_release},
    {offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_prs_sched_request_id_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_phy_mac_rlc_cfg_v1700_s::sl_prs_sched_request_id_r18)>::value,
     &seq_table_sl_phy_mac_rlc_cfg_v1700_s_sl_prs_sched_request_id_r18_setup_release}};
static const asn1::table::field_name seq_table_sl_phy_mac_rlc_cfg_v1700_s_g0_names[] = {
    {"sl-RLC-BearerToAddModListSizeExt-v1800", nullptr},
    {"sl-RLC-BearerToReleaseListSizeExt-v1800", nullptr},
    {"sl-FreqInfoToAddModListExt-v1800", nullptr},
    {"sl-LBT-SchedulingRequestId-r18", nullptr},
    {"sl-SyncFreqList-r18", nullptr},
    {"sl-SyncTxMultiFreq-r18", "true"},
    {"sl-MaxTransPowerCA-r18", nullptr},
    {"sl-SCCH-CarrierSetConfig-r18", nullptr},
    {"sl-PRS-SchedulingRequestId-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_phy_mac_rlc_cfg_v1700_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, asn1::table::seq_field::ptr_presence},
    {4, asn1::table::seq_field::ptr_presence},
    {5, (int32_t)offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_sync_tx_multi_freq_r18_present)},
    {6, (int32_t)offsetof(sl_phy_mac_rlc_cfg_v1700_s, sl_max_trans_pwr_ca_r18_present)},
    {7, asn1::table::seq_field::ptr_presence},
    {8, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_phy_mac_rlc_cfg_v1700_s_groups[] = {
    {{seq_table_sl_phy_mac_rlc_cfg_v1700_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_phy_mac_rlc_cfg_v1700_s_g0_opts,
      seq_table_sl_phy_mac_rlc_cfg_v1700_s_g0_names,
      9,
      9},
     false}};
const asn1::table::seq_desc sl_phy_mac_rlc_cfg_v1700_s::desc = {{seq_table_sl_phy_mac_rlc_cfg_v1700_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_sl_phy_mac_rlc_cfg_v1700_s_opts,
                                                                 seq_table_sl_phy_mac_rlc_cfg_v1700_s_names,
                                                                 3,
                                                                 3},
                                                                true,
                                                                false,
                                                                seq_table_sl_phy_mac_rlc_cfg_v1700_s_groups,
                                                                1};

// SL-RelayUE-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_relay_ue_cfg_r17_s_fields[] = {
    {offsetof(sl_relay_ue_cfg_r17_s, thresh_high_relay_r17),
     (int32_t)offsetof(sl_relay_ue_cfg_r17_s, thresh_high_relay_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {offsetof(sl_relay_ue_cfg_r17_s, thresh_low_relay_r17),
     (int32_t)offsetof(sl_relay_ue_cfg_r17_s, thresh_low_relay_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {offsetof(sl_relay_ue_cfg_r17_s, hyst_max_relay_r17),
     (int32_t)offsetof(sl_relay_ue_cfg_r17_s, hyst_max_relay_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr},
    {offsetof(sl_relay_ue_cfg_r17_s, hyst_min_relay_r17),
     (int32_t)offsetof(sl_relay_ue_cfg_r17_s, hyst_min_relay_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr}};
static const asn1::table::field_name seq_table_sl_relay_ue_cfg_r17_s_names[] = {{"threshHighRelay-r17", nullptr},
                                                                                {"threshLowRelay-r17", nullptr},
                                                                                {"hystMaxRelay-r17", nullptr},
                                                                                {"hystMinRelay-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_relay_ue_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(sl_relay_ue_cfg_r17_s, thresh_high_relay_r17_present)},
    {1, (int32_t)offsetof(sl_relay_ue_cfg_r17_s, thresh_low_relay_r17_present)},
    {2, (int32_t)offsetof(sl_relay_ue_cfg_r17_s, hyst_max_relay_r17_present)},
    {3, (int32_t)offsetof(sl_relay_ue_cfg_r17_s, hyst_min_relay_r17_present)}};
const asn1::table::seq_desc sl_relay_ue_cfg_r17_s::desc = {{seq_table_sl_relay_ue_cfg_r17_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_sl_relay_ue_cfg_r17_s_opts,
                                                            seq_table_sl_relay_ue_cfg_r17_s_names,
                                                            4,
                                                            4},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// SL-ReselectionConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_resel_cfg_r17_s_fields[] = {
    {offsetof(sl_resel_cfg_r17_s, sl_rsrp_thresh_r17),
     (int32_t)offsetof(sl_resel_cfg_r17_s, sl_rsrp_thresh_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(sl_resel_cfg_r17_s, sl_filt_coef_rsrp_r17),
     (int32_t)offsetof(sl_resel_cfg_r17_s, sl_filt_coef_rsrp_r17_present),
     asn1::table::enum_op<decltype(sl_resel_cfg_r17_s::sl_filt_coef_rsrp_r17)>(),
     decltype(sl_resel_cfg_r17_s::sl_filt_coef_rsrp_r17)::has_ext,
     false,
     decltype(sl_resel_cfg_r17_s::sl_filt_coef_rsrp_r17)::nof_types,
     decltype(sl_resel_cfg_r17_s::sl_filt_coef_rsrp_r17)::nof_exts,
     nullptr,
     &filt_coef_opts::desc},
    {offsetof(sl_resel_cfg_r17_s, sl_hyst_min_r17),
     (int32_t)offsetof(sl_resel_cfg_r17_s, sl_hyst_min_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr}};
static const asn1::table::field_name seq_table_sl_resel_cfg_r17_s_names[] = {{"sl-RSRP-Thresh-r17", nullptr},
                                                                             {"sl-FilterCoefficientRSRP-r17", nullptr},
                                                                             {"sl-HystMin-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_resel_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(sl_resel_cfg_r17_s, sl_rsrp_thresh_r17_present)},
    {1, (int32_t)offsetof(sl_resel_cfg_r17_s, sl_filt_coef_rsrp_r17_present)},
    {2, (int32_t)offsetof(sl_resel_cfg_r17_s, sl_hyst_min_r17_present)}};
const asn1::table::seq_desc sl_resel_cfg_r17_s::desc = {{seq_table_sl_resel_cfg_r17_s_fields,
                                                         uint64_t{0x0},
                                                         seq_table_sl_resel_cfg_r17_s_opts,
                                                         seq_table_sl_resel_cfg_r17_s_names,
                                                         3,
                                                         3},
                                                        false,
                                                        false,
                                                        nullptr,
                                                        0};

// SL-RemoteUE-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_remote_ue_cfg_r17_s_fields[] = {
    {offsetof(sl_remote_ue_cfg_r17_s, thresh_high_remote_r17),
     (int32_t)offsetof(sl_remote_ue_cfg_r17_s, thresh_high_remote_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {offsetof(sl_remote_ue_cfg_r17_s, hyst_max_remote_r17),
     (int32_t)offsetof(sl_remote_ue_cfg_r17_s, hyst_max_remote_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr},
    {offsetof(sl_remote_ue_cfg_r17_s, sl_resel_cfg_r17),
     (int32_t)offsetof(sl_remote_ue_cfg_r17_s, sl_resel_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_resel_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_remote_ue_cfg_r17_s_names[] = {{"threshHighRemote-r17", nullptr},
                                                                                 {"hystMaxRemote-r17", nullptr},
                                                                                 {"sl-ReselectionConfig-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_remote_ue_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(sl_remote_ue_cfg_r17_s, thresh_high_remote_r17_present)},
    {1, (int32_t)offsetof(sl_remote_ue_cfg_r17_s, hyst_max_remote_r17_present)},
    {2, (int32_t)offsetof(sl_remote_ue_cfg_r17_s, sl_resel_cfg_r17_present)}};
const asn1::table::seq_desc sl_remote_ue_cfg_r17_s::desc = {{seq_table_sl_remote_ue_cfg_r17_s_fields,
                                                             uint64_t{0x0},
                                                             seq_table_sl_remote_ue_cfg_r17_s_opts,
                                                             seq_table_sl_remote_ue_cfg_r17_s_names,
                                                             3,
                                                             3},
                                                            false,
                                                            false,
                                                            nullptr,
                                                            0};

// SL-DiscConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_disc_cfg_r17_s_sl_relay_ue_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_relay_ue_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_r17_s_sl_relay_ue_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_disc_cfg_r17_s_sl_relay_ue_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_disc_cfg_r17_s_sl_relay_ue_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sl_disc_cfg_r17_s_sl_relay_ue_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_relay_ue_cfg_r17_s>::type_offset(),
    setup_release_c<sl_relay_ue_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_disc_cfg_r17_s_sl_remote_ue_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_remote_ue_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_r17_s_sl_remote_ue_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_disc_cfg_r17_s_sl_remote_ue_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_disc_cfg_r17_s_sl_remote_ue_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sl_disc_cfg_r17_s_sl_remote_ue_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_remote_ue_cfg_r17_s>::type_offset(),
    setup_release_c<sl_remote_ue_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_disc_cfg_r17_s_fields[] = {
    {offsetof(sl_disc_cfg_r17_s, sl_relay_ue_cfg_r17),
     (int32_t)offsetof(sl_disc_cfg_r17_s, sl_relay_ue_cfg_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_disc_cfg_r17_s_sl_relay_ue_cfg_r17_setup_release},
    {offsetof(sl_disc_cfg_r17_s, sl_remote_ue_cfg_r17),
     (int32_t)offsetof(sl_disc_cfg_r17_s, sl_remote_ue_cfg_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_disc_cfg_r17_s_sl_remote_ue_cfg_r17_setup_release}};
static const asn1::table::field_name seq_table_sl_disc_cfg_r17_s_names[] = {{"sl-RelayUE-Config-r17", nullptr},
                                                                            {"sl-RemoteUE-Config-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_disc_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(sl_disc_cfg_r17_s, sl_relay_ue_cfg_r17_present)},
    {1, (int32_t)offsetof(sl_disc_cfg_r17_s, sl_remote_ue_cfg_r17_present)}};
const asn1::table::seq_desc sl_disc_cfg_r17_s::desc = {{seq_table_sl_disc_cfg_r17_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_sl_disc_cfg_r17_s_opts,
                                                        seq_table_sl_disc_cfg_r17_s_names,
                                                        2,
                                                        2},
                                                       false,
                                                       false,
                                                       nullptr,
                                                       0};

// SL-RelayUE-ConfigU2U-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_relay_ue_cfg_u2_u_r18_s_fields[] = {
    {offsetof(sl_relay_ue_cfg_u2_u_r18_s, sl_rsrp_thresh_disc_cfg_r18),
     (int32_t)offsetof(sl_relay_ue_cfg_u2_u_r18_s, sl_rsrp_thresh_disc_cfg_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(sl_relay_ue_cfg_u2_u_r18_s, sd_rsrp_thresh_disc_cfg_r18),
     (int32_t)offsetof(sl_relay_ue_cfg_u2_u_r18_s, sd_rsrp_thresh_disc_cfg_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(sl_relay_ue_cfg_u2_u_r18_s, sd_hyst_max_relay_r18),
     (int32_t)offsetof(sl_relay_ue_cfg_u2_u_r18_s, sd_hyst_max_relay_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr}};
static const asn1::table::field_name seq_table_sl_relay_ue_cfg_u2_u_r18_s_names[] = {
    {"sl-RSRP-Thresh-DiscConfig-r18", nullptr},
    {"sd-RSRP-ThreshDiscConfig-r18", nullptr},
    {"sd-hystMaxRelay-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_relay_ue_cfg_u2_u_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_relay_ue_cfg_u2_u_r18_s, sl_rsrp_thresh_disc_cfg_r18_present)},
    {1, (int32_t)offsetof(sl_relay_ue_cfg_u2_u_r18_s, sd_rsrp_thresh_disc_cfg_r18_present)},
    {2, (int32_t)offsetof(sl_relay_ue_cfg_u2_u_r18_s, sd_hyst_max_relay_r18_present)}};
const asn1::table::seq_desc sl_relay_ue_cfg_u2_u_r18_s::desc = {{seq_table_sl_relay_ue_cfg_u2_u_r18_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_sl_relay_ue_cfg_u2_u_r18_s_opts,
                                                                 seq_table_sl_relay_ue_cfg_u2_u_r18_s_names,
                                                                 3,
                                                                 3},
                                                                false,
                                                                false,
                                                                nullptr,
                                                                0};

// SL-RemoteUE-ConfigU2U-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_remote_ue_cfg_u2_u_r18_s_fields[] = {
    {offsetof(sl_remote_ue_cfg_u2_u_r18_s, sl_rsrp_thresh_u2_u_r18),
     (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sl_rsrp_thresh_u2_u_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(sl_remote_ue_cfg_u2_u_r18_s, sl_hyst_min_u2_u_r18),
     (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sl_hyst_min_u2_u_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr},
    {offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_rsrp_thresh_u2_u_r18),
     (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_rsrp_thresh_u2_u_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_filt_coef_u2_u_r18),
     (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_filt_coef_u2_u_r18_present),
     asn1::table::enum_op<decltype(sl_remote_ue_cfg_u2_u_r18_s::sd_filt_coef_u2_u_r18)>(),
     decltype(sl_remote_ue_cfg_u2_u_r18_s::sd_filt_coef_u2_u_r18)::has_ext,
     false,
     decltype(sl_remote_ue_cfg_u2_u_r18_s::sd_filt_coef_u2_u_r18)::nof_types,
     decltype(sl_remote_ue_cfg_u2_u_r18_s::sd_filt_coef_u2_u_r18)::nof_exts,
     nullptr,
     &filt_coef_opts::desc},
    {offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_hyst_min_u2_u_r18),
     (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_hyst_min_u2_u_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{30},
     nullptr}};
static const asn1::table::field_name seq_table_sl_remote_ue_cfg_u2_u_r18_s_names[] = {
    {"sl-RSRP-ThreshU2U-r18", nullptr},
    {"sl-HystMinU2U-r18", nullptr},
    {"sd-RSRP-ThreshU2U-r18", nullptr},
    {"sd-FilterCoefficientU2U-r18", nullptr},
    {"sd-HystMinU2U-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_remote_ue_cfg_u2_u_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sl_rsrp_thresh_u2_u_r18_present)},
    {1, (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sl_hyst_min_u2_u_r18_present)},
    {2, (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_rsrp_thresh_u2_u_r18_present)},
    {3, (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_filt_coef_u2_u_r18_present)},
    {4, (int32_t)offsetof(sl_remote_ue_cfg_u2_u_r18_s, sd_hyst_min_u2_u_r18_present)}};
const asn1::table::seq_desc sl_remote_ue_cfg_u2_u_r18_s::desc = {{seq_table_sl_remote_ue_cfg_u2_u_r18_s_fields,
                                                                  uint64_t{0x0},
                                                                  seq_table_sl_remote_ue_cfg_u2_u_r18_s_opts,
                                                                  seq_table_sl_remote_ue_cfg_u2_u_r18_s_names,
                                                                  5,
                                                                  5},
                                                                 false,
                                                                 false,
                                                                 nullptr,
                                                                 0};

// SL-DiscConfig-v1800 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sl_disc_cfg_v1800_s_sl_relay_ue_cfg_u2_u_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_relay_ue_cfg_u2_u_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_v1800_s_sl_relay_ue_cfg_u2_u_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_disc_cfg_v1800_s_sl_relay_ue_cfg_u2_u_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_disc_cfg_v1800_s_sl_relay_ue_cfg_u2_u_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_disc_cfg_v1800_s_sl_relay_ue_cfg_u2_u_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_relay_ue_cfg_u2_u_r18_s>::type_offset(),
    setup_release_c<sl_relay_ue_cfg_u2_u_r18_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_sl_disc_cfg_v1800_s_sl_remote_ue_cfg_u2_u_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_remote_ue_cfg_u2_u_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_v1800_s_sl_remote_ue_cfg_u2_u_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_disc_cfg_v1800_s_sl_remote_ue_cfg_u2_u_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_disc_cfg_v1800_s_sl_remote_ue_cfg_u2_u_r18_setup_release_alternatives,
    nullptr,
    seq_table_sl_disc_cfg_v1800_s_sl_remote_ue_cfg_u2_u_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_remote_ue_cfg_u2_u_r18_s>::type_offset(),
    setup_release_c<sl_remote_ue_cfg_u2_u_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_disc_cfg_v1800_s_fields[] = {
    {offsetof(sl_disc_cfg_v1800_s, sl_relay_ue_cfg_u2_u_r18),
     (int32_t)offsetof(sl_disc_cfg_v1800_s, sl_relay_ue_cfg_u2_u_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_disc_cfg_v1800_s_sl_relay_ue_cfg_u2_u_r18_setup_release},
    {offsetof(sl_disc_cfg_v1800_s, sl_remote_ue_cfg_u2_u_r18),
     (int32_t)offsetof(sl_disc_cfg_v1800_s, sl_remote_ue_cfg_u2_u_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_disc_cfg_v1800_s_sl_remote_ue_cfg_u2_u_r18_setup_release}};
static const asn1::table::field_name seq_table_sl_disc_cfg_v1800_s_names[] = {{"sl-RelayUE-ConfigU2U-r18", nullptr},
                                                                              {"sl-RemoteUE-ConfigU2U-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_disc_cfg_v1800_s_opts[]  = {
    {0, (int32_t)offsetof(sl_disc_cfg_v1800_s, sl_relay_ue_cfg_u2_u_r18_present)},
    {1, (int32_t)offsetof(sl_disc_cfg_v1800_s, sl_remote_ue_cfg_u2_u_r18_present)}};
const asn1::table::seq_desc sl_disc_cfg_v1800_s::desc = {{seq_table_sl_disc_cfg_v1800_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_sl_disc_cfg_v1800_s_opts,
                                                          seq_table_sl_disc_cfg_v1800_s_names,
                                                          2,
                                                          2},
                                                         false,
                                                         false,
                                                         nullptr,
                                                         0};

// SL-RemoteUE-ConfigU2U-v1830 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_remote_ue_cfg_u2_u_v1830_s_fields[] = {
    {offsetof(sl_remote_ue_cfg_u2_u_v1830_s, sl_filt_coef_u2_u_r18),
     (int32_t)offsetof(sl_remote_ue_cfg_u2_u_v1830_s, sl_filt_coef_u2_u_r18_present),
     asn1::table::enum_op<decltype(sl_remote_ue_cfg_u2_u_v1830_s::sl_filt_coef_u2_u_r18)>(),
     decltype(sl_remote_ue_cfg_u2_u_v1830_s::sl_filt_coef_u2_u_r18)::has_ext,
     false,
     decltype(sl_remote_ue_cfg_u2_u_v1830_s::sl_filt_coef_u2_u_r18)::nof_types,
     decltype(sl_remote_ue_cfg_u2_u_v1830_s::sl_filt_coef_u2_u_r18)::nof_exts,
     nullptr,
     &filt_coef_opts::desc}};
static const asn1::table::field_name seq_table_sl_remote_ue_cfg_u2_u_v1830_s_names[] = {
    {"sl-FilterCoefficientU2U-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_remote_ue_cfg_u2_u_v1830_s_opts[] = {
    {0, (int32_t)offsetof(sl_remote_ue_cfg_u2_u_v1830_s, sl_filt_coef_u2_u_r18_present)}};
const asn1::table::seq_desc sl_remote_ue_cfg_u2_u_v1830_s::desc = {{seq_table_sl_remote_ue_cfg_u2_u_v1830_s_fields,
                                                                    uint64_t{0x0},
                                                                    seq_table_sl_remote_ue_cfg_u2_u_v1830_s_opts,
                                                                    seq_table_sl_remote_ue_cfg_u2_u_v1830_s_names,
                                                                    1,
                                                                    1},
                                                                   true,
                                                                   false,
                                                                   nullptr,
                                                                   0};

// SL-DiscConfig-v1830 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sl_disc_cfg_v1830_s_sl_remote_ue_cfg_u2_u_v1830_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_remote_ue_cfg_u2_u_v1830_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_v1830_s_sl_remote_ue_cfg_u2_u_v1830_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_disc_cfg_v1830_s_sl_remote_ue_cfg_u2_u_v1830_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_disc_cfg_v1830_s_sl_remote_ue_cfg_u2_u_v1830_setup_release_alternatives,
    nullptr,
    seq_table_sl_disc_cfg_v1830_s_sl_remote_ue_cfg_u2_u_v1830_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_remote_ue_cfg_u2_u_v1830_s>::type_offset(),
    setup_release_c<sl_remote_ue_cfg_u2_u_v1830_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_disc_cfg_v1830_s_fields[] = {
    {offsetof(sl_disc_cfg_v1830_s, sl_remote_ue_cfg_u2_u_v1830),
     (int32_t)offsetof(sl_disc_cfg_v1830_s, sl_remote_ue_cfg_u2_u_v1830_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_disc_cfg_v1830_s_sl_remote_ue_cfg_u2_u_v1830_setup_release}};
static const asn1::table::field_name seq_table_sl_disc_cfg_v1830_s_names[] = {{"sl-RemoteUE-ConfigU2U-v1830", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_disc_cfg_v1830_s_opts[]  = {
    {0, (int32_t)offsetof(sl_disc_cfg_v1830_s, sl_remote_ue_cfg_u2_u_v1830_present)}};
const asn1::table::seq_desc sl_disc_cfg_v1830_s::desc = {{seq_table_sl_disc_cfg_v1830_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_sl_disc_cfg_v1830_s_opts,
                                                          seq_table_sl_disc_cfg_v1830_s_names,
                                                          1,
                                                          1},
                                                         false,
                                                         false,
                                                         nullptr,
                                                         0};

// SL-RelayUE-ConfigU2U-v1840 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_relay_ue_cfg_u2_u_v1840_s_fields[] = {
    {offsetof(sl_relay_ue_cfg_u2_u_v1840_s, sl_filt_coef_u2_u_r18),
     (int32_t)offsetof(sl_relay_ue_cfg_u2_u_v1840_s, sl_filt_coef_u2_u_r18_present),
     asn1::table::enum_op<decltype(sl_relay_ue_cfg_u2_u_v1840_s::sl_filt_coef_u2_u_r18)>(),
     decltype(sl_relay_ue_cfg_u2_u_v1840_s::sl_filt_coef_u2_u_r18)::has_ext,
     false,
     decltype(sl_relay_ue_cfg_u2_u_v1840_s::sl_filt_coef_u2_u_r18)::nof_types,
     decltype(sl_relay_ue_cfg_u2_u_v1840_s::sl_filt_coef_u2_u_r18)::nof_exts,
     nullptr,
     &filt_coef_opts::desc},
    {offsetof(sl_relay_ue_cfg_u2_u_v1840_s, sd_filt_coef_u2_u_r18),
     (int32_t)offsetof(sl_relay_ue_cfg_u2_u_v1840_s, sd_filt_coef_u2_u_r18_present),
     asn1::table::enum_op<decltype(sl_relay_ue_cfg_u2_u_v1840_s::sd_filt_coef_u2_u_r18)>(),
     decltype(sl_relay_ue_cfg_u2_u_v1840_s::sd_filt_coef_u2_u_r18)::has_ext,
     false,
     decltype(sl_relay_ue_cfg_u2_u_v1840_s::sd_filt_coef_u2_u_r18)::nof_types,
     decltype(sl_relay_ue_cfg_u2_u_v1840_s::sd_filt_coef_u2_u_r18)::nof_exts,
     nullptr,
     &filt_coef_opts::desc}};
static const asn1::table::field_name seq_table_sl_relay_ue_cfg_u2_u_v1840_s_names[] = {
    {"sl-FilterCoefficientU2U-r18", nullptr},
    {"sd-FilterCoefficientU2U-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_relay_ue_cfg_u2_u_v1840_s_opts[] = {
    {0, (int32_t)offsetof(sl_relay_ue_cfg_u2_u_v1840_s, sl_filt_coef_u2_u_r18_present)},
    {1, (int32_t)offsetof(sl_relay_ue_cfg_u2_u_v1840_s, sd_filt_coef_u2_u_r18_present)}};
const asn1::table::seq_desc sl_relay_ue_cfg_u2_u_v1840_s::desc = {{seq_table_sl_relay_ue_cfg_u2_u_v1840_s_fields,
                                                                   uint64_t{0x0},
                                                                   seq_table_sl_relay_ue_cfg_u2_u_v1840_s_opts,
                                                                   seq_table_sl_relay_ue_cfg_u2_u_v1840_s_names,
                                                                   2,
                                                                   2},
                                                                  true,
                                                                  false,
                                                                  nullptr,
                                                                  0};

// SL-DiscConfig-v1840 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sl_disc_cfg_v1840_s_sl_relay_ue_cfg_u2_u_v1840_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_relay_ue_cfg_u2_u_v1840_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_v1840_s_sl_relay_ue_cfg_u2_u_v1840_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_disc_cfg_v1840_s_sl_relay_ue_cfg_u2_u_v1840_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_disc_cfg_v1840_s_sl_relay_ue_cfg_u2_u_v1840_setup_release_alternatives,
    nullptr,
    seq_table_sl_disc_cfg_v1840_s_sl_relay_ue_cfg_u2_u_v1840_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_relay_ue_cfg_u2_u_v1840_s>::type_offset(),
    setup_release_c<sl_relay_ue_cfg_u2_u_v1840_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_disc_cfg_v1840_s_fields[] = {
    {offsetof(sl_disc_cfg_v1840_s, sl_relay_ue_cfg_u2_u_v1840),
     (int32_t)offsetof(sl_disc_cfg_v1840_s, sl_relay_ue_cfg_u2_u_v1840_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_disc_cfg_v1840_s_sl_relay_ue_cfg_u2_u_v1840_setup_release}};
static const asn1::table::field_name seq_table_sl_disc_cfg_v1840_s_names[] = {{"sl-RelayUE-ConfigU2U-v1840", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_disc_cfg_v1840_s_opts[]  = {
    {0, (int32_t)offsetof(sl_disc_cfg_v1840_s, sl_relay_ue_cfg_u2_u_v1840_present)}};
const asn1::table::seq_desc sl_disc_cfg_v1840_s::desc = {{seq_table_sl_disc_cfg_v1840_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_sl_disc_cfg_v1840_s_opts,
                                                          seq_table_sl_disc_cfg_v1840_s_names,
                                                          1,
                                                          1},
                                                         false,
                                                         false,
                                                         nullptr,
                                                         0};

// SL-ConfigDedicatedNR-r16 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sl_cfg_ded_nr_r16_s_sl_phy_mac_rlc_cfg_v1700_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_phy_mac_rlc_cfg_v1700_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_r16_s_sl_phy_mac_rlc_cfg_v1700_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_cfg_ded_nr_r16_s_sl_phy_mac_rlc_cfg_v1700_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_cfg_ded_nr_r16_s_sl_phy_mac_rlc_cfg_v1700_setup_release_alternatives,
    nullptr,
    seq_table_sl_cfg_ded_nr_r16_s_sl_phy_mac_rlc_cfg_v1700_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_phy_mac_rlc_cfg_v1700_s>::type_offset(),
    setup_release_c<sl_phy_mac_rlc_cfg_v1700_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_cfg_ded_nr_r16_s_sl_disc_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_disc_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_r16_s_sl_disc_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sl_cfg_ded_nr_r16_s_sl_disc_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sl_cfg_ded_nr_r16_s_sl_disc_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sl_cfg_ded_nr_r16_s_sl_disc_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_disc_cfg_r17_s>::type_offset(),
    setup_release_c<sl_disc_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_sl_cfg_ded_nr_r16_s_fields[] = {
    {offsetof(sl_cfg_ded_nr_r16_s, sl_phy_mac_rlc_cfg_r16),
     (int32_t)offsetof(sl_cfg_ded_nr_r16_s, sl_phy_mac_rlc_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_phy_mac_rlc_cfg_r16_s::desc},
    {offsetof(sl_cfg_ded_nr_r16_s, sl_radio_bearer_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::list_codec<decltype(sl_cfg_ded_nr_r16_s::sl_radio_bearer_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{511}, false, false>>::value},
    {offsetof(sl_cfg_ded_nr_r16_s, sl_radio_bearer_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::item_list_codec<decltype(sl_cfg_ded_nr_r16_s::sl_radio_bearer_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_radio_bearer_cfg_r16_s::desc},
    {offsetof(sl_cfg_ded_nr_r16_s, sl_meas_cfg_info_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(sl_cfg_ded_nr_r16_s::sl_meas_cfg_info_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(sl_cfg_ded_nr_r16_s, sl_meas_cfg_info_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(sl_cfg_ded_nr_r16_s::sl_meas_cfg_info_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_meas_cfg_info_r16_s::desc},
    {offsetof(sl_cfg_ded_nr_r16_s, t400_r16),
     (int32_t)offsetof(sl_cfg_ded_nr_r16_s, t400_r16_present),
     asn1::table::enum_op<decltype(sl_cfg_ded_nr_r16_s::t400_r16)>(),
     decltype(sl_cfg_ded_nr_r16_s::t400_r16)::has_ext,
     false,
     decltype(sl_cfg_ded_nr_r16_s::t400_r16)::nof_types,
     decltype(sl_cfg_ded_nr_r16_s::t400_r16)::nof_exts,
     nullptr,
     &sl_cfg_ded_nr_r16_s::t400_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_r16_s_names[] = {
    {"sl-PHY-MAC-RLC-Config-r16", nullptr},
    {"sl-RadioBearerToReleaseList-r16", nullptr},
    {"sl-RadioBearerToAddModList-r16", nullptr},
    {"sl-MeasConfigInfoToReleaseList-r16", nullptr},
    {"sl-MeasConfigInfoToAddModList-r16", nullptr},
    {"t400-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_ded_nr_r16_s_opts[] = {
    {0, (int32_t)offsetof(sl_cfg_ded_nr_r16_s, sl_phy_mac_rlc_cfg_r16_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(sl_cfg_ded_nr_r16_s, t400_r16_present)}};
static const asn1::table::seq_field seq_table_sl_cfg_ded_nr_r16_s_g0_fields[] = {
    {offsetof(sl_cfg_ded_nr_r16_s, sl_phy_mac_rlc_cfg_v1700),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_cfg_ded_nr_r16_s::sl_phy_mac_rlc_cfg_v1700)>::value,
     &seq_table_sl_cfg_ded_nr_r16_s_sl_phy_mac_rlc_cfg_v1700_setup_release},
    {offsetof(sl_cfg_ded_nr_r16_s, sl_disc_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_cfg_ded_nr_r16_s::sl_disc_cfg_r17)>::value,
     &seq_table_sl_cfg_ded_nr_r16_s_sl_disc_cfg_r17_setup_release}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_r16_s_g0_names[] = {
    {"sl-PHY-MAC-RLC-Config-v1700", nullptr},
    {"sl-DiscConfig-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_ded_nr_r16_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                             {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_cfg_ded_nr_r16_s_g1_fields[] = {
    {offsetof(sl_cfg_ded_nr_r16_s, sl_disc_cfg_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_cfg_ded_nr_r16_s::sl_disc_cfg_v1800)>::value,
     &sl_disc_cfg_v1800_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_r16_s_g1_names[] = {{"sl-DiscConfig-v1800", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_ded_nr_r16_s_g1_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_cfg_ded_nr_r16_s_g2_fields[] = {
    {offsetof(sl_cfg_ded_nr_r16_s, sl_disc_cfg_v1830),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_cfg_ded_nr_r16_s::sl_disc_cfg_v1830)>::value,
     &sl_disc_cfg_v1830_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_r16_s_g2_names[] = {{"sl-DiscConfig-v1830", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_ded_nr_r16_s_g2_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sl_cfg_ded_nr_r16_s_g3_fields[] = {
    {offsetof(sl_cfg_ded_nr_r16_s, sl_disc_cfg_v1840),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_cfg_ded_nr_r16_s::sl_disc_cfg_v1840)>::value,
     &sl_disc_cfg_v1840_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_r16_s_g3_names[] = {{"sl-DiscConfig-v1840", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_ded_nr_r16_s_g3_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_cfg_ded_nr_r16_s_groups[] = {{{seq_table_sl_cfg_ded_nr_r16_s_g0_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g0_opts,
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g0_names,
                                                                                2,
                                                                                2},
                                                                               false},
                                                                              {{seq_table_sl_cfg_ded_nr_r16_s_g1_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g1_opts,
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g1_names,
                                                                                1,
                                                                                1},
                                                                               false},
                                                                              {{seq_table_sl_cfg_ded_nr_r16_s_g2_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g2_opts,
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g2_names,
                                                                                1,
                                                                                1},
                                                                               false},
                                                                              {{seq_table_sl_cfg_ded_nr_r16_s_g3_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g3_opts,
                                                                                seq_table_sl_cfg_ded_nr_r16_s_g3_names,
                                                                                1,
                                                                                1},
                                                                               false}};
const asn1::table::seq_desc         sl_cfg_ded_nr_r16_s::desc              = {{seq_table_sl_cfg_ded_nr_r16_s_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_sl_cfg_ded_nr_r16_s_opts,
                                                                               seq_table_sl_cfg_ded_nr_r16_s_names,
                                                                               6,
                                                                               6},
                                                                              true,
                                                                              false,
                                                                              seq_table_sl_cfg_ded_nr_r16_s_groups,
                                                                              4};

static const char* enum_names_sl_cfg_ded_nr_r16_s_t400_r16_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc sl_cfg_ded_nr_r16_s::t400_r16_opts::desc = {
    enum_names_sl_cfg_ded_nr_r16_s_t400_r16_e__values,
    8,
    "sl_cfg_ded_nr_r16_s::t400_r16_e_"};
uint16_t sl_cfg_ded_nr_r16_s::t400_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "sl_cfg_ded_nr_r16_s::t400_r16_e_");
}

// SL-RemoteUE-RB-Identity-r17 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sl_remote_ue_rb_id_r17_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc sl_remote_ue_rb_id_r17_c::storage_desc = {
    choice_storage_sl_remote_ue_rb_id_r17_c_alternatives,
    2,
    "sl_remote_ue_rb_id_r17_c"};

uint8_t& sl_remote_ue_rb_id_r17_c::set_srb_id_r17()
{
  set(types::srb_id_r17);
  return c.get<uint8_t>();
}
uint8_t& sl_remote_ue_rb_id_r17_c::set_drb_id_r17()
{
  set(types::drb_id_r17);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_sl_remote_ue_rb_id_r17_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{3},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name choice_table_sl_remote_ue_rb_id_r17_c_names[] = {{"srb-Identity-r17", nullptr},
                                                                                      {"drb-Identity-r17", nullptr}};
const asn1::table::choice_desc       sl_remote_ue_rb_id_r17_c::desc                = {
    asn1::table::enum_int_type<sl_remote_ue_rb_id_r17_c::types>(),
    sl_remote_ue_rb_id_r17_c::types::nof_types,
    sl_remote_ue_rb_id_r17_c::types::nof_exts,
    sl_remote_ue_rb_id_r17_c::types::has_ext,
    false,
    choice_table_sl_remote_ue_rb_id_r17_c_alternatives,
    nullptr,
    choice_table_sl_remote_ue_rb_id_r17_c_names,
    "sl_remote_ue_rb_id_r17_c",
    offsetof(sl_remote_ue_rb_id_r17_c, c.type),
    offsetof(sl_remote_ue_rb_id_r17_c, c.value),
    false,
    &sl_remote_ue_rb_id_r17_c::storage_desc};

static const char* enum_names_sl_remote_ue_rb_id_r17_c_types_values[]   = {"srb-Identity-r17", "drb-Identity-r17"};
const asn1::table::enum_desc sl_remote_ue_rb_id_r17_c::types_opts::desc = {
    enum_names_sl_remote_ue_rb_id_r17_c_types_values,
    2,
    "sl_remote_ue_rb_id_r17_c::types"};

// SL-MappingToAddMod-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_map_to_add_mod_r17_s_fields[] = {
    {offsetof(sl_map_to_add_mod_r17_s, sl_remote_ue_rb_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_remote_ue_rb_id_r17_c::desc},
    {offsetof(sl_map_to_add_mod_r17_s, sl_egress_rlc_ch_uu_r17),
     (int32_t)offsetof(sl_map_to_add_mod_r17_s, sl_egress_rlc_ch_uu_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(sl_map_to_add_mod_r17_s, sl_egress_rlc_ch_pc5_r17),
     (int32_t)offsetof(sl_map_to_add_mod_r17_s, sl_egress_rlc_ch_pc5_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr}};
static const asn1::table::field_name seq_table_sl_map_to_add_mod_r17_s_names[] = {
    {"sl-RemoteUE-RB-Identity-r17", nullptr},
    {"sl-EgressRLC-ChannelUu-r17", nullptr},
    {"sl-EgressRLC-ChannelPC5-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_map_to_add_mod_r17_s_opts[] = {
    {1, (int32_t)offsetof(sl_map_to_add_mod_r17_s, sl_egress_rlc_ch_uu_r17_present)},
    {2, (int32_t)offsetof(sl_map_to_add_mod_r17_s, sl_egress_rlc_ch_pc5_r17_present)}};
const asn1::table::seq_desc sl_map_to_add_mod_r17_s::desc = {{seq_table_sl_map_to_add_mod_r17_s_fields,
                                                              uint64_t{0x1},
                                                              seq_table_sl_map_to_add_mod_r17_s_opts,
                                                              seq_table_sl_map_to_add_mod_r17_s_names,
                                                              3,
                                                              2},
                                                             true,
                                                             false,
                                                             nullptr,
                                                             0};

// SL-SRAP-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_srap_cfg_r17_s_fields[] = {
    {offsetof(sl_srap_cfg_r17_s, sl_local_id_r17),
     (int32_t)offsetof(sl_srap_cfg_r17_s, sl_local_id_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(sl_srap_cfg_r17_s, sl_map_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(sl_srap_cfg_r17_s::sl_map_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_map_to_add_mod_r17_s::desc},
    {offsetof(sl_srap_cfg_r17_s, sl_map_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(sl_srap_cfg_r17_s::sl_map_to_release_list_r17),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &sl_remote_ue_rb_id_r17_c::desc}};
static const asn1::table::field_name seq_table_sl_srap_cfg_r17_s_names[] = {{"sl-LocalIdentity-r17", nullptr},
                                                                            {"sl-MappingToAddModList-r17", nullptr},
                                                                            {"sl-MappingToReleaseList-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_srap_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(sl_srap_cfg_r17_s, sl_local_id_r17_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_srap_cfg_r17_s::desc = {{seq_table_sl_srap_cfg_r17_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_sl_srap_cfg_r17_s_opts,
                                                        seq_table_sl_srap_cfg_r17_s_names,
                                                        3,
                                                        3},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

// SL-MappingConfig-U2U-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_map_cfg_u2_u_r18_s_fields[] = {
    {offsetof(sl_map_cfg_u2_u_r18_s, sl_remote_ue_slrb_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr},
    {offsetof(sl_map_cfg_u2_u_r18_s, sl_egress_rlc_ch_pc5_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr}};
static const asn1::table::field_name seq_table_sl_map_cfg_u2_u_r18_s_names[] = {
    {"sl-RemoteUE-SLRB-Identity-r18", nullptr},
    {"sl-EgressRLC-ChannelPC5-r18", nullptr}};
const asn1::table::seq_desc sl_map_cfg_u2_u_r18_s::desc = {
    {seq_table_sl_map_cfg_u2_u_r18_s_fields, uint64_t{0x3}, nullptr, seq_table_sl_map_cfg_u2_u_r18_s_names, 2, 0},
    true,
    false,
    nullptr,
    0};

// SL-SRAP-ConfigU2U-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_srap_cfg_u2_u_r18_s_fields[] = {
    {offsetof(sl_srap_cfg_u2_u_r18_s, sl_map_to_add_mod_u2_u_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::item_list_codec<decltype(sl_srap_cfg_u2_u_r18_s::sl_map_to_add_mod_u2_u_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_map_cfg_u2_u_r18_s::desc},
    {offsetof(sl_srap_cfg_u2_u_r18_s, sl_map_to_release_u2_u_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     511,
     &asn1::table::list_codec<decltype(sl_srap_cfg_u2_u_r18_s::sl_map_to_release_u2_u_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{511}, false, false>>::value}};
static const asn1::table::field_name seq_table_sl_srap_cfg_u2_u_r18_s_names[] = {
    {"sl-MappingToAddMod-U2U-List-r18", nullptr},
    {"sl-MappingToRelease-U2U-List-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_srap_cfg_u2_u_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_srap_cfg_u2_u_r18_s::desc = {{seq_table_sl_srap_cfg_u2_u_r18_s_fields,
                                                             uint64_t{0x0},
                                                             seq_table_sl_srap_cfg_u2_u_r18_s_opts,
                                                             seq_table_sl_srap_cfg_u2_u_r18_s_names,
                                                             2,
                                                             2},
                                                            false,
                                                            false,
                                                            nullptr,
                                                            0};

// SL-TargetRemoteUE-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_target_remote_ue_cfg_r18_s_fields[] = {
    {offsetof(sl_target_remote_ue_cfg_r18_s, sl_target_ue_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_target_remote_ue_cfg_r18_s::sl_target_ue_id_r18),
                                asn1::table::struct_json>::value},
    {offsetof(sl_target_remote_ue_cfg_r18_s, sl_srap_cfg_u2_u_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_srap_cfg_u2_u_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_target_remote_ue_cfg_r18_s_names[] = {
    {"sl-TargetUE-Identity-r18", nullptr},
    {"sl-SRAP-ConfigU2U-r18", nullptr}};
const asn1::table::seq_desc sl_target_remote_ue_cfg_r18_s::desc = {{seq_table_sl_target_remote_ue_cfg_r18_s_fields,
                                                                    uint64_t{0x3},
                                                                    nullptr,
                                                                    seq_table_sl_target_remote_ue_cfg_r18_s_names,
                                                                    2,
                                                                    0},
                                                                   true,
                                                                   false,
                                                                   nullptr,
                                                                   0};

// SL-U2U-RelayUE-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_u2_u_relay_ue_cfg_r18_s_fields[] = {
    {offsetof(sl_u2_u_relay_ue_cfg_r18_s, sl_l2_id_relay_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_u2_u_relay_ue_cfg_r18_s::sl_l2_id_relay_r18),
                                asn1::table::struct_json>::value},
    {offsetof(sl_u2_u_relay_ue_cfg_r18_s, sl_target_remote_ue_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(sl_u2_u_relay_ue_cfg_r18_s::sl_target_remote_ue_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_target_remote_ue_cfg_r18_s::desc},
    {offsetof(sl_u2_u_relay_ue_cfg_r18_s, sl_target_remote_ue_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(sl_u2_u_relay_ue_cfg_r18_s::sl_target_remote_ue_to_release_list_r18),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sl_u2_u_relay_ue_cfg_r18_s_names[] = {
    {"sl-L2IdentityRelay-r18", nullptr},
    {"sl-TargetRemoteUE-ToAddModList-r18", nullptr},
    {"sl-TargetRemoteUE-ToReleaseList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_u2_u_relay_ue_cfg_r18_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_u2_u_relay_ue_cfg_r18_s::desc = {{seq_table_sl_u2_u_relay_ue_cfg_r18_s_fields,
                                                                 uint64_t{0x1},
                                                                 seq_table_sl_u2_u_relay_ue_cfg_r18_s_opts,
                                                                 seq_table_sl_u2_u_relay_ue_cfg_r18_s_names,
                                                                 3,
                                                                 2},
                                                                true,
                                                                false,
                                                                nullptr,
                                                                0};

// SL-L2RemoteUE-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_l2_remote_ue_cfg_r17_s_fields[] = {
    {offsetof(sl_l2_remote_ue_cfg_r17_s, sl_srap_cfg_remote_r17),
     (int32_t)offsetof(sl_l2_remote_ue_cfg_r17_s, sl_srap_cfg_remote_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_srap_cfg_r17_s::desc},
    {offsetof(sl_l2_remote_ue_cfg_r17_s, sl_ue_id_remote_r17),
     (int32_t)offsetof(sl_l2_remote_ue_cfg_r17_s, sl_ue_id_remote_r17_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_sl_l2_remote_ue_cfg_r17_s_names[] = {
    {"sl-SRAP-ConfigRemote-r17", nullptr},
    {"sl-UEIdentityRemote-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_l2_remote_ue_cfg_r17_s_opts[] = {
    {0, (int32_t)offsetof(sl_l2_remote_ue_cfg_r17_s, sl_srap_cfg_remote_r17_present)},
    {1, (int32_t)offsetof(sl_l2_remote_ue_cfg_r17_s, sl_ue_id_remote_r17_present)}};
static const asn1::table::seq_field seq_table_sl_l2_remote_ue_cfg_r17_s_g0_fields[] = {
    {offsetof(sl_l2_remote_ue_cfg_r17_s, sl_u2_u_relay_ue_to_add_mod_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_item_list_codec<decltype(sl_l2_remote_ue_cfg_r17_s::sl_u2_u_relay_ue_to_add_mod_list_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_u2_u_relay_ue_cfg_r18_s::desc},
    {offsetof(sl_l2_remote_ue_cfg_r17_s, sl_u2_u_relay_ue_to_release_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(sl_l2_remote_ue_cfg_r17_s::sl_u2_u_relay_ue_to_release_list_r18),
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sl_l2_remote_ue_cfg_r17_s_g0_names[] = {
    {"sl-U2U-RelayUE-ToAddModList-r18", nullptr},
    {"sl-U2U-RelayUE-ToReleaseList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_l2_remote_ue_cfg_r17_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_l2_remote_ue_cfg_r17_s_groups[] = {
    {{seq_table_sl_l2_remote_ue_cfg_r17_s_g0_fields,
      uint64_t{0x0},
      seq_table_sl_l2_remote_ue_cfg_r17_s_g0_opts,
      seq_table_sl_l2_remote_ue_cfg_r17_s_g0_names,
      2,
      2},
     false}};
const asn1::table::seq_desc sl_l2_remote_ue_cfg_r17_s::desc = {{seq_table_sl_l2_remote_ue_cfg_r17_s_fields,
                                                                uint64_t{0x0},
                                                                seq_table_sl_l2_remote_ue_cfg_r17_s_opts,
                                                                seq_table_sl_l2_remote_ue_cfg_r17_s_names,
                                                                2,
                                                                2},
                                                               true,
                                                               false,
                                                               seq_table_sl_l2_remote_ue_cfg_r17_s_groups,
                                                               1};

// RRCSetup-v1700-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_setup_v1700_ies_s_fields[] = {
    {offsetof(rrc_setup_v1700_ies_s, sl_cfg_ded_nr_r17),
     (int32_t)offsetof(rrc_setup_v1700_ies_s, sl_cfg_ded_nr_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_cfg_ded_nr_r16_s::desc},
    {offsetof(rrc_setup_v1700_ies_s, sl_l2_remote_ue_cfg_r17),
     (int32_t)offsetof(rrc_setup_v1700_ies_s, sl_l2_remote_ue_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_l2_remote_ue_cfg_r17_s::desc},
    {0,
     (int32_t)offsetof(rrc_setup_v1700_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_rrc_setup_v1700_ies_s_names[] = {{"sl-ConfigDedicatedNR-r17", nullptr},
                                                                                {"sl-L2RemoteUE-Config-r17", nullptr},
                                                                                {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_rrc_setup_v1700_ies_s_opts[]  = {
    {0, (int32_t)offsetof(rrc_setup_v1700_ies_s, sl_cfg_ded_nr_r17_present)},
    {1, (int32_t)offsetof(rrc_setup_v1700_ies_s, sl_l2_remote_ue_cfg_r17_present)},
    {2, (int32_t)offsetof(rrc_setup_v1700_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_setup_v1700_ies_s::desc = {{seq_table_rrc_setup_v1700_ies_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_rrc_setup_v1700_ies_s_opts,
                                                            seq_table_rrc_setup_v1700_ies_s_names,
                                                            3,
                                                            3},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// RRCSetup-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_setup_ies_s_fields[] = {
    {offsetof(rrc_setup_ies_s, radio_bearer_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &radio_bearer_cfg_s::desc},
    {offsetof(rrc_setup_ies_s, master_cell_group),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_setup_ies_s::master_cell_group), asn1::table::string_json>::value},
    {offsetof(rrc_setup_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_setup_ies_s::late_non_crit_ext), asn1::table::string_json>::value},
    {offsetof(rrc_setup_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_setup_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_setup_v1700_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_setup_ies_s_names[] = {{"radioBearerConfig", nullptr},
                                                                          {"masterCellGroup", nullptr},
                                                                          {"lateNonCriticalExtension", nullptr},
                                                                          {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_rrc_setup_ies_s_opts[]  = {
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(rrc_setup_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_setup_ies_s::desc = {{seq_table_rrc_setup_ies_s_fields,
                                                      uint64_t{0x3},
                                                      seq_table_rrc_setup_ies_s_opts,
                                                      seq_table_rrc_setup_ies_s_names,
                                                      4,
                                                      2},
                                                     false,
                                                     false,
                                                     nullptr,
                                                     0};

// RRCSetup ::= SEQUENCE
static const asn1::table::seq_field  seq_table_rrc_setup_s_fields[] = {{offsetof(rrc_setup_s, rrc_transaction_id),
                                                                        asn1::table::seq_field::mandatory,
                                                                        asn1::table::field_op::int_u8,
                                                                        false,
                                                                        false,
                                                                        int64_t{0},
                                                                        uint64_t{3},
                                                                        nullptr},
                                                                       {offsetof(rrc_setup_s, crit_exts),
                                                                        asn1::table::seq_field::mandatory,
                                                                        asn1::table::field_op::choice,
                                                                        false,
                                                                        false,
                                                                        0,
                                                                        0,
                                                                        nullptr,
                                                                        &rrc_setup_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_rrc_setup_s_names[]  = {{"rrc-TransactionIdentifier", nullptr},
                                                                       {"criticalExtensions", nullptr}};
const asn1::table::seq_desc          rrc_setup_s::desc              = {
    {seq_table_rrc_setup_s_fields, uint64_t{0x3}, nullptr, seq_table_rrc_setup_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

void rrc_setup_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_setup_ies_s& rrc_setup_s::crit_exts_c_::set_rrc_setup()
{
  set(types::rrc_setup);
  return c;
}
void rrc_setup_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_rrc_setup_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_setup_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_rrc_setup_s_crit_exts_c__names[] = {
    {"rrcSetup", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc rrc_setup_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<rrc_setup_s::crit_exts_c_::types>(),
    rrc_setup_s::crit_exts_c_::types::nof_types,
    rrc_setup_s::crit_exts_c_::types::nof_exts,
    rrc_setup_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_rrc_setup_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<rrc_setup_s::crit_exts_c_>,
    choice_table_rrc_setup_s_crit_exts_c__names,
    "rrc_setup_s::crit_exts_c_",
    offsetof(rrc_setup_s::crit_exts_c_, type_),
    offsetof(rrc_setup_s::crit_exts_c_, c),
    false};

static const char* enum_names_rrc_setup_s_crit_exts_c__types_values[]    = {"rrcSetup", "criticalExtensionsFuture"};
const asn1::table::enum_desc rrc_setup_s::crit_exts_c_::types_opts::desc = {
    enum_names_rrc_setup_s_crit_exts_c__types_values,
    2,
    "rrc_setup_s::crit_exts_c_::types"};
