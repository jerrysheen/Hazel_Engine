#pragma once

#include "glm/glm.hpp"
#include "Runtime/Graphics/RenderStruct.h"
#include "Runtime/Graphics/Renderer.h"
#include "Runtime/Graphics/RenderTarget.h"

namespace Hazel {
	class Camera {
	public:
		Camera(float fov, float width, float height, float nearPlane, float farPlane);
		
		void SetProjection(float fov, float width, float height, float nearPlane, float farPlane);

		//inline const glm::vec3& GetPosition() const { return m_Position; }

		void SetPosition(const glm::vec3& position)
		{
			m_Position = position;
			RecalculateViewMatrix();
		}

		inline float GetRotation() const { return m_Rotation; }

		void Render(RenderNode* node, RenderingData* data);

		void SetCameraFront(const glm::vec3& front) {
			m_Front = front;
			RecalculateViewMatrix();
		}
		void SetRotation(float rotation)
		{
			m_Rotation = rotation;
			RecalculateViewMatrix();
		}

		void ResetCamera();
		void ResetAspectRatio(float width, float height);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; };
		const  glm::vec3& GetCamPos() const { return m_Position; };
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; };
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; };

		Renderer* m_Renderer;
	private:
		void RecalculateViewMatrix();
	private:

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;
		glm::vec3 m_Position;
		
		float m_Fov;
		float m_Width;
		float m_Height;
		float m_NearPlane;
		float m_FarPlane;
		glm::mat4 m_Translation = glm::mat4(1.0f);
		float m_Rotation = 0.0f;
		glm::vec3 m_Front = { 0, 0, 1 };
		glm::vec3 m_up = { 0, 1.0f, 0.0f };

		Ref<RenderTarget> m_CameraColorAttachment;
		Ref<RenderTarget> m_CameraDepthAttachment;

	};
}
