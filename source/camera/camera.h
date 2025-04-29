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
        glm::vec3 moveDirection = glm::vec3(0.0f);

        if (direction == forward)
            moveDirection += m_front;
        if (direction == backward)
            moveDirection -= m_front;
        if (direction == right)
            moveDirection += m_right;
        if (direction == left)
            moveDirection -= m_right;

        moveDirection.y = 0.0f;

        m_position += glm::normalize(moveDirection) * cameraSpeed;
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

    void jump()
    {
        if (!m_isJumping) {
            m_isJumping = true;
            m_jumpVelocity = m_jumpStrength;
        }
    }

    void updateJump(float deltaTime)
    {
        if (m_isJumping) {
            m_jumpVelocity += m_gravity * deltaTime;
            m_position.y += m_jumpVelocity * deltaTime;

            if (m_position.y <= 2.0f) {
                m_position.y = 2.0f;
                m_isJumping = false;
                m_jumpVelocity = 0.0f;
            }
        }
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

    bool m_isJumping{ false };
    float m_jumpVelocity{ 0.0f };
    const float m_gravity{ -9.81f };
    const float m_jumpStrength{ 5.0f };

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