#include "CForms/Form.hpp"
#include "CForms/Control.hpp"

#include <iostream>

// Test control which is supposed to move left and right inside the window
class TestControl : public cf::Control {

private:
    
    float m_speed;
    bool m_goright;

protected:
    
    virtual bool Init() override {
        if (!cf::Drawable::Init()) return false;
        m_transform.SetSize({20, 20});
        m_background = sf::Color(0x00FF00FF);
        m_speed = 200.0f;
        m_goright = true;
        return true;
    }
    
    virtual void Update(const sf::Time& delta) override {
        if (m_transform.Position().x == 10.0f)
            m_goright = true;
        else if (m_transform.Position().x == 370.0f)
            m_goright = false;
        
        float move = delta.asSeconds() * m_speed;
        
        if (m_goright)
            m_transform.SetX(m_transform.Position().x + move);
        else
            m_transform.SetX(m_transform.Position().x - move);
        
        if (m_transform.Position().x < 10.0f)
            m_transform.SetX(10.0f);
        else if (m_transform.Position().x > 370.0f)
            m_transform.SetX(370.0f);
    }
    
public:
    
    TestControl(cf::ObjectOwner* owner, const std::string& name) : cf::Object(owner, name) {}
    
};

// Test form which is supposed to create the window and 13 test controls
class TestForm : public cf::Form {

protected:
    
    virtual bool Init() override { // Issue: this override is not executed on a shared libraries build
        m_size = sf::Vector2u({400, 400});
        for (int i=0; i < 13; i++) {
            TestControl* c = Create<TestControl>("TestControl" + std::to_string(i));
            if (!c) return false;
            c->Transform()->SetPosition({10.0f + 30.0f * i, 10.0f + 30.0f * i});
        }
        
        return true;
    }
    
public:
    
    TestForm() : cf::Object("TestForm") { // Issue: This constructor call does happen tho.
        m_plotstats = true; // set this false to hide statistics in the console. (statistics overwrite error messages)
    }
    
};


int main() {
    TestForm test;
    test.Open();
}