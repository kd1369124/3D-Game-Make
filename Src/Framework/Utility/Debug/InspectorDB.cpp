#include "InspectorDB.h"
#include <sstream>


#ifdef _WIN32
#include <windows.h>
#include"../../../Application/Component/Component.h"
#include"../../../Application/Component/Transform/TransformComponent.h"
#include"../../../Application/Component/ModelComponent/ModelComp.h"
#include"../../../Application/Component/PlayerComponent/PlayerControl.h"
#include"../../../Application/Component/CameraComponent/CameraComp.h"
#include"../../../Application/Component/ColiderComponent/ColiderComponent.h"

static void dbg(const std::string& s) { OutputDebugStringA((s + "\n").c_str()); }
#else
static void dbg(const std::string& s) { (void)s; }
#endif
std::vector<std::string> InspectorDB::ListComponents(ObjId id) const
{
	std::vector<std::string> out;
	auto it = m_objs.find(id);
	if (it == m_objs.end()) return out;

	if (auto sp = it->second.lock()) {
		const auto& comps = sp->GetComponents();
		for (const auto& c : comps) 
		{
		 
			if (c) out.push_back(c->GetComponentName());
		}
	}
	return out;
}

std::vector<Property> InspectorDB::GetProperties(ObjId id, const std::string& compName) const
{
	std::vector<Property> props;
	auto it = m_objs.find(id);
	if (it == m_objs.end()) return props;

	if (auto sp = it->second.lock()) {
		const auto& comps = sp->GetComponents();
		for (const auto& c : comps) {
			if (!c || c->GetComponentName() != compName) continue;
			c->ExposeProperties(props);
			break;
		}
	}
	return props;
}

std::optional<PropValue> InspectorDB::Get(ObjId id, const std::string& comp, const std::string& prop) const
{
	auto it = m_objs.find(id);
	if (it == m_objs.end()) return std::nullopt;

	if (auto sp = it->second.lock())
	{
		const auto& comps = sp->GetComponents();
		for (const auto& c : comps)
		{
			if (!c || c->GetComponentName() != comp) continue;

			std::vector<Property> ps;
			c->ExposeProperties(ps);
			for (auto& p : ps)
				if (p.name == prop) return p.getter();

			//break; // 同名コンポーネントを複数許容しない設計なら break
		}
	}
	return std::nullopt;
}

bool InspectorDB::Set(ObjId id, const std::string& comp, const std::string& prop, const PropValue& v) const
{
	auto it = m_objs.find(id);
	if (it == m_objs.end()) return false;
	if (auto sp = it->second.lock())
	{
		const auto& comps = sp->GetComponents();
		for (const auto& c : comps)
		{
			if (!c || c->GetComponentName() != comp) continue;
			std::vector<Property> ps;
			c->ExposeProperties(ps);
			for (auto& p : ps)
			{
				if (p.name == prop)
				{
					p.setter(v);
					return true;
				}
			}
			//break; // 同名コンポーネントを複数許容しない設計なら break
		}
	}
	return false;
}

nlohmann::json InspectorDB::Dump(ObjId id) const
{
	nlohmann::json j; // 返却用

	auto it = m_objs.find(id);
	if (it == m_objs.end()) return j;

	if (auto sp = it->second.lock())
	{
		j["name"] = sp->GetObjectName();

		
		nlohmann::json jcomps = nlohmann::json::array();
		const auto& compList = sp->GetComponents();

		for (const auto& comp : compList)
		{
			if (!comp) continue;

			nlohmann::json cj;
			cj["name"] = comp->GetComponentName();

			// プロパティを JSON に落とす
			std::vector<Property> props;
			comp->ExposeProperties(props);
			if (!props.empty())
			{
				nlohmann::json jprops = nlohmann::json::object();
				for (auto& p : props)
				{
					// PropValue -> json 変換関数（既存の ToJson を使用）
					jprops[p.name] = ToJson(p.getter());
				}
				cj["props"] = std::move(jprops);
			}

			jcomps.push_back(std::move(cj));
		}

		j["components"] = std::move(jcomps);
	}

	return j;
}

bool InspectorDB::Apply(ObjId id, const nlohmann::json& j) const
{
	auto it = m_objs.find(id);
	if (it == m_objs.end()) return false;

	if (auto sp = it->second.lock()) {
		if (!j.contains("components")) return false;
		for (const auto& cj : j["components"]) {
			std::string cname = cj.value("name", "");
			if (cname.empty() || !cj.contains("props")) continue;

			auto props = GetProperties(id, cname);
			const auto& pj = cj["props"];
			for (auto& p : props) {
				if (!pj.contains(p.name)) continue;
				PropValue hint = p.getter(), nv;
				if (FromJson(pj[p.name], nv, hint)) p.setter(nv);
			}
		}
		return true;
	}
	return false;
}

bool InspectorDB::SaveToFile(ObjId id, const std::string& pathStr) const
{
	dbg(std::string("SaveToFile: ") + pathStr);

	auto it = m_objs.find(id);
	if (it == m_objs.end()) return false;

	if (auto obj = it->second.lock())
	{
		ProjectSpec proj = NodeManager::LoadProject(pathStr);

		CharacterSpec me = BuildCharacterSpec(obj);

		if (me.name.empty() || me.components.empty())
			return false;

		bool replaced = false;
		for (auto& ch : proj.characters) {
			if (ch.name == me.name) { ch = me; replaced = true; break; }
		}
		if (!replaced) proj.characters.push_back(me);

		nlohmann::json j = proj;
		std::filesystem::create_directories(std::filesystem::path(pathStr).parent_path());
		std::ofstream ofs(pathStr, std::ios::binary | std::ios::trunc);
		if (!ofs.is_open()) return false;
		ofs << j.dump(2);
		return true;
	}


	return false;
}

bool InspectorDB::LoadFromFile(ObjId id, const std::string& pathStr) const
{
	auto it = m_objs.find(id);
	if (it == m_objs.end()) return false;

	if (auto obj = it->second.lock())
	{
		ProjectSpec proj = NodeManager::LoadProject(pathStr);
		if (proj.characters.empty()) return false;

		const std::string myName = obj->GetObjectName();
		const CharacterSpec* found = nullptr;
		for (auto& ch : proj.characters) {
			if (ch.name == myName) { found = &ch; break; }
		}
		if (!found) return false;

		ApplyCharacterSpec(obj, *found,proj.base);
		return true;
	}
	return false;
}

nlohmann::json InspectorDB::ToJson(const PropValue& v)
{
	// variant を JSON へ
	return std::visit([](auto&& val) -> nlohmann::json
		{
			using T = std::decay_t<decltype(val)>;
			if constexpr (std::is_same_v<T, Math::Vector3>)
			{
				return nlohmann::json::array({ val.x, val.y, val.z });
			}
			else
			{
				// int / float / bool / string はそのまま入る
				return nlohmann::json(val);
			}
		}, v);
}

static bool ReadVec3(const nlohmann::json& j, Math::Vector3& out)
{
	if (!j.is_array() || j.size() < 3) return false;
	// 数値かどうかの緩め判定（文字列→数値にしたいならここで変換ロジックを）
	if (!j[0].is_number() || !j[1].is_number() || !j[2].is_number()) return false;
	out.x = j[0].get<float>();
	out.y = j[1].get<float>();
	out.z = j[2].get<float>();
	return true;
}

bool InspectorDB::FromJson(const nlohmann::json& j, PropValue& out, const PropValue& hint)
{
	// ヒントの型に合わせて読み取る（型タグ不要）
	switch (hint.index())
	{
	case 0: // int
		if (j.is_number_integer()) { out = j.get<int>();   return true; }
		if (j.is_number_float()) { out = (int)j.get<float>(); return true; }
		break;
	case 1: // float
		if (j.is_number()) { out = j.get<float>(); return true; }
		break;
	case 2: // bool
		if (j.is_boolean()) { out = j.get<bool>();  return true; }
		// 文字列 "true"/"false" を許容したい場合はここで変換
		if (j.is_string()) {
			std::string s = j.get<std::string>();
			if (s == "true") { out = true;  return true; }
			if (s == "false") { out = false; return true; }
		}
		break;
	case 3: // std::string
		if (j.is_string()) { out = j.get<std::string>(); return true; }
		// 数値→文字列化を許容するなら以下
		if (j.is_number_integer()) { out = std::to_string(j.get<int>());   return true; }
		if (j.is_number_float()) { out = std::to_string(j.get<float>()); return true; }
		if (j.is_boolean()) { out = j.get<bool>() ? "true" : "false"; return true; }
		break;
	case 4: // Math::Vector3
	{
		Math::Vector3 v{};
		if (ReadVec3(j, v)) { out = v; return true; }
	}
	break;
	default:
		break;
	}
	return false; // 型が合わない/読めない
}

CharacterSpec InspectorDB::BuildCharacterSpec(const std::shared_ptr<KdGameObject>& obj) const
{
	CharacterSpec out;
	if (!obj) return out;

	out.name = obj->GetObjectName();

	// --- Transform ---
	if (auto t = obj->GetComponent<TransformComponent>()) 
	{
		
		CompSpec c;
		c.name = "TransformComponent";

		Math::Vector3 p = t->GetPosition();
		Math::Vector3 r = t->GetRotation(); // ラジアン前提
		Math::Vector3 s = t->GetScale();

		c.props["position"] = { p.x, p.y, p.z };
		c.props["rotation_deg"] = {
			DirectX::XMConvertToDegrees(r.x),
			DirectX::XMConvertToDegrees(r.y),
			DirectX::XMConvertToDegrees(r.z)
		};
		c.props["scale"] = { s.x, s.y, s.z };

		out.components.push_back(std::move(c));
	}

	// --- Model ---
	
		const auto& comps = obj->GetComponents();
		for (const auto& anyc : comps)
		{
			auto m = std::dynamic_pointer_cast<ModelComp>(anyc);
			if (!m) continue;

			CompSpec c; 
			c.name = "ModelComp";

			if (!m->GetModelPath().empty())
				c.props["modelPath"] = m->GetModelPath();
			c.props["modelPath"] = m->GetModelPath();
			c.props["modelBackend"] = (m->GetBackend() == ModelComp::Backend::Work ? "work" : "data");

			out.components.push_back(std::move(c));
		}
	

	// --- PlayerControl（プロパティ無し想定） ---
	if (obj->GetComponent<PlayerControl>()) {
		CompSpec c; c.name = "PlayerControl";
		out.components.push_back(std::move(c));
	}

	{
		//const auto& comps = obj->GetComponents();
		for (const auto& anyc : obj->GetComponents()) {
			auto cam = std::dynamic_pointer_cast<CameraCompo>(anyc);
			if (!cam) continue;

			CompSpec c; c.name = "CameraComp";
			Math::Vector3 p = cam->GetLookPos();  
			c.props["pos"] = { p.x, p.y, p.z };
			out.components.push_back(std::move(c));
		}
	}

	{
		for (const auto& anyc : obj->GetComponents()) {
			auto col = std::dynamic_pointer_cast<ColiderComp>(anyc);
			if (!col) continue;

			// Exposeからjson文字列をもらう
			std::vector<Property> props;
			col->ExposeProperties(props);
			nlohmann::json jprops;
			for (auto& p : props) {
				if (p.name == "json") {
					jprops["json"] = ToJson(p.getter());
					break;
				}
			}

			CompSpec c; c.name = "ColliderComponent";
			if (!jprops.empty()) c.props = std::move(jprops);
			out.components.push_back(std::move(c));
		}
	}

	return out;
}

void InspectorDB::ApplyCharacterSpec(const std::shared_ptr<KdGameObject>& obj,
	const CharacterSpec& chSpec,
	const std::string& base) const
{
	if (!obj) return;

	// --- Transform（1個想定）
	if (auto it = std::find_if(chSpec.components.begin(), chSpec.components.end(),
		[](const CompSpec& c) { return c.name == "TransformComponent"; }); it != chSpec.components.end())
	{
		auto t = obj->GetComponent<TransformComponent>();
		if (!t) { t = std::make_shared<TransformComponent>(); obj->AddComponent(t); }
		t->ApplyFromJson(it->props, base);   
	}

	//{
	//	auto& list = obj->GetComponents();
	//	obj->RemoveComponentsOfType<ModelComp>();
	//	for (const auto& c : chSpec.components)
	//	{
	//		if (c.name != "ModelComp") continue;
	//		auto m = std::make_shared<ModelComp>();
	//		m->ApplyFromJson(c.props, base);
	//		obj->AddComponent(m);
	//	}
	//}

	// --- Model（複数置換）
	{
		
		auto& list = obj->WorkComponents();
		list.erase(std::remove_if(list.begin(), list.end(),
			[](const std::shared_ptr<Component>& c) {
				return std::dynamic_pointer_cast<ModelComp>(c) != nullptr;
			}), list.end());

	
		for (const auto& c : chSpec.components)
		{
			if (c.name != "ModelComp") continue;
			auto m = std::make_shared<ModelComp>();
			m->ApplyFromJson(c.props, base); 
			obj->AddComponent(m);
		}
	}

	// --- PlayerControl（有無）
	const bool needPC = std::any_of(chSpec.components.begin(), chSpec.components.end(),
		[](const CompSpec& c) { return c.name == "PlayerControl"; });
	if (needPC && !obj->GetComponent<PlayerControl>())
		obj->AddComponent(std::make_shared<PlayerControl>());

	auto& list = obj->WorkComponents();
	list.erase(std::remove_if(list.begin(), list.end(),
		[](const std::shared_ptr<Component>& c) {
			return std::dynamic_pointer_cast<CameraCompo>(c) != nullptr;
		}), list.end());
	for (const auto& c : chSpec.components) {
		if (c.name != "CameraComp") continue;

		auto cam = std::make_shared<CameraCompo>();
		cam->ApplyFromJson(c.props, base);
		obj->AddComponent(cam);
	}


	
	//auto& list = obj->WorkComponents();
	list.erase(std::remove_if(list.begin(), list.end(),
		[](const std::shared_ptr<Component>& c) {
			return std::dynamic_pointer_cast<ColiderComp>(c) != nullptr;
		}), list.end());

	for (const auto& c : chSpec.components) {
		if (c.name != "ColliderComponent") continue;
		auto col = std::make_shared<ColiderComp>();
		col->ApplyFromJson(c.props, base);   // "json" を読む実装でOK
		obj->AddComponent(col);
	}
	

}
