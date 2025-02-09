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

std::string EdgeClient::getCPUInfo() {
    std::ostringstream cpuInfo;
    std::string modelName;
    int cpuCores = 0;
    double cpuUsage = 0.0;

#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    cpuCores = sysInfo.dwNumberOfProcessors;
    modelName = "Windows CPU";

    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER idle, kernel, user;
        idle.LowPart = idleTime.dwLowDateTime; idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime; kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime; user.HighPart = userTime.dwHighDateTime;

        static ULARGE_INTEGER prevIdle = idle, prevKernel = kernel, prevUser = user;
        cpuUsage = 100.0 * (1.0 - (double)(idle.QuadPart - prevIdle.QuadPart) /
                            (kernel.QuadPart + user.QuadPart - prevKernel.QuadPart - prevUser.QuadPart));
        prevIdle = idle; prevKernel = kernel; prevUser = user;
    }
#elif __linux__
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

    cpuUsage = (1.0 - (double)(totalIdle2 - totalIdle) / (totalCpu2 - totalCpu)) * 100.0;
#elif __APPLE__
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
        cpuUsage = (double)usedTicks / totalTicks * 100.0;
    }
#endif
    
    cpuInfo << "\"Model\": \"" << modelName << "\", ";
    cpuInfo << "\"Cores\": " << cpuCores << ", ";
    cpuInfo << "\"Usage\": " << cpuUsage;
    return cpuInfo.str();
}

std::string EdgeClient::getMemoryInfo() {
    std::ostringstream memoryInfo;
    long long totalMemory = 0, usedMemory = 0;

#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    totalMemory = memInfo.ullTotalPhys;
    usedMemory = totalMemory - memInfo.ullAvailPhys;
#elif __linux__
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    totalMemory = memInfo.totalram * memInfo.mem_unit;
    usedMemory = (memInfo.totalram - memInfo.freeram) * memInfo.mem_unit;
#elif __APPLE__
    int64_t memSize;
    size_t memSizeLen = sizeof(memSize);
    sysctlbyname("hw.memsize", &memSize, &memSizeLen, NULL, 0);
    totalMemory = memSize;

    vm_statistics64_data_t vmStat;
    mach_msg_type_number_t count = sizeof(vmStat) / sizeof(integer_t);

    vm_size_t pageSize;
    host_page_size(mach_host_self(), &pageSize);

    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info_t)&vmStat, &count) != KERN_SUCCESS) {
        std::cerr << "Error: host_statistics64() failed to retrieve memory info." << std::endl;
    } else {
        // macOS에서는 free memory 외에도 wired, active, inactive memory를 사용된 메모리로 포함해야 한다.
        usedMemory = (vmStat.active_count + vmStat.inactive_count + vmStat.wire_count) * pageSize;
    }
#endif

    memoryInfo << "\"Total\": " << totalMemory << ", ";
    memoryInfo << "\"Used\": " << usedMemory;
    return memoryInfo.str();
}


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
        diskInfo << "\"" << disks[i] << "\": {";
        diskInfo << "\"Total\": " << totalDisk << ", ";
        diskInfo << "\"Available\": " << availableDisk << "}";
        if (i != disks.size() - 1) diskInfo << ", ";
    }
    diskInfo << "}";
    return diskInfo.str();
}

std::string EdgeClient::getSystemInfo() {
    std::ostringstream systemInfo;
    systemInfo << "{";
    systemInfo << "\"CPU\": {" << getCPUInfo() << "}, ";
    systemInfo << "\"Memory\": {" << getMemoryInfo() << "}";
    systemInfo << "\"Disk\": " << getDiskInfo();
    systemInfo << "}";
    return systemInfo.str();
}
