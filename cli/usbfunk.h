// #############################################################################
// #                      --- USB Funk CLI Tool ---                            #
// #############################################################################
// # usbfunk.h - USBFunk communication header                                  #
// #############################################################################
// #                       Version: 1.2 - Compiler: GCC                        #
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

#include <stdio.h>

#ifndef USBFUNK_H
 #define USBFUNK_H

 #define USBFUNK_SUCCESS   0 // Everything fine
 #define USBFUNK_BUSY      1 // Device busy
 #define USBFUNK_DEV_ERROR 2 // Communication failed
 #define USBFUNK_RNG_ERROR 3 // Parameter out of range

 int usbfunk_init( void );
 int usbfunk_switch_0( int code );
 int usbfunk_switch_2( int housecode, char code, int state );
 int usbfunk_switch_4( char housecode, int code, int state );
 int usbfunk_switch_he( int code, int state );
 int usbfunk_switch_raw( char* code );
 int usbfunk_bell( int code, int sound );
 int usbfunk_setrepeat( int ntimes );
 int usbfunk_setled( int inverted );
 int usbfunk_status( void );
 int usbfunk_test( FILE *f );
 int usbfunk_close( void );

#endif
