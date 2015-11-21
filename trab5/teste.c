#include <GL/glut.h>
#include <stdio.h>

GLfloat angle, fAspect;

GLfloat obsX=0, obsY=0, obsZ=200; //acrescente esta linha

/* Função callback chamada para fazer o desenho
 * Largura: X
 * Comprimento: Y
 * Altura: Z
*/
void desenhaCaixa( GLfloat *centro, GLfloat largura, GLfloat comprimento, GLfloat altura)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 1.0f);

    // Desenha um cubo


    glBegin(GL_QUADS);			// Face posterior
    glNormal3f(0.0, 0.0, 1.0);	// Normal da face
    glVertex3f(centro[0] + largura/2, centro[1] + comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] + comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] - comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] - comprimento/2, centro[2] + altura/2);
    glEnd();


    glBegin(GL_QUADS);			// Face frontal
    glNormal3f(0.0, 0.0, -1.0); 	// Normal da face
    glVertex3f(centro[0] + largura/2, centro[1] + comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] - comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] - comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] + comprimento/2, centro[2] - altura/2);
    glEnd();

    glBegin(GL_QUADS);			// Face lateral esquerda
    glNormal3f(-1.0, 0.0, 0.0); 	// Normal da face
    glVertex3f(centro[0] - largura/2, centro[1] + comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] + comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] - comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] - comprimento/2, centro[2] + altura/2);
    glEnd();

    glBegin(GL_QUADS);			// Face lateral direita
    glNormal3f(1.0, 0.0, 0.0);	// Normal da face
    glVertex3f(centro[0] + largura/2, centro[1] + comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] - comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] - comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] + comprimento/2, centro[2] - altura/2);
    glEnd();

    glBegin(GL_QUADS);			// Face superior
    glNormal3f(0.0, 1.0, 0.0);  	// Normal da face
    glVertex3f(centro[0] - largura/2, centro[1] + comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] + comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] + comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] + comprimento/2, centro[2] - altura/2);
    glEnd();

    glBegin(GL_QUADS);			// Face inferior
    glNormal3f(0.0, -1.0, 0.0); 	// Normal da face
    glVertex3f(centro[0] - largura/2, centro[1] - comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] - comprimento/2, centro[2] - altura/2);
    glVertex3f(centro[0] + largura/2, centro[1] - comprimento/2, centro[2] + altura/2);
    glVertex3f(centro[0] - largura/2, centro[1] - comprimento/2, centro[2] + altura/2);
    glEnd();
    glutSwapBuffers();
}

void desenha(){
    GLfloat centro [3] = {0,0,0};
    desenhaCaixa(centro, 20.0, 60.0, 80);
}

void SpecialKeys(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_LEFT :
            obsX -=10;
            break;
        case GLUT_KEY_RIGHT :
            obsX +=10;
            break;
        case GLUT_KEY_UP :
            obsY +=10;
            break;
        case GLUT_KEY_DOWN :
            obsY -=10;
            break;

        case GLUT_KEY_HOME :
            obsZ +=10;
            break;
        case GLUT_KEY_END :
            obsZ -=10;
            break;
    }
    glLoadIdentity();
    gluLookAt(obsX,obsY,obsZ, 0,0,0, 0,1,0);
    glutPostRedisplay();
}

// Inicializa parâmetros de rendering
void Inicializa (void)
{
    GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};
    GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};	   // "cor"
    GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho"
    GLfloat posicaoLuz[4]={0.0, 50.0, 50.0, 1.0};

    // Capacidade de brilho do material
    GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
    GLint especMaterial = 60;

    // Especifica que a cor de fundo da janela será preta
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Habilita o modelo de colorização de Gouraud
    glShadeModel(GL_SMOOTH);

    // Define a refletância do material
    glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
    // Define a concentração do brilho
    glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

    // Ativa o uso da luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    // Define os parâmetros da luz de número 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

    // Habilita a definição da cor do material a partir da cor corrente
    glEnable(GL_COLOR_MATERIAL);
    //Habilita o uso de iluminação
    glEnable(GL_LIGHTING);
    // Habilita a luz de número 0
    glEnable(GL_LIGHT0);
    // Habilita o depth-buffering
    glEnable(GL_DEPTH_TEST);

    angle=45;
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(void)
{
    // Especifica sistema de coordenadas de projeção
    glMatrixMode(GL_PROJECTION);
    // Inicializa sistema de coordenadas de projeção
    glLoadIdentity();

    // Especifica a projeção perspectiva
    gluPerspective(angle,fAspect,0.4,500);

    // Especifica sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);
    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // Especifica posição do observador e do alvo
    gluLookAt(obsX,obsY,obsZ, 0,0,0, 0,1,0);
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    // Para previnir uma divisão por zero
    if ( h == 0 ) h = 1;

    // Especifica o tamanho da viewport
    glViewport(0, 0, w, h);

    // Calcula a correção de aspecto
    fAspect = (GLfloat)w/(GLfloat)h;

    EspecificaParametrosVisualizacao();
}

// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    if (state == GLUT_DOWN) {  // Zoom-in
        if (angle >= 10) angle -= 5;
    }
    if (button == GLUT_RIGHT_BUTTON)
    if (state == GLUT_DOWN) {  // Zoom-out
        if (angle <= 130) angle += 5;
    }
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}


// Programa Principal
int main(int argc, char **argv)
{

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500,500);

    glutCreateWindow("Visualizacao 3D");
    glutDisplayFunc(desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutSpecialFunc(SpecialKeys);
    glutMouseFunc(GerenciaMouse);
    Inicializa();
    glutMainLoop();
}
