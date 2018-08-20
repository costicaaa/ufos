# Proiect Grafica pe calculator : Unidentified flying object ( UFO ) 
#### Costache Costin | Grupa 253

##
## Descrierea proiectului
Pentru acest proiect, am ales sa reprezint OZN - uri, deoarece se pot ilustra usor cunostiintele acumulate, chiar daca este un proiect relativ simplu. 
OZN - ul verde este in zbor, de aceea lumineaza ( este sursa luminii din scena ) si se apropie de locul de aterizare. 
Locul de aterizare ( torus ) este ocupat de OZN - ul albastru, care nu este iluminat, deoarece nu zboara. 

## Scena a fost realizeaza cu ajutorul :
- texturilor
- obiectelor 3D solide
- obiectelor cuadrice
- transformarilor 3D
- iluminarii
- umbrelor
- testelor de adancime
- 
### Exemple de obiecte desenate impreuna cu transformarile aplicate:
- ##### platforma aterizare
```C++
void landing_beacon(void)
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, landing_beacon_mat);
	glTranslatef(70.f, -95.f, -600.f);
	glRotatef(90.f, 2020.f, 20.f, 40.f); // depth test prove works ASDFG 90  to 0 
	glutSolidTorus(10.0, 40.0, 100, 100);
	glPopMatrix();
}
```
- ##### ozn
```C++
void flyingUFO(void) {
	glPushMatrix();
	glTranslatef(lightpos[X], lightpos[Y], lightpos[Z]);
	glDisable(GL_LIGHTING);
	glCallList(FLYING_UFO);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}
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
```


### Teste de adancime si iluminare
```C++
// pozitia luminii ( si a ozn ului care zboara ) 
GLfloat lightpos[] = { 0.f, 80.f, -350.f, 1.f };

// activarea luminii si a testului de adancime
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
//setarile pentru lumina
	float white[] = { 1.0, 1.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
```

### Texturare
Am folosit pentru texturarea ierbii o poza gasita pe internet. 
Initial am vrut sa texturez si fiecare sfera/disc, de aceea am modificat functia de incarcare a texturii : 
```c++
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
```
Nu am ajuns la rezultate placute, asa ca singura textura din proeiect este cea pentru iarba. 
```C++
void drawTextureGrass() {
	LoadTexture("iarba.jpg");
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-500.f, -100.f, -320.f);
	glTexCoord2f(0.0, 1.0); glVertex3f(500.f, -100.f, -320.f);
	glTexCoord2f(1.0, 1.0); glVertex3f(1550.f, -100.f, -2020.f);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1550.f, -100.f, -2020.f);
	glEnd();
}
```
### Umbre
```C++
//Calculul matricii pentru umbre
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

//Ecuatia planului cu ajutorul a 3 puncte
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

// Calcul plan si matrice pentru 3 pct de pe podea
	v0[X] = -200.f; v0[Y] = -100.f; v0[Z] = -320.f;
	v1[X] = 200.f;  v1[Y] = -100.f; v1[Z] = -320.f;
	v2[X] = 200.f;  v2[Y] = -100.f; v2[Z] = -620.f;
	findplane(plane, v0, v1, v2);
	shadowmatrix(floorshadow, plane, lightpos);
```


## Originalitate
Nota de originalitate a proiectului o reprezinta design-ul abstract si simplist al OZN urilor si modul de iluminare : sursa luminii este chiar un obiect. 



## Bibliografie
- coduri sursa moodle 
- https://github.com/netcse/opengl-3d-home
- https://www.khronos.org
