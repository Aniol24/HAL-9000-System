#include "global.h"

/**
 * @brief Print a message with the color specified
 * 
 * @param color Color to print the message
 * @param format Format of the message
 * @param ... Arguments of the message
 * @return void
*/
void printF(int color, const char *format, ...) {
    char *colorCode;
    switch (color) {
        case GREEN:
            colorCode = "\033[32m";
            break;
        case YELLOW:
            colorCode = "\033[33m";
            break;
        case RED:
            colorCode = "\033[31m";
            break;
        default:
            colorCode = "";
            break;
    }

    va_list args;
    va_start(args, format);

    char *buffer = NULL;
    char *full_format = malloc(strlen(colorCode) + strlen(format) + strlen("\033[0m") + 1);
    if (!full_format) {
        va_end(args);
        return; 
    }
    sprintf(full_format, "%s%s\033[0m", colorCode, format);
    
    int len = vasprintf(&buffer, full_format, args);
    if (buffer) {
        write(1, buffer, len);
        free(buffer);
    }

    free(full_format);
    va_end(args);
}

/**
 * @brief Read from a file descriptor until a delimiter is found
 * 
 * @param fd File descriptor to read from
 * @param end Delimiter to stop reading
 * @return char* String read from the file descriptor
*/
char *read_until_delimiter(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char *string = (char *)malloc(sizeof(char));

    while (1) {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0) {
            string = (char *)realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else { 
            break; 
        }
    }

    string[i] = '\0';
    return string;
}

/**
 * @brief Check if the number of arguments is the expected
 * 
 * @param current Number of arguments received
 * @param expected Number of arguments expected
 * @return void
*/
void check_input_arguments(const int current, const int expected) {
    if (current != expected) {
        if (current < expected) {
                printF(RED, "ERROR: Less arguments than expected!\n");
        } else {
                printF(RED, "ERROR: More arguments than expected!\n");
        }
        exit(EXIT_FAILURE);
    } 
}

/**
 * @brief Display a loading spinner for a certain amount of time
 * 
 * @param color Color of the spinner
 * @param duration Duration of the spinner
 * @return void
*/
void display_loading_spinner(int color, int duration) {
    char spinner[] = "|/-\\\0";
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 200 * 1000000;

    int end_time = time(NULL) + duration;

    while (time(NULL) < end_time) {
        for (int i = 0; i < (int) strlen(spinner); i++) {
            printF(color, "\r%c", spinner[i]);
            fflush(stdout);
            nanosleep(&delay, NULL);
        }
    }
    printF(color, "\r ");
    printF(WHITE, "\n");
}

/**
 * @brief Sends a frame through a socket
 * 
 * @param socket Socket to send the frame through
 * @param frame Frame to send
 * 
 * @return void
*/
void send_frame(int socket, Frame *frame) {
    char buffer[FRAME_SIZE];
    memset(buffer, 0, FRAME_SIZE); // Inicializar el buffer con ceros para el padding
    int index = 0;

    // Construir la trama en el buffer
    buffer[index++] = frame->type;
    memcpy(buffer + index, &frame->header_length, sizeof(frame->header_length));
    index += sizeof(frame->header_length);
    memcpy(buffer + index, frame->header, frame->header_length);
    index += frame->header_length;
    memcpy(buffer + index, frame->data, FRAME_SIZE - 3 - frame->header_length);

    // Enviar trama
    write(socket, buffer, FRAME_SIZE);
}

/**
 * @brief Receives a frame through a socket
 * 
 * @param socket Socket to receive the frame through
 * @param frame Frame to receive
 * 
 * @return void
*/
void receive_frame(int socket, Frame *frame) {
    char buffer[FRAME_SIZE];
    read(socket, buffer, FRAME_SIZE);

    // Descomponer la trama del buffer
    int index = 0;
    frame->type = buffer[index++];
    memcpy(&frame->header_length, buffer + index, sizeof(frame->header_length));
    index += sizeof(frame->header_length);
    memcpy(frame->header, buffer + index, frame->header_length);
    index += frame->header_length;
    memcpy(frame->data, buffer + index, FRAME_SIZE - 3 - frame->header_length);
}
