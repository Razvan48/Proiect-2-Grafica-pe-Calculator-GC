#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

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
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	// Getters
	static Camera& Get();
	inline glm::mat4 GetViewMatrix() const { return glm::lookAt(position, position + front, up); }
	inline float GetZoom() const { return zoom; }

	inline glm::vec3 GetPosition() const { return position; }

	// Setters
	inline void SetMovementSpeed(float speed) { movementSpeed = speed; }

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

	void UpdateCameraVectors();
};
