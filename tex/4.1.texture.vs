#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 camera;

void main()
{
//	gl_Position = transform * vec4(aPos, 1.0);
//	gl_Position = projection * transform * vec4(aPos, 1.0);
	gl_Position = projection * camera * transform * vec4(aPos, 1.0);
	FragPos = vec3(transform * vec4(aPos, 1.0));
//	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	Normal = aNormal;
}