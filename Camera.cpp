#include "Camera.h"

glm::vec3 Camera::s_worldUp(0.0f, 1.0f, 0.0f);
float Camera::speed = 10.0f;

Camera::Camera(const glm::vec3& position, const glm::vec3& front, float yaw, float pitch){
	m_position = position;
	m_front = front;
	m_yaw = yaw;
	m_pitch = pitch;
	m_projection = glm::perspective(glm::radians(70.0f), (float)800 / (float)600, 0.1f, 1000.0f);
	RecreateLootAt();
}

void Camera::Rotate(const sf::Vector2i& mouseDelta)
{
	m_yaw += mouseDelta.x;
	m_pitch -= mouseDelta.y;

	m_pitch = std::min(std::max(m_pitch, -89.0f), 89.0f);
	RecreateLootAt();
}

void Camera::MoveForward(float dt){
	m_position += m_front * dt * speed;
	RecreateLootAt();
}

void Camera::MoveBackward(float dt){
	m_position -= m_front * dt * speed;
	RecreateLootAt();
}

void Camera::MoveLeft(float dt){
	m_position -= m_right * dt * speed;
	RecreateLootAt();
}

void Camera::MoveRight(float dt){
	m_position += m_right * dt * speed;
	RecreateLootAt();
}

void Camera::MoveUp(float dt){
	m_position += m_up * dt * speed;
	RecreateLootAt();
}

void Camera::MoveDown(float dt){
	m_position -= m_up * dt * speed;
	RecreateLootAt();
}

void Camera::RecreateLootAt(){
	m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front.y = sin(glm::radians(m_pitch));
	m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(m_front);

	m_right = glm::normalize(glm::cross(m_front, s_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));

	m_lookAt = glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::vec3 Camera::GetPosition() {
	return m_position;
}

glm::vec3 Camera::GetFront() {
	return m_front;
}



