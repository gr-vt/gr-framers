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

include(GrPython)
enable_testing()

add_executable(test_hdlc_framer_lib test_hdlc_framer_lib.cc ../lib/hdlc_framer.cc ../lib/hdlc_deframer.cc)
add_test(hdlc_framer_lib_test  test_hdlc_framer_lib)
GR_PYTHON_INSTALL(
    PROGRAMS
    DESTINATION bin
)
