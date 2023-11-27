#include <iostream>
#include <mosquitto.h>

void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    if (message->payloadlen) {
        std::cout << "Received message on topic " << message->topic << ": " << (char *)message->payload << std::endl;
    } else {
        std::cout << "Received empty message on topic " << message->topic << std::endl;
    }
}

int main() {
    struct mosquitto *mosq = NULL;
    const char *topic = "test";              // MQTT topic to subscribe to
    int keepalive = 60;                      // Keep-alive interval in seconds
    
    const char *host = std::getenv("MQTT_HOST"); // Set MQTT_HOST environment variable
    const char *port_str = std::getenv("MQTT_PORT"); // Set MQTT_PORT environment variable

    if (!host || !port_str) {
        std::cerr << "Error: MQTT_HOST or MQTT_PORT environment variables not set" << std::endl;
        return 1;
    }

    int port = std::atoi(port_str); // Convert port_str to an integer

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        std::cerr << "Error: Unable to create Mosquitto client" << std::endl;
        return 1;
    }

    // Set the message callback function
    mosquitto_message_callback_set(mosq, message_callback);

    // Connect to the MQTT broker
    if (mosquitto_connect(mosq, host, port, keepalive)) {
        std::cerr << "Error: Unable to connect to the MQTT broker" << std::endl;
        return 1;
    }

    // Subscribe to the MQTT topic
    if (mosquitto_subscribe(mosq, NULL, topic, 0)) {
        std::cerr << "Error: Unable to subscribe to the MQTT topic" << std::endl;
        return 1;
    }

    // Enter the network loop to handle incoming messages
    mosquitto_loop_forever(mosq, -1, 1);

    // Cleanup and disconnect
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
