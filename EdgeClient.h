#ifndef EDGECLIENT_H
#define EDGECLIENT_H

#include <string>

class EdgeClient {
public:
    std::string getCPUInfo();
    std::string getMemoryInfo();
    std::string getDiskInfo();
    std::string getSystemInfo();
};

#endif // EDGECLIENT_H