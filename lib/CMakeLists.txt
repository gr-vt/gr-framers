# Copyright 2011,2012,2016 Free Software Foundation, Inc.
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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND framers_sources
    gr_hdlc_framer_b_impl.cc
    gr_hdlc_deframer_b_impl.cc
    hdlc_framer.cc
    hdlc_deframer.cc
    gr_hdlc_framer_2_channel_b_impl.cc )

set(framers_sources "${framers_sources}" PARENT_SCOPE)
if(NOT framers_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT framers_sources)

add_library(gnuradio-framers SHARED ${framers_sources})
message(STATUS "GNURADIO_PMT_LIBRARIES: ${GNURADIO_PMT_LIBRARIES}")
target_link_libraries(gnuradio-framers ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-framers PROPERTIES DEFINE_SYMBOL "gnuradio_framers_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-framers PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-framers RUNTIME_COMPONENT "framers_runtime" DEVEL_COMPONENT "framers_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_framers_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_framers.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_framers.cc
)

add_executable(test-framers ${test_framers_sources})

target_link_libraries(
  test-framers
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-framers
)

GR_ADD_TEST(test_framers test-framers)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

