// #############################################################################
// #                      --- USB Funk Firmware ---                            #
// #############################################################################
// # requests.h - USB class command definitions                                #
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

#ifndef REQUESTS_H
 #define REQUESTS_H

 // Request that the device sends back wValue and wIndex. This is used with
 // random data to test the reliability of the communication.
 #define FUNK_RQ_ECHO   0
 
 // We use Control Endpoint 0 to submit data to the
 // device which converts and sends it immediately

 // --- PT2272 (12 bit code, no data) ---
 // wValue: code
 #define FUNK_RQ_TX_0   1
 
 // --- PT2272A-M2/L2 (10 bit code, 2 bit data) ---
 // wValue: low byte: house code, high byte: code, 
 // wIndex: low byte: 1 = on, 2 = off
 #define FUNK_RQ_TX_2   2
 
 // --- PT2272A-M4/L4 (8 bit code, 4 bit data) ---
 // wValue: low byte: house code, high byte: code, 
 // wIndex: low byte: 1 = on, 2 = off
 #define FUNK_RQ_TX_4   3

 // --- PT2272 raw mode (raw write to tx buffer) ---
 // wValue: low byte: buffer LSB, high byte: buffer,
 // wIndex: low byte: buffer MSB
 #define FUNK_RQ_TX_RAW 4 
 
 // --- Heidemann HX doorbell ---
 // wValue: low byte: code, high byte: tone
 #define FUNK_RQ_TX_HX  5 

 // Set how many times the code should be sent
 // wValue: low byte: n
 #define FUNK_RQ_SETTIM 6
 
 // Request the tx status from the device. Value is returned in buffer[0],
 // 0 = send in progress.
 #define FUNK_RQ_STATUS 7

 // Set if the led status should be inverted or not
 // wValue: low byte: 0 or 1
 #define FUNK_RQ_SETLED 8
 
 // --- Elro Home Easy ---
 // wValue: code
 // wIndex: status
 #define FUNK_RQ_TX_HE  9

#endif
