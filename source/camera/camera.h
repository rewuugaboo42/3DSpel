#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement
{
    forward,
    backward,
    left,
    right,
};

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
        : m_position{ position }
        , m_worldUp{ up }
	{
        updateCameraVectors();
	}

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    glm::vec3 getPosition() const
    {
        return m_position;
    }

    glm::vec3 getFront() const
    {
        return m_front;
    }

    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        float cameraSpeed = m_movementSpeed * deltaTime;
        if (direction == forward)
            m_position += m_front * cameraSpeed;
        if (direction == backward)
            m_position -= m_front * cameraSpeed;
        if (direction == right)
            m_position += m_right * cameraSpeed;
        if (direction == left)
            m_position -= m_right * cameraSpeed;
    }

    void processMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= m_sensitivity;
        yoffset *= m_sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        updateCameraVectors();
    }

private:
    glm::vec3 m_position{};
    glm::vec3 m_front{ glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    glm::vec3 m_worldUp{};

    float m_yaw{ -90.0f };
    float m_pitch{ 0.0f };

    float m_movementSpeed{ 2.5f };
    float m_sensitivity{ 0.1f };
    float m_fov{ 45.0f };

    void updateCameraVectors()
    {
        glm::vec3 direction{};
        direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        direction.y = sin(glm::radians(m_pitch));
        direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(direction);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
};

#endif