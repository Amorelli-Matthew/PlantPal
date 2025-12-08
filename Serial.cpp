#include "Serial.h"

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int *myUBRR0 = (unsigned int *)0x00C4;
volatile unsigned char *myUDR0 = (unsigned char *)0x00C6;


void U0init(int U0baud) {
  unsigned long FCPU = 16000000UL;  // Mega 2560 clock

  *myUCSR0A = 0x00;  // U2X0=0 (normal 16x)
  *myUCSR0B = 0x18;  // RXEN0=1, TXEN0=1
  *myUCSR0C = 0x06;  // 8N1: UCSZ01=1, UCSZ00=1

  unsigned int tbaud = (FCPU / (16UL * U0baud)) - 1;
  *myUBRR0 = tbaud;  // set baud
}

void U0putchar(unsigned char c) {
  while ((*myUCSR0A & TBE) == 0) {}  // wait while UDRE0==0
  *myUDR0 = c;
}

void print(const char *s) {
  while (*s) {
    U0putchar((unsigned char)*s++);
  }
}

void print_crlf(void) {
  U0putchar('\r');
  U0putchar('\n');
}

// Print ASCII digits
void print(unsigned int v) {
  char buf[5];  // up to "1023"
  int i = 0;

  if (v == 0) {
    U0putchar('0');
    return;
  }
  while (v > 0 && i < 4) {
    buf[i++] = (char)('0' + (v % 10));
    v /= 10;
  }
  while (i--) {
    U0putchar(buf[i]);  // send in correct order
  }
}

// Print floating point number (double) over UART
void print(double value, unsigned int decimals) {
  unsigned int int_part;
  double frac_part;

  // Handle negative numbers
  if (value < 0) {
    U0putchar('-');
    value = -value;
  }

  // Integer part
  int_part = (unsigned int)value;
  print(int_part);

  // Decimal point
  U0putchar('.');

  // Fractional part
  frac_part = value - (double)int_part;

  // Print decimals
  while (decimals--) {
    frac_part *= 10.0;
    U0putchar((char)('0' + (int)frac_part));
    frac_part -= (int)frac_part;
  }
}


//print line versions of printing either a string, int, or double
void println(const char *s) {
  while (*s) {
    U0putchar((unsigned char)*s++);
  }
  print_crlf();
}

void println(unsigned int v) {
  char buf[5];  // up to "1023"
  int i = 0;

  if (v == 0) {
    U0putchar('0');
    // New line
    print_crlf();
    return;
  }
  while (v > 0 && i < 4) {
    buf[i++] = (char)('0' + (v % 10));
    v /= 10;
  }
  while (i--) {
    U0putchar(buf[i]);  // send in correct order
  }

  // New line
  print_crlf();
}

void println(double value, unsigned int decimals) {
  unsigned int int_part;
  double frac_part;

  // Handle negative numbers
  if (value < 0) {
    U0putchar('-');
    value = -value;
  }

  // Print integer part
  int_part = (unsigned int)value;
  print(int_part);

  // Decimal point
  U0putchar('.');

  // Print fractional part
  frac_part = value - (double)int_part;

  while (decimals--) {
    frac_part *= 10.0;
    U0putchar((char)('0' + (int)frac_part));
    frac_part -= (int)frac_part;
  }

  // New line
  print_crlf();
}

void print(uint16_t value, uint8_t base) {
  char buf[17]; // Enough for 16-bit binary plus '\0'
  int i = 0;

  if (value == 0) {
    U0putchar('0');
    return;
  }

  while (value > 0 && i < 16) {
    uint8_t digit = value % base;
    if (digit < 10) {
      buf[i++] = '0' + digit;
    } else {
      buf[i++] = 'A' + (digit - 10); // HEX A-F
    }
    value /= base;
  }

  while (i--) {
    U0putchar(buf[i]); // reverse print
  }
}


void print(uint8_t value, uint8_t base) {
  char buf[9]; // Enough for 8-bit binary
  int i = 0;

  if (value == 0) {
    U0putchar('0');
    return;
  }

  while (value > 0 && i < 8) {
    uint8_t digit = value % base;
    if (digit < 10) {
      buf[i++] = '0' + digit;
    } else {
      buf[i++] = 'A' + (digit - 10);
    }
    value /= base;
  }

  while (i--) {
    U0putchar(buf[i]);
  }
}



