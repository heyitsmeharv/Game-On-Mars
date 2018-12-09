#pragma once
#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT
#include <D3D11.h>
#include <xnamath.h>
#include <math.h>


class Camera
{
public:
	Camera(float x, float y, float z, float CameraRotation);
	~Camera();

	void RotateRight(float noOfDegrees);
	void RotateLeft(float noOfDegrees);
	void Forward(float distance);
	void Back(float distance);
	void Up(float yAxis);
	void Down(float yAxis);
	void Left(float xAxis);
	void Right(float xAxis);
	void Run(float distance);
	void Jump(float amount); 

	float getX();
	float getY();
	float getZ(); 

	float m_x;
	float m_y;
	float m_z;
	float m_d;
	float m_dx;
	float m_dy;
	float m_dz;
	float m_Camera_rotation;

	XMMATRIX GetViewMatrix();
	XMVECTOR GetPosition() { return XMVectorSet(m_x, m_y, m_z, 0.0f); }

private:

	XMVECTOR m_position;
	XMVECTOR m_lookat;
	XMVECTOR m_up;
	XMMATRIX view;

protected:

};

