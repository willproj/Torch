#include "EditorCamera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <core/events/Keyboard.h>
#include <core/events/Mouse.h>

namespace core
{
    EditorCamera::EditorCamera()
    {
        position = glm::vec3(0, 6360.0f-1, 0);
        focalPoint = glm::vec3(0.0f, 6360.0f, 0); // Look at the origin
        distance = glm::distance(position, focalPoint); // Calculate initial distance
        UpdateView();
    }

    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : FOV(fov), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip),
        Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
    {
        UpdateView();
    }

    void EditorCamera::Update()
    {
        if (m_Keyboard->IsKeyPressed(KeyCode::KEY_LEFT_CONTROL))
        {
            if (m_Keyboard->IsKeyPressed(KeyCode::KEY_LEFT_SHIFT))
            {
                // Move the camera up along the Y-axis when both Ctrl and Shift are pressed
                position += glm::vec3(0.0f, 100.0f, 0.0f) * ZoomSpeed();
                focalPoint += glm::vec3(0.0f, 100.0f, 0.0f) * ZoomSpeed();
            }

            if (isOnFocus)
            {
                auto posX = m_Mouse->GetCursorPosX();
                auto posY = m_Mouse->GetCursorPosY();

                auto offsetMouse = m_Mouse->GetPositionOffset() * 0.003f;

                if (m_Mouse->IsLeftButtonPressed())
                {
                    MouseRotate(offsetMouse);
                }
                else if (m_Mouse->IsRightButtonPressed())
                {
                    MousePan(offsetMouse);
                }

                auto offsetScroll = m_Mouse->GetScrollOffsetY();
                MouseZoom(offsetScroll);

                m_Mouse->UpdateMouse();
            }
            UpdateView();
        }
    }

    void EditorCamera::SetCameraAttributes(float fov, float aspectRatio, float nearClip, float farClip)
    {
        FOV = fov;
        this->aspectRatio = aspectRatio;
        this->nearClip = nearClip;
        this->farClip = farClip;

        projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        UpdateView();
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
    }

    void EditorCamera::UpdateProjection()
    {
        if (viewportHeight <= 0.0f)
        {
            viewportWidth = 1.0f;
        }
        float aspectRatio = viewportWidth / viewportHeight;
        projection = glm::perspective(glm::radians(FOV), aspectRatio, nearClip, farClip);
    }

    void EditorCamera::UpdateView()
    {
        position = CalculatePosition();
        glm::quat orientation = GetOrientation();
        viewMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
        viewMatrix = glm::inverse(viewMatrix);
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        focalPoint += -GetRightDirection() * delta.x * xSpeed * distance;
        focalPoint += GetUpDirection() * delta.y * ySpeed * distance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        yaw += yawSign * delta.x * RotationSpeed();
        pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta)
    {
        distance -= delta * ZoomSpeed();
        if (distance < 1.0f)
        {
            focalPoint += GetForwardDirection();
            distance = 1.0f;
        }
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return focalPoint - GetForwardDirection() * distance;
    }

    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(viewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(viewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float EditorCamera::RotationSpeed() const
    {
        return 0.8f;
    }

    float EditorCamera::ZoomSpeed() const
    {
        float distance = this->distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

    void EditorCamera::SetIsOnFocus(bool focus)
    {
        isOnFocus = focus;
    }
}
