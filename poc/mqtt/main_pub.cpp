#include <stdio.h>
#include <stdlib.h> // Include the header for getenv
#include <mosquitto.h>
#include <cstring>

#define MQTT_TOPIC "test"
#define MQTT_MESSAGE "Hello, MQTT!"

void on_connect(struct mosquitto *mosq, void *userdata, int result) {
    if (result == 0) {
        printf("Connected to MQTT broker\n");
    } else {
        fprintf(stderr, "Failed to connect to MQTT broker\n");
    }
}

void on_publish(struct mosquitto *mosq, void *userdata, int mid) {
    printf("Message sent successfully\n");
    mosquitto_disconnect(mosq);
}

int main() {
    struct mosquitto *mosq = NULL;

    mosquitto_lib_init();

    // Get MQTT host and port from environment variables
    const char *mqtt_host = getenv("MQTT_HOST");
    const char *mqtt_port_str = getenv("MQTT_PORT");

    if (!mqtt_host || !mqtt_port_str) {
        fprintf(stderr, "Error: MQTT_HOST or MQTT_PORT environment variables not set\n");
        return 1;
    }

    int mqtt_port = atoi(mqtt_port_str);

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Unable to create Mosquitto client instance\n");
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_publish_callback_set(mosq, on_publish);

    if (mosquitto_connect(mosq, mqtt_host, mqtt_port, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Unable to connect to MQTT broker\n");
        return 1;
    }

    if (mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(MQTT_MESSAGE), MQTT_MESSAGE, 0, false) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Unable to publish message\n");
        return 1;
    }

    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
