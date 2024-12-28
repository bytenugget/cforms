#pragma once

#include "Object.hpp"
#include "Transform.hpp"
#include "Event.hpp"

#include <SFML/Graphics.hpp>

#include <iostream>

namespace cf {

/// Interface type for drawable objects.
class Drawable : public virtual Object {

protected:
    
    /// SFML render texture of the object.
    sf::RenderTexture m_canvas;
    
    /// Position and size of the object.
    cf::Transform m_transform;
    
    /// Dirty state of the object. If true at draw time, the object will be redrawn.
    bool m_dirty;
    
public:
    
    /// Fired when the object's transform position was changed, through Transform().
    /// @param sender Object which fired the event.
    /// @param position New transform position.
    Event<Drawable*, const sf::Vector2i&> PositionChanged;
    
    /// Fired when the object's transform size was changed, through Transform().
    /// @param sender Object which fired the event.
    /// @param size New transform size.
    Event<Drawable*, const sf::Vector2u&> SizeChanged;
    
private:
    
    /// Internal handler call to report changes of the object's transform position.
    void __OnTransformPositionChanged(const sf::Vector2i& position) {
        PositionChanged(this, position);
    }
    
    /// Internal handler call to report changes of the object's transform size.
    void __OnTransformSizeChanged(const sf::Vector2u& size) {
        m_canvas.create(size.x, size.y);
        SizeChanged(this, size);
    }
    
protected:
    
    /// Override this to initialize your object.
    /// Call Drawable::Init() to create the object's render texture, if you override!
    virtual bool Init() override {
        if (!m_canvas.create(60, 20)) {
            // ERROR Failed to create canvas
            std::cerr << "[X] Drawable: Failed to create canvas\n";
            return false;
        }
        return true;
    }
    
    /// Override this call to draw your object
    virtual void Draw() {}
    
public:
    
    /// Reference pointer to the object's transform.
    virtual cf::Transform* Transform() {
        return &m_transform;
    }
    
    /// Reference pointer to the object's SFML render texture. 
    virtual sf::RenderTexture* Canvas() {
        return &m_canvas;
    }
    
    /// True if the object needs to be redrawn.
    virtual bool IsDirty() const {
        return m_dirty;
    }
    
    /// Change your object's dirty state.
    virtual void SetDirty(bool dirty = true) {
        m_dirty = dirty;
    }
    
    /// Internal Draw() call of the object.
    virtual void __DrawCall() {
        if (m_dirty) {
            Draw();
            m_dirty = false;
        }
    }
    
    /// Do not use this constructor!
    /// Types derived from cf::Drawable should call cf::Object(owner, name) or cf::Object(name)!
    Drawable() {
        m_transform = cf::Transform({0, 0}, {60, 20});
        m_transform.__PositionChanged.Bind(&Drawable::__OnTransformPositionChanged, this);
        m_transform.__SizeChanged.Bind(&Drawable::__OnTransformSizeChanged, this);
        m_dirty = true;
    }
    
    virtual ~Drawable() {
        m_transform.__PositionChanged.Unbind(&Drawable::__OnTransformPositionChanged, this);
        m_transform.__SizeChanged.Unbind(&Drawable::__OnTransformSizeChanged, this);
    }
    
};

}