#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include "gr_hdlc_deframer_b_impl.h"
#include <gnuradio/blocks/pdu.h>
#include <gnuradio/io_signature.h>

// win32 (mingw/msvc) specific
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef O_BINARY
#define	OUR_O_BINARY O_BINARY
#else
#define	OUR_O_BINARY 0
#endif

// should be handled via configure
#ifdef O_LARGEFILE
#define	OUR_O_LARGEFILE	O_LARGEFILE
#else
#define	OUR_O_LARGEFILE 0
#endif

namespace gr {
    namespace framers {
        
        gr_hdlc_deframer_b::sptr
        gr_hdlc_deframer_b::make(int dlci)
        {
            return gnuradio::get_initial_sptr
                (new gr_hdlc_deframer_b_impl(dlci));
        }
        
        /*
         * The private constructor
         */
        gr_hdlc_deframer_b_impl::gr_hdlc_deframer_b_impl(int dlci)
            : gr::block("gr_hdlc_deframer_b",
                        gr::io_signature::make(1, 1, sizeof(char)),
                        gr::io_signature::make(0, 0, 0)),
              d_dlci(dlci),
              d_deframer(dlci),
              total_packets(0)
        {
            message_port_register_out(pmt::mp("pdu"));
        }
        
        /*
         * Our virtual destructor.
         */
        gr_hdlc_deframer_b_impl::~gr_hdlc_deframer_b_impl()
        {
        }
        
        void
        gr_hdlc_deframer_b_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
        {
            ninput_items_required[0] = noutput_items;            
        }
        
        int
    gr_hdlc_deframer_b_impl::general_work (int noutput_items,
                                           gr_vector_int &ninput_items,
                                           gr_vector_const_void_star &input_items,
                                           gr_vector_void_star &output_items)
        {
            if (ninput_items[0]!=noutput_items){
  				GR_LOG_INFO(d_logger, (boost::format("nread: %d,nreadDEBUG: deframer general_work: ninput_items: \t%d \tnoutput_items: %d") % ninput_items[0] % noutput_items).str() );
            }
            int nread=0;
            const uint8_t *inbuf = (const uint8_t *) input_items[0];
            for(int i=0; i<ninput_items[0]; i++)
            {
                d_deframer.hdlc_state_machine(inbuf[i] & 0x01);  // Low order bit
                nread++;
            }
            int num_packets=0;
            while(!d_deframer.empty()){
                vector<unsigned char> packet=d_deframer.get_packet();
                route_packet(packet.size(),&packet[0]);
                num_packets++;
            }
            if (num_packets>0){
  				//GR_LOG_INFO(d_logger, (boost::format("num_packets: %d") % num_packets).str() );
  				//GR_LOG_INFO(d_logger, (boost::format("nread: %d") % nread).str() );
            }
            consume_each (nread);
            //printf("\nxxxxxxxxxxxxxxxxTotal Packets pushed(%d) = %d\n",d_dlci,total_packets);
            
            return 0;
            
        }

        // ****************
        // Private Methods
        // ****************

        void gr_hdlc_deframer_b_impl::route_packet(const int             hdlc_frame_size, 
                                                   const unsigned char * hdlc_frame){
            blocks::pdu::vector_type vtype=blocks::pdu::byte_t;
            pmt::pmt_t pdu_vector=blocks::pdu::make_pdu_vector(vtype,hdlc_frame,hdlc_frame_size);
            
            pmt::pmt_t msg=pmt::cons(pmt::PMT_NIL, pdu_vector);
            message_port_pub(pmt::mp("pdu"),msg);
            
            total_packets++;
            
        }

        /*
         * Public Methods
         */

        /* Bit error rate */
        float gr_hdlc_deframer_b_impl::get_ber()
        {
            return d_deframer.d_ber;
        }

        /* Frame error rate */
        float gr_hdlc_deframer_b_impl::get_fer()
        {
            return d_deframer.d_fer;
        }

        /* Total number of frames so far */
        int gr_hdlc_deframer_b_impl::get_n_frames()
        {
            return d_deframer.d_n_frames;
        }
    } /* namespace framers */
} /* namespace gr */

