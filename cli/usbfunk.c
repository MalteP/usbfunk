// #############################################################################
// #                      --- USB Funk CLI Tool ---                            #
// #############################################################################
// # usbfunk.c - USBFunk communication functions                               #
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
#include <time.h>
#include <usb.h>                    // this is libusb

#include "../firmware/requests.h"   // custom request numbers
#include "../firmware/usbconfig.h"  // device's VID/PID and names

#include "usbfunk.h"

usb_dev_handle *handle;


// ***** Initialize libusb and find matching device *****
// Parameters: none
// Return val: false if failed, true if found
int usbfunk_init( void )
 {
   int len;
   struct usb_bus *bus;
   struct usb_device *dev;
   const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
   int vid = rawVid[1] * 256 + rawVid[0];
   int pid = rawPid[1] * 256 + rawPid[0];
   char vendor[] = {USB_CFG_VENDOR_NAME, '\0'}, product[] = {USB_CFG_DEVICE_NAME, '\0'};
   char usb_vendor[256], usb_product[256];
   // Init libusb
   usb_init();
   usb_find_busses();
   usb_find_devices();
   // Loop through all busses and devices
   for (bus = usb_get_busses(); bus; bus = bus->next)
    {
     for(dev = bus->devices; dev; dev = dev->next)
      {
       // Check vendor and product id
       if(dev->descriptor.idVendor == vid && dev->descriptor.idProduct == pid)
        {
         // Open device and check strings
         handle = usb_open(dev);
         if(!handle) continue;
         // Check vendor
         if(dev->descriptor.iManufacturer > 0)
          {
           len = usb_get_string_simple(handle, dev->descriptor.iManufacturer, usb_vendor, sizeof(usb_vendor));
           if(len < 0) continue;
           if(strcmp(vendor, usb_vendor)!=0)
            {
             usb_close(handle);
             continue;
            }
          }
         // Check product
         if(dev->descriptor.iProduct > 0)
          {
           len = usb_get_string_simple(handle, dev->descriptor.iProduct, usb_product, sizeof(usb_product));
           if(len < 0) continue;
           if(strcmp(product, usb_product)!=0)
            {
             usb_close(handle);
             continue;
            }
          }
         // Device found
         return USBFUNK_SUCCESS;
        }
      }
    }
  return USBFUNK_DEV_ERROR;
 }


// ***** PT2272 (12 bit code, no data) *****
// Parameters: code: range 0...4095
// Return val: false if anything failed, true if sent
int usbfunk_switch_0( int code )
 {
  int len;
  char buffer[4];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(code<0||code>4095) return USBFUNK_RNG_ERROR;
  len = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_0, code, 0, buffer, 0, 5000);
  if(len < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** PT2272A-M2/L2 (10 bit code, 2 bit data) *****
// Parameters: housecode: value in range 0...31, code: value in range A...E, state: true to switch on / false to switch off
// Return val: false if anything failed, true if sent
int usbfunk_switch_2( int housecode, char code, int state )
 {
  int len;
  char buffer[4];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(housecode<0||housecode>31) return USBFUNK_RNG_ERROR;
  if(code>='a'&&code<='e') code -= 32; // Convert to uppercase
  if(code<'A'||code>'E') return USBFUNK_RNG_ERROR;
  if(state<0||state>1) return USBFUNK_RNG_ERROR;
  len = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_2, (housecode+(((int)code-'A')<<8)), (state==0?0x02:0x01), buffer, 0, 5000);
  if(len < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** PT2272A-M4/L4 (8 bit code, 4 bit data) *****
// Parameters: housecode: value in range A...P, code: value in range 0...31, state: true to switch on / false to switch off
// Return val: false if anything failed, true if sent
int usbfunk_switch_4( char housecode, int code, int state )
 {
  int len;
  char buffer[4];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(housecode>='a'&&housecode<='p') housecode -= 32; // Convert to uppercase
  if(housecode<'A'||housecode>'P') return USBFUNK_RNG_ERROR;
  if(code<1||code>16) return USBFUNK_RNG_ERROR;
  if(state<0||state>1) return USBFUNK_RNG_ERROR;
  // code, housecode, switchcode (each in inverted logic!)
  len = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_4, ((15-((int)housecode-'A'))+((16-code)<<8)), (state==0?0x09:0x01), buffer, 0, 5000);
  if(len < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** Elro Home Easy UK *****
// Parameters: code: value in range 1...6000, state: true to switch on / false to switch off
// Return val: false if anything failed, true if sent
int usbfunk_switch_he( int code, int state )
 {
  int len;
  char buffer[4];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(code<1||code>6000) return USBFUNK_RNG_ERROR;
  if(state<0||state>1) return USBFUNK_RNG_ERROR;
  len = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_HE, code, (state==0?0x14:0x15), buffer, 0, 5000);
  if(len < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** PT2272 raw mode (raw write to tx buffer) *****
// Parameters: code: protocol code (12 chars) of 1/f/0
// Return val: false if anything failed, true if sent
int usbfunk_switch_raw( char* code )
 {
  int len, i;
  char buffer[4];
  int raw[3];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(code==NULL) return USBFUNK_RNG_ERROR;
  if(strlen(code)!=12)
   {
    return USBFUNK_RNG_ERROR;
   }
  raw[0]=0; raw[1]=0; raw[2]=0;
  for(i=0; i<strlen(code); i++)
   {
    switch(code[i])
     {
      case '1':
       raw[i/4] = raw[i/4] << 1;
       raw[i/4] |= 0x01;    // 1
       raw[i/4] = raw[i/4] << 1;
       raw[i/4] |= 0x01;    // 1
       break;
      case 'f':
      case 'F':
       raw[i/4] = raw[i/4] << 1;
       raw[i/4] &= ~0x01;   // 0
       raw[i/4] = raw[i/4] << 1;
       raw[i/4] |= 0x01;    // 1
       break;
      case '0':
       raw[i/4] = raw[i/4] << 1;
       raw[i/4] &= ~0x01;   // 0
       raw[i/4] = raw[i/4] << 1;
       raw[i/4] &= ~0x01;   // 0
       break;
      default:
       return USBFUNK_RNG_ERROR;
       break;
     }
   }
  len = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_RAW, (raw[0]+(raw[1]<<8)), raw[2], buffer, 0, 5000);
  if(len < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** Heidemann HX doorbell *****
// Parameters: code: value in range 0...254, sound: value in range 0..7
// Return val: false if anything failed, true if sent
int usbfunk_bell( int code, int sound )
 {
  int len;
  char buffer[4];
  int sound_r[] = { 0x0E, 0x0C, 0x0A, 0x06, 0x02, 0x01, 0x09, 0x0D };
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(sound<0||sound>7) return USBFUNK_RNG_ERROR;
  if(code<0||code>254) return USBFUNK_RNG_ERROR;
  len = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_TX_HX, (code+(sound_r[sound]<<8)), 0, buffer, 0, 5000);
  if(len < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** Set how many times the code should be sent *****
// Parameters: ntimes: value in range 0...254 (default: 5)
// Return val: false if anything failed, true if set
int usbfunk_setrepeat( int ntimes )
 {
  int cnt;
  char buffer[4];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(ntimes<1||ntimes>254) return USBFUNK_RNG_ERROR;
  cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_SETTIM, ntimes, 0, buffer, 0, 5000);
  if(cnt < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** Set if the led status should be inverted or not *****
// Parameters: inverted: false: on in standby, off if sending (default), true: led off in standby, on if sending
// Return val: false if anything failed, true if set
int usbfunk_setled( int inverted )
 {
  int cnt;
  char buffer[4];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  if(inverted<0||inverted>1) return USBFUNK_RNG_ERROR;
  cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, FUNK_RQ_SETLED, inverted, 0, buffer, 0, 5000);
  if(cnt < 0) return USBFUNK_DEV_ERROR;
  return USBFUNK_SUCCESS;
 }


// ***** Request the tx status from the device *****
// Parameters: none
// Return val: false if sending, true if standby
int usbfunk_status( void )
 {
  int cnt;
  char buffer[4];
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, FUNK_RQ_STATUS, 0, 0, buffer, sizeof(buffer), 5000);
  if(cnt < 1) return USBFUNK_DEV_ERROR;
  return (buffer[0]==0?USBFUNK_SUCCESS:USBFUNK_BUSY);
 }


// ***** Driver reliability test *****
// Parameters: f: Pointer to output file (or stdio, etc)
// Return val: false if anything failed, true if done
int usbfunk_test( FILE *f )
 {
  int cnt, i;
  char buffer[4];
  srand(time(NULL));
  for(i = 0; i < 50000; i++)
   {
    int value = rand() & 0xffff, index = rand() & 0xffff;
    int rxValue, rxIndex;
    if((i+1) % 100 == 0)
     {
      if(f!=NULL)
       {
        fprintf(f, "\rSending test data %05d / 50000", i+1);
        fflush(f);
       }
     }
    cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, FUNK_RQ_ECHO, value, index, buffer, sizeof(buffer), 5000);
    if(cnt < 0)
     {
      if(f!=NULL) fprintf(f, "\nUSB error in iteration %d: %s\n", i, usb_strerror());
      return USBFUNK_DEV_ERROR;
     } else
      if(cnt != 4)
       {
        if(f!=NULL) fprintf(f, "\nError in iteration %d: %d bytes received instead of 4\n", i, cnt);
        return USBFUNK_DEV_ERROR;
       }
    rxValue = ((int)buffer[0] & 0xff) | (((int)buffer[1] & 0xff) << 8);
    rxIndex = ((int)buffer[2] & 0xff) | (((int)buffer[3] & 0xff) << 8);
    if(rxValue != value || rxIndex != index)
     {
      if(f!=NULL)
      {
       fprintf(f, "\nData error in iteration %d:\n", i);
       fprintf(f, "rxValue = 0x%04x value = 0x%04x\n", rxValue, value);
       fprintf(f, "rxIndex = 0x%04x index = 0x%04x\n", rxIndex, index);
       return USBFUNK_DEV_ERROR;
      }
     }
   }
  if(f!=NULL) fprintf(f, "\nTest completed successfully.\n");
  return USBFUNK_SUCCESS;
 }


// ***** Close USB communication *****
// Parameters: none
// Return val: false if anything failed, true if done
int usbfunk_close( void )
 {
  if(handle==NULL) return USBFUNK_DEV_ERROR;
  usb_close(handle);
  handle = NULL;
  return USBFUNK_SUCCESS;
 }