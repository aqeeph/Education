#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int angle = 0;
int position = 0;

int animation = 0;
int eyeX = 0;
int eyeY = 0;
int eyeZ = 10;

void cati()
{
    glPushMatrix();
    glColor3f(1,0,0);
    glTranslatef(8,0,0);
    glRotatef(90,-1,0,0);
    glutSolidCone(6,6,20,20);
    glPopMatrix();
}

void bina()
{
    glPushMatrix();
    glColor3f(0,0,1);
    glTranslatef(8,-4,0);
    glutSolidCube(8);
    glPopMatrix();
}

void kapi()
{
    glColor3f(1,0.8,0);
    glBegin(GL_QUADS);
    glVertex3f(7.5,-5,0);
    glVertex3f(9,-5,0);
    glVertex3f(9,-8,0);
    glVertex3f(7.5,-8,0);
    glEnd();
}

void eviCiz()
{
    bina();
    cati();
    kapi();
}

void govde()
{
    glPushMatrix();
    glColor3f(0.8,0.8,0.8);
    glTranslatef(-15,0,0);
    glRotatef(90,1,0,0);
    GLUquadric *quadric;
    quadric = gluNewQuadric();
    gluCylinder(quadric, 2, 2,8,20,20);
    glEnd();
    glPopMatrix();
}

void kanatlar()
{
    glColor3f(1,0,0);
    glPushMatrix();
    glTranslatef(-15,2,0);
    glRotatef(angle,0,0,1);
    glutSolidCube(5);
    glRotatef(45,0,0,1);
    glutSolidCube(5);
    glPopMatrix();
}

void degirmeniCiz()
{
    govde();
    kanatlar();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    //gluLookAt nedir??
    gluLookAt(eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0);
    eviCiz();
    degirmeniCiz();
    glutSwapBuffers();
}

void reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -20., 20., -20., 20., -200, 200. );
    glMatrixMode(GL_MODELVIEW);
}

// timer ve idle fonksiyonları ne işe yarar?
void timer(int id)
{
    if (animation == 1)
    {
        angle+=1;
        glutPostRedisplay();
    }
    glutTimerFunc(10, timer, 0);
}

void keyboard( unsigned char key, int x, int y )
{
    switch (key)
    {
    case 27: // ESC tusu
        exit(0);
        break;
    case 'a': // gözün hareket ettirilmesi
        eyeX -= 1;
        break;
    case 'd':
        eyeX += 1;
        break;
    case 'w':
        eyeY+= 1;
        break;
    case 's':
        eyeY -= 1;
        break;
    case 'z':
        eyeX=0;
        eyeY=0;
        break;
    case 'i': // animasyon modu aktif
        if ( animation == 0)
            animation = 1;
        else animation = 0;
        break;
    }
    glutPostRedisplay();
}

int main (int argc, char *argv[])
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( 512,512 );
    glutCreateWindow( "Basit Ev ve Değirmen Cizimi" );
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutTimerFunc(10,timer,0);
    glutMainLoop();
}

