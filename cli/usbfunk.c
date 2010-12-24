// #############################################################################
// #                      --- USB Funk Host SW ---                             #
// #############################################################################
// # usbfunk.c - Main program                                                 #
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

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <usb.h>                    // this is libusb
 #include "opendevice.h"             // common code moved to separate module

 #include "../firmware/requests.h"   // custom request numbers
 #include "../firmware/usbconfig.h"  // device's VID/PID and names

 
 // Show usage
 static void usage(char *name)
  {
   fprintf(stderr, "usage:\n");
   fprintf(stderr, "  %s housecode code on ....... turn on plug\n", name);
   fprintf(stderr, "  %s housecode code off ...... turn off plug\n", name);
   fprintf(stderr, "  %s status .................. ask current status of tx\n", name);
   #if ENABLE_TEST
    fprintf(stderr, "  %s test ................... run driver reliability test\n", name);
   #endif // ENABLE_TEST
   fprintf(stderr, "  where housecode = 0...31 and code = A, B, C, D, E.\n"); 
  }


 // Main function
 int main(int argc, char **argv)
  {
   usb_dev_handle *handle = NULL;
   const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
   char vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
   char buffer[4];
   int cnt, vid, pid, isOn, housecode, code;

   usb_init();

   // we need at least one argument
   if(argc < 2)
    {
     usage(argv[0]);
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
     if((isOn = (strcasecmp(argv[3], "on") == 0)) || strcasecmp(argv[3], "off") == 0)
      {
       // we need 4 arguments here
       if(argc!=4)
        {
         usage(argv[0]);
         exit(1);
        }
       // start transmission
       housecode = atoi(argv[1]);
       if(housecode<0||housecode>31) housecode = 0;
       switch(argv[2][0])
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
          code = 0;
          break;                  
        }
       cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX, (housecode+(code<<8)), (isOn?1:2), buffer, 0, 5000);
       if(cnt < 0)
        {
         fprintf(stderr, "USB error: %s\n", usb_strerror());
        }
      #if ENABLE_TEST
      } else
       if(strcasecmp(argv[1], "test") == 0)
        {
         int i;
         srandomdev();
         for(i = 0; i < 50000; i++)
          {
           int value = random() & 0xffff, index = random() & 0xffff;
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
     usage(argv[0]);
     exit(1);
    }
   usb_close(handle);
   return 0;
  }
