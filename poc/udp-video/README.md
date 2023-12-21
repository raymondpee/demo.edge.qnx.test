# Introduction

This is the sample where we transmit the video stream using udp as follow

```
UDP source -> UDP processor -> UDP sink
```

Both 3 of the application will be happen at different node. UDP processor will be taken place in QNX environment

Currently the test will be running on local machines for 2 nodes (UDP source and UDP sink)

# Instruction

In UDP source node, execute script below
```
sudo bash udp-src.sh
```

In UDP sink node, execute script below
```
sudo bash udp-sink.sh
```

Then you should able to see the X server window displaying random video frame