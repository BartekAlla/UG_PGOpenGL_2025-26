#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

struct LightParam
{
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    vec3 Attenuation;
    vec3 Position;
};

struct MaterialParam
{
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};

out vec2 fragUV;
out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragLightCoef;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;
uniform vec3 cameraPos;
uniform bool enablePointLight;
uniform float lightingModel; 
uniform float lightIntensity;

uniform LightParam myLight;
uniform MaterialParam myMaterial;

vec3 calculatePointLight(
    vec3 Position,
    vec3 Normal,
    vec3 cameraPos,
    LightParam light,
    MaterialParam material,
    int model
)
{
    //Ambient
    vec3 ambient = light.Ambient * material.Ambient;
    //Diffuse
    vec3 L = normalize(light.Position - Position.xyz);
    float diff = max(dot(L, Normal), 0);
    vec3 diffuse = diff * light.Diffuse * material.Diffuse;
    //Specular
    vec3 E = normalize(cameraPos - Position.xyz);
    vec3 specular = vec3(0.0);

    if(model == 0) // Phong
    {
        vec3 R = reflect(-E, Normal);
        float spec = pow(max(dot(R, L), 0), material.Shininess);
        specular = spec * light.Specular * material.Specular;
    }
    else // Blinn-Phong
    {
        vec3 H = normalize(L + E);
        float spec = pow(max(dot(Normal, H), 0.0), material.Shininess);
        specular = spec * light.Specular * material.Specular;
    }
    //Attenuation
    float LV = distance(Position.xyz, light.Position);
    float latt = 1.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);
    vec3 result = ambient + latt * (diffuse + specular);
    return result * lightIntensity;
}




void main()
{
    fragPos = vec3(matModel * vec4(inPos, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(matModel))) * inNormal);
    fragUV = inUV;

    if(enablePointLight)
    {
        fragLightCoef = calculatePointLight(
            fragPos,
            fragNormal,
            cameraPos,
            myLight,
            myMaterial,
            int(lightingModel)
        );
    }
    else
    {
        fragLightCoef = vec3(1.0);
    }

    gl_Position = matProj * matView * matModel * vec4(inPos, 1.0);

}
