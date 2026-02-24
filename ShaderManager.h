#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool LoadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void use();

	void setMat4Value(const std::string& name, const glm::mat4& value);
	void setVec4Value(const std::string& name, const glm::vec4& value);
	void setVec3Value(const std::string& name, const glm::vec3& value);
	void setVec2Value(const std::string& name, const glm::vec2& value);
	void setFloatValue(const std::string& name, float value);
	void setIntValue(const std::string& name, int value);
	void setSampler2DValue(const std::string& name, int value);

private:
	GLuint m_programId = 0;
};
