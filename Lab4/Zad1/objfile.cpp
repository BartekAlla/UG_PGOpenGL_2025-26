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
int windowWidth = 600, windowHeight = 600;
const char *windowTitle = "OpenGL w GLFW (obrot klawiszami WSAD oraz mysza)";

// Funkcje pomocnicze
#include "utilities.hpp"

// ----------------------------------------------
// NOWE: plik naglowkowy z funkcja do plikow OBJ
// ----------------------------------------------
#include "objloader.hpp"



// Macierze transformacji i rzutowania
glm::mat4 matProj;
glm::mat4 matView;
glm::mat4 matModel;

// Zmienne do kontroli obrotu obiektu
float angleY = 0.0f;
float angleX = 0.0f;
float distance = 0.0f;

class CMesh
{
public:
    GLuint VAO;
    GLuint VBO_vertices;
    GLuint VBO_normals;
    size_t vertexCount;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // rotacja w radianach
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
};
// ---------------------------------------
class CProgram
{
public:
    GLuint id;

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
};

void DisplayScene(CProgram &program, const std::vector<CMesh*> &meshes, const std::vector<glm::vec3> &colors) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // --- KAMERA ---
    matView = glm::mat4(1.0f);

    // ZOOM
    matView = glm::translate(matView, glm::vec3(0.0f, 0.0f, -4.0f - distance));

    // OBRÓT KAMERY
    matView = glm::rotate(matView, angleX, glm::vec3(1,0,0));
    matView = glm::rotate(matView, angleY, glm::vec3(0,1,0));

    program.Use();
    program.SetMatrix("matProj", matProj);
    program.SetMatrix("matView", matView);

    // for (CMesh* mesh : meshes)
    // {
    //     glm::mat4 model = mesh->GetModelMatrix();
    //     program.SetMatrix("matModel", model);
    //     mesh->Draw();
    // }
    for (int i = 0; i < meshes.size(); i++)
    {
        CMesh* mesh = meshes[i];

        glm::mat4 model = mesh->GetModelMatrix();
        program.SetMatrix("matModel", model);

        // ustawiamy kolor dla danego obiektu
        program.SetVec3("objectColor", colors[i]);

        mesh->Draw();
    }

    program.Stop();
}
//---------------------------------------
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

// ---------------------------------------------------
// funkcja zwrotna do obslugi scrolla myszy
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	 distance -= yoffset;
}

// --------------------------------------------------------------
// funkcja zwrotna do obslugi ruchu kursora myszy
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


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// Kontekst i okno aplikacji
    GLFWwindow* window = nullptr;
	Initialize_GLFW(window);

 	CProgram program("vertex.glsl", "fragment.glsl");
    CMesh monkey("../models/monkey.obj");
    monkey.position = glm::vec3(0.0f, 1.1f, -5.5f);
    monkey.scale    = glm::vec3(2.0f);
    glm::vec3 colorMonkey  = glm::vec3(1.0f, 0.5f, 0.1f); 
    CMesh palm("../models/palm.obj");
    palm.position = glm::vec3(2.0f, -1.0f, 0.0f);
    glm::vec3 colorPalm    = glm::vec3(0.2f, 0.7f, 0.2f); 
    CMesh cactus("../models/kaktus.obj");
    cactus.position = glm::vec3(-2.0f, -1.0f, 0.0f);
    glm::vec3 colorCactus  = glm::vec3(0.1f, 0.8f, 0.3f); 
	CMesh terrain("../models/terrain.obj");
    terrain.position = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 colorTerrain = glm::vec3(0.4f, 0.3f, 0.1f); 
    CMesh rock("../models/rock.obj");
    rock.position = glm::vec3(4.5f, -1.0f, 0.5f);
    glm::vec3 colorRock    = glm::vec3(0.5f, 0.5f, 0.5f); 

   





    std::vector<CMesh*> meshes = {
        &monkey,
        &palm,
        &cactus,
        &terrain,
        &rock
    };
    std::vector<glm::vec3> colors = {
        colorMonkey,
        colorPalm,
        colorCactus,
        colorTerrain,
        colorRock
    };

	// Inicjalizacja sceny
	Initialize();


	// Glowna petla
	while (!glfwWindowShouldClose(window))
	{
		// Obsluga zdarzen
		glfwPollEvents();
        float currentTime = glfwGetTime();
        glUseProgram(program.id);
        glUniform1f(glGetUniformLocation(program.id, "time"), currentTime);
        glUseProgram(0);
		// Render Sceny
		DisplayScene(program, meshes, colors);
        monkey.rotation.y += 0.01f;
        cactus.rotation.y -= 0.02f;
		//DisplayScene();
		glfwSwapBuffers(window);
	}



	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
