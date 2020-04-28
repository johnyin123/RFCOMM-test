#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char *argv[]){

    time_t time_stamp;
    //time_string will hold the date using the ISO 8601 format
    char time_string[32] = {0};

    //cli_addr is the data structure that holds information about the connected client
    struct sockaddr_rc loc_addr = {0}, cli_addr = {0};
    socklen_t opt = sizeof(cli_addr);
    //cli_mac will hold the string representation of the client's mac address
    char cli_mac[32] = {0};
    int s, client;
    //RFCOMM uses channels from 1 to 30 similarly to socket ports
    uint8_t chan = 1;

    int bytes_read;
    float readout;

    FILE * report = fopen("report.csv", "w");

    if (report == NULL){
        fprintf(stderr, "FILE error: %s\n", strerror(errno));
        exit(1);
    }

    fprintf(report,"Timestamp,MacAddress,Temperature\n");

    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    //BDADDR_ANY is the first available bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = chan;

    //This socket will keep listening for new connections
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    listen(s, chan);

    while(1){

        client = accept(s, (struct sockaddr *) &cli_addr, &opt);

        //Copy the client's mac address to a string
        ba2str(&cli_addr.rc_bdaddr, cli_mac);

        bytes_read = read(client, &readout, sizeof(readout));

        if (bytes_read > 0) {

            time_stamp = time(NULL);
            //strftime wants the date passed as a tm data structure. Since we have a date expressed as
            //count of seconds since the beginning of Unix time, we are using localtime(time_t *) to
            //convert to tm
            strftime(time_string, sizeof(time_string), "%FT%T%z", localtime(&time_stamp));
            
            //Write the date, the mac address and the received vale in a "csv format"
            int ret = fprintf(report, "%s,%s,%f\n", time_string, cli_mac, readout);
            
            if (ret < 0) {
                fprintf(stderr, "fprintf error: %s\n", strerror(errno));
            } else {
                //Since we are dealing with an infinite loop, we use fflush to immediately flush
                //the buffer onto the file, insted of waiting for fclose (which won't happen)
                fflush(report);
            }

            //cli_mac and time_string should be cleared out to avoid holding old values on a new cicle
            // in case those vector are not filled again
            //memset(..., 0, sizeof(...));
        }

        close(client);
    }

    //Unreachable

    close(s);

    fclose(report);

    return 0;
}
