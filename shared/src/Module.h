#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>

#include "v8.h"

#include "helpers/JS.h"
#include "helpers/Template.h"

namespace js
{
    class Class;
    class IResource;

    class Module
    {
        static std::unordered_map<std::string, Module*>& GetAll()
        {
            static std::unordered_map<std::string, Module*> modules;
            return modules;
        }

    public:
        using ModuleInitializationCallback = std::function<void(ModuleTemplate&)>;

    private:
        std::string name;
        std::string parentModule;
        ModuleInitializationCallback initCb;
        std::unordered_map<v8::Isolate*, ModuleTemplate> templateMap;
        std::unordered_map<IResource*, Persistent<v8::Object>> instanceMap;
        std::vector<Class*> classes;

        void Register(ModuleTemplate& tpl);

    public:
        Module(const std::string& _name, ModuleInitializationCallback _cb) : name(_name), initCb(_cb)
        {
            GetAll().insert({ name, this });
        }
        Module(const std::string& _name, const std::vector<Class*>& _classes, ModuleInitializationCallback _cb) : name(_name), classes(_classes), initCb(_cb)
        {
            GetAll().insert({ name, this });
        }
        Module(const std::string& _name, const std::string& _parent, ModuleInitializationCallback _cb) : name(_name), parentModule(_parent), initCb(_cb)
        {
            GetAll().insert({ name, this });
        }
        Module(const std::string& _name, const std::string& _parent, const std::vector<Class*>& _classes, ModuleInitializationCallback _cb)
            : name(_name), parentModule(_parent), classes(_classes), initCb(_cb)
        {
            GetAll().insert({ name, this });
        }

        const std::string& GetName() const
        {
            return name;
        }
        v8::Local<v8::ObjectTemplate> GetTemplate(v8::Isolate* isolate)
        {
            return templateMap.at(isolate).Get();
        }
        v8::Local<v8::Object> GetNamespace(IResource* resource);

        static bool Exists(const std::string& name)
        {
            return GetAll().contains(name);
        }
        static Module& Get(const std::string& name)
        {
            return *GetAll().at(name);
        }

        static void Initialize(v8::Isolate* isolate);
        static void Cleanup(v8::Isolate* isolate);
        static void CleanupForResource(IResource* resource);
    };
}  // namespace js
