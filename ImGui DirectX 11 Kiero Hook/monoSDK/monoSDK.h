// monoSDK.h
#pragma once

#include <Windows.h>
#include <iostream>

// ================== 不透明类型声明 ==================
typedef void MonoDomain;
typedef void MonoAssembly;
typedef void MonoImage;
typedef void MonoClass;
typedef void MonoMethod;
typedef void MonoClassField;
typedef void MonoObject;
typedef void MonoVTable;
typedef void MonoType;
typedef void MonoReflectionType;
typedef void MonoException;
typedef void MonoProperty;
struct MonoArray;

// ================== 函数指针类型定义 ==================
typedef MonoDomain* (*t_mono_get_root_domain)();
typedef void            (*t_mono_thread_attach)(MonoDomain* domain);
typedef MonoImage* (*t_mono_assembly_get_image)(MonoAssembly* assembly);
typedef const char* (*t_mono_image_get_name)(MonoImage* image);
typedef void            (*t_mono_assembly_foreach)(void(*func)(MonoAssembly*, void*), void* user_data);
typedef MonoClass* (*t_mono_class_from_name)(MonoImage* image, const char* name_space, const char* name);
typedef MonoMethod* (*t_mono_class_get_method_from_name)(MonoClass* klass, const char* name, int param_count);
typedef void* (*t_mono_compile_method)(MonoMethod* method);
typedef MonoClassField* (*t_mono_class_get_field_from_name)(MonoClass* klass, const char* name);
typedef void            (*t_mono_field_get_value)(void* obj, MonoClassField* field, void* value);
typedef void            (*t_mono_field_set_value)(void* obj, MonoClassField* field, void* value);
typedef MonoClass* (*t_mono_object_get_class)(MonoObject* obj);
typedef const char* (*t_mono_class_get_name)(MonoClass* klass);
typedef const char* (*t_mono_class_get_namespace)(MonoClass* klass);
typedef MonoVTable* (*t_mono_class_vtable)(MonoDomain* domain, MonoClass* klass);
typedef void            (*t_mono_field_static_get_value)(MonoVTable* vtable, MonoClassField* field, void* value);

typedef void (*t_mono_gc_walk_heap_func)(void** data, void* user_data);
typedef void (*t_mono_gc_walk_heap)(int flags, void(*callback)(void**, void*), void* user_data);

typedef MonoDomain* (*t_mono_domain_get)();
typedef MonoType* (*t_mono_class_get_type)(MonoClass* klass);
typedef MonoReflectionType* (*t_mono_type_get_object)(MonoDomain* domain, MonoType* type);
typedef MonoObject* (*t_mono_runtime_invoke)(MonoMethod* method, void* obj, void** args, MonoException** exc);
typedef MonoProperty* (*t_mono_class_get_property_from_name)(MonoClass* klass, const char* name);
typedef MonoMethod* (*t_mono_property_get_set_method)(MonoProperty* prop);
typedef MonoMethod* (*t_mono_property_get_get_method)(MonoProperty* prop);
typedef void* (*t_mono_object_unbox)(MonoObject* obj);
typedef void* (*t_mono_class_get_methods)(MonoClass* klass, void** iter);
typedef const char* (*t_mono_method_get_name)(MonoMethod* method);

// 数组 API（使用 void* 代替 MonoArray*）
typedef uint32_t(*t_mono_array_length)(void* array);
typedef MonoObject* (*t_mono_array_get)(void* array, MonoClass* klass, uintptr_t idx);

// ================== extern 声明 ==================
extern t_mono_array_length                  mono_array_length;
extern t_mono_array_get                     mono_array_get;
extern t_mono_class_get_methods             mono_class_get_methods;
extern t_mono_method_get_name               mono_method_get_name;
extern t_mono_class_get_property_from_name  mono_class_get_property_from_name;
extern t_mono_property_get_set_method       mono_property_get_set_method;
extern t_mono_get_root_domain               mono_get_root_domain;
extern t_mono_thread_attach                 mono_thread_attach;
extern t_mono_assembly_get_image            mono_assembly_get_image;
extern t_mono_image_get_name                mono_image_get_name;
extern t_mono_assembly_foreach              mono_assembly_foreach;
extern t_mono_class_from_name               mono_class_from_name;
extern t_mono_class_get_method_from_name    mono_class_get_method_from_name;
extern t_mono_compile_method                mono_compile_method;
extern t_mono_class_get_field_from_name     mono_class_get_field_from_name;
extern t_mono_field_get_value               mono_field_get_value;
extern t_mono_field_set_value               mono_field_set_value;
extern t_mono_object_get_class              mono_object_get_class;
extern t_mono_gc_walk_heap                  mono_gc_walk_heap;
extern t_mono_class_get_name                mono_class_get_name;
extern t_mono_class_get_namespace           mono_class_get_namespace;
extern t_mono_class_vtable                  mono_class_vtable;
extern t_mono_field_static_get_value        mono_field_static_get_value;
extern t_mono_property_get_get_method       mono_property_get_get_method;
extern t_mono_object_unbox                  mono_object_unbox;
extern t_mono_domain_get                    mono_domain_get;
extern t_mono_class_get_type                mono_class_get_type;
extern t_mono_type_get_object               mono_type_get_object;
extern t_mono_runtime_invoke                mono_runtime_invoke;

extern MonoAssembly* g_AssemblyCSharp;
extern MonoAssembly* g_unsafeAssem;

template<typename T>
T GetMonoFunction(HMODULE monoModule, const char* name)
{
    return reinterpret_cast<T>(GetProcAddress(monoModule, name));
}

bool InitMono();
void AssemblyCallback(MonoAssembly* assembly, void* userData);

MonoReflectionType* GetTypeObject(MonoClass* klass);
MonoObject* FindObjectOfType(MonoClass* targetClass);
MonoObject* GetStaticFieldObject(MonoClass* klass, const char* fieldName);
std::wstring ReadMonoString(void* monoStr);
std::string ReadMonoStringUtf8(void* monoStringPtr);
static inline bool IsNullOrDestroyed(MonoObject* obj)
{
    if (!obj) return true;

    void* klass = *(void**)obj;

    return klass == nullptr;
}
static inline bool IsValidMonoObject(MonoObject* obj)
{
    if (!obj)
        return false;

    MonoClass* klass = mono_object_get_class(obj);

    if (!klass)
        return false;

    return true;
}