﻿#pragma once
#include"../../Application/Component/Component.h"
// ゲーム上に存在するすべてのオブジェクトの基底となるクラス
class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum
	{
		eDrawTypeLit = 1 << 0,
		eDrawTypeUnLit = 1 << 1,
		eDrawTypeBright = 1 << 2,
		eDrawTypeUI = 1 << 3,
		eDrawTypeDepthOfShadow = 1 << 4,
	};
	enum ObjctType
	{
		Player,
		Enemy,
		NoEntity
	};

	KdGameObject() {}
	virtual ~KdGameObject() { Release(); }


	// コンポーネント追加用
	void AddComponent(std::shared_ptr<Component> comp) 
	{
		comp->SetOwner(shared_from_this());
		m_components.push_back(comp);
	}

	// 生成される全てに共通するパラメータに対する初期化のみ
	virtual void Init() {}
	virtual void PreUpdate() {}
	virtual void Update();
	virtual void PostUpdate() {}

	// それぞれの状況で描画する関数
	virtual void GenerateDepthMapFromLight() {}
	virtual void PreDraw()    {}
	virtual void DrawLit()    { for (auto& comp : m_components) comp->DrawLit(); }
	virtual void DrawUnLit()  { for (auto& comp : m_components) comp->DrawUnLit(); }
	virtual void DrawEffect() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() { for (auto& comp : m_components) comp->Draw(); }
	virtual void DrawDebug();
	void OnImGui()
	{
		for (auto& comp : m_components) comp->OnImGui();
	}

	virtual std::string GetObjectName() const { return "Entity"; }

	template<typename T>
	std::shared_ptr<T> GetComponent() {
		for (auto& comp : m_components) {
			if (auto c = std::dynamic_pointer_cast<T>(comp)) {
				return c;
			}
		}
		return nullptr;
	}

	const std::vector<std::shared_ptr<Component>>& GetComponents() const { return m_components; }
	virtual void SetAsset(const std::string&) {}

	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	// 拡大率を変更する関数
	void SetScale(float scalar);
	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	const Math::Matrix& GetMatrix() const { return m_mWorld; }

	virtual bool IsExpired() const { return m_isExpired; }

	virtual bool IsVisible()	const { return false; }
	virtual bool IsRideable()	const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);

	virtual ObjctType GetObjectType() { return m_objtype; }


	// 3D空間に存在する機能
	Math::Matrix	m_mWorld;

protected:

	void Release() {}

	// 描画タイプ・何の描画を行うのかを決める / 最適な描画リスト作成用
	UINT m_drawType = 0;

	// カメラからの距離
	float m_distSqrFromCamera = 0;

	// 存在消滅フラグ
	bool m_isExpired = false;


	// 当たり判定クラス
	std::unique_ptr<KdCollider> m_pCollider = nullptr;

	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

	std::vector<std::shared_ptr<Component>> m_components;

	ObjctType m_objtype;
};
