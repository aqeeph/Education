#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int win_width = 512, win_height = 512;

void cati(){
	glColor3f(1,0,0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-3.25,0,0);
		glVertex3f(0,4,0);
		glVertex3f(3.25,0,0);
	glEnd();
}

void bina(){
	glColor3f(0,0,1);
	glBegin(GL_QUADS);
		glVertex3f(-3,0,0);
		glVertex3f(3,0,0);
		glVertex3f(3,-5,0);
		glVertex3f(-3,-5,0);
	glEnd();
}

void kapi(){
	glColor3f(1,0.8,0);
	glBegin(GL_QUADS);
		glVertex3f(-1,-3,0);
		glVertex3f(1,-3,0);
		glVertex3f(1,-5,0);
		glVertex3f(-1,-5,0);
	glEnd();
}

void eviCiz()
{
	bina();
	cati();
	kapi();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    eviCiz();
    glutSwapBuffers();
}

void reshape( int w, int h )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    //Ortographic projection nedir?
    glOrtho( -10., 10., -10., 10., -10., 10. );
    //Diger projection komutlari ve cesitleri?
    //Asagidaki kod ne is yariyor?
    glViewport( 0, 0, w, h );
    win_width = w;
    win_height = h;
    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
    switch (key)
    {
    case 27: // ESC tusu
        exit(0);
        break;
    }
}

int main (int argc, char *argv[])
{
    glutInit( &argc, argv );
    //Asagidaki cagrim parcasi ne ise yariyor?
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( win_width, win_height );
    glutCreateWindow( "Basit Ev Cizimi" );
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMainLoop();
}

