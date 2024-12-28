#pragma once

#include "Object.hpp"
#include "Event.hpp"
#include "Predicate.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>

namespace cf {

/// Base type for an object owner.
class ObjectOwner : public virtual Object {

private:
    
    std::vector<std::unique_ptr<Object>> m_objects;
    std::unordered_map<uint64_t, size_t> m_objectmap;
    std::unordered_map<std::string, std::unordered_map<uint64_t, size_t>> m_typemap;

public:
    
    /// Fired when an object was created.
    /// @param sender Owner which fired the Event.
    /// @param object Object which was created.
    Event<ObjectOwner*, Object*&> ObjectCreated;
    
    /// Fired when an object was initialized.
    /// @param sender Owner which fired the Event.
    /// @param object Object which was initialized.
    Event<ObjectOwner*, Object*&> ObjectInitialized;
    
    /// Fired when an object is about to be destroyed.
    /// @param sender Owner which fired the Event.
    /// @param object Object to be destroyed.
    Event<ObjectOwner*, Object*&> ObjectDeleted;
    
protected:
    
    /// Register an object type for use in the object owner's search functions.
    /// Main types used in Create() are automatically registered!
    template<typename TObject>
    void Register(TObject* object) {
        static_assert(std::is_base_of<Object, TObject>::value, "TObject must inherit from cf::Object");
        auto it = m_objectmap.find(object->ID());
        if (it == m_objectmap.end()) {
            std::cerr << "[X] ObjectOwner: Failed to register. '" + m_name + "' does not own object '" + object->Name() + "'.\n";
            return;
        }
        std::string tname = typeid(TObject).name();
        m_typemap[tname][object->ID()] = it->second;
    }
    
    /// Create new object of type <TObject>.
    /// @param name Name for the object. Should be unique inside its owner!
    template<typename TObject>
    TObject* Create(const std::string& name) {
        static_assert(std::is_base_of<Object, TObject>::value, "TObject must inherit from cf::Object");
        auto& ptr = m_objects.emplace_back(std::make_unique<TObject>(this, name));
        if (!ptr) {
            // ERROR Failed to allocate/create object
            std::cerr << "[X] ObjectOwner: Failed to allocate/create object \'" + std::string(name) + "\'.\n";
            return nullptr;
        }
        size_t index = m_objects.size() - 1;
        uint64_t id = m_objects[index]->ID();
        m_objectmap[id] = index;
        std::string tname = typeid(TObject).name();
        m_typemap[tname][id] = index;
        cf::Object* object = ptr.get();
        ObjectCreated(this, object);
        if (!object->__InitCall()) {
            // ERROR Failed to initialize the object
            Delete(object);
            return nullptr;
        }
        ObjectInitialized(this, object);
        return dynamic_cast<TObject*>(object);
    }
    
    /// Delete an owned object.
    bool Delete(Object* object) {
        auto it = m_objectmap.find(object->ID());
        if (it == m_objectmap.end()) {
            return false;
        }
        for (auto& m: m_typemap) {
            if (m.second.erase(object->ID()) > 0)
                break;
        }
        m_objectmap.erase(object->ID());
        ObjectDeleted(this, object);
        m_objects.erase(m_objects.begin() + it->second);
        return true;
    }
    
public:
    
    /// Current ammount of owned objects 
    size_t ObjectCount() const {
        return m_objects.size();
    }
    
    /// Get object by ID.
    Object* Get(const uint64_t& id) {
        auto it = m_objectmap.find(id);
        if (it == m_objectmap.end()) {
            return nullptr;
        }
        else {
            return m_objects[it->second].get();
        }
    }
    
    /// Get all object of type <TObject>.
    /// <TObject> must be a registered type!
    template<typename TObject>
    std::vector<TObject*> GetAll() {
        static_assert(std::is_base_of<cf::Object, TObject>::value, "TObject must inherit from cf::Object");
        std::vector<TObject*> result;
        std::string t_id = typeid(TObject).name();
        auto t_it = m_typemap.find(t_id);
        if (t_it == m_typemap.end()) {
            return result;
        }
        auto& t_om = t_it->second;
        for (auto& index: t_om) {
            TObject* obj = dynamic_cast<TObject*>(m_objects[index.second].get());
            result.push_back(obj);    
        }
        return result;
    }
    
    /// Find first object matching the comparison function.
    Object* Find(Predicate<Object>::Ptr p) {
        for (auto& obj : m_objects) {
            if (p(obj.get())) return obj.get();
        }
        return nullptr;
    }
    
    /// Find first object of type <TObject> matching the comparison function.
    /// <TObject> must be a registered type!
    template<typename TObject>
    TObject* Find(typename Predicate<TObject>::Ptr p) {
        static_assert(std::is_base_of<cf::Object, TObject>::value, "TObject must inherit from cf::Object");
        std::string t_id = typeid(TObject).name();
        auto t_it = m_typemap.find(t_id);
        if (t_it == m_typemap.end()) {
            return nullptr;
        }
        auto& t_om = t_it->second;
        
        for (auto& index: t_om) {
            TObject* obj = dynamic_cast<TObject*>(m_objects[index.second].get());
            if (obj && p(obj))
                return obj;
        }
        return nullptr;
    }
    
    /// Find all objects matching the comparison function.
    std::vector<Object*> FindAll(Predicate<Object>::Ptr p) {
        std::vector<cf::Object*> result;
        auto it = m_objects.begin();
        while (it != m_objects.end()) {
            if (p(it->get())) {
                result.push_back(it->get());
            }
            ++it;
        }
        return result;
    }
    
    /// Find all objects of type <TObject> matching the comparison function.
    /// <TObject> must be a registered type!
    template<typename TObject>
    std::vector<TObject*> FindAll(typename Predicate<TObject>::Ptr p) {
        static_assert(std::is_base_of<cf::Object, TObject>::value, "TObject must inherit from cf::Object");
        std::vector<TObject*> result;
        std::string t_id = typeid(TObject).name();
        auto t_it = m_typemap.find(t_id);
        if (t_it == m_typemap.end()) {
            return result;
        }
        auto& t_om = t_it->second;
        for (auto& index: t_om) {
            TObject* obj = dynamic_cast<TObject*>(m_objects[index.second].get());
            if (obj && p(obj))
                result.push_back(obj);
        }
        return result;
    }
    
    /// Do not use this constructor!
    /// Types derived from cf::ObjectOwner should call cf::Object(owner, name) or cf::Object(name)!
    ObjectOwner(ObjectOwner* owner, const std::string& name) : Object(owner, name) {}
    
    /// Do not use this constructor!
    /// Types derived from cf::ObjectOwner should call cf::Object(owner, name) or cf::Object(name)!
    ObjectOwner(const std::string& name) : ObjectOwner(nullptr, name) {}
    
    /// Do not use this constructor!
    /// Types derived from cf::ObjectOwner should call cf::Object(owner, name) or cf::Object(name)!
    ObjectOwner() : ObjectOwner(nullptr, "ObjectOwner") {}
    
    virtual ~ObjectOwner() {}
    
};

}
