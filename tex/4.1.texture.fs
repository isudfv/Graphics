#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// texture sampler
uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
	//常量环境因子
	float ambientStrength = 0.1;
	//镜面强度(Specular Intensity)
	float specularStrength = 0.5;
	vec3 ambient = ambientStrength * lightColor;
	// FragColor = vec4(1.0, 1.0, 1.0 , 1.0) * texture(texture1, TexCoord);
	// FragColor = vec4(ambient , 1.0) * texture(texture1, TexCoord);
	// FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 cameraDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 128);
	vec3 specular = specularStrength * spec * lightColor;

	FragColor = vec4((ambient + diffuse + specular), 1.0) * texture(texture1, TexCoord);
}

