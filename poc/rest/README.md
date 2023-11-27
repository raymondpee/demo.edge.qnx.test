# REST demo

This section is used for testing of the REST client communication from QNX to REST server at node PC.

#####   REST   ####
#QNX# <------> #PC#
#####          ####


## QNX REST client Compilation

Following command will build the QNX REST client
```
source /opt/qnx710/qnxsdp-env.sh
mkdir build
cd build 
cmake ..
make
```

Later on copy the rest_client to the QNX machine

## Execution

### 1. Run REST server

To start the test we can start the REST server as below

```
cd fastapi
docker-compose up
```

### 2. Run REST QNX client

1) Login to the QNX environment using ssh and go to the remote_dir where the mqtt_pub and mqtt_sub is copied,
```
ssh <username>@<remote address>
cd <remote_dir>
```

2) Run the rest client as below
```
./rest_client
```

3) You will see the printout as below from the rest client
