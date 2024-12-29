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
    
    /// Form statistics plotting. If true, statistics about the form will be printed to the console.
    bool m_plotstats;
    
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
        if (m_plotstats) std::cout << "Time Profile '" + m_name + "':\n\n\n\n\n\n\n";
        sf::Time print;
        m_clock.restart();
        while (m_window.isOpen()) {
            m_time.cycle = m_clock.restart();
            if (m_plotstats) {
                print += m_time.cycle;
                if (print.asSeconds() > 1.0f / 4.0f) {
                    std::cout << "\e[6F\e[0J" << m_time.ToString() << "\n";
                    std::cout << "Objects: " << ObjectCount() << ", Updatables: " << m_updatables.Count() << ", Drawables: " << m_drawables.Count() << "\n";
                    print = {};
                }
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
                if (updatable->Error() != 0U) continue;
                updatable->__UpdateCall(m_time.cycle);
            }
            m_time.object_updates = m_clock.getElapsedTime() - m_time.object_updates;
            
            m_time.object_draws = m_clock.getElapsedTime();
            for (auto& drawable : m_drawables) {
                if (drawable->Error() != 0U) continue;
                if (drawable->IsDirty()) m_dirty = true;
                drawable->__DrawCall();
            }
            m_time.object_draws = m_clock.getElapsedTime() - m_time.object_draws;
            
            m_time.form_draw = m_clock.getElapsedTime();
            if (m_dirty) {
                Draw();
                for (auto& drawable : m_drawables) {
                    if (drawable->Error() != 0U) continue;
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
    
    /// Internal handler call to manage position changes of drawable child objects.
    void __OnObjectPositionChanged(Drawable*, const sf::Vector2f& position) {
        m_dirty = true;
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
            drawable->PositionChanged.Bind(&Form::__OnObjectPositionChanged, this);
        }
    }
    
    /// Internal handler call to manage deleted updatable and drawable objects.
    void __OnObjectDeleted(ObjectOwner* sender, Object*& object) {
        if (cf::Updatable* updatable = dynamic_cast<cf::Updatable*>(object)) {
            m_updatables.Remove(updatable);
        }
        if (cf::Drawable* drawable = dynamic_cast<cf::Drawable*>(object)) {
            m_drawables.Remove(drawable);
            drawable->PositionChanged.Unbind(&Form::__OnObjectPositionChanged, this);
        }
    }
    
public:
    
    /// Opens the SFML window of the form.
    virtual void Open() {
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
    virtual sf::RenderWindow* Window() {
        return &m_window;
    }
    
    /// Current SFML window title of the form. 
    virtual const std::string& Title() const {
        return m_title;
    }
    
    /// Current window position of the form.
    virtual const sf::Vector2i& Position() const {
        return m_position;
    }
    
    /// Current window size of the form.
    virtual const sf::Vector2u& Size() const {
        return m_size;
    }
    
    /// Current SFML style parameter of the form. 
    virtual const uint32_t& Style() const {
        return m_style;
    }
    
    /// Current maximum frames per second of the form.
    virtual const uint32_t& FrameLimit() const {
        return m_framelimit;
    }
    
    /// Current SFML context settings of the form.
    virtual const sf::ContextSettings& ContextSettings() const {
        return m_contextsettings;
    }
    
    /// Current background color of the form 
    virtual const sf::Color& Background() const {
        return m_background;
    }
    
    /// True if the form needs to be redrawn.
    virtual bool IsDirty() const {
        return m_dirty;
    }
    
    /// Change the SFML window title of the form.
    virtual void SetTitle(const std::string& title) {
        if (m_title == title) return;
        m_title = title;
        if (m_window.isOpen()) m_window.setTitle(m_title);
        TitleChanged(this, m_title);
    }
    
    /// Change the SFML window size of the form.
    virtual void SetSize(const sf::Vector2u& size) {
        if (m_size == size) return;
        m_size = size;
        if (m_window.isOpen()) m_window.setSize(m_size);
        SizeChanged(this, m_size);
    }
    
    /// Change the background color of the form.
    virtual void SetBackground(const sf::Color& color) {
        if (m_background == color) return;
        m_background = color;
        m_dirty = true;
        BackgroundChanged(this, m_background);
    }
    
    /// Mark the form to be redrawn
    virtual void SetDirty(bool dirty = true) {
        m_dirty = dirty;
    }
    
    Form(ObjectOwner* owner, const std::string& name) : Object(owner, name) {
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
        m_plotstats = false;
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
