#!/bin/bash
 
cd /opt/mjpg-streamer/
sudo ./mjpg_streamer -i "./input_uvc.so -f 15 -r 640x480 -d /dev/video0 -y -q 25" -o "./output_http.so -w ./www -p 8080" &
