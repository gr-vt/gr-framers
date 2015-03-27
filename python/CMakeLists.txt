# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

########################################################################
# Install python sources
########################################################################
GR_PYTHON_INSTALL(
    FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/framers
)
set(GR_REQUIRED_COMPONENTS RUNTIME BLOCKS PMT)
find_package(Gnuradio "3.7")

find_package(Grcc)

if(GRCC_FOUND)
  message(STATUS "GRCC FOUND: ${GRCC_EXECUTABLE}")
else(GRCC_FOUND)
  message(STATUS "GRCC NOT FOUND")
endif(GRCC_FOUND)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-framers)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig $ENV{HOME}/.grc_gnuradio)
message(STATUS "grcc -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/test_framers.grc")

#add_custom_target(test_framers_heir ALL
GR_ADD_TEST(setup_tests export PYTHONPATH="${CMAKE_INSTALL_PREFIX}/python/:${CMAKE_INSTALL_PREFIX}/lib/python2.6/site-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.6/site-packages/:${CMAKE_INSTALL_PREFIX}/lib/python2.6/dist-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.6/dist-packages/:${CMAKE_INSTALL_PREFIX}/lib/python2.7/site-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.7/site-packages/:${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.7/dist-packages/:${CMAKE_INSTALL_PREFIX}/python/:${CMAKE_INSTALL_PREFIX}/lib/python2.6/site-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.6/site-packages/:${CMAKE_INSTALL_PREFIX}/lib/python2.6/dist-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.6/dist-packages/:${CMAKE_INSTALL_PREFIX}/lib/python2.7/site-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.7/site-packages/:${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/:${CMAKE_INSTALL_PREFIX}/lib64/python2.7/dist-packages/:" && export LD_LIBRARY_PATH="${CMAKE_INSTALL_PREFIX}/lib/:${CMAKE_INSTALL_PREFIX}/lib64/:" && ${GRCC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_framers_001.grc && ${GRCC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_framers_002.grc && ${GRCC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_messages_001.grc
) 
#add_custom_target(test_messages_heir ALL
#  COMMAND ${GRCC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_messages.grc
#) 
GR_ADD_TEST(qa_gr_messages ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_messages.py)
GR_ADD_TEST(qa_gr_hdlc_framer_b ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_hdlc_framer_b.py)
GR_ADD_TEST(qa_gr_hdlc_deframer_b ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_hdlc_deframer_b.py)
