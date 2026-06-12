#ifndef ANCHORPOINT_CORE_API_H
#define ANCHORPOINT_CORE_API_H

#if defined(ANCHORPOINT_SHARED)
    #if defined(_WIN32)
        #if defined(ANCHORPOINT_EXPORTS)
            #define AP_API __declspec(dllexport)
        #else
            #define AP_API __declspec(dllimport)
        #endif
    #else
        #define AP_API __attribute__((visibility("default")))
    #endif
#else
    #define AP_API
#endif

#endif //ANCHORPOINT_CORE_API_H
