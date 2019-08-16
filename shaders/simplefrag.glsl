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
in vec4 FragPosLightSpace;

//uniform vec3 lightPos; 
//uniform vec3 lightColor;
uniform vec3 viewPos;  

uniform bool useDiffuseTex = false;

uniform sampler2D shadowMap;

#define NR_POINT_LIGHTS 6
uniform Lights lights[NR_POINT_LIGHTS];

vec3 CalcDirLight(Lights light, vec3 normal, vec3 viewDir,float shadow);
vec3 CalcPointLight(Lights light, vec3 normal, vec3 fragPos, vec3 viewDir,float shadow);
float ShadowCalculation(Lights light,vec4 fragPosLightSpace);

void main()
{
    // properties
    vec3 norm = normalize(ourNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    float shadow = ShadowCalculation(lights[0],FragPosLightSpace);  
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        if(lights[i].trigger==true)
        {
            if (lights[i].light_type==0)
                result += CalcPointLight(lights[i], norm, FragPos, viewDir,shadow);  
            if (lights[i].light_type==1)
                result += CalcDirLight(lights[i], norm, FragPos,shadow);  
        }
    }  
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(Lights light, vec3 normal, vec3 viewDir,float shadow)
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
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));    
    return lighting;
}

vec3 CalcPointLight(Lights light, vec3 normal, vec3 fragPos, vec3 viewDir,float shadow)
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
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));    
    return lighting;
}

float ShadowCalculation(Lights light,vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(ourNormal);
    vec3 lightDir = normalize(light.position - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.004);
    bias = 0.0;
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 4.0 / textureSize(shadowMap, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            float contri_shadow = currentDepth - bias > pcfDepth  ? 1.0 : 0.0; 
            shadow += contri_shadow;        
        }    
    }
    shadow /= 25.0;
    shadow*=0.5;
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}