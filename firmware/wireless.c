// #############################################################################
// #                      --- USB Funk Firmware ---                            #
// #############################################################################
// # wireless.c - Wireless functions                                           #
// #############################################################################
// #              Version: 1.0 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #  (c) 2011 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@maltepoeggel.de   #
// #############################################################################
// #  This program is free software; you can redistribute it and/or modify it  #
// #   under the terms of the GNU General Public License as published by the   #
// #        Free Software Foundation; either version 3 of the License,         #
// #                  or (at your option) any later version.                   #
// #                                                                           #
// #      This program is distributed in the hope that it will be useful,      #
// #      but WITHOUT ANY WARRANTY; without even the implied warranty of       #
// #           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            #
// #           See the GNU General Public License for more details.            #
// #                                                                           #
// #  You should have received a copy of the GNU General Public License along  #
// #      with this program; if not, see <http://www.gnu.org/licenses/>.       #
// #############################################################################

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "wireless.h"
 #include "portdef.h"


 // Global variables
 volatile uint8_t counter;
 volatile uint8_t mask;
 volatile uint8_t buffer[3];
 volatile uint8_t repeat;


 // --- Initialize wireless ---
 void wireless_init( void )
  {
   TX_PORT_DDR |= (1<<TX_BIT);
   TX_PORT_OUTPUT &= ~(1<<TX_BIT);

   repeat=0;
   mask=0;
   counter=0;

   TCCR0B = (1<<CS00) | (1<<CS01);               // Setup prescaler (12MHz / 64)
   TIMSK0 |= (1<<TOIE0);                         // Setup interrupt   
    
   // Remember to call sei() after initialization!
  }


 // --- Translate bits for wireless plug to tx pattern ---
 void wireless_switch( uint8_t housecode, uint8_t code, uint8_t state )
  {
   uint8_t i;
   // Calculate pulses for buffer: 0 -> short, 1 -> long
   // where 00 = logical 0, 01 = floating, 11 = logical 1
   for(i=0; i<24; i++)
    {
     if(i<=9)                                    // housecode
      {
       if(housecode&(1<<(i/2))) 
        {
         // 1 which means switch to ground = 00
         buffer[i/8] &= ~(0x80>>(i%8));
        } else {
         // 0 which means floating = 01 
         if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
        }      
      } else if(i<=19)                           // code 0-4 (A/B/C/D/E)
       {
        if(code==((i-10)/2)) 
         {
          // 1 which means switch to ground = 00
          buffer[i/8] &= ~(0x80>>(i%8));
         } else {
          // 0 which means floating = 01 
          if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
         }
       } else {                                  // state
        if(state&(1<<((i-20)/2))) 
         {
          // 1 which means switch to ground = 00
          buffer[i/8] &= ~(0x80>>(i%8));
         } else {
          // 0 which means floating = 01 
          if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
         }        
       }
    }
   // Start transmission (5 times) in timer interrupt
   repeat=5;
  }


 // --- Returns status: are we sending? ---
 uint8_t wireless_status( void )
  {
   if(repeat>0)
    {
     return 1;
    } else {
     return 0;
    }
  }


 // --- Interrupt for protocol waveform ---
 ISR( TIMER0_OVF_vect ) 
  {
   if(repeat>0)
    {
     if(counter==0||mask==0) mask=0x80;
     if(counter<48)                              // Data or sync?
      {  
       if(!(counter&0x01))                       // Last bit: 0 = TX on, 1 = TX off
        {
         TX_PORT_OUTPUT |= (1<<TX_BIT);
         if(buffer[counter/16]&mask)             // First bit in buffer 0 = short pulse, 1 = long pulse
          {
           TCNT0 = PRELOAD_LONG_PULSE;
          } else {
           TCNT0 = PRELOAD_SHORT_PULSE;        
          }
        } else {
         TX_PORT_OUTPUT &= ~(1<<TX_BIT);
         if(buffer[counter/16]&mask)             // First bit in buffer 0 = long break, 1 = short break
          {
           TCNT0 = PRELOAD_SHORT_PULSE;
          } else {
           TCNT0 = PRELOAD_LONG_PULSE;        
          }       
         mask=mask>>1;                           // Next bit in next interrupt!
        } 
      } else {
       if(counter==48)
        {
         TX_PORT_OUTPUT |= (1<<TX_BIT);          // Sync pattern short pulse
         TCNT0 = PRELOAD_SHORT_PULSE;
        } else
         if(counter==49)
          {
           TX_PORT_OUTPUT &= ~(1<<TX_BIT);       // Sync pattern break
          }
       TCNT0 = PRELOAD_SHORT_PULSE;
      }
     if(counter>=80)
      {
       counter = 0;
       repeat--;
      } else {
       counter++;
      }
    }
  }
