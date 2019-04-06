#include "hdlc_deframer.h"

#include <stdio.h>

#ifndef DEBUG
//#define DEBUG
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

// **********************
// Constructor and destructor
// **********************

hdlc_deframer::hdlc_deframer(int dlci) :
    d_dlci(dlci), 
    d_state(HUNT),
    d_byte(0x00),
    d_accumulated_bits(0),
    d_bit_buf_size(0),
    d_consecutive_one_bits(0),
    d_flag_cnt(0),
    d_good_frame_cnt(0),
    d_good_byte_cnt(0),
    d_good_dlci_cnt(0),
    d_crc_err_cnt(0),
    d_err_byte_cnt(0),
    d_total_byte_cnt(0),
    d_abort_cnt(0),
    d_seven_ones_cnt(0),
    d_non_align_cnt(0),
    d_giant_cnt(0),
    d_runt_cnt(0),
    d_unstuff_zero_cnt(0),
    d_ber(0.0),
    d_fer(0.0),
    d_n_frames(0)
{

}

hdlc_deframer::~hdlc_deframer(){

}



int hdlc_deframer::unstuff(const int             bit_buf_size, 
                           const unsigned char * bit_buf, 
                           int *           frame_buf_size, 
                           unsigned char * frame_buf){
    int           i;
    unsigned char data_bit;
    int           accumulated_bits;
    int           bytes;
    int           consecutive_one_bits;
    int           status;
    
    accumulated_bits = 0;
    bytes = 0;
    consecutive_one_bits = 0;
    
    for(i=0; i<bit_buf_size; i++)
    {
        data_bit = bit_buf[i];
        if( (consecutive_one_bits != 5) || (data_bit == 1) )
        { 
            // Not a stuffed 0,  Write it to frame_buf
            frame_buf[bytes] = (frame_buf[bytes] >> 1) | (data_bit << 7);
            accumulated_bits++;
            if(accumulated_bits == 8)
            {
                bytes++;
                accumulated_bits = 0;
            }
        } else {
            d_unstuff_zero_cnt++;
            DEBUG1("unstuff a zero: %d\n",d_unstuff_zero_cnt);
        }
        
        if(data_bit == 1)
        {
            consecutive_one_bits++;
        }
        else
        {
            consecutive_one_bits = 0;
        }
    }
    
    // Now check for an integer number of bytes
    if(accumulated_bits == 0)
    {
        status = SUCCESS;
        *frame_buf_size = bytes;
    }
    else
    {
        status = FAIL;
        *frame_buf_size = 0;
        //*frame_buf_size = bytes;
    }
    
    return status;
    
}


void
hdlc_deframer::hdlc_state_machine(const unsigned char next_bit)
{
    int           next_state;
    int           i;
    unsigned char frame_buf[FRAME_BUF_MAX];
    int           frame_size;
    int           status;
    int           dlci;
            
    //printf("new bit: %02x\n",next_bit);
    switch(d_state)
    {
    case HUNT:
        //DEBUG0("State = HUNT\n");
        //fprintf(stderr, "State = HUNT\n");
        // Preload the first 7 bits to get things started
        d_byte = (d_byte >> 1) | (next_bit << 7);
        d_accumulated_bits++;
        if(d_accumulated_bits < 7)
        {
            next_state = HUNT;
        }
        else
        {
            next_state = IDLE;
        }
        break;
        
    case IDLE:
        //DEBUG0("State = IDLE\n");
        //fprintf(stderr, "State = IDLE\n");
        d_byte = (d_byte >> 1) | (next_bit << 7);
        if(d_byte == FLAG)
        {
            // Count it and keep hunting for more flags
            d_flag_cnt++;
            d_byte = 0x00;
            d_accumulated_bits = 0;
            next_state = HUNT;
        }
        else
        {
            // A non-FLAG byte starts a frame
            // Store the bits in the bit_buf, lsb first, and 
            // change states.
            for(i=0; i<8; i++)
            {
                d_bit_buf[i] = (d_byte >> i) & 0x01;
            }
            d_bit_buf_size = 8;
            next_state = FRAMING;
        }
        break;
        
    case FRAMING:
        //DEBUG1("State = FRAMING: d_bit_buf_size: %d\n",d_bit_buf_size);
        //fprintf(stderr, "State = FRAMING   bit_buf_size = %d\n", bit_buf_size);
        // Collect frame bits in bit_buf for later unstuffing
        if(d_bit_buf_size < BIT_BUF_MAX)
        {
            d_bit_buf[d_bit_buf_size] = next_bit;
            d_bit_buf_size++;
        }
        
        // Count consecutive 1 bits
        if(next_bit == 1)
        {
            d_consecutive_one_bits++;
        }
        else
        {
            d_consecutive_one_bits = 0;
        }
        
        // Test for Aborts and FLAGs
        if(d_consecutive_one_bits > 7)
        {
            // Too many 1 bits in a row. Abort.
            DEBUG0("ERROR Too many 1 bits in a row. Abort.");
            d_abort_cnt++;
            d_seven_ones_cnt++;
            d_byte = 0x00;
            d_accumulated_bits = 0;
            next_state = HUNT;
        }
        else
        {
            
            // Pack bit into byte buffer and check for FLAG
            d_byte = (d_byte >> 1) | (next_bit << 7);
            //printf("d_byte:%02x\n",d_byte);
            if(d_byte != FLAG)
            {
                // Keep on collecting frame bits
                next_state = FRAMING;
            }
            else 
            {
                //DEBUG0("DEBUG: It's a flag\n");
                // It's a FLAG. Frame is terminated.
                d_flag_cnt++;
                
                // Remove flag from bit_buf
                d_bit_buf_size -= 8;
                
                // Process bit_buf and
                // see if we got a good frame.
                status = unstuff(d_bit_buf_size, d_bit_buf, &frame_size, frame_buf);
                
                //fprintf(stderr, "  Unstuffed Frame Size = %d\n", frame_size);
                //DEBUG1("  Unstuffed Frame Size = %d\n", frame_size);
                if(status == FAIL)
                {
                    // Not an integer number of bytes.  Abort.
                    d_abort_cnt++;
                    d_non_align_cnt++;
                    DEBUG0("  NON-ALIGNED FRAME\n");
                    //print_frame(frame_size, frame_buf);
                    //fprintf(stderr, "    NON-ALIGNED FRAME\n\n");
                    //fflush(stderr);
                }
                else
                {
                    // Check frame size
                    if(frame_size < 6)
                    {
                        // Too small
                        d_runt_cnt++;
                        //fprintf(stderr, "    RUNT\n\n");
                        //fflush(stderr);
                    }
                    else if(frame_size > FRAME_MAX)
                    {
                        // Too big
                        d_giant_cnt++;
                        //fprintf(stderr, "    GIANT\n\n");
                        //fflush(stderr);
                    }
                    else
                    {
                        // Size OK. Check crc
                        status = crc_valid(frame_size, frame_buf);

                        // Regardless of the result, take bytes into
                        // account in the total number of bytes
                        d_total_byte_cnt += frame_size-2; // don't count CRC

                        if(status == FAIL)
                        {
                            // Log crc error
                            d_crc_err_cnt++;
                            //print_frame(frame_size, frame_buf);
                            DEBUG0("DEBUG: BAD CRC\n");
                            //fprintf(stderr, "    BAD CRC\n\n");
                            //fflush(stderr);
                            d_err_byte_cnt += frame_size-2; // don't count CRC
                        }
                        else
                        {
                            // Good frame! Log statistics
                            d_good_frame_cnt++;
                            DEBUG1("deframer good frame cnt: %d\n", d_good_frame_cnt);
                            d_good_byte_cnt += frame_size-2; // don't count CRC
                            //fprintf(stdout, "    Good Frame\n\n");
                            //fflush(stdout);
                            
                            // Check for proper virtual channel (DLCI)
                            dlci = ((frame_buf[0]<<2) & 0x03F0) | 
                                ((frame_buf[1]>>4) & 0x000F);
                            if(dlci == d_dlci)
                            {
                                // Correct channel. Log it and check for IP payload.
                                d_good_dlci_cnt++;
                                //fprintf(stdout, "    Good DLCI\n\n");
                                //fflush(stdout);
                                if( (frame_buf[2] == 0x03) &&
                                    (frame_buf[3] == 0xCC))
                                {
                                    // It's an IP packet. Route it.
                                    //fprintf(stdout, "    IP Packet Found\n\n");
                                    //fflush(stdout);
                                    
                                    route_packet(frame_size, frame_buf);
                                }
                            }
                        }

                        // Update statistics: BER, BER and number of frames
                        d_n_frames = d_crc_err_cnt + d_good_frame_cnt;
                        d_fer      = (float) d_crc_err_cnt / d_n_frames;
                        d_ber      = (float) d_err_byte_cnt / d_total_byte_cnt;
                    }
                }
                // Hunt for next flag or frame
                d_byte = 0x00;
                d_accumulated_bits = 0;
                next_state = HUNT;
            }
        }     
        break;
        
    } // end switch
    
    // Save new state and return
    d_state = next_state;
}


bool hdlc_deframer::empty(){
    return d_packets.empty();
}
vector<unsigned char> hdlc_deframer::get_packet(){
    vector<unsigned char> packet;
    if (d_packets.empty()){
        return packet;
    }
    packet=d_packets.front();
    d_packets.pop();
    return packet;
}


void hdlc_deframer::print_packet(vector<unsigned char> packet){
    printf("*** packet ***\n");
    printf("header: %02x %02x %02x %02x\n",
           packet[0],
           packet[1],
           packet[2],
           packet[3]);
    printf("data: ");
    for (unsigned int i=4;i<packet.size()-2;i++){
        printf("%02x ",packet[i]);
    }
    printf("\n");
    printf("crc: ");
    for (unsigned int i=packet.size()-2;i<packet.size();i++){
        printf("%02x ",packet[i]);
    }
    printf("\n\n");
}

void hdlc_deframer::route_packet(int             hdlc_frame_size, 
                                 unsigned char * hdlc_frame){
    vector<unsigned char> packet;
    packet.reserve(hdlc_frame_size);
    //DEBUG0("ROUTE_PACKET\n");
    for (int i=4;i<hdlc_frame_size-2;i++){
        //printf("hdlc_frame[%d]=%02x\n",i,hdlc_frame[i]);
        packet.push_back(hdlc_frame[i]);
    }
    /*
    printf("routed packet: ");
    for (int i=0;i<packet.size();i++){
        printf("%02x ",packet[i]);
    }
    printf("\n");
    */
    d_packets.push(packet);
}


int hdlc_deframer::crc_valid(int frame_size, unsigned char * frame)
{
    unsigned short frame_crc;
    unsigned short calc_crc;
    
    frame_crc = frame[frame_size-1] | (frame[frame_size-2] << 8);
    calc_crc = crc16(frame, frame_size-2);
    //printf("Frame_crc = %04X   Calc_crc = %04X\n", frame_crc, calc_crc);
    return(calc_crc == frame_crc);
}        

        

unsigned short hdlc_deframer::crc16(unsigned char *data_p, unsigned short length)
{
    const int     POLY = 0x8408;   // 1021H bit reversed 
    unsigned char i;
    unsigned int  data;
    unsigned int  crc = 0xffff;
    
    if (length == 0)
        return (~crc);
    do
    {
        for (i=0, data=(unsigned int)0xff & *data_p++;
             i < 8; 
             i++, data >>= 1)
        {
            if ((crc & 0x0001) ^ (data & 0x0001))
                crc = (crc >> 1) ^ POLY;
            else  crc >>= 1;
        }
    } while (--length);
    
    crc = ~crc;
    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff);
    
    return (crc);
}
