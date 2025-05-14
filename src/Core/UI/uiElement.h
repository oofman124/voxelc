#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H
#include <glm/glm.hpp>
#include <vector>
#include "uiVertex.h"
#include "../Rendering/texture.h"
class UIElement {
protected:
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 color;
    bool isDirty;
    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<Texture> texture;

public:
    UIElement(): color(1.0f), isDirty(true) {}
    
    virtual void updateGeometry() = 0;
    
    void setTexture(std::shared_ptr<Texture> newTexture)
    {
        texture = newTexture;
    }

    std::shared_ptr<Texture> getTexture()
    {
        return texture;
    }

    void setPosition(const glm::vec2& pos) { 
        position = pos; 
        isDirty = true; 
    }
    
    void setSize(const glm::vec2& newSize) { 
        size = newSize; 
        isDirty = true; 
    }
    
    void setColor(const glm::vec4& newColor) {
        color = newColor;
        isDirty = true;
    }
    
    const std::vector<UIVertex>& getVertices() {
        if(isDirty) {
            updateGeometry();
            isDirty = false;
        }
        return vertices;
    }
    
    const std::vector<uint32_t>& getIndices() const { return indices; }
};

#endif