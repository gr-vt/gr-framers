#ifndef INCLUDED_FRAMERS_GR_HDLC_DEFRAMER_B_IMPL_H
#define INCLUDED_FRAMERS_GR_HDLC_DEFRAMER_B_IMPL_H

#include <hdlc_deframer.h>
#include <framers/gr_hdlc_deframer_b.h>

namespace gr {
    namespace framers {
        
        class gr_hdlc_deframer_b_impl : public gr_hdlc_deframer_b
        {

        private:
            int total_packets;
            // Private Attributes ---------
            hdlc_deframer d_deframer;
            // Frame Relay's Data Link Channel Indicator
            int            d_dlci; 
            
            // Private Methods ----------
            
            void route_packet(const int             hdlc_frame_size, 
                              const unsigned char * hdlc_frame);
 
        protected:
            
        public:
            gr_hdlc_deframer_b_impl(int dlci);
            ~gr_hdlc_deframer_b_impl();
            
            // Where all the action really happens
            void forecast (int noutput_items, gr_vector_int &ninput_items_required);
            
            int general_work(int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items);

            // Getters for public statistics
            float get_ber();
            float get_fer();
            int get_n_frames();
        };
        
    } // namespace framers
} // namespace gr

#endif /* INCLUDED_FRAMERS_GR_HDLC_DEFRAMER_B_IMPL_H */

