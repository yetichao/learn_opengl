#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 ourColor;
uniform vec3 translation;
out vec3 outPosition;

void main()
{
    //gl_Position = vec4(translation.x + position.x, translation.y - position.y,
    //translation.z  + position.z, 1.0f);
    gl_Position = vec4(position, 1.0f);
    ourColor = color;
    //outPosition = position;
}