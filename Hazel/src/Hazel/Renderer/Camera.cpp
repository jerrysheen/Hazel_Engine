#include "hzpch.h"
#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Hazel {
	Camera::Camera(float fov, float width, float height, float nearPlane, float farPlane)
		:m_ProjectionMatrix(glm::perspective(fov, width/height, nearPlane, farPlane))
		,m_ViewMatrix(1.0f)
		,m_Position(1.0f)
		,m_Fov(fov)
		,m_Width(width)
		,m_Height(height)
		,m_NearPlane(nearPlane)
		,m_FarPlane(farPlane)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::SetProjection(float fov, float width, float height, float nearPlane, float farPlane)
	{
		m_ProjectionMatrix = glm::perspective(fov, width/height , nearPlane, farPlane);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::ResetCamera()
	{
		m_Position = { 0, 10,0 };
		m_Front = { 0, 0, -1 };
		m_up = { 0, 1.0f, 0.0f };
		RecalculateViewMatrix();
	}

	void Camera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::lookAt(m_Position, m_Position + m_Front, m_up);

		m_ViewMatrix = transform;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::ResetAspectRatio(float width, float height)
	{
		m_ProjectionMatrix = glm::perspective(m_Fov, width / height, m_NearPlane, m_FarPlane);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}