#include <vector>

using std::vector;
#include <stdio.h>
class fifo_c
{
private:
    static const int  FIFO_SIZE = 76651;     // (8192 x 8 x 1.2) + 7
    unsigned char     d_ring_buf[FIFO_SIZE];
    int               d_pop_index;           // next position to pop from
    int               d_push_index;          // next position to push to
public:
fifo_c():d_pop_index(0),d_push_index(0) {};
    ~fifo_c() {};
    void push(unsigned char c) 
    {
      if (d_push_index-d_pop_index>500){
	//printf("framer: push-pop %d\n",d_push_index-d_pop_index);
      }
        d_ring_buf[d_push_index] = c;
        d_push_index = (d_push_index + 1) % FIFO_SIZE;
    };
    unsigned char pop()
    {
        unsigned char c;
        c = d_ring_buf[d_pop_index];
        d_pop_index = (d_pop_index + 1) % FIFO_SIZE;
        return c;
    };
    int empty() { return d_pop_index==d_push_index; };
    int full()  { return (d_push_index+1)%FIFO_SIZE == d_pop_index; };
    int space_left() { return FIFO_SIZE - 1 - (d_push_index - d_pop_index)%FIFO_SIZE; };
};


// Private CONSTANTS ------------------
static const int           LINUX        = 0;
static const int           OSX          = 1;
static const int           FRAME_MAX    = 8192;
static const int           STR_MAX      = 256;   // Limit string lengths
static const unsigned char FLAG         = 0x7E;




class hdlc_framer{
public:
    hdlc_framer(int dlci);
    ~hdlc_framer();
    unsigned short crc16(unsigned char *data_p, 
                         unsigned short length);


    // Private Methods ----------


    void encapsulate_incoming_packet(unsigned char * frame_buf,int packet_length);
    vector<unsigned char> dump_buffer();

    // For debug only
    void push_flag(void);
    void bitstuff_byte(unsigned char byte);
    void bitstuff_and_frame_packet(unsigned char * frame_buf,
                                   int             frame_size);

private:
    // Private CONSTANTS ------------------
    static const int           LINUX        = 0;
    static const int           OSX          = 1;
    static const int           FRAME_MAX    = 8192;
    static const int           STR_MAX      = 256;   // Limit string lengths
    static const unsigned char FLAG         = 0x7E;
            
    // Private Attributes ---------
            
    // Frame Relay's Data Link Channel Indicator
    int            d_dlci; 
    
    // MPoFR Header
    unsigned char  d_header[4];
    
    
    // Bitstuffing state variable
    int            d_consecutive_one_bits;
            
    // File descriptor for tun device
    int            d_tun_fd;
    
    // Bitstream fifo
    fifo_c         d_fifo;  
            
    // Data statistics
    int            d_flag_cnt;
    int            d_frame_cnt;
    int            d_byte_cnt;
    int            d_stuffed_zero_cnt;
    
};


