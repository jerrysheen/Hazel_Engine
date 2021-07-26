#pragma once
#include "Hazel/Core/Timestep.h"
#include "glm/glm.hpp"
#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
namespace Hazel {

	class OrthographicCameraController 
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);
		//void Bind(const OrthographicCamera& camera);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		inline const OrthographicCamera GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		//inline const glm::vec3 GetPosition() const { return m_CameraPosition; }
		//inline const float GetRoation() const { return m_CameraRotation; }
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;
		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};
}