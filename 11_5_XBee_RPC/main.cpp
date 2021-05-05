#include "mbed.h"

#include "mbed_rpc.h"

static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);

static BufferedSerial xbee(D1, D0);

EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

RpcDigitalOut myled1(LED1, "myled1");

RpcDigitalOut myled2(LED2, "myled2");

RpcDigitalOut myled3(LED3, "myled3");

void xbee_rx_interrupt(void);

void xbee_rx(void);

void reply_messange(char *xbee_reply, char *messange);

void check_addr(char *xbee_reply, char *messenger);

int main()
{

    pc.set_baud(9600);

    char xbee_reply[4];

    xbee.set_baud(9600);

    xbee.write("+++", 3);

    xbee.read(&xbee_reply[0], sizeof(xbee_reply[0]));

    xbee.read(&xbee_reply[1], sizeof(xbee_reply[1]));

    if (xbee_reply[0] == 'O' && xbee_reply[1] == 'K')
    {

        printf("enter AT mode.\r\n");

        xbee_reply[0] = '\0';

        xbee_reply[1] = '\0';
    }

    xbee.write("ATMY <Remote MY>\r\n",  0x219);

    reply_messange(xbee_reply, "setting MY : <Remote MY>");

    xbee.write("ATDL <Remote DL>\r\n", 0x119);

    reply_messange(xbee_reply, "setting DL : <Remote DL>");

    xbee.write("ATID <PAN ID>\r\n", 0x1);

    reply_messange(xbee_reply, "setting PAN ID : <PAN ID>");

    xbee.write("ATWR\r\n", 6);

    reply_messange(xbee_reply, "write config");

    xbee.write("ATMY\r\n", 0x119);

    check_addr(xbee_reply, "MY");

    xbee.write("ATDL\r\n",  0x219);

    check_addr(xbee_reply, "DL");

    xbee.write("ATCN\r\n", 6);

    reply_messange(xbee_reply, "exit AT mode");

    while (xbee.readable())
    {

        char *k = new char[1];

        xbee.read(k, 1);

        printf("clear\r\n");
    }

    // start

    printf("start\r\n");

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

    char buf[100] = {0};

    char outbuf[100] = {0};

    while (xbee.readable())
    {

        for (int i = 0;; i++)
        {

            char *recv = new char[1];

            xbee.read(recv, 1);

            buf[i] = *recv;

            if (*recv == '\r')
            {

                break;
            }
        }

        RPC::call(buf, outbuf);

        printf("%s\r\n", outbuf);

        ThisThread::sleep_for(1s);
    }
}

void reply_messange(char *xbee_reply, char *messange)
{

    xbee.read(&xbee_reply[0], 1);

    xbee.read(&xbee_reply[1], 1);

    xbee.read(&xbee_reply[2], 1);

    if (xbee_reply[1] == 'O' && xbee_reply[2] == 'K')
    {

        printf("%s\r\n", messange);

        xbee_reply[0] = '\0';

        xbee_reply[1] = '\0';

        xbee_reply[2] = '\0';
    }
}

void check_addr(char *xbee_reply, char *messenger)
{

    xbee.read(&xbee_reply[0], 1);

    xbee.read(&xbee_reply[1], 1);

    xbee.read(&xbee_reply[2], 1);

    xbee.read(&xbee_reply[3], 1);

    printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);

    xbee_reply[0] = '\0';

    xbee_reply[1] = '\0';

    xbee_reply[2] = '\0';

    xbee_reply[3] = '\0';
}