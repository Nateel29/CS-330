#include "ShaderManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace
{
	std::string ReadTextFile(const std::string& filePath)
	{
		std::ifstream fileStream(filePath, std::ios::in);
		if (!fileStream.is_open())
		{
			std::cerr << "ERROR: Failed to open shader file: " << filePath << std::endl;
			return std::string();
		}

		std::ostringstream buffer;
		buffer << fileStream.rdbuf();
		return buffer.str();
	}

	GLuint CompileShader(GLenum shaderType, const std::string& source, const std::string& label)
	{
		if (source.empty())
		{
			return 0;
		}

		GLuint shaderId = glCreateShader(shaderType);
		const char* sourceCStr = source.c_str();
		glShaderSource(shaderId, 1, &sourceCStr, nullptr);
		glCompileShader(shaderId);

		GLint success = 0;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
			std::cerr << "ERROR: Shader compile failed (" << label << ")\n" << infoLog << std::endl;
			glDeleteShader(shaderId);
			return 0;
		}

		return shaderId;
	}
}

ShaderManager::ShaderManager() = default;

ShaderManager::~ShaderManager()
{
	if (m_programId != 0)
	{
		glDeleteProgram(m_programId);
		m_programId = 0;
	}
}

bool ShaderManager::LoadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::string vertexSource = ReadTextFile(vertexShaderPath);
	std::string fragmentSource = ReadTextFile(fragmentShaderPath);

	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource, "vertex");
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, "fragment");
	if (vertexShader == 0 || fragmentShader == 0)
	{
		return false;
	}

	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShader);
	glAttachShader(programId, fragmentShader);
	glLinkProgram(programId);

	GLint success = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLchar infoLog[1024];
		glGetProgramInfoLog(programId, sizeof(infoLog), nullptr, infoLog);
		std::cerr << "ERROR: Shader program link failed\n" << infoLog << std::endl;
		glDeleteProgram(programId);
		programId = 0;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (programId == 0)
	{
		return false;
	}

	if (m_programId != 0)
	{
		glDeleteProgram(m_programId);
	}
	
	m_programId = programId;
	return true;
}

void ShaderManager::use()
{
	if (m_programId != 0)
	{
		glUseProgram(m_programId);
	}
}

void ShaderManager::setMat4Value(const std::string& name, const glm::mat4& value)
{
	GLint location = glGetUniformLocation(m_programId, name.c_str());
	if (location >= 0)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}
}

void ShaderManager::setVec4Value(const std::string& name, const glm::vec4& value)
{
	GLint location = glGetUniformLocation(m_programId, name.c_str());
	if (location >= 0)
	{
		glUniform4fv(location, 1, &value[0]);
	}
}

void ShaderManager::setVec3Value(const std::string& name, const glm::vec3& value)
{
	GLint location = glGetUniformLocation(m_programId, name.c_str());
	if (location >= 0)
	{
		glUniform3fv(location, 1, &value[0]);
	}
}

void ShaderManager::setVec2Value(const std::string& name, const glm::vec2& value)
{
	GLint location = glGetUniformLocation(m_programId, name.c_str());
	if (location >= 0)
	{
		glUniform2fv(location, 1, &value[0]);
	}
}

void ShaderManager::setFloatValue(const std::string& name, float value)
{
	GLint location = glGetUniformLocation(m_programId, name.c_str());
	if (location >= 0)
	{
		glUniform1f(location, value);
	}
}

void ShaderManager::setIntValue(const std::string& name, int value)
{
	GLint location = glGetUniformLocation(m_programId, name.c_str());
	if (location >= 0)
	{
		glUniform1i(location, value);
	}
}

void ShaderManager::setSampler2DValue(const std::string& name, int value)
{
	GLint location = glGetUniformLocation(m_programId, name.c_str());
	if (location >= 0)
	{
		glUniform1i(location, value);
	}
}
