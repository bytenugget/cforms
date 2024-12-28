#pragma once

#include "Object.hpp"

#include <SFML/System.hpp>

namespace cf {

/// Interface type for updatable objects.
class Updatable : public virtual Object {

protected:
    
    /// Override this to update your object.
    /// @param delta Excecution time of the previous cycle.
    virtual void Update(const sf::Time& delta) {}
    
public:
    
    /// Internal Update() call of the object.
    virtual void __UpdateCall(const sf::Time& delta) {
        Update(delta);
    }
    
    /// Do not use this constructor!
    /// Types derived from cf::Updatable should call cf::Object(owner, name) or cf::Object(name)!
    Updatable() {}
    
    virtual ~Updatable() {}
    
};

}