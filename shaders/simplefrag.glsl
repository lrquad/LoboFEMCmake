#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    sampler2D diffuse_tex;
    sampler2D emissive_tex;
    float shininess;
}; 
  
uniform Material material;

out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoords;
in vec3 ourNormal;
in vec3 FragPos;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 viewPos;  

uniform bool useDiffuseTex = false;

void main()
{
    // ambient
    vec3 ambient = lightColor * material.ambient;

    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * material.diffuse);

    if (useDiffuseTex == true)
    {
        diffuse = lightColor * diff * vec3(texture(material.diffuse_tex, TexCoords)); 
        vec3 emission = texture(material.emissive_tex, TexCoords).rgb;
        diffuse+=emission;
    }

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (spec * material.specular) * lightColor; 



    vec3 result = ambient+diffuse+specular;
    FragColor = vec4(result, 1.0);
    //FragColor = texture(ourTexture, TexCoord);
    //FragColor = vec4(diffuse_color,1);
}