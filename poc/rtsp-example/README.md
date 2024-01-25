# Introduction

This section is regarding of streaming via rtsp server from live555.

The RTSPClient would be build in following 2 platforms
- x86-64 
- qnx

# Steps to run
1) Run the rtsp server from the docker compose cmd:
```
docker-compose up
```

2) For target of x-86-64, Build and run the rtsp client (qnx will be supported later)
```
mkdir build
cd build
cmake ..
make
```

3) Run the stream as follow and expect the output
```
./RTSPClient rtsp://192.168.18.76:8554/test.264
Created new TCP socket 3 for connection
Connecting to 192.168.18.76, port 8554 on socket 3...
...remote connection opened
Sending request: DESCRIBE rtsp://192.168.18.76:8554/test.264 RTSP/1.0
CSeq: 2
User-Agent: ./RTSPClient (LIVE555 Streaming Media v2023.06.14)
Accept: application/sdp


Received 703 new bytes of response data.
Received a complete DESCRIBE response:
RTSP/1.0 200 OK
CSeq: 2
Date: Thu, Jan 25 2024 03:59:11 GMT
Content-Base: rtsp://192.168.18.76:8554/test.264/
Content-Type: application/sdp
Content-Length: 535

v=0
o=- 1706155151750134 1 IN IP4 192.168.18.76
s=H.264 Video, streamed by the LIVE555 Media Server
i=test.264
t=0 0
a=tool:LIVE555 Streaming Media v2023.06.14
a=type:broadcast
a=control:*
a=range:npt=now-
a=x-qt-text-nam:H.264 Video, streamed by the LIVE555 Media Server
a=x-qt-text-inf:test.264
m=video 0 RTP/AVP 96
c=IN IP4 0.0.0.0
b=AS:500
a=rtpmap:96 H264/90000
a=fmtp:96 packetization-mode=1;profile-level-id=64001F;sprop-parameter-sets=J2QAH6wTFsBQBbsBagICAoAAE4gAA9CQcDAAXcAAF3Be98FA,KO4fLA==
a=control:track1

[URL:"rtsp://192.168.18.76:8554/test.264/"]: Got a SDP description:
v=0
o=- 1706155151750134 1 IN IP4 192.168.18.76
s=H.264 Video, streamed by the LIVE555 Media Server
i=test.264
t=0 0
a=tool:LIVE555 Streaming Media v2023.06.14
a=type:broadcast
a=control:*
a=range:npt=now-
a=x-qt-text-nam:H.264 Video, streamed by the LIVE555 Media Server
a=x-qt-text-inf:test.264
m=video 0 RTP/AVP 96
c=IN IP4 0.0.0.0
b=AS:500
a=rtpmap:96 H264/90000
a=fmtp:96 packetization-mode=1;profile-level-id=64001F;sprop-parameter-sets=J2QAH6wTFsBQBbsBagICAoAAE4gAA9CQcDAAXcAAF3Be98FA,KO4fLA==
a=control:track1

[URL:"rtsp://192.168.18.76:8554/test.264/"]: Initiated the "video/H264" subsession (client ports 57596-57597)
Sending request: SETUP rtsp://192.168.18.76:8554/test.264/track1 RTSP/1.0
CSeq: 3
User-Agent: ./RTSPClient (LIVE555 Streaming Media v2023.06.14)
Transport: RTP/AVP;unicast;client_port=57596-57597


Received 216 new bytes of response data.
Received a complete SETUP response:
RTSP/1.0 200 OK
CSeq: 3
Date: Thu, Jan 25 2024 03:59:11 GMT
Transport: RTP/AVP;unicast;destination=192.168.18.76;source=192.168.18.76;client_port=57596-57597;server_port=6970-6971
Session: 6EABBF17;timeout=65


[URL:"rtsp://192.168.18.76:8554/test.264/"]: Set up the "video/H264" subsession (client ports 57596-57597)
here
[URL:"rtsp://192.168.18.76:8554/test.264/"]: Created a data sink for the "video/H264" subsession
Sending request: PLAY rtsp://192.168.18.76:8554/test.264/ RTSP/1.0
CSeq: 4
User-Agent: ./RTSPClient (LIVE555 Streaming Media v2023.06.14)
Session: 6EABBF17
Range: npt=0.000-


Received 188 new bytes of response data.
Received a complete PLAY response:
RTSP/1.0 200 OK
CSeq: 4
Date: Thu, Jan 25 2024 03:59:11 GMT
Range: npt=0.000-
Session: 6EABBF17
RTP-Info: url=rtsp://192.168.18.76:8554/test.264/track1;seq=5705;rtptime=2387017923


[URL:"rtsp://192.168.18.76:8554/test.264/"]: Started playing session...
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 20 bytes.      Presentation time: 1706155151.852059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 36 bytes.      Presentation time: 1706155151.852059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 4 bytes.       Presentation time: 1706155151.852059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 66704 bytes.   Presentation time: 1706155151.852059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 40989 bytes.   Presentation time: 1706155151.892059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 21787 bytes.   Presentation time: 1706155151.932059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 11331 bytes.   Presentation time: 1706155151.972059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 8346 bytes.    Presentation time: 1706155152.012059
Stream "rtsp://192.168.18.76:8554/test.264/"; video/H264:       Received 12006 bytes.   Presentation time: 1706155152.052059

```

The last line will be indicate that the streaming is in progress

