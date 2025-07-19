#pragma once
class TransformComponent;

class CharacterBase : public KdGameObject
{
public:

	CharacterBase(){}
	virtual ~CharacterBase()override{}

	void Init()			override;
	void Update()		override;
	void DrawLit()		override;
	void DrawUnLit()	override;

	virtual void Setmodel(){}

protected:

	std::shared_ptr<TransformComponent> m_transform;
	std::shared_ptr<KdModelWork> m_modelwork;

};
