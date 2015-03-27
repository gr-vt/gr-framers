/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_FRAMERS_GR_HDLC_FRAMER_2_CHANNEL_B_IMPL_H
#define INCLUDED_FRAMERS_GR_HDLC_FRAMER_2_CHANNEL_B_IMPL_H

#include <queue>

#include <framers/gr_hdlc_framer_2_channel_b.h>
#include <hdlc_framer.h>

using std::queue;
namespace gr {
    namespace framers {

        class gr_hdlc_framer_2_channel_b_impl : public gr_hdlc_framer_2_channel_b
        {
        private:
            // This does most of the framing work
            hdlc_framer d_framer0;
            int d_dlci0;
            hdlc_framer d_framer1;
            int d_dlci1;
            queue<unsigned char> d_fifo;
            int read_packet0(unsigned char * packet_buf);
            int read_packet1(unsigned char * packet_buf);
            int total_packets0;
            int total_packets1;
        public:
            gr_hdlc_framer_2_channel_b_impl(int dlci_0, int dlci_1);
            ~gr_hdlc_framer_2_channel_b_impl();
            
            // Where all the action really happens
            void forecast (int noutput_items, gr_vector_int &ninput_items_required);
            
            int general_work(int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items);
        };
        
    } // namespace framers
} // namespace gr

#endif /* INCLUDED_FRAMERS_GR_HDLC_FRAMER_2_CHANNEL_B_IMPL_H */

