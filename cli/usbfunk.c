// #############################################################################
// #                      --- USB Funk Host SW ---                             #
// #############################################################################
// # usbfunk.c - Main program                                                  #
// #############################################################################
// #              Version: 1.2 - Compiler: GCC                                 #
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
 #include <time.h>
 #include <usb.h>                    // this is libusb
 #include "opendevice.h"             // common code moved to separate module

 #include "../firmware/requests.h"   // custom request numbers
 #include "../firmware/usbconfig.h"  // device's VID/PID and names

 // Uncomment to enable test function
 //#define ENABLE_TEST

 // Show usage
 static void usage(char *name, char *command)
  {
   if(command!=NULL&&strcasecmp(command, "pt0") == 0)
    {
     fprintf(stderr, "usage:\n");
     fprintf(stderr, "  %s pt0 [code]\n", name);
     fprintf(stderr, "    [code] = 0...4095 dec.\n");
     fprintf(stderr, "    Simply convert binary to decimal. (Dip 1,2,3,4,5 -> 1,2,4,8...)\n");
     fprintf(stderr, "    Assumes that a closed dip switch pulls the PT22x2 pin to low.\n");
     fprintf(stderr, "\n");
    } else
     if(command!=NULL&&strcasecmp(command, "pt2") == 0)
      {
       fprintf(stderr, "usage:\n");
       fprintf(stderr, "  %s pt2 [housecode] [code] [status]\n", name);
       fprintf(stderr, "    [housecode] = 0...31 dec.\n");
       fprintf(stderr, "    [code] = A, B, C, D, E\n");
       fprintf(stderr, "    [status] = on / off\n");
       fprintf(stderr, "    Compatible to Kangtai, ELRO, ...\n");
       fprintf(stderr, "\n");
      } else
       if(command!=NULL&&strcasecmp(command, "pt4") == 0)
        {
         fprintf(stderr, "usage:\n");
         fprintf(stderr, "  %s pt4 [housecode] [code] [status]\n", name);
         fprintf(stderr, "    [housecode] = A...P\n");
         fprintf(stderr, "    [code] = 1...16\n");
         fprintf(stderr, "    [status] = on / off\n");
         fprintf(stderr, "    Compatible to Duewi, Intertechno, HIG, ...\n");
         fprintf(stderr, "\n");
        } else
         if(command!=NULL&&strcasecmp(command, "he") == 0)
          {
           fprintf(stderr, "usage:\n");
           fprintf(stderr, "  %s he [code] [status]\n", name);
           fprintf(stderr, "    [code] = 1...6000\n");
           fprintf(stderr, "    [status] = on / off\n");
           fprintf(stderr, "    Compatible Elro Home Easy UK (self-learning)\n");
           fprintf(stderr, "\n");
          } else
           if(command!=NULL&&strcasecmp(command, "raw") == 0)
            {
             fprintf(stderr, "usage:\n");
             fprintf(stderr, "  %s raw [code]\n", name);
             fprintf(stderr, "    [code] = must be 12 chars long\n");
             fprintf(stderr, "             1: two long pulses\n");
             fprintf(stderr, "             f: short, long\n");
             fprintf(stderr, "             0: two short pulses\n");
             fprintf(stderr, "    As seen in PT22x2 datasheet. Use this to send own special codes.\n");
             fprintf(stderr, "\n");
            } else
             if(command!=NULL&&strcasecmp(command, "hx") == 0)
              {
               fprintf(stderr, "usage:\n");
               fprintf(stderr, "  %s hx [code] [tone]\n", name);
               fprintf(stderr, "    [code] = 0...254\n");
               fprintf(stderr, "    [tone] = 0...7\n");
               fprintf(stderr, "    Compatible to Heidemann HX doorbells.\n");
               fprintf(stderr, "\n");
              } else
               if(command!=NULL&&strcasecmp(command, "setrepeat") == 0)
                {
                 fprintf(stderr, "usage:\n");
                 fprintf(stderr, "  %s setrepeat [n]\n", name);
                 fprintf(stderr, "    [n] = 1...254\n");
                 fprintf(stderr, "    Repeat the wireless code [n] times. Value is stored in eeprom.\n");
                 fprintf(stderr, "    Remember that the PT22x2 needs to receive at least\n");
                 fprintf(stderr, "    2 correct codes to switch so [n] = 6 is a good choice.\n");
                 fprintf(stderr, "\n");
                } else
                 if(command!=NULL&&strcasecmp(command, "setled") == 0)
                  {
                   fprintf(stderr, "usage:\n");
                   fprintf(stderr, "  %s setled [n]\n", name);
                   fprintf(stderr, "    [n] = 0: idle on / active off, 1: idle off / active on\n");
                   fprintf(stderr, "    Invert the status LED. Value is stored in eeprom.\n");
                   fprintf(stderr, "\n");
                  } else
                   if(command!=NULL&&strcasecmp(command, "status") == 0)
                    {
                     fprintf(stderr, "usage:\n");
                     fprintf(stderr, "  %s status\n", name);
                     fprintf(stderr, "    Returns if tx is active.\n");
                     fprintf(stderr, "\n");
                    } else {
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
                     #endif // ENABLE_TEST
                    }
  }

 // Main function
 int main(int argc, char **argv)
  {
   usb_dev_handle *handle = NULL;
   const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
   char vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
   char buffer[4];
   int cnt, vid, pid, switchcode, housecode, code, sound;
   int raw[3];
   int i;

   usb_init();

   // we need at least one argument
   if(argc < 2)
    {
     usage(argv[0],(argc>=2?argv[1]:NULL));
     exit(1);
    }

   // compute VID/PID from usbconfig.h so that there is a central source of information
   vid = rawVid[1] * 256 + rawVid[0];
   pid = rawPid[1] * 256 + rawPid[0];
   // The following function is in opendevice.c:
   if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0)
    {
     fprintf(stderr, "Could not find USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
     exit(1);
    }
   // Since we use only control endpoint 0, we don't need to choose a
   // configuration and interface. Reading device descriptor and setting a
   // configuration and interface is done through endpoint 0 after all.
   // However, newer versions of Linux require that we claim an interface
   // even for endpoint 0. Enable the following code if your operating system
   // needs it:
   #if 0
    int retries = 1, usbConfiguration = 1, usbInterface = 0;
    if(usb_set_configuration(handle, usbConfiguration) && showWarnings)
     {
      fprintf(stderr, "Warning: could not set configuration: %s\n", usb_strerror());
     }
    // now try to claim the interface and detach the kernel HID driver on
    // Linux and other operating systems which support the call.
    while((len = usb_claim_interface(handle, usbInterface)) != 0 && retries-- > 0)
     {
      #ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
       if(usb_detach_kernel_driver_np(handle, 0) < 0 && showWarnings)
        {
         fprintf(stderr, "Warning: could not detach kernel driver: %s\n", usb_strerror());
        }
      #endif
     }
   #endif

   if(strcasecmp(argv[1], "status") == 0)
    {
     if(argc!=2)
      {
       usage(argv[0],(argc>=2?argv[1]:NULL));
       exit(1);
      }
     cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, FUNK_RQ_STATUS, 0, 0, buffer, sizeof(buffer), 5000);
     if(cnt < 1)
      {
       if(cnt < 0)
        {
         fprintf(stderr, "USB error: %s\n", usb_strerror());
        } else {
         fprintf(stderr, "only %d bytes received.\n", cnt);
        }
      } else {
       printf("Sending: %s\n", buffer[0] ? "yes" : "no");
      }
    } else
     if(strcasecmp(argv[1], "setrepeat") == 0)
      {
       // we need 3 arguments here
       if(argc!=3)
        {
         usage(argv[0],(argc>=2?argv[1]:NULL));
         exit(1);
        }
       code = atoi(argv[2]);
       if(code<0||code>254)
        {
         usage(argv[0],(argc>=2?argv[1]:NULL));
         exit(1);
        }
       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_SETTIM, code, 0, buffer, 0, 5000);
       if(cnt < 0)
        {
         fprintf(stderr, "USB error: %s\n", usb_strerror());
        }
      } else
       if(strcasecmp(argv[1], "setled") == 0)
        {
         // we need 3 arguments here
         if(argc!=3)
          {
           usage(argv[0],(argc>=2?argv[1]:NULL));
           exit(1);
          }
         code = atoi(argv[2]);
         if(code<0||code>1)
          {
           usage(argv[0],(argc>=2?argv[1]:NULL));
           exit(1);
          }
         cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_SETLED, code, 0, buffer, 0, 5000);
         if(cnt < 0)
          {
           fprintf(stderr, "USB error: %s\n", usb_strerror());
          }
        } else
         if(strcasecmp(argv[1], "pt0") == 0)
          {
           // we need 3 arguments here
           if(argc!=3)
            {
             usage(argv[0],(argc>=2?argv[1]:NULL));
             exit(1);
            }
           code = atoi(argv[2]);
           if(code<0||code>4095)
            {
             usage(argv[0],(argc>=2?argv[1]:NULL));
             exit(1);
            }
           cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_0, code, 0, buffer, 0, 5000);
           if(cnt < 0)
            {
             fprintf(stderr, "USB error: %s\n", usb_strerror());
            }
          } else
           if(strcasecmp(argv[1], "pt2") == 0)
            {
             // we need 5 arguments here
             if(argc!=5)
              {
               usage(argv[0],(argc>=2?argv[1]:NULL));
               exit(1);
              }
             housecode = atoi(argv[2]);
             if(housecode<0||housecode>31) housecode = -1;
             switch(argv[3][0])
              {
               case 'A':
                code = 0;
                break;
               case 'B':
                code = 1;
                break;
               case 'C':
                code = 2;
                break;
               case 'D':
                code = 3;
                break;
               case 'E':
                code = 4;
                break;
               default:
                code = -1;
                break;
              }
             if(strcasecmp(argv[4], "on") == 0) switchcode=0x01;
              else if(strcasecmp(argv[4], "off") == 0) switchcode=0x02;
               else switchcode=-1;
             if(housecode==-1||code==-1||switchcode==-1)
              {
               usage(argv[0],(argc>=2?argv[1]:NULL));
               exit(1);
              }
             cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_2, (housecode+(code<<8)), switchcode, buffer, 0, 5000);
             if(cnt < 0)
              {
               fprintf(stderr, "USB error: %s\n", usb_strerror());
              }
            } else
             if(strcasecmp(argv[1], "pt4") == 0)
              {
               // we need 5 arguments here
               if(argc!=5)
                {
                 usage(argv[0],(argc>=2?argv[1]:NULL));
                 exit(1);
                }
               switch(argv[2][0])
                {
                 case 'A':
                  housecode = 15;
                  break;
                 case 'B':
                  housecode = 14;
                  break;
                 case 'C':
                  housecode = 13;
                  break;
                 case 'D':
                  housecode = 12;
                  break;
                 case 'E':
                  housecode = 11;
                  break;
                 case 'F':
                  housecode = 10;
                  break;
                 case 'G':
                  housecode = 9;
                  break;
                 case 'H':
                  housecode = 8;
                  break;
                 case 'I':
                  housecode = 7;
                  break;
                 case 'J':
                  housecode = 6;
                  break;
                 case 'K':
                  housecode = 5;
                  break;
                 case 'L':
                  housecode = 4;
                  break;
                 case 'M':
                  housecode = 3;
                  break;
                 case 'N':
                  housecode = 2;
                  break;
                 case 'O':
                  housecode = 1;
                  break;
                 case 'P':
                  housecode = 0;
                  break;
                 default:
                  housecode = -1;
                  break;
                }
               code = atoi(argv[3]);
               if(code<1||code>16) code = -1; else code = 16 - code;
               if(strcasecmp(argv[4], "on") == 0) switchcode=0x01;
                else if(strcasecmp(argv[4], "off") == 0) switchcode=0x09;
                 else switchcode=-1;
               if(housecode==-1||code==-1||switchcode==-1)
                {
                 usage(argv[0],(argc>=2?argv[1]:NULL));
                 exit(1);
                }
               // code, housecode, switchcode in inverted logic!
               cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_4, (housecode+(code<<8)), switchcode, buffer, 0, 5000);
               if(cnt < 0)
                {
                 fprintf(stderr, "USB error: %s\n", usb_strerror());
                }
              } else
               if(strcasecmp(argv[1], "he") == 0)
                {
                 // we need 4 arguments here
                 if(argc!=4)
                  {
                   usage(argv[0],(argc>=2?argv[1]:NULL));
                   exit(1);
                  }
                 code = atoi(argv[2]);
                 if(strcasecmp(argv[3], "on") == 0) switchcode=0x15;
                  else if(strcasecmp(argv[3], "off") == 0) switchcode=0x14;
                   else switchcode=-1;
                 if(code<1||code>6000||switchcode==-1)
                  {
                   usage(argv[0],(argc>=2?argv[1]:NULL));
                   exit(1);
                  }
                 cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_HE, code, switchcode, buffer, 0, 5000);
                 if(cnt < 0)
                  {
                   fprintf(stderr, "USB error: %s\n", usb_strerror());
                  }
                } else
                 if(strcasecmp(argv[1], "raw") == 0)
                  {
                   if(argc!=3)
                    {
                     usage(argv[0],(argc>=2?argv[1]:NULL));
                     exit(1);
                    }
                   if(strlen(argv[2])!=12)
                    {
                     usage(argv[0],(argc>=2?argv[1]:NULL));
                     exit(1);
                    }
                   raw[0]=0; raw[1]=0; raw[2]=0;
                   for(i=0; i<strlen(argv[2]); i++)
                    {
                     switch(argv[2][i])
                      {
                       case '1':
                        raw[i/4] = raw[i/4] << 1;
                        raw[i/4] |= 0x01;    // 1
                        raw[i/4] = raw[i/4] << 1;
                        raw[i/4] |= 0x01;    // 1
                        break;
                       case 'f':
                        raw[i/4] = raw[i/4] << 1;
                        raw[i/4] &= ~0x01;   // 0
                        raw[i/4] = raw[i/4] << 1;
                        raw[i/4] |= 0x01;    // 1
                        break;
                       case '0':
                        // same as default
                       default:
                        raw[i/4] = raw[i/4] << 1;
                        raw[i/4] &= ~0x01;   // 0
                        raw[i/4] = raw[i/4] << 1;
                        raw[i/4] &= ~0x01;   // 0
                        break;
                      }
                    }
                   cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_RAW, (raw[0]+(raw[1]<<8)), raw[2], buffer, 0, 5000);
                   if(cnt < 0)
                    {
                     fprintf(stderr, "USB error: %s\n", usb_strerror());
                    }
                  } else
                   if(strcasecmp(argv[1], "hx") == 0)
                    {
                     // we need 4 arguments here
                     if(argc!=4)
                      {
                       usage(argv[0],(argc>=2?argv[1]:NULL));
                       exit(1);
                      }
                     code = atoi(argv[2]);
                     if(code<0||code>254)
                      {
                       usage(argv[0],(argc>=2?argv[1]:NULL));
                       exit(1);
                      }
                     switch(atoi(argv[3]))
                      {
                       case 0:
                        sound = 0x0E;
                        break;
                       case 1:
                        sound = 0x0C;
                        break;
                       case 2:
                        sound = 0x0A;
                        break;
                       case 3:
                        sound = 0x06;
                        break;
                       case 4:
                        sound = 0x02;
                        break;
                       case 5:
                        sound = 0x01;
                        break;
                       case 6:
                        sound = 0x09;
                        break;
                       case 7:
                        sound = 0x0D;
                        break;
                       default:
                        sound = -1;
                        break;
                      }
                     if(sound==-1)
                      {
                       usage(argv[0],(argc>=2?argv[1]:NULL));
                       exit(1);
                      }
                     cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_HX, (code+(sound<<8)), 0, buffer, 0, 5000);
                     if(cnt < 0)
                      {
                       fprintf(stderr, "USB error: %s\n", usb_strerror());
                      }
                    #ifdef ENABLE_TEST
                    } else
                     if(strcasecmp(argv[1], "test") == 0)
                      {
                       int i;
                       srand(time(NULL));
                       for(i = 0; i < 50000; i++)
                        {
                         int value = rand() & 0xffff, index = rand() & 0xffff;
                         int rxValue, rxIndex;
                         if((i+1) % 100 == 0)
                          {
                           fprintf(stderr, "\r%05d", i+1);
                           fflush(stderr);
                          }
                         cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, FUNK_RQ_ECHO, value, index, buffer, sizeof(buffer), 5000);
                         if(cnt < 0)
                          {
                           fprintf(stderr, "\nUSB error in iteration %d: %s\n", i, usb_strerror());
                           break;
                          } else
                           if(cnt != 4)
                            {
                             fprintf(stderr, "\nerror in iteration %d: %d bytes received instead of 4\n", i, cnt);
                             break;
                            }
                         rxValue = ((int)buffer[0] & 0xff) | (((int)buffer[1] & 0xff) << 8);
                         rxIndex = ((int)buffer[2] & 0xff) | (((int)buffer[3] & 0xff) << 8);
                         if(rxValue != value || rxIndex != index)
                          {
                           fprintf(stderr, "\ndata error in iteration %d:\n", i);
                           fprintf(stderr, "rxValue = 0x%04x value = 0x%04x\n", rxValue, value);
                           fprintf(stderr, "rxIndex = 0x%04x index = 0x%04x\n", rxIndex, index);
                          }
                        }
                       fprintf(stderr, "\nTest completed.\n");
                    #endif // ENABLE_TEST
                    } else {
                     usage(argv[0],(argc>=2?argv[1]:NULL));
                     exit(1);
                    }
   usb_close(handle);
   return 0;
  }
