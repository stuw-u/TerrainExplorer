#include "Shader.h"
#include "Renderer.h"
#include "RendererUtils.h"

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// The shader types are:
// - Surface (Frag + Vertex)
// - FragmentOnly
// - Compute



SurfaceShader::SurfaceShader(const std::string& filepath) 
: m_filepath(filepath) {
	
	SurfaceShaderProgramSource ssps = GetParsedSurfaceShader();
	m_RendererID = CreateSurfaceShader(ssps);
}

SurfaceShader::~SurfaceShader () {
	GLTry(glDeleteProgram(m_RendererID));
}

/// <summary>
/// Separates the surface shader file into two parts: the vertex shader and the fragment shader.
/// </summary>
SurfaceShaderProgramSource SurfaceShader::GetParsedSurfaceShader () {

	std::ifstream stream;
	stream.open(m_filepath);
	if(!stream) {
		std::cerr << "Failed to load shader at " << std::endl;
	}

	enum class SubShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	auto type = SubShaderType::NONE;

	while(getline(stream, line)) {
		if(line.find("#shader") != std::string::npos) { // If we detect a compiler tag
			
			if(line.find(" vert") != std::string::npos) // Switch string stream to vert if vertex is detected
				type = SubShaderType::VERTEX;

			else if(line.find(" frag") != std::string::npos) // Same for fragment
				type = SubShaderType::FRAGMENT;

			else
				type = SubShaderType::NONE;

		}
		else if((int)type >= 0) {
			ss[(int)type] << line << '\n';
		}
	}

	return {ss[0].str(), ss[1].str()};
}


/// <summary>
/// Sends the shader to be compiled by opengl and prints any compile error in the console.
/// </summary>
int SurfaceShader::CompileShader (unsigned int type, const std::string& source) {
	GLTry(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLTry(glShaderSource(id, 1, &src, nullptr));
	GLTry(glCompileShader(id));

	int result;
	GLTry(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	// If failed to compile shader
	if(result == GL_FALSE) { 

		int length;
		GLTry(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLTry(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;
		std::cout << message << std::endl;
		GLTry(glDeleteShader(id));
		return 0;

	}

	return id;
}


/// <summary>
/// Links frag. and vert. shader into a surface shader.
/// </summary>
unsigned int SurfaceShader::CreateSurfaceShader (SurfaceShaderProgramSource& shaderSurfaceProgramSource) {
	GLTry(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, shaderSurfaceProgramSource.vertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, shaderSurfaceProgramSource.fragmentSource);

	GLTry(glAttachShader(program, vs));
	GLTry(glAttachShader(program, fs));
	GLTry(glLinkProgram(program));
	GLTry(glValidateProgram(program));

	GLTry(glDeleteShader(vs));
	GLTry(glDeleteShader(fs));

	return program;
}


void SurfaceShader::Bind() const { GLTry(glUseProgram(m_RendererID)); }
void SurfaceShader::Unbind() const { GLTry(glUseProgram(0)); }


void SurfaceShader::SetMVPMatrix (const glm::mat4x4 model, const glm::mat4x4 viewProjection) {
	SetUniformMat4f(std::string("MVP_Matrix"), viewProjection * model);
}

void SurfaceShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	GLTry(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void SurfaceShader::SetUniformMat4f(const std::string& name, glm::mat4x4 mat) {
	GLTry(glUniformMatrix4fv(GetUniformLocation(name), 1, false, glm::value_ptr(mat)));
}


int SurfaceShader::GetUniformLocation(const std::string& name) {

	if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLTry(int location = glGetUniformLocation(m_RendererID, name.c_str()));
		m_UniformLocationCache[name] = location;
	if(location == -1)
		std::cout << "Warning : uniform name '" << name << "' doesn't exist." << std::endl;

	return location; 
}