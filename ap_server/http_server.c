#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "device_info.h"

#define PORT 80

// 간단한 MIME 지정 (필요 시 확장)
static const char* guess_mime(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "text/plain";
    if (!strcmp(ext, ".html")) return "text/html; charset=UTF-8";
    if (!strcmp(ext, ".json")) return "application/json";
    if (!strcmp(ext, ".css"))  return "text/css";
    if (!strcmp(ext, ".js"))   return "application/javascript";
    return "application/octet-stream";
}

// 정적 파일 전송
static void send_file(int client, const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        dprintf(client, "HTTP/1.1 404 Not Found\r\n\r\n");
        return;
    }

    dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", guess_mime(path));
    char buf[2048];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        write(client, buf, n);
    }
    fclose(fp);
}

// SSID/PASS JSON
static void send_info_json(int client) {
    char ssid[64] = "unknown";
    char pass[64] = "unknown";
    get_ap_info(ssid, sizeof(ssid), pass, sizeof(pass));


    dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
    dprintf(client, "{\"ssid\":\"%s\",\"password\":\"%s\"}", ssid, pass);
}

// 연결 기기 목록 JSON
static void send_devices_json(int client) {
    DeviceInfo devices[64];
    int n = load_connected_devices(devices, 64);

    dprintf(client, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n[");
    for (int i = 0; i < n; i++) {
        dprintf(client,
            "{\"ip\":\"%s\",\"mac\":\"%s\",\"name\":\"%s\"}%s",
            devices[i].ip, devices[i].mac, devices[i].name,
            (i == n - 1) ? "" : ",");
    }
    dprintf(client, "]");
}

// 간단 HTTP 서버: GET 라인만 파싱
void run_http_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return;
    }

    // TIME_WAIT 등에서 재바인드 가능하게
    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // ★중요: INADDR_ANY는 네트워크 바이트오더 필요

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 8) < 0) {
        perror("listen");
        close(server_fd);
        return;
    }

    printf("[HTTP] Server started on port %d\n", PORT);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        char req[1024] = {0};
        read(client_fd, req, sizeof(req) - 1);

        // 아주 단순한 라우팅: info.json / devices.json / /
        if (strstr(req, "GET /info.json") == req + 0) {
            send_info_json(client_fd);
        } else if (strstr(req, "GET /devices.json") == req + 0) {
            send_devices_json(client_fd);
        } else {
            // 기본: 대시보드
            send_file(client_fd, "/home/pi/ap_server/www/index.html");
        }
        close(client_fd);
    }
}
