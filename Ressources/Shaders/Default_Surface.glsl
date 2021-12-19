#shader vertex
#version 450 core
 
uniform mat4 MVP_Matrix; // model * view * projection
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

out vec3 inNormal;
out vec3 inColor;

void main () {
    float light = 0.7 + (dot(normal, -normalize(vec3(0.5, -1.0, 0.75)))) * 0.3;
    gl_Position = MVP_Matrix * vec4(position, 1.0);
    inNormal = normal;
    inColor = color * light;
}

#shader fragment
#version 450 core

in vec3 inNormal;
in vec3 inColor;

out vec4 color;

void main () {
	color = vec4(inColor, 1.0);
}