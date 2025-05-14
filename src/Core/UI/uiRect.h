#ifndef UI_RECT_H
#define UI_RECT_H

#include "uiElement.h"

class UIRect : public UIElement
{
public:
    UIRect() : UIElement()
    {
        // Initialize indices for a quad (two triangles)
        indices = {0, 1, 2, 2, 3, 0};
    }

    void updateGeometry() override
    {
        vertices.clear();

        // Create quad vertices
        vertices = {
            {position.x, position.y, 0, 0, color.x, color.y, color.z, color.w},
            {position.x + size.x, position.y, 1, 0, color.x, color.y, color.z, color.w},
            {position.x + size.x, position.y + size.y, 1, 1, color.x, color.y, color.z, color.w},
            {position.x, position.y + size.y, 0, 1, color.x, color.y, color.z, color.w}
        };

        // Create quad indices
        indices = {0, 1, 2, 2, 3, 0};
    }
};

#endif