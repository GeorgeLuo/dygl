#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

struct TransformComponent
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    // Constructor to initialize the position, scale, and rotation
    TransformComponent(float x = 0.0f, float y = 0.0f, float z = 0.0f,
                       float scaleX = 1.0f, float scaleY = 1.0f,
                       float scaleZ = 1.0f, float rotX = 0.0f, float rotY = 0.0f,
                       float rotZ = 0.0f)
        : position(x, y, z), scale(scaleX, scaleY, scaleZ),
          rotation(rotX, rotY, rotZ) {}

    // Method to calculate and return the model matrix.
    // This model matrix can be passed to a shader to apply the transformations
    // (position, rotation, scale) to an object in the scene.
    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);       // Start with the identity matrix
        model = glm::translate(model, position); // Apply translation
        // Apply rotation around the x-axis
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        // Apply rotation around the y-axis
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        // Apply rotation around the z-axis
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        // Apply scaling
        model = glm::scale(model, scale);

        return model;
    }
};