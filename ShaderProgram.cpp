#include "ShaderProgram.h"

#include <iostream>
#include <assert.h>

std::string ShaderProgram::s_vertexShaderSource = "#version 460 core\n"
		"layout(location = 0) in vec3 inPosition;\n"
		"layout(location = 1) in vec2 inTexCoords;\n"
		"out vec2 outTexCoords;\n"
		"uniform mat4 mvp;\n"
		"void main() {\n"
		"gl_Position = mvp * vec4(inPosition, 1.0);\n"
		"outTexCoords = inTexCoords;\n"
		"}\0";

std::string ShaderProgram::s_fragmentShaderSource = "#version 460 core\n"
		"out vec4 fragmentColor;\n"
		"in vec2 outTexCoords;\n"
		"uniform sampler2D texture1;\n"
		"void main() {\n"
		"fragmentColor = texture(texture1, outTexCoords);\n"
		"}\0";

ShaderProgram::ShaderProgram()
	: m_programId(glCreateProgram()) {
	const GLuint vertexShader = CreateShader(s_vertexShaderSource.c_str(), GL_VERTEX_SHADER);
	glAttachShader(m_programId, vertexShader);

	const GLuint fragmentShader = CreateShader(s_fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);
	glAttachShader(m_programId, fragmentShader);

	glLinkProgram(m_programId);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	Use();
	SetInt("texture1", 0);
}

ShaderProgram::ShaderProgram(ShaderProgram&& rhs) noexcept
	: m_programId(std::exchange(rhs.m_programId, 0)) {
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& rhs) noexcept {
	if (&rhs == this) {
		return *this;
	}

	m_programId = std::exchange(rhs.m_programId, 0);

	return *this;
}

ShaderProgram::~ShaderProgram(){

}

void ShaderProgram::Use() {
	glUseProgram(m_programId);
}

void ShaderProgram::SetInt(const std::string_view name, int value) {
	glUniform1i(glGetUniformLocation(m_programId, name.data()), value);
}

void ShaderProgram::SetMat4(const std::string_view name, const glm::mat4& value) {
	glUniformMatrix4fv(glGetUniformLocation(m_programId, name.data()), 1, GL_FALSE, glm::value_ptr(value));
}

GLuint ShaderProgram::CreateShader(const GLchar* shaderSource, GLenum shaderType) {
	const GLuint shaderId = glCreateShader(shaderType);
	if (!shaderId) {
		return 0;
	}

	glShaderSource(shaderId, 1, &shaderSource, nullptr);
	glCompileShader(shaderId);

	int  success;
	char infoLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shaderId;
}

GLuint ShaderProgram::CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0) {
	const GLuint programId = glCreateProgram();
	if (!programId) {
		return 0;
	}

	glAttachShader(programId, vertexShader);
	glAttachShader(programId, fragmentShader);
	if (geometryShader) glAttachShader(programId, geometryShader);

	glLinkProgram(programId);

	return programId;
}
