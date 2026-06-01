// monoSDK.cpp
#include "monoSDK.h"
#include "../data.h"

// ==================== 已用 API 定义 ====================
t_mono_get_root_domain              mono_get_root_domain = nullptr;
t_mono_thread_attach                mono_thread_attach = nullptr;
t_mono_assembly_get_image           mono_assembly_get_image = nullptr;
t_mono_image_get_name               mono_image_get_name = nullptr;
t_mono_assembly_foreach             mono_assembly_foreach = nullptr;
t_mono_class_from_name              mono_class_from_name = nullptr;
t_mono_class_get_method_from_name   mono_class_get_method_from_name = nullptr;
t_mono_compile_method               mono_compile_method = nullptr;
t_mono_class_get_field_from_name    mono_class_get_field_from_name = nullptr;
t_mono_field_get_value              mono_field_get_value = nullptr;
t_mono_field_set_value              mono_field_set_value = nullptr;
t_mono_object_get_class             mono_object_get_class = nullptr;
t_mono_gc_walk_heap                 mono_gc_walk_heap = nullptr;
t_mono_class_get_name               mono_class_get_name = nullptr;
t_mono_class_get_namespace          mono_class_get_namespace = nullptr;
t_mono_class_vtable                 mono_class_vtable = nullptr;
t_mono_field_static_get_value       mono_field_static_get_value = nullptr;

t_mono_domain_get                   mono_domain_get = nullptr;
t_mono_class_get_type               mono_class_get_type = nullptr;
t_mono_type_get_object              mono_type_get_object = nullptr;
t_mono_runtime_invoke               mono_runtime_invoke = nullptr;
t_mono_class_get_property_from_name mono_class_get_property_from_name = nullptr;
t_mono_property_get_set_method      mono_property_get_set_method = nullptr;
t_mono_property_get_get_method      mono_property_get_get_method = nullptr;
t_mono_object_unbox                 mono_object_unbox = nullptr;
t_mono_class_get_methods            mono_class_get_methods = nullptr;
t_mono_method_get_name              mono_method_get_name = nullptr;

t_mono_array_length                 mono_array_length = nullptr;
t_mono_array_get                    mono_array_get = nullptr;

MonoAssembly* g_AssemblyCSharp = nullptr;
MonoAssembly* g_unsafeAssem = nullptr;

bool InitMono()
{
    HMODULE monoModule = GetModuleHandleA("mono-2.0-bdwgc.dll");
    if (!monoModule)
        monoModule = GetModuleHandleA("mono.dll");
    if (!monoModule) {
        std::cout << "[!] Mono module not found" << std::endl;
        return false;
    }

    std::cout << "[+] Mono module: 0x" << std::hex << monoModule << std::dec << std::endl;

    mono_get_root_domain = GetMonoFunction<t_mono_get_root_domain>(monoModule, "mono_get_root_domain");
    mono_thread_attach = GetMonoFunction<t_mono_thread_attach>(monoModule, "mono_thread_attach");
    mono_assembly_get_image = GetMonoFunction<t_mono_assembly_get_image>(monoModule, "mono_assembly_get_image");
    mono_image_get_name = GetMonoFunction<t_mono_image_get_name>(monoModule, "mono_image_get_name");
    mono_assembly_foreach = GetMonoFunction<t_mono_assembly_foreach>(monoModule, "mono_assembly_foreach");
    mono_class_from_name = GetMonoFunction<t_mono_class_from_name>(monoModule, "mono_class_from_name");
    mono_class_get_method_from_name = GetMonoFunction<t_mono_class_get_method_from_name>(monoModule, "mono_class_get_method_from_name");
    mono_compile_method = GetMonoFunction<t_mono_compile_method>(monoModule, "mono_compile_method");
    mono_class_get_field_from_name = GetMonoFunction<t_mono_class_get_field_from_name>(monoModule, "mono_class_get_field_from_name");
    mono_field_get_value = GetMonoFunction<t_mono_field_get_value>(monoModule, "mono_field_get_value");
    mono_field_set_value = GetMonoFunction<t_mono_field_set_value>(monoModule, "mono_field_set_value");
    mono_object_get_class = GetMonoFunction<t_mono_object_get_class>(monoModule, "mono_object_get_class");
    //mono_gc_walk_heap = GetMonoFunction<t_mono_gc_walk_heap>(monoModule, "mono_gc_walk_heap");
    mono_class_get_name = GetMonoFunction<t_mono_class_get_name>(monoModule, "mono_class_get_name");
    mono_class_get_namespace = GetMonoFunction<t_mono_class_get_namespace>(monoModule, "mono_class_get_namespace");
    mono_class_vtable = GetMonoFunction<t_mono_class_vtable>(monoModule, "mono_class_vtable");
    mono_field_static_get_value = GetMonoFunction<t_mono_field_static_get_value>(monoModule, "mono_field_static_get_value");

    mono_domain_get = GetMonoFunction<t_mono_domain_get>(monoModule, "mono_domain_get");
    mono_class_get_type = GetMonoFunction<t_mono_class_get_type>(monoModule, "mono_class_get_type");
    mono_type_get_object = GetMonoFunction<t_mono_type_get_object>(monoModule, "mono_type_get_object");
    mono_runtime_invoke = GetMonoFunction<t_mono_runtime_invoke>(monoModule, "mono_runtime_invoke");
    mono_class_get_property_from_name = GetMonoFunction<t_mono_class_get_property_from_name>(monoModule, "mono_class_get_property_from_name");
    mono_property_get_set_method = GetMonoFunction<t_mono_property_get_set_method>(monoModule, "mono_property_get_set_method");
    mono_property_get_get_method = GetMonoFunction<t_mono_property_get_get_method>(monoModule, "mono_property_get_get_method");
    mono_object_unbox = GetMonoFunction<t_mono_object_unbox>(monoModule, "mono_object_unbox");
    mono_class_get_methods = GetMonoFunction<t_mono_class_get_methods>(monoModule, "mono_class_get_methods");
    mono_method_get_name = GetMonoFunction<t_mono_method_get_name>(monoModule, "mono_method_get_name");

    mono_array_length = GetMonoFunction<t_mono_array_length>(monoModule, "mono_array_length");
    mono_array_get = GetMonoFunction<t_mono_array_get>(monoModule, "mono_array_get");

    if (!mono_get_root_domain) {
        std::cout << "[!] Failed to get mono_get_root_domain" << std::endl;
        return false;
    }
    if (!mono_thread_attach) {
        std::cout << "[!] Failed to get mono_thread_attach" << std::endl;
        return false;
    }
    if (!mono_assembly_get_image) {
        std::cout << "[!] Failed to get mono_assembly_get_image" << std::endl;
        return false;
    }
    if (!mono_image_get_name) {
        std::cout << "[!] Failed to get mono_image_get_name" << std::endl;
        return false;
    }
    if (!mono_assembly_foreach) {
        std::cout << "[!] Failed to get mono_assembly_foreach" << std::endl;
        return false;
    }
    if (!mono_class_from_name) {
        std::cout << "[!] Failed to get mono_class_from_name" << std::endl;
        return false;
    }
    if (!mono_class_get_method_from_name) {
        std::cout << "[!] Failed to get mono_class_get_method_from_name" << std::endl;
        return false;
    }
    if (!mono_compile_method) {
        std::cout << "[!] Failed to get mono_compile_method" << std::endl;
        return false;
    }
    if (!mono_class_get_field_from_name) {
        std::cout << "[!] Failed to get mono_class_get_field_from_name" << std::endl;
        return false;
    }
    if (!mono_field_get_value) {
        std::cout << "[!] Failed to get mono_field_get_value" << std::endl;
        return false;
    }
    if (!mono_field_set_value) {
        std::cout << "[!] Failed to get mono_field_set_value" << std::endl;
        return false;
    }
    if (!mono_object_get_class) {
        std::cout << "[!] Failed to get mono_object_get_class" << std::endl;
        return false;
    }
    /*
    if (!mono_gc_walk_heap) {
        std::cout << "[!] Failed to get mono_gc_walk_heap" << std::endl;
        return false;
    }
    */
    if (!mono_class_get_name) {
        std::cout << "[!] Failed to get mono_class_get_name" << std::endl;
        return false;
    }
    if (!mono_class_get_namespace) {
        std::cout << "[!] Failed to get mono_class_get_namespace" << std::endl;
        return false;
    }
    if (!mono_class_vtable) {
        std::cout << "[!] Failed to get mono_class_vtable" << std::endl;
        return false;
    }
    if (!mono_field_static_get_value) {
        std::cout << "[!] Failed to get mono_field_static_get_value" << std::endl;
        return false;
    }
    if (!mono_domain_get) {
        std::cout << "[!] Failed to get mono_domain_get" << std::endl;
        return false;
    }
    if (!mono_class_get_type) {
        std::cout << "[!] Failed to get mono_class_get_type" << std::endl;
        return false;
    }
    if (!mono_type_get_object) {
        std::cout << "[!] Failed to get mono_type_get_object" << std::endl;
        return false;
    }

    if (!mono_runtime_invoke) {
        std::cout << "[!] Failed to get mono_runtime_invoke" << std::endl;
        return false;
    }
    if (!mono_class_get_property_from_name) {
        std::cout << "[!] Failed to get mono_class_get_property_from_name" << std::endl;
        return false;
    }
    if (!mono_property_get_set_method) {
        std::cout << "[!] Failed to get mono_property_get_set_method" << std::endl;
        return false;
    }
    if (!mono_property_get_get_method) {
        std::cout << "[!] Failed to get mono_property_get_get_method" << std::endl;
        return false;
    }
 

    std::cout << "[+] Mono exports loaded" << std::endl;
    return true;
}

void AssemblyCallback(MonoAssembly* assembly, void* userData)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    if (!image) return;
    const char* name = mono_image_get_name(image);

    if (strcmp(name, "Assembly-CSharp") == 0) {
        g_AssemblyCSharp = assembly;
        std::cout << "[+] Found Assembly-CSharp\n";
    }
    else if (strcmp(name, "UnityEngine.CoreModule") == 0) {
        monodata::coreImage = image;
        std::cout << "[+] Found UnityEngine.CoreModule\n";
    }
}

MonoReflectionType* GetTypeObject(MonoClass* klass) {
    MonoDomain* domain = mono_domain_get();
    if (!domain) return nullptr;
    MonoType* type = mono_class_get_type(klass);
    if (!type) return nullptr;
    return mono_type_get_object(domain, type);
}

MonoObject* FindObjectOfType(MonoClass* targetClass) {
    MonoClass* objClass = mono_class_from_name(monodata::image, "UnityEngine", "Object");
    if (!objClass) return nullptr;
    MonoMethod* method = mono_class_get_method_from_name(objClass, "FindObjectOfType", 1);
    if (!method) return nullptr;
    MonoReflectionType* typeObj = GetTypeObject(targetClass);
    if (!typeObj) return nullptr;
    void* args[1] = { typeObj };
    MonoObject* result = mono_runtime_invoke(method, nullptr, args, nullptr);
    return result;
}

MonoObject* GetStaticFieldObject(MonoClass* klass, const char* fieldName)
{
    if (!klass || !monodata::domain) return nullptr;
    MonoVTable* vtable = mono_class_vtable(monodata::domain, klass);
    if (!vtable) return nullptr;
    MonoClassField* field = mono_class_get_field_from_name(klass, fieldName);
    if (!field) return nullptr;
    MonoObject* obj = nullptr;
    mono_field_static_get_value(vtable, field, &obj);
    return obj;
}

std::wstring ReadMonoString(void* monoStr) {
    if (!monoStr) return L"";
    int32_t len = 0;
    memcpy(&len, (void*)((uintptr_t)monoStr + 0x10), sizeof(int32_t));
    if (len <= 0) return L"";
    wchar_t* buf = new wchar_t[len + 1];
    memcpy(buf, (void*)((uintptr_t)monoStr + 0x14), len * sizeof(wchar_t));
    buf[len] = L'\0';
    std::wstring result(buf);
    delete[] buf;
    return result;
}

std::string ReadMonoStringUtf8(void* monoStringPtr) {
    if (!monoStringPtr) return "";

    int32_t len = 0;
    memcpy(&len, (void*)((uintptr_t)monoStringPtr + 0x10), sizeof(int32_t));
    if (len <= 0) return "";
    wchar_t* wbuf = new wchar_t[len + 1];
    memcpy(wbuf, (void*)((uintptr_t)monoStringPtr + 0x14), len * sizeof(wchar_t));
    wbuf[len] = L'\0';
    int required = WideCharToMultiByte(CP_UTF8, 0, wbuf, len, nullptr, 0, nullptr, nullptr);
    std::string result(required, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wbuf, len, &result[0], required, nullptr, nullptr);
    delete[] wbuf;
    return result;
}