#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include "shader_utils.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1280;
    int height = 720;

    window = glfwCreateWindow(width, height, "Animated Square", nullptr, nullptr);
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
    
    // Enable VSync to cap FPS at monitor refresh rate (typically 60 FPS)
    glfwSwapInterval(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::string vertexShaderName = "res/shaders/rect.vert";
    std::string fragmentShaderName = "res/shaders/rect.frag";
    GLuint shaderProgram = createProgram(vertexShaderName, fragmentShaderName);

    // Vertex format: position(3 floats) + texCoords(2 floats) = 5 floats
    // Centered at (0,0) for proper rotation around the intersection of diagonals
    float vertices[] = {
        -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, // Top Left
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, // Bottom Left
         0.5f, -0.5f, 0.0f,   1.0f, 1.0f, // Bottom Right
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f  // Top Right
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

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

    unsigned int squareTexture = loadTexture("res/textures/0.jpeg");

    float angle = 0.0f;
    float lastFrameTime = glfwGetTime();
    bool isPaused = false;
    bool prevSpacePressed = false;

    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        bool currentSpacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (currentSpacePressed && !prevSpacePressed) {
            isPaused = !isPaused;
        }
        prevSpacePressed = currentSpacePressed;

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        if (!isPaused) {
            angle += glm::radians(90.0f) * deltaTime;
        }

        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, squareTexture);
        glUniform1i(texture_loc, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &squareTexture);

    glfwTerminate();
    return 0;
}