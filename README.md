# RFCOMM-test

Example code that sends data via bluetooth between two devices. The server listens indefinitely for connections, whereas the client sends a value to the server. Upon receiving the data, the server saves a line to a csv file with the timestamp, the mac address of the client, and the value.

## Compilation

On Debian based distributions, first install the developement library of BlueZ (the official Linux bluetooth stack) with `sudo apt install libbluetooth-dev`.

Edit `srv_mac` inside `clent.c` with the actual MAC of the device you want to send data to, then compile using `gcc -o file file.c -lbluetooth`.

## Usage

Run `./server` first and `./client` on one client (or many) to send the generated value. You should get a `report.csv` file on the server. CTRL-C to stop.
