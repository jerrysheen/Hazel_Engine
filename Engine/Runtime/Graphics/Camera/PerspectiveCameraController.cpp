#include "hzpch.h"
#include "PerspectiveCameraController.h"
#include "Runtime/Core/Input/Input.h"
#include "Runtime/Core/Input/KeyCode.h"


namespace Hazel {
	PerspectiveCameraController::PerspectiveCameraController(float fov, float width, float height, float nearPlane, float farPlane)
		:m_Camera(fov, width, height, nearPlane, farPlane),
		m_Width(width),
		m_Height(height),
		lastRotationX(width/2),
		lastRotationY(height/2),
		lastMoveX(width/2),
		lastMoveY(height/2),
		m_viewMatrix(glm::mat4(1.0f)),
		m_CameraPosition(0.0, 3, -10.0)
	{

	}
	

	void PerspectiveCameraController::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(PerspectiveCameraController::OnMouseScrolled));
		dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(PerspectiveCameraController::OnMouseClicked));
		dispatcher.Dispatch<MouseMovedEvent>(HZ_BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoved));
	}

	void PerspectiveCameraController::OnUpdate(Timestep ts)
	{

		//HZ_CORE_INFO("{0}, {1}, {2}", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
		glm::vec3 up = glm::dot(m_up, m_front) * m_front - m_up;
		glm::vec3 right = glm::normalize(glm::cross(m_front, up));

		if (Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_2)) {
			auto [xpos, ypos] = Input::GetMousePosition();
			float xoffset = xpos - lastRotationX;
			float yoffset = lastRotationY - ypos;
			if (m_FirstTimeTriggerCameraDirectionRotation) {
				m_FirstTimeTriggerCameraDirectionRotation = false;
				xoffset = 0;
				yoffset = 0;
			}

			lastRotationX = xpos;
			lastRotationY = ypos;

			float sensitivity = 0.05;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw -= xoffset;
			pitch -= yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			
			m_front.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			m_front.y = -sin(glm::radians(pitch));
			m_front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			m_front = glm::normalize(m_front);
			//HZ_CORE_INFO("x : {0}  y:{1}  z{2}", m_front.x, m_front.y, m_front.z);
			m_Camera.SetCameraFront(m_front);


		}
		else {
			m_FirstTimeTriggerCameraDirectionRotation = true;
		}
		up = glm::dot(m_up, m_front) * m_front - m_up;
		right = glm::normalize(glm::cross(m_front, up));


		if (Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_MIDDLE)) {
			auto [xpos, ypos] = Input::GetMousePosition();
			float xoffset = xpos - lastMoveX;
			float yoffset = lastMoveY - ypos;
			if (m_FirstTimeTriggerCameraDirectionMove) {
				m_FirstTimeTriggerCameraDirectionMove = false;
				xoffset = 0;
				yoffset = 0;
			}

			lastMoveX = xpos;
			lastMoveY = ypos;

			float sensitivity = 0.05;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			m_CameraPosition += m_CameraTranslationSpeed * ts * right * xoffset + m_CameraTranslationSpeed * ts * up * yoffset;
		}
		else {
			m_FirstTimeTriggerCameraDirectionMove = true;
		}

		if (Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_2)) 
		{
			if (Input::IsKeyPressed(HZ_KEY_A))
				m_CameraPosition += m_CameraTranslationSpeed * ts * right;
			else if (Input::IsKeyPressed(HZ_KEY_D))
				m_CameraPosition -= m_CameraTranslationSpeed * ts * right;

			if (Input::IsKeyPressed(HZ_KEY_W))
				m_CameraPosition += m_CameraTranslationSpeed * ts * m_front;
			else if (Input::IsKeyPressed(HZ_KEY_S))
				m_CameraPosition -= m_CameraTranslationSpeed * ts * m_front;
		}

		m_Camera.SetPosition(m_CameraPosition);
	
	}

	

	void PerspectiveCameraController::ResetCamera()
	{
		m_Camera.ResetCamera();
		m_FirstTimeTriggerCameraDirectionRotation = true;
		m_FirstTimeTriggerCameraDirectionMove = true;
		m_front = {0, 0, -1};
		m_CameraPosition = { 0, 0, -10 };
		yaw = 0;
		pitch = 0;
	}

	bool PerspectiveCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		float diff = e.GetYOffset();
		m_CameraPosition += diff * m_front * 0.2f;
		m_Camera.SetPosition(m_CameraPosition);
		return false;
	}
	bool PerspectiveCameraController::OnMouseMoved(MouseMovedEvent& e)
	{
		float x = e.GetX();
		float y = e.GetY();
		//HZ_CORE_INFO("{0}, {1} ", x, y);
		return false;
	}


	bool PerspectiveCameraController::OnMouseClicked(MouseButtonPressedEvent& e)
	{
		return false;
	}

	void PerspectiveCameraController::ResetAspectRatio(float width, float height)
	{
		m_Camera.ResetAspectRatio(width, height);
	}

	//bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	//{
	//	/*m_ZoomLevel -= e.GetYOffset() * 0.25;
	//	m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
	//	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	//	return false;*/
	//}

}