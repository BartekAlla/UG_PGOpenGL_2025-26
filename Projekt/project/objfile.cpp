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
const char *windowTitle = "OpenGL w GLFW";

#include "utilities.hpp"
#include "camera/Camera.hpp"
#include "object/Mesh.hpp"
#include "object/InstancedMesh.hpp"
#include "imgui/GUI/GUIManager.hpp"
#include "skybox/CSkyBox.hpp"


glm::mat4 matProj;
glm::mat4 matModel;

float monkeyOrbitAngle = 0.0f;   // aktualny kąt ruchu
float monkeyOrbitRadius = 10.0f;  // promień okręgu
float monkeyOrbitSpeed = 0.01f;  // prędkość ruchu
Camera camera;
glm::vec3 lightBasePos = glm::vec3(2.0f, 8.0f, 2.0f);
//float lightAngle = 0.0f;

struct Light
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 attenuation; 
    glm::vec3 position;
};

Light pointLight = {
    glm::vec3(0.1f),                 // ambient
    glm::vec3(1.0f),                 // diffuse
    glm::vec3(1.0f),                 // specular
    glm::vec3(1.0f, 0.09f, 0.032f),  // attenuation
    glm::vec3(2.0f, 5.0f, 2.0f)      // position
};

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
    void SetLight(const std::string& name,
                const glm::vec3& ambient,
                const glm::vec3& diffuse,
                const glm::vec3& specular,
                const glm::vec3& attenuation,
                const glm::vec3& position)
    {
        SetVec3((name + ".Ambient").c_str(), ambient);
        SetVec3((name + ".Diffuse").c_str(), diffuse);
        SetVec3((name + ".Specular").c_str(), specular);
        SetVec3((name + ".Attenuation").c_str(), attenuation);
        SetVec3((name + ".Position").c_str(), position);
    }

    void SetMaterial(const char* name, const Material& mat)
    {
        SetVec3((std::string(name) + ".Ambient").c_str(), mat.ambient);
        SetVec3((std::string(name) + ".Diffuse").c_str(), mat.diffuse);
        SetVec3((std::string(name) + ".Specular").c_str(), mat.specular);
        SetFloat((std::string(name) + ".Shininess").c_str(), mat.shininess);
    }

    void SetBool(const char* name, bool v)
    {
        glUniform1i(glGetUniformLocation(id, name), v ? 1 : 0);
    }
};

void DisplayScene(
    CProgram& program,
    const std::vector<Mesh*>& meshes,
    const std::vector<glm::vec3>& colors,
    GUIManager& gui,
    const Light& light,
    Mesh* lightSphere,
    const CSkyBox& activeSkybox  
)   
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- MACIERZ WIDOKU I POZYCJA KAMERY ---
    glm::mat4 matView = camera.GetViewMatrix();
    activeSkybox.Render(matProj, matView);
    glm::vec3 cameraPos = glm::vec3(glm::inverse(matView)[3]);

    program.Use();

    // --- MACIERZE ---
    program.SetMatrix("matProj", matProj);
    program.SetMatrix("matView", matView);
    program.SetVec3("cameraPos", cameraPos);
    // --- USTAWIENIE SWIATLA PUNKTOWEGO ---
    program.SetVec3("myLight.Ambient",     light.ambient);
    program.SetVec3("myLight.Diffuse",     light.diffuse);
    program.SetVec3("myLight.Specular",    light.specular);
    program.SetVec3("myLight.Attenuation", light.attenuation);
    program.SetVec3("myLight.Position",    light.position);
    
    // --- STEROWANIE Z IMGUI ---
    program.SetBool("enablePointLight", gui.enablePointLight);
    program.SetFloat("lightingModel", (float)gui.lightingModel);
    program.SetFloat("lightIntensity", gui.lightIntensity);
    program.SetVec3("myLight.Position", light.position);
    // --- RYSOWANIE OBIEKTÓW ---
    program.SetBool("isLightSphere", false);
    for (size_t i = 0; i < meshes.size(); i++)
    {
        program.SetMatrix("matModel", meshes[i]->transform.GetMatrix());
        program.SetVec3("objectColor", colors[i]);
        program.SetMaterial("myMaterial", meshes[i]->material);
        meshes[i]->Draw(program.getProgramID());
    }
     // --- rysowanie sfery światła ---
    program.SetBool("isLightSphere", true);
    glm::mat4 lightMat = glm::mat4(1.0f);
    lightMat = glm::translate(lightMat, lightSphere->transform.position);
    lightMat = glm::scale(lightMat, lightSphere->transform.scale);
    program.SetMatrix("matModel", lightMat);
    lightSphere->Draw(program.getProgramID());

    program.SetBool("isLightSphere", false);
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
		matProj = glm::perspective(glm::radians(80.0f), windowWidth/(float)windowHeight, 0.1f, 500.0f);
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
	matProj = glm::perspective(glm::radians(70.0f), width/(float)height, 0.1f, 500.0f );

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
    if (__mouse_press && __mouse_button == GLFW_MOUSE_BUTTON_RIGHT)
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

    GUIManager gui(window);

    CProgram mosquitoProg("shaders/mosquito_inst_vert.glsl", "shaders/mosquito_inst_frag.glsl");

 	CProgram program("shaders/vertex.glsl", "shaders/fragment.glsl");

    Mesh monkey("../models/monkey.obj");
    glm::vec3 monkeyCenter = glm::vec3(0.0f, 1.1f, -5.5f);
    monkey.transform.position = monkeyCenter;
    monkey.transform.scale = glm::vec3(2.0f);
    glm::vec3 colorMonkey;//  = glm::vec3(1.0f, 0.5f, 0.1f);
    monkey.LoadTexture("../assets/monkey.png"); 
    monkey.material.diffuse  = glm::vec3(1.0f);   
    monkey.material.specular = glm::vec3(1.0f);  
    monkey.material.shininess = 128.0f; 

    Mesh tower("../models/tower.obj");
    tower.transform.position = glm::vec3(2.0f, -5.0f, -5.0f);
    tower.transform.scale = glm::vec3(5.0f);
    glm::vec3 colorTower;//    = glm::vec3(0.2f, 0.7f, 0.2f); 
    tower.LoadTexture("../assets/metal.png");
    tower.material.diffuse  = glm::vec3(1.0f);  
    tower.material.specular = glm::vec3(1.0f);   
    tower.material.shininess = 96.0f;            
    
    Mesh cactus("../models/kaktus.obj");
    cactus.transform.position = glm::vec3(-2.0f, -1.0f, 0.0f);
    glm::vec3 colorCactus;//  = glm::vec3(0.1f, 0.8f, 0.3f); 
    cactus.LoadTexture("../assets/cactus.png");
    cactus.material.diffuse  = glm::vec3(1.0f);
    cactus.material.specular = glm::vec3(0.02f); 
    cactus.material.shininess = 3.0f;            
	
    Mesh terrain("../models/terrain.obj");
    terrain.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 colorTerrain;// = glm::vec3(0.4f, 0.3f, 0.1f); 
    terrain.LoadTexture("../assets/terrain.png");
    terrain.material.diffuse  = glm::vec3(1.0f);
    terrain.material.specular = glm::vec3(0.0f); 
    terrain.material.shininess = 1.0f;
    
    Mesh rock("../models/rock.obj");
    rock.transform.position = glm::vec3(4.5f, -1.0f, 0.5f);
    glm::vec3 colorRock;//    = glm::vec3(0.5f, 0.5f, 0.5f); 
    rock.LoadTexture("../assets/rock.png");
    rock.material.diffuse  = glm::vec3(0.7f);
    rock.material.specular = glm::vec3(0.05f);  
    rock.material.shininess = 4.0f;            


    Mesh* lightSphere = new Mesh("../models/sphere.obj");
    lightSphere->transform.position = pointLight.position;
    lightSphere->transform.scale = glm::vec3(1.0f);   
    glm::vec3 colorLightSphere = glm::vec3(1.0f, 1.0f, 0.0f);


    std::vector<Mesh*> meshes = {
        &monkey,
        &tower,
        &cactus,
        &terrain,
        &rock
    };
    std::vector<glm::vec3> colors = {
        colorMonkey,
        colorTower,
        colorCactus,
        colorTerrain,
        colorRock
    };


    // Mesh* flowerModel = new Mesh(flowerVertices, flowerUV);
    // flowerModel->LoadTexture("../assets/flower32bit.png");

    // std::vector<Mesh*> flowerMeshes;

    // for (int i = 0; i < 20; i++)
    // {
    //     Mesh* f = new Mesh(*flowerModel); 
    //     f->transform.position = glm::vec3((rand() % 50 - 10) / 2.0f, -1.0f, (rand() % 50 - 10) / 2.0f);
    //     float angle = (rand() % 360) * 1.0f; 
    //     f->transform.rotation = glm::vec3(0.0f, glm::radians(angle), 0.0f);
    //     float s = 1.5f + (rand() / (float)RAND_MAX) * 2.0f;
    //     f->transform.scale = glm::vec3(s);
    //     flowerMeshes.push_back(f);
    // }

    // meshes.insert(meshes.end(), flowerMeshes.begin(), flowerMeshes.end());


	Initialize();

    CMultipleMesh mosquitoes;
    const size_t MOSQ_MAX = 100000;

    mosquitoes.InitQuadInstanced(MOSQ_MAX);
    mosquitoes.LoadTexture("../assets/mosquito.png");

    // ustaw początkową liczbę (np. z GUI)
    mosquitoes.SetActiveCount(
        (size_t)gui.mosquitoCount,
        terrain.transform.position,
        20.0f,                       // areaHalfSize
        1.0f,                        // yMin
        8.0f                         // yMax
    );



    

    CSkyBox skyA;
    CSkyBox skyB;

    // ścieżki do shaderów:
    const std::string skyVS = "shaders/skybox-vertex.glsl";
    const std::string skyFS = "shaders/skybox-fragment.glsl";

    // jeśli masz katalogi z teksturami:
    skyA.InitFromDirectory("skybox/forest", skyVS, skyFS);
    skyB.InitFromDirectory("skybox/nearLake",  skyVS, skyFS);

	// Glowna petla
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
        glUseProgram(program.getProgramID());
        glUseProgram(0);

        gui.StartFrame();
        gui.RenderFrame();

        static float lastTime = (float)glfwGetTime();
        float now = (float)glfwGetTime();
        float dt = now - lastTime;
        lastTime = now;
        static int lastCount = -1;
        if (gui.mosquitoCount != lastCount)
        {
            mosquitoes.SetActiveCount(
                (size_t)gui.mosquitoCount,
                terrain.transform.position,
                20.0f,
                1.0f,
                8.0f
            );
            lastCount = gui.mosquitoCount;
        }

        mosquitoes.Update(dt, 20.0f, 1.0f, 8.0f);
        glm::vec3 monkeyPos = monkey.transform.position;

        // promień “dotyku” (dopasuj)
        float killRadius = 1.5f;
        // usuń komary, które “dotknęła” małpa (po XZ)
        int removed = mosquitoes.RemoveColliding(monkeyPos, killRadius, true);

        if (removed > 0)
        {
            gui.mosquitoCount = (int)mosquitoes.ActiveCount();
        }
        //glm::vec3 monkeyPos = monkey.transform.position;

        // promienie w jednostkach świata — dopasuj do sceny:
        float monkeyRadius = 1.5f;
        float mosquitoRadius = 0.2f;

        // jeśli chcesz tylko XZ (bez wysokości), w RemoveInSphere ustaw d.y = 0
        int killed = mosquitoes.RemoveInSphere(monkeyPos, monkeyRadius + mosquitoRadius);
        if (killed > 0)
            std::cout << "Killed mosquitoes: " << killed << "\n";
        const CSkyBox& activeSky = (gui.skyboxIndex == 0) ? skyA : skyB;
		DisplayScene(program, meshes, colors,gui, pointLight, lightSphere, activeSky);
        glm::mat4 matView = camera.GetViewMatrix();
        mosquitoes.DrawInstanced(mosquitoProg.getProgramID(), matProj, matView);
        gui.EndFrame();
        

        monkey.transform.rotation.y += 0.03f;
        monkeyOrbitAngle += monkeyOrbitSpeed;
        monkey.transform.position.x = monkeyCenter.x + cos(monkeyOrbitAngle) * monkeyOrbitRadius;
        monkey.transform.position.z = monkeyCenter.z + sin(monkeyOrbitAngle) * monkeyOrbitRadius;
        cactus.transform.rotation.y -= 0.02f;

        float currentTime = glfwGetTime();

        if (gui.animateLight)
        {
            float radius = 6.0f;

            pointLight.position.x = cos(currentTime * gui.lightAnimSpeed) * radius;
            pointLight.position.z = sin(currentTime * gui.lightAnimSpeed) * radius;
            lightSphere->transform.position = pointLight.position;
        }


		glfwSwapBuffers(window);
	}


	glfwDestroyWindow(window);
	glfwTerminate();

    // for (Mesh* m : flowerMeshes)
    //     delete m;

    // delete flowerModel;
	exit(EXIT_SUCCESS);

}
