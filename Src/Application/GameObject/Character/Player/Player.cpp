#include "Player.h"
#include"../../../Component/PlayerComponent/PlayerControl.h"
#include"../../../Component/Transform/TransformComponent.h"
#include"../../../Component/ModelComponent/ModelComp.h"
void Player::Init()
{
	m_objtype = KdGameObject::ObjctType::Player;
	std::shared_ptr<PlayerControl> playercontrol = std::make_shared<PlayerControl>();
	m_transform = std::make_shared<TransformComponent>();
	std::shared_ptr<ModelCpmp> modelcomp = std::make_shared<ModelCpmp>();

	AddComponent(modelcomp);
	AddComponent(m_transform);
	AddComponent(playercontrol);
	modelcomp->SetModel("Asset/Models/Character/Player/Maid.gltf");
}

void Player::Update()
{
	CharacterBase::Update();
}

void Player::DrawLit()
{
	if (!m_modelData)return;
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_modelData,m_mWorld);
}
