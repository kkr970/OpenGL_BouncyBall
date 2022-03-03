#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>

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
//방향
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

//플레이어 공 설정
const float PLAYER_X_SPEED_MAX(200.0f);
const float PLAYER_Y_SPEED_MAX(400.0f);
const float PLAYER_RADIUS(7.0f);
float PLAYER_SPEED_X;
float PLAYER_SPEED_Y;
float PLAYER_ACC_X;
float PLAYER_ACC_Y;

//충돌데이터 튜플
typedef std::tuple<bool, Direction, glm::vec2> Collision;
//충돌 방향 벡터 구하기
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
} 
//충돌 함수
Collision CheckCollision(GameObject &one, GameObject &two) // AABB-Circle
{
    // get center point circle first 
    glm::vec2 center(one.Position + PLAYER_RADIUS);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x, 
        two.Position.y + aabb_half_extents.y
    );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    if (glm::length(difference) < PLAYER_RADIUS)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

class Game
{
private:
    SpriteRenderer *Renderer;
    GameObject *Player;

public:
    GameState State;
    unsigned int Width, Height;
    bool Keys[1024];

    std::vector<GameLevel> Levels;
    unsigned int Level;
    unsigned int maxLevel = 4;

    // 생성자 파괴자
    Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height)
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
        this->Level = 0;
        // 플레이어
        PLAYER_SPEED_X = 0.0f;
        PLAYER_SPEED_Y = 0.0f;
        PLAYER_ACC_X = 2400.0f;
        PLAYER_ACC_Y = 1000.0f;
    }

    // 키보드 입력
    void ProcessInput(float dt)
    {
        if (this->State == GAME_ACTIVE)
        {
            float maxX = (PLAYER_X_SPEED_MAX);
            float acc = (PLAYER_ACC_X * dt);
            // move ball.x
            if (this->Keys[GLFW_KEY_A])
            {
                if (Player->Position.x >= 0.0f)
                {
                    if(PLAYER_SPEED_X > -maxX) // 현재 속도가 최고 속도보다 낮을때 
                    {
                        PLAYER_SPEED_X -= acc;
                    }
                    else
                    {
                        PLAYER_SPEED_X = -maxX; // 속도가 최고 속도보다 높을경우 최고 속도로 고정
                    }
                    this->Player->Position.x += (PLAYER_SPEED_X * dt);
                }
            }
            else if (this->Keys[GLFW_KEY_D])
            {
                if (Player->Position.x <= this->Width - Player->Size.x)
                {
                    if(PLAYER_SPEED_X < maxX) // 현재 속도가 최고 속도보다 낮을때 
                    {
                        PLAYER_SPEED_X += acc;
                    }
                    else
                    {
                        PLAYER_SPEED_X = maxX; // 속도가 최고 속도보다 높을경우 최고 속도로 고정
                    }
                    this->Player->Position.x += (PLAYER_SPEED_X * dt);
                }
            }

            // 관성, 가속도가 남아있는데 점점 줄어드는 것
            if(!this->Keys[GLFW_KEY_A] && !this->Keys[GLFW_KEY_D])
            {
                if (PLAYER_SPEED_X > acc/2.0f)
                {
                    PLAYER_SPEED_X -= (acc/3.0f);
                    this->Player->Position.x += (PLAYER_SPEED_X * dt);
                }
                else if(PLAYER_SPEED_X < -acc/2.0f)
                {
                    PLAYER_SPEED_X += (acc/3.0f);
                    this->Player->Position.x += (PLAYER_SPEED_X * dt);
                }
                else
                {
                    PLAYER_SPEED_X = 0;
                }
            }
        }
        
        if (this->State == GAME_MENU)
        {
            if (this->Keys[GLFW_KEY_SPACE])
            {
                this->State = GAME_ACTIVE;
            }
        }
    }

    // 레벨 리셋
    void ResetLevel()
    {
        std::string path = "resources/gamelevels/"+std::to_string(this->Level+1)+".txt";
        this->Levels[this->Level].Load(path.c_str(), this->Width, this->Height);
        Player->Destroyed = false;
    }

    // 충돌 처리함수
    void DoCollisions(float dt)
    {
        for (GameObject &box : this->Levels[this->Level].Blocks)
        {
            if (!box.Destroyed)
            {
                Collision collision = CheckCollision(*Player, box);
                // 공-블록 충돌
                if (std::get<0>(collision))
                {
                    Direction dir = std::get<1>(collision);
                    glm::vec2 diff_vector = std::get<2>(collision);
                    if(box.Type == GOAL)
                    {
                        Level = (Level + 1) % maxLevel;
                        ResetLevel();
                    }
                    else if(box.Type == TRAP)
                    {
                        Player->Destroyed = true;
                    }
                    else
                    {
                        // UP
                        if(dir == UP)
                        {   
                            float penetration = PLAYER_RADIUS - std::abs(diff_vector.y);
                            Player->Position.y -= penetration;
                            //부서지는 불록
                            if (box.Type == BREAKABLE)
                                box.Destroyed = true;
                            //일반 블록
                            if (box.Type == NORMAL || box.Type == BREAKABLE)
                                PLAYER_SPEED_Y = -330.0f;
                        }
                        // DOWN
                        if(dir == DOWN)
                        {      
                            float penetration = PLAYER_RADIUS - std::abs(diff_vector.y);
                            Player->Position.y += penetration;
                            //부서지는 불록
                            if (box.Type == BREAKABLE)
                                box.Destroyed = true;
                            //일반 블록
                            if (box.Type == NORMAL || box.Type == BREAKABLE)
                                PLAYER_SPEED_Y = -PLAYER_SPEED_Y;
                        }
                        // LEFT, RIGHT
                        if(dir == LEFT || dir == RIGHT)
                        {   
                            float penetration = PLAYER_RADIUS - std::abs(diff_vector.x);
                            if(dir == LEFT) Player->Position.x += penetration;
                            else Player->Position.x -= penetration;
                            //부서지는 불록
                            if (box.Type == BREAKABLE)
                            {
                                box.Destroyed = true;
                            }
                            if (box.Type == NORMAL || box.Type == BREAKABLE)
                            {   
                                PLAYER_SPEED_X = -(PLAYER_SPEED_X);
                            }
                        }
                    }
                }
            }
        }
    }
    // 공 낙하 가속함수
    void BallAccelation(float dt)
    {
        float maxY = (PLAYER_Y_SPEED_MAX);
        float acc = (PLAYER_ACC_Y * dt);
        if(PLAYER_SPEED_Y < maxY) // 현재 속도가 최고 속도보다 낮을때 
        {
            PLAYER_SPEED_Y += acc;
        }
        else
        {
            PLAYER_SPEED_Y = maxY; // 속도가 최고 속도보다 높을경우 최고 속도로 고정
        }
        //공이 아래로 계속 움직임
        this->Player->Position.y += (PLAYER_SPEED_Y * dt);
    }

    // 게임 업데이트
    void Update(float dt)
    {
        if(State == GAME_ACTIVE)
        {
            this->BallAccelation(dt);
            this->DoCollisions(dt);
            //스테이지 실패
            if(Player->Position.y >= this->Height || Player->Destroyed)
            {
                ResetLevel();
            }
        }
    }
    // 게임화면 렌더링
    void Render()
    {   
        // draw background
        Texture2D background = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(background, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        Player = this->Levels[this->Level].Ball;
        Player->Draw(*Renderer);
    }

    float getXSpeed(){
        return PLAYER_SPEED_X;
    }

};

#endif