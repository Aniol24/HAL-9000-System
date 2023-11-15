#include "config.h"

#define BACKLOG 128

typedef struct PooleServer {
    int socket_fd;
    int connected_users;
    struct sockaddr_in address;
    struct PooleServer* next;
} PooleServer;

PooleServer* poole_servers_head = NULL;

DiscoveryConfig *discoverConfig;

void* handlePooleConnection(void* arg);
void* handleBowmanConnection(void* arg);

int main(int argc, char* argv[]) {
    check_input_arguments(argc, 2);

    discoverConfig = (DiscoveryConfig *) malloc (sizeof(DiscoveryConfig));

    if (readDiscoveryConfig(argv[1], discoverConfig) < 0) {
        printF(RED, "Error reading discovery file\n");
        exit(EXIT_FAILURE);
    }

    int poole_fd = socket(AF_INET, SOCK_STREAM, 0);
    int bowman_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (poole_fd < 0 || bowman_fd < 0) {
        printF(RED, "Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Enlazar y escuchar en las direcciones de Poole y Bowman
    if (bind(poole_fd, (struct sockaddr *)&discoverConfig->poole_addr, sizeof(discoverConfig->poole_addr)) < 0 ||
        bind(bowman_fd, (struct sockaddr *)&discoverConfig->bowman_addr, sizeof(discoverConfig->bowman_addr)) < 0) {
        printF(RED, "Bind failed\n");
        exit(EXIT_FAILURE);
    }

    listen(poole_fd, BACKLOG);
    listen(bowman_fd, BACKLOG);

    // Configurar los sockets para que sean no bloqueantes
    fcntl(poole_fd, F_SETFL, O_NONBLOCK);
    fcntl(bowman_fd, F_SETFL, O_NONBLOCK);

    // Aceptar las conexiones y crear hilos para manejar cada una de ellas
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int* client_fd_ptr;

        // Aceptar conexiones de Poole
        int client_fd = accept(poole_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd >= 0) {
            client_fd_ptr = malloc(sizeof(int));
            *client_fd_ptr = client_fd;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handlePooleConnection, client_fd_ptr);
        }

        // Aceptar conexiones de Bowman
        client_fd = accept(bowman_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd >= 0) {
            client_fd_ptr = malloc(sizeof(int));
            *client_fd_ptr = client_fd;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handleBowmanConnection, client_fd_ptr);
        }

        usleep(10000); // Esperar 10ms (pequeño retardo para reducir el uso de CPU)
    }

    close(poole_fd);
    close(bowman_fd);
    free(discoverConfig);

    return EXIT_SUCCESS;
}

void* handlePooleConnection(void* arg) {
    int poole_socket = *(int*)arg;
    free(arg);

    // Registrar este servidor Poole
    struct sockaddr_in poole_addr;
    socklen_t addr_len = sizeof(poole_addr);
    getpeername(poole_socket, (struct sockaddr *)&poole_addr, &addr_len);

    add_poole_server(poole_socket, poole_addr);
    printF(GREEN, "Poole connected\n");

    // Enviar confirmación de conexión a Poole
    Frame frame;
    frame.type = 0x01; // Tipo: CON_OK
    strcpy(frame.header, "CON_OK");
    frame.header_length = strlen(frame.header);
    memset(frame.data, 0, sizeof(frame.data)); // Limpiar el campo data
    send_frame(poole_socket, &frame);

    // Ccontinuar la comunicación si hace falta

    return NULL;
}

void* handleBowmanConnection(void* arg) {
    int bowman_socket = *(int*)arg;
    free(arg);

    Frame frame;
    PooleServer* least_loaded_poole = find_least_loaded_poole_server();
    if (least_loaded_poole == NULL) {
        frame.type = 0x01; // Type: CON_KO
        strcpy(frame.header, "CON_KO");
        frame.header_length = strlen(frame.header);
        memset(frame.data, 0, sizeof(frame.data)); // Limpiar el campo data
    } else {
        frame.type = 0x01; // Type: CON_OK
        strcpy(frame.header, "CON_OK");
        frame.header_length = strlen(frame.header);
        sprintf(frame.data, "%s&%d", inet_ntoa(least_loaded_poole->address.sin_addr), ntohs(least_loaded_poole->address.sin_port));
    }

    send_frame(bowman_socket, &frame);
    printF(GREEN, "Bowman connection handled\n");

    return NULL;
}

// Función para añadir un nuevo servidor Poole a la lista
void add_poole_server(int socket_fd, struct sockaddr_in address) {
    PooleServer* new_server = (PooleServer*)malloc(sizeof(PooleServer));
    new_server->socket_fd = socket_fd;
    new_server->connected_users = 0;
    new_server->address = address;
    new_server->next = poole_servers_head;
    poole_servers_head = new_server;
}

// Función para encontrar el servidor Poole con menos usuarios conectados
PooleServer* find_least_loaded_poole_server() {
    PooleServer* current = poole_servers_head;
    PooleServer* least_loaded = NULL;
    int min_users = INT_MAX;

    while (current != NULL) {
        if (current->connected_users < min_users) {
            min_users = current->connected_users;
            least_loaded = current;
        }
        current = current->next;
    }

    return least_loaded;
}