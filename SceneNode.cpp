#include "SceneNode.h"


SceneNode::SceneNode(Model* model)
{
	m_pModel = model; 

	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
	m_xangle = 0.0f;
	m_yangle = 0.0f;
	m_zangle = 0.0f;
	m_scale = 1.0f;

	draw = true; 

}


SceneNode::~SceneNode()
{
}

void SceneNode::addChildNode(SceneNode *n)
{
	m_children.push_back(n);
}

bool SceneNode::detatchNode(SceneNode *n)
{
	// traverse tree to find node to detatch
	for (int i = 0; i < m_children.size(); i++)
	{
		if (n == m_children[i])
		{
			m_children.erase(m_children.begin() + i);
			return true;
		}
		if (m_children[i]->detatchNode(n) == true) return true;
	}
	return false; // node not in this tree
}

void SceneNode::execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection)
{
	// the local_world matrix will be used to calc the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	local_world *= XMMatrixScaling(m_scale, m_scale, m_scale);

	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	// only draw if there is a model attached
	if (m_pModel && draw) {
		m_pModel->Draw(&local_world, view, projection);
	}
	// traverse all child nodes, passing in the concatenated world matrix
	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->execute(&local_world, view, projection);
	}
}

void SceneNode::up(float amount){
	m_y += amount;
}

void SceneNode::down(float amount){

	m_y -= amount;
}

void SceneNode::left(float amount) {

	m_x -= amount;
}

void SceneNode::right(float amount) {

	m_x += amount;

}

void SceneNode::Rotate(float rotateAmount) {

	m_yangle -= rotateAmount; 

}

void SceneNode::Update_Collision_Tree(XMMATRIX* world, float scale) {

	// the local_world matrix will be used to calculate the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	local_world *= XMMatrixScaling(m_scale, m_scale, m_scale);

	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	// calc the world space scale of this object, is needed to calculate the  
	// correct bounding sphere radius of an object in a scaled hierarchy
	m_world_scale = scale * m_scale;

	XMVECTOR v;
	if (m_pModel)
	{
		v = m_pModel->GetBoundingSphereWorldSpacePosition();
		/*v = XMVectorSet(m_pModel->GetBoundingSphereX(),
			m_pModel->GetBoundingSphereY(),
			m_pModel->GetBoundingSphereZ(), 0.0);*/
	}
	else v = XMVectorSet(0, 0, 0, 0); // no model, default to 0

	// find and store world space bounding sphere centre
	v = XMVector3Transform(v, local_world);
	m_world_centre_x = XMVectorGetX(v);
	m_world_centre_y = XMVectorGetY(v);
	m_world_centre_z = XMVectorGetZ(v);

	// traverse all child nodes, passing in the concatenated world matrix and scale
	for (int i = 0; i< m_children.size(); i++)
	{
		m_children[i]->Update_Collision_Tree(&local_world, m_world_scale);
	}

}

bool SceneNode::Check_Collision(SceneNode* compare_tree, SceneNode* object_tree_root) //15
{
	// check to see if root of tree being compared is same as root node of object tree being checked
	// i.e. stop object node and children being checked against each other
	if (object_tree_root == compare_tree){
		return false;
	}

	// only check for collisions if both nodes contain a model
	if (m_pModel && compare_tree->m_pModel)
	{
		XMVECTOR v1 = Get_World_Position();
		XMVECTOR v2 = compare_tree->Get_World_Position();
		XMVECTOR vdiff = v1 - v2;

		//XMVECTOR a = XMVector3Length(vdiff);
		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		float front1 = z1 - m_pModel->GetDepth(); 
		float back1 = z1 + m_pModel->GetDepth(); 
		float top1 = y1 + m_pModel->GetHeight(); 
		float bottom1 = y1 - m_pModel->GetHeight(); 
		float left1 = x1 - m_pModel->GetWidth();
		float right1 = x1 + m_pModel->GetWidth();

		float front2 = z2 - compare_tree->GetModel()->GetDepth(); 
		float back2 = z2 + compare_tree->GetModel()->GetDepth(); 
		float top2 = y2 + compare_tree->GetModel()->GetHeight();
		float bottom2 = y2 - compare_tree->GetModel()->GetHeight();
		float left2 = x2 - compare_tree->GetModel()->GetWidth();
		float right2 = x2 + compare_tree->GetModel()->GetWidth(); 

		if ((left1 < right2) && (right1 > left2) && (top1 > bottom2) &&
			(bottom1 < top2) && (front1 < back2) && (back1 > front2)) {
			
			draw = false;
			return true;

		}
	}


		//// check bounding sphere collision
		//if (sqrt(dx*dx + dy*dy + dz*dz) <
		//	(compare_tree->m_pModel->GetBoundingSphereRadius() * compare_tree->m_world_scale) +
		//	(this->m_pModel->GetBoundingSphereRadius() * m_world_scale))
		//{
		//	return true;
		//}

	
		// iterate through compared tree child nodes
		for (int i = 0; i < compare_tree->m_children.size(); i++)
		{
			// check for collsion against all compared tree child nodes 
			if (Check_Collision(compare_tree->m_children[i], object_tree_root) == true) return true;
		}

		// iterate through composite object child nodes
		for (int i = 0; i < m_children.size(); i++)
		{
			// check all the child nodes of the composite object against compared tree
			if (m_children[i]->Check_Collision(compare_tree, object_tree_root) == true) return true;
		}

		return false;

}

bool SceneNode::check_collision(SceneNode* compare_tree)
{
	return Check_Collision(compare_tree, this);
}


bool SceneNode::IncX(float in, SceneNode* root_node)
{
	float old_x = m_x;	// save current state 
	m_x += in;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->Update_Collision_Tree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (Check_Collision(root_node, this) == true)
	{
		// if collision restore state
		m_x = old_x;

		return true;
	}

	return false;
}

bool SceneNode::IncY(float in, SceneNode* root_node)
{
	float old_y = m_y;	// save current state 
	m_y += in;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->Update_Collision_Tree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (Check_Collision(root_node, this) == true)
	{
		// if collision restore state
		m_y = old_y;

		return true;
	}

	return false;
}

bool SceneNode::IncZ(float in, SceneNode* root_node)
{
	float old_z = m_z;	// save current state 
	m_z += in;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->Update_Collision_Tree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (Check_Collision(root_node, this) == true)
	{
		// if collision restore state
		m_z = old_z;

		return true;
	}

	return false;
}
