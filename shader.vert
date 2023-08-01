#version 330 core
layout (location=0) in vec2 position;
layout (location=1) in float color_s;
uniform mat4 u_projection;
out float v_color;

void main()
{
gl_Position = u_projection * vec4(position,1.0,1.0);
v_color = color_s;
}