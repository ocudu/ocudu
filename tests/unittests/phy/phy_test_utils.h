// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

// Constructs a factory of spy components.
#define PHY_SPY_FACTORY_TEMPLATE(COMPONENT)                                                                            \
  class COMPONENT##_factory_spy : public COMPONENT##_factory                                                           \
  {                                                                                                                    \
  public:                                                                                                              \
    std::unique_ptr<COMPONENT> create() override                                                                       \
    {                                                                                                                  \
      std::unique_ptr<COMPONENT##_spy> spy = std::make_unique<COMPONENT##_spy>();                                      \
      entries.push_back(spy.get());                                                                                    \
      return spy;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    std::vector<COMPONENT##_spy*>& get_entries() { return entries; }                                                   \
                                                                                                                       \
  private:                                                                                                             \
    std::vector<COMPONENT##_spy*> entries;                                                                             \
  }
