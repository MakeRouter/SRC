#include <stdio.h>
#include "device_info.h"

// run_http_server()는 http_server.c에 구현
void run_http_server();

int main() {
    printf("[INIT] Starting AP Web Server...\n");
    run_http_server();
    return 0;
}
