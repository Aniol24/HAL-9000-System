#include "command_handler.h"

/**
 * @brief Parse the command to remove the extra spaces
 * @example "LIST             SONGS" -> "LIST SONGS
 * 
 * @param command Command to parse
 * @return char* Parsed command
*/
static char *parse_command(char *command) {
    char *opcode = strtok(command, " ");
    char *aux = strtok(NULL, " ");
    
    if (aux != NULL && aux[0] != '\0') {
        char *combined = (char *) malloc (strlen(opcode) + strlen(aux) + 2);
        sprintf(combined, "%s %s", opcode, aux);
        return combined;
    }

    return strdup(opcode);
}

/**
 * @brief Handle the command received from the user
 * 
 * @param command Command to handle
 * @param connected Flag to know if the user wants to logout or not
*/
unsigned char handle_bowman_command(char *command, unsigned char *connected) {
    char *opcode = parse_command(command);

    if (!*connected) {
        if (!strcasecmp(opcode, "CONNECT")) {
            printF(GREEN, "Floyd connected to HAL 9000 system, welcome music lover!\n");
            *connected = 1; 
            //connectServer("poole.data");
        } else if (strcasecmp(opcode, "LOGOUT") == 0) { //PREGUNTAR SI SE PUEDE SALIR SIN ESTAR CONECTADO
            printF(GREEN, "Thanks for using HAL 9000, see you soon, music lover!\n");
            free(opcode);
            return 1;
        } else {
            printF(RED, "Cannot execute command, you are not connected to HAL 9000\n");
        }
    } else {
        if (!strcasecmp(opcode, "LOGOUT")) {
            printF(GREEN, "Thanks for using HAL 9000, see you soon, music lover!\n");
            free(opcode);
            return 1;
        } else if (!strcasecmp(opcode, "LOGOUT")) {
            /* 
                Handle the DOWNLOAD
            */
           printF(WHITE, "Download\n");
        } else if (!strcasecmp(opcode, "CHECK DOWNLOADS")) {
            /* 
                Handle the CHECK DOWNLOADS
            */
           printF(WHITE, "Check downloads\n");
        } else if (!strcasecmp(opcode, "LIST SONGS")) {
            /* 
                Handle the LIST SONGS
            */
           printF(WHITE, "List songs\n");
        } else if (!strcasecmp(opcode, "LIST PLAYLISTS")) {
            /* 
                Handle the LIST PLAYLISTS
            */
           printF(WHITE, "List playlists\n");
        } else {
            printF(RED, "ERROR: Please input a valid command.\n");
        }
    }
    
    free(opcode);
    return 0;
}