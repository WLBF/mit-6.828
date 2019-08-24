# Lab 6: Network Driver

**Question**

1. How did you structure your transmit implementation? In particular, what do you do if the transmit ring is full?

   If the transmit ring is full, we simply discard the packet and return `-E_NO_MEM` . The output environment will suspended by call `sys_yield` if get the `-E_NO_MEN` error code. When the output environment running again it will try to resend the last packet.

2. How did you structure your receive implementation? In particular, what do you do if the receive queue is empty and a user environment requests the next incoming packet?

   If the receive queue is empty and a user environment requests the next incoming packet, the input environment loop retry receive call until next incomming packet.

3. What does the web page served by JOS's web server say?

   ```html
   <html>
   <head>
           <title>jhttpd on JOS</title>
   </head>
   <body>
           <center>
                   <h2>This file came from JOS.</h2>
                   <marquee>Cheesy web page!</marquee>
           </center>
   </body>
   </html>
   ```

4. How long approximately did it take you to do this lab?

   30 hours

