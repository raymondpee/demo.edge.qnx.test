# Instruction

This instruction is to show how to build and run RTSP client under QNX environment

## Steps to build QNX binary
Before compilation run following cmd to compile FFMPEG to support QNX
```
source /opt/qnx710/qnxsdp-env.sh
bash ./setup-ffmpeg-env-qnx.sh
```

Next perform following to build ffmpeg decoder for QNX
```
mkdir build
cd build
cmake ..
make
```

Copy the binary to the QNX target machine
```
scp -r ./build/h264_decoder ./FFmpeg/libavcodec/libavcodec.so.58 ./FFmpeg/libavformat/libavformat.so.58 ./FFmpeg/libavutil/libavutil.so.56 ./FFmpeg/libswscale/libswscale.so.5 ./FFmpeg/libswresample/libswresample.so.3 <username>@<remote_address>:<remote_dir>
```

On target machine (RTSP server), run following command on existing repo directory
```
docker-compose up
```

On edge machine (QNX RTSP client), go to the <remote_dir> and run following execution
./h264_decoder rtsp://<target machine ip>:8554/mystream

You will see following outcome 

```
root@aarch64le:# ./h264_decoder rtsp://192.168.2.191:8554/mystream
[swscaler @ 37ceee3040] No accelerated colorspace conversion found from yuv420p to rgb24.
1 Frame: 0
2 Is Video
3 create stream
4 decoding
Bytes decoded 8464 check 0
1 Frame: 1
2 Is Video
4 decoding
Bytes decoded 8472 check 0
1 Frame: 2
2 Is Video
4 decoding
```
You will also get the ppm file in the same directory, copy out and observe the outcome as follow
```
apt-get install imagemagick
convert test101.ppm test101.jpg
![Sample](picture/image.png)
```