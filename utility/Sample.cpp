#include "EdgeClient.h"
#include <iostream>

int main() {
    EdgeClient client;
    
    std::cout << "System Info: " << client.getSystemInfo() << std::endl;
    
    return 0;
}