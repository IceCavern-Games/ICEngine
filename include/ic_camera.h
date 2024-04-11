#pragma once

#include <glm/glm.hpp>

namespace IC
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        glm::vec3 GetLookVector() { return _lookVector; }
        glm::vec3 GetPosition() { return _position; }

    private:
        glm::vec3 _lookVector;
        glm::vec3 _position;
    };
}
