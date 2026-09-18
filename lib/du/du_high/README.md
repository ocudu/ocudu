# DU-high

![image](du_high.png)

The DU-high, or Distributed Unit - High, is responsible for the handling of both uplink and downlink traffic.
Specifically the MAC and RLC processing of these signals. The DU-high, has three main components and three main
interfaces. The DU-high communicates directly with the CU-CP and CU-UP via the F1-c and F1-u interfaces, the DU-low via
the FAPI interface, and can also be connected to the near-RT RIC via the E2 interface.

[Return to top level architecture diagram](../../README.md).

## Components

- [DU Manager](du_manager/README.md): The DU Manager is responsible for managing the DU, specifically the connected UEs
  and RAN Resources.
- [RLC](../../rlc/README.md): The Radio Link Control (RLC) layer sits between the MAC in the DU-high and the F1AP/PDCP
  in the CU, and provides the transport services for SRBs and PRBs between these layers. It operates in three modes:
  transparent, unacknowledged and acknowledged.
- [MAC](../../mac/README.md): The Medium Access Control (MAC) is responsible for the encoding and decoding of MAC PDUs,
  scheduling uplink and downlink grants and other control services.

## Interfaces

- E2: Interfaces with the nRT RIC.
- F1: Interfaces with the CU control and user plane.
- FAPI: Interface between the DU-high and DU-low.
