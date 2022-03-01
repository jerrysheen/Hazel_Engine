#pragma once
#include "PerspectiveCamera.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Core/Timestep.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "glm/gtc/matrix_transform.hpp"
namespace Hazel {

	class PerspectiveCameraController {
	public:
		PerspectiveCameraController(float fov, float width, float height, float nearPlane, float farPlane);

		inline const PerspectiveCamera GetCamera() { return m_Camera; };
		void OnEvent(Event& e);
		void OnUpdate(Timestep ts);
		void onRender() {
		}
		void onImguiRender() {
		}

		void SetCameraProjection() {
		}

		void ResetCamera();
	private:
		bool OnMouseClicked(MouseButtonPressedEvent& e);

		bool OnMouseScrolled(MouseScrolledEvent& e);
	private:
		PerspectiveCamera m_Camera;
		glm::vec3 m_CameraPosition = { 0, 3, -10 };
		glm::vec3 m_front = { 0,0,1 };
		glm::vec3 m_up = { 0, 1, 0 };
		glm::mat4 m_viewMatrix;
		float m_CameraTranslationSpeed = 2.5;
		float m_Width, m_Height;
		bool m_FirstTimeTriggerCameraDirectionMove = true;
		bool m_FirstTimeTriggerCameraDirectionRotation = true;
		float lastRotationX, lastRotationY;
		float lastMoveX, lastMoveY;
		float yaw = 0, pitch = 0;
	};
}