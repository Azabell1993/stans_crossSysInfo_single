/**
 * EdgeClient System Information Retrieval Libaray
 * 
 * @file EdgeClient.cpp
 * @brief 이 파일은 다양한 운영체제에서 시스템 정보를 수집하는 기능을 제공하는 클래스 EdgeClient의 구현부입니다.
 * 
 * @details
 * 이 라이브러리는 Windows, Linux, macOS에서 실행될 수 있도록 설계되었습니다.
 * CPU, 메모리, 디스크 등의 주요 시스템 정보를 JSON 형식의 문자열로 반환합니다.
 * 
 * 주요 기능:
 * - getCPUInfo(): CPU 모델명, 코어 수 및 사용량을 반환
 * - getMemoryInfo(): 총 메모리 및 사용된 메모리 크기를 반환
 * - getDiskInfo(): 각 드라이브의 총 용량 및 사용 가능한 용량을 반환
 * - getSystemInfo(): CPU, 메모리, 디스크 정보를 종합하여 JSON 형식으로 반환
 * 
 * 지원하는 운영체제:
 * - Windows (GetSystemInfo, GlobalMemoryStatusEx, GetDiskFreeSpaceEx 사용)
 * - Linux (/proc/stat, /proc/meminfo, sysinfo, statvfs 사용)
 * - macOS (sysctl, host_statistics64, statvfs 사용)
 * 
 * @author 박지우 (Azabell1993)
 * @date 2025-02-09
 * 
 * @note 이 코드는 오픈소스 라이선스 하에 배포될 수 있으며, 자유롭게 수정 및 배포할 수 있습니다.
 */

#include "EdgeClient.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#elif __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/host_info.h>
#include <sys/statvfs.h>
#endif

// CPU 정보를 가져오는 함수
std::string EdgeClient::getCPUInfo() {
    std::ostringstream cpuInfo;
    std::string modelName;
    int cpuCores = 0;
    double cpuUsage = 0.0;

#ifdef _WIN32
    // Windows에서 CPU 코어 수 및 모델 정보 가져오기
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    cpuCores = sysInfo.dwNumberOfProcessors;
    modelName = "Windows CPU";

    // CPU 사용량 계산
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER idle, kernel, user;
        idle.LowPart = idleTime.dwLowDateTime; idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime; kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime; user.HighPart = userTime.dwHighDateTime;

        static ULARGE_INTEGER prevIdle = idle, prevKernel = kernel, prevUser = user;
        // CPU 사용량 계산 공식: (1 - (idle 증가량 / 전체 증가량)) * 100
        cpuUsage = 100.0 * (1.0 - (double)(idle.QuadPart - prevIdle.QuadPart) /
                            (kernel.QuadPart + user.QuadPart - prevKernel.QuadPart - prevUser.QuadPart));
        prevIdle = idle; prevKernel = kernel; prevUser = user;
    }
#elif __linux__
    // Linux에서 CPU 모델명 및 코어 수 가져오기
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                modelName = line.substr(pos + 2);
            }
        }
        if (line.find("processor") != std::string::npos) {
            cpuCores++;
        }
    }

    // CPU 사용량 계산
    std::ifstream statfile("/proc/stat");
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    statfile >> line >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    unsigned long long totalIdle = idle + iowait;
    unsigned long long totalCpu = user + nice + system + idle + iowait + irq + softirq + steal;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::ifstream statfile2("/proc/stat");
    unsigned long long user2, nice2, system2, idle2, iowait2, irq2, softirq2, steal2;
    statfile2 >> line >> user2 >> nice2 >> system2 >> idle2 >> iowait2 >> irq2 >> softirq2 >> steal2;
    unsigned long long totalIdle2 = idle2 + iowait2;
    unsigned long long totalCpu2 = user2 + nice2 + system2 + idle2 + iowait2 + irq2 + softirq2 + steal2;

    // CPU 사용량 계산 공식: (1 - (idle 증가량 / 전체 증가량)) * 100
    cpuUsage = (1.0 - (double)(totalIdle2 - totalIdle) / (totalCpu2 - totalCpu)) * 100.0;
#elif __APPLE__
    // macOS에서 CPU 모델명 및 코어 수 가져오기
    char buffer[128];
    size_t bufferSize = sizeof(buffer);
    sysctlbyname("machdep.cpu.brand_string", &buffer, &bufferSize, NULL, 0);
    modelName = buffer;
    int coreCount;
    size_t coreSize = sizeof(coreCount);
    sysctlbyname("hw.logicalcpu", &coreCount, &coreSize, NULL, 0);
    cpuCores = coreCount;

    // macOS CPU 사용량 계산
    host_cpu_load_info_data_t cpuLoad;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuLoad, &count) == KERN_SUCCESS)
    {
        unsigned long long totalTicks = cpuLoad.cpu_ticks[CPU_STATE_USER] +
                                        cpuLoad.cpu_ticks[CPU_STATE_SYSTEM] +
                                        cpuLoad.cpu_ticks[CPU_STATE_IDLE];

        unsigned long long usedTicks = totalTicks - cpuLoad.cpu_ticks[CPU_STATE_IDLE];
        // CPU 사용량 계산 공식: (사용된 틱 수 / 전체 틱 수) * 100
        cpuUsage = (double)usedTicks / totalTicks * 100.0;
    }
#endif
    
    // JSON 형식으로 CPU 정보 구성
    cpuInfo << "\"Model\": \"" << modelName << "\", ";
    cpuInfo << "\"Cores\": " << cpuCores << ", ";
    cpuInfo << "\"Usage\": " << cpuUsage;
    return cpuInfo.str();
}


// 메모리 정보를 가져오는 함수
std::string EdgeClient::getMemoryInfo() {
    std::ostringstream memoryInfo;
    long long totalMemory = 0, usedMemory = 0;

#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    totalMemory = memInfo.ullTotalPhys;
    usedMemory = totalMemory - memInfo.ullAvailPhys;
    // 사용된 메모리 = 총 물리 메모리 - 사용 가능한 물리 메모리
#elif __linux__
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    totalMemory = memInfo.totalram * memInfo.mem_unit;
    usedMemory = (memInfo.totalram - memInfo.freeram) * memInfo.mem_unit;
    // totalram: 총 메모리 크기
    // freeram: 사용 가능한 메모리 크기
    // 사용된 메모리 = (총 메모리 - 사용 가능한 메모리) * 단위 크기(mem_unit)
#elif __APPLE__
    int64_t memSize;
    size_t memSizeLen = sizeof(memSize);
    sysctlbyname("hw.memsize", &memSize, &memSizeLen, NULL, 0);
    totalMemory = memSize;
    // macOS는 전체 메모리 크기만 제공하며, 사용된 메모리는 별도로 계산해야 함

    vm_statistics64_data_t vmStat;
    mach_msg_type_number_t count = sizeof(vmStat) / sizeof(integer_t);

    vm_size_t pageSize;
    host_page_size(mach_host_self(), &pageSize);

    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info_t)&vmStat, &count) != KERN_SUCCESS) {
        std::cerr << "Error: host_statistics64() failed to retrieve memory info." << std::endl;
    } else {
        /**
         * macOS에서는 단순한 free memory 외에도 wired, active, inactive memory를 사용된 메모리로 간주해야 한다.
         *
         * 1. Active Memory (활성 메모리)
         *    - vmStat.active_count
         *    - 현재 실행 중이거나, 자주 액세스되는 데이터가 저장된 페이지.
         *    - 운영체제에서 가장 자주 사용되는 페이지들이 포함되며, 필요하면 빠르게 접근할 수 있음.
         *
         * 2. Inactive Memory (비활성 메모리)
         *    - vmStat.inactive_count
         *    - 한 번 사용되었지만 현재는 사용되지 않는 페이지.
         *    - 즉, 프로세스가 사용했던 메모리인데, 현재는 직접적으로 사용되지 않고 캐시처럼 보관됨.
         *    - 다른 프로세스가 필요하면 다시 사용되거나, 새로운 데이터에 의해 덮어씌워질 수 있음.
         *
         * 3. Wired Memory (고정 메모리)
         *    - vmStat.wire_count
         *    - 운영 체제에서 반드시 유지해야 하는 페이지.
         *    - 이 메모리는 페이지 아웃(스왑)이 불가능하며, 커널이 중요한 작업을 수행하는 데 필요함.
         *    - 장치 드라이버, 커널 공간의 중요한 구조체 등이 여기에 포함됨.
         *
         * 사용된 메모리는 (active_count + inactive_count + wire_count) * pageSize 로 계산됨.
         */
        usedMemory = (vmStat.active_count + vmStat.inactive_count + vmStat.wire_count) * pageSize;
    }
#endif

    // JSON 형식으로 메모리 정보 구성
    memoryInfo << "\"Total\": " << totalMemory << ", ";
    memoryInfo << "\"Used\": " << usedMemory;
    return memoryInfo.str();
}


// 디스크 정보를 가져오는 함수
std::string EdgeClient::getDiskInfo() {
    std::ostringstream diskInfo;
    std::vector<std::string> disks;
#ifdef _WIN32
    disks = {"C:\\", "D:\\"};
#elif __linux__
    disks = {"/", "/home", "/var", "/tmp"};
#elif __APPLE__
    disks = {"/", "/System/Volumes/Data"};
#endif
    diskInfo << "{";
    for (size_t i = 0; i < disks.size(); i++) {
        long long totalDisk = 0, availableDisk = 0;
#ifdef _WIN32
        ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
        if (GetDiskFreeSpaceEx(disks[i].c_str(), &freeBytes, &totalBytes, &totalFreeBytes)) {
            totalDisk = totalBytes.QuadPart;
            availableDisk = freeBytes.QuadPart;
        }
#elif defined(__linux__) || defined(__APPLE__)
        struct statvfs diskStat;
        if (statvfs(disks[i].c_str(), &diskStat) == 0) {
            totalDisk = diskStat.f_blocks * diskStat.f_frsize;
            availableDisk = diskStat.f_bavail * diskStat.f_frsize;
        }
#endif
        // 디스크 사용량 계산 공식: 전체 블록 수 * 블록 크기
        // 사용 가능 디스크 용량: 사용 가능 블록 수 * 블록 크기
        diskInfo << "\"" << disks[i] << "\": {";
        diskInfo << "\"Total\": " << totalDisk << ", ";
        diskInfo << "\"Available\": " << availableDisk << "}";
        if (i != disks.size() - 1) diskInfo << ", ";
    }
    diskInfo << "}";
    return diskInfo.str();
}


// 시스템 정보를 가져오는 함수
std::string EdgeClient::getSystemInfo() {
    std::ostringstream systemInfo;
    systemInfo << "{";
    systemInfo << "\"CPU\": {" << getCPUInfo() << "}, ";
    systemInfo << "\"Memory\": {" << getMemoryInfo() << "}";
    systemInfo << "\"Disk\": " << getDiskInfo();
    systemInfo << "}";
    return systemInfo.str();
}
