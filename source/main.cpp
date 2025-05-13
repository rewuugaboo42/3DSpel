/*
* File: main.cpp
* Author: Simon Olesen
* Date: 2025-05-13
* Description: The program entry point for a 3D graphics engine
*/

#include "camera/camera.h"
#include "shader/shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stbi/stb_image.h"

#include <iostream>

constexpr int SCREEN_WIDTH{ 1600 };
constexpr int SCREEN_HEIGHT{ 960 };

float deltaTime{ 0.0f };
float lastFrame{ 0.0f };

Camera camera{ glm::vec3(0.0f, 2.0f, 0.0f) };
float lastX{ SCREEN_WIDTH / 2.0f };
float lastY{ SCREEN_HEIGHT / 2.0f };
bool firstMouse{ true };

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadTexture(const char* path);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window{ glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Freakmon", NULL, NULL) };
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader lightingShader{ "source/shader/lighting.vs", "source/shader/lighting.fs" };
	Shader skyboxShader{ "source/shader/skybox.vs", "source/shader/skybox.fs" };
	Shader lightCubeShader{ "source/shader/light_cube.vs", "source/shader/light_cube.fs" };

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f, 0.0f),
		glm::vec3(1.0f,  0.0f, 0.0f),
		glm::vec3(2.0f,  0.0f, 0.0f),
		glm::vec3(3.0f,  0.0f, 0.0f),
		glm::vec3(4.0f,  0.0f, 0.0f),
		glm::vec3(5.0f,  0.0f, 0.0f),
		glm::vec3(6.0f,  0.0f, 0.0f),
		glm::vec3(7.0f,  0.0f, 0.0f),
		glm::vec3(-1.0f,  0.0f, 0.0f),
		glm::vec3(-2.0f,  0.0f, 0.0f),
		glm::vec3(-3.0f,  0.0f, 0.0f),
		glm::vec3(-4.0f,  0.0f, 0.0f),
		glm::vec3(-5.0f,  0.0f, 0.0f),
		glm::vec3(-6.0f,  0.0f, 0.0f),
		glm::vec3(-7.0f,  0.0f, 0.0f),
		glm::vec3(-8.0f,  0.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(1.0f,  0.0f, 1.0f),
		glm::vec3(2.0f,  0.0f, 1.0f),
		glm::vec3(3.0f,  0.0f, 1.0f),
		glm::vec3(4.0f,  0.0f, 1.0f),
		glm::vec3(5.0f,  0.0f, 1.0f),
		glm::vec3(6.0f,  0.0f, 1.0f),
		glm::vec3(7.0f,  0.0f, 1.0f),
		glm::vec3(-1.0f,  0.0f, 1.0f),
		glm::vec3(-2.0f,  0.0f, 1.0f),
		glm::vec3(-3.0f,  0.0f, 1.0f),
		glm::vec3(-4.0f,  0.0f, 1.0f),
		glm::vec3(-5.0f,  0.0f, 1.0f),
		glm::vec3(-6.0f,  0.0f, 1.0f),
		glm::vec3(-7.0f,  0.0f, 1.0f),
		glm::vec3(-8.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  0.0f, 2.0f),
		glm::vec3(1.0f,  0.0f, 2.0f),
		glm::vec3(2.0f,  0.0f, 2.0f),
		glm::vec3(3.0f,  0.0f, 2.0f),
		glm::vec3(4.0f,  0.0f, 2.0f),
		glm::vec3(5.0f,  0.0f, 2.0f),
		glm::vec3(6.0f,  0.0f, 2.0f),
		glm::vec3(7.0f,  0.0f, 2.0f),
		glm::vec3(-1.0f,  0.0f, 2.0f),
		glm::vec3(-2.0f,  0.0f, 2.0f),
		glm::vec3(-3.0f,  0.0f, 2.0f),
		glm::vec3(-4.0f,  0.0f, 2.0f),
		glm::vec3(-5.0f,  0.0f, 2.0f),
		glm::vec3(-6.0f,  0.0f, 2.0f),
		glm::vec3(-7.0f,  0.0f, 2.0f),
		glm::vec3(-8.0f,  0.0f, 2.0f),
		glm::vec3(0.0f,  0.0f, 3.0f),
		glm::vec3(1.0f,  0.0f, 3.0f),
		glm::vec3(2.0f,  0.0f, 3.0f),
		glm::vec3(3.0f,  0.0f, 3.0f),
		glm::vec3(4.0f,  0.0f, 3.0f),
		glm::vec3(5.0f,  0.0f, 3.0f),
		glm::vec3(6.0f,  0.0f, 3.0f),
		glm::vec3(7.0f,  0.0f, 3.0f),
		glm::vec3(-1.0f,  0.0f, 3.0f),
		glm::vec3(-2.0f,  0.0f, 3.0f),
		glm::vec3(-3.0f,  0.0f, 3.0f),
		glm::vec3(-4.0f,  0.0f, 3.0f),
		glm::vec3(-5.0f,  0.0f, 3.0f),
		glm::vec3(-6.0f,  0.0f, 3.0f),
		glm::vec3(-7.0f,  0.0f, 3.0f),
		glm::vec3(-8.0f,  0.0f, 3.0f),
		glm::vec3(0.0f,  0.0f, 4.0f),
		glm::vec3(1.0f,  0.0f, 4.0f),
		glm::vec3(2.0f,  0.0f, 4.0f),
		glm::vec3(3.0f,  0.0f, 4.0f),
		glm::vec3(4.0f,  0.0f, 4.0f),
		glm::vec3(5.0f,  0.0f, 4.0f),
		glm::vec3(6.0f,  0.0f, 4.0f),
		glm::vec3(7.0f,  0.0f, 4.0f),
		glm::vec3(-1.0f,  0.0f, 4.0f),
		glm::vec3(-2.0f,  0.0f, 4.0f),
		glm::vec3(-3.0f,  0.0f, 4.0f),
		glm::vec3(-4.0f,  0.0f, 4.0f),
		glm::vec3(-5.0f,  0.0f, 4.0f),
		glm::vec3(-6.0f,  0.0f, 4.0f),
		glm::vec3(-7.0f,  0.0f, 4.0f),
		glm::vec3(-8.0f,  0.0f, 4.0f),
		glm::vec3(0.0f,  0.0f, 5.0f),
		glm::vec3(1.0f,  0.0f, 5.0f),
		glm::vec3(2.0f,  0.0f, 5.0f),
		glm::vec3(3.0f,  0.0f, 5.0f),
		glm::vec3(4.0f,  0.0f, 5.0f),
		glm::vec3(5.0f,  0.0f, 5.0f),
		glm::vec3(6.0f,  0.0f, 5.0f),
		glm::vec3(7.0f,  0.0f, 5.0f),
		glm::vec3(-1.0f,  0.0f, 5.0f),
		glm::vec3(-2.0f,  0.0f, 5.0f),
		glm::vec3(-3.0f,  0.0f, 5.0f),
		glm::vec3(-4.0f,  0.0f, 5.0f),
		glm::vec3(-5.0f,  0.0f, 5.0f),
		glm::vec3(-6.0f,  0.0f, 5.0f),
		glm::vec3(-7.0f,  0.0f, 5.0f),
		glm::vec3(-8.0f,  0.0f, 5.0f),
		glm::vec3(0.0f,  0.0f, 6.0f),
		glm::vec3(1.0f,  0.0f, 6.0f),
		glm::vec3(2.0f,  0.0f, 6.0f),
		glm::vec3(3.0f,  0.0f, 6.0f),
		glm::vec3(4.0f,  0.0f, 6.0f),
		glm::vec3(5.0f,  0.0f, 6.0f),
		glm::vec3(6.0f,  0.0f, 6.0f),
		glm::vec3(7.0f,  0.0f, 6.0f),
		glm::vec3(-1.0f,  0.0f, 6.0f),
		glm::vec3(-2.0f,  0.0f, 6.0f),
		glm::vec3(-3.0f,  0.0f, 6.0f),
		glm::vec3(-4.0f,  0.0f, 6.0f),
		glm::vec3(-5.0f,  0.0f, 6.0f),
		glm::vec3(-6.0f,  0.0f, 6.0f),
		glm::vec3(-7.0f,  0.0f, 6.0f),
		glm::vec3(-8.0f,  0.0f, 6.0f),
		glm::vec3(0.0f, 0.0f, 7.0f),
		glm::vec3(1.0f, 0.0f, 7.0f),
		glm::vec3(2.0f, 0.0f, 7.0f),
		glm::vec3(3.0f, 0.0f, 7.0f),
		glm::vec3(4.0f, 0.0f, 7.0f),
		glm::vec3(5.0f, 0.0f, 7.0f),
		glm::vec3(6.0f, 0.0f, 7.0f),
		glm::vec3(7.0f, 0.0f, 7.0f),
		glm::vec3(-1.0f, 0.0f, 7.0f),
		glm::vec3(-2.0f, 0.0f, 7.0f),
		glm::vec3(-3.0f, 0.0f, 7.0f),
		glm::vec3(-4.0f, 0.0f, 7.0f),
		glm::vec3(-5.0f, 0.0f, 7.0f),
		glm::vec3(-6.0f, 0.0f, 7.0f),
		glm::vec3(-7.0f, 0.0f, 7.0f),
		glm::vec3(-8.0f, 0.0f, 7.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(1.0f, 0.0f, -1.0f),
		glm::vec3(2.0f, 0.0f, -1.0f),
		glm::vec3(3.0f, 0.0f, -1.0f),
		glm::vec3(4.0f, 0.0f, -1.0f),
		glm::vec3(5.0f, 0.0f, -1.0f),
		glm::vec3(6.0f, 0.0f, -1.0f),
		glm::vec3(7.0f, 0.0f, -1.0f),
		glm::vec3(-1.0f, 0.0f, -1.0f),
		glm::vec3(-2.0f, 0.0f, -1.0f),
		glm::vec3(-3.0f, 0.0f, -1.0f),
		glm::vec3(-4.0f, 0.0f, -1.0f),
		glm::vec3(-5.0f, 0.0f, -1.0f),
		glm::vec3(-6.0f, 0.0f, -1.0f),
		glm::vec3(-7.0f, 0.0f, -1.0f),
		glm::vec3(-8.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -2.0f),
		glm::vec3(1.0f, 0.0f, -2.0f),
		glm::vec3(2.0f, 0.0f, -2.0f),
		glm::vec3(3.0f, 0.0f, -2.0f),
		glm::vec3(4.0f, 0.0f, -2.0f),
		glm::vec3(5.0f, 0.0f, -2.0f),
		glm::vec3(6.0f, 0.0f, -2.0f),
		glm::vec3(7.0f, 0.0f, -2.0f),
		glm::vec3(-1.0f, 0.0f, -2.0f),
		glm::vec3(-2.0f, 0.0f, -2.0f),
		glm::vec3(-3.0f, 0.0f, -2.0f),
		glm::vec3(-4.0f, 0.0f, -2.0f),
		glm::vec3(-5.0f, 0.0f, -2.0f),
		glm::vec3(-6.0f, 0.0f, -2.0f),
		glm::vec3(-7.0f, 0.0f, -2.0f),
		glm::vec3(-8.0f, 0.0f, -2.0f),
		glm::vec3(0.0f, 0.0f, -3.0f),
		glm::vec3(1.0f, 0.0f, -3.0f),
		glm::vec3(2.0f, 0.0f, -3.0f),
		glm::vec3(3.0f, 0.0f, -3.0f),
		glm::vec3(4.0f, 0.0f, -3.0f),
		glm::vec3(5.0f, 0.0f, -3.0f),
		glm::vec3(6.0f, 0.0f, -3.0f),
		glm::vec3(7.0f, 0.0f, -3.0f),
		glm::vec3(-1.0f, 0.0f, -3.0f),
		glm::vec3(-2.0f, 0.0f, -3.0f),
		glm::vec3(-3.0f, 0.0f, -3.0f),
		glm::vec3(-4.0f, 0.0f, -3.0f),
		glm::vec3(-5.0f, 0.0f, -3.0f),
		glm::vec3(-6.0f, 0.0f, -3.0f),
		glm::vec3(-7.0f, 0.0f, -3.0f),
		glm::vec3(-8.0f, 0.0f, -3.0f),
		glm::vec3(0.0f, 0.0f, -4.0f),
		glm::vec3(1.0f, 0.0f, -4.0f),
		glm::vec3(2.0f, 0.0f, -4.0f),
		glm::vec3(3.0f, 0.0f, -4.0f),
		glm::vec3(4.0f, 0.0f, -4.0f),
		glm::vec3(5.0f, 0.0f, -4.0f),
		glm::vec3(6.0f, 0.0f, -4.0f),
		glm::vec3(7.0f, 0.0f, -4.0f),
		glm::vec3(-1.0f, 0.0f, -4.0f),
		glm::vec3(-2.0f, 0.0f, -4.0f),
		glm::vec3(-3.0f, 0.0f, -4.0f),
		glm::vec3(-4.0f, 0.0f, -4.0f),
		glm::vec3(-5.0f, 0.0f, -4.0f),
		glm::vec3(-6.0f, 0.0f, -4.0f),
		glm::vec3(-7.0f, 0.0f, -4.0f),
		glm::vec3(-8.0f, 0.0f, -4.0f),
		glm::vec3(0.0f, 0.0f, -5.0f),
		glm::vec3(1.0f, 0.0f, -5.0f),
		glm::vec3(2.0f, 0.0f, -5.0f),
		glm::vec3(3.0f, 0.0f, -5.0f),
		glm::vec3(4.0f, 0.0f, -5.0f),
		glm::vec3(5.0f, 0.0f, -5.0f),
		glm::vec3(6.0f, 0.0f, -5.0f),
		glm::vec3(7.0f, 0.0f, -5.0f),
		glm::vec3(-1.0f, 0.0f, -5.0f),
		glm::vec3(-2.0f, 0.0f, -5.0f),
		glm::vec3(-3.0f, 0.0f, -5.0f),
		glm::vec3(-4.0f, 0.0f, -5.0f),
		glm::vec3(-5.0f, 0.0f, -5.0f),
		glm::vec3(-6.0f, 0.0f, -5.0f),
		glm::vec3(-7.0f, 0.0f, -5.0f),
		glm::vec3(-8.0f, 0.0f, -5.0f),
		glm::vec3(0.0f, 0.0f, -6.0f),
		glm::vec3(1.0f, 0.0f, -6.0f),
		glm::vec3(2.0f, 0.0f, -6.0f),
		glm::vec3(3.0f, 0.0f, -6.0f),
		glm::vec3(4.0f, 0.0f, -6.0f),
		glm::vec3(5.0f, 0.0f, -6.0f),
		glm::vec3(6.0f, 0.0f, -6.0f),
		glm::vec3(7.0f, 0.0f, -6.0f),
		glm::vec3(-1.0f, 0.0f, -6.0f),
		glm::vec3(-2.0f, 0.0f, -6.0f),
		glm::vec3(-3.0f, 0.0f, -6.0f),
		glm::vec3(-4.0f, 0.0f, -6.0f),
		glm::vec3(-5.0f, 0.0f, -6.0f),
		glm::vec3(-6.0f, 0.0f, -6.0f),
		glm::vec3(-7.0f, 0.0f, -6.0f),
		glm::vec3(-8.0f, 0.0f, -6.0f),
		glm::vec3(0.0f, 0.0f, -7.0f),
		glm::vec3(1.0f, 0.0f, -7.0f),
		glm::vec3(2.0f, 0.0f, -7.0f),
		glm::vec3(3.0f, 0.0f, -7.0f),
		glm::vec3(4.0f, 0.0f, -7.0f),
		glm::vec3(5.0f, 0.0f, -7.0f),
		glm::vec3(6.0f, 0.0f, -7.0f),
		glm::vec3(7.0f, 0.0f, -7.0f),
		glm::vec3(-1.0f, 0.0f, -7.0f),
		glm::vec3(-2.0f, 0.0f, -7.0f),
		glm::vec3(-3.0f, 0.0f, -7.0f),
		glm::vec3(-4.0f, 0.0f, -7.0f),
		glm::vec3(-5.0f, 0.0f, -7.0f),
		glm::vec3(-6.0f, 0.0f, -7.0f),
		glm::vec3(-7.0f, 0.0f, -7.0f),
		glm::vec3(-8.0f, 0.0f, -7.0f),
		glm::vec3(0.0f, 0.0f, -8.0f),
		glm::vec3(1.0f, 0.0f, -8.0f),
		glm::vec3(2.0f, 0.0f, -8.0f),
		glm::vec3(3.0f, 0.0f, -8.0f),
		glm::vec3(4.0f, 0.0f, -8.0f),
		glm::vec3(5.0f, 0.0f, -8.0f),
		glm::vec3(6.0f, 0.0f, -8.0f),
		glm::vec3(7.0f, 0.0f, -8.0f),
		glm::vec3(-1.0f, 0.0f, -8.0f),
		glm::vec3(-2.0f, 0.0f, -8.0f),
		glm::vec3(-3.0f, 0.0f, -8.0f),
		glm::vec3(-4.0f, 0.0f, -8.0f),
		glm::vec3(-5.0f, 0.0f, -8.0f),
		glm::vec3(-6.0f, 0.0f, -8.0f),
		glm::vec3(-7.0f, 0.0f, -8.0f),
		glm::vec3(-8.0f, 0.0f, -8.0f),
	};

	glm::vec3 lavaCubePositions[]
	{
		glm::vec3(0.0f, 0.0f, -9.0f),
		glm::vec3(1.0f, 0.0f, -9.0f),
		glm::vec3(2.0f, 0.0f, -9.0f),
		glm::vec3(3.0f, 0.0f, -9.0f),
		glm::vec3(4.0f, 0.0f, -9.0f),
		glm::vec3(5.0f, 0.0f, -9.0f),
		glm::vec3(6.0f, 0.0f, -9.0f),
		glm::vec3(7.0f, 0.0f, -9.0f),
		glm::vec3(-1.0f, 0.0f, -9.0f),
		glm::vec3(-2.0f, 0.0f, -9.0f),
		glm::vec3(-3.0f, 0.0f, -9.0f),
		glm::vec3(-4.0f, 0.0f, -9.0f),
		glm::vec3(-5.0f, 0.0f, -9.0f),
		glm::vec3(-6.0f, 0.0f, -9.0f),
		glm::vec3(-7.0f, 0.0f, -9.0f),
		glm::vec3(-8.0f, 0.0f, -9.0f),
	};

	glm::vec3 snowManPositions[] {
		glm::vec3(-3.0f, 1.0f, -6.0f),
		glm::vec3(-3.0f, 2.0f, -6.0f),
		glm::vec3(-3.0f, 3.0f, -6.0f),
	};

	glm::vec3 ironGolemPositions[]{
		glm::vec3(1.0f,  1.0f, 5.0f),
		glm::vec3(1.0f,  2.0f, 5.0f),
		glm::vec3(2.0f,  2.0f, 5.0f),
		glm::vec3(0.0f,  2.0f, 5.0f),
		glm::vec3(1.0f,  3.0f, 5.0f),
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  2.2f,  2.0f),
		glm::vec3(2.3f, 3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, 9.0f),
		glm::vec3(0.0f,  3.0f, -3.0f)
	};

	float skyboxVertices[] =
	{
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

	unsigned int skyboxIndices[] =
	{
		1, 2, 6,
		6, 5, 1,
		0, 4, 7,
		7, 3, 0,
		4, 5, 6,
		6, 7, 4,
		0, 3, 2,
		2, 1, 0,
		0, 1, 5,
		5, 4, 0,
		3, 7, 6,
		6, 2, 3
	};

	std::string facesCubemap[6]
	{
		"resource/texture/skybox/right.jpg",
		"resource/texture/skybox/left.jpg",
		"resource/texture/skybox/top.jpg",
		"resource/texture/skybox/bottom.jpg",
		"resource/texture/skybox/front.jpg",
		"resource/texture/skybox/back.jpg"
	};

	// cube
	unsigned int cubeVAO{};
	unsigned int cubeVBO{};
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// skybox
	unsigned int skyboxVAO{}, skyboxVBO{}, skyboxEBO{};
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);

	glBindVertexArray(skyboxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	unsigned int lightCubeVAO{};
	glGenVertexArrays(1, &lightCubeVAO);

	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned cubemapTexture{};
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i{ 0 }; i < 6; ++i)
	{
		int width{}, height{}, nrChannels{};
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << '\n';
			stbi_image_free(data);
		}
	}

	unsigned int diffuseMap{ loadTexture("resource/texture/grass.jpg") };
	unsigned int specularMap{ loadTexture("resource/texture/grass_specular.jpg") };
	unsigned int lavaDiffuseMap{ loadTexture("resource/texture/lava.jpg") };
	unsigned int lavaSpecularMap{ loadTexture("resource/texture/lava_specular.jpg") };
	unsigned int snowDiffuseMap{ loadTexture("resource/texture/snow.jpg") };
	unsigned int snowSpecularMap{ loadTexture("resource/texture/snow_specular.jpg") };
	unsigned int pumpkinDiffuseMap{ loadTexture("resource/texture/pumpkin.jpg") };
	unsigned int pumpkinSpecularMap{ loadTexture("resource/texture/pumpkin_specular.jpg") };
	unsigned int ironDiffuseMap{ loadTexture("resource/texture/iron.jpg") };
	unsigned int ironSpecularMap{ loadTexture("resource/texture/iron_specular.jpg") };

	/*unsigned int grassDiffuse = loadTexture("resource/texture/grass.jpg");
	unsigned int grassSpecular = loadTexture("resource/texture/grass_specular.jpg");

	unsigned int blockDiffuse = loadTexture("resource/texture/grass_block.jpg");
	unsigned int blockSpecular = loadTexture("resource/texture/grass_block_specular.jpg");

	unsigned int dirtDiffuse = loadTexture("resource/texture/dirt.jpg");
	unsigned int dirtSpecular = loadTexture("resource/texture/dirt_specular.jpg");*/

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame{ static_cast<float>(glfwGetTime()) };
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		camera.updateJump(deltaTime);

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.getPosition());
		lightingShader.setFloat("material.shininess", 32.0f);

		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09f);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09f);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09f);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09f);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		/*lightingShader.setVec3("spotLight.position", camera.getPosition());
		lightingShader.setVec3("spotLight.direction", camera.getFront());
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09f);
		lightingShader.setFloat("spotLight.quadratic", 0.032f);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));*/

		glm::mat4 projection{ glm::perspective(glm::radians(45.0f), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f) };
		lightingShader.setMat4("projection", projection);

		glm::mat4 view{ camera.getViewMatrix() };
		lightingShader.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 256; i++)
		{
			glm::mat4 model{ glm::mat4(1.0f) };
			model = glm::translate(model, cubePositions[i]);
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lavaDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lavaSpecularMap);

		for (unsigned int i = 0; i < 16; ++i)
		{
			glm::mat4 model{ glm::mat4(1.0f) };
			model = glm::translate(model, lavaCubePositions[i]);
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, snowDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, snowSpecularMap);

		for (unsigned int i = 0; i < 2; ++i)
		{
			glm::mat4 model{ glm::mat4(1.0f) };
			model = glm::translate(model, snowManPositions[i]);
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pumpkinDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pumpkinSpecularMap);

		for (unsigned int i = 2; i < 3; ++i)
		{
			glm::mat4 model{ glm::mat4(1.0f) };
			model = glm::translate(model, snowManPositions[i]);
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ironDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ironSpecularMap);

		for (unsigned int i = 0; i < 4; ++i)
		{
			glm::mat4 model{ glm::mat4(1.0f) };
			model = glm::translate(model, ironGolemPositions[i]);
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pumpkinDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pumpkinSpecularMap);

		for (unsigned int i = 4; i < 5; ++i)
		{
			glm::mat4 model{ glm::mat4(1.0f) };
			model = glm::translate(model, ironGolemPositions[i]);
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glDepthFunc(GL_LEQUAL);

		skyboxShader.use();
		skyboxShader.setMat4("projection", projection);
		view = glm::mat4(glm::mat3(camera.getViewMatrix()));
		skyboxShader.setMat4("view", view);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Restore openGl state
		glDepthFunc(GL_LESS);

		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		view = camera.getViewMatrix();
		lightCubeShader.setMat4("view", view);

		glBindVertexArray(lightCubeVAO);
		for (int i{ 0 }; i < 4; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

/*
* Callback to resize the viewport when the window is resized
* Parameters:
* - window: Pointer to the GLFW window
* - width: New width of the window in pixels
* - height: New height of the window in pixels
* Returns: void
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/*
* Handles keyboard input to control camera movement and exit
* Parameters:
* - window: Pointer to the GLFW window
* Returns: void
*/
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(forward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(backward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(right, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(left, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.jump();
}

/*
* Callback to process mouse movement and update camera orientation
* Parameters:
* - window: Pointer to the GLFW window
* - xpos: New x-coordinate of the mouse cursor
* - ypos: New y-coordinate of the mouse cursor
* Returns: void
*/
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	float xpos = static_cast<float>(xPosIn);
	float ypos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.processMouseMovement(xoffset, yoffset);
}

/*
* Loads a 2D texture from a file using stb_image
* Parameters:
* - path: Char pointer to the path of the image file
* Returns: OpenGL texture ID (unsigned int)
*/
unsigned int loadTexture(const char* path)
{
	unsigned int texture{};
	glGenTextures(1, &texture);

	int width{}, height{}, nrChannels{};
	unsigned char* data{ stbi_load(path, &width, &height, &nrChannels, 0) };
	if (data)
	{
		stbi_set_flip_vertically_on_load(true);

		GLenum format{};
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << '\n';
	}
	stbi_image_free(data);

	return texture;
}