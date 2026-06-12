#include "anchorpoint/assets/mesh.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "anchorpoint/core/log.h"

namespace ap {

PrimitiveTopology ParseTopology(const std::string& str) {
    if (str == "LINELIST") return PrimitiveTopology::LineList;
    if (str == "POINTLIST") return PrimitiveTopology::PointList;
    return PrimitiveTopology::TriangleList;
}

namespace {

bool LoadMTRL(const std::string& mtlName, MaterialData& out) {
    std::ifstream in("assets/materials/" + mtlName + ".mtrl");
    if (!in)
        return false;

    nlohmann::json mats;
    in >> mats;

    out.diffuseColor = {mats["diffuseColor"][0], mats["diffuseColor"][1], mats["diffuseColor"][2]};
    out.vertexShader = mats["shader"]["vertex"].get<std::string>();
    out.pixelShader = mats["shader"]["pixel"].get<std::string>();
    out.texture = mats["texture"].get<std::string>();
    return true;
}

std::unordered_map<std::string, MaterialData>& MaterialCache() {
    static std::unordered_map<std::string, MaterialData> cache;
    return cache;
}

} // namespace

bool LoadOBJ(const std::string& filename, MeshData& out) {
    std::ifstream file(filename);
    if (!file) {
        LogError(("Failed to open model file: " + filename).c_str());
        return false;
    }

    std::vector<Vector3> tempPosition;
    std::vector<Vector2> tempUv;
    std::vector<Vector3> tempNormal;
    std::string line, triplet, token;
    float x, y, z;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type, mtlName;
        iss >> type;

        if (type == "v") {
            iss >> x >> y >> z;
            tempPosition.push_back({x, y, z});
        } else if (type == "vt") {
            iss >> x >> y;
            tempUv.push_back({x, y});
        } else if (type == "vn") {
            iss >> x >> y >> z;
            tempNormal.push_back({x, y, z});
        } else if (type == "f") {
            std::vector<unsigned> fIndices;
            std::vector<unsigned> vtIndices;
            std::vector<unsigned> vnIndices;
            while (iss >> triplet) {
                std::istringstream vec(triplet);
                if (std::getline(vec, token, '/') && !token.empty())
                    fIndices.push_back(std::stoi(token));
                if (std::getline(vec, token, '/') && !token.empty())
                    vtIndices.push_back(std::stoi(token));
                if (std::getline(vec, token, '/') && !token.empty())
                    vnIndices.push_back(std::stoi(token));
            }

            auto makeVertex = [&](std::size_t i) -> Vertex {
                Vertex vertex;
                vertex.position = tempPosition[fIndices[i] - 1];
                if (fIndices.size() == vnIndices.size())
                    vertex.normal = tempNormal[vnIndices[i] - 1];
                if (fIndices.size() == vtIndices.size()) {
                    vertex.u = tempUv[vtIndices[i] - 1].x;
                    vertex.v = tempUv[vtIndices[i] - 1].y;
                }
                return vertex;
            };

            for (std::size_t i = 1; i + 1 < fIndices.size(); ++i) {
                out.vertices.push_back(makeVertex(0));
                out.vertices.push_back(makeVertex(i));
                out.vertices.push_back(makeVertex(i + 1));
            }
        } else if (type == "usemtl") {
            iss >> mtlName;
            auto& cache = MaterialCache();
            auto it = cache.find(mtlName);
            if (it == cache.end()) {
                MaterialData material;
                if (!LoadMTRL(mtlName, material))
                    LogError(("Failed to load material: " + mtlName).c_str());
                it = cache.emplace(mtlName, material).first;
            }
            out.material = it->second;
        }
    }

    return !out.vertices.empty();
}

} // namespace ap
