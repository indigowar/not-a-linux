#ifndef H_NAL_DRIVERS_PORTS_H
#define H_NAL_DRIVERS_PORTS_H

// read a byte from the specified port(0)
unsigned char port_byte_in(unsigned short port);

// write to port(0) data(1)
void port_byte_out(unsigned short port, unsigned char data);

unsigned short port_word_in(unsigned short port);

void port_word_out(unsigned short port, unsigned short data);

#endif  // H_NAL_DRIVERS_PORTS_H
