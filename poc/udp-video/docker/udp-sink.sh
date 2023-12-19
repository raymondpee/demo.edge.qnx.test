#!/bin/bash
export DISPLAY=:1.0
xhost +
docker run -it --network=host --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix raymondpyn/video-udpsink:19122023