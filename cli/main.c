// #############################################################################
// #                      --- USB Funk CLI Tool ---                            #
// #############################################################################
// # main.c - Main function                                                    #
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
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <time.h>

 #include "main.h"
 #include "usbfunk.h"

 // Uncomment to enable test function
 //#define ENABLE_TEST


 // ***** Main function *****
 int main(int argc, char **argv)
  {
   int val;
   int wait = 0;

   val = usbfunk_init();
   // Device found?
   if(val!=USBFUNK_SUCCESS)
    {
     fprintf(stderr, ERRSTR_NOTFOUND);
     exit(1);
    }

   // Print usage by default
   val = USBFUNK_RNG_ERROR;

   // We need at least one argument to continue
   if(argc < 2)
    {
     print_usage(argv[0],(argc>=2?argv[1]:NULL));
     exit(1);
    }

   // Last argument --wait?
   if(strcasecmp(argv[(argc-1)], "--wait") == 0)
    {
     argc--;
     wait = 1;
    }

   // Query status
   if(strcasecmp(argv[1], "status") == 0)
    {
     if(argc!=2)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     val = usbfunk_status();
     if(val!=USBFUNK_DEV_ERROR)
      {
       printf("Sending: %s\n", val ? "yes" : "no");
      }
    }

   // Set repeat
   if(strcasecmp(argv[1], "setrepeat") == 0)
    {
     if(argc!=3)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     val = usbfunk_setrepeat(atoi(argv[2]));
    }

   // Set led
   if(strcasecmp(argv[1], "setled") == 0)
    {
     if(argc!=3)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     val = usbfunk_setled(atoi(argv[2]));
    }

   #ifdef ENABLE_TEST
   // Driver reliability test
   if(strcasecmp(argv[1], "test") == 0)
    {
     if(argc!=2)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     val = usbfunk_test(stderr);
    }
    #endif

   // PT22x2 devices with 12 bit dip switch (unknown)
   if(strcasecmp(argv[1], "pt0") == 0)
    {
     if(argc!=3)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     val = usbfunk_switch_0(atoi(argv[2]));
    }

   // 10 bit dip (most wireless controlled outlets use this)
   if(strcasecmp(argv[1], "pt2") == 0)
    {
     if(argc!=5)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     if(strlen(argv[3])==1&&(strcasecmp(argv[4], "off")==0||strcasecmp(argv[4], "on")==0))
      {
       val = usbfunk_switch_2(atoi(argv[2]), argv[3][0], (strcasecmp(argv[4], "off")==0?0:1));
      } else {
       val = USBFUNK_RNG_ERROR;
      }
    }

   // 8 bit dip (Duewi/Intertechno compatible)
   if(strcasecmp(argv[1], "pt4") == 0)
    {
     if(argc!=5)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     if(strlen(argv[2])==1&&(strcasecmp(argv[4], "off")==0||strcasecmp(argv[4], "on")==0))
      {
       val = usbfunk_switch_4(argv[2][0], atoi(argv[3]), (strcasecmp(argv[4], "off")==0?0:1));
      } else {
       val = USBFUNK_RNG_ERROR;
      }
    }

   // Elro Home Easy UK
   if(strcasecmp(argv[1], "he") == 0)
    {
     if(argc!=4)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     if(strcasecmp(argv[3], "off")==0||strcasecmp(argv[3], "on")==0)
      {
       val = usbfunk_switch_he(atoi(argv[2]), (strcasecmp(argv[3], "off")==0?0:1));
      } else {
       val = USBFUNK_RNG_ERROR;
      }
    }

   // PT2272 raw mode (raw write to tx buffer)
   if(strcasecmp(argv[1], "raw") == 0)
    {
     if(argc!=3)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     val = usbfunk_switch_raw(argv[2]);
    }

   // Heidemann HX doorbell
   if(strcasecmp(argv[1], "hx") == 0)
    {
     if(argc!=4)
      {
       print_usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     val = usbfunk_bell(atoi(argv[2]), atoi(argv[3]));
    }

   // Wait until transfer finished?
   if(wait==1)
    {
     while(usbfunk_status() == USBFUNK_BUSY)
      {
       usleep(25000); // 25ms
      }
    }

   // Close device
   usbfunk_close();

   // Error occured?
   if(val==USBFUNK_DEV_ERROR)
    {
     fprintf(stderr, ERRSTR_COMMUNICATION);
     exit(1);
    }
   if(val==USBFUNK_RNG_ERROR)
    {
     print_usage(argv[0],(argc>=2?argv[1]:NULL));
     exit(1);
    }

   // Success
   exit(0);
  }


 // ***** Show usage *****
 void print_usage(char *name, char *command)
  {
   if(command!=NULL)
    {
     if(strcasecmp(command, "pt0") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s pt0 [code]\n", name);
       fprintf(stderr, "    [code] = 0...4095 dec.\n");
       fprintf(stderr, "    Simply convert binary to decimal. (Dip 1,2,3,4,5 -> 1,2,4,8...)\n");
       fprintf(stderr, "    Assumes that a closed dip switch pulls the PT22x2 pin to low.\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "pt2") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s pt2 [housecode] [code] [status]\n", name);
       fprintf(stderr, "    [housecode] = 0...31 dec.\n");
       fprintf(stderr, "    [code] = A, B, C, D, E\n");
       fprintf(stderr, "    [status] = on / off\n");
       fprintf(stderr, "    Compatible to Kangtai, ELRO, ...\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "pt4") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s pt4 [housecode] [code] [status]\n", name);
       fprintf(stderr, "    [housecode] = A...P\n");
       fprintf(stderr, "    [code] = 1...16\n");
       fprintf(stderr, "    [status] = on / off\n");
       fprintf(stderr, "    Compatible to Duewi, Intertechno, HIG, ...\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "he") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s he [code] [status]\n", name);
       fprintf(stderr, "    [code] = 1...6000\n");
       fprintf(stderr, "    [status] = on / off\n");
       fprintf(stderr, "    Compatible Elro Home Easy UK (self-learning)\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "raw") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s raw [code]\n", name);
       fprintf(stderr, "    [code] = must be 12 chars long\n");
       fprintf(stderr, "             1: two long pulses\n");
       fprintf(stderr, "             f: short, long\n");
       fprintf(stderr, "             0: two short pulses\n");
       fprintf(stderr, "    As seen in PT22x2 datasheet. Use this to send own special codes.\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "hx") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s hx [code] [tone]\n", name);
       fprintf(stderr, "    [code] = 0...254\n");
       fprintf(stderr, "    [tone] = 0...7\n");
       fprintf(stderr, "    Compatible to Heidemann HX doorbells.\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "setrepeat") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s setrepeat [n]\n", name);
       fprintf(stderr, "    [n] = 1...254\n");
       fprintf(stderr, "    Repeat the wireless code [n] times. Value is stored in eeprom.\n");
       fprintf(stderr, "    Remember that the PT22x2 needs to receive at least\n");
       fprintf(stderr, "    2 correct codes to switch so [n] = 6 is a good choice.\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "setled") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s setled [n]\n", name);
       fprintf(stderr, "    [n] = 0: idle on / active off, 1: idle off / active on\n");
       fprintf(stderr, "    Invert the status LED. Value is stored in eeprom.\n");
       fprintf(stderr, "\n");
       return;
      }
     if(strcasecmp(command, "status") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s status\n", name);
       fprintf(stderr, "    Returns if tx is active.\n");
       fprintf(stderr, "\n");
       return;
      }
    }
   // If nothing else matched before, print general usage
   fprintf(stderr, "usage:\n");
   fprintf(stderr, "  %s [device] [arguments] .... send code\n", name);
   fprintf(stderr, "    [device] = pt0 - PT22x2 devices with 12 bit dip switch (unknown)\n");
   fprintf(stderr, "    [device] = pt2 - 10 bit dip (most wireless controlled outlets use this) \n");
   fprintf(stderr, "    [device] = pt4 - 8 bit dip (Duewi/Intertechno compatible)\n");
   fprintf(stderr, "    [device] = he  - Elro Home Easy UK (self-learning)\n");
   fprintf(stderr, "    [device] = raw - raw write to tx buffer\n");
   fprintf(stderr, "    [device] = hx  - Heidemann HX doorbell compatible\n");
   fprintf(stderr, "    -> enter device without arguments to get more information <-\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  %s setrepeat [n] ........... repeat code [n] times\n", name);
   fprintf(stderr, "    [n] = 1...254\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  %s setled [n] .............. invert status LED\n", name);
   fprintf(stderr, "    [n] = 0: idle on / active off, 1: idle off / active on\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  %s status .................. ask current status of tx\n", name);
   fprintf(stderr, "\n");
   #ifdef ENABLE_TEST
   fprintf(stderr, "  %s test .................... run driver reliability test\n", name);
   fprintf(stderr, "\n");
   #endif
   fprintf(stderr, "  Add parameter --wait to block until wireless transmission has finished.\n");
   fprintf(stderr, "\n");
  }