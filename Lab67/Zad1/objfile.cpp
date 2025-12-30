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
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// Bibliteka GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Okno aplikacji
int windowWidth = 1500, windowHeight = 900;
const char *windowTitle = "OpenGL w GLFW (obrot klawiszami WSAD oraz mysza)";

#include "utilities.hpp"
//#include "objloader.hpp"
#include "camera/Camera.hpp"
#include "object/Mesh.hpp"
#include "imgui/GUI/GUIManager.hpp"



glm::mat4 matProj;
glm::mat4 matModel;

float monkeyOrbitAngle = 0.0f;   // aktualny kąt ruchu
float monkeyOrbitRadius = 7.0f;  // promień okręgu
float monkeyOrbitSpeed = 0.01f;  // prędkość ruchu
Camera camera;

std::vector<glm::vec3> flowerVertices = {
    {-0.5f, 0.0f, 0.0f},
    { 0.5f, 0.0f, 0.0f},
    { 0.5f, 1.0f, 0.0f},

    {-0.5f, 0.0f, 0.0f},
    { 0.5f, 1.0f, 0.0f},
    {-0.5f, 1.0f, 0.0f},


    {0.0f, 0.0f, -0.5f},
    {0.0f, 0.0f,  0.5f},
    {0.0f, 1.0f,  0.5f},

    {0.0f, 0.0f, -0.5f},
    {0.0f, 1.0f,  0.5f},
    {0.0f, 1.0f, -0.5f},
};

std::vector<glm::vec2> flowerUV = {

    {0,0}, {1,0}, {1,1},
    {0,0}, {1,1}, {0,1},

    {0,0}, {1,0}, {1,1},
    {0,0}, {1,1}, {0,1},
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

void DisplayScene(CProgram& program,
                  const std::vector<Mesh*>& meshes,
                  const std::vector<glm::vec3>& colors) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 matView = camera.GetViewMatrix();
    program.SetMatrix("matView", matView);

    program.Use();
    program.SetMatrix("matProj", matProj);
    program.SetMatrix("matView", matView);

    for (int i = 0; i < meshes.size(); i++)
    {
        Mesh* mesh = meshes[i];

        program.SetMatrix("matModel", mesh->transform.GetMatrix());
        program.SetVec3("objectColor", colors[i]);

        mesh->Draw(program.getProgramID());
    }

    program.Stop();
}
//---------------------------------------
void Initialize()
{

	// Ustawianie koloru, ktorym bedzie czyszczony bufor koloru
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
         if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_ESCAPE) {
                printf("Nacisnieto klawisz %d \n", key);
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
                
            else {
                printf("Nacisnieto klawisz %d \n", key);
                camera.ProcessKeyboard(key);
            }
        }
	}
}

//obsługa scrolla
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessScroll(yoffset);
}

// obsługa kursora
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (__mouse_press && __mouse_button == GLFW_MOUSE_BUTTON_LEFT)
    {
        camera.ProcessMouseDrag(
            xpos - __mouse_buttonX,
            ypos - __mouse_buttonY,
            windowWidth,
            windowHeight
        );

        __mouse_buttonX = xpos;
        __mouse_buttonY = ypos;
    }
}


int main( int argc, char *argv[] )
{
	// Kontekst i okno aplikacji
    GLFWwindow* window = nullptr;
	Initialize_GLFW(window);

    // ImGui setup
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // ImGui::StyleColorsDark();
    // ImGui_ImplGlfw_InitForOpenGL(window, true);
    // ImGui_ImplOpenGL3_Init("#version 450"); 
    GUIManager gui(window);


 	CProgram program("shaders/vertex.glsl", "shaders/fragment.glsl");

    Mesh monkey("../models/monkey.obj");
    glm::vec3 monkeyCenter = glm::vec3(0.0f, 1.1f, -5.5f);
    monkey.transform.position = monkeyCenter;
    monkey.transform.scale = glm::vec3(2.0f);
    glm::vec3 colorMonkey;//  = glm::vec3(1.0f, 0.5f, 0.1f);
    monkey.LoadTexture("../assets/monkey.png"); 

    Mesh palm("../models/palm.obj");
    palm.transform.position = glm::vec3(2.0f, -1.0f, 0.0f);
    glm::vec3 colorPalm;//    = glm::vec3(0.2f, 0.7f, 0.2f); 
    palm.LoadTexture("../assets/palm.png");
    
    Mesh cactus("../models/kaktus.obj");
    cactus.transform.position = glm::vec3(-2.0f, -1.0f, 0.0f);
    glm::vec3 colorCactus;//  = glm::vec3(0.1f, 0.8f, 0.3f); 
    cactus.LoadTexture("../assets/kaktus.jpg");
	
    Mesh terrain("../models/terrain.obj");
    terrain.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 colorTerrain;// = glm::vec3(0.4f, 0.3f, 0.1f); 
    terrain.LoadTexture("../assets/terrain.jpg");
    
    Mesh rock("../models/rock.obj");
    rock.transform.position = glm::vec3(4.5f, -1.0f, 0.5f);
    glm::vec3 colorRock;//    = glm::vec3(0.5f, 0.5f, 0.5f); 
    rock.LoadTexture("../assets/rock.png");


    std::vector<Mesh*> meshes = {
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


    Mesh* flowerModel = new Mesh(flowerVertices, flowerUV);
    flowerModel->LoadTexture("../assets/flower32bit.png");

    std::vector<Mesh*> flowerMeshes;

    for (int i = 0; i < 20; i++)
    {
        Mesh* f = new Mesh(*flowerModel); 
        f->transform.position = glm::vec3((rand() % 50 - 10) / 2.0f, -1.0f, (rand() % 50 - 10) / 2.0f);
        float angle = (rand() % 360) * 1.0f; 
        f->transform.rotation = glm::vec3(0.0f, glm::radians(angle), 0.0f);
        float s = 1.5f + (rand() / (float)RAND_MAX) * 2.0f;
        f->transform.scale = glm::vec3(s);
        flowerMeshes.push_back(f);
    }

    meshes.insert(meshes.end(), flowerMeshes.begin(), flowerMeshes.end());


	Initialize();


	// Glowna petla
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
        glUseProgram(program.getProgramID());
        glUseProgram(0);

        gui.StartFrame();
        gui.RenderFrame();

		DisplayScene(program, meshes, colors);
        
        gui.EndFrame();
        // // ImGui
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
        // ImGui::Begin("Kontrola sceny");
        // ImGui::Text("Przykładowe parametry:");
        // ImGui::SliderFloat("Kąt obrotu małpy", &monkeyOrbitAngle, 0.0f, 6.28f);
        // ImGui::End();
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        monkeyOrbitAngle = gui.monkeyAngle;

        monkey.transform.rotation.y += 0.03f;
        //monkeyOrbitAngle += monkeyOrbitSpeed;
        monkey.transform.position.x = monkeyCenter.x + cos(monkeyOrbitAngle) * monkeyOrbitRadius;
        monkey.transform.position.z = monkeyCenter.z + sin(monkeyOrbitAngle) * monkeyOrbitRadius;
        cactus.transform.rotation.y -= 0.02f;
		glfwSwapBuffers(window);
	}


	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

    // ImGui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
