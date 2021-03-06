/* vim: set noexpandtab ai ts=4 sw=4 tw=4: */
/* hardware.h -- emulation of various Motorola and Rockwell peripherals
   Copyright (C) 2021 Michel J Wurtz

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

extern uint16_t mem_low ;	// base address of physical memory emulated
extern uint16_t mem_high ;	// upper limit of physical memory emulated
extern uint16_t rom ;		// base address of rom (allways on top of memory)

extern int loading ;

extern struct Device {
	char devname[16];
	int type;
	uint16_t addr;
	uint16_t end;
	char interrupt;
	void *registers;
	struct Device *next;
	} *devices;

extern struct Device *look_dev( uint16_t adr);
extern void showdev();
extern void device_run();
extern uint8_t read_device(uint16_t adr);
extern void write_device(uint16_t adr, uint8_t val);

extern void mc6820_init( char* devname, uint16_t adr, char int_line);
extern void mc6820_run( struct Device *dev);
extern uint8_t mc6820_read( struct Device *dev, uint16_t adr);
extern void mc6820_write( struct Device *dev, uint16_t adr, uint8_t val);
extern void mc6820_reg( struct Device *dev);

extern void mc6840_init( char* devname, uint16_t adr, char int_line);
extern void mc6840_run( struct Device *dev);
extern uint8_t mc6840_read( struct Device *dev, uint16_t adr);
extern void mc6840_write( struct Device *dev, uint16_t adr, uint8_t val);
extern void mc6840_reg( struct Device *dev);

extern void mc6850_init( char* devname, uint16_t adr, char int_line, int32_t speed);
extern void mc6850_run( struct Device *dev);
extern uint8_t mc6850_read( struct Device *dev, uint16_t adr);
extern void mc6850_write( struct Device *dev, uint16_t adr, uint8_t val);
extern void mc6850_reg( struct Device *dev);

extern void r6522_init( char* devname, uint16_t adr, char int_line);
extern void r6522_run( struct Device *dev);
extern uint8_t r6522_read( struct Device *dev, uint16_t adr);
extern void r6522_write( struct Device *dev, uint16_t adr, uint8_t val);
extern void r6522_reg( struct Device *dev);

extern void r6532_init( char* devname, uint16_t adr, char int_line);
extern void r6532_run( struct Device *dev);
extern uint8_t r6532_read( struct Device *dev, uint16_t adr);
extern void r6532_write( struct Device *dev, uint16_t adr, uint8_t val);
extern void r6532_reg( struct Device *dev);

extern void fd1795_init( char* devname, uint16_t adr, char int_line, char *dskname);
extern void fd1795_run( struct Device *dev);
extern uint8_t fd1795_read( struct Device *dev, uint16_t adr);
extern void fd1795_write( struct Device *dev, uint16_t adr, uint8_t val);
extern void fd1795_reg( struct Device *dev);

extern void fake_init( char* devname, uint16_t adr, uint16_t end);
extern uint8_t fake_read( struct Device *dev, uint16_t adr);
extern void fake_write( struct Device *dev, uint16_t adr, uint8_t val);

// Interface adapters kown, other can be added
// Motorola :
#define MC6820 0	// PIA <=> MC6821, R6520, R6521
#define MC6840 1	// TIMER
#define MC6850 2	// ACIA

// Rockwell :
#define R6522 4		// VIA (Versatile Interface adapter : I/O + timer)
#define R6532 5		// RIOT (RAM, I/O, TIMER)

// Western Digital
#define FD1795 8	// Floppy disk controler

// Dummy device - emulates memory
#define FAKE 0x10
