// #############################################################################
// #                      --- USB Funk Firmware ---                            #
// #############################################################################
// # wireless.h - Wireless functions (Header)                                  #
// #############################################################################
// #              Version: 1.1 - Compiler: AVR-GCC 4.5.0 (Linux)               #
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

#ifndef WIRELESS_H
 #define WIRELESS_H

 volatile uint8_t n_times; 
 #define N_TIMES_DEFAULT 5

 // PT2262/2272
 #define PRELOAD_SHORT_PULSE 196                 // ~320us
 #define PRELOAD_LONG_PULSE   78                 // ~950us
 #define CODE_REPEAT     n_times                 // Send code n times

 // Heidemann HX
 #define HX_SHORT_PULSE      196                 // ~320us
 #define HX_LONG_PULSE       136                 // ~640us
 #define HX_REPEAT       n_times                 // Send code n times
 
 // Modes
 #define MODE_PT               0                 // PT2272
 #define MODE_HX               1                 // Heidemann HX

 void wireless_init( void );
 void wireless_switch_0( uint16_t code );
 void wireless_switch_2( uint8_t housecode, uint8_t code, uint8_t state );
 void wireless_switch_4( uint8_t housecode, uint8_t code, uint8_t state );
 void wireless_switch_raw( uint8_t code_0, uint8_t code_1, uint8_t code_2 );
 void wireless_bell( uint8_t code, uint8_t sound );
 uint8_t wireless_status( void );
  
#endif



