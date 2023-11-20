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
    vec3 cameraPosition = this->m_pTransform->GetPosition();
    vec3 cameraOrientation = normalize(this->m_pTransform->GetOrientation());
    vec3 cameraUpVector = this->m_pCamera->upVector;

    vec3 cameraDirection = normalize(cross(cameraOrientation, cameraUpVector));

	// Front movement
	if (Input::IsKeyPressed(GLFW_KEY_W))
	{
		newVelocity += this->m_velocity * cameraOrientation * (float)deltaTime;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_S))
	{
		newVelocity -= this->m_velocity * cameraOrientation * (float)deltaTime;
	}

	// Sides movement
	if (Input::IsKeyPressed(GLFW_KEY_D))
	{
		newVelocity += this->m_velocity * cameraDirection * (float)deltaTime;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_A))
	{
		newVelocity -= this->m_velocity * cameraDirection * (float)deltaTime;
	}

	this->m_pForce->SetVelocity(newVelocity);

	vec3 newPosition = this->m_pTransform->GetPosition();

	this->m_pTransform->SetPosition(newPosition);
	vec3 forward = cameraOrientation * vec3(-1, 0, -1);

	MediaPlayer::Get()->SetListenerAttributes(newPosition, vec3(0),
											forward, this->m_pCamera->upVector);
}

void Player::UpdateCamera()
{
	using namespace glm;

	// Only horizontal movement for camera
	vec2 mousePos = Input::MousePosition();
	float xpos = mousePos.x;
	float ypos = mousePos.y;

	if (m_firstUpdate)
	{
		m_lastX = xpos;
		m_lastY = ypos;
		m_firstUpdate = false;
	}

	float xoffset = xpos - m_lastX;
	float yoffset = ypos - m_lastY;
	m_lastX = xpos;
	m_lastY = ypos;
	xoffset *= m_sensitivity;
	yoffset *= m_sensitivity;

	vec3 rotation = m_pTransform->GetOrientation();

	rotation.x += xoffset;
	rotation.y += yoffset;

	m_pTransform->SetOrientation(rotation);
}

void Player::Notify(iEvent* pEvent, sCollisionData* pCollision)
{
}
