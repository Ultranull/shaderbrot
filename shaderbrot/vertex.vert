#version 330 core
precision highp float;

layout(location = 0) in vec3 vertpos;

uniform mat4 model;

out vec3 fragcoord;

void main(){
    gl_Position=model*vec4(vertpos,1);
	fragcoord=vertpos;
}