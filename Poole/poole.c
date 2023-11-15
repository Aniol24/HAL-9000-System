#include "../global.h"

/*
    Variables globales necesárias (pata liberar en caso de Ctrl+C)
*/
static PooleConfig *poole_config; // configuración de Poole
static int connected_users = 0;   // Variable global para contar los usuarios conectados

/*
    Delaración de funciones
*/
void free_all_dynamic_memory(void);
int connect_to_discovery(const PooleConfig *config);

int main(int argc, char** argv) {
    
    check_input_arguments(argc, 2);

    poole_config = (PooleConfig *) malloc (sizeof(PooleConfig));

    printF(YELLOW, "Reading configuration file...\n");
    display_loading_spinner(YELLOW, 3);

    // Rellenar la configuración de Bowman y comprobación si se rellena correctamente
    if (fill_poole_config(poole_config, argv[1]) == 0) {
        printF(RED, "ERROR: Server connection unreachable. Please, check the server's filename and try again!\n");
        exit(EXIT_FAILURE);
    }

    printF(YELLOW, "Connecting %s Server to the system...\n", poole_config->username);

    int discovery_socket = connect_to_discovery(poole_config);
    if (discovery_socket == -1) {
        printF(RED, "Failed to connect to HAL 9000 System\n");
        exit(EXIT_FAILURE);
    }

    printF(GREEN, "Connected to HAL 9000 System, ready to listen to Bowmans petitions\n\n");
    printF(WHITE, "Waiting for connections...\n");

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
    free(poole_config->username);
    free(poole_config->folder_path);
    free(poole_config->discovery_ip);
    free(poole_config->poole_ip);
    free(poole_config);
}

/**
 * @brief Establece una conexión con el servidor Discovery
 * 
 * @param config Configuración de Poole que contiene la IP y el puerto del servidor Discovery
 * @return int Descriptor del socket si la conexión es exitosa, -1 en caso de error
*/
int connect_to_discovery(const PooleConfig *config) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printF(RED, "Socket creation failed\n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(config->discovery_port);

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