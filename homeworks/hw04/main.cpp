#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <cmath>

#include "shader_utils.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1280;
    int height = 720;

    GLFWwindow* window = glfwCreateWindow(width, height, "Tracking Zero", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    std::string vertexShaderName = "res/shaders/rect.vert";
    std::string fragmentShaderName = "res/shaders/rect.frag";
    GLuint shaderProgram = createProgram(vertexShaderName, fragmentShaderName);

    float vertices[] = {
        -0.5f,  0.5f, 0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 1.0f,
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    GLuint VBO, indexBuffer, VAO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &indexBuffer);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const int STRIDE = 5 * sizeof(float);
    GLuint posAttribLocation = glGetAttribLocation(shaderProgram, "aPos");
    glVertexAttribPointer(posAttribLocation, 3, GL_FLOAT, GL_FALSE, STRIDE, (void*)0);
    glEnableVertexAttribArray(posAttribLocation);

    GLuint textureCoordsAttribLocation = glGetAttribLocation(shaderProgram, "aUV");
    glVertexAttribPointer(textureCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, STRIDE, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(textureCoordsAttribLocation);
    glBindVertexArray(0);

    GLint transform_loc = glGetUniformLocation(shaderProgram, "uTransformation");
    GLint texture_loc = glGetUniformLocation(shaderProgram, "uTexture0");
    GLint chroma_loc = glGetUniformLocation(shaderProgram, "uUseChromaKey");

    unsigned int textureZero = loadTexture("res/textures/0.jpeg");

    float posX = 0.0f;
    float posY = 0.0f;
    float moveSpeed = 1.5f;
    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    posY += moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  posY -= moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  posX -= moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) posX += moveSpeed * deltaTime;

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glUniform1i(chroma_loc, 1);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float aspect = (float)width / (float)height;
        float ndcX = (2.0f * mouseX) / width - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY) / height;

        // Map mouse coordinates to the new orthographic space
        float worldMouseX = ndcX * aspect;
        float worldMouseY = ndcY;

        float dirX = worldMouseX - posX;
        float dirY = worldMouseY - posY;
        float angle = atan2(dirY, dirX) - glm::radians(90.0f);

        // 1. Create a proper 2D Orthographic Projection matrix
        glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

        // 2. Create the Model matrix
        glm::mat4 modelZero = glm::mat4(1.0f);
        modelZero = glm::translate(modelZero, glm::vec3(posX, posY, 0.0f));
        modelZero = glm::rotate(modelZero, angle, glm::vec3(0.0f, 0.0f, 1.0f));

        // 3. Multiply them together before sending to the shader
        glm::mat4 finalTransform = projection * modelZero;
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(finalTransform));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureZero);
        glUniform1i(texture_loc, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &textureZero);

    glfwTerminate();
    return 0;
}
