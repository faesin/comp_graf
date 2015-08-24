#include "renderer.h"

void displayCallback(void)
{
	glEnd();
	glFlush();
}

void reshapeCallback(int w, int h)
{

}

Renderer::Renderer()
{
	char param[] = "swag";
	char *fakeargv[] = { param, NULL };
	int fakeargc = 1;

	glutInit(&fakeargc, fakeargv);

	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Trabalho 1");

	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);

}

void Renderer::run()
{
	glutMainLoop();
}