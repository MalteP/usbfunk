// #############################################################################
// #                      --- USB Funk CLI Tool ---                            #
// #############################################################################
// # main.h - Main header                                                      #
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

#ifndef MAIN_H
 #define MAIN_H

 #define ERRSTR_NOTFOUND      "Could not find USB device!\n"
 #define ERRSTR_COMMUNICATION "Communication error!\n"

 void print_usage(char *name, char *command);

#endif