#ifndef GLOBAL_H
#define GLOBAL_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "Bowman/config.h"
#include "Bowman/command_handler.h"
#include "Poole/config.h"
#include "Discovery/config.h"
#include <pthread.h>
#include <fcntl.h>
#include <limits.h>

/**
 * @brief Colors to print the messages
*/
#define WHITE    0
#define GREEN    1
#define YELLOW   2
#define RED      3

/**
 * @brief Maximum size for frames protocol
*/
#define FRAME_SIZE 256
#define HEADER_MAX_SIZE 253 // Tamaño máximo del header, ajustado para el peor caso

typedef struct {
    char type; // 1 byte
    unsigned short header_length; // 2 bytes
    char header[HEADER_MAX_SIZE]; // Hasta 253 bytes
    char data[FRAME_SIZE - 3 - HEADER_MAX_SIZE]; // El resto hasta 256 bytes
} Frame;

/**
 * @brief Print a message with the color specified
 * 
 * @param color Color to print the message
 * @param format Format of the message
 * @param ... Arguments of the message
 * @return void
*/
void printF(int color, const char *format, ...);

/**
 * @brief Read from a file descriptor until a delimiter is found
 * 
 * @param fd File descriptor to read from
 * @param end Delimiter to stop reading
 * @return char* String read from the file descriptor
*/
char *read_until_delimiter(int fd, char end);

/**
 * @brief Check if the number of arguments is the expected
 * 
 * @param current Number of arguments received
 * @param expected Number of arguments expected
 * @return void
*/
void check_input_arguments(const int current, const int expected);

/**
 * @brief Display a loading spinner for a certain amount of time
 * 
 * @param color Color of the spinner
 * @param duration Duration of the spinner
 * @return void
*/
void display_loading_spinner(int color, int duration);

#endif // !GLOBAL_H
