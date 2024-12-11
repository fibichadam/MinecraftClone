#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string>

class ShaderProgram {
public:
	ShaderProgram();
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& rhs) noexcept;
	ShaderProgram& operator=(ShaderProgram&& rhs) noexcept;
	~ShaderProgram();

	void Use();


	void SetInt(const std::string_view name, int value);
	void SetMat4(const std::string_view name, const glm::mat4& value);

private:
	GLuint m_programId;

	static std::string s_vertexShaderSource;
	static std::string s_fragmentShaderSource;

	GLuint CreateShader(const GLchar* shaderSource, GLenum shaderType);
	GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader);
};