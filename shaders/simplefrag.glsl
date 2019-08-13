#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;

uniform vec3 diffuse_color;

uniform sampler2D ourTexture;

void main()
{
    //FragColor = texture(ourTexture, TexCoord);
    FragColor = vec4(diffuse_color,1);
}