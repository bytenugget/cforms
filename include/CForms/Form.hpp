#pragma once

#include "ObjectOwner.hpp"
#include "Collection.hpp"
#include "Updatable.hpp"
#include "Drawable.hpp"
#include "Control.hpp"
#include "TimeProfile.hpp"

#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <string>
#include <iostream>

namespace cf {

/// Base type for a SFML window form, with child objects.
class Form : public ObjectOwner {

private:
    
    Collection<Updatable> m_updatables;
    Collection<Drawable> m_drawables;
    sf::Clock m_clock;
    TimeProfile m_time;
    sf::Event m_window_event;
    
protected:
    
    /// SFML render window.
    sf::RenderWindow m_window;
    
    /// Window title.
    std::string m_title;
    
    /// Window position.
    sf::Vector2i m_position;
    
    /// Window size.
    sf::Vector2u m_size;
    
    /// Window style.
    uint32_t m_style;
    
    /// Maximum frames per second.
    uint32_t m_framelimit;
    
    /// Window context settings.
    sf::ContextSettings m_contextsettings;
    
    /// Background color.
    sf::Color m_background;
    
    /// Dirty state of the form. If true at draw time, the form will be redrawn.
    bool m_dirty;
    
public:
    
    /// Fired when the form was opened.
    /// @param sender Form which fired the event.
    Event<Form*> Opened;
    
    /// Fired when the form was closed.
    /// @param sender Form which fired the event.
    Event<Form*> Closed;
    
    /// Fired when the form's title was changed.
    /// @param sender Form which fired the event.
    /// @param title New form title.
    Event<Form*, const std::string&> TitleChanged;
    
    /// Fired when the form's size was changed.
    /// @param sender Form which fired the event.
    /// @param size New form size.
    Event<Form*, const sf::Vector2u&> SizeChanged;
    
    /// Fired when the form's background color was changed.
    /// @param sender Form which fired the event.
    /// @param color New background color.
    Event<Form*, const sf::Color&> BackgroundChanged;
    
protected:
    
    /// Override this to handle SFML window events.
    virtual void WindowEvent(sf::Event& window_event) {}
    
    /// Override this to update your form.
    /// @param delta Execution time of the previous cycle.
    virtual void Update(const sf::Time& delta) {}
    
    /// Override this to draw your form
    virtual void Draw() {
        m_window.clear(m_background);
    }
    
private:
    
    /// Internal operating loop of the form.
    void __Loop() {
        Opened(this);
        std::cout << "Time Profile '" + m_name + "':\n\n\n\n\n\n\n";
        sf::Time print;
        m_clock.restart();
        while (m_window.isOpen()) {
            m_time.cycle = m_clock.restart();
            print += m_time.cycle;
            if (print.asSeconds() > 1.0f / 4.0f) {
                std::cout << "\e[6F\e[0J" << m_time.ToString() << "\n";
                std::cout << "Objects: " << ObjectCount() << ", Updatables: " << m_updatables.Count() << ", Drawables: " << m_drawables.Count() << "\n";
                print = {};
            }
            
            m_time.window_events = m_clock.getElapsedTime();
            while (m_window.pollEvent(m_window_event)) {
                if (m_window_event.type == sf::Event::Closed) {
                    m_window.close();
                }
                else {
                    WindowEvent(m_window_event);
                }
            }
            m_time.window_events = m_clock.getElapsedTime() - m_time.window_events;
            
            m_time.form_update = m_clock.getElapsedTime();
            Update(m_time.cycle);
            m_time.form_update = m_clock.getElapsedTime() - m_time.form_update;
            
            m_time.object_updates = m_clock.getElapsedTime();
            for (auto& updatable : m_updatables) {
                updatable->__UpdateCall(m_time.cycle);
            }
            m_time.object_updates = m_clock.getElapsedTime() - m_time.object_updates;
            
            m_time.object_draws = m_clock.getElapsedTime();
            for (auto& drawable : m_drawables) {
                if (drawable->IsDirty()) m_dirty = true;
                drawable->__DrawCall();
            }
            m_time.object_draws = m_clock.getElapsedTime() - m_time.object_draws;
            
            m_time.form_draw = m_clock.getElapsedTime();
            if (m_dirty) {
                Draw();
                for (auto& drawable : m_drawables) {
                    cf::Transform* transform = drawable->Transform();
                    if (!transform) continue;
                    sf::Sprite sprite(drawable->Canvas()->getTexture());
                    sprite.setPosition(sf::Vector2f(drawable->Transform()->Position()));
                    m_window.draw(sprite);
                }
                m_window.display();
                m_dirty = false;
            }
            m_time.form_draw = m_clock.getElapsedTime() - m_time.form_draw;
        }
        Closed(this);
    }
    
    /// Internal handler call to manage created updatable and drawable objects.
    void __OnObjectCreated(ObjectOwner* sender, Object*& object) {
        if (ObjectOwner* owner = dynamic_cast<ObjectOwner*>(object)) {
            Register<ObjectOwner>(owner);
        }
        if (Control* control = dynamic_cast<Control*>(object)) {
            Register<Control>(control);
        }
        if (Updatable* updatable = dynamic_cast<Updatable*>(object)) {
            Register<Updatable>(updatable);
            m_updatables.Add(updatable);
        }
        if (Drawable* drawable = dynamic_cast<Drawable*>(object)) {
            Register<Drawable>(drawable);
            m_drawables.Add(drawable);
        }
    }
    
    /// Internal handler call to manage deleted updatable and drawable objects.
    void __OnObjectDeleted(ObjectOwner* sender, Object*& object) {
        if (cf::Updatable* updatable = dynamic_cast<cf::Updatable*>(object)) {
            m_updatables.Remove(updatable);
        }
        if (cf::Drawable* drawable = dynamic_cast<cf::Drawable*>(object)) {
            m_drawables.Remove(drawable);
        }
    }
    
public:
    
    /// Opens the SFML window of the form.
    void Open() {
        if (!__InitCall()) {
            std::cout << "[X] Form: Failed to initialize '" + m_name + "'\n";
            return;
        }
        m_window.create({m_size.x, m_size.y}, m_title, m_style, m_contextsettings);
        m_window.setFramerateLimit(m_framelimit);
        Display* display = XOpenDisplay(nullptr);
        XRRScreenResources *screens = XRRGetScreenResources(display, DefaultRootWindow(display));
        XRRCrtcInfo *info = XRRGetCrtcInfo(display, screens, screens->crtcs[0]);
        m_window.setPosition(sf::Vector2i((info->width / 2) - (m_size.x / 2), (info->height / 2) - (m_size.y / 2)));
        XRRFreeCrtcInfo(info);
        XRRFreeScreenResources(screens);
        __Loop();
    }
    
    /// Pointer reference to the SFML window of the form.
    sf::RenderWindow* Window() {
        return &m_window;
    }
    
    /// Current SFML window title of the form. 
    const std::string& Title() const {
        return m_title;
    }
    
    /// Current window position of the form.
    const sf::Vector2i& Position() const {
        return m_position;
    }
    
    /// Current window size of the form.
    const sf::Vector2u& Size() const {
        return m_size;
    }
    
    /// Current SFML style parameter of the form. 
    const uint32_t& Style() const {
        return m_style;
    }
    
    /// Current maximum frames per second of the form.
    const uint32_t& FrameLimit() const {
        return m_framelimit;
    }
    
    /// Current SFML context settings of the form.
    const sf::ContextSettings& ContextSettings() const {
        return m_contextsettings;
    }
    
    /// Current background color of the form 
    const sf::Color& Background() const {
        return m_background;
    }
    
    /// True if the form needs to be redrawn.
    bool IsDirty() const {
        return m_dirty;
    }
    
    /// Change the SFML window title of the form.
    void SetTitle(const std::string& title) {
        if (m_title == title) return;
        m_title = title;
        if (m_window.isOpen()) m_window.setTitle(m_title);
        TitleChanged(this, m_title);
    }
    
    /// Change the SFML window size of the form.
    void SetSize(const sf::Vector2u& size) {
        if (m_size == size) return;
        m_size = size;
        if (m_window.isOpen()) m_window.setSize(m_size);
        SizeChanged(this, m_size);
    }
    
    /// Change the background color of the form.
    void SetBackground(const sf::Color& color) {
        if (m_background == color) return;
        m_background = color;
        m_dirty = true;
        BackgroundChanged(this, m_background);
    }
    
    /// Mark the form to be redrawn
    void SetDirty(bool dirty = true) {
        m_dirty = dirty;
    }
    
    Form(ObjectOwner* owner, const std::string& name) : ObjectOwner(owner, name) {
        m_title = m_name;
        m_size = sf::Vector2u(500U, 400U);
        m_style = 7U;
        m_framelimit = 60U;
        m_contextsettings = sf::ContextSettings(
            0U,   // depth
            0U,   // stencil
            0U,   // antialiasing
            1U,   // major
            1U,   // minor
            0U,   // attributes
            false // sRgb
        );
        m_background = sf::Color(0x000000FF);
        m_dirty = true;
        ObjectCreated.Bind(&cf::Form::__OnObjectCreated, this);
        ObjectDeleted.Bind(&cf::Form::__OnObjectDeleted, this);
    }
    
    Form(const std::string& name) : Form(nullptr, name) {}
    
    Form() : Form(nullptr, "Form") {}
    
    virtual ~Form() {
        ObjectCreated.Unbind(&cf::Form::__OnObjectCreated, this);
        ObjectDeleted.Unbind(&cf::Form::__OnObjectDeleted, this);
    }
    
};

}
