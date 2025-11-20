#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

out vec4 outColor;

uniform vec3 objectColor;      
uniform vec3 lightDir = vec3(0.3, 1.0, 0.4); 
uniform float time;  

void main()
{
    // Normalizacja
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightDir);

    // Proste cieniowanie Lambertowe
    float diff = max(dot(N, L), 0.0);

    // GRADIENT — zależny od wysokości (Y)
    float h = clamp((vWorldPos.y + 2.0) / 4.0, 0.0, 1.0);

    // Gradient kolorów: ciemniejszy → jaśniejszy
    vec3 baseGradient = mix(objectColor * 0.3, objectColor, h);
	// Animacja koloru tylko dla podłoża
    if (objectColor == vec3(0.4, 0.3, 0.1)) { // kolor podłoża, można też użyć bool/uniform
        float pulse = sin(time * 2.0) * 0.5 + 0.5; // pulsowanie od 0 do 1
        baseGradient = mix(baseGradient, vec3(0.0, 1.0, 0.0), pulse); // np. zielony pulsujący
    }
    // Dodajemy oświetlenie
    vec3 finalColor = baseGradient * (0.4 + diff * 0.6);

    outColor = vec4(finalColor, 1.0);
}
