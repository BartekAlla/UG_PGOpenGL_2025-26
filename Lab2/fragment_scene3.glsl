#version 330 core

in vec2 vPos;
out vec4 FragColor;

void main()
{
    int id = gl_PrimitiveID % 9; 

    vec3 baseColor;
    if (id == 0) baseColor = vec3(0.8, 0.2, 0.2);     
    else if (id == 1) baseColor = vec3(0.9, 0.7, 0.3); 
    else if (id == 2) baseColor = vec3(0.4, 0.3, 0.8); 
    else if (id == 3) baseColor = vec3(0.3, 0.6, 0.9); 
    else if (id == 4) baseColor = vec3(0.2, 0.8, 0.5); 
    else if (id == 5) baseColor = vec3(0.7, 0.9, 0.7); 
    else if (id == 6) baseColor = vec3(0.5, 0.1, 0.2); 
    else if (id == 7) baseColor = vec3(0.1, 0.3, 0.3); 
    else if (id == 8) baseColor = vec3(0.3, 0.7, 0.4); 
    else baseColor = vec3(0.8, 0.2, 0.2);     
     


    float gradient = gl_FragCoord.y / 500.0; 
    gradient = clamp(gradient, 0.0, 1.0);

    // Wzmocnienie kontrastu kolorów gradientem
    vec3 color = mix(baseColor * 0.6, baseColor, gradient);

    FragColor = vec4(color, 1.0);
}
