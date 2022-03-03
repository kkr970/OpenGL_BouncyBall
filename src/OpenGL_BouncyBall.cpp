#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "game.h"

// 함수 선언
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// 스크린 화면 크기
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

Game BouncyBall(SCREEN_WIDTH, SCREEN_HEIGHT);

// 메인 함수 ---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL_BouncyBall // kkr970 (Shin-Icksu)", nullptr, nullptr);
    glfwMakeContextCurrent(window); 

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //Multi Sampling 사용
    glEnable(GL_MULTISAMPLE);

    //타이밍 관련
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    //게임 초기화
    BouncyBall.Init();

    //직교 투영 행렬 projection
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    //렌더링 함수
    while (!glfwWindowShouldClose(window))
    {
        //delta 시간
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        //게임 state update
        BouncyBall.Update(deltaTime);

        //입력
        BouncyBall.ProcessInput(deltaTime);

        //렌더링
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        BouncyBall.Render();

        //std::cout << BouncyBall.getXSpeed() << std::endl;

        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();

    glfwTerminate(); 
    return 0;
}

void key_callback(GLFWwindow* window, int key, int  scancode, int action, int mode)
{
    // ESC 종료 버튼
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // 나머지 key 입력 확인
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            BouncyBall.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            BouncyBall.Keys[key] = false;
    } 
    
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
