/* dis6809.c -- 6809 disassembler
   Copyright (C) 1998 Jerome Thoen

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include <stdio.h>

#include "config.h"
#include "emu6809.h"

/*
        modes:
        1 immediate
        2 direct    
        3 indexed   
        4 extended  
        5 inherent
        6 relative
*/

static const int mode[] = {
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  0,0,5,5,0,0,6,6,0,5,5,0,5,5,5,5,
  6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
  3,3,3,3,5,5,5,5,5,5,5,5,5,5,5,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  1,1,1,1,1,1,1,1,1,1,1,1,1,6,1,0,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,1,0,0,0,0,0,0,0,0,1,0,1,0,
  0,0,0,2,0,0,0,0,0,0,0,0,2,0,2,2,
  0,0,0,3,0,0,0,0,0,0,0,0,3,0,3,3,
  0,0,0,4,0,0,0,0,0,0,0,0,4,0,4,4,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,
  0,0,0,2,0,0,0,0,0,0,0,0,2,0,0,0,
  0,0,0,3,0,0,0,0,0,0,0,0,3,0,0,0,
  0,0,0,4,0,0,0,0,0,0,0,0,4,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

static const int size[] = {
  2,1,1,2,2,1,2,2,2,2,2,1,2,2,2,2,
  1,1,1,1,1,1,3,3,1,1,2,1,2,1,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,1,1,1,1,2,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,1,1,2,2,1,2,2,2,2,2,1,2,2,2,2,
  3,1,1,3,3,1,3,3,3,3,3,1,3,3,3,3,
  2,2,2,3,2,2,2,1,2,2,2,2,3,2,3,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
  2,2,2,3,2,2,2,1,2,2,2,2,3,1,3,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,

  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,4,1,1,1,1,1,1,1,1,4,1,4,1,
  1,1,1,3,1,1,1,1,1,1,1,1,3,1,3,3,
  1,1,1,3,1,1,1,1,1,1,1,1,3,1,3,3,
  1,1,1,4,1,1,1,1,1,1,1,1,4,1,4,4,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,
  
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,4,1,1,1,1,1,1,1,1,4,1,1,1,
  1,1,1,3,1,1,1,1,1,1,1,1,3,1,1,1,
  1,1,1,3,1,1,1,1,1,1,1,1,3,1,1,1,
  1,1,1,4,1,1,1,1,1,1,1,1,4,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

static const char inst[] = "\
NEG ??  ??  COM LSR ??  ROR ASR ASL ROL DEC ??  INC TST JMP CLR \
--  --  NOP SYNC??  ??  LBRALBSR??  DAA ORCC??  ANDCSEX EXG TFR \
BRA BRN BHI BLS BCC BLO BNE BEQ BVC BVS BPL BMI BGE BLT BGT BLE \
LEAXLEAYLEASLEAUPSHSPULSPSHUPULU??  RTS ABX RTI CWAIMUL ??  SWI \
NEGA??  ??  COMALSRA??  RORAASRAASLAROLADECA??  INCATSTA??  CLRA\
NEGB??  ??  COMBLSRB??  RORBASRBLSLBROLBDECB??  INCBTSTB??  CLRB\
NEG ??  ??  COM LSR ??  ROR ASR ASL ROL DEC ??  INC TST JMP CLR \
NEG ??  ??  COM LSR ??  ROR ASR ASL ROL DEC ??  INC TST JMP CLR \
SUBACMPASBCASUBDANDABITALDA ??  EORAADCAORA ADDACMPXBSR LDX ??  \
SUBACMPASBCASUBDANDABITALDA STA EORAADCAORA ADDACMPXJSR LDX STX \
SUBACMPASBCASUBDANDABITALDA STA EORAADCAORA ADDACMPXJSR LDX STX \
SUBACMPASBCASUBDANDABITALDA STA EORAADCAORA ADDACMPXJSR LDX STX \
SUBBCMPBSBCBADDDANDBBITBLDB ??  EORBADCBORB ADDBLDD ??  LDU ??  \
SUBBCMPBSBCBADDDANDBBITBLDB STB EORBADCBORB ADDBLDD STD LDU STU \
SUBBCMPBSBCBADDDANDBBITBLDB STB EORBADCBORB ADDBLDD STD LDU STU \
SUBBCMPBSBCBADDDANDBBITBLDB STB EORBADCBORB ADDBLDD STD LDU STU \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  LBRNLBHILBLSLBCCLBLOLBNELBEQLBVCLBVSLBPLLBMILBGELBLTLBGTLBLE\
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  SWI2\
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY ??  \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY STY \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY STY \
??  ??  ??  CMPD??  ??  ??  ??  ??  ??  ??  ??  CMPY??  LDY STY \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS STS \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS STS \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  LDS STS \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  SWI3\
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  CMPU??  ??  ??  ??  ??  ??  ??  ??  CMPS??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  \
??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ??  ";

static const char regi[] = { 'X', 'Y', 'U', 'S' };

static const char *exgi[] = { "D", "X", "Y", "U", "S", "PC", "??", "??", "A",
                              "B", "CC", "DP", "??", "??", "??", "??" };

static const char *pshsregi[] = { "PC", "U", "Y", "X", "DP", "B", "A", "CC" };
static const char *pshuregi[] = { "PC", "S", "Y", "X", "DP", "B", "A", "CC" };

/* disassemble one instruction at adress adr and return its size */

int dis6809(uint16_t adr, FILE *stream)
{
  int d = get_memb(adr);
  int s, sd, i;
  uint8_t pb;
  char reg;

  if (d == 0x10)
    d = get_memb(adr + 1) + 0x100; 
  
  if (d == 0x11)
    d = get_memb(adr + 1) + 0x200;
  
  s = size[d];
  
  fprintf( stream, "%04hX:  ", adr);

  sd = s;  
  if (mode[d] == 3) {
    switch (get_memb(adr + s - 1) & 0xf) {
      case  8:
      case 12:
        sd++;
        break;
      case  9:
      case 13:
      case 15:
        sd +=2;
        break;
    }
  }

  for (i = 0; i < sd; i++)
        fprintf( stream, "%02X", get_memb(adr+i));
  for (i = 6-sd; i > 0; i--)
    fputs("  ", stream);

  for (i = 0; i < 4; i++)
    fputc(inst[d * 4 + i], stream);
  fputc( ' ', stream);

  switch(mode[d]) {
  case 1:             /* immediate */
    fputs("#$", stream);
    if (s == 2)
      fputs(hex8str(get_memb(adr + 1)), stream);
    else
      fputs(hex16str(get_memw(adr + s - 2)), stream);
    break;
  case 2:             /* direct */
    fputs("<$", stream);
    fputs(hex8str(get_memb(adr + s - 1)), stream);
    break;
  case 3:             /* indexed */
    pb = get_memb(adr + s - 1);
    reg = regi[(pb >> 5) & 0x03];

    if (!(pb & 0x80)) {       /* n4,R */
      if (pb & 0x10)
        fprintf(stream, "-$%s,%c", hex8str(((pb & 0x0f) ^ 0x0f) + 1), reg);
      else
        fprintf(stream, "$%s,%c", hex8str(pb & 0x0f), reg);
    }
    else {
      if (pb & 0x10)
        fputc('[', stream);
      switch (pb & 0x0f) {
      case 0:                 /* ,R+ */
        fprintf(stream, ",%c+", reg);
        break;
      case 1:                 /* ,R++ */
        fprintf(stream, ",%c++", reg);
        break;
      case 2:                 /* ,-R */
        fprintf(stream, ",-%c", reg);
        break;
      case 3:                 /* ,--R */
        fprintf(stream, ",--%c", reg);
        break;
      case 4:                 /* ,R */
        fprintf(stream, ",%c", reg);
        break;
      case 5:                 /* B,R */
        fprintf(stream, "B,%c", reg);
        break;
      case 6:                 /* A,R */
        fprintf(stream, "A,%c", reg);
        break;
      case 8:                 /* n7,R */
        s += 1;
        fprintf(stream, "<$%s,%c", hex8str(get_memb(adr + s - 1)), reg);
        break;
      case 9:                 /* n15,R */
        s += 2;
        fprintf(stream, ">$%s,%c", hex16str(get_memw(adr + s - 2)), reg);
        break;
      case 11:                /* D,R */
        fprintf(stream, "D,%c", reg);
        break;
      case 12:                /* n7,PCR */
        s += 1;
        fprintf(stream, "<$%s,PCR", hex8str(get_memb(adr + s - 1)));
        break;
      case 13:                /* n15,PCR */
        s += 2;
        fprintf(stream, ">$%s,PCR", hex16str(get_memw(adr + s - 2)));
        break;
      case 15:                /* [n] */
        s += 2;
        fprintf(stream, "$%s", hex16str(get_memw(adr + s - 2)));
        break;
      default:
        fputs("??", stream);
        break; }
      if (pb & 0x10)
        fputc(']', stream);
    }
    break;
  case 4:          /* extended */
    fprintf(stream, ">$%s", hex16str(get_memw(adr + s - 2)));
    break;
  case 5:          /* inherent */
    pb = get_memb(adr + 1);
    switch (d) {
    case 0x1e: case 0x1f:              /* exg tfr */
      fprintf(stream, "%s,%s", exgi[(pb >> 4) & 0x0f], exgi[pb & 0x0f]);
      break; 
    case 0x1a: case 0x1c: case 0x3c:   /* orcc andcc cwai */
      fprintf(stream, "#$%s=%s", hex8str(pb), ccstr(pb));
      break;
    case 0x34:                         /* pshs */
      {
        int p = 0;

        for (i = 0; i < 8; i++) {
          if (pb & 0x80) {
            if (p)
              fputc(',', stream);
            fputs(pshsregi[i], stream);
            p = 1;
          }
          pb <<= 1;
        }
      }
      break;
    case 0x35:                         /* puls */
      {
        int p = 0;

        for (i = 7; i >= 0; i--) {
          if (pb & 0x01) {
            if (p)
              fputc(',', stream);
            fputs(pshsregi[i], stream);
            p = 1;
          }
          pb >>= 1;
        }
      }
      break;
    case 0x36:                         /* pshu */
      {
        int p = 0;

        for (i = 0; i < 8; i++) {
          if (pb & 0x80) {
            if (p)
              fputc(',', stream);
            fputs(pshuregi[i], stream);
            p = 1;
          }
          pb <<= 1;
        }
      }
      break;
    case 0x37:                         /* pulu */
      {
        int p = 0;

        for (i = 7; i >= 0; i--) {
          if (pb & 0x01) {
            if (p)
              fputc(',', stream);
            fputs(pshuregi[i], stream);
            p = 1;
          }
          pb >>= 1;
        }
      }
      break;
    }
    break; 
  case 6:             /* relative */
    {
      int16_t v;

      if (s == 2)
        v = (int16_t)(int8_t)get_memb(adr + 1);
      else
        v = (int16_t)get_memw(adr + s - 2);
      fprintf(stream, ">$%s", hex16str(adr + (uint16_t)s + v));
      break;
    }
  }
  fputc('\n', stream);

  return s;
}





