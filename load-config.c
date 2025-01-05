#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

// Global variables
int CLIENT_PORT;
int MME_PORT;
char MME_ADDRESS[50];
int BUFFER_SIZE;
int MAX_CLIENTS;
char MCC_MNC_BUF[50];  // Holds "\\x00\\xF1\\x10" format
int MCC_MNC_LEN;
char TAC_BUF[50];      // Holds "\\x00\\x01" format
int TAC_LEN;
char ENB_NAME[50];

// Function to convert a numeric string into the "\\xHH" format
void string_to_network_hex(const char *input, char *output, size_t output_size) {
    size_t len = strlen(input) / 2;  // Each pair of characters represents one byte
    output[0] = '\0';                // Initialize the output as an empty string

    for (size_t i = 0; i < len; ++i) {
        char temp[10];
        unsigned int byte;
        // Parse two characters at a time as a hexadecimal number
        sscanf(input + (i * 2), "%2x", &byte);
        snprintf(temp, sizeof(temp), "\\x%02X", byte);  // Format as "\\xHH"
        strncat(output, temp, output_size - strlen(output) - 1);  // Append to output
    }
}

// Function to parse the YAML file
void load_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open YAML file");
        exit(EXIT_FAILURE);
    }

    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize YAML parser!\n", stderr);
        exit(EXIT_FAILURE);
    }
    yaml_parser_set_input_file(&parser, file);

    char key[50] = {0};
    int parsing_network_info = 0;

    while (1) {
        if (!yaml_parser_parse(&parser, &event)) {
            fputs("Failed to parse YAML file!\n", stderr);
            exit(EXIT_FAILURE);
        }

        if (event.type == YAML_SCALAR_EVENT) {
            const char *value = (const char *)event.data.scalar.value;

            if (parsing_network_info) {
                if (strcmp(key, "mcc_mnc_buf") == 0) {
                    char temp_buf[50];
                    strncpy(temp_buf, value, sizeof(temp_buf) - 1);
                    string_to_network_hex(temp_buf, MCC_MNC_BUF, sizeof(MCC_MNC_BUF));
                } else if (strcmp(key, "mcc_mnc_len") == 0) {
                    MCC_MNC_LEN = atoi(value);
                } else if (strcmp(key, "tac_buf") == 0) {
                    char temp_buf[50];
                    strncpy(temp_buf, value, sizeof(temp_buf) - 1);
                    string_to_network_hex(temp_buf, TAC_BUF, sizeof(TAC_BUF));
                } else if (strcmp(key, "tac_len") == 0) {
                    TAC_LEN = atoi(value);
                } else if (strcmp(key, "enb_name") == 0) {
                    strncpy(ENB_NAME, value, sizeof(ENB_NAME) - 1);
                }
            } else {
                if (strcmp(key, "client_port") == 0) {
                    CLIENT_PORT = atoi(value);
                } else if (strcmp(key, "mme_port") == 0) {
                    MME_PORT = atoi(value);
                } else if (strcmp(key, "mme_address") == 0) {
                    strncpy(MME_ADDRESS, value, sizeof(MME_ADDRESS) - 1);
                } else if (strcmp(key, "buffer_size") == 0) {
                    BUFFER_SIZE = atoi(value);
                } else if (strcmp(key, "max_clients") == 0) {
                    MAX_CLIENTS = atoi(value);
                }
            }

            strncpy(key, value, sizeof(key) - 1);  // Save the current key
        } else if (event.type == YAML_MAPPING_START_EVENT) {
            if (strcmp(key, "network_info") == 0) {
                parsing_network_info = 1;
            }
        } else if (event.type == YAML_MAPPING_END_EVENT) {
            parsing_network_info = 0;
        } else if (event.type == YAML_STREAM_END_EVENT) {
            break;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(file);
}

int main() {
    load_config("config.yaml");

    printf("CLIENT_PORT: %d\n", CLIENT_PORT);
    printf("MME_PORT: %d\n", MME_PORT);
    printf("MME_ADDRESS: %s\n", MME_ADDRESS);
    printf("BUFFER_SIZE: %d\n", BUFFER_SIZE);
    printf("MAX_CLIENTS: %d\n", MAX_CLIENTS);
    printf("MCC_MNC_BUF: %s\n", MCC_MNC_BUF);
    printf("MCC_MNC_LEN: %d\n", MCC_MNC_LEN);
    printf("TAC_BUF: %s\n", TAC_BUF);
    printf("TAC_LEN: %d\n", TAC_LEN);
    printf("ENB_NAME: %s\n", ENB_NAME);

    return 0;
}
