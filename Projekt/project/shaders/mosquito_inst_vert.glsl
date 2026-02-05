#version 330 core
layout(location=0) in vec3 inPos;
layout(location=2) in vec2 inUV;

// per-instance:
layout(location=3) in vec4 iPosScale;  // xyz + scale
layout(location=4) in vec4 iRotPhase;  // rotY + phase + unused + unused

out vec2 fragUV;

uniform mat4 matProj;
uniform mat4 matView;

mat4 rotY(float a){
    float c = cos(a), s = sin(a);
    return mat4(
        c,0,-s,0,
        0,1, 0,0,
        s,0, c,0,
        0,0, 0,1
    );
}

void main()
{
    fragUV = inUV;

    vec3 pos = iPosScale.xyz;
    float sc = iPosScale.w;
    float rot = iRotPhase.x;
    float phase = iRotPhase.y;

    pos.y += 0.2 * sin(phase);

    mat4 M = mat4(1.0);
    M[3] = vec4(pos, 1.0);
    M = M * rotY(rot);

    vec4 world = M * vec4(inPos * sc, 1.0);
    gl_Position = matProj * matView * world;
}
