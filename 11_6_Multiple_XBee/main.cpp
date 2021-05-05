#include "mbed.h"



DigitalOut redLED(LED1);

static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);

static BufferedSerial xbee(D1, D0);


EventQueue queue(32 * EVENTS_EVENT_SIZE);


Thread t;


void xbee_rx_interrupt(void);

void xbee_rx(void);

void reply_messange(char *xbee_reply, char *messange);


int main(){


pc.set_baud(9600);


char xbee_reply[3];


// XBee setting


xbee.set_baud(9600);


xbee.write("+++", 3);

xbee.read(&xbee_reply[0], sizeof(xbee_reply[0]));

xbee.read(&xbee_reply[1], sizeof(xbee_reply[1]));


if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){


   printf("enter AT mode.\r\n");

   xbee_reply[0] = '\0';

   xbee_reply[1] = '\0';


}



// SLAVE_MY should be different between Slave A and Slave B


xbee.write("ATMY 0x240\r\n", 12);

reply_messange(xbee_reply, "setting MY : 0x240");


xbee.write("ATDL 0x0\r\n", 10);

reply_messange(xbee_reply, "setting DL : 0x0");


xbee.write("ATID 0x1\r\n", 15);

reply_messange(xbee_reply, "setting PAN ID : 0x1");


xbee.write("ATWR\r\n", 6);

reply_messange(xbee_reply, "write config");


xbee.write("ATCN\r\n", 6);

reply_messange(xbee_reply, "exit AT mode");


while(xbee.readable()){

   char *k = new char[1];

   xbee.read(k,1);

   printf("clear\r\n");

}



// start


printf("start\r\n");


redLED = 1;


t.start(callback(&queue, &EventQueue::dispatch_forever));


// Setup a serial interrupt function of receiving data from xbee


xbee.set_blocking(false);

xbee.sigio(mbed_event_queue()->event(xbee_rx_interrupt));


}



void xbee_rx_interrupt(void)

{

   queue.call(&xbee_rx);

}



void xbee_rx(void)


{


while(xbee.readable()){


   //static char receive_data = xbee.getc();

   char *receive_data = new char[1];

   xbee.read(receive_data, 1);

   printf("Receive data %c\r\n", receive_data);

   // receive_data=='a' for Slave A

   // receive_data=='b' for Slave B

   if (*receive_data == 'a') {


      redLED = 0;

      // "A received the data." for Slave A

      // "B received the data." for Slave B

      xbee.write("A received the data.", 20);


   }


   ThisThread::sleep_for(500ms);

   redLED = 1;

   receive_data = 0;


}

}



void reply_messange(char *xbee_reply, char *messange){


xbee.read(&xbee_reply[0], 1);

xbee.read(&xbee_reply[1], 1);

xbee.read(&xbee_reply[2], 1);

if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){

   printf("%s\r\n", messange);

   xbee_reply[0] = '\0';

   xbee_reply[1] = '\0';

   xbee_reply[2] = '\0';

}


}