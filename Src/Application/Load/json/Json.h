#pragma once
#include"../../../../../Library/tinygltf/json.hpp"
struct NodeData {
	std::string name;
	std::string type;
	Math::Vector3 position;
	Math::Vector3 rotation;
	Math::Vector3 scale;

};

// JSON変換
inline void to_json(nlohmann::json& j, const NodeData& node)
{
	j = nlohmann::json{
		{"name", node.name},
		{"type", node.type},
		{"position", { node.position.x, node.position.y, node.position.z }},
		{"rotation", { node.rotation.x, node.rotation.y, node.rotation.z }},
		{"scale", { node.scale.x, node.scale.y, node.scale.z }}
	};
}

inline void from_json(const nlohmann::json& j, NodeData& node)
{
	node.name = j.at("name").get<std::string>();
	node.type = j.value("type", "Unknown");
	node.position = Math::Vector3(j["position"][0], j["position"][1], j["position"][2]);
	node.rotation = Math::Vector3(j["rotation"][0], j["rotation"][1], j["rotation"][2]);
	node.scale = Math::Vector3(j["scale"][0], j["scale"][1], j["scale"][2]);
}

class NodeManager
{
public:
	static std::vector<NodeData> LoadNodes(const std::string& filepath);
	static void SaveNodes(const std::vector<NodeData>& nodes, const std::string& filepath);
};
