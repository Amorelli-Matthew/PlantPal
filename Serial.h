#ifndef SERIAL_H
#define SERIAL_H

//UART bit masks
#define RDA 0x80  // RXC0: Receive Data Available
#define TBE 0x20  // UDRE0: Transmit Buffer Empty


// UART0 registers for water sample
extern volatile unsigned char *myUCSR0A;
extern volatile unsigned char *myUCSR0B;
extern volatile unsigned char *myUCSR0C;
extern volatile unsigned int *myUBRR0;
extern volatile unsigned char *myUDR0;


//init medthod
void U0init(int U0baud);
void U0putchar(unsigned char c);


//printing methods
void print(const char *s);
void print(unsigned int v);
void print_crlf(void);

//print line versions
//bascially add in the crlf
void println(const char *s);
void println(unsigned int v);
void println(double value, unsigned int decimals);

#endif