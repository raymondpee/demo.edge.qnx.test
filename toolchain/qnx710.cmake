# this one is important
SET(CMAKE_SYSTEM_NAME QNX)

set(CMAKE_C_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-gcc)
set(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++)

SET(CMAKE_AR "$ENV{QNX_HOST}/usr/bin/ntoaarch64-ar" CACHE PATH "QNX AR Program" )

# which library to find
SET(XML_LIBRARY xml2)
SET(SSL_LIBRARY ssl)

# where to search the lib
SET(LIBRARY_ARCH x86)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH $ENV{QNX_HOST}/../../../target)

#set(ENV{OPENSSL_ROOT_DIR} ${CMAKE_FIND_ROOT_PATH}/qnx7/aarch64le/usr/bin/openssl CACHE INTERNAL "")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

