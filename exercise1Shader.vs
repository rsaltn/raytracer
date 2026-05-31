#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 tPos;

out vec3 color;
out vec4 vPos;
out vec2 TexPos;

uniform float XoffSet;
uniform float YoffSet;
uniform float AspectRatio;
void main()
{
  gl_Position = vec4(aPos.x/AspectRatio + XoffSet, aPos.y + YoffSet, aPos.z, 1.0);
  color = aColor;
  vPos = gl_Position;
  TexPos = tPos;
}
