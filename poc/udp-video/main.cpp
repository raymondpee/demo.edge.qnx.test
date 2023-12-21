#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define DEFAULT_UDP_ADDR "0.0.0.0"
#define DEFAULT_UDP_PORT "5000"
#define MAX_BUFFER_SIZE 65536  // Maximum UDP packet size

// Custom getenv function with a default value
const char *getenv_with_default(const char *name, const char *default_value) {
    const char *value = getenv(name);
    return (value != NULL) ? value : default_value;
}

int main() {
    int udpSocket;
    struct sockaddr_in serverAddr, udp_src, udp_sink;
    socklen_t addrLen = sizeof(udp_src);
    char buffer[MAX_BUFFER_SIZE];

    const char *udp_src_addr  = getenv_with_default("UDP_SRC_ADDR",DEFAULT_UDP_ADDR);
    const char *udp_sink_addr = getenv_with_default("UDP_SINK_ADDR",DEFAULT_UDP_ADDR);
    const char *udp_server_port = getenv_with_default("UDP_SERVER_PORT",DEFAULT_UDP_PORT);
    const char *udp_src_port    = getenv_with_default("UDP_SRC_PORT",DEFAULT_UDP_PORT);
    const char *udp_sink_port   = getenv_with_default("UDP_SINK_PORT",DEFAULT_UDP_PORT);

    // Create UDP socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        perror("Error creating UDP socket");
        exit(1);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&udp_src, 0, sizeof(udp_src));
    memset(&udp_sink, 0, sizeof(udp_sink));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(udp_server_port));

    udp_src.sin_family = AF_INET;
    udp_src.sin_addr.s_addr = inet_addr(udp_src_addr);
    udp_src.sin_port = htons(atoi(udp_src_port));

    udp_sink.sin_family = AF_INET;
    udp_sink.sin_addr.s_addr = inet_addr(udp_sink_addr);
    udp_sink.sin_port = htons(atoi(udp_sink_port));

    // Bind the socket to the server address
    if (bind(udpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        close(udpSocket);
        exit(1);
    }
    printf("UDP server is listening on port %s...\n", udp_server_port);

    while (1) {
        ssize_t bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&udp_src, &addrLen);
        if (bytesRead < 0) {
            perror("Error receiving UDP packet");
            continue;
        }
        printf("Successfully receieve package");
        // Process the received frame (you can add your logic here)

        // Forward the frame to another UDP destination
        ssize_t bytesSent = sendto(udpSocket, buffer, bytesRead, 0, (struct sockaddr*)&udp_sink, sizeof(udp_sink));
        if (bytesSent < 0) {
            perror("Error forwarding UDP packet");
            continue;
        }

        printf("Forwarded %ld bytes from %s:%d to %s:%d\n", bytesSent, inet_ntoa(udp_src.sin_addr), ntohs(udp_src.sin_port), inet_ntoa(udp_sink.sin_addr), ntohs(udp_sink.sin_port));
    }

    // Close the socket (this code will not be reached in this example)
    close(udpSocket);

    return 0;
}
