#include "hzpch.h"
#include "PerspectiveCameraController.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCode.h"


namespace Hazel {
	PerspectiveCameraController::PerspectiveCameraController(float fov, float width, float height, float nearPlane, float farPlane)
		:m_Camera(fov, width, height, nearPlane, farPlane),
		m_Width(width),
		m_Height(height),
		lastX(width/2),
		lastY(height/2),
		m_viewMatrix(glm::mat4(1.0f))
	{

	}
	

	void PerspectiveCameraController::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
		dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(PerspectiveCameraController::OnMouseClicked));
	}

	void PerspectiveCameraController::OnUpdate(Timestep ts)
	{
		if (Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_2)) {
			auto [xpos, ypos] = Input::GetMousePosition();
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;
			if (FirstTimeTriggerCameraDirectionMove) {
				FirstTimeTriggerCameraDirectionMove = false;
				xoffset = 0;
				yoffset = 0;
			}

			lastX = xpos;
			lastY = ypos;

			float sensitivity = 0.05;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			glm::vec3 front;
			front.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front.y = -sin(glm::radians(pitch));
			front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			front = glm::normalize(front);
			HZ_CORE_INFO("x : {0}  y:{1}  z{2}", front.x, front.y, front.z);
			m_Camera.SetCameraFront(front);

		}
		else {
			FirstTimeTriggerCameraDirectionMove = true;
		}

		if (Input::IsKeyPressed(HZ_KEY_A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(HZ_KEY_D))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;

		if (Input::IsKeyPressed(HZ_KEY_W))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(HZ_KEY_S))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;

		m_Camera.SetPosition(m_CameraPosition);
	}

	

	bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		float diff = e.GetYOffset();
		m_CameraPosition.z += diff;
		m_Camera.SetPosition(m_CameraPosition);
		return false;
	}

	bool PerspectiveCameraController::OnMouseClicked(MouseButtonPressedEvent& e)
	{
		return false;
	}

	//bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	//{
	//	/*m_ZoomLevel -= e.GetYOffset() * 0.25;
	//	m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
	//	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	//	return false;*/
	//}

}