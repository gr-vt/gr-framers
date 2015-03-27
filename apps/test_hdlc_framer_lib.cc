#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <hdlc_framer.h>
#include <hdlc_deframer.h>
#define DATA_SIZE 100


void print_bits(const unsigned char * bits, int len);
void print_bytes(const unsigned char * bytes, int len);


bool test_bitstuff(){
    // This test will bitstuff and then unstuff randomly generated data

    // Generate some random data
    int dlci=0;
    unsigned char data[DATA_SIZE];
    
    //srand(time(NULL))
    for(size_t i = 0; i < DATA_SIZE; i++){
        data[i] = rand() % 256;
    }

    // Create framer and deframer classes
    hdlc_framer framer(dlci);
    hdlc_deframer deframer(dlci);
    
    // Bit Stuff the data
    for (int i=0;i<DATA_SIZE;i++){
        framer.bitstuff_byte(data[i]);
    }

    // Dump the bitstuffed data from the framer
    vector<unsigned char> bitstuffed;
    bitstuffed=framer.dump_buffer();

    // Unstuff the data (inverse of bitstuffing)
    int unstuffed_size;
    unsigned char data_unstuffed[DATA_SIZE];
    deframer.unstuff(bitstuffed.size(),
                     &bitstuffed[0],
                     &unstuffed_size,
                     data_unstuffed);
    
    // Compare the unstuffed with the original data
    if (unstuffed_size!=DATA_SIZE){
        printf("ERROR: unstuffed size is not equal to original size\n");
        return false;
    }

    for (int i=0;i<unstuffed_size;i++){
        if (data[i]!=data_unstuffed[i]){
            printf("ERROR: data[%d]!=data_unstuffed[%d]\n",i,i);
            return false;
        }
    }
    return true;
}

bool test_bitstuff_and_frame_packet(){
    // This tests the bitstuff and frame packet function.  The inverse operations are then applied to make sure the output data matches the original input data


    // Generate some random data
    int dlci=0;
    unsigned char data[DATA_SIZE];
    
    //srand(time(NULL))
    for(size_t i = 0; i < DATA_SIZE; i++){
        data[i] = rand() % 256;
    }

    // Create framer and deframer classes
    hdlc_framer framer(dlci);
    hdlc_deframer deframer(dlci);

    framer.bitstuff_and_frame_packet(data,DATA_SIZE);

    vector<unsigned char> framed;
    framed=framer.dump_buffer();

    //print_bits(&framed[0],framed.size());
    //print_bits(&bitstuffed[0],bitstuffed.size());

    int framed_unstuffed_size;
    unsigned char framed_unstuffed[DATA_SIZE*2];
    deframer.unstuff(framed.size(),
                     &framed[0],
                     &framed_unstuffed_size,
                     framed_unstuffed);

    //print_bytes(data,DATA_SIZE);
    //print_bytes(framed_unstuffed,framed_unstuffed_size);

    // Make sure that the first byte is a flag
    if (framed_unstuffed[0]!=0x7e){
        printf("ERROR: FLAG Incorrect\n");
        return false;
    }
    
    // Make sure that the rest of the bytes match
    for (int i=0;i<DATA_SIZE;i++){
        if (data[i]!=framed_unstuffed[i+1]){
            printf("ERROR: data[%d]!=framed_unstuffed[%d]\n",i,i+1);
            return false;
        }
    }
    return true;
}


bool test_encapsulate_data(){
    // This test runs the hdlc framer and deframer all the way through.  It does its best to simulate the gnuradio data flow without actually using gnuradio
    
    srand(time(NULL));
    // Generate some random data
    int dlci=0;
    int num_data_packets=10000; // Number of packets to be transmitted
    vector<vector<unsigned char> >  data_vector; // Vector to hold all transmitted packets for comparison at the end
    data_vector.reserve(num_data_packets);
    
    // Create framer and deframer classes
    hdlc_framer framer(dlci);
    hdlc_deframer deframer(dlci);

    for (unsigned int j=0;j<num_data_packets;j++){
        int rand_length=rand()%DATA_SIZE; // Create a random length for each packet
        vector<unsigned char> data; // Temporary location for packet data
        data.reserve(rand_length);
        for(size_t i = 0; i < rand_length; i++){
            data.push_back(rand() % 256); // Fill the packet
        }
        data_vector.push_back(data);
        // Run the framer on the packet data
        framer.encapsulate_incoming_packet(&(data_vector[j][0]),data_vector[j].size());
        
        // Every once in awhile, run the code to deframe the data that has been transmitted thus far
        if (rand()%3==0){
            //printf("read out all encapsulated data: %d\n",j);
            vector<unsigned char> encapsulated_data;
            encapsulated_data=framer.dump_buffer();
            for (int k=0;k<encapsulated_data.size();k++){
                deframer.hdlc_state_machine(encapsulated_data[k]);
            }
        }
        // Every once in awhile, add some flag frames
        if(rand()%5==0){
            for(int f=0;f<rand()%10;f++){
                framer.push_flag();
            }
        }
        
    }

    // Make sure a flag gets pushed at the end so that the last data frame is read
    framer.push_flag(); 
    
    // Deframe the rest of the data
    vector<unsigned char> encapsulated_data;
    encapsulated_data=framer.dump_buffer();
    
    for (int k=0;k<encapsulated_data.size();k++){
        deframer.hdlc_state_machine(encapsulated_data[k]);
    }
    


    for (int packet_index=0;packet_index<num_data_packets;packet_index++){
        // For each packet

        // Get the deframed packet
        vector<unsigned char> packet=deframer.get_packet();

        // Compare each byte with the transmitted packet
        for (int i=0;i<data_vector[packet_index].size();i++){
            if (data_vector[packet_index][i]!=packet[i]){
                printf("ERROR: data[%d]!=packet[%d]",i,i);
                return false;
            }
        }
    }

    // If all packets were correctly framed and deframed then return success
    return true;

}

int main(void){
    
    // Test bitstuff
    printf("Testing bitstuff\n");
    if (!test_bitstuff()){
        printf("Bitstuff test failed\n");
        return -1;
    }
    printf("bitstuff test succeeded\n\n");
    
    // Test bitstuff and frame packet
    printf("Testing bitstuff_and_frame_packet\n");
    if (!test_bitstuff_and_frame_packet()){
        printf("Bitstuff_and_frame_packet failed\n");
        return -1;
    }
    printf("Bit Stuff and frame packet succeeded\n\n");

    // Test encapsulate_data
    printf("Testing encapsulate data\n");
    if (!test_encapsulate_data()){
        printf("Encapsulate data failed\n");
        return -1;
    }
    printf("Encapsulate data succeeded\n\n");


    printf("All hdlc library tests passed\n");
    return 0;
}


// Debugging function to print a vector of bytes

void print_bytes(const unsigned char * bytes, int len){
    for (unsigned int i=0;i<len;i++){
        printf("%02x ",bytes[i]);
        if ((i+9)%8==0){
            printf("\n");
        }
    }
    printf("\n\n");
}

// Debugging function to print a vector of unstuffed bits (one bit in each byte)

void print_bits(const unsigned char * bits, int len){
    for (unsigned int i=0;i<len;i++){
        printf("%x ",bits[i]);
        if ((i+9)%8==0){
            printf("\n");
        }
    }
    printf("\n\n");
}
