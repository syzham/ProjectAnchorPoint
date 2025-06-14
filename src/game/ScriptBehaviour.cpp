#include "game/ScriptBehaviour.h"

using hostfxr_initialize_for_runtime_config_fn = int(*)(const wchar_t*, const hostfxr_initialize_parameters*, hostfxr_handle*);
using hostfxr_get_runtime_delegate_fn = int(*)(hostfxr_handle, hostfxr_delegate_type, void**);
using hostfxr_close_fn = int(*)(hostfxr_handle);
hostfxr_handle cxt = nullptr;
hostfxr_close_fn close_fn;
void (*update_fn)();
void (*create_fn)(const char*);
void (*setPointer_fn)(const char*, void*);
void (*setVector3_fn)(const char*, float[3]);
void (*setFloat_fn)(const char*, float);

template <typename T>
T load_function(HMODULE hmod, const char* name) {
    return reinterpret_cast<T>(GetProcAddress(hmod, name));
}

int InitHost() {
    wchar_t hostfxr_path[MAX_PATH];
    size_t size = sizeof(hostfxr_path) / sizeof(wchar_t);
    if (get_hostfxr_path(hostfxr_path, &size, nullptr) != 0) {
        std::cerr << "Failed to find hostfxr.dll" << std::endl;
        return -1;
    }

    HMODULE hmod = LoadLibraryW(hostfxr_path);
    if (!hmod) {
        std::cerr << "Failed to load hostfxr.dll" << std::endl;
        return -1;
    }

    auto init_fn = load_function<hostfxr_initialize_for_runtime_config_fn>(hmod, "hostfxr_initialize_for_runtime_config");
    auto get_delegate_fn = load_function<hostfxr_get_runtime_delegate_fn>(hmod, "hostfxr_get_runtime_delegate");
    close_fn = load_function<hostfxr_close_fn>(hmod, "hostfxr_close");

    const wchar_t* config_path = L"GameScripts.runtimeconfig.json";
    int rc = init_fn(config_path, nullptr, &cxt);
    if (rc != 0 || cxt == nullptr) {
        std::cerr << "hostfxr_initialize_for_runtime_config failed " << rc << std::endl;
        return -1;
    }
    std::cout << "Initialized .NET host" << std::endl;

    load_assembly_and_get_function_pointer_fn load_assembly_fn = nullptr;
    rc = get_delegate_fn(
            cxt,
            hdt_load_assembly_and_get_function_pointer,
            (void**)&load_assembly_fn
    );
    if (rc != 0 || load_assembly_fn == nullptr) {
        std::cerr << "Failed to load assembly and get function pointer" << std::endl;
        close_fn(cxt);
        return -1;
    }

    void* createFunc = nullptr;
    void* updateFunc = nullptr;
    void* setPointerFunc = nullptr;
    void* setVector3Func = nullptr;
    void* setInputManagerFunc = nullptr;
    void* setFloatFunc = nullptr;

    wchar_t fullAssemblyPath[MAX_PATH];
    GetFullPathNameW(L"GameScripts.dll", MAX_PATH, fullAssemblyPath, nullptr);
    std::wcout << fullAssemblyPath << std::endl;

    rc = load_assembly_fn(fullAssemblyPath, L"GameScripts.MyScript, GameScripts", L"CreateScript", L"GameScripts.MyScript+CreateScriptDelegate, GameScripts", nullptr, (void**)&createFunc);
    if (rc != 0 || createFunc == nullptr) {
        std::cerr << "Failed to load create script function: " << std::hex << rc << std::endl;
        close_fn(cxt);
        return -1;
    }

    rc = load_assembly_fn(fullAssemblyPath, L"GameScripts.MyScript, GameScripts", L"Update", UNMANAGEDCALLERSONLY_METHOD, nullptr, (void**)&updateFunc);
    if (rc != 0 || updateFunc == nullptr) {
        std::cerr << "Failed to load update function: " << std::hex << rc << std::endl;
        close_fn(cxt);
        return -1;
    }

    rc = load_assembly_fn(fullAssemblyPath, L"GameScripts.MyScript, GameScripts", L"SetPointer", L"GameScripts.MyScript+SetPointerDelegate, GameScripts", nullptr, (void**)&setPointerFunc);
    if (rc != 0 || updateFunc == nullptr) {
        std::cerr << "Failed to load setPointer function: " << std::hex << rc << std::endl;
        close_fn(cxt);
        return -1;
    }

    rc = load_assembly_fn(fullAssemblyPath, L"GameScripts.MyScript, GameScripts", L"SetVector3", L"GameScripts.MyScript+SetVector3Delegate, GameScripts", nullptr, (void**)&setVector3Func);
    if (rc != 0 || updateFunc == nullptr) {
        std::cerr << "Failed to load setVector3 function: " << std::hex << rc << std::endl;
        close_fn(cxt);
        return -1;
    }

    rc = load_assembly_fn(fullAssemblyPath, L"GameScripts.MyScript, GameScripts", L"SetInputManager", UNMANAGEDCALLERSONLY_METHOD, nullptr, (void**)&setInputManagerFunc);
    if (rc != 0 || updateFunc == nullptr) {
        std::cerr << "Failed to load setVector3 function: " << std::hex << rc << std::endl;
        close_fn(cxt);
        return -1;
    }

    rc = load_assembly_fn(fullAssemblyPath, L"GameScripts.MyScript, GameScripts", L"SetFloat", L"GameScripts.MyScript+SetFloatDelegate, GameScripts", nullptr, (void**)&setFloatFunc);
    if (rc != 0 || updateFunc == nullptr) {
        std::cerr << "Failed to load setVector3 function: " << std::hex << rc << std::endl;
        close_fn(cxt);
        return -1;
    }

    std::cout << "Loaded function" << std::endl;
    create_fn = (create_entry_point)createFunc;
    update_fn = (update_entry_point)updateFunc;
    setPointer_fn = (setPointer_entry_point)setPointerFunc;
    setVector3_fn = (setVector3_entry_point)setVector3Func;
    setFloat_fn = (setFloat_entry_point)setFloatFunc;


    typedef void (CORECLR_DELEGATE_CALLTYPE* setInputManager_entry_point)(void*);
    auto setInputManager_fn = (setInputManager_entry_point)setInputManagerFunc;
    setInputManager_fn(&InputManager::getInstance());

    return 0;
}

void UpdateScript() {
    update_fn();
}

void CloseHost() {
    close_fn(cxt);
}