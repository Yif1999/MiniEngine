#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

// texture samplers
uniform sampler2D result;

void main()
{
	FragColor = texture(result, TexCoord);
	FragColor.rgb = pow(FragColor.rgb, vec3(2.2));
}