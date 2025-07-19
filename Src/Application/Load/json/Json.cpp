#include"Json.h"
#include <fstream>

std::vector<NodeData> NodeManager::LoadNodes(const std::string& filepath)
{
	std::vector<NodeData> nodes;
	std::ifstream file(filepath);
	if (!file.is_open()) return nodes;

	nlohmann::json json;
	file >> json;

	for (const auto& j : json["nodes"]) {
		NodeData node = j.get<NodeData>();
		nodes.push_back(node);
	}

	return nodes;
}

void NodeManager::SaveNodes(const std::vector<NodeData>& nodes, const std::string& filepath)
{
	nlohmann::json json;
	json["nodes"] = nodes;

	std::ofstream file(filepath);
	if (file.is_open()) {
		file << json.dump(4);
		file.close();
	}
}
