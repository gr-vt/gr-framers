#ifndef INCLUDED_FRAMERS_GR_HDLC_FRAMER_B_IMPL_H
#define INCLUDED_FRAMERS_GR_HDLC_FRAMER_B_IMPL_H

#include <queue>


#include <framers/gr_hdlc_framer_b.h>
#include <hdlc_framer.h>

using std::queue;
namespace gr {
    namespace framers {

        class gr_hdlc_framer_b_impl : public gr_hdlc_framer_b
        {
        private:
            // This does most of the framing work
            hdlc_framer d_framer;
            int d_dlci;
            queue<unsigned char> d_fifo;
            int read_packet(unsigned char * packet_buf);
	  int total_packets;
        public:
            gr_hdlc_framer_b_impl(int dlci);
            ~gr_hdlc_framer_b_impl();
            
            // Where all the action really happens
            void forecast (int noutput_items, gr_vector_int &ninput_items_required);
            
            int general_work(int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items);
        };
        
    } // namespace framers
} // namespace gr

#endif /* INCLUDED_FRAMERS_GR_HDLC_FRAMER_B_IMPL_H */

