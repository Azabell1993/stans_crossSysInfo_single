/**
 * EdgeClient System Information Retrieval Library
 * 
 * @file EdgeClient.h
 * @brief 이 파일은 다양한 운영체제에서 시스템 정보를 수집하는 기능을 제공하는 클래스 EdgeClient의 헤더 파일입니다.
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

#ifndef EDGECLIENT_H
#define EDGECLIENT_H

#include <string>

/**
 * @class EdgeClient
 * @brief EdgeClient 클래스는 CPU, 메모리, 디스크 및 전체 시스템 정보를 가져오는 기능을 제공합니다.
 * 
 * @details
 * 이 클래스는 크로스플랫폼 환경에서 시스템 정보를 수집할 수 있도록 설계되었습니다.
 * getCPUInfo(), getMemoryInfo(), getDiskInfo(), getSystemInfo() 메서드를 통해 시스템 상태를 JSON 형식의 문자열로 반환합니다.
 */
class EdgeClient {
public:
    /**
     * @brief CPU 정보를 반환하는 함수
     * @return CPU 모델명, 코어 수 및 사용량을 포함한 JSON 문자열
     */
    std::string getCPUInfo();

    /**
     * @brief 메모리 정보를 반환하는 함수
     * @return 총 메모리 및 사용된 메모리를 포함한 JSON 문자열
     */
    std::string getMemoryInfo();

    /**
     * @brief 디스크 정보를 반환하는 함수
     * @return 각 드라이브의 총 용량 및 사용 가능한 용량을 포함한 JSON 문자열
     */
    std::string getDiskInfo();

    /**
     * @brief 전체 시스템 정보를 반환하는 함수
     * @return CPU, 메모리, 디스크 정보를 포함한 JSON 문자열
     */
    std::string getSystemInfo();
};

#endif // EDGECLIENT_H
