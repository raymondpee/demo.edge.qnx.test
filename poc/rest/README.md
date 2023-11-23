# REST demo

This section is used for testing of the REST client communication from QNX to REST server at node PC.

#####   REST   ####
#QNX# <------> #PC#
#####          ####


## QNX REST client Compilation

Following command will build the QNX REST client
```
source ${QNX_HOST}/
mkdir build
cd build 
cmake -DCMAKE_TOOLCHAIN_FILE=./../../toolchain/qnx710.cmake ..
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

Later on at QNX client launch REST client as below