#pragma once

#include "Event.hpp"

#include <SFML/System.hpp>

namespace cf {

/// Type for position and size of a drawable object.
class Transform {

protected:
    
    /// Position of the transform.
    sf::Vector2f m_position;
    
    /// Size of the transform.
    sf::Vector2u m_size;
    
public:
    
    /// Internal event, fired when the transform position was changed, through SetPosition(), SetX() or SetY().
    /// You should subscribe to PositionChanged of a drawable object instead.
    Event<const sf::Vector2f&> __PositionChanged;
    
    /// Internal event, fired when the transform size was changed, through SetSize(), SetWidth() or SetHeight().
    /// You should subscribe to SizeChanged of a drawable object instead.
    Event<const sf::Vector2u&> __SizeChanged;
    
    /// Get the current position of the object's transform.
    const sf::Vector2f& Position() const {
        return m_position;
    }
    
    /// Get the current x-axis position of the object's transform.
    float X() const {
        return m_position.x;
    }
    
    /// Get the current y-axis position of the object's transform.
    float Y() const {
        return m_position.y;
    }
    
    /// Get the current size of the object's transform.
    const sf::Vector2u& Size() const {
        return m_size;
    }
    
    /// Get the current width of the object's transform.
    uint Width() const {
        return m_size.x;
    }
    
    /// Get the current height of the object's transform.
    uint Height() const {
        return m_size.y;
    }
    
    /// Change the position of the object's transform.
    void SetPosition(const sf::Vector2f& position) {
        if (m_position == position) return;
        m_position = position;
        __PositionChanged(m_position);
    }
    
    /// Change the x-axis position of the object's transform.
    void SetX(float x) {
        if (m_position.x == x) return;
        m_position.x = x;
        __PositionChanged(m_position);
    }
    
    /// Change the y-axis position of the object's transform.
    void SetY(float y) {
        if (m_position.y == y) return;
        m_position.y = y;
        __PositionChanged(m_position);
    }
    
    /// Change the size of the object's transform.
    void SetSize(const sf::Vector2u& size) {
        if (m_size == size) return;
        m_size = size;
        __SizeChanged(m_size);
    }
    
    /// Change the width of the object's transform.
    void SetWidth(uint width) {
        if (m_size.x == width) return;
        m_size.x = width;
        __SizeChanged(m_size);
    }
    
    /// Change the height of the object's transform.
    void SetHeight(uint height) {
        if (m_size.y == height) return;
        m_size.y = height;
        __SizeChanged(m_size);
    }
    
    Transform(const sf::Vector2f& position, const sf::Vector2u& size) {
        m_position = position;
        m_size = size;
    }
    
    Transform() {
        m_position = sf::Vector2f(0.0f, 0.0f);
        m_size = sf::Vector2u(20, 20);
    }
    
    virtual ~Transform() {}
    
};

}