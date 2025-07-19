#include "JsonLoad.h"

std::vector<NodeData> JsonNodeLoader::LoadNodeS(const std::string& filepath)
{
	std::vector<NodeData> nodes;
	std::ifstream file(filepath);
	if (!file.is_open()) return nodes;

	nlohmann::json json;
	file >> json;

	for (const auto& node : json["nodes"]) 
	{
		NodeData data;
		data.name = node["name"];
		// ここで type を追加読み取り
		if (node.contains("type") && node["type"].is_string()) {
			data.type = node["type"].get<std::string>();
		}
		else {
			data.type = "Unknown";
		}
		data.pos = Math::Vector3(
			node["position"][0], node["position"][1], node["position"][2]);
		data.rotation = Math::Vector3(
			node["rotation"][0], node["rotation"][1], node["rotation"][2]);
		data.scale = Math::Vector3(
			node["scale"][0], node["scale"][1], node["scale"][2]);
		data.params = node.value("params", nlohmann::json{});

		nodes.push_back(data);
	}

	return nodes;
}
