// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
// Przyklad programu, w ktorym wczytujemy dane
// modelu 3D z pliku w formacie OBJ.
// -------------------------------------------------
// Szablon aplikacji w OpenGL korzystajacej
// z GLFW zamiast freeGLUT
//
// Kompilacja pod windowsem
// g++ plik.cpp glad.c -lglfw3 -lopengl32 -lgdi32
//
// Wymagana jest bibliteka glad
// https://glad.dav1d.de/
//
// Dolaczony plik glad.c jest skonfigurowany pod
// Opengl 4.5. W razie problemow nalezy podmienic
// skonfigurowac glada pod swoje srodowisko.
// -------------------------------------------------
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

// Bibliteka GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Okno aplikacji
int windowWidth = 800, windowHeight = 800;
const char *windowTitle = "OpenGL w GLFW (obrot klawiszami WSAD oraz mysza)";

#include "utilities.hpp"
#include "objloader.hpp"



glm::mat4 matProj;
glm::mat4 matView;
glm::mat4 matModel;

float angleY = 0.0f;
float angleX = 0.0f;
float distance = 0.0f;
float orbitAngle = 0.0f;
float R = 3.0f;
class CMesh
{
private:
    GLuint VAO;
    GLuint VBO_vertices;
    GLuint VBO_normals;
    size_t vertexCount;

public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); 
    glm::vec3 scale    = glm::vec3(1.0f);
    CMesh(const char* objPath)
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!loadOBJ(objPath, vertices, uvs, normals))
        {
            printf("Could not load OBJ: %s\n", objPath);
            return;
        }

        vertexCount = vertices.size();

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &VBO_normals);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

	  ~CMesh()
    {
        glDeleteBuffers(1, &VBO_vertices);
        glDeleteBuffers(1, &VBO_normals);
        glDeleteVertexArrays(1, &VAO);
    }
     glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        model = glm::rotate(model, rotation.x, glm::vec3(1,0,0));
        model = glm::rotate(model, rotation.y, glm::vec3(0,1,0));
        model = glm::rotate(model, rotation.z, glm::vec3(0,0,1));

        model = glm::scale(model, scale);

        return model;
    }
    void Draw() const
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
    }
    void ChangePosition(const glm::vec3& newPos)
    {
        position = newPos;
    }
    void OrbitAroundPoint(const glm::vec3& center, float radius, float orbitAngle, float offsetAngle)
    {
        float x = center.x + radius * cos(orbitAngle + offsetAngle);
        float y = center.y + radius * sin(orbitAngle + offsetAngle);
        float z = center.z;
        position = glm::vec3(x, y, z);

        glm::vec3 dirToCenter = center - position;
        float angle = atan2(dirToCenter.y, dirToCenter.x);
        rotation.z = angle - glm::radians(-90.0f); 
    }
    glm::vec3 GetPosition() const
    {
        return position;
    }
};

class CProgram
{
private:
    GLuint id;
public:

    CProgram(const char* vsPath, const char* fsPath)
    {
        id = glCreateProgram();
        glAttachShader(id, LoadShader(GL_VERTEX_SHADER, vsPath));
        glAttachShader(id, LoadShader(GL_FRAGMENT_SHADER, fsPath));
        LinkAndValidateProgram(id);
    }
	~CProgram()
    {
        glDeleteProgram(id);
    }
    void Use() const { glUseProgram(id); }
    void Stop() const { glUseProgram(0); }

    void SetVec3(const char* name, const glm::vec3& v) const
    {
        glUniform3fv(glGetUniformLocation(id, name), 1, glm::value_ptr(v));
    }

    void SetMatrix(const char* name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(mat));
    }
    GLuint getProgramID() {
        return this->id;
    }
    void SetFloat(const char* name, float v)
    {
        glUniform1f(glGetUniformLocation(id, name), v);
    }
};

void DisplayScene(CProgram &program, const std::vector<CMesh*> &meshes, const std::vector<glm::vec3> &colors) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // kamera
    matView = glm::mat4(1.0f);

    // zoom
    matView = glm::translate(matView, glm::vec3(0.0f, 0.0f, -10.0f - distance));

    // obrót kamery
    matView = glm::rotate(matView, angleX, glm::vec3(1,0,0));
    matView = glm::rotate(matView, angleY, glm::vec3(0,1,0));

    program.Use();
    program.SetMatrix("matProj", matProj);
    program.SetMatrix("matView", matView);

    for (int i = 0; i < meshes.size(); i++)
    {
        CMesh* mesh = meshes[i];

        glm::mat4 model = mesh->GetModelMatrix();
        program.SetMatrix("matModel", model);

        program.SetVec3("objectColor", colors[i]);

        mesh->Draw();
    }

    program.Stop();
}

void Initialize()
{

	// Ustawianie koloru, ktorym bedzie czyszczony bufor koloru
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glEnable(GL_DEPTH_TEST);

	// Obliczanie macierzy rzutowania perspektywicznego
	// za pierwszym razem (po utworzeniu okna aplikacji)
	if (windowHeight != 0) {
		matProj = glm::perspective(glm::radians(80.0f), windowWidth/(float)windowHeight, 0.1f, 50.0f);
	}
		

}

// ---------------------------------------------------
// funkcja zwrotna do obslugi zmiany rozmiaru framebuffera
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;


    // Viewport
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    // -------------------------------------------------------
	// NOWE! Obliczanie macierzy rzutowania perspektywicznego
	// Dlaczego akurat tutaj?
	// -------------------------------------------------------
	matProj = glm::perspective(glm::radians(70.0f), width/(float)height, 0.1f, 20.0f );

}


// ---------------------------------------------------
// funkcja zwrotna do obslugi klawiatury
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch(key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;

		case GLFW_KEY_W:
			angleX += 0.1f;
			break;

		case GLFW_KEY_S:
			angleX -= 0.1f;
			break;

		case GLFW_KEY_D:
			angleY += 0.1f;
			break;

		case GLFW_KEY_A:
			angleY -= 0.1f;
			break;
		case GLFW_KEY_KP_ADD: 
			distance -= 0.2f;
    		break;
		case GLFW_KEY_KP_SUBTRACT: 
    		distance += 0.2f;
    		break;
		default:
			printf("Nacisnieto klawisz %d \n", key);
			break;
		}
	}
}

//obsługa scrolla
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	 distance -= yoffset;
}


// obsługa kursora
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (__mouse_press && __mouse_button == GLFW_MOUSE_BUTTON_LEFT)
	{
		angleX += (ypos - __mouse_buttonY)/(float)windowWidth;
		angleY += (xpos - __mouse_buttonX)/(float)windowHeight;
        __mouse_buttonX = xpos;
        __mouse_buttonY = ypos;
	}
}

int main( int argc, char *argv[] )
{
	// Kontekst i okno aplikacji
    GLFWwindow* window = nullptr;
	Initialize_GLFW(window);

 	CProgram program("vertex.glsl", "fragment.glsl");
    CMesh monkey("../models/monkey.obj");
    monkey.position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 colorMonkey  = glm::vec3(1.0f, 0.5f, 0.1f); 
    CMesh cone1("../models/cone.obj");
    cone1.position = glm::vec3(R, 0.0f, 0.0f);
    cone1.scale = glm::vec3(1.0f, 2.0f, 1.0f);
    glm::vec3 cone1Color    = glm::vec3(0.2f, 0.7f, 0.2f); 
    CMesh cone2("../models/cone.obj");
    cone2.position = glm::vec3(-R, 0.0f, 0.0f);
    cone2.scale = glm::vec3(1.0f, 2.0f, 1.0f);
    glm::vec3 cone2Color  = glm::vec3(0.2f, 0.7f, 0.2f); 
	CMesh cone3("../models/cone.obj");
    cone3.position = glm::vec3(0.0f, R, 0.0f);
    cone3.scale = glm::vec3(1.0f, 2.0f, 1.0f); 
    glm::vec3 cone3Color = glm::vec3(0.2f, 0.7f, 0.2f); 
    CMesh cone4("../models/cone.obj");
    cone4.position = glm::vec3(0.0f, -R, 0.0f);  
    cone4.scale = glm::vec3(1.0f, 2.0f, 1.0f); 
    glm::vec3 cone4Color    = glm::vec3(0.2f, 0.7f, 0.2f); 

    std::vector<CMesh*> meshes = {
        &monkey,
        &cone1,
        &cone2,
        &cone3,
        &cone4
    };
    std::vector<glm::vec3> colors = {
        colorMonkey,
        cone1Color,
        cone2Color,
        cone3Color,
        cone4Color
    };
	Initialize();


	// Glowna petla
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
        float currentTime = glfwGetTime();
        glUseProgram(program.getProgramID());
        glUniform1f(glGetUniformLocation(program.getProgramID(), "time"), currentTime);
        glUseProgram(0);
    
        cone1.OrbitAroundPoint(monkey.GetPosition(), R, orbitAngle, 0.0f);
        cone2.OrbitAroundPoint(monkey.GetPosition(), R, orbitAngle, glm::radians(90.0f));
        cone3.OrbitAroundPoint(monkey.GetPosition(), R, orbitAngle, glm::radians(180.0f));
        cone4.OrbitAroundPoint(monkey.GetPosition(), R, orbitAngle, glm::radians(270.0f));
		DisplayScene(program, meshes, colors);
        monkey.rotation.y += 0.05f;
        orbitAngle += 0.01f;
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
