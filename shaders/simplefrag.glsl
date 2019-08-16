#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    sampler2D diffuse_tex;
    sampler2D emissive_tex;
    float shininess;
}; 

struct Lights {
    vec3 position;
    vec3 direction;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 lightColor;
    bool trigger;
    int light_type; //0 point 1 directional
};
  
uniform Material material;

out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoords;
in vec3 ourNormal;
in vec3 FragPos;

//uniform vec3 lightPos; 
//uniform vec3 lightColor;
uniform vec3 viewPos;  

uniform bool useDiffuseTex = false;


#define NR_POINT_LIGHTS 6
uniform Lights lights[NR_POINT_LIGHTS];

vec3 CalcDirLight(Lights light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Lights light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        if(lights[i].trigger==true)
        {
            if (lights[i].light_type==0)
                result += CalcPointLight(lights[i], norm, FragPos, viewDir);  
            if (lights[i].light_type==1)
                result += CalcDirLight(lights[i], norm, FragPos);  
        }
    }  

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(Lights light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.lightColor * material.ambient;
    vec3 diffuse = light.lightColor * diff * material.diffuse;
    vec3 specular = light.lightColor * spec * material.specular;

    if (useDiffuseTex == true)
    {
        diffuse = light.lightColor * diff * vec3(texture(material.diffuse_tex, TexCoords)); 
        vec3 emission = texture(material.emissive_tex, TexCoords).rgb;
        diffuse+=emission;
    }
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(Lights light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.lightColor * material.ambient;
    vec3 diffuse = light.lightColor * diff * material.diffuse;
    vec3 specular = light.lightColor * spec * material.specular;

    if (useDiffuseTex == true)
    {
        diffuse = light.lightColor * diff * vec3(texture(material.diffuse_tex, TexCoords)); 
        vec3 emission = texture(material.emissive_tex, TexCoords).rgb;
        diffuse+=emission;
    }

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    //return material.ambient;
    return (ambient + diffuse + specular);
}
