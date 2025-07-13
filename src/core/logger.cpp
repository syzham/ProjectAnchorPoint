#include "core/logger.h"

void LogHeader(const char* title) {
    int side = (int)((headerLength - strlen(title)) / 2);
    std::cout << std::string(side, header) << title << std::string(side, header) << std::endl;
}

void Log(const char* body, int level) {
    std::cout << std::string(level * tabLength, tab) << body << std::endl;
}

void LogError(const char* error, int code) {
    if (!code) std::cout << error << std::endl;
    std::cout << error << ": " << std::hex << code << std::endl;
}
