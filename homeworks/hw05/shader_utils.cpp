//
// Created by Andrii Krenevych on 09.04.2026.
//

#include "shader_utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "glad/glad.h"

std::string LoadShaderFromFile(const std::string &filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Не вдалося відкрити файл: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << shaderFile.rdbuf(); // зчитує весь вміст файлу в потік

    return buffer.str(); // повертає як std::string
}

GLuint createShader(std::string &filePath, GLuint shaderType) {
    auto vertexShaderCode_str = LoadShaderFromFile(filePath);
    auto vertexShaderCode= vertexShaderCode_str.c_str();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &vertexShaderCode, nullptr);
    glCompileShader(shader);

    // Перевірка компіляції шейдера
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Помилка компіляції шейдера:\n" << filePath << "\n" <<infoLog << std::endl;
    }


    return shader;
}

GLuint createProgram(
    std::string &vertexShaderName,
    std::string &fragmentShaderName) {

    unsigned int vertexShader = createShader(vertexShaderName, GL_VERTEX_SHADER);
    GLuint fragmentShader = createShader(fragmentShaderName, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Перевірка лінкування шейдерної програми
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Помилка лінкування shader program:\n" << infoLog << std::endl;
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
