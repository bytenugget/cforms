#pragma once

#include "ObjectOwner.hpp"
#include "Updatable.hpp"
#include "Drawable.hpp"
#include "Collection.hpp"
#include "Event.hpp"

#include <SFML/Graphics.hpp>

#include <string>

namespace cf {

/// Base type for an updatable and drawable object, with child objects.
class Control : public Updatable, public Drawable, public ObjectOwner {

private:
    
    Collection<Updatable> m_updatables;
    Collection<Drawable> m_drawables;
    
protected:
    
    /// Background color of the control.
    sf::Color m_background;
    
public:
    
    /// Fired when the control's background color was changed, through SetBackground().
    /// @param sender Control which fired the event.
    /// @param color New background color.
    Event<Control*, const sf::Color&> BackgroundChanged;
    
private:
    
    /// Internal handler call to manage created updatable and drawable objects.
    virtual void __OnObjectCreated(ObjectOwner* sender, Object*& object) {
        if (ObjectOwner* owner = dynamic_cast<ObjectOwner*>(object)) {
            Register<ObjectOwner>(owner);
        }
        if (Control* control = dynamic_cast<Control*>(object)) {
            Register<Control>(control);
        }
        if (cf::Updatable* updatable = dynamic_cast<cf::Updatable*>(object)) {
            Register<Updatable>(updatable);
            m_updatables.Add(updatable);
        }
        if (cf::Drawable* drawable = dynamic_cast<cf::Drawable*>(object)) {
            Register<Drawable>(drawable);
            m_drawables.Add(drawable);
        }
    }
    
    /// Internal handler call to manage deleted updatable and drawable objects.
    virtual void __OnObjectDeleted(ObjectOwner* sender, Object*& object) {
        if (cf::Updatable* updatable = dynamic_cast<cf::Updatable*>(object)) {
            m_updatables.Remove(updatable);
        }
        if (cf::Drawable* drawable = dynamic_cast<cf::Drawable*>(object)) {
            m_drawables.Remove(drawable);
        }
    }
    
protected:
    
    /// Override this call to initialize your control's contents
    virtual bool Init() override {
        if (!Updatable::Init()) {
            return false;
        }
        return Drawable::Init();
    }
    
    /// Override this call to draw your control
    virtual void Draw() override {
        m_canvas.clear(m_background);
    }
    
public:
    
    /// Internal Update() call of the control.
    virtual void __UpdateCall(const sf::Time& delta) override {
        Update(delta);
        for (auto& updatable : m_updatables) {
            updatable->__UpdateCall(delta);
        }
    }
    
    /// Internal Draw() call of the control.
    virtual void __DrawCall() override {
        if (m_dirty) {
            for (auto& drawable : m_drawables) {
                if (drawable->IsDirty()) m_dirty = true;
                drawable->__DrawCall();
            }
            Draw();
            for (auto& drawable : m_drawables) {
                cf::Transform* transform = drawable->Transform();
                if (!transform) continue;
                sf::Sprite sprite(drawable->Canvas()->getTexture());
                sprite.setPosition(sf::Vector2f(drawable->Transform()->Position()));
                m_canvas.draw(sprite);
            }
            m_canvas.display();
            m_dirty = false;
        }
    }
    
    /// Current background color of the control 
    virtual const sf::Color& Background() const {
        return m_background;
    }
    
    /// Change the background color of the control.
    virtual void SetBackground(const sf::Color& color) {
        if (m_background == color) return;
        m_background = color;
        BackgroundChanged(this, m_background);
    }
    
    /// Do not use constructors to create a control! Instead, use Create() from the object owner.
    Control(ObjectOwner* owner, const std::string& name) : Object(owner, name) {
        m_background = sf::Color(0x000000FF);
        ObjectCreated.Bind(&cf::Control::__OnObjectCreated, this);
        ObjectDeleted.Bind(&cf::Control::__OnObjectDeleted, this);
    }
    
    /// Do not use constructors to create a control! Instead, use Create() from the object owner.
    Control() : Control(nullptr, "Control") {}
    
    /// Do not use destructors to destroy a control! Instead, use Delete() from the object owner.
    virtual ~Control() {
        ObjectCreated.Unbind(&cf::Control::__OnObjectCreated, this);
        ObjectDeleted.Unbind(&cf::Control::__OnObjectDeleted, this);
    }
    
};

}