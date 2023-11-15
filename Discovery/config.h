#ifndef CONFIG_H
#define CONFIG_H

#include "../global.h"

// Estructura para almacenar la configuración
typedef struct {
    struct sockaddr_in poole_addr;
    struct sockaddr_in bowman_addr;
} DiscoveryConfig;

// Prototipo de función
unsigned char readDiscoveryConfig(const char* filename, DiscoveryConfig* config);

#endif // CONFIG_H