#include <ic_camera.h>

namespace IC {
    Camera::Camera() {
        _position = {2.0f, 0.0f, 2.0f};
        _lookVector = {0.0f, 1.0f, 0.0f};
    }
} // namespace IC
