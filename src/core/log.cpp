#include "anchorpoint/core/log.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

namespace {
constexpr char kHeaderChar = '=';
constexpr int kHeaderLength = 60;
constexpr int kTabLength = 4;
}

namespace ap {

void LogHeader(const char* title) {
    const int titleLength = static_cast<int>(std::strlen(title));
    const int side = std::max(0, (kHeaderLength - titleLength) / 2);
    std::cout << std::string(side, kHeaderChar) << title
              << std::string(side, kHeaderChar) << std::endl;
}

void Log(const char* body, int level) {
    std::cout << std::string(level * kTabLength, ' ') << body << std::endl;
}

void LogError(const char* error, int code) {
    if (code == 0) {
        std::cout << error << std::endl;
        return;
    }
    std::cout << error << ": " << std::hex << code << std::dec << std::endl;
}

} // namespace ap
