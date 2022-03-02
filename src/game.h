#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include "shader.h"
#include "texture.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"

//게임 state
enum GameState{
    GAME_ACTIVE, // 게임중
    GAME_MENU,   // 게임 매뉴
    GAME_WIN     // 게임 승리
};

class Game
{
private:
    SpriteRenderer *Renderer;

public:
    GameState State;
    unsigned int Width, Height;
    bool Keys[1024];

    std::vector<GameLevel> Levels;
    unsigned int Level;
    unsigned int maxLevel = 2;

    // 생성자 파괴자
    Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
    {

    }
    ~Game()
    {
        delete Renderer;
    }

    // 게임 초기설정, 초기화
    void Init()
    {
        // 쉐이더 로드
        ResourceManager::LoadShader("src/shader/sprite.vs", "src/shader/sprite.fs", nullptr, "sprite");
        // 쉐이더 데이터 전달
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
            static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
        ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
        ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
        // private 변수에 쉐이더 전달
        Shader spriteshader = ResourceManager::GetShader("sprite");
        Renderer = new SpriteRenderer(spriteshader);
        // texture 불러오기
        ResourceManager::LoadTexture("resources/textures/ball.png", true, "ball");
        ResourceManager::LoadTexture("resources/textures/block_normal.png", true, "block_normal");
        ResourceManager::LoadTexture("resources/textures/block_breakable.png", true, "block_breakable");
        ResourceManager::LoadTexture("resources/textures/block_goal.png", true, "block_goal");
        ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
        // 레벨 로드
        for(int i = 1 ; i <= maxLevel ; i ++)
        {
            GameLevel gamelevel;
            std::string path = "resources/gamelevels/"+std::to_string(i)+".txt";
            gamelevel.Load(path.c_str(), this->Width, this->Height);
            this->Levels.push_back(gamelevel);
        }
        this->Level = 1;
    }

    // 키보드 입력
    void ProcessInput(float dt)
    {

    }
    
    // 레벨 리셋
    void ResetLevel()
    {
        std::string path = "resources/gamelevels/"+std::to_string(++this->Level)+".txt";
        this->Levels[this->Level].Load(path.c_str(), this->Width, this->Height);
    }

    // 게임 업데이트
    void Update(float dt)
    {

    }
    // 게임화면 렌더링
    void Render()
    {   
        // draw background
        Texture2D background = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(background, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
    }

};

#endif