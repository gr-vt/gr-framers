#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 Free Software Foundation, Inc
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import numpy
from time import sleep

from message_file import msg_vector_sink
import framers_swig as framers
import pmt
from test_framers_001 import test_framers_001
from test_framers_002 import test_framers_002
class qa_gr_hdlc_framer_b (gr_unittest.TestCase):

    def setUp (self):
        pass

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # This test makes sure that the framer outputs only flags when it has no inputs
        # Create Blocks
        return #HACK
        samp_rate=10000
        packet_size=packet_size=10
        tb = gr.top_block ()
        test=test_framers_001()
        test.set_samp_rate(samp_rate)

        snk0 = blocks.vector_sink_b(1)

        tb.connect(test,snk0)


        # set up fg
        tb.start ()
        sleep(1)
        tb.stop()
        tb.wait()
        # check data
        
        result_data=snk0.data()
        if len(result_data)!=1000:
            raise(Exception("Framer flag test should have 1000 flags"))
        for i in range(1000):
            if (result_data[i]!=0x7e):
                raise(Exception("Framer flag test does not pass: result_data["+str(i)+"]="+hex(int(result_data[i]))+" != 0x7e"))

    def test_002_t (self):
        # This test makes sure that the framer/deframer pairing outputs what gets input
        # Create Blocks
        samp_rate=100000
        vector=(0,1,2,3,4,5,6,5,4,3,2,1)
        packet_size=len(vector)
        tb = gr.top_block ()
        test=test_framers_002()
        test.set_samp_rate(samp_rate)
        test.set_vector(vector)
        sink=msg_vector_sink(packet_size)
        tb.msg_connect(test,"out",sink,"print_pdu")

        # set up fg
        tb.start ()
        sleep(2)
        tb.stop()
        tb.wait()
        # check data
        
        result_data=sink.data()
        for i in range(len(result_data)):
            if result_data[i]!=vector[i%len(vector)]:
                raise(Exception("qa_gr_hdlc_framer_b test_002_t failed because result_data doesn't match vector"))
        print result_data

if __name__ == '__main__':
    gr_unittest.run(qa_gr_hdlc_framer_b, "qa_gr_hdlc_framer_b.xml")
    
