#include "GameObjects/Player.h"
#include "common/Input.h"
#include "common/utilsMat.h"
#include "common/opengl.h" // TODO: Remove this necessity, abstract key buttons values
#include "EngineMedia/MediaPlayer.h"
#include <glm/gtc/matrix_transform.hpp>

bool Player::Load(EntityID entityID, SceneView* pScene)
{
	this->DefaultObject::Load(entityID, pScene);

	this->m_pCamera = pScene->GetComponent<CameraComponent>(entityID, "camera");
	this->m_pForce = pScene->GetComponent<ForceComponent>(entityID, "force");

	return true;
}

void Player::OnStart(iEvent* pEvent)
{
	this->DefaultObject::OnStart(pEvent);

	this->m_currDirection = glm::vec3(0);
	this->m_firstUpdate = true;

	// Script custom
	//----------------
	this->m_sensitivity = std::stof(this->m_pScript->GetVariable("sensitivity"));
	this->m_velocity = std::stof(this->m_pScript->GetVariable("velocity"));
}

void Player::Update(double deltaTime)
{
	this->Move(deltaTime);

	this->UpdateCamera();

	if (Input::IsKeyPressed(GLFW_KEY_E))
	{
		this->m_pMediator->Interact(this->m_pTag->name, this->GetPosition());
	}
}

void Player::Move(double deltaTime)
{
	using namespace glm;

	vec3 newVelocity = vec3(0);
	vec3 cameraFront = this->m_pCamera->cameraFront;
	vec3 cameraUp = this->m_pCamera->upVector;
	vec3 direction = normalize(cross(cameraFront, cameraUp));

	// Front movement
	if (Input::IsKeyPressed(GLFW_KEY_W))
	{
		newVelocity += this->m_velocity * this->m_pCamera->cameraFront * (float)deltaTime;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_S))
	{
		newVelocity -= this->m_velocity * this->m_pCamera->cameraFront * (float)deltaTime;
	}

	// Sides movement
	if (Input::IsKeyPressed(GLFW_KEY_D))
	{
		newVelocity += this->m_velocity * direction * (float)deltaTime;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_A))
	{
		newVelocity -= this->m_velocity * direction * (float)deltaTime;
	}

	this->m_pForce->SetVelocity(newVelocity);

	vec3 newPosition = this->m_pTransform->GetPosition();

	this->m_pCamera->cameraEye = newPosition;
	vec3 forward = this->m_pCamera->cameraFront * vec3(-1, 0, -1);

	MediaPlayer::Get()->SetListenerAttributes(newPosition, vec3(0),
											forward, this->m_pCamera->upVector);
}

void Player::UpdateCamera()
{
	using namespace glm;

	// Only horizontal movement for camera
	vec2 mousePos = Input::MousePosition();
	float xpos = mousePos.x;

	if (this->m_firstUpdate)
	{
		this->m_lastX = xpos;
		this->m_firstUpdate = false;
	}

	float xoffset = xpos - this->m_lastX;
	this->m_lastX = xpos;

	xoffset *= m_sensitivity;

	this->m_yaw += xoffset;

	glm::vec3 front;
	front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	front.y = sin(glm::radians(this->m_pitch));
	front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	this->m_pCamera->cameraFront = glm::normalize(front);
}

void Player::Notify(iEvent* pEvent, sCollisionData* pCollision)
{
}
