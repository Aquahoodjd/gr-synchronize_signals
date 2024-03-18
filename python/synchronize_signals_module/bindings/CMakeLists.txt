# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT synchronize_signals_module_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT synchronize_signals_module_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND synchronize_signals_module_python_files
    synchronize_signals_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(synchronize_signals_module
   ../../..
   gr::synchronize_signals_module
   "${synchronize_signals_module_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(TARGET synchronize_signals_module_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:synchronize_signals_module_python>
    ${PROJECT_BINARY_DIR}/test_modules/gnuradio/synchronize_signals_module/
)

install(TARGETS synchronize_signals_module_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/synchronize_signals_module COMPONENT pythonapi)
