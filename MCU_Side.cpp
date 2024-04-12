#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#define F_CPU 16000000UL
#define BAUD_RATE 2400
#define DATA_SIZE 1000

void uart_init() {
    UBRR0H = (F_CPU / 16 / BAUD_RATE - 1) >> 8;
    UBRR0L = (F_CPU / 16 / BAUD_RATE - 1);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Set frame format: 8 data bits, 1 stop bit
}

void uart_transmit(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = data; // Put data into buffer, sends the data
}

unsigned char uart_receive() {
    while (!(UCSR0A & (1 << RXC0))); // Wait for data to be received
    return UDR0; // Get and return received data from buffer
}

void store_data_to_eeprom(char* data) {
    for (int i = 0; i < DATA_SIZE; i++) {
        eeprom_write_byte((uint8_t*)i, data[i]);
        _delay_ms(5); // Delay to ensure EEPROM write cycle completion
    }
}

void transmit_data_from_eeprom() {
    for (int i = 0; i < DATA_SIZE; i++) {
        char data = eeprom_read_byte((uint8_t*)i);
        uart_transmit(data);
    }
}

int main() {
    char data[DATA_SIZE];
    int index = 0;

    uart_init();

    while (1) {
        char received_data = uart_receive();
        data[index++] = received_data;

        if (index >= DATA_SIZE) {
            store_data_to_eeprom(data);
            break;
        }
    }

    transmit_data_from_eeprom();

    return 0;
}
