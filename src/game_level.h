#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"


class GameLevel
{
public:
    // player
    GameObject *Ball;
    float radius = 7.0f;
    // level state
    std::vector<GameObject> Blocks;
    // constructor
    GameLevel() { }
    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
    {
        // clear old data
        this->Blocks.clear();
        // load from file
        unsigned int tileCode;
        GameLevel level;
        std::string line;
        std::ifstream fstream(file);
        std::vector<std::vector<unsigned int>> tileData;
        if (fstream)
        {
            while (std::getline(fstream, line)) // read each line from level file
            {
                std::istringstream sstream(line);
                std::vector<unsigned int> row;
                while (sstream >> tileCode) // read each word separated by spaces
                    row.push_back(tileCode);
                tileData.push_back(row);
            }
            if (tileData.size() > 0)
                this->init(tileData, levelWidth, levelHeight);
        }
    }
    // render level
    void Draw(SpriteRenderer &renderer)
    {
        for (GameObject &tile : this->Blocks)
            if (!tile.Destroyed)
                tile.Draw(renderer);
    }
    // check if the level is completed (GOAL block이 파괴되면 클리어)
    bool IsCompleted()
    {
        for (GameObject &tile : this->Blocks)
            if (tile.Type == GOAL && !tile.Destroyed)
                return false;
        return true;
    }
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
    {
    // calculate dimensions
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size(); // note we can index vector at [0] since this function is only called if height > 0
    float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height; 
        // initialize level tiles based on tileData		
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                // check block type from level data (2D level array)
                // 0=빈공간, 1=기본, 2=충돌 시 파괴, 3=함정, 4=바운스, 5=좌우움돌, 6=상하움돌 8=시작지점, 9=도착지점
                if (tileData[y][x] == 1) // NORMAL
                {
                    glm::vec2 pos(unit_width * x, unit_height * y);
                    glm::vec2 size(unit_width, unit_height);
                    GameObject obj(pos, size, ResourceManager::GetTexture("block_normal"), glm::vec3(1.0f));
                    obj.Type = NORMAL;
                    this->Blocks.push_back(obj);
                }
                else if (tileData[y][x] == 2)	// BREAKABLE
                {
                    glm::vec2 pos(unit_width * x, unit_height * y);
                    glm::vec2 size(unit_width, unit_height);
                    GameObject obj(pos, size, ResourceManager::GetTexture("block_breakable"), glm::vec3(1.0f));
                    obj.Type = BREAKABLE;
                    this->Blocks.push_back(obj);
                }
                else if (tileData[y][x] == 3)	// TRAP
                {
                    glm::vec2 pos(unit_width * x, unit_height * y);
                    glm::vec2 size(unit_width, unit_height);
                    GameObject obj(pos, size, ResourceManager::GetTexture("block_normal"), glm::vec3(0.8f, 0.3f, 0.3f));
                    obj.Type = TRAP;
                    this->Blocks.push_back(obj);
                }
                else if (tileData[y][x] == 4)	// BOUNCE
                {
                    glm::vec2 pos(unit_width * x, unit_height * y);
                    glm::vec2 size(unit_width, unit_height);
                    GameObject obj(pos, size, ResourceManager::GetTexture("block_normal"), glm::vec3(0.8f, 0.3f, 0.8f));
                    obj.Type = BOUNCE;
                    this->Blocks.push_back(obj);
                }
                else if (tileData[y][x] == 5)	// LRMOVE
                {
                    glm::vec2 pos(unit_width * x + 0.1f, unit_height * y + 0.1f);
                    glm::vec2 size(unit_width - 0.2f, unit_height - 0.2f);
                    GameObject obj(pos, size, ResourceManager::GetTexture("block_lrmove"), glm::vec3(1.0f));
                    obj.Type = LRMOVE;
                    this->Blocks.push_back(obj);
                }
                else if (tileData[y][x] == 6)	// UDMOVE
                {
                    glm::vec2 pos(unit_width * x + 0.1f, unit_height * y + 0.1f);
                    glm::vec2 size(unit_width - 0.2f, unit_height - 0.2f);
                    GameObject obj(pos, size, ResourceManager::GetTexture("block_udmove"), glm::vec3(1.0f));
                    obj.Type = UDMOVE;
                    this->Blocks.push_back(obj);
                }
                else if (tileData[y][x] == 8)	// START
                {
                    glm::vec2 pos(unit_width * x + 14.0f, unit_height * y + 14.0f);
                    glm::vec2 size(radius * 2.0f, radius * 2.0f);
                    Ball = new GameObject(pos, size, ResourceManager::GetTexture("ball"), glm::vec3(1.0f));
                }
                else if (tileData[y][x] == 9)	// GOAL
                {
                    glm::vec2 pos(unit_width * x, unit_height * y);
                    glm::vec2 size(unit_width, unit_height);
                    GameObject obj(pos, size, ResourceManager::GetTexture("block_goal"), glm::vec3(1.0f));
                    obj.Type = GOAL;
                    this->Blocks.push_back(obj);
                }
            }
        }
    }
};

#endif