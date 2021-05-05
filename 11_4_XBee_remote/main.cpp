#include "mbed.h"

using namespace std;

static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);

static BufferedSerial xbee(D1, D0);

EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

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

    xbee.write("ATMY <Remote MY>\r\n", 0x219);

    reply_messange(xbee_reply, "setting MY : <Remote MY>");

    xbee.write("ATDL <Remote DL>\r\n",  0x119);

    reply_messange(xbee_reply, "setting DL : <Remote DL>");

    xbee.write("ATID <PAN ID>\r\n", 0x1);

    reply_messange(xbee_reply, "setting PAN ID : <PAN ID>");

    xbee.write("ATWR\r\n", 6);

    reply_messange(xbee_reply, "write config");

    xbee_reply[0] = '\0';

    xbee_reply[1] = '\0';

    xbee_reply[2] = '\0';

    xbee_reply[3] = '\0';

    xbee.write("ATMY\r\n", 0x119);

    check_addr(xbee_reply, "MY");

    xbee.write("ATDL\r\n",  0x219);

    check_addr(xbee_reply, "DL");

    xbee.write("ATCN\r\n", 6);

    reply_messange(xbee_reply, "exit AT mode");

    while (xbee.readable())
    { //used to clear buffer

        char *c = new char[1];

        xbee.read(c, 1);

        printf("clear \r\n");
    }

    //start send

    printf("start\r\n");

    t.start(callback(&queue, &EventQueue::dispatch_forever));

    xbee.set_blocking(false);

    xbee.sigio(mbed_event_queue()->event(xbee_rx));
}

void xbee_rx(void)

{

    static int i = 0;

    static char buf[100] = {0};

    while (xbee.readable())
    {

        char *c = new char[1];

        xbee.read(c, 1);

        if (*c != '\r' && *c != '\n')
        {

            buf[i] = *c;

            i++;

            buf[i] = '\0';
        }
        else if ((*c == '\r' || *c == '\n') && i == 0)
        { // ignore redundant char in buffer
        }

        else

        {

            i = 0;

            printf("Get: %s\r\n", buf);

            xbee.write(buf, sizeof(buf));
        }
    }

    ThisThread::sleep_for(100ms);
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