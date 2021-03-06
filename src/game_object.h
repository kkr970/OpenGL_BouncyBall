#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "texture.h"
#include "sprite_renderer.h"

enum BlockType{
    EMPTY,
    NORMAL,
    BREAKABLE,
    TRAP,
    BOUNCE,
    LRMOVE,
    UDMOVE,

    START,
    GOAL,
    RIGHTDIR,
    LEFTDIR
};
// 벽돌 오브젝트 종류 (줄 보면서 찾기 쉽게 아래로 내렸음)
//  0=빈공간,  1=기본,  2=충돌파괴, 3=함정,  4=바운스, 5=좌우움돌, 6=상하움돌, 8=시작지점, 9=도착지점
// 10=우직진, 11=좌직진

class GameObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;
    BlockType   Type;
    bool        Destroyed;
    bool        isDirectional;
    int         Dir;

    // render state
    Texture2D   Sprite;	
    // constructor(s)
    GameObject()
            : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f),
            Color(1.0f), Rotation(0.0f), Sprite(), Type(NORMAL), Destroyed(false), isDirectional(false), Dir(1) { }
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f))
            : Position(pos), Size(size), Velocity(velocity),
            Color(color), Rotation(0.0f), Sprite(sprite), Type(NORMAL), Destroyed(false), isDirectional(false), Dir(1) { }
    // draw sprite
    virtual void Draw(SpriteRenderer &renderer)
    {
        renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
    }
};

#endif
