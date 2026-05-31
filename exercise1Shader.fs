#version 330 core
out vec4 FragColor;
in vec3 color;
in vec4 vPos;
in vec2 TexPos;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float mixValue;
void main()
{
  FragColor = mix(texture(tex1, TexPos), texture(tex2, TexPos), mixValue);
}
