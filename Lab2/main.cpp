#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
using namespace std;
// Funkcje pomocnicze
#include "utilities.hpp"
#include "house.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL

GLuint idProgram;	// poroku/programu
GLuint idVAO;		// tablicy wierzcholkow
GLuint idVAO_scene1; //tablica wierzchołków scena 1
GLuint idVBO_coord; // bufora na wspolrzedne wierzcholkow
GLuint idVBO_color; // bufora na kolory wierzcholkow
GLuint idVBO_color_grad; //bufora na kolory wierzchołków z gradientem
//Zmienne scena 3
GLuint idProgram_scene3;
GLuint idVAO_scene3;
GLuint idVBO_scene3;

//Scena
int sceneNumber = 0;
const int sceneAmount = 3;
//Liczba obiektów
const int N = 100;
//Wektor na dane wierzchołków
vector<GLfloat> vertexData;
vector<GLfloat> vertexColorScene1;
vector<GLfloat> vertexColorScene2;
float houseOffsetX = 0.0f;
float houseOffsetY = 0.0f;

const float step = 0.05f;

void RenderScene(GLuint idProgram, GLuint idVAO) {
	glClear( GL_COLOR_BUFFER_BIT );
	//Potok graficzny
	glUseProgram( idProgram );
	//Dane o geometrii
	glBindVertexArray( idVAO );
	//Rendering
	glDrawArrays( GL_TRIANGLES, 0, vertexData.size()/2 );
	//Dezaktywacja VAO
	glBindVertexArray( 0 );
	glUseProgram( 0 );
	glutSwapBuffers();
}

void DisplayScene()
{
	switch (sceneNumber) {
		case 0:
			glutSetWindowTitle("Scena1");
			RenderScene(idProgram, idVAO);
			break;
		case 1:
			glutSetWindowTitle("Scena2");
			RenderScene(idProgram, idVAO_scene1);
			break;
		case 2:
			glutSetWindowTitle("Scena3");
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(idProgram_scene3);
            glBindVertexArray(idVAO_scene3);

            GLint locOffset = glGetUniformLocation(idProgram_scene3, "uOffset");
            glUniform2f(locOffset, houseOffsetX, houseOffsetY);

            glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);

            glBindVertexArray(0);
            glUseProgram(0);
            glutSwapBuffers();
			break;
	}
}
void InitializeScene0(){
	for (int i = 0; i < N; i++) {
		// losowy środek kwadratu
		float cx = (rand() % 1000) / 500.0f - 1.0f;
		float cy = (rand() % 1000) / 500.0f - 1.0f;

		// losowy rozmiar
		float s = 0.1f + (rand() % 100) / 500.0f; 

		// wierzchołki kwadratu 
		float x1 = cx - s, y1 = cy - s;
		float x2 = cx + s, y2 = cy - s;
		float x3 = cx + s, y3 = cy + s;
		float x4 = cx - s, y4 = cy + s;

		// Dwa trójkąty: (1,2,3) i (1,3,4)
		vertexData.insert(vertexData.end(), {
			x1, y1,
			x2, y2,
			x3, y3,

			x1, y1,
			x3, y3,
			x4, y4
		});
	}
	for(int i = 0; i<N; i++) {
		GLfloat r = (rand()%1000)/1000.0;
		GLfloat g = (rand()%1000)/1000.0;
		GLfloat b = (rand()%1000)/1000.0;

		for (int j = 0; j < 6; j++) {
			vertexColorScene1.push_back(r); // r
			vertexColorScene1.push_back(g); // g
			vertexColorScene1.push_back(b); // b
		}
	}

	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholk�w do OpenGL
	// -------------------------------------------------

	// Tworzymy tablice wierzcholkow (VAO)
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );
	// Tworzymy bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
	glBufferData( GL_ARRAY_BUFFER, vertexData.size()*sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW );
	// Ustawienie danych bufora pod atrybut o numerze 0
	// ten sam ustawiony w vertex shaderze
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// Tworzymy bufor na kolory wierzcholkow 
	glGenBuffers( 1, &idVBO_color);
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color);
	glBufferData( GL_ARRAY_BUFFER, vertexColorScene1.size()*sizeof(GLfloat), vertexColorScene1.data(), GL_STATIC_DRAW );
	// Ustawienie danych bufora pod atrybut o numerze 1
	// ten sam ustawiony w vertex shaderze
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
	glBindVertexArray( 0 );
}
void InitializeScene1(){

	 for (int i = 0; i < N; i++) {
        GLfloat r1 = (rand()%1000)/1000.0f;
        GLfloat g1 = (rand()%1000)/1000.0f;
        GLfloat b1 = (rand()%1000)/1000.0f;

        GLfloat r2 = (rand()%1000)/1000.0f;
        GLfloat g2 = (rand()%1000)/1000.0f;
        GLfloat b2 = (rand()%1000)/1000.0f;

        GLfloat r3 = (rand()%1000)/1000.0f;
        GLfloat g3 = (rand()%1000)/1000.0f;
        GLfloat b3 = (rand()%1000)/1000.0f;

        GLfloat r4 = (rand()%1000)/1000.0f;
        GLfloat g4 = (rand()%1000)/1000.0f;
        GLfloat b4 = (rand()%1000)/1000.0f;

        vertexColorScene2.insert(vertexColorScene2.end(), {
            r1,g1,b1,  r2,g2,b2,  r3,g3,b3,
            r1,g1,b1,  r3,g3,b3,  r4,g4,b4
        });
    }

	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholk�w do OpenGL
	// -------------------------------------------------

	// Tworzymy tablice wierzcholkow (VAO)
	glGenVertexArrays( 1, &idVAO_scene1 );
	glBindVertexArray( idVAO_scene1 );
	// Przypisanie idVBO_cord ze sceny0
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
	// Ustawienie danych bufora pod atrybut o numerze 0
	// ten sam ustawiony w vertex shaderze
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// Tworzymy bufor na kolory wierzcholkow 
	glGenBuffers( 1, &idVBO_color_grad);
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color_grad);
	glBufferData( GL_ARRAY_BUFFER, vertexColorScene2.size()*sizeof(GLfloat), vertexColorScene2.data(), GL_STATIC_DRAW );
	// Ustawienie danych bufora pod atrybut o numerze 1
	// ten sam ustawiony w vertex shaderze
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
	glBindVertexArray( 0 );
}
void InitializeScene2() {
	glGenVertexArrays(1, &idVAO_scene3);
    glBindVertexArray(idVAO_scene3);

    glGenBuffers(1, &idVBO_scene3);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO_scene3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh_Vertices), Mesh_Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
void Initialize()
{
	srand(time(NULL));
	InitializeScene0();	
	InitializeScene1();
	InitializeScene2();
		// ---------------------------------------
	// Etap (3) stworzenie potoku graficznego
	// ---------------------------------------
	idProgram = glCreateProgram();
		glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
		glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );

	idProgram_scene3= glCreateProgram();
    glAttachShader(idProgram_scene3, LoadShader(GL_VERTEX_SHADER, "vertex_scene3.glsl"));
    glAttachShader(idProgram_scene3, LoadShader(GL_FRAGMENT_SHADER, "fragment_scene3.glsl"));
    LinkAndValidateProgram(idProgram_scene3);
	// -----------------------------------------
	// Etap (4) ustawienie maszyny stanow OpenGL
	// -----------------------------------------

	// ustawienie koloru czyszczenia ramki koloru
	glClearColor( 0.9f, 0.9f, 0.9f, 0.9f );
}

// ---------------------------------------------------
// Funkcja wywolywana podczas tworzenia okna aplikacji
// oraz zmiany jego rozdzielczosci
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
}



// ---------------------------------------------------
// Funkcja wywolywana podczas wcisniecia klawisza ASCII
void Keyboard( unsigned char key, int x, int y )
{
	
    switch(key)
    {
		case 27:	// ESC key
			// opuszczamy glowna petle
			glutLeaveMainLoop();
			break;

		case ' ':	// Space key
			sceneNumber = (sceneNumber + 1) % sceneAmount;
			printf("Scena numer %d!\n", sceneNumber + 1);
			// ponowne renderowanie
			glutPostRedisplay();
			break;

		case 'x':
			// opuszczamy glowna petle
			glutLeaveMainLoop();
			break;
		case 'w': case 'W':
            if (sceneNumber == 2) houseOffsetY += step; break;
        case 's': case 'S':
            if (sceneNumber == 2) houseOffsetY -= step; break;
        case 'a': case 'A':
            if (sceneNumber == 2) houseOffsetX -= step; break;
        case 'd': case 'D':
            if (sceneNumber == 2) houseOffsetX += step; break;
    }
	glutPostRedisplay();
}


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// -----------------------------------------------
	// Etap (1) utworzynie kontektu OpenGLa i okna
	// aplikacji, rejestracja funkcji zwrotnych
	// -----------------------------------------------

	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Szablon programu w OpenGL" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutKeyboardFunc( Keyboard );


	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err ) {
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_3 ) {
		printf("Brak OpenGL 3.3!\n");
		exit(1);
	}


	// Inicjalizacja
	Initialize();

	// Rendering
	glutMainLoop();


	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );
	glDeleteVertexArrays( 1, &idVAO_scene1 );
	glDeleteProgram(idProgram_scene3);
	glDeleteBuffers(1, &idVBO_scene3);
	glDeleteVertexArrays(1, &idVAO_scene3);
	return 0;
}
