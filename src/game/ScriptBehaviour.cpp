#include "game/ScriptBehaviour.h"

using hostfxr_initialize_for_runtime_config_fn = int(*)(const wchar_t*, const hostfxr_initialize_parameters*, hostfxr_handle*);
using hostfxr_get_runtime_delegate_fn = int(*)(hostfxr_handle, hostfxr_delegate_type, void**);
using hostfxr_close_fn = int(*)(hostfxr_handle);
hostfxr_handle cxt = nullptr;
hostfxr_close_fn close_fn;
void (*update_fn)(float);
void (*fixedUpdate_fn)();
void (*create_fn)(const char*);
void (*setPointer_fn)(const char*, void*);
void (*setVector3_fn)(const char*, float[3]);
void (*setFloat_fn)(const char*, float);

template <typename T>
T load_function(HMODULE hmod, const char* name) {
    return reinterpret_cast<T>(GetProcAddress(hmod, name));
}

int LoadFunctions(const load_assembly_and_get_function_pointer_fn load, const char_t* fullAssemblyPath, const char_t* method, const char_t* delegate, void*& func) {
    if (int rc = load(fullAssemblyPath, L"GameScripts.MyScript, GameScripts", method, delegate, nullptr, static_cast<void**>(&func)); rc != 0 || func == nullptr) {
        LogError("Failed to Load Function", rc);
        close_fn(cxt);
        return -1;
    }

    return 0;
}

int InitHost() {
    wchar_t hostfxr_path[MAX_PATH];
    size_t size = sizeof(hostfxr_path) / sizeof(wchar_t);
    if (get_hostfxr_path(hostfxr_path, &size, nullptr) != 0) {
        LogError("Failed to find hostfxr.dll");
        return -1;
    }

    HMODULE hmod = LoadLibraryW(hostfxr_path);
    if (!hmod) {
        LogError("Failed to load hostfxr.dll");
        return -1;
    }

    auto init_fn = load_function<hostfxr_initialize_for_runtime_config_fn>(hmod, "hostfxr_initialize_for_runtime_config");
    auto get_delegate_fn = load_function<hostfxr_get_runtime_delegate_fn>(hmod, "hostfxr_get_runtime_delegate");
    close_fn = load_function<hostfxr_close_fn>(hmod, "hostfxr_close");

    const wchar_t* config_path = L"GameScripts.runtimeconfig.json";
    int rc = init_fn(config_path, nullptr, &cxt);
    if (rc != 0 || cxt == nullptr) {
        LogError("hostfxr_initialize_for_runtime_config failed",rc);
        return -1;
    }

    load_assembly_and_get_function_pointer_fn load_assembly_fn = nullptr;
    rc = get_delegate_fn(
            cxt,
            hdt_load_assembly_and_get_function_pointer,
            (void**)&load_assembly_fn
    );
    if (rc != 0 || load_assembly_fn == nullptr) {
        LogError("Failed to load assembly and get function pointer", rc);
        close_fn(cxt);
        return -1;
    }


    wchar_t fullAssemblyPath[MAX_PATH];
    GetFullPathNameW(L"GameScripts.dll", MAX_PATH, fullAssemblyPath, nullptr);

    void* createFunc = nullptr;
    LoadFunctions(load_assembly_fn, fullAssemblyPath, L"CreateScript", L"GameScripts.MyScript+CreateScriptDelegate, GameScripts", createFunc);
    create_fn = (create_entry_point)createFunc;

    void* updateFunc = nullptr;
    LoadFunctions(load_assembly_fn, fullAssemblyPath, L"Update", UNMANAGEDCALLERSONLY_METHOD, updateFunc);
    update_fn = (update_entry_point)updateFunc;

    void* fixedUpdateFunc = nullptr;
    LoadFunctions(load_assembly_fn, fullAssemblyPath, L"FixedUpdate", UNMANAGEDCALLERSONLY_METHOD, fixedUpdateFunc);
    fixedUpdate_fn = (fixedUpdate_entry_point)fixedUpdateFunc;

    void* setPointerFunc = nullptr;
    LoadFunctions(load_assembly_fn, fullAssemblyPath, L"SetPointer", L"GameScripts.MyScript+SetPointerDelegate, GameScripts", setPointerFunc);
    setPointer_fn = (setPointer_entry_point)setPointerFunc;

    void* setVector3Func = nullptr;
    LoadFunctions(load_assembly_fn, fullAssemblyPath, L"SetVector3", L"GameScripts.MyScript+SetVector3Delegate, GameScripts", setVector3Func);
    setVector3_fn = (setVector3_entry_point)setVector3Func;

    void* setFloatFunc = nullptr;
    LoadFunctions(load_assembly_fn, fullAssemblyPath, L"SetFloat", L"GameScripts.MyScript+SetFloatDelegate, GameScripts", setFloatFunc);
    setFloat_fn = (setFloat_entry_point)setFloatFunc;

    void* setInputManagerFunc = nullptr;
    LoadFunctions(load_assembly_fn, fullAssemblyPath, L"SetInputManager", UNMANAGEDCALLERSONLY_METHOD, setInputManagerFunc);

    typedef void (CORECLR_DELEGATE_CALLTYPE* setInputManager_entry_point)(void*);
    auto setInputManager_fn = (setInputManager_entry_point)setInputManagerFunc;
    setInputManager_fn(&InputManager::getInstance());

    return 0;
}

void UpdateScript(float deltaTime) {
    update_fn(deltaTime);
}

void FixedUpdateScript() {
    fixedUpdate_fn();
}

void CloseHost() {
    close_fn(cxt);
}