// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/ranac.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/ran/tac.h"
#include <optional>
#include <variant>
#include <vector>

namespace ocudu::ocucp {

struct cu_cp_tx_bw {
  subcarrier_spacing nr_scs;
  uint16_t           nr_nrb;

  bool operator==(const cu_cp_tx_bw& rhs) const { return nr_scs == rhs.nr_scs and nr_nrb == rhs.nr_nrb; }
  bool operator!=(const cu_cp_tx_bw& rhs) const { return !(rhs == *this); }
};

struct cu_cp_sul_info {
  uint32_t    sul_nr_arfcn;
  cu_cp_tx_bw sul_tx_bw;

  bool operator==(const cu_cp_sul_info& rhs) const
  {
    return sul_nr_arfcn == rhs.sul_nr_arfcn and sul_tx_bw == rhs.sul_tx_bw;
  }
  bool operator!=(const cu_cp_sul_info& rhs) const { return !(rhs == *this); }
};

struct cu_cp_supported_sul_freq_band_item {
  uint16_t freq_band_ind_nr;

  bool operator==(const cu_cp_supported_sul_freq_band_item& rhs) const
  {
    return freq_band_ind_nr == rhs.freq_band_ind_nr;
  }
  bool operator!=(const cu_cp_supported_sul_freq_band_item& rhs) const { return !(rhs == *this); }
};

struct cu_cp_freq_band_nr_item {
  uint16_t                                        freq_band_ind_nr;
  std::vector<cu_cp_supported_sul_freq_band_item> supported_sul_band_list;

  bool operator==(const cu_cp_freq_band_nr_item& rhs) const
  {
    return freq_band_ind_nr == rhs.freq_band_ind_nr and supported_sul_band_list == rhs.supported_sul_band_list;
  }
  bool operator!=(const cu_cp_freq_band_nr_item& rhs) const { return !(rhs == *this); }
};

struct cu_cp_nr_freq_info {
  uint32_t                             nr_arfcn;
  std::optional<cu_cp_sul_info>        sul_info;
  std::vector<cu_cp_freq_band_nr_item> freq_band_list_nr;

  bool operator==(const cu_cp_nr_freq_info& rhs) const
  {
    return nr_arfcn == rhs.nr_arfcn and sul_info == rhs.sul_info and freq_band_list_nr == rhs.freq_band_list_nr;
  }
  bool operator!=(const cu_cp_nr_freq_info& rhs) const { return !(rhs == *this); }
};

struct cu_cp_fdd_info {
  cu_cp_nr_freq_info ul_nr_freq_info;
  cu_cp_nr_freq_info dl_nr_freq_info;
  cu_cp_tx_bw        ul_tx_bw;
  cu_cp_tx_bw        dl_tx_bw;

  bool operator==(const cu_cp_fdd_info& rhs) const
  {
    return ul_nr_freq_info == rhs.ul_nr_freq_info and dl_nr_freq_info == rhs.dl_nr_freq_info and
           ul_tx_bw == rhs.ul_tx_bw and dl_tx_bw == rhs.dl_tx_bw;
  }
  bool operator!=(const cu_cp_fdd_info& rhs) const { return !(rhs == *this); }
};

struct cu_cp_tdd_info {
  cu_cp_nr_freq_info nr_freq_info;
  cu_cp_tx_bw        tx_bw;

  bool operator==(const cu_cp_tdd_info& rhs) const { return nr_freq_info == rhs.nr_freq_info and tx_bw == rhs.tx_bw; }
  bool operator!=(const cu_cp_tdd_info& rhs) const { return !(rhs == *this); }
};

using cu_cp_nr_mode_info = std::variant<cu_cp_fdd_info, cu_cp_tdd_info>;

struct cu_cp_served_cell_info {
  nr_cell_global_id_t        nr_cgi;
  pci_t                      nr_pci;
  std::optional<tac_t>       five_gs_tac;
  std::vector<plmn_identity> served_plmns;
  cu_cp_nr_mode_info         nr_mode_info;
  byte_buffer                meas_timing_cfg;
  std::optional<ranac_t>     ranac;

  cu_cp_served_cell_info() = default;
  cu_cp_served_cell_info(const cu_cp_served_cell_info& other) :
    nr_cgi(other.nr_cgi),
    nr_pci(other.nr_pci),
    five_gs_tac(other.five_gs_tac),
    served_plmns(other.served_plmns),
    nr_mode_info(other.nr_mode_info),
    meas_timing_cfg(other.meas_timing_cfg.copy()),
    ranac(other.ranac)
  {
  }
  cu_cp_served_cell_info& operator=(const cu_cp_served_cell_info& other)
  {
    if (this != &other) {
      nr_cgi          = other.nr_cgi;
      nr_pci          = other.nr_pci;
      five_gs_tac     = other.five_gs_tac;
      served_plmns    = other.served_plmns;
      nr_mode_info    = other.nr_mode_info;
      meas_timing_cfg = other.meas_timing_cfg.copy();
      ranac           = other.ranac;
    }
    return *this;
  }

  bool operator==(const cu_cp_served_cell_info& rhs) const
  {
    return nr_cgi == rhs.nr_cgi and nr_pci == rhs.nr_pci and five_gs_tac == rhs.five_gs_tac and
           served_plmns == rhs.served_plmns and nr_mode_info == rhs.nr_mode_info and
           meas_timing_cfg == rhs.meas_timing_cfg and ranac == rhs.ranac;
  }
  bool operator!=(const cu_cp_served_cell_info& rhs) const { return !(rhs == *this); }
};

struct cu_cp_gnb_du_sys_info {
  byte_buffer mib_msg;
  byte_buffer sib1_msg;

  cu_cp_gnb_du_sys_info() = default;
  cu_cp_gnb_du_sys_info(const cu_cp_gnb_du_sys_info& other) : mib_msg(other.mib_msg), sib1_msg(other.sib1_msg) {}
  cu_cp_gnb_du_sys_info& operator=(const cu_cp_gnb_du_sys_info& other)
  {
    if (this != &other) {
      mib_msg  = other.mib_msg.copy();
      sib1_msg = other.sib1_msg.copy();
    }
    return *this;
  }
};

struct cu_cp_du_served_cells_item {
  cu_cp_served_cell_info               served_cell_info;
  std::optional<cu_cp_gnb_du_sys_info> gnb_du_sys_info; // not optional for NG-RAN
};

} // namespace ocudu::ocucp
