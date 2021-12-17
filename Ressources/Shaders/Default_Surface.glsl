#shader vertex
#version 450 core
 
uniform mat4 MVP_Matrix; // model * view * projection
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

out vec3 inNormal;
out vec3 inColor;

void main () {
    gl_Position = MVP_Matrix * vec4(position, 1.0);
    inNormal = normal;
    inColor = color;
}

#shader fragment
#version 450 core

in vec3 inNormal;
in vec3 inColor;

out vec4 color;

void main () {
    //float c = frct(fragCoord/iResolution.xy) - frct((fragCoord+CONT)/iResolution.xy) + 0.5;
    float c = abs(dot(inNormal, -normalize(vec3(0.5, -1.0, 0.75))));

	color = vec4(inColor * c, 1.0);
}