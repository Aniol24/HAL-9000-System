#include "../global.h"

/*
    Defines
*/
#define MAX_PROMPT_SIZE 512 // para limitar el máximo de memória dinámica para un promt
                            // y evitar que el sistema reviente en casos extremos, p.ej:
                            // 50 millones de líneas

/*
    Variables globales necesárias (pata liberar en caso de Ctrl+C)
*/
static BowmanConfig *bowman_config;// configuración de Bowman
static char *user_prompt;          // prompt del usuario

/*
    Delaración de funciones
*/
void kctrlc(int signum);
void free_all_dynamic_memory(void);
int connect_to_discovery(const BowmanConfig *config);

/**
 * @brief Función principal del programa Bowman
 * 
 * @param argc Número de argumentos
 * @param argv Argumentos
 * @return int 0 si todo ha ido bien, 1 si ha habido algún error
*/
int main(int argc, char **argv) {

    check_input_arguments(argc, 2);

    unsigned char end = 0, connected = 0;
    bowman_config = (BowmanConfig *) malloc (sizeof(BowmanConfig));
    user_prompt = (char *) malloc (sizeof(char) * MAX_PROMPT_SIZE);

    signal(SIGINT, kctrlc); //Activar signal Ctrl+C
    
    // Rellenar la configuración de Bowman y comprobación si se rellena correctamente
    if (fill_bowman_config(bowman_config, argv[1]) == 0) {
        printF(RED, "ERROR: User connection unreachable. Please, check the user's filename and try again!\n");
        exit(EXIT_FAILURE);
    }

    printF(GREEN, "\n%s user initialized\n", bowman_config->username);

    int discovery_socket = connect_to_discovery(bowman_config);
    if (discovery_socket == -1) {
        printF(RED, "Failed to connect to Discovery server\n");
        exit(EXIT_FAILURE);
    }

    do {
        printF(WHITE, "$ ");
        int n = read(0, user_prompt, MAX_PROMPT_SIZE);
        user_prompt[n-1] = '\0';

        end = handle_bowman_command(user_prompt, &connected);
    } while (!end);

    close(discovery_socket);
    free_all_dynamic_memory();

    return (EXIT_SUCCESS);
}

/**
 * @brief Libera toda la memoria dinámica reservada en el programa
 * 
 * @return void
*/
void free_all_dynamic_memory(void) {
    free(bowman_config->username);
    free(bowman_config->folder_path);
    free(bowman_config->discovery_ip);
    free(bowman_config);
    free(user_prompt);
}

/**
 * @brief Función que se ejecuta cuando se pulsa Ctrl+C
 * 
 * @param signum Señal que se ha recibido
 * @return void
*/
void kctrlc(int signum) {
    if (signum == SIGINT) {
        free_all_dynamic_memory();
    }
    exit(EXIT_SUCCESS);
}

/**
 * @brief Establece una conexión con el servidor Discovery
 * 
 * @param config Configuración de Bowman que contiene la IP y el puerto del servidor Discovery
 * @return int Descriptor del socket si la conexión es exitosa, -1 en caso de error
*/
int connect_to_discovery(const BowmanConfig *config) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printF(RED, "Socket creation failed\n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(config->discovery_port);

    // Convertir la dirección IP a formato binario y almacenarla en serv_addr
    if (inet_pton(AF_INET, config->discovery_ip, &serv_addr.sin_addr) <= 0) {
        printF(RED, "Invalid address / Address not supported\n");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printF(RED, "Connection Failed\n");
        close(sock);
        return -1;
    }

    return sock;
}