#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

out vec2 fragUV;
out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragLightCoef;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;
uniform vec3 cameraPos;
uniform bool enablePointLight;
uniform float lightingModel; // 0.0 = Phong, 11.0 = Blinn-Phong

// -----------------------
// STRUKTURY
// -----------------------
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

// -----------------------
// PHONG – ŚWIATŁO PUNKTOWE
// -----------------------
vec3 calculatePointLight(
    vec3 Position,
    vec3 Normal,
    vec3 CameraPos,
    LightParam light,
    MaterialParam material,
    float lightingModel // float z GUI
)
{
    // konwersja na int
    int model = int(lightingModel + 0.5);

    vec3 ambient = light.Ambient * material.Ambient;
    vec3 L = normalize(light.Position - Position);
    float diff = max(dot(Normal, L), 0.0);
    vec3 diffuse = diff * light.Diffuse * material.Diffuse;

    vec3 specular = vec3(0.0);
    vec3 V = normalize(CameraPos - Position);

    if(model == 0) // Phong
    {
        vec3 R = reflect(-L, Normal);
        float spec = pow(max(dot(V, R), 0.0), material.Shininess);
        specular = spec * light.Specular * material.Specular;
    }
    else // Blinn-Phong
    {
        vec3 H = normalize(L + V);
        float spec = pow(max(dot(Normal, H), 0.0), material.Shininess);
        specular = spec * light.Specular * material.Specular;
    }

    float dist = length(light.Position - Position);
    float att = 1.0 / (light.Attenuation.x + light.Attenuation.y * dist + light.Attenuation.z * dist * dist);

    return ambient + att * (diffuse + specular);
}



void main()
{
    // Pozycja i normalna w przestrzeni świata
    fragPos = vec3(matModel * vec4(inPos, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(matModel))) * inNormal);
    fragUV = inUV;

    // PARAMETRY ŚWIATŁA
    LightParam light = LightParam(
        vec3(0.1),
        vec3(1.0),
        vec3(1.0),
        vec3(1.0, 0.0, 0.02),
        vec3(2.0, 4.0, 2.0)
    );

    // PARAMETRY MATERIAŁU
    MaterialParam material = MaterialParam(
        vec3(0.2),
        vec3(1.0),
        vec3(0.5),
        32.0
    );

    // LICZENIE PHONGA/BLINN
    if(enablePointLight)
    {
        fragLightCoef = calculatePointLight(
            fragPos,
            fragNormal,
            cameraPos,
            light,
            material,
            lightingModel 
        );
    }
    else
    {
        fragLightCoef = vec3(1.0);
    }


    gl_Position = matProj * matView * matModel * vec4(inPos, 1.0);
}
