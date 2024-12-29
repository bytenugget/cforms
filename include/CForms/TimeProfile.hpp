#pragma once

#include "Utility.hpp"

#include <SFML/System.hpp>
#include <string>

namespace cf {

/// Storage type for SFML time values of a cf::Form.
struct TimeProfile {
    
    /// Previous cycle excecution time.
    sf::Time cycle;
    
    /// Previous cycle's excecution time of handling window events.
    sf::Time window_events;
    
    /// Previous cycle's excecution time of updating the form itself.
    sf::Time form_update;
    
    /// Previous cycle's excecution time of updating objects inside the form.
    sf::Time object_updates;
    
    /// Previous cycle's excecution time of drawing the form itself.
    sf::Time form_draw;
    
    /// Previous cycle's excecution time of drawing objects inside the form.
    sf::Time object_draws;
    
    /// Formatted multi-line string representation.
    std::string ToString() {
        return StringF(
            "Cycle: %8.5f\nWindow: %6.5f\nUpdate: %6.5f\nDraw: %9.5f\nFPS: %10.5f",
            cycle.asSeconds(),
            window_events.asSeconds(),
            form_update.asSeconds() + object_updates.asSeconds(),
            form_draw.asSeconds() + object_draws.asSeconds(),
            1.0f / cycle.asSeconds()
        );
    }
    
    TimeProfile(
        const sf::Time& c,
        const sf::Time& wevents,
        const sf::Time& fupdate,
        const sf::Time& oupdates,
        const sf::Time& fdraw,
        const sf::Time& odraws) : 
            cycle(c),
            window_events(wevents),
            form_update(fupdate),
            object_updates(oupdates),
            form_draw(fdraw),
            object_draws(odraws) {}
    
    TimeProfile() {}
    
    ~TimeProfile() {}
    
};

}