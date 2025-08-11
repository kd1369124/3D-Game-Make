#pragma once
class KdGameObject;
// 基底
class Component {
public:

	Component(){}
	virtual ~Component() {}

	
	virtual void Update(float dt) = 0; 

	virtual void Draw() {}

	virtual void DrawLit() {}
	
	virtual void DrawUnLit() {}

	virtual std::string GetComponentName() const { return "Component"; }

	// エディタ
	virtual void OnImGui() = 0;

	void SetOwner(std::weak_ptr<KdGameObject> owner) { m_owner = owner; }

protected:
	std::weak_ptr<KdGameObject> m_owner;
};
