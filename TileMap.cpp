#include "TileMap.h"
#include <fstream>
#include <SDL3_image/SDL_image.h>
#include <iostream>

using json = nlohmann::json;

TileMap::TileMap(SDL_Renderer* rend) : renderer(rend) {}

TileMap::~TileMap() {
    for (auto& ts : tilesets)
        SDL_DestroyTexture(ts.texture);
}

bool TileMap::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Ошибка загрузки карты: " << path << std::endl;
        return false;
    }

    std::cout << "Файл карты загружен: " << path << std::endl;
    json j;
    file >> j;

    tileWidth = j["tilewidth"];
    tileHeight = j["tileheight"];
    mapWidth = j["width"];
    mapHeight = j["height"];

    for (const auto& layer : j["layers"]) {
        if (layer["type"] == "tilelayer") {
            layers.push_back({ layer["name"], layer["data"].get<std::vector<int>>() });
        }
    }

    loadCollisions(j["layers"]);
    std::string folder = path.substr(0, path.find_last_of("/\\") + 1);
    loadTilesets(folder, j["tilesets"]);

    return true;
}

void TileMap::renderLayer(SDL_Renderer* renderer, const SDL_FRect& camera, const std::string& name) {
    SDL_FRect dest = { 0, 0, 32, 32 };

    for (const auto& layer : layers) {
        if (layer.name != name) continue;

        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                int tileID = layer.data[y * mapWidth + x];
                if (tileID == 0) continue;

                const Tileset* ts = nullptr;
                for (const auto& tileset : tilesets) {
                    if (tileID >= tileset.firstgid) ts = &tileset;
                }
                if (!ts || !ts->texture) continue;

                SDL_FRect src = {
                    float((tileID - ts->firstgid) % ts->columns * 32),
                    float((tileID - ts->firstgid) / ts->columns * 32),
                    32, 32
                };
                dest.x = x * 32.0f - camera.x;
                dest.y = y * 32.0f - camera.y;
                SDL_RenderTexture(renderer, ts->texture, &src, &dest);
            }
        }
    }
}

void TileMap::loadTilesets(const std::string& folder, const json& tilesetsJson) {
    for (const auto& entry : tilesetsJson) {
        std::string tsxPath = folder + entry["source"].get<std::string>();
        std::ifstream tsx(tsxPath);
        if (!tsx.is_open()) {
            std::cerr << "Ошибка загрузки tileset: " << tsxPath << std::endl;
            continue;
        }

        std::string xml((std::istreambuf_iterator<char>(tsx)), {});
        std::size_t imgPos = xml.find("image source=\"");
        std::size_t colPos = xml.find("columns=\"");

        if (imgPos == std::string::npos || colPos == std::string::npos) continue;

        std::string imgPath = xml.substr(imgPos + 14);
        imgPath = imgPath.substr(0, imgPath.find("\""));

        int columns = std::stoi(xml.substr(colPos + 9, xml.find("\"", colPos + 9) - (colPos + 9)));

        std::string fullImgPath = folder + imgPath;

        Tileset ts = {
            entry["firstgid"],
            columns,
            32, 32,
            IMG_LoadTexture(renderer, fullImgPath.c_str())
        };

        if (!ts.texture) {
            std::cerr << "Ошибка загрузки текстуры: " << fullImgPath << std::endl;
            continue;
        }

        tilesets.push_back(ts);
    }
}

void TileMap::loadCollisions(const json& layersJson) {
    for (const auto& layer : layersJson) {
        if (layer["type"] != "objectgroup" || layer["name"] != "Collisions") continue;

        for (const auto& obj : layer["objects"]) {
            if (obj.contains("name") && obj["name"] == "Spawn") {
                spawnPoint.x = obj["x"].get<float>();
                spawnPoint.y = obj["y"].get<float>();
            }
            else {
                SDL_FRect rect = { obj["x"], obj["y"], obj["width"], obj["height"] };
                collisionRects.push_back(rect);
            }
        }
    }
}

const std::vector<SDL_FRect>& TileMap::getCollisionRects() const {
    return collisionRects;
}

SDL_FPoint TileMap::getSpawnPoint() const {
    return spawnPoint;
}
