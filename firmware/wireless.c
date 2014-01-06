// #############################################################################
// #                      --- USB Funk Firmware ---                            #
// #############################################################################
// # wireless.c - Wireless functions                                           #
// #############################################################################
// #              Version: 1.2 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #  (c) 2011-2014 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de   #
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
 volatile uint8_t mode;

 // Array with powers and multiples of powers of three,
 // needed for ELRO Home Easy ternary code conversion
 const uint16_t powers_of_three[16] =
  {
   4374, 2187, 1458, 729, 486, 243, 162, 81, 54, 27, 18, 9, 6, 3, 2, 1
  };


 // --- Initialize wireless ---
 void wireless_init( void )
  {
   TX_PORT_DDR |= (1<<TX_BIT);
   TX_PORT_OUTPUT &= ~(1<<TX_BIT);

   wireless_clear();
   n_times = N_TIMES_DEFAULT;

   TCCR0B = (1<<CS00) | (1<<CS01);               // Setup prescaler (12MHz / 64)
   TIMSK0 |= (1<<TOIE0);                         // Setup interrupt   
    
   // Remember to call sei() after initialization!
  }


 // --- Clear wireless buffer ---
 void wireless_clear( void )
  {
   repeat=0;
   mask=0;
   counter=0;
   mode=MODE_PT;
   buffer[0] = 0;
   buffer[1] = 0;
   buffer[2] = 0;
  }


 // --- Translate bits for wireless plug to tx pattern ---
 // --- for PT2272 (12 bit code, no data)              --- 
 void wireless_switch_0( uint16_t code )
  {
   uint8_t i;
   // Calculate pulses for buffer: 0 -> short, 1 -> long
   // where 00 = logical 0, 01 = floating, 11 = logical 1
   for(i=0; i<24; i++)
    {
     if(code&(1<<(i/2))) 
      {
       // 1 which means switch to ground = 00
       buffer[i/8] &= ~(0x80>>(i%8));
      } else {
       // 0 which means floating = 01 
       if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
      }      
    }
   // Start transmission in timer interrupt
   mode=MODE_PT;
   repeat=CODE_REPEAT;
  }


 // --- Translate bits for wireless plug to tx pattern ---
 // --- for PT2272A-M2/L2 (10 bit code, 2 bit data)    --- 
 void wireless_switch_2( uint8_t housecode, uint8_t code, uint8_t state )
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
   // Start transmission in timer interrupt
   mode=MODE_PT;
   repeat=CODE_REPEAT;
  }


 // --- Translate bits for wireless plug to tx pattern ---
 // --- for PT2272-M4/L4 (8 bit code, 4 bit data)      --- 
 void wireless_switch_4( uint8_t housecode, uint8_t code, uint8_t state )
  {
   uint8_t i;
   // Calculate pulses for buffer: 0 -> short, 1 -> long
   // where 00 = logical 0, 01 = floating, 11 = logical 1
   for(i=0; i<24; i++)
    {
     if(i<=7)                                    // housecode 0-16 (A-P)
      {
       if(housecode&(1<<(i/2))) 
        {
         // 1 which means switch to ground = 00
         buffer[i/8] &= ~(0x80>>(i%8));
        } else {
         // 0 which means floating = 01 
         if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
        }      
      } else if(i<=15)                           // code 0-16
       {
        if(code&(1<<((i-8)/2))) 
         {
          // 1 which means switch to ground = 00
          buffer[i/8] &= ~(0x80>>(i%8));
         } else {
          // 0 which means floating = 01 
          if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
         }
       } else {                                  // state
        if(state&(1<<((i-16)/2))) 
         {
          // 1 which means switch to ground = 00
          buffer[i/8] &= ~(0x80>>(i%8));
         } else {
          // 0 which means floating = 01 
          if(i&0x01) buffer[i/8] |= (0x80>>(i%8)); else buffer[i/8] &= ~(0x80>>(i%8));
         }        
       }
    }
   // Start transmission in timer interrupt
   mode=MODE_PT;
   repeat=CODE_REPEAT;
  }


 // --- Translate decimal value for wireless plug to   ---
 // --- ternary tx pattern for Elro Home Easy UK       ---
 // --- (8 bit code, 4 bit data)                       ---
 // --- Conversion code by Yannic "Yan Nic" Schröder   ---
 void wireless_switch_he( uint16_t code, uint8_t state )
  {
   uint8_t i;
   // Result of the test if a certain power of three fits into the decimal number
   int16_t result;
   // Clear buffer
   buffer[0] = 0;
   buffer[1] = 0;
   // Add two bits of pulsecode for every power of three
   for (i=0; i<16; i+=2)
    {
     // Make space for new bits
     buffer[i/8] = (buffer[i/8]<<2);
     // Subtract power of three
     result = code - powers_of_three[i];
     // If (2*3^i) does not fit into the decimal number
     if (result < 0)
      {
       result = code - powers_of_three[i+1];
       // If (1*3^i) does not fit into the decimal number
       if (result < 0)
        {
         // Do nothing
        } else {
         // Set the lower two bits of the pulsecode to 0b11
         buffer[i/8] |= 0x3;
         // Use the current result for the next loop
         code = (uint16_t)result;
        }
      } else {
       // Set the lower two bits of the pulsecode to 0b01
       buffer[i/8] |= 0x1;
       // Use the current result for the next loop
       code = (uint16_t)result;
      }
    }
   // Shift and write last four bits
   buffer[2] = state;
   // Start transmission in timer interrupt
   mode=MODE_PT;
   repeat=CODE_REPEAT;
  }


 // --- Write directly to tx buffer. May be used to    ---
 // --- create more special codes and to send logical  ---
 // --- 1 (11) which is not used by the most devices   ---
 void wireless_switch_raw( uint8_t code_0, uint8_t code_1, uint8_t code_2 )
  {
   buffer[0] = code_0;
   buffer[1] = code_1;
   buffer[2] = code_2;
   // Start transmission in timer interrupt
   mode=MODE_PT;
   repeat=CODE_REPEAT;  
  }


 // --- Copy bits for wireless bell to tx pattern      ---
 // --- for Heidemann HX compatible doorbells          ---
 // --- (8 bit code, 4 bit sound)                      ---
 void wireless_bell( uint8_t code, uint8_t sound )
  {
   buffer[0] = code;
   buffer[1] = (sound<<4);  
   // Start transmission in timer interrupt
   mode=MODE_HX;
   repeat=HX_REPEAT;  
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
 ISR( TIMER0_OVF_vect, ISR_NOBLOCK )
  {
   if(repeat>0)
    {
     switch(mode)
      {
       case MODE_PT:
        // BEGIN PT CODE
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
       break;
      case MODE_HX:
       // BEGIN HX CODE
       if(counter==0||mask==0) mask=0x80;
       if(counter<2)                                // Starts /w sync pulse
        {
         if(counter==0)
          {
           TX_PORT_OUTPUT &= ~(1<<TX_BIT);
           TCNT0 = HX_LONG_PULSE;
          } else {
           TX_PORT_OUTPUT |= (1<<TX_BIT);
           TCNT0 = HX_SHORT_PULSE;        
          }        
        } else {
         if(counter<=26)
          {
           if(!(counter&0x01))                      // Last bit: 1 = TX on, 0 = TX off
            {
             TX_PORT_OUTPUT &= ~(1<<TX_BIT);
             if(buffer[(counter-2)/16]&mask)        // First bit in buffer 0 = short break, 1 = long break
              {
               TCNT0 = HX_SHORT_PULSE;
              } else {
               TCNT0 = HX_LONG_PULSE;        
              }
            } else {
             TX_PORT_OUTPUT |= (1<<TX_BIT);
             if(buffer[(counter-2)/16]&mask)        // First bit in buffer 0 = long pulse, 1 = short pulse
              {
               TCNT0 = HX_LONG_PULSE;
              } else {
               TCNT0 = HX_SHORT_PULSE;        
              }       
             mask=mask>>1;
            }
          } else {
           TX_PORT_OUTPUT &= ~(1<<TX_BIT);          // Sync delay after code
           TCNT0 = HX_LONG_PULSE;           
          }
        }
       if(counter>=32)
        {
         counter = 0;
         repeat--;
        } else {
         counter++;
        }      
       break;
      default:
       repeat = 0;  
       break;  
      }
    }
  }
