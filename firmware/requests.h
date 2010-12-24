// #############################################################################
// #                      --- USB Funk Firmware ---                            #
// #############################################################################
// # requests.h - USB class command definitions                                #
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

#ifndef REQUESTS_H
 #define REQUESTS_H

 // Request that the device sends back wValue and wIndex. This is used with
 // random data to test the reliability of the communication.
 #define FUNK_RQ_ECHO 0
 
 // Uses Control Endpoint 0 to submit housecode, code and on/off state to the
 // device which sends it immediately
 // wValue: low byte: house code, high byte: code, 
 // wIndex: low byte: 1 = on, 2 = off
 #define FUNK_RQ_TX 1
 
 // Request the tx status from the device. Value is returned in buffer[0],
 // 0 = send in progress.
 #define FUNK_RQ_STATUS 2
 
#endif
