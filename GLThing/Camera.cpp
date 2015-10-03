#include "Camera.h"

void Camera::setPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	projectionTransform = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
}

void Camera::setLookAt(vec3 From, vec3 To, vec3 Up)
{
	glm::lookAt(From, To, Up);
}

void Camera::setPosition(vec3 Position)
{
	worldTransform[0][0] = Position[0];
	worldTransform[0][1] = Position[1];
	worldTransform[0][2] = Position[2];
}

mat4 Camera::getProjection()
{
	return projectionTransform;
}

mat4 Camera::getWorldTransform()
{
	return worldTransform;
}

mat4 Camera::getView()
{
	return viewTransform;
}

mat4 Camera::getProjectionView()
{
	return projectionViewTransform;
}

void FlyCamera::update(float deltaTime)
{

}

void FlyCamera::setSpeed(float speed)
{
	this->speed = speed;
}