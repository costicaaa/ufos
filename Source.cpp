#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include "SOIL.h"
enum {
	NONE, SHADOW
};

int rendermode = NONE;

void menu(int mode)
{
	rendermode = mode;
	glutPostRedisplay();
}

enum {
	FLYING_UFO = 1, UFO_SPHERE, UFO_DISK
};

enum {
	X, Y, Z, W
};
enum {
	A, B, C, D
};


GLfloat floorshadow[4][4];
GLfloat lightpos[] = { -100.f, 80.f, -800.f, 1.f }; // change 800 to 500 for presentation 
GLfloat shadowpos[] = { 70.f, -55.f, -600.f, 1.f }; 

/* Mats colors */
static GLfloat grass_mat[] = { 1.f, 1.f, 1.f, 1.f };
static GLfloat landing_beacon_mat[] = { 0.f, 0.3f, .4f, 1.f };
static GLfloat ufo_sphere_mat[] = { 0.f, 0.f, 0.2f, 1.f };
static GLfloat ufo_disk_mat[] = { 0.f, 0.f, 0.4f, 1.f };

/* Calculul matricii pentru umbre */
void shadowmatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4])
{
	GLfloat dot;

	dot = groundplane[X] * lightpos[X] +
		groundplane[Y] * lightpos[Y] +
		groundplane[Z] * lightpos[Z] +
		groundplane[W] * lightpos[W];

	shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
	shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
	shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
	shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];

	shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
	shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
	shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
	shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];

	shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
	shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
	shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
	shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];

	shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
	shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
	shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
	shadowMat[3][3] = dot - lightpos[W] * groundplane[W];
}

/* Ecuatia planului cu ajutorul a 3 puncte */
void findplane(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
	GLfloat vec0[3], vec1[3];

	vec0[X] = v1[X] - v0[X];
	vec0[Y] = v1[Y] - v0[Y];
	vec0[Z] = v1[Z] - v0[Z];

	vec1[X] = v2[X] - v0[X];
	vec1[Y] = v2[Y] - v0[Y];
	vec1[Z] = v2[Z] - v0[Z];

	plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
	plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
	plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];

	plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}

void LoadTexture(const char *texture_name)
{
	GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height;
	unsigned char* image = SOIL_load_image(texture_name, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}

void drawTextureGrass() {
	LoadTexture("iarba.jpg");
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-500.f, -100.f, -320.f);
	glTexCoord2f(0.0, 1.0); glVertex3f(500.f, -100.f, -320.f);
	glTexCoord2f(1.0, 1.0); glVertex3f(1550.f, -100.f, -2020.f);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1550.f, -100.f, -2020.f);
	glEnd();
}

void flyingUFO(void) {
	glPushMatrix();
	glTranslatef(lightpos[X], lightpos[Y], lightpos[Z]);
	glDisable(GL_LIGHTING);
	glCallList(FLYING_UFO);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void ufo(void) {
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ufo_sphere_mat);
	glTranslatef(shadowpos[X], shadowpos[Y], shadowpos[Z]);
	glCallList(UFO_SPHERE);
	glPopMatrix();

	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ufo_disk_mat);
	glTranslatef(shadowpos[X], shadowpos[Y], shadowpos[Z]);
	glCallList(UFO_DISK);
	glPopMatrix();
}

void landing_beacon(void)
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, landing_beacon_mat);
	glTranslatef(70.f, -95.f, -600.f);
	glRotatef(90.f, 2020.f, 20.f, 40.f); // depth test prove works ASDFG 90  to 0 
	glutSolidTorus(10.0, 40.0, 100, 100);
	glPopMatrix();
}

void drawShadowGrass() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	/* call objects here to apply shadow on them lol */
	glPushMatrix();
	glMultMatrixf((GLfloat *)floorshadow);
	landing_beacon();
	ufo();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void draw(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grass_mat);

	/* textures */
	glEnable(GL_TEXTURE_2D);
	drawTextureGrass();
	glDisable(GL_TEXTURE_2D);

	/* shhadows */
	if (rendermode == SHADOW) {
		glColor3f(0.f, 0.f, 0.f);
		drawShadowGrass();
	}

	/* objects from here */
	flyingUFO();
	ufo();
	landing_beacon();

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	GLfloat *tex;
	GLUquadricObj *sphere, *disk;
	GLfloat plane[4];
	GLfloat v0[3], v1[3], v2[3];

	glutInit(&argc, argv);
	glutInitWindowSize(1080, 520);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL | GLUT_DOUBLE);
	(void)glutCreateWindow("UFOs");
	glutDisplayFunc(draw);

	glutCreateMenu(menu);
	glutAddMenuEntry("No ", NONE);
	glutAddMenuEntry("Shadows", SHADOW);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* scene persp */
	glMatrixMode(GL_PROJECTION);
	glFrustum(-200., 200., -100., 100., 320., 2040.);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	/* does not work properly without this shit */
	float white[] = { 1.0, 1.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* Matricile de umbre */
	/* Cele 3 pct de pe podea */
	v0[X] = -200.f; v0[Y] = -100.f; v0[Z] = -320.f;
	v1[X] = 200.f;  v1[Y] = -100.f; v1[Z] = -320.f;
	v2[X] = 200.f;  v2[Y] = -100.f; v2[Z] = -620.f;
	findplane(plane, v0, v1, v2);
	shadowmatrix(floorshadow, plane, lightpos);

	glCullFace(GL_BACK);


	/* Liste de display pentru fiecare obiect */

	glNewList(FLYING_UFO, GL_COMPILE);
	sphere = gluNewQuadric();
	disk = gluNewQuadric();
	glColor3f(0.f, 0.5f, 0.f);
	gluSphere(sphere, 50.f, 100, 20);
	glColor3f(0.f, 0.8f, 0.f);
	glRotatef(-70.f, 1.f, 0.f, 0.f);
	gluDisk(disk, 0., 100., 100, 1);
	gluDeleteQuadric(sphere);
	gluDeleteQuadric(disk);
	glEndList();

	glNewList(UFO_SPHERE, GL_COMPILE);
	sphere = gluNewQuadric();
	gluSphere(sphere, 45, 100, 20); // 60 to 45 for presentation 
	gluDeleteQuadric(sphere);
	glEndList();

	glNewList(UFO_DISK, GL_COMPILE);
	disk = gluNewQuadric();
	glRotatef(-110.f, 1.f, 0.f, 0.f);
	gluDisk(disk, 0., 90., 100, 1);
	gluDeleteQuadric(disk);
	glEndList();

	glutMainLoop();
	return 0;
}