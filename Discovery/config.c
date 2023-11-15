#include "config.h"

unsigned char readDiscoveryConfig(const char* filename, DiscoveryConfig* config) {
    char *fullPath;

    asprintf(&fullPath, "../%s", filename);

    int fd = open(fullPath, O_RDONLY);
    free(fullPath);

    if (fd < 0) {
        printF(RED, "Error opening config file");
        return -1;
    }

    char *line;

    // Leer IP para Poole
    line = read_until_delimiter(fd, '\n');
    config->poole_addr.sin_family = AF_INET;
    config->poole_addr.sin_addr.s_addr = inet_addr(line);
    free(line);

    // Leer puerto para Poole
    line = read_until_delimiter(fd, '\n');
    config->poole_addr.sin_port = htons((uint16_t)atoi(line));
    free(line);

    // Leer IP para Bowman
    line = read_until_delimiter(fd, '\n');
    config->bowman_addr.sin_family = AF_INET;
    config->bowman_addr.sin_addr.s_addr = inet_addr(line);
    free(line);

    // Leer puerto para Bowman
    line = read_until_delimiter(fd, '\n');
    config->bowman_addr.sin_port = htons((uint16_t)atoi(line));
    free(line);

    printf("Poole IP: %s\n", inet_ntoa(config->poole_addr.sin_addr));
    printf("Poole port: %d\n", ntohs(config->poole_addr.sin_port));
    printf("Bowman IP: %s\n", inet_ntoa(config->bowman_addr.sin_addr));
    printf("Bowman port: %d\n", ntohs(config->bowman_addr.sin_port));

    close(fd);
    
    return 0;
}