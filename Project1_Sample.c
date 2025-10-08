

#include <stdio.h>
#include <stdint.h>
#define ARR_SIZE 100
#define SPI_BYTE_LENGTH 8


enum STATES {high, low};
enum OUTPUT {RISING, FALLING, HIGH, LOW};
struct row {enum STATES Qn; uint8_t in; enum STATES Qnp1; enum OUTPUT out;};
const struct row TransitionTable[4] = {	{high,	0,	low,	FALLING},
										{high,	1,	high,	HIGH},
										{low,	0,	low,	LOW},
										{low,	1,	high,	RISING}};


struct CompleteFSM {enum STATES state; const uint8_t num_transitions; const struct row *table;};

struct CompleteFSM ClockFSM = {low, 4, TransitionTable};


enum OUTPUT getFSMoutput (uint8_t input, struct CompleteFSM *x){
	enum OUTPUT out = LOW;
	for (int i = 0; i < (x -> num_transitions); i++){
		if ((x -> table[i].Qn) == (x -> state)){
			if ((x -> table[i].in) == input){
				(x -> state) = (x -> table[i].Qnp1);
				out  = (x -> table[i].out);
				break;
			}
		}
	}
	return out;
}







// AI generated function to create dummy SPI arrays
void SPI_data_gen(uint8_t CLK[ARR_SIZE], uint8_t MOSI[ARR_SIZE], uint8_t data) {
    const uint8_t NUM_SAMPLES = ARR_SIZE;
    const uint8_t BITS_PER_BYTE = SPI_BYTE_LENGTH;
    const uint8_t samples_per_clock = 6;   // total samples per clock cycle (e.g., 3 low + 3 high)
    const uint8_t start_index = 25;        // where the "burst" starts
    
    // MSB First

    // 1. Initialize arrays to idle state (Mode 0: CLK low, MOSI low)
    for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
        CLK[i] = 0;
        MOSI[i] = 0;
    }

    // 2. Generate the 8 clock pulses + MOSI bits starting at start_index
    uint8_t index = start_index;
    for (int8_t bit = 7; bit >= 0; bit--) {
        uint8_t bit_val = (data >> bit) & 1;

        // Low phase (first half of the cycle)
        for (uint8_t i = 0; i < samples_per_clock / 2; i++) {
            if (index >= NUM_SAMPLES) break;
            CLK[index] = 0;
            MOSI[index] = bit_val;
            index++;
        }

        // High phase (second half of the cycle)
        for (uint8_t i = 0; i < samples_per_clock / 2; i++) {
            if (index >= NUM_SAMPLES) break;
            CLK[index] = 1;
            MOSI[index] = bit_val;
            index++;
        }
    }
}



void SPI_data_print(uint8_t CLK[ARR_SIZE], uint8_t MOSI[ARR_SIZE]){
    // Debug print samples to inspect waveform
    printf("CLK =[");
    for (int i = 0; i < ARR_SIZE; i++) {
        printf("%d", CLK[i]);
        if (i != (ARR_SIZE-1)){printf(",");}
    }
    printf("]\n");

    printf("MOSI=[");
    for (int i = 0; i < ARR_SIZE; i++) {
        printf("%d", MOSI[i]);
        if (i != (ARR_SIZE-1)){printf(",");}
    }
    printf("]\n");
}


// Converting an array to an integer
uint8_t ARR2HEX (uint8_t arr[SPI_BYTE_LENGTH]){
    uint8_t value = 0;
    
    for (int i = 0; i < SPI_BYTE_LENGTH; i++){
        if (arr[i] == 1){
            value |= 1<<((SPI_BYTE_LENGTH-1)-i); // Adjusting for MSB first
        }
    }
    
    return value;
}


void PrintBinData(uint8_t arr[SPI_BYTE_LENGTH]){
    printf("SPI byte = [");
    for (int i = 0; i < SPI_BYTE_LENGTH; i++) {
        printf("%d", arr[i]);
        if (i != (SPI_BYTE_LENGTH-1)){printf(",");}
    }
    printf("]\n");
}



void acquire_SPI_data(uint8_t CLK[ARR_SIZE], uint8_t MOSI[ARR_SIZE], uint8_t SPIbyte[SPI_BYTE_LENGTH]){
    uint8_t n = 0; // used for SPIbyte indexing
    
    //Setting the state of the FSM as the first value to ensure there are no edge detections from previous states/runs
    ClockFSM.state = (CLK[0] == 1) ? high : low;
    
    // Clearing SPIbyte array to avoid leftover bits from previous runs
    for (int i = 0; i < SPI_BYTE_LENGTH; i++) {
        SPIbyte[i] = 0;
    }
    
    for (uint8_t i = 0; i < ARR_SIZE; i++){
        // Iterating through the clock list, if rising clock edge, grab the SPI array value at that spot and appending it to BinData
        if (getFSMoutput(CLK[i], &ClockFSM) == RISING){
            // MSB first = lowest index in MOSI ([0]), and therefore lowest index in SPIbyte ([0])
            if (n<8){ // Just to prevent overflow
                SPIbyte[n] = MOSI[i];
                n++;
            }
        }
    }
}


int main() {
    
    // Declaring arrays for the SPI data
    uint8_t CLK[ARR_SIZE];
    uint8_t MOSI[ARR_SIZE];
    // Creating an array to hold the aquired SPI byte
    uint8_t SPIbyte[SPI_BYTE_LENGTH];
    // Varaible to hold the SPI data as a uint8_t
    uint8_t value;
    
    
    // Generating and printing dummy SPI data
    SPI_data_gen(CLK, MOSI, 0x9f);
    SPI_data_print(CLK, MOSI);
    
    
    // Acquiring the SPI byte from the given arrays
    acquire_SPI_data(CLK, MOSI, SPIbyte);
    
    // Printing the SPI byte as an array (MSB first)
    PrintBinData(SPIbyte);
    
    // Converting the SPIbyte array to a uint8_t
    value = ARR2HEX(SPIbyte);
    
    printf("Data value (hex): %x\n", value);
    
    
    
    
    return 0;
    
}