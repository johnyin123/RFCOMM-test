#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

float fake_temp(int min, int max){
    //Fake sensor that returns a temperature in the interval [min, max]
    float temp = min + (float)rand()/RAND_MAX * (max - min);
    return temp;
}

int main(int argc, char * argv[]){

    float temp;
    //Initialize the (insecure) random number generator using the current time
    srand(time(NULL));

    struct sockaddr_rc addr = {0};
    //MAC address of the device to whom send the readout
    char srv_mac[18] = "26:8C:60:DE:13:F8";
    int s, status;
    uint8_t chan = 1;

    char buffer[128] = {0};

    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = chan;

    //Convert the MAC address string to a bdaddr_t type
    str2ba(srv_mac, &addr.rc_bdaddr);

    //Connect, send value and disconnect
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    temp = fake_temp(10, 20);

    if(status == 0){
        printf("Connected, sending temperature of %f °C\n", temp);
        //Once connected we can use the standard write() system call passing the file descritor
        status = write(s, &temp, sizeof(temp));
    } else if (status < 0){
        fprintf(stderr, "connect error: %s\n", strerror(errno));
    }

    close(s);

    return 0;
}
