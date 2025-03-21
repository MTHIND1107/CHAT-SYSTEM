#include "../inc/client.h"

// Global variables
char my_ip[16];
char my_username[6];

// Get Client IP address based on the connected socket
void get_client_ip(int socket) {
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);

    if (getsockname(socket, (struct sockaddr *)&local_addr, &addr_len) < 0) {
        perror("Failed to get socket name");
        return;
    }

    inet_ntop(AF_INET, &local_addr.sin_addr, my_ip, sizeof(my_ip));

    if (strcmp(my_ip, "127.0.0.1") == 0) {
        struct ifaddrs *ifaddr, *ifa;

        if (getifaddrs(&ifaddr) != -1) {
            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
                    struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                    char *ip = inet_ntoa(addr->sin_addr);

                    if (strcmp(ip, "127.0.0.1") != 0) {
                        strncpy(my_ip, ip, sizeof(my_ip) - 1);
                        break;
                    }
                }
            }
            freeifaddrs(ifaddr);
        }
    }
}