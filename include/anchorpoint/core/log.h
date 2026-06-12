#ifndef ANCHORPOINT_CORE_LOG_H
#define ANCHORPOINT_CORE_LOG_H

#include "anchorpoint/core/api.h"

namespace ap {

AP_API void LogHeader(const char* title);
AP_API void Log(const char* body, int level = 0);
AP_API void LogError(const char* error, int code = 0);

} // namespace ap

#endif //ANCHORPOINT_CORE_LOG_H
