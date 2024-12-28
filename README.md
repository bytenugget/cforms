## CForms
Small UI framework which utilizes SFML 2.3 and is mainly, but not only, inspired by personal experience with Windows Forms.

### How to use:
Create your own custom types derived from **cf::Form** and/or **cf::Control** and override the Init(), Update() and Draw() functions.

```cpp
/// This Example creates a custom cf::Control and cf::Form.
/// The control will move around from left to right inside the window.

#include "CForms/Form.hpp"
#include "CForms/Control.hpp"

class MyControl : public cf::Control {

private:
    
    float m_speed;
    bool m_goright;

protected:
    
    virtual bool Init() override {
        m_transform.SetSize({20, 20});
        m_background = sf::Color(0x00FF00FF);
        m_speed = 200.0f;
        m_goright = true;
        return true;  // we return true to signal that everything went as planned.
    }
    
    virtual void Update(const sf::Time& delta) override {
        if (m_transform.Position().x <= 10.0f)
            m_goright = true;
        else if (m_transform.Position().x >= 370.0f)
            m_goright = false;
        
        int move = (int)(delta.asSeconds() * m_speed);
        
        if (m_goright)
            m_transform.SetX(m_transform.Position().x + move);
        else
            m_transform.SetX(m_transform.Position().x - move);
        
        m_dirty = true; // we set the control "dirty" to signal that it needs to be redrawn. The owner will redraw aswell.
    }
    
public:
    
    MyControl(cf::ObjectOwner* owner, const std::string& name) : cf::Object(owner, name) {}
    
};

class MyForm : public cf::Form {

protected:
    
    virtual bool Init() override {
        m_size = sf::Vector2u({400, 400});
        
        MyControl* ctrl = Create<MyControl>("MyControl");
        ctrl->Transform()->SetPosition({10, 200});
        
        return true;
    }
    
public:
    
    MyForm() : cf::Object("MyForm") {}
    
};


int main() {
    MyForm myform;
    myform.Open();
}
```

#### Main types to use:
- **cf::Form**: Base type for a SFML window form, with child objects.
- **cf::Control**: Base type for an updatable and drawable object, with child objects.

#### Main overridable functions:
- Init(): Customize the form/control and create child objects.
- Update(): Perform per-frame changes to the form.
- Draw(): Draw the form/control itself.
- Create<>(): Instantiate a child object owned by the form/control. Objects must atleast be derived from type cf::Object.
- Delete<>(): Destroy a previously created child object owned by the form/control.

#### Other types:
- **cf::Object**: Base type for an object owner managed object.
- **cf::ObjectOwner**: Base type for an object owner, which can create and destroy other objects.
- **cf::Updatable**: Base type for updatable objects.
- **cf::Drawable**: Base type for drawable objects. Contains a SFML render texture that can be drawn by an owner.

### TODO:
- Fix shared libraries issue.
- Threaded cf::Form, to be able to create child windows.
- Expand properties of cf::Control and cf::Form.
- ...
