#include "Player.h"
#include"../../../Component/PlayerComponent/PlayerControl.h"
#include"../../../Component/Transform/TransformComponent.h"
void Player::Init()
{
	CharacterBase::Init();
	m_objtype = KdGameObject::ObjctType::Player;
	std::shared_ptr<PlayerControl> playercontrol = std::make_shared<PlayerControl>();
	m_transform = std::make_shared<TransformComponent>();
	AddComponent(m_transform);
	AddComponent(playercontrol);
}

void Player::Update()
{
	CharacterBase::Update();
}
