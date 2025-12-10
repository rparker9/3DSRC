#include "Transform.h"

// Default constructor: initializes position and rotation to zero.
Transform3D::Transform3D()
{
    position      = { 0.0f, 0.0f, 0.0f };
    rotationEuler = { 0.0f, 0.0f, 0.0f };
}