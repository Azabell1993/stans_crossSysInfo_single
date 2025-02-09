#include "EdgeClient.h"
#include <iostream>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

int main() {
    EdgeClient client;
    std::string systemInfo = client.getSystemInfo();
    spdlog::info("Fetching system info...");
    fmt::print("System Info: {}\n", systemInfo);
    spdlog::info("Fetching system info...");
    spdlog::info("System info displayed successfully.");

    
    
    std::string cpuInfo = client.getCPUInfo();
    std::string memoryInfo = client.getMemoryInfo();
    std::string diskInfo = client.getDiskInfo();

    spdlog::info("Fetching CPU info...");
    fmt::print("CPU Info: {}\n", cpuInfo);
    spdlog::info("CPU info displayed successfully.");

    spdlog::info("Fetching memory info...");
    fmt::print("Memory Info: {}\n", memoryInfo);
    spdlog::info("Memory info displayed successfully.");

    spdlog::info("Fetching Disk info...");
    fmt::print("Disk Info: {}\n", diskInfo);
    spdlog::info("Disk info displayed successfully.");
    
    /*
        또는
        std::string getCPUInfo();
        std::string getMemoryInfo();
        std::string getDiskInfo();
        로 개별 출력 가능
    */
    
    return 0;
}
