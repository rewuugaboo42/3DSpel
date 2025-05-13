/*
* File: shader.h
* Author: Simon Olesen
* Date: 2025-05-13
* Description: This program links a vertex shader and fragment shader to a shader program
			   by loading from file and defines functions to bind it when needed
*/

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int shaderProgram{};

	/*
	* Loads and compiles vertex and fragment shaders and links them into a shader program
	* Parameters:
	* - vertexPath: Char pointer to the vertex shader file path
	* - fragmentPath: Char pointer to the fragment shader file path
	* Returns: Shader object containing a bindable shader program
	*/
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode{};
		std::string fragmentCode{};
		std::ifstream vShaderFile{};
		std::ifstream fShaderFile{};
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// Read shader files into streams
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream{};
			std::stringstream fShaderStream{};
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
		}

		const char* vShaderCode{ vertexCode.c_str() };
		const char* fShaderCode{ fragmentCode.c_str() };

		// Compile vertex shader
		unsigned int vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		checkCompileErrors(vertexShader, "VERTEX");

		// Compile fragment shader
		unsigned int fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		checkCompileErrors(fragmentShader, "FRAGMENT");

		// Link shaders into a program
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		checkCompileErrors(shaderProgram, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	/*
	* Activates the shader program for use in rendering
	* Parameters: None
	* Returns: void
	*/
	void use()
	{
		glUseProgram(shaderProgram);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
	}

	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat)
	{
		glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

	void setMat3(const std::string& name, const glm::mat3& mat)
	{
		glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

	void setMat4(const std::string& name, const glm::mat4& mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

private:
	/*
	* Checks and prints compile or linking errors for shaders
	* Parameters:
	* - shader: ID of the shader or program
	* - type: String indicating "VERTEX", "FRAGMENT", or "PROGRAM"
	* Returns: void
	*/
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success{};
		char infoLog[1024]{};
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << '\n' << infoLog << '\n';
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << '\n' << infoLog << '\n';
			}
		}
	}
};

#endif