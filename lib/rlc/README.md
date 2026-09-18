# Radio Link Control (RLC)

RLC layer connects the MAC and the F1AP/PDCP, as shown in the figure below, and provides that transfer services to these
layers.

![image](images/rlc_stack.svg)

It does this, by providing three different modes:

## Transparent Mode (TM)

TM is used only in control signaling (SRB0 only) and provides data transfer services without modifying the SDUs/PDUs at
all.

## Unacknowledged Mode (UM)

UM can only be used in data traffic (DRBs only) and provides data transfer services with segmentation/reassembly. It is
usually used in delay-sensitive and loss-tolerant traffic, as it does not provide re-transmissions.

## Acknowledged Mode (AM)

AM can be used in data and control traffic (mandatory for SRBs, optional for DRBs) and provides data transfer services
with segmentation and ARQ procedures. This mode is usually used for traffic that is more loss-sensitive, but more
delay-tolerant.

More details about our implementation can be found [here](rlc_am.md).
