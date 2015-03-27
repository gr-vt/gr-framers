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
from test_messages_001 import test_messages_001
import framers_swig as framers
import pmt
class qa_gr_hdlc_framer_b (gr_unittest.TestCase):

    def setUp (self):
        self.samp_rate=samp_rate=50
        self.packet_size=packet_size=10
        self.tb = gr.top_block ()
        self.src_vector=(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20)
        self.src0 = blocks.vector_source_b(self.src_vector, True, 1, [])
        self.head_0=blocks.head(1,2000)        
        self.throttle_0 = blocks.throttle(gr.sizeof_char*1, samp_rate)
        self.test=test_messages_001()
        self.sink0 = blocks.vector_sink_b(1)
        self.sink1 = blocks.vector_sink_b(1)

        self.tb.connect(self.src0,self.throttle_0)
        self.tb.connect(self.throttle_0,self.head_0)
        self.tb.connect(self.head_0,self.test,self.sink1)
        #self.tb.connect(self.head_0,self.sink1)
        self.tb.connect(self.head_0,self.sink0)
    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.start ()
        sleep(2)
        self.tb.stop()
        self.tb.wait()
        # check data
        src_data=self.sink0.data()
        result_data=self.sink1.data()
        print len(src_data),len(result_data)
        res1=src_data[0:min(len(src_data),len(result_data))]
        res2=result_data[0:min(len(src_data),len(result_data))]
        
        self.assertFloatTuplesAlmostEqual(res1, res2, 6)
if __name__ == '__main__':
    gr_unittest.run(qa_gr_hdlc_framer_b, "qa_gr_hdlc_framer_b.xml")
    
