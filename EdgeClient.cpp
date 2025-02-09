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
#elif __APPLE__
    char buffer[128];
    size_t bufferSize = sizeof(buffer);
    sysctlbyname("machdep.cpu.brand_string", &buffer, &bufferSize, NULL, 0);
    modelName = buffer;
    int coreCount;
    size_t coreSize = sizeof(coreCount);
    sysctlbyname("hw.logicalcpu", &coreCount, &coreSize, NULL, 0);
    cpuCores = coreCount;
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
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmStat, &count) == KERN_SUCCESS) {
        usedMemory = totalMemory - (vmStat.free_count * sysconf(_SC_PAGESIZE));
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
