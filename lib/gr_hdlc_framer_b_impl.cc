#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "gr_hdlc_framer_b_impl.h"
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
        
        gr_hdlc_framer_b::sptr
        gr_hdlc_framer_b::make(int dlci)
        {
            return gnuradio::get_initial_sptr
                (new gr_hdlc_framer_b_impl(dlci));
        }
        
        /*
         * The private constructor
         */
        gr_hdlc_framer_b_impl::gr_hdlc_framer_b_impl(int dlci)
            : gr::block("gr_hdlc_framer_b",
                        gr::io_signature::make(0, 0, 0),
                        gr::io_signature::make(1, 1, sizeof(char))),
              d_dlci(dlci),
              d_framer(dlci),
	      total_packets(0)
        {
            message_port_register_in(pmt::mp("in"));
            d_framer.push_flag();
        }
        
        /*
         * Our virtual destructor.
         */
        gr_hdlc_framer_b_impl::~gr_hdlc_framer_b_impl()
        {
        }
        
        void
        gr_hdlc_framer_b_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
        {
	  ninput_items_required[0]=0;
        }
        
        
        // Read packet from pmt
        
        int gr_hdlc_framer_b_impl::read_packet(unsigned char * packet_buf)
        {
            pmt::pmt_t msg(delete_head_nowait(pmt::mp("in")));
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
        gr_hdlc_framer_b_impl::general_work (int noutput_items,
                                             gr_vector_int &ninput_items,
                                             gr_vector_const_void_star &input_items,
                                             gr_vector_void_star &output_items)
        {
            unsigned char * outbuf = (unsigned char *) output_items[0];
            int             i=0;

            int num_processed=0;

            // Loop until the requested number of output stream bytes have been generated
	          d_framer.push_flag();
            while(i<noutput_items && i<999){
                // If we need more data
                if (d_fifo.empty()){
                    // Try to read more data from async input
                    unsigned char packet_buf[FRAME_MAX];
                    int packet_length=read_packet(packet_buf);
		    
                    if (packet_length>0){ // If we get some, then encapsulate it
		                  d_framer.encapsulate_incoming_packet(packet_buf,packet_length);
		                  total_packets++;
                    } else { // If not, then just push a flag byte
		                  d_framer.push_flag();
                    }
                    // Dump the data from the framer
                    vector<unsigned char> framed_bytes=d_framer.dump_buffer();

	                  for (int j=0;j<framed_bytes.size();j++){
                        d_fifo.push(framed_bytes[j]);
                    }
                }

                outbuf[i] = d_fifo.front();
                d_fifo.pop();
                i++;
                num_processed++;
          }
          //printf("\n**************Total packets processed(%d) = %d\n",d_dlci,total_packets);
            
          return num_processed;
            
        }
    } /* namespace framers */
} /* namespace gr */

