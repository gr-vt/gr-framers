/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "gr_hdlc_framer_2_channel_b_impl.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef DEBUG
#define DEBUG
#endif

#ifdef DEBUG
#define DEBUG0(s)       fprintf(stderr, s); fflush(stderr)
#define DEBUG1(s,a1)    fprintf(stderr, s, a1); fflush(stderr)
#define DEBUG2(s,a1,a2) fprintf(stderr, s, a1, a2); fflush(stderr)
#else
#define DEBUG0(s)       
#define DEBUG1(s,a1)    
#define DEBUG2(s,a1,a2) 
#endif

namespace gr {
  namespace framers {

    gr_hdlc_framer_2_channel_b::sptr
    gr_hdlc_framer_2_channel_b::make(int dlci_0, int dlci_1)
    {
      return gnuradio::get_initial_sptr
        (new gr_hdlc_framer_2_channel_b_impl(dlci_0, dlci_1));
    }

    /*
     * The private constructor
     */
    gr_hdlc_framer_2_channel_b_impl::gr_hdlc_framer_2_channel_b_impl(int dlci_0, int dlci_1)
      : gr::block("gr_hdlc_framer_2_channel_b",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(char))),
        d_dlci0(dlci_0),
        d_framer0(dlci_0),
        d_dlci1(dlci_1),
        d_framer1(dlci_1),
        total_packets0(0),
        total_packets1(0)
    {
      message_port_register_in(pmt::mp("in0"));
      d_framer0.push_flag();
      message_port_register_in(pmt::mp("in1"));
      d_framer1.push_flag();
    }

    /*
     * Our virtual destructor.
     */
    gr_hdlc_framer_2_channel_b_impl::~gr_hdlc_framer_2_channel_b_impl()
    {
    }

    void
    gr_hdlc_framer_2_channel_b_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = 0;
    }

    // Read packet from pmt
    int gr_hdlc_framer_2_channel_b_impl::read_packet0(unsigned char * packet_buf)
    {
        pmt::pmt_t msg(delete_head_nowait(pmt::mp("in0")));
        if (msg.get()==NULL){
            // Didn't do anything
            return 0;
        }
        //printf("got a packet\n");
        pmt::pmt_t vect(pmt::cdr(msg));
        size_t ncopy = (size_t)pmt::length(vect);
        memcpy(packet_buf,uniform_vector_elements(vect,ncopy),ncopy);
        return ncopy;
    }
    int gr_hdlc_framer_2_channel_b_impl::read_packet1(unsigned char * packet_buf)
    {
        pmt::pmt_t msg(delete_head_nowait(pmt::mp("in1")));
        if (msg.get()==NULL){
            // Didn't do anything
            return 0;
        }
        //printf("got a packet\n");
        pmt::pmt_t vect(pmt::cdr(msg));
        size_t ncopy = (size_t)pmt::length(vect);
        memcpy(packet_buf,uniform_vector_elements(vect,ncopy),ncopy);
        return ncopy;
    }

    int
    gr_hdlc_framer_2_channel_b_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        uint8_t *out = (uint8_t *) output_items[0];
        int i=0;

        int num_processed=0;

        // Loop until the requested number of output stream bytes have been generated
	      d_framer0.push_flag();
	      d_framer1.push_flag();
        
        while(i<noutput_items && i<999){
          // If we need more data
          if (d_fifo.empty()){

        //****************************USER 0*************************
            // Try to read more data from async input
            uint8_t packet_buf[FRAME_MAX];
            int packet_length0=read_packet0(packet_buf);
		    
            if (packet_length0>0){ // If we get some, then encapsulate it
		          d_framer0.encapsulate_incoming_packet(packet_buf,packet_length0);
		          total_packets0++;
            } else { // If not, then just push a flag byte
		          d_framer0.push_flag();
            }
            // Dump the data from the framer
            vector<uint8_t> framed_bytes0=d_framer0.dump_buffer();

		        for (int j=0;j<framed_bytes0.size();j++){
              d_fifo.push(framed_bytes0[j]);
            }

        //****************************USER 1*************************
            // Try to read more data from async input
            //uint8_t packet_buf[FRAME_MAX];
            int packet_length1=read_packet1(packet_buf);
		    
            if (packet_length1>0){ // If we get some, then encapsulate it
		          d_framer1.encapsulate_incoming_packet(packet_buf,packet_length1);
		          total_packets1++;
            } else { // If not, then just push a flag byte
		          d_framer1.push_flag();
            }
            // Dump the data from the framer
            vector<uint8_t> framed_bytes1=d_framer1.dump_buffer();

		        for (int j=0;j<framed_bytes1.size();j++){
              d_fifo.push(framed_bytes1[j]);
            }
          }

          out[i] = d_fifo.front();
          d_fifo.pop();
          i++;
          num_processed++;
        }

        // Tell runtime system how many output items we produced.
        return num_processed;
    }

  } /* namespace framers */
} /* namespace gr */

