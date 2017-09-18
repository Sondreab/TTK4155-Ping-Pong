/*
 * XMEM_driver.c
 *
 * Created: 11.09.2017 12:57:55
 *  Author: hploubat
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

int XMEM_init(void) {
	//enable external memory
	MCUCR |= (1<<SRE);
	//mask out JTAG p32
	SFIOR |= (1<<XMM2);
	return 0;
}

void XMEM_test(void) {
		volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
		uint16_t ext_ram_size = 0x800;
		uint16_t write_errors = 0;
		uint16_t retrieval_errors = 0;
		printf("Starting SRAM test...\n");
		// rand() stores some internal state, so calling this function in a loop will
		// yield different seeds each time (unless srand() is called before this function)
		uint16_t seed = rand();
		// Write phase: Immediately check that the correct value was stored 
		srand(seed);
		for (uint16_t i = 0; i < ext_ram_size; i++) {
			uint8_t some_value = rand();
			ext_ram[i] = some_value;
			_delay_us(100);
			uint8_t retreived_value = ext_ram[i];
			if (retreived_value != some_value) {
				printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
				write_errors++;
			}
		}
		// Retrieval phase: Check that no values were changed during or after the write phase
		srand(seed);
		// reset the PRNG to the state it had before the write phase
		for (uint16_t i = 0; i < ext_ram_size; i++) {
			uint8_t some_value = rand();
			uint8_t retreived_value = ext_ram[i];
			if (retreived_value != some_value) {
				printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
				retrieval_errors++;
			}
		}
		printf("SRAM test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase \n\n", write_errors, retrieval_errors);
}

void XMEM_simple_test(void) {
	volatile char *specific_address = (char *) 0x1902;
	specific_address[0] = 0xB2;
	
	_delay_ms(2000);
	printf("%02X", specific_address[0]);
	
	
}