#pragma once
#include "Model.h"


class SceneNode
{
public:
	SceneNode(Model* model = nullptr);
	~SceneNode();

	XMVECTOR Get_World_Position() { return XMVectorSet(m_world_centre_x, m_world_centre_y, m_world_centre_z, 0.0f); }

	void addChildNode(SceneNode* n);
	bool detatchNode(SceneNode* n);
	void execute(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);

	void SetPos(float x, float y, float z) { m_x = x, m_y = y, m_z = z; }
	float GetPosX() { return m_x; }
	float GetPosY() { return m_y; }
	float GetPosZ() { return m_z; }

	void SetRot(float x, float y, float z) { m_xangle = x, m_yangle = y, m_zangle = z; }
	float GetRotX() { return m_xangle; }
	float GetRotY() { return m_yangle; }
	float GetRotZ() { return m_zangle; }

	void SetScale(float scale) { m_scale = scale; }
	float GetScaleX() { return m_scale; }
	float GetScaleY() { return m_scale; }
	float GetScaleZ() { return m_scale; }

	void up(float amount);
	void down(float amount);
	void left(float amount);
	void right(float amount);

	void Rotate(float rotateAmount);

	void Update_Collision_Tree(XMMATRIX* world, float scale); 
	bool Check_Collision(SceneNode* compare_tree, SceneNode* object_tree_root);
	bool check_collision(SceneNode* compare_tree);
	bool IncX(float in, SceneNode* root_node);
	bool IncY(float in, SceneNode* root_node);
	bool IncZ(float in, SceneNode* root_node);

	bool draw;


	//void setPos(XMVECTOR setPosition) { m_Position = setPosition; }
	//XMVECTOR getPos() { return m_Position; }

	//void setRot(XMVECTOR setRotation) { m_Rotation = setRotation; }
	//XMVECTOR getRot() { return m_Rotation; }

	//void setScale(XMVECTOR setScale) { m_Scale = setScale; }
	//XMVECTOR getScale() { return m_Scale; }

protected:
	Model*		m_pModel; 
	Model*		GetModel() { return m_pModel; }

	vector<SceneNode*> m_children;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;
	float m_scaleX;

	float m_world_centre_x, m_world_centre_y, m_world_centre_z;
	float m_world_scale;

	// Direct X XMVECTOR more efficient than three floats?
	/*XMVECTOR m_Position;
	XMVECTOR m_Rotation;
	XMVECTOR m_Scale;*/
};

