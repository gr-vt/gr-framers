#include "hdlc_framer.h"

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


hdlc_framer::hdlc_framer(int dlci)
    : d_dlci(dlci),
      d_consecutive_one_bits(0),
      d_flag_cnt(0),
      d_frame_cnt(0),
      d_byte_cnt(0),
      d_stuffed_zero_cnt(0)
{
    // Pre-construct MPoFR header, using 10-bit DLCI
    d_header[0] = (d_dlci>>2) & 0xFC;  // hi-order 6 bits of DLCI into bits 7 thru 2
    d_header[1] = (d_dlci<<4) & 0xF0;  // lo-order 4 bits of DLCI into bits 7 thru 4
    d_header[2] = 0x03;
    d_header[3] = 0xCC;
}

hdlc_framer::~hdlc_framer(){

}



void hdlc_framer::encapsulate_incoming_packet(unsigned char * frame_in,int packet_size){
    unsigned short crc;
    
    if (packet_size<0){
        DEBUG0("hdlc_framer ERROR: packet_size<0\n");
        return;
    }
    vector<unsigned char> frame_buf;
    frame_buf.reserve(packet_size+6);
    
    for (int i=0;i<4; i++){
        frame_buf.push_back(d_header[i]);
    }
    for (int i=0;i<packet_size;i++){
        frame_buf.push_back(frame_in[i]);
    }
    // Calculate frame's crc, including header
    crc = crc16(&frame_buf[0], packet_size+4);
    
    // Add crc to end of frame
    frame_buf.push_back(crc>>8);
    frame_buf.push_back(crc &0xFF);

    // Add FLAG and do bitstuffing
    bitstuff_and_frame_packet(&frame_buf[0], frame_buf.size());
}



void hdlc_framer::push_flag(void)
{
    int           i;
    unsigned char flag = FLAG;
    unsigned char bit;
    
    // Take care of any pending zero-stuffing
    if(d_consecutive_one_bits == 5)
    {
        d_fifo.push(0);  // Stuff a zero before the next bit
        d_stuffed_zero_cnt++;
        //printf("stuff a zero in push_flag\n");
        d_consecutive_one_bits = 0;
    }
    
    if (d_fifo.space_left() >= 8)
    {
        for(i=0; i<8; i++)
        {
            bit = flag & 0x01;
            flag = flag >> 1;
            d_fifo.push(bit);
        }
        d_flag_cnt++;
        // Reset 1's counter
        d_consecutive_one_bits = 0;
    }
    else
    {
        fprintf(stderr, "Router Source: Fifo overflow while pushing flag.\n");
    }
}

unsigned short hdlc_framer::crc16(unsigned char *data_p, unsigned short length)
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

void hdlc_framer::bitstuff_and_frame_packet(unsigned char * frame_buf,
                                            int             frame_size)
{
    int i;
    
    // Push Flag byte into fifo to mark frame start
    push_flag();
    
    // Bitstuff frame and push it into fifo
    for(i=0; i<frame_size; i++)  // For each byte of frame
    {
        bitstuff_byte(frame_buf[i]);
    }
    // Count frames
    d_frame_cnt++;
    DEBUG1("bitstuff and frame packet:%d \n",d_frame_cnt);
    push_flag();
}
// Take a byte and bitstuff it into the fifo, lsb first.

void hdlc_framer::bitstuff_byte(unsigned char byte)
{
    unsigned char bit;
    int           i;
    
    for(i=0; i<8; i++)  // For each bit
    {
        bit = byte & 0x01;
        byte = byte >> 1;
        
        if(d_consecutive_one_bits == 5)
        {
            d_fifo.push(0);  // Stuff a zero before the next bit
            d_stuffed_zero_cnt++;
            DEBUG1("stuff a zero: %d\n",d_stuffed_zero_cnt);
            d_consecutive_one_bits = 0;
        }
        
        d_fifo.push(bit);
        
        if(bit == 0)
        {
            d_consecutive_one_bits = 0;
        }
        else // Bit is '1'
        {
            d_consecutive_one_bits++;
        }
    }
    d_byte_cnt++;
}



// DEBUG FUNCTIONS

vector<unsigned char> hdlc_framer::dump_buffer(){
    vector<unsigned char> buffer;

    while(!d_fifo.empty()){
        buffer.push_back(d_fifo.pop());
    }
    /*
    d_dlci=0;
    d_consecutive_one_bits=0;
    d_flag_cnt=0;
    d_frame_cnt=0;
    d_byte_cnt=0;
    */
    return buffer;
}
