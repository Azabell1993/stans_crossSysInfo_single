﻿# 시스템 정보 수집기
 ![image](https://github.com/user-attachments/assets/ea034595-8fe5-4b48-9cca-ab60727ad1bc)  

## 개요
CrossSysInfo는 Windows와 Linux를 지원하는 크로스플랫폼 시스템 정보 수집 도구입니다. CPU, 메모리 및 디스크 사용량에 대한 정보를 수집하여 출력하며, 각 플랫폼에 맞는 빌드 스크립트를 제공합니다.
- `Window`
- `Linux`
- `MacOS`

## 결과물부터 보기(MacOS)
- CMake는 (`MacOS`, `Linux`, `Window`) 공통으로 제공
https://github.com/Azabell1993/stans_crossSysInfo_single/blob/2a6c87ab76e6a2e1e407f65c988936d8de2ca420/CMakeLists.txt#L1-L72

- 제공되는 빌드 스크립트 중 아키텍처 별 컴파일 부분(case by MacOS)
https://github.com/Azabell1993/stans_crossSysInfo_single/blob/2a6c87ab76e6a2e1e407f65c988936d8de2ca420/mac_build.sh#L27-L40

- 빌드
<img width="815" alt="스크린샷 2025-02-10 오전 10 01 37" src="https://github.com/user-attachments/assets/a09278c2-8ffb-4d10-ab38-fef41e02a7ea" />

- 생성된 결과물(리눅스, 윈도우도 운영체제별 아키텍처에 맞게 생성이 됩니다.)
<img width="271" alt="스크린샷 2025-02-10 오전 10 04 38" src="https://github.com/user-attachments/assets/b7307a7f-ff1c-42cd-8740-154381f0de06" />

- 결과 출력(`./Sample`)
<img width="1269" alt="스크린샷 2025-02-10 오전 10 01 55" src="https://github.com/user-attachments/assets/a0298575-ffe6-486e-b913-bc3ea0cf0f66" />  

- `Sample.cpp`
https://github.com/Azabell1993/stans_crossSysInfo_single/blob/2a6c87ab76e6a2e1e407f65c988936d8de2ca420/utility/Sample.cpp#L1-L80  

## utility 하위의 lib에는 각 운영체제 아키텍처별 컴파일된 결과물이 표시되어있음. (스크립트에 통합으로 빌드되나, 별도로 라이브러리 표시)
<img width="466" alt="스크린샷 2025-02-10 오후 12 13 56" src="https://github.com/user-attachments/assets/3bd59eb6-795d-4219-b2ea-bdaa18e66888" />  

## 로직 요약 설명
주어진 `EdgeClient` 클래스는 Windows, Linux, macOS에서 시스템 정보를 수집하여 JSON 형식으로 반환하는 기능을 제공합니다. 각 운영체제별로 CPU, 메모리, 디스크 정보를 수집하는 방법은 다음과 같습니다.

**1. CPU 정보 수집 (`getCPUInfo` 함수):**

- **Windows:**
  - 참조 : https://learn.microsoft.com/ko-kr/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsysteminfo
  - **코어 수 및 모델명:** `GetSystemInfo` 함수를 사용하여 `SYSTEM_INFO` 구조체를 채워 코어 수를 확인합니다.

  - 참조 : https://learn.microsoft.com/ko-kr/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtime
  - **CPU 사용량:** `GetSystemTimes` 함수를 사용하여 시스템의 유휴 시간(`idleTime`), 커널 시간(`kernelTime`), 사용자 시간(`userTime`)을 가져와 이전 값과 비교하여 사용량을 계산합니다.
- **Linux:**
  - **모델명 및 코어 수:** `/proc/cpuinfo` 파일을 읽어 `model name`과 `processor` 항목을 파싱하여 모델명과 코어 수를 얻습니다.
  - **CPU 사용량:** `/proc/stat` 파일에서 CPU 시간 정보를 읽어와 일정 시간 간격 후의 값과 비교하여 사용량을 계산합니다.

- **macOS:**
  - 참조 : https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/sysctlbyname.3.html
  - **모델명:** `sysctlbyname` 함수를 사용하여 `machdep.cpu.brand_string` 값을 가져옵니다.
  - **코어 수:** `sysctlbyname` 함수를 사용하여 `hw.logicalcpu` 값을 가져옵니다.

  - 참조 : https://developer.apple.com/documentation/kernel/1502546-host_statistics/
  - 참조 : https://www.green-coding.io/blog/cpu-utilization-mac/
  - **CPU 사용량:** `host_statistics` 함수를 사용하여 `host_cpu_load_info_data_t` 구조체의 값을 통해 사용량을 계산합니다.

**2. 메모리 정보 수집 (`getMemoryInfo` 함수):**

- **Windows:**
  - 참조 : https://learn.microsoft.com/ko-kr/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex
  - `GlobalMemoryStatusEx` 함수를 사용하여 `MEMORYSTATUSEX` 구조체를 채워 총 물리 메모리(`ullTotalPhys`)와 사용 가능한 메모리(`ullAvailPhys`)를 얻습니다.
- **Linux:**
  - `sysinfo` 함수를 사용하여 `sysinfo` 구조체의 `totalram`과 `freeram` 값을 통해 총 메모리와 사용된 메모리를 계산합니다.
- **macOS:**
  - 참조 : https://support.apple.com/ko-kr/guide/activity-monitor/actmntr1004/mac
  - 참조 : https://github.com/Kyome22/SystemInfoKit/blob/49ddfa9314101d599cc9a07152dfe52760e53540/Sources/SystemInfoKit/Repository/MemoryRepository.swift#L25-L34
  - 참조 : https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/sysctlbyname.3.html
  - `sysctlbyname` 함수를 사용하여 `hw.memsize` 값을 통해 총 메모리를 얻습니다.
  - `host_statistics64` 함수를 사용하여 `vm_statistics64_data_t` 구조체의 `active_count`, `inactive_count`, `wire_count` 값을 통해 사용된 메모리를 계산합니다.

**3. 디스크 정보 수집 (`getDiskInfo` 함수):**

- **Windows:**
  - 참조 : https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-getdiskfreespaceexa
  - `GetDiskFreeSpaceEx` 함수를 사용하여 드라이브의 총 용량(`totalBytes.QuadPart`)과 사용 가능한 용량(`freeBytes.QuadPart`)을 얻습니다.
- **Linux 및 macOS:**
  - `statvfs` 함수를 사용하여 파일 시스템의 총 블록 수(`f_blocks`)와 사용 가능한 블록 수(`f_bavail`), 그리고 각 블록의 크기(`f_frsize`)를 통해 디스크 정보를 계산합니다.

이와 같은 방식으로 각 운영체제에서 제공하는 API와 시스템 파일을 활용하여 CPU, 메모리, 디스크 정보를 수집하고 있습니다. 

## 기능
- CPU 모델, 코어 개수, 사용량 수집
- 총 메모리 및 사용 중인 메모리 정보 수집
- 디스크 정보(총 용량 및 사용 가능 용량) 수집
- Windows 및 Linux 지원
- `EdgeClient` 클래스를 활용하여 구조화된 시스템 정보 제공

## 프로젝트 구조
```
CrossSysInfo/
├── EdgeClient.h           # 시스템 정보 클래스 헤더 파일
├── EdgeClient.cpp         # 시스템 정보 수집 구현 파일
├── utility/
│   ├── Sample.cpp         # EdgeClient 사용 예제(fmt, spdlog 의존성 사용)
├── build.sh               # Linux 빌드 스크립트
├── build.bat              # Windows 빌드 스크립트
├── linux_result/          # Linux 빌드 결과 디렉터리
│   ├── build/             # 빌드 아티팩트
│   ├── output/            # 최종 실행 파일 저장 디렉터리
├── macos_result/          # MacOS 빌드 결과 디렉터리
│   ├── build/             # 빌드 아티팩트
│   ├── output/            # 최종 실행 파일 저장 디렉터리
├── window_result/         # Windows 빌드 결과 디렉터리
│   ├── build/             # 빌드 아티팩트
│   ├── output/            # 최종 실행 파일 저장 디렉터리
```

## 의존성

#### **Linux**
- `g++` (C++20 지원 GCC 필요)
- `fmt` 및 `spdlog` 라이브러리 (Linux용 패키지 설치 필요)

#### **Windows**
- Microsoft Visual Studio 2022 (`vcvars64.bat` 활성화 필요)
- `vcpkg`로 설치한 `fmt` 및 `spdlog` 라이브러리
- `$env:PATH = "C:\vcpkg\installed\x64-windows\bin;" + $env:PATH` (실행 전에 필수 설정)

#### **macOS**
- `clang++` (C++20 지원 Clang 필요, 기본적으로 macOS에 포함)
- `fmt` 및 `spdlog` 라이브러리
  - Homebrew를 사용하여 설치:
    ```bash
    brew install fmt spdlog
    ```
- 라이브러리 경로를 컴파일 및 링크에 포함해야 함:
  - 컴파일 시:
    ```bash
    -I/usr/local/include
    ```
  - 링크 시:
    ```bash
    -L/usr/local/lib -lfmt -lspdlog
    ```

---

## 빌드 및 실행 방법

### ! 일러두기
- CPU 정보 조회에서 사용량 같은 경우에는 `std::string systemInfo = client.getSystemInfo();`으로 한번에 조회하는 것보다, `std::string cpuInfo = client.getCPUInfo();` 개별 함수로 보는게 더 정확합니다.

### Linux
#### 빌드
```sh
./build.sh
```
빌드가 완료되면 `linux_result/output/` 디렉터리에 다음 파일들이 생성됩니다.
- `libEdgeClient.a`
- `libEdgeClient.so`
- `Sample`

#### 실행
```sh
cd linux_result/output/
./Sample
```
#### 출력
```
azabell@DESKTOP-BNUTRL1:/mnt/c/Users/jeewo/Desktop/crossSysInfo/utils/linux_result/output$ ./Sample 
[2025-02-09 20:29:14.891] [info] Fetching system info...
System Info: {"CPU": {"Model": "AMD Ryzen 5 7530U with Radeon Graphics", "Cores": 12, "Usage": 0}, "Memory": {"Total": 8014446592, "Used": 2146643968}"Disk": {"/": {"Total": 1081101176832, "Available": 1017010249728}, "/home": {"Total": 1081101176832, "Available": 1017010249728}, "/var": {"Total": 1081101176832, "Available": 1017010249728}, "/tmp": {"Total": 1081101176832, "Available": 1017010249728}}}
[2025-02-09 20:29:15.392] [info] System info displayed successfully.
[2025-02-09 20:29:15.392] [info] Fetching CPU info...
CPU Info: "Model": "AMD Ryzen 5 7530U with Radeon Graphics", "Cores": 12, "Usage": 0.332779
[2025-02-09 20:29:15.893] [info] CPU info displayed successfully.
[2025-02-09 20:29:15.893] [info] Fetching memory info...
Memory Info: "Total": 8014446592, "Used": 2146643968
[2025-02-09 20:29:15.893] [info] Memory info displayed successfully.
[2025-02-09 20:29:15.893] [info] Fetching Disk info...
Disk Info: {"/": {"Total": 1081101176832, "Available": 1017010249728}, "/home": {"Total": 1081101176832, "Available": 1017010249728}, "/var": {"Total": 1081101176832, "Available": 1017010249728}, "/tmp": {"Total": 1081101176832, "Available": 1017010249728}}
[2025-02-09 20:29:15.893] [info] Disk info displayed successfully.
```

----


### MacOS
#### 빌드
```sh
./mac_build.sh
```
빌드가 완료되면 'macos_result/output/' 디렉터리에 다음 파일들이 생성됩니다.
- `libEdgeClient.a`
- `libEdgeClient.dylib`
- `Sample`

#### 실행
``` sh
cd macos_result/output/
./Sample
```

#### 출력
```
azabell@azabellui-MacBookPro output % ./Sample
DEBUG: Total Memory = 34359738368, Used Memory = 31873073152
[2025-02-09 20:01:34.616] [info] Fetching system info...
System Info: {"CPU": {"Model": "Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz", "Cores": 12, "Usage": 6.21858}, "Memory": {"Total": 34359738368, "Used": 31873073152}"Disk": {"/": {"Total": 499963174912, "Available": 243216470016}, "/System/Volumes/Data": {"Total": 499963174912, "Available": 243216470016}}}
[2025-02-09 20:01:34.617] [info] Fetching system info...
[2025-02-09 20:01:34.617] [info] System info displayed successfully.
DEBUG: Total Memory = 34359738368, Used Memory = 31873183744
[2025-02-09 20:01:34.617] [info] Fetching CPU info...
CPU Info: "Model": "Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz", "Cores": 12, "Usage": 6.21858
[2025-02-09 20:01:34.617] [info] CPU info displayed successfully.
[2025-02-09 20:01:34.617] [info] Fetching memory info...
Memory Info: "Total": 34359738368, "Used": 31873183744
[2025-02-09 20:01:34.617] [info] Memory info displayed successfully.
[2025-02-09 20:01:34.617] [info] Fetching Disk info...
Disk Info: {"/": {"Total": 499963174912, "Available": 243216470016}, "/System/Volumes/Data": {"Total": 499963174912, "Available": 243216470016}}
[2025-02-09 20:01:34.617] [info] Disk info displayed successfully.
```

----

### Windows
#### 빌드
```bat
build.bat
```
빌드가 완료되면 `window_result/output/` 디렉터리에 다음 파일들이 생성됩니다.
- `EdgeClient.dll`
- `EdgeClient.lib`
- `Sample.exe`

#### 실행
```powershell
cd window_result/output/
./Sample.exe
```
#### 출력
```
PS C:\Users\jeewo\Desktop\crossSysInfo\utils\window_result\output> .\Sample.exe
[2025-02-09 20:29:49.206] [info] Fetching system info...
System Info: {"CPU": {"Model": "Windows CPU", "Cores": 12, "Usage": -nan(ind)}, "Memory": {"Total": 16539570176, "Used": 11264299008}"Disk": {"C:\": {"Total": 511176601600, "Available": 349753446400}, "D:\": {"Total": 0, "Available": 0}}}   
[2025-02-09 20:29:49.208] [info] System info displayed successfully.
[2025-02-09 20:29:49.208] [info] Fetching CPU info...
CPU Info: "Model": "Windows CPU", "Cores": 12, "Usage": -nan(ind)
[2025-02-09 20:29:49.209] [info] CPU info displayed successfully.
[2025-02-09 20:29:49.209] [info] Fetching memory info...
Memory Info: "Total": 16539570176, "Used": 11264323584
[2025-02-09 20:29:49.209] [info] Memory info displayed successfully.
[2025-02-09 20:29:49.210] [info] Fetching Disk info...
Disk Info: {"C:\": {"Total": 511176601600, "Available": 349753446400}, "D:\": {"Total": 0, "Available": 0}}
[2025-02-09 20:29:49.210] [info] Disk info displayed successfully.
```

## 라이선스
이 프로젝트는 MIT 라이선스로 배포됩니다.
