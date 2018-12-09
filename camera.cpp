#include "Camera.h"


Camera::Camera(float x, float y, float z, float CameraRotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_Camera_rotation = CameraRotation;

	m_dx = sin(m_Camera_rotation * (XM_PI / XMConvertToRadians(180.0)));
	m_dz = cos(m_Camera_rotation * (XM_PI / XMConvertToRadians(180.0)));
	m_dy = tan(m_Camera_rotation * (XM_PI / XMConvertToRadians(180.0)));

	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
}


Camera::~Camera()
{
}


void Camera::RotateRight(float noOfDegrees)
{

	m_Camera_rotation += noOfDegrees;

	m_dx = sin(XMConvertToRadians(m_Camera_rotation));
	m_dz = cos(XMConvertToRadians(m_Camera_rotation));

}

void Camera::RotateLeft(float noOfDegrees)
{

	m_Camera_rotation -= noOfDegrees;

	m_dx = sin(XMConvertToRadians(m_Camera_rotation));
	m_dz = cos(XMConvertToRadians(m_Camera_rotation));

}

void::Camera::Jump(float amount) {

	m_y += amount;

}

void Camera::Forward(float distance)
{
	m_x += distance * m_dx;
	m_z += distance * m_dz;
}

void Camera::Back(float distance) {

	m_x -= distance * m_dx;
	m_z -= distance * m_dz;

}

void Camera::Run(float distance) {

	m_x += distance * m_dx;
	m_z += distance * m_dz;

}

void Camera::Up(float yAxis)
{

	m_y += yAxis; 

}

void Camera::Down(float yAxis)
{

	m_y -= yAxis;

}

void Camera::Left(float xAxis)
{

	m_x -= xAxis;

}

void Camera::Right(float xAxis)
{

	m_x += xAxis;

}

XMMATRIX Camera::GetViewMatrix()
{

	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	view = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return view;

}

float Camera::getX(){

	return m_x; 
}

float Camera::getY(){

	return m_y;
}

float Camera::getZ(){

	return m_z;
}