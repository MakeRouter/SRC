# RPRouter v1.000

## Project File Download
```
# 0. git이 없을 경우
sudo apt install git

# 1. 저장소 다운로드
git clone https://github.com/MakeRouter/SRC.git

# 2. 파일 이동 및 삭제
mv SRC/ap_server/ /home/pi && sudo rm -r SRC
```

---

## Directory Structure
```
/home/pi/ap_server/
├── Makefile
├── main.c
├── http_server.c
├── device_info.c
├── device_info.h
└── www/
    └── index.html
```

---

## Build & Run
```
# 0. 폴더 접속
cd ap_server

# 1. 빌드
make

# 2. 실행 (root 권한 필요)
sudo ./http_server

# 3. 웹 접속
http://192.168.50.1/

```

---

## 주요 파일 및 함수 요약

| 파일 | 주요 함수 | 설명 |
|------|------------|------|
| `main.c` | `main()` | HTTP 서버 초기화 및 루프 실행 |
| `http_server.c` | `serve_info_json()` | `/info.json` 요청 시 SSID, PW 반환 |
| 〃 | `serve_devices_json()` | `/devices.json` 요청 시 현재 DHCP 클라이언트 목록 반환 |
| `device_info.c` | `get_connected_devices()` | `/var/lib/misc/dnsmasq.leases` 파일 파싱 |
| 〃 | `read_wifi_info()` | SSID, 비밀번호 파일(`/etc/hostapd/hostapd.conf`) 읽기 |
| `device_info.h` | — | `struct device_info` 정의 및 함수 프로토타입 선언 |
| `www/index.html` | — | 라우터 대시보드 UI (5초마다 자동 새로고침, JS Fetch로 `/info.json`, `/devices.json` 요청) |

---

## HTTP Endpoints

| Endpoint | Method | Description |
|-----------|---------|--------------|
| `/` | GET | 대시보드 페이지 (`index.html`) |
| `/info.json` | GET | SSID, 비밀번호 정보 |
| `/devices.json` | GET | 현재 연결된 장치 리스트 |

---

## System Flow
```
[브라우저] → [Raspberry Pi HTTP Server]
   ├── /info.json     → hostapd.conf 읽음
   ├── /devices.json  → dnsmasq.leases 파싱
   └── /index.html    → JS에서 위 2개 JSON 주기적 fetch ( 5 sec )
```



