# Introduction

This section is regarding on MQTT publisher-subscriber test on QNX environment

## Steps to build QNX binary
Please perform following to build the mqtt_pub and mqtt_sub for QNX

```
source /opt/qnx710/qnxsdp-env.sh
mkdir build
cd build
cmake ..
make
```

Copy the binary to the QNX target machine
```
scp -r mqtt_pub mqtt_sub libmosquitto.so.1 <username>@<remote_address>:<remote_dir>
```

## Step for MQTT PUB/SUB test execution

1) Run MQTT broker on any host machine outside QNX
Run following cmd on the any host machine.
```
cd docker
docker-compose up
```

2) Capture the host address of host machine using ipconfig or ifconfig
```
ifconfig
```

3) Login to the QNX environment using ssh and go to the remote_dir where the mqtt_pub and mqtt_sub is copied,
```
ssh <username>@<remote address>
cd <remote_dir>
```

4) set MQTT broker ip address and port as below using export 
```
export MQTT_ADDRESS=<host address>
export MQTT_PORT=1883
```


5) Execute MQTT sub
```
./mqtt_sub
```

6) Create another QNX console, rerun (3)-(4) and execute below
```
./mqtt_pub
```

7) You will see following printout on mqtt_sub
```
Received message on topic test: Hello, MQTT!
```