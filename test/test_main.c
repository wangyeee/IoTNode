#include <stdlib.h>
#include <stdio.h>
#include <string.h>   /* String function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

#define MSG_PAYLOAD_SIZE 16

typedef enum {
    HEART_BEAT = 0,
    RTC_SYNC,
    SWITCH,
    // etc.
    MSG_LISTENER_AMOUNT
} msg_type;

typedef struct {
    uint8_t channel;
    uint8_t addr[5];
} node_t;

typedef struct {
    uint8_t type;
    uint8_t seq;
    uint8_t payload[MSG_PAYLOAD_SIZE];
} message_t;

typedef struct {
    uint8_t id : 4;
    uint8_t status : 2;
    uint8_t enabled : 2;
} __attribute__ ((__packed__)) switch_t;

#define SWITCH_SIZE sizeof(switch_t)
#define NODE_SIZE sizeof(node_t)
#define MSG_SIZE sizeof(message_t)

// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error
int serialport_init(const char* serialport, int baud)
{
    struct termios toptions;
    int fd;

    fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        //perror("init_serialport: Unable to open port ");
        return -1;
    }

    if (tcgetattr(fd, &toptions) < 0) {
        //perror("init_serialport: Couldn't get term attributes");
        return -2;
    }
    speed_t brate = baud; // let you override switch below if needed
    switch(baud) {
        case 4800:   brate=B4800;   break;
        case 9600:   brate=B9600;   break;
#ifdef B14400
        case 14400:  brate=B14400;  break;
#endif
        case 19200:  brate=B19200;  break;
#ifdef B28800
        case 28800:  brate=B28800;  break;
#endif
        case 38400:  brate=B38400;  break;
        case 57600:  brate=B57600;  break;
        case 115200: brate=B115200; break;
    }
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;

    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
        //perror("init_serialport: Couldn't set term attributes");
        return -3;
    }
    
    return fd;
}

typedef struct {
    node_t node;
    message_t msg;
} test_data;

/**
 * 0 1 2 3 | 4 5     | 6 7
 * id      | enabled | status
 */
static uint8_t switch_pack(switch_t* sw, uint8_t* buf, uint8_t len) {
    if (len > 0) {
        buf[0] = (uint8_t) ((sw->id << 4) | (sw->enabled << 2) | sw->status);
        return 1;
    }
    return 0;
}

static uint8_t switch_unpack(switch_t* sw, uint8_t* buf, uint8_t len) {
    if (len > 0) {
        sw->id = (buf[0] & 0b11110000) >> 4;
        sw->enabled = ((buf[0] & 0b00001100) >> 2) ? 1 : 0;
        sw->status = (buf[0] & 0b00000011) ? 1 : 0;
        return 1;
    }
    return 0;
}

//#define TEST_SW_PACK

int main(int argc, char** argv) {
#ifdef TEST_SW_PACK
    switch_t swt;
    switch_t repk;
    uint8_t x;
    uint8_t r;
    uint8_t i, s, e;
    for (i = 0; i < 7; i++) {
        for (s = 0; s < 2; s++) {
            for (e = 0; e < 2; e++) {
                swt.id = i;
                swt.status = s;
                swt.enabled = e;
                r = switch_pack(&swt, &x, 1);
                printf("swt id = %d, status = %d, enabled = %d\n", swt.id, swt.status, swt.enabled);
                printf("pack result: x = 0x%02x, r = %d\n", x, r);
                r = switch_unpack(&repk, &x, 1);
                printf("re-pack result: r = %d\n", r);
                printf("swt id = %d, status = %d, enabled = %d\n", repk.id, repk.status, repk.enabled);
                r = (swt.id == repk.id) &&
                    (swt.status == repk.status) &&
                    (swt.enabled == repk.enabled);
                printf("Compare r = %s\n", r ? "sucess" : "failure");
            }
        }
    }
#else
    test_data data;
    // node Addr[5]= {97, 89, 64, 222, 121};
    int serial_desc = serialport_init("/dev/tty.usbserial-A601XMZP", 115200);
    if (serial_desc < 0) {
        printf("Error opening serial port: %d\n", serial_desc);
        return 1;
    }
    data.node.channel = 92;
    data.node.addr[0] = 97;
    data.node.addr[1] = 89;
    data.node.addr[2] = 64;
    data.node.addr[3] = 222;
    data.node.addr[4] = 121;
    data.msg.type = SWITCH;
    data.msg.seq = 0;
    for (int i = 0; i < MSG_PAYLOAD_SIZE; i++) {
        if (i < 7) {
            switch_t sw;
            sw.id = i;
            sw.status = 0;
            sw.enabled = 1;
            switch_pack(&sw, data.msg.payload + i, MSG_PAYLOAD_SIZE - i);
        } else {
            data.msg.payload[i] = 0xF0;
        }
        printf("byte %d is 0x%02x\n", i, data.msg.payload[i]);
    }
    int len = write(serial_desc, &data, NODE_SIZE + MSG_SIZE);
    printf("CMD: %d bytes written.\n", len);
    close(serial_desc);
#endif /* TEST_SW_PACK */
    return 0;
}
