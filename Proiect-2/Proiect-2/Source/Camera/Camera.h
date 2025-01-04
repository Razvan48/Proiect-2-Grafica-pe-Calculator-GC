#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "../Source/WindowManager/WindowManager.h"

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	// Destructor
	~Camera();

	// Not copyable
	Camera(const Camera&) = delete;
	Camera& operator = (const Camera&) = delete;

	// Not moveable
	Camera(Camera&&) = delete;
	Camera& operator = (Camera&&) = delete;

	// Input
	void processKeyboard(Camera_Movement direction, float deltaTime);
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void processMouseScroll(float yoffset);

	// Getters
	static Camera& get();
	inline glm::mat4 getViewMatrix() const { return glm::lookAt(position, position + front, up); }
	inline glm::mat4 getProjectionMatrix() const { return glm::perspective(glm::radians(zoom), static_cast<float>(WindowManager::get().getWidth()) / static_cast<float>(WindowManager::get().getHeight()), 0.1f, 1000.0f); }
	inline float getZoom() const { return zoom; }
	inline glm::vec3 getPosition() const { return position; }

	// Setters
	inline void setMovementSpeed(float speed) { movementSpeed = speed; }
	inline void setPosition(glm::vec3 pos) { this->position = pos; }

	inline void invertPitch() { this->pitch *= -1; updateCameraVectors(); };

private:
	// camera attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	// euler angles
	float yaw;
	float pitch;

	// camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

	// Constructor with scalars
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	void updateCameraVectors();
};
