# Introduction

This section is regarding on hello world test

## Steps to run
Please perform following to build the helloworld

```
mkdir build
cd build
cmake ..
make
```

Copy the binary to the QNX target machine
```
scp -r ./hello username@remote_address:remote_dir
```

ssh to the remote diretory and peform follow
```
/remote_dir/hello
```
