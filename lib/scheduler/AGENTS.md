# Scheduler

Read a page of this list only when your task needs it.

- [README.md](README.md) holds the architecture of the scheduler: the component hierarchy, the slot processing flow, the
  event handling, the resource grid, the RAN slicing, the UE context and the directory layout. Read it when you need to
  know where new code belongs, or in which order a slot runs.
- [config_refactor.md](config_refactor.md) holds the rules for the configuration code of the cells and the UEs. A long
  refactor changes that code step by step, thus the current code is not always a good example. Read the page before you
  add or change a configuration parameter, a configuration structure or RRM code. It tells you which structure holds a
  new parameter, and which structures the scheduler does not read.
- [log_reference.md](log_reference.md) holds the format of each scheduler log line and field.
