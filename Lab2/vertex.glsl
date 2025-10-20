
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColor;

out vec3 vColor;

void main()
{

	// zmienna typu vec4
	vec4 finalPosition = inPosition;

	// Ustalenie wartosci zmiennej out
	vColor = inColor;

	// finalna pozycja wierzcholka
	gl_Position = finalPosition;
}
