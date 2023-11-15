#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "../global.h"

/**
 * @brief Handle the command received from the user
 * 
 * @param command Command to handle
 * @param connected Flag to know if the user wants to logout or not
*/
unsigned char handle_bowman_command(char *command, unsigned char *connected);

#endif