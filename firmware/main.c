// #############################################################################
// #                      --- USB Funk Firmware ---                            #
// #############################################################################
// # main.c - Main function                                                    #
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

 #include <avr/io.h>
 #include <avr/wdt.h>
 #include <avr/interrupt.h>                      // for sei()
 #include <util/delay.h>                         // for _delay_ms()

 #include <avr/pgmspace.h>                       // required by usbdrv.h
 #include "usbdrv.h"
 #include "requests.h"                           // The custom request numbers we use

 #include "portdef.h"
 #include "wireless.h"
 

 // --- USB interface ---
 usbMsgLen_t usbFunctionSetup(uchar data[8])
  {
   usbRequest_t *rq = (void *)data;
   static uchar dataBuffer[4];                   // buffer must stay valid when usbFunctionSetup returns 

   if(rq->bRequest == FUNK_RQ_ECHO)
    { 
     //used for reliability tests
     dataBuffer[0] = rq->wValue.bytes[0];
     dataBuffer[1] = rq->wValue.bytes[1];
     dataBuffer[2] = rq->wIndex.bytes[0];
     dataBuffer[3] = rq->wIndex.bytes[1];
     usbMsgPtr = dataBuffer;                     // tell the driver which data to return
     return 4;
    } else
     if(rq->bRequest == FUNK_RQ_TX)
      {
       // start tx
       wireless_switch(rq->wValue.bytes[0],rq->wValue.bytes[1],rq->wIndex.bytes[0]);
      } else
       if(rq->bRequest == FUNK_RQ_STATUS)
        {
         dataBuffer[0] = wireless_status();      // send in progress?
         usbMsgPtr = dataBuffer;                 // tell the driver which data to return
         return 1;                               // tell the driver to send 1 byte
        }
   return 0;                                     // default for not implemented requests: return no data back to host
  }


 // --- main() ---
 int __attribute__((noreturn)) main(void)
  {
   uint8_t i=0;
   wdt_enable(WDTO_1S);

   usbInit();
   usbDeviceDisconnect();                        // enforce re-enumeration, do this while interrupts are disabled!
   while(--i)                                    // fake USB disconnect for > 250 ms
    {
     wdt_reset();
     _delay_ms(1);
    }
   usbDeviceConnect();
   
   LED_PORT_DDR |= (1<<LED_BIT);                 // make the LED bit an output
      
   wireless_init();   
   sei();

   for(;;)                                       // main event loop 
    {
     wdt_reset();
     usbPoll();
     // make the led flash if send in progress
     if(wireless_status()==0) LED_PORT_OUTPUT &= ~(1<<LED_BIT); else LED_PORT_OUTPUT |= (1<<LED_BIT);
    }
  }
