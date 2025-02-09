# CrossSysInfo

## 개요
CrossSysInfo는 Windows와 Linux를 지원하는 크로스플랫폼 시스템 정보 수집 도구입니다. CPU, 메모리 및 디스크 사용량에 대한 정보를 수집하여 출력하며, 각 플랫폼에 맞는 빌드 스크립트를 제공합니다.

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
├── window_result/         # Windows 빌드 결과 디렉터리
│   ├── build/             # 빌드 아티팩트
│   ├── output/            # 최종 실행 파일 저장 디렉터리
```

## 의존성
### Linux
- `g++` (C++20 지원 GCC 필요)
- `fmt` 및 `spdlog` 라이브러리 (Linux용 패키지 설치 필요)

### Windows
- Microsoft Visual Studio 2022 (`vcvars64.bat` 활성화 필요)
- vcpkg로 설치한 fmt 및 spdlog 라이브러리
- $env:PATH = "C:\vcpkg\installed\x64-windows\bin;" + $env:PATH (콘솔 필수 명령어)


## 빌드 및 실행 방법
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
[2025-02-09 19:16:50.897] [info] Fetching system info...
System Info: {"CPU": {"Model": "AMD Ryzen 5 7530U with Radeon Graphics", "Cores": 12, "Usage": 0}, "Memory": {"Total": 8014450688, "Used": 2126569472}"Disk": {"/": {"Total": 1081101176832, "Available": 1017011507200}, "/home": {"Total": 1081101176832, "Available": 1017011507200}, "/var": {"Total": 1081101176832, "Available": 1017011507200}, "/tmp": {"Total": 1081101176832, "Available": 1017011507200}}}
[2025-02-09 19:16:50.923] [info] Fetching system info...
[2025-02-09 19:16:50.923] [info] System info displayed successfully.
[2025-02-09 19:16:50.923] [info] Fetching CPU info...
CPU Info: "Model": "AMD Ryzen 5 7530U with Radeon Graphics", "Cores": 12, "Usage": 0
[2025-02-09 19:16:50.923] [info] CPU info displayed successfully.
[2025-02-09 19:16:50.923] [info] Fetching memory info...
Memory Info: "Total": 8014450688, "Used": 2126827520
[2025-02-09 19:16:50.923] [info] Memory info displayed successfully.
[2025-02-09 19:16:50.923] [info] Fetching Disk info...
Disk Info: {"/": {"Total": 1081101176832, "Available": 1017011507200}, "/home": {"Total": 1081101176832, "Available": 1017011507200}, "/var": {"Total": 1081101176832, "Available": 1017011507200}, "/tmp": {"Total": 1081101176832, "Available": 1017011507200}}
[2025-02-09 19:16:50.923] [info] Disk info displayed successfully.
azabell@DESKTOP-BNUTRL1:/mnt/c/Users/jeewo/Desktop/crossSysInfo/utils
```

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
[2025-02-09 19:16:06.350] [info] Fetching system info...
System Info: {"CPU": {"Model": "Windows CPU", "Cores": 12, "Usage": 0}, "Memory": {"Total": 16539570176, "Used": 10487701504}"Disk": {"C:\": {"Total": 511176601600, "Available": 350195191808}, "D:\": {"Total": 0, "Available": 0}}}
[2025-02-09 19:16:06.351] [info] Fetching system info...
[2025-02-09 19:16:06.351] [info] System info displayed successfully.
[2025-02-09 19:16:06.351] [info] Fetching CPU info...
CPU Info: "Model": "Windows CPU", "Cores": 12, "Usage": 0
[2025-02-09 19:16:06.352] [info] CPU info displayed successfully.
[2025-02-09 19:16:06.352] [info] Fetching memory info...
Memory Info: "Total": 16539570176, "Used": 10487726080
[2025-02-09 19:16:06.352] [info] Memory info displayed successfully.
[2025-02-09 19:16:06.352] [info] Fetching Disk info...
Disk Info: {"C:\": {"Total": 511176601600, "Available": 350195191808}, "D:\": {"Total": 0, "Available": 0}}
[2025-02-09 19:16:06.352] [info] Disk info displayed successfully.
```

## 라이선스
이 프로젝트는 MIT 라이선스로 배포됩니다.
