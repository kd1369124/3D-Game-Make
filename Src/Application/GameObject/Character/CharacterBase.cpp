#include "CharacterBase.h"
#include"../../Component/Transform/TransformComponent.h"
void CharacterBase::Init()
{

	
	Setmodel();
}

void CharacterBase::Update()
{
	KdGameObject::Update();
}

void CharacterBase::DrawLit()
{
	KdGameObject::DrawLit();
}

void CharacterBase::DrawUnLit()
{
	KdGameObject::DrawUnLit();

}
