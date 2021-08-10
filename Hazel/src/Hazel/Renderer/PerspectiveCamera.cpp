#include "hzpch.h"
#include "PerspectiveCamera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Hazel {
	PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, float nearPlane, float farPlane)
		:m_ProjectionMatrix(glm::perspective(fov, width/height, nearPlane, farPlane))
		,m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::SetProjection(float fov, float width, float height, float nearPlane, float farPlane)
	{
		m_ProjectionMatrix = glm::perspective(fov, width/height , nearPlane, farPlane);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::lookAt(m_Position, m_Position + m_Front, m_up);

		m_ViewMatrix = transform;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}