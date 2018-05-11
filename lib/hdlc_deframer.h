#include <vector>
#include <queue>
using std::vector;
using std::queue;


class hdlc_deframer{
public:
    // Constructor/Destructor
    hdlc_deframer(int dlci);
    ~hdlc_deframer();

    // This function should be called on each new bit
    void hdlc_state_machine(const unsigned char next_bit);

    // Remove packet from the queue of valid packets
    vector<unsigned char> get_packet();
    bool empty();
    // ******************
    // Static functions
    // ******************

    // Converts bit stream to byte stream (and removes inserted zeros)
    int unstuff(const int             bit_buf_size, 
                const unsigned char * bit_buf, 
                int *           frame_buf_size, 
                unsigned char * frame_buf);


    // Debugging helper function for printing an HDLC packet
    static void print_packet(vector<unsigned char> packet);

    // Public statistics
    float d_ber;

private:
    // CONSTANTS ------------------
    static const int           SUCCESS      = 1;
    static const int           FAIL         = 0;
    static const int           BIT_BUF_MAX  = 78644; // Allow for up to 20% bitstuffing
    static const int           FRAME_BUF_MAX= 9831;  // Allow for no stuffed bits
    static const int           FRAME_MAX    = 8192;
    static const unsigned char FLAG         = 0x7E;
    static const int           HUNT         = 0;
    static const int           IDLE         = 1;
    static const int           FRAMING      = 2;
    
    // Private Attributes ---------
    
    // Frame Relay's Data Link Channel Indicator
    int            d_dlci; 
    
    // State machine state info
    int            d_state;
    unsigned char  d_byte;  // Accumulator for building a flag byte from bits
    int            d_accumulated_bits;        // Bit counter for d_byte
    unsigned char  d_bit_buf[BIT_BUF_MAX];
    int            d_bit_buf_size;
    int            d_consecutive_one_bits;
    
    // Data statistics
    int            d_flag_cnt;
    int            d_total_byte_cnt;
    int            d_good_frame_cnt;
    int            d_good_byte_cnt;
    int            d_good_dlci_cnt;
    int            d_unstuff_zero_cnt;
    // Error statistics
    int            d_crc_err_cnt;
    int            d_err_byte_cnt;
    int            d_abort_cnt;
    int            d_seven_ones_cnt;
    int            d_non_align_cnt;
    int            d_giant_cnt;
    int            d_runt_cnt;

    // Private Functions

    unsigned short crc16(unsigned char *data_p, 
                         unsigned short length);
    
    int crc_valid(int frame_size, unsigned char * frame);
    
    // This is the function that gets called when a valid packet is found
    // It adds the valid packet to the list
    virtual void route_packet(int             hdlc_frame_size, 
                      unsigned char * hdlc_frame);

    // Output
    queue<vector<unsigned char > > d_packets;

};

