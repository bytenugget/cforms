#pragma once

#include "Event.hpp"

#include <SFML/System.hpp>
#include <string>
#include <random>

namespace cf {

class ObjectOwner;

/// Base type for an object owner managed object.
class Object {

private:
    
    uint64_t m_id;
    ObjectOwner* m_owner;
    bool m_initialized;
    
protected:
    
    /// Name of the object. Should be unique to its owner!
    std::string m_name;
    
    /// Error state of the object. If greater than 0, the object will be locked out of application cycle.
    uint32_t m_error;
    
public:
    
    /// Fired if the object encountered an error.
    Event<Object*, const uint32_t&> ErrorEncoutered;
    
private:
    
    /// Internal call to generate an object ID.
    static uint64_t __GenerateRuntimeID() {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        return gen();
    }
    
protected:
    
    /// Override this to initialize your object.
    /// Should return false on error!
    virtual bool Init() {
        return true;
    }
    
public:
    
    /// Internal Init() call of the object.
    virtual bool __InitCall() {
        if (m_initialized) return true;
        if (!Init()) return false;
        m_initialized = true;
        return true;
    }
    
    /// Runtime ID of the object.
    const uint64_t& ID() const {
        return m_id;
    }
    
    /// Owner of the object; 
    ObjectOwner* Owner() {
        return m_owner;
    }
    
    /// True if the object was successfully initialized.
    bool IsInitialized() const {
        return m_initialized;
    }
    
    /// Current name of the object.
    const std::string& Name() const {
        return m_name;
    }
    
    /// Error state of the object. If greater than 0, the object will be locked out of application cycle.
    const uint32_t& Error() const {
        return m_error;
    }
    
    /// Change the name of the object.
    void SetName(const std::string& name) {
        m_name = name;
    }
    
    /// Do not use constructors to create an object! Instead, use Create() from the object owner.
    Object(ObjectOwner* owner, const std::string& name) {
        m_id = __GenerateRuntimeID();
        m_owner = owner;
        m_name = name;
        m_error = 0U;
    }
    
    /// Do not use constructors to create an object! Instead, use Create() from the object owner.
    Object(const std::string& name) : Object(nullptr, name) {}
    
    /// Do not use constructors to create an object! Instead, use Create() from the object owner.
    Object() : Object(nullptr, "Object") {}
    
    /// Do not use destructors to destroy an object! Instead, use Delete() from the object owner.
    virtual ~Object() {}
    
};

}
