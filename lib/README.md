# OCUDU Software Architecture Documentation

A primer on the O-RAN gNB architecture has already been outlined in the Knowledge Base, this can be found
[here](https://docs.ocudu.org).

This documentation aims to outline how this architecture is implemented in the OCUDU codebase. The function and
implementation of each component will be discussed in subsequent sections.

## Overview and Components

The code implements all of the components and interfaces in the below diagram. All of these elements have been
implemented in software and are fully performant, customizable and compliant with the O-RAN standard. Users can also
integrate 3rd-party RICs, RUs, and gNB components with OCUDU components.

- [CU-CP](cu_cp/README.md)
- [CU-UP](cu_up/README.md)
- [DU](du/README.md)

![image](oran_gnb_arch.jpg)
