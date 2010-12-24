// #############################################################################
// #                      --- USB Funk Firmware ---                            #
// #############################################################################
// # wireless.h - Wireless functions (Header)                                  #
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

#ifndef WIRELESS_H
 #define WIRELESS_H

 #define PRELOAD_SHORT_PULSE 196                 // ~320us
 #define PRELOAD_LONG_PULSE   78                 // ~950us

 void wireless_init( void );
 void wireless_switch( uint8_t housecode, uint8_t code, uint8_t state );
 uint8_t wireless_status( void );
  
#endif



