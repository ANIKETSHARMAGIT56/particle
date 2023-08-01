#version 440 core
layout (location=0) out vec4 color;
in float v_color;

void main()
{
    vec3 col = vec3(0.9529, 0.1412, 0.0667);
    color=vec4(col.x,col.y,col.z,v_color);
    // color = vec4(0.1922, 0.8353, 0.0941, 0.767)
}