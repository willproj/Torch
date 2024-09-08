#pragma once
#include <pch/pch.h>
#include "Camera.h"

namespace core
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera();
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void Update();

        void SetCameraAttributes(float fov, float aspectRatio, float nearClip, float farClip);

        inline float GetDistance() const { return distance; }
        inline void SetDistance(float distance) { this->distance = distance; }

        inline void SetViewportSize(float width, float height) { viewportWidth = width; viewportHeight = height; UpdateProjection(); }

        const glm::mat4& GetViewMatrix() const { return viewMatrix; }
        glm::mat4 GetViewProjection() const { return projection * viewMatrix; }

        glm::vec3 GetUpDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3& GetPosition() const { return position; }
        glm::quat GetOrientation() const;

        float GetPitch() const { return pitch; }
        float GetYaw() const { return yaw; }
        void SetIsOnFocus(bool focus);

        float GetNearClip() const { return nearClip; }
        float GetFarClip() const { return farClip; }
        float GetZoom() const { return FOV; }


        float GetViewportWidth() const { return viewportWidth; }
        float GetViewportHeight() const { return viewportHeight; }

        std::vector<float> shadowCascadeLevels{ 25.0f, 75.0f, 200.0f, 1000.0f };

    private:
        void UpdateProjection();
        void UpdateView();

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const;

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;


    private:
        float FOV = 45.0f, aspectRatio = 1.778f, nearClip = 0.1f, farClip = 1000.0f;

        glm::mat4 viewMatrix;
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 focalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 initialMousePosition = { 0.0f, 0.0f };

        float distance = 10.0f;
        float pitch = 0.0f, yaw = 0.0f;

        float viewportWidth = 1280, viewportHeight = 720;

        bool isOnFocus = true;
    };
}
