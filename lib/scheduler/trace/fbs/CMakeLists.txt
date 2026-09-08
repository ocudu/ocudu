# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

include(FetchContent)
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_INSTALL OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "" FORCE)
# FLATBUFFERS_VERSION is set next to the SBOM entry in the top-level CMakeLists.txt.
FetchContent_Declare(flatbuffers
  GIT_REPOSITORY https://github.com/google/flatbuffers.git
  GIT_TAG v${FLATBUFFERS_VERSION}
  GIT_SHALLOW TRUE)
# Suppress warnings when compiling flatbuffers itself; the project's -Werror flags are not its concern.
set(SAVED_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# flatc is a host tool that the build has to execute to generate the headers, so don't inherit the target -march.
# That way we support running the builder in a different machine.
string(REGEX REPLACE "-m(arch|tune|cpu)=[^ ]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
FetchContent_MakeAvailable(flatbuffers)
set(CMAKE_CXX_FLAGS "${SAVED_CXX_FLAGS}")

set(FBS_SCHEMAS
  common.fbs
  cell_start_event.fbs
  cell_stop_event.fbs
  slot_input.fbs
  slot_decision.fbs
  cell_slot_event.fbs
  cell_event.fbs)

set(FBS_GEN_HEADERS "")
set(FBS_SCHEMA_PATHS "")
foreach (schema ${FBS_SCHEMAS})
  get_filename_component(name ${schema} NAME_WE)
  list(APPEND FBS_GEN_HEADERS ${FBS_OUT_DIR}/${name}_generated.h)
  list(APPEND FBS_SCHEMA_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/${schema})
endforeach ()

# Note: not using the upstream flatbuffers_generate_headers() helper because its per-schema custom
# commands only depend on the schema file itself, not on the schemas it includes. Editing e.g.
# common.fbs would leave cell_start_event_generated.h stale on incremental builds. This single
# command regenerates all headers whenever any schema changes.
add_custom_command(
  OUTPUT ${FBS_GEN_HEADERS}
  COMMAND $<TARGET_FILE:flatc> --cpp --scoped-enums --cpp-std c++17
          -I ${CMAKE_CURRENT_SOURCE_DIR} -o ${FBS_OUT_DIR} ${FBS_SCHEMA_PATHS}
  DEPENDS flatc ${FBS_SCHEMA_PATHS}
  COMMENT "Generating FlatBuffers headers for schedtrace schemas")

add_custom_target(schedtrace_fbs_gen DEPENDS ${FBS_GEN_HEADERS})

# Headers are included as "fbs/<name>_generated.h", so expose the parent of FBS_OUT_DIR.
get_filename_component(FBS_INCLUDE_DIR ${FBS_OUT_DIR} DIRECTORY)

add_library(schedtrace_fbs INTERFACE)
add_dependencies(schedtrace_fbs schedtrace_fbs_gen)
target_include_directories(schedtrace_fbs INTERFACE ${FBS_INCLUDE_DIR})
target_link_libraries(schedtrace_fbs INTERFACE flatbuffers)
