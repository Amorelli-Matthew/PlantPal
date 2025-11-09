#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

//UART bit masks
#define RDA 0x80  // RXC0: Receive Data Available
#define TBE 0x20  // UDRE0: Transmit Buffer Empty


// UART0 registers 
extern volatile unsigned char *myUCSR0A; 
extern volatile unsigned char *myUCSR0B;
extern volatile unsigned char *myUCSR0C;
extern volatile unsigned int  *myUBRR0;
extern volatile unsigned char *myUDR0;



void U0init(int U0baud);
void U0putchar(unsigned char c);

void uart_print_str(const char *s);
void uart_print_uint(unsigned int v);
void uart_print_crlf(void);


#ifdef __cplusplus
}
#endif

#endif