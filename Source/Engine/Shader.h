#pragma once
#include <string>
#include <unordered_map>
#include "../External/glm/mat4x4.hpp"
#include "../External/glm/gtc/type_ptr.hpp"

struct SurfaceShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

struct ShaderGenericProgramSource {
	std::string source;
};

class SurfaceShader {
private:
	std::string m_filepath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	SurfaceShader(const std::string& filename);
	~SurfaceShader();

	void Bind() const;
	void Unbind() const;

	void SetProjectionViewMatrix (const glm::mat4x4);
	void SetUniform4f(const std::string& name, float v0, float v2, float v3, float v4);
	void SetUniformMat4f(const std::string& name, glm::mat4x4 mat);
private:
	int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateSurfaceShader (SurfaceShaderProgramSource& shaderSurfaceProgramSource);
	SurfaceShaderProgramSource GetParsedSurfaceShader ();
	int GetUniformLocation (const std::string& name);

};