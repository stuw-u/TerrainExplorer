#shader vertex
#version 450 core
 
uniform mat4 projectionMatrix;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

out vec3 outNormal;

void main () {
    //gl_Position = vec4(position.x, position.y, 0.0, 0.0);
    gl_Position = projectionMatrix * vec4(position, 1.0);
    outNormal = normal;
}

#shader fragment
#version 450 core

in vec3 outNormal;
out vec4 color;

void main () {
    //float c = frct(fragCoord/iResolution.xy) - frct((fragCoord+CONT)/iResolution.xy) + 0.5;
    float c = max(0.1, dot(outNormal, -normalize(vec3(0.5, -1.0, 0.75))));

	color = vec4(vec3(c, c, c), 1.0);
}