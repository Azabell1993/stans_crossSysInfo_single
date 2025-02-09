#include "EdgeClient.h"
#include <iostream>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

/**
 * @brief 시스템 정보를 가져와 출력하는 프로그램
 * @details 이 프로그램은 시스템의 CPU, 메모리, 디스크 정보를 가져와 출력합니다.
 * EdgeClient 클래스를 활용하여 데이터를 수집하며, `spdlog`를 사용하여 로깅을 수행합니다.
 * @return int 프로그램 실행 상태 (성공: 0, 실패: 1)
 *
 * 이 프로그램은 시스템의 CPU, 메모리, 디스크 정보를 가져와 출력합니다.
 * EdgeClient 클래스를 활용하여 데이터를 수집하며, `spdlog`를 사용하여 로깅을 수행합니다.
 *
 * 예제 :
 * ```cpp
 * EdgeClient client;
 * std::string systemInfo = client.getSystemInfo();
 * fmt::print("System Info: {}\n", systemInfo);
 * ```
 */
int main() {
    try {
        EdgeClient client;

        // 📌 전체 시스템 정보 가져오기
        spdlog::info("Fetching system info...");
        std::string systemInfo;
        try {
            systemInfo = client.getSystemInfo();
            fmt::print("System Info: {}\n", systemInfo);
        } catch (const std::exception& e) {
            spdlog::error("Failed to fetch system info: {}", e.what());
            fmt::print("System Info: ERROR\n");
        }
        spdlog::info("System info displayed successfully.");

        // 📌 CPU 정보 가져오기
        spdlog::info("Fetching CPU info...");
        std::string cpuInfo;
        try {
            cpuInfo = client.getCPUInfo();
            fmt::print("CPU Info: {}\n", cpuInfo);
        } catch (const std::exception& e) {
            spdlog::error("Failed to fetch CPU info: {}", e.what());
            fmt::print("CPU Info: ERROR\n");
        }
        spdlog::info("CPU info displayed successfully.");

        // 📌 메모리 정보 가져오기
        spdlog::info("Fetching memory info...");
        std::string memoryInfo;
        try {
            memoryInfo = client.getMemoryInfo();
            fmt::print("Memory Info: {}\n", memoryInfo);
        } catch (const std::exception& e) {
            spdlog::error("Failed to fetch memory info: {}", e.what());
            fmt::print("Memory Info: ERROR\n");
        }
        spdlog::info("Memory info displayed successfully.");

        // 📌 디스크 정보 가져오기
        spdlog::info("Fetching Disk info...");
        std::string diskInfo;
        try {
            diskInfo = client.getDiskInfo();
            fmt::print("Disk Info: {}\n", diskInfo);
        } catch (const std::exception& e) {
            spdlog::error("Failed to fetch Disk info: {}", e.what());
            fmt::print("Disk Info: ERROR\n");
        }
        spdlog::info("Disk info displayed successfully.");

    } catch (const std::exception& e) {
        spdlog::critical("Unexpected error occurred: {}", e.what());
        return 1;
    }

    return 0;
}
