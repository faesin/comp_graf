#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_

#include <GL/gl.h>
#include <GL/glut.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;

class Vertec
{
public:
	//Vertex coordinate
	float X;
	float Y;
	float Z;
    
	//Vertex normal 
	float nX;
	float nY;
	float nZ;
    
	//Vertex texture coordinate
	float U;
	float V;

	Vertec();
	Vertec(float x, float y, float z, float u, float v, float nx, float ny, float nz);

}; 

class Mesh{
	vector<Vertec> verts;//Lista ordenada de vertices para o desenho do modelo
	GLuint meshTex;//Idex de textura carregada via imageLoader
	
public:
	Mesh();
	bool loadMesh(string path);//Path é o caminho para o arquivo .obj e deve conter o nome do arquivo.obj
	void loadMeshTex(GLuint mtx) { meshTex = mtx; };//mtx é o índice da textura carregada em OpenGL através do ImageLoader
	void draw();
	//~mesh();
};

#endif
