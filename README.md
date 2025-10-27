# RPRouter v1.001

## Install
```
sudo apt install -y liboping-dev arping git
```

## Project File Download
```
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
| `http_server.c` | `send_info_json()` | `/info.json` 요청 시 SSID, Password 반환 |
| 〃 | `send_devices_json()` | `/devices.json` 요청 시 현재 DHCP 클라이언트 목록 반환 |
| 〃 | `send_status_json()` | `/status.json` 요청 시 CPU 온도, 업타임, 인터넷 연결 상태 반환 |
| 〃 | `handle_block_mac()` | `/block?mac=` 요청 시 해당 MAC 주소 차단 (iptables DROP) |
| `device_info.c` | `load_connected_devices()` | `/var/lib/misc/dnsmasq.leases` 파일 파싱 및 병렬 ping 확인 |
| 〃 | `get_ap_info()` | `/etc/hostapd/hostapd.conf` 파일에서 SSID / 비밀번호 추출 |
| `device_info.h` | — | `DeviceInfo` 구조체 및 함수 프로토타입 정의 |
| `www/index.html` | — | 웹 대시보드 (JS fetch로 JSON 갱신, 라이트/다크 테마, 새로고침 모달 포함) |


---

## HTTP Endpoints

| Endpoint | Method | Description |
|-----------|---------|-------------|
| `/` | GET | 대시보드 페이지 (`index.html`) |
| `/info.json` | GET | SSID / Password 정보 반환 |
| `/devices.json` | GET | 연결된 DHCP 클라이언트 목록 |
| `/status.json` | GET | CPU 온도, 업타임, 인터넷 연결 여부 |
| `/block?mac=` | GET | 특정 MAC 주소 차단 (iptables DROP) |

---

## System Flow
```
[브라우저] → [Raspberry Pi HTTP Server]
   ├── /info.json     → hostapd.conf 에서 SSID/PW 추출
   ├── /devices.json  → dnsmasq.leases 파싱 + ping alive 확인
   ├── /status.json   → CPU 온도, 업타임, 인터넷 연결 여부 반환
   ├── /block?mac=xx  → iptables DROP rule 추가
   └── /index.html    → JS로 위 API들을 10초마다 fetch

```

---

## 자동화 적용

```
# 1. 서비스 생성
sudo nano /etc/systemd/system/http_server.service

# 2. service 내용
[Unit]
Description=Raspberry Pi Router Dashboard Web Server
After=network.target
Wants=network.target

[Service]
Type=simple
ExecStart=/home/pi/ap_server/http_server
WorkingDirectory=/home/pi/ap_server
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target

# 3. 적용
sudo systemctl daemon-reload
sudo systemctl enable http_server
sudo systemctl start http_server
```

