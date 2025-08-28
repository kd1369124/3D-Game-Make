#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <filesystem>
#include <fstream>

using ObjId = KdGameObject::ObjId;

class InspectorDB {
public:
	static InspectorDB& Instance() { static InspectorDB i; return i; }

	void Register(const std::shared_ptr<KdGameObject>& obj) { if (obj) m_objs[obj->GetId()] = obj; }
	void Unregister(ObjId id) { m_objs.erase(id); }
	void Clear() { m_objs.clear(); }

	std::vector<std::string> ListComponents(ObjId id) const;
	std::vector<Property>    GetProperties(ObjId id, const std::string& compName) const;

	std::optional<PropValue> Get(ObjId id, const std::string& comp, const std::string& prop) const;
	bool                     Set(ObjId id, const std::string& comp, const std::string& prop, const PropValue& v) const;

	nlohmann::json Dump(ObjId id) const;
	bool           Apply(ObjId id, const nlohmann::json& j) const;

	bool SaveToFile(ObjId id, const std::string& path) const;
	bool LoadFromFile(ObjId id, const std::string& path) const;

	std::unordered_map<ObjId, std::weak_ptr<KdGameObject>> m_objs;

	static nlohmann::json ToJson(const PropValue& v);
	static bool FromJson(const nlohmann::json& j, PropValue& out, const PropValue& hint);

	CharacterSpec BuildCharacterSpec(const std::shared_ptr<KdGameObject>& obj) const;
	void ApplyCharacterSpec(const std::shared_ptr<KdGameObject>& obj,
		const CharacterSpec& chSpec, const std::string& base) const;


private:

	InspectorDB() = default;
};