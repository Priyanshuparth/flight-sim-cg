#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
#include <stdlib.h>
#define rad (3.1416/180)
#define EN_SIZE 20


#include <fstream>
#include <iostream>


#define checkImageWidth 512
#define checkImageHeight 512
char quote[6][80];
int numberOfQuotes = 0, i;
float view = 10.0;

using namespace std;

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uchar;

fstream inf;

static GLubyte checkImage[checkImageHeight][checkImageWidth][4];


class mRGB
{
public:
	uchar r,g,b,a;
	mRGB(){r = g = b = 0,a=255;}
};

class RGBpixmap
{
	public:
		int nRows,nCols;
		mRGB *pixel;
		void readBMPFile(char *frame);
		void makeCheckerBoard();
		void makeCheckImage();
		void setTexture(GLuint textureName);
};


void RGBpixmap::makeCheckImage()
{
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ( ((i&0x8)==0) ^ ((j&0x8)==0) )*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
			checkImage[i][j][3] = (GLubyte) 255;
		}
	}
}

void RGBpixmap::makeCheckerBoard()
{
 // make checkerboard pattern
	nRows=nCols=64;
	pixel = new mRGB[3*nRows*nCols];
	if(!pixel)
	{
		cout<<"Out of memeory!";
		return;
	}
	long count=0;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
		{
			int c=(( (i/8)+(j/8) ) %2)*255;
			pixel[count].r=c;	//red
			pixel[count].g=c;	//green
			pixel[count++].b=0;	//blue

		}
}

void RGBpixmap::setTexture(GLuint textureName)
{
	glBindTexture(GL_TEXTURE_2D,textureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//This function sets several texture mapping parameters.
	//These parameters are bound to the current texture state
	//that can be made current with glBindTexture.
	//parameters:
	//P1: GLenum: The texture target for which this parameter applies.
		//Must be one of GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, or GL_TEXTURE_CUBE_MAP.
	//P2: GLenum: The texturing parameter to set.
		//GL_TEXTURE_MAG_FILTER  Returns the texture magnification filter value
	//P3: GLfloat or GLfloat* or GLint or GLint*: Value of the parameter specified by pname.

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//GL_TEXTURE_MIN_FILTER: Specifies the texture image minification (reduction) method or filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	//Defines parameters for texture coordinate generation
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//set texture mapping environment parameters

	if(textureName==1)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	//This function defines a one-, two-, or three-dimensional texture image
	//parameters:
	//P1: GLenum: The texture target being specified
	//P2: GLint:The level of detail. Usually, 0 unless mipmapping"is used
	//P3: GLint:The internal format of the image data
	//P4: GLsizei: The width of the one-, two-, or three-dimensional texture image. This must be a power of 2 but may include a border.
	//P5: GLsizei: The height of the two- or three-dimensional texture image. This must be a power of 2 but may include a border.
	//P6: GLint: The width of the border. All implementations must support 0, 1, and 2 texel borders
	//P7: GLenum: The format of the pixel data.
	//P8: GLenum: The data type of each texel value
	//P9: GLvoid *: The pixel data.
	else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nRows, nCols, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

}

ushort getShort(){
	char ic;
	ushort ip;
	inf.get(ic);
	ip = ic;
	inf.get(ic);
	ip |= ((ushort)ic<<8);
	return ip;
}

ulong getLong()
{
	ulong ip = 0;
	char ic = 0;
	unsigned char uc = ic;
	inf.get(ic);
	uc = ic; ip = uc;
	inf.get(ic); uc = ic; ip |= ((ulong)uc<<8);
	inf.get(ic); uc = ic; ip |= ((ulong)uc<<16);
	inf.get(ic); uc = ic; ip |= ((ulong)uc<<24);
	return ip;
}

void RGBpixmap::readBMPFile( char *fname)
{
	inf.open(fname, ios::in | ios::binary); //read binary characters
	if(!inf)
	{
		cout<<"cannot open file!!!!"<<fname<<endl;
		return ;
	}
	int k, row, col, numPadBytes, nBytesInRow;
	// keep the file header information
	char ch1, ch2;
	inf.get(ch1);
	inf.get(ch2); // type: always 'BM'
	ulong fileSize = getLong();
	ushort reserved1 = getShort(); //always 0
	ushort reserved2 = getShort(); //always 0
	ulong offBits = getLong(); //offset to image unreliable
	ulong headerSize = getLong(); //always 40
	ulong numCols = getLong(); //number of colums in image
	ulong numRows = getLong(); //number of rows in image
	ushort planes = getShort(); //always 1
	ushort bitsPerPixel = getShort(); //8 or 24; allow only 24 here
	ulong compression = getLong(); //must be 0 for uncompressed
	ulong imageSize = getLong(); //total byte in image
	ulong xPels = getLong(); //always 0
	ulong yPels = getLong(); //always 0
	ulong numLUTentries = getLong(); //256 for 8 bit, otherwise 0
	ulong impColors = getLong(); //always 0
	if(bitsPerPixel != 24)
	{
		//error must be a 24 bit uncompressed image
		cout<<"Not a 24 bit pixel image or is compressed"<<endl;
		inf.close();
		return ;
	}
	//add bytes at the end of each row so total # is a multiple of 4
	//round up 3 into numCols then 2 next mult. of 4
	nBytesInRow = ((3*numCols + 3)/4)*4;
	numPadBytes = nBytesInRow - 3 * numCols; //need this many
	nRows = numRows; //set classes data members
	nCols = numCols;
	pixel = new mRGB[nRows * nCols]; //make space for array
	if(!pixel)
		return ; //out of memory
	long count = 0;
	char dum;
	for(row = 0; row < nRows; row++) //read pixel values
	{
		for (col = 0; col < nCols; col++)
		{
			char r,g,b;
			inf.get(b); inf.get(g); inf.get(r); //read bytes
			pixel[count].r = r; //place them in colors
			pixel[count].g = g;
			pixel[count++].b = b;
		}
		for (k = 0; k < numPadBytes; k++) //skip padBytes at rows end
		{
			inf >> dum;
		}
	}
	inf.close();
	return ;
	//success
}

#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
#include<mmsystem.h>
#include <stdlib.h>
#define rad (3.1416/180)
#define EN_SIZE 20

using namespace std;

float zoom=4;
int tola[5000][5000];
float tX=0,tY=0,tZ=-8,rX=0,rY=0,rZ=4;
float tZ1=-20,tZ2=-40,tZ3=-60,tZ4=-80,tZ5=-100,tZ6=-120;
float rotX=0,rotY=0,rotZ=0;
float cosX=0,cosY=1,cosZ=0;
float angle=0;
float xEye=0.0f,yEye=5.0f,zEye=30.0f;
float cenX=0,cenY=0,cenZ=0,roll=0;
float radius=0;
float theta=0,slope=0;
float speed = 0.3;
float angleBackFrac = 0.2;
bool saheedMinarVisible = false;
float r[] = {0.1,0.4,0.0,0.9,0.2,0.5,0.0,0.7,0.5,0.0};
float g[] = {0.2,0.0,0.4,0.5,0.2,0.0,0.3,0.9,0.0,0.2};
float b[] = {0.4,0.5,0.0,0.7,0.9,0.0,0.1,0.2,0.5,0.0};
int TIME=0;
bool START = false;
float torusPosX[7] = {1,-2,3,-4,-2,0,2};
float torusPosY[7] = {2,3,10,6,7,4,1};

bool rot = false;


static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void fan(){
    glColor3d(0.5,1,0);
    glPushMatrix();
        glTranslated(0,0,0);
        glScaled(1,1,0.7);
        glutSolidSphere(0.8,30,30);
    glPopMatrix();

    glColor3d(0.5,1,0);
    glPushMatrix();
        glTranslated(0,0,0);
        glRotated(5,0,1,0);
        glScaled(0.5,2.5,0.05);
        glutSolidSphere(1,30,30);
    glPopMatrix();

    glColor3d(0.5,1,0);
    glPushMatrix();
        glTranslated(0,0,0);
        glRotated(-5,0,1,0);
        glRotated(90,0,0,1);
        glScaled(0.5,2.5,0.05);
        glutSolidSphere(1,30,30);
    glPopMatrix();

}



void plane(){
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t*90.0;

    /// Main body
    glColor3d(0.5,1,0);
    glPushMatrix();
        glTranslated(0,0,0);
        glScaled(3,0.4,0.5);
        glutSolidSphere(1,30,30);  ///Body of the plain
    glPopMatrix();

    glColor3d(0,0,0);
    glPushMatrix();
        glTranslated(1.7,0.1,0);
        glScaled(1.5,0.7,0.8);
        glRotated(40,0,1,0);
        glutSolidSphere(0.45,30,30);  ///The black starting pick the glass
    glPopMatrix();


    ///Right
    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(0,0,1.2);
        glRotated(-50,0,1,0);
        glScaled(0.7,0.1,3);
        glRotated(25,0,1,0);
        glutSolidCube(1);  ///right wing
    glPopMatrix();

    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(-0.3,-0.15,1.5);
        glRotated(90,0,1,0);
        glPushMatrix();
            glTranslated(0,0,0.5);
            glRotated(10*a,0,0,1);
            glScaled(0.1,0.1,0.1);
            fan();
  glPopMatrix();
        glScaled(0.1,0.1,0.9);
        glutSolidTorus(0.5,0.5,50,50);  ///the booster
    glPopMatrix();

    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(0.2,-0.15,0.9);
        glRotated(90,0,1,0);
        glPushMatrix();
            glTranslated(0,0,0.5);
            glRotated(10*a,0,0,1);
            glScaled(0.1,0.1,0.1);
            fan();
        glPopMatrix();
        glScaled(0.1,0.1,0.9);
        glutSolidTorus(0.5,0.5,50,50);   ///the booster
    glPopMatrix();

    ///Left
    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(0,0,-1.2);
        glRotated(50,0,1,0);
        glScaled(0.7,0.1,3);
        glRotated(-25,0,1,0);
        glutSolidCube(1);     ///right wing
    glPopMatrix();

    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(-0.3,-0.15,-1.5);
        glRotated(90,0,1,0);
        glPushMatrix();
            glTranslated(0,0,0.5);
            glRotated(10*a,0,0,1);
            glScaled(0.1,0.1,0.1);
            fan();
  glPopMatrix();
        glScaled(0.1,0.1,0.9);
        glutSolidTorus(0.5,0.5,50,50);  ///the booster
    glPopMatrix();

    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(0.2,-0.15,-0.9);
        glRotated(90,0,1,0);
        glPushMatrix();
            glTranslated(0,0,0.5);
            glRotated(10*a,0,0,1);
            glScaled(0.1,0.1,0.1);
            fan();
  glPopMatrix();
        glScaled(0.1,0.1,0.9);
        glutSolidTorus(0.5,0.5,50,50);   ///the booster
    glPopMatrix();


    glPushMatrix();
        glTranslated(-2.8,0,0);
        glScaled(0.8,0.5,0.3);

        ///Right
        glColor3d(0.8,1,0);
        glPushMatrix();
            glTranslated(0.4,0,1.5);
            glRotated(-30,0,1,0);
            glScaled(0.7,0.1,3);
            glRotated(10,0,1,0);
            glutSolidCube(1);    /// right back wing
        glPopMatrix();

        ///left
        glColor3d(0.8,1,0);
        glPushMatrix();
            glTranslated(0.4,0,-1.5);
            glRotated(30,0,1,0);
            glScaled(0.7,0.1,3);
            glRotated(-10,0,1,0);
            glutSolidCube(1);   // left back wing
        glPopMatrix();
    glPopMatrix();


    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(-2.7,0.5,0);
        glRotated(45,0,0,1);
        glScaled(0.8,2,0.1);
        glRotated(-20,0,0,1);
        glutSolidCube(0.5);   // left back wing
    glPopMatrix();

    glColor3d(0.8,1,0);
    glPushMatrix();
        glTranslated(-2.95,0.85,0);
        glRotated(90,0,1,0);
        glScaled(0.05,0.05,0.6);
        glutSolidTorus(0.5,0.5,50,50);
    glPopMatrix();

}

//Extra Display Function
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0, 1.0, 3200);
	glMatrixMode(GL_MODELVIEW);
}

GLfloat UpwardsScrollVelocity = -1.0;

void timeTick(void)
{
	if (UpwardsScrollVelocity< -1)
		view -= 0.0011;
	if (view < 0) { view = 2; UpwardsScrollVelocity = -1.0; }
	//  exit(0);
	UpwardsScrollVelocity -= 0.2;
	glutPostRedisplay();
}


void RenderToDisplay()
{
	int l, lenghOfQuote, i;

	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(-20, 1.0, 0.0, 0.0);
	glScalef(0.05, 0.05, 0.05);

	for (l = 0; l<numberOfQuotes; l++)
	{
		lenghOfQuote = (int)strlen(quote[l]);
		glPushMatrix();
		glTranslatef(-(lenghOfQuote * 37), (l * 200), 0.0);
		for (i = 0; i < lenghOfQuote; i++)
		{
			glColor3f((UpwardsScrollVelocity / 10) + 300 + (l * 10), (UpwardsScrollVelocity / 10) + 300 + (l * 10), 0.0);
			glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[l][i]);
		}
		glPopMatrix();
	}
}

void myDisplayFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 30.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	RenderToDisplay();
	glutSwapBuffers();
}

int winner(char a)
{


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1366,720);
	glutCreateWindow("GAME OVER");
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glLineWidth(3);
	strcpy(quote[1], "Game Over");
	glutDisplayFunc(myDisplayFunction);
	numberOfQuotes = 5;
	glutReshapeFunc(reshape);
	//char c=char(score);
	glutIdleFunc(timeTick);

	glutMainLoop();

	return 0;
}

void singleHouse(int R,int G,int B){
    glColor3d(r[R%11],g[G%11],b[B%11]);
    glPushMatrix();
        glTranslated(0,0,0);
        glutSolidCube(1);
    glPopMatrix();

    glColor3d(0,0,0);
    glPushMatrix();
        glTranslated(0.2,0,0);
        glScaled(0.3,0.3,1.001);
        glutSolidCube(1);
    glPopMatrix();

    glColor3d(0,0,0);
    glPushMatrix();
        glTranslated(-0.2,0,0);
        glScaled(0.3,0.3,1.001);
        glutSolidCube(1);
    glPopMatrix();

    glColor3d(0,0,0);
    glPushMatrix();
        glTranslated(0,0,0.2);
        glScaled(1.001,0.3,0.3);
        glutSolidCube(1);
    glPopMatrix();

    glColor3d(0,0,0);
    glPushMatrix();
        glTranslated(0,0,-0.2);
        glScaled(1.001,0.3,0.3);
        glutSolidCube(1);
    glPopMatrix();

}

void house(int n,int R,int G){
    for(int i=0;i<n;i++){
        glPushMatrix();
            glTranslated(0,0.8+i,0);
            singleHouse(G,R,i);
        glPopMatrix();
    }
}


void environment(int n){

    /// Ground
    glColor3d(0,0.5,0.1);
    glPushMatrix();
        glTranslated(0,0,0);
        glScaled(EN_SIZE*2,0.3,EN_SIZE*2);
        glutSolidCube(1);
    glPopMatrix();


    glColor3d(0,1,0.1);
    glPushMatrix();
        glTranslated(torusPosX[n],torusPosY[n],0);
        glScaled(0.3,0.3,0.3);
        glutSolidTorus(1,3,30,30);
    glPopMatrix();

        for(int i=-(EN_SIZE/2)+1;i<(EN_SIZE/2);i+=2){
            for(int j=-(EN_SIZE/2)+1;j<(EN_SIZE/2);j+=2){
                if(tola[i+(EN_SIZE/2)+1][j+(EN_SIZE/2)+1]!=0){
                    glPushMatrix();
                        glTranslated(i,0,j);
                        house(tola[i+(EN_SIZE/2)+1][j+(EN_SIZE/2)+1],i,j);
                    glPopMatrix();
                }else if(i>=-5&&i<=5){}
                else{
                    tola[i+(EN_SIZE/2)+1][j+(EN_SIZE/2)+1]=(rand()%5)+1;
                    glPushMatrix();
                        glTranslated(i,0,j);
                        house(tola[i+(EN_SIZE/2)+1][j+(EN_SIZE/2)+1],i,j);
                    glPopMatrix();
                }
            }
        }
}

void draw(){
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t*90.0;

    TIME = t;

    ///Plane
    if(rotX>11)rotX=11;
    if(rotX<-11)rotX=-11;
    if(rotZ>10)rotZ=10;
    if(rotZ<-15)rotZ=-15;

    glPushMatrix();
        glTranslated(0,1,0);
        glRotated(90,0,1,0);
        glRotated(5,0,0,1);
        glRotated(rotX,1,0,0);
        glRotated(rotY,0,1,0);
        glRotated(rotZ,0,0,1);

        glScaled(0.4,0.4,0.4);
        plane();
    glPopMatrix();

    ///Environment
    if(tX>=4.1){winner(a);
    exit(0);};
    if(tX<=-4.1){winner(a);
    exit(0);};
    if(tY>0.1){
        winner(a);
    exit(0);};
    if(tY<-15)tY= -15;

    glPushMatrix();
        glTranslated(tX,tY,tZ);
        //environment(2);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ1);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ2);
        environment(3);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ3);
        environment(1);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ4);
        environment(5);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ5);
        environment(4);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ6);
        environment(2);
    glPopMatrix();

    tZ+=speed-10;
    tZ1+=speed;//ground
    tZ2+=speed;
    tZ3+=speed;
    tZ4+=speed;
    tZ5+=speed;
    tZ6+=speed;

    if(tZ>=20)tZ=-100;
    if(tZ1>=20)tZ1=-110;
    if(tZ2>=20)tZ2=-100;
    if(tZ3>=20)tZ3=-110;
    if(tZ4>=20)tZ4=-100;
    if(tZ5>=20)tZ5=-110;
    if(tZ6>=20)tZ6=-10;

    if(rotX>0)rotX-=angleBackFrac;
    if(rotX<0)rotX+=angleBackFrac;
    if(rotY>0)rotY-=angleBackFrac;
    if(rotY<0)rotY+=angleBackFrac;
    if(rotZ>0)rotZ-=angleBackFrac;
    if(rotZ<0)rotZ+=angleBackFrac;

    speed += 0.1;
    if(speed>=0.12)speed=0.12;
}



void drawStrokeText(char* str,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x, y+8,z);
	  glScalef(0.002f,0.002f,z);

	  for (c=str; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}

void drawStrokeText2(char* str,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x, y+8,z);
	  glScalef(0.005f,0.005f,z);

	  for (c=str; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}
void drawStrokeChar(char c,float x,float y,float z)
{
	  glPushMatrix();
          glTranslatef(x, y+8,z);
          glScalef(0.002f,0.002f,z);
          glutStrokeCharacter(GLUT_STROKE_ROMAN , c);
	  glPopMatrix();
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t*90.0;
    double aa=a;

    if(!rot){
        a=0;
    }
    glClearColor(0.2,0.6,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(	0.0, 4.5, 10.0,
                0, 4, 0,
                0, 1.0f, 0.0f);

    if(START){

        glPushMatrix();
            glTranslated(0,0,0);
            glScaled(zoom,zoom,zoom);
            glRotated(a,0,1,0);
            draw();
        glPopMatrix();

        drawStrokeText("UP: W, DOWN: S, LEFT: A, RIGHT: D, MAIN MENU: M",-8,0.9,0);
        drawStrokeText("TIME : ",3,0,0);
        int mod,number=0;
        while(TIME){
            mod=TIME%10;
            number=number*10+mod;
            TIME/=10;
        }
        float tmp=0;
        while(number){
            mod=number%10;
            drawStrokeChar(mod+48,4+tmp,0,0);
            number/=10;
            tmp+=0.2;
        }
    }
    else{

        glPushMatrix();
            glTranslated(0,3,0);
            glRotated(aa,0,1,0);
            glScaled(1.5,1.5,1.5);
            plane();
        glPopMatrix();
        glColor3b(1,1,1);
        drawStrokeText("Press G to Start",-1,-1,0);
        drawStrokeText2("Plane Game",-2,0,0);
    }

    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    float frac = 0.3;
    float rotFrac = 1;
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
        case 'r':
            rot=true;
            break;
        case 't':
            rot=false;
            break;
        case 'z':
            zoom+=0.05;
            break;
        case 'Z':
            zoom-=0.05;
        case 'w':
            tY-=frac;
            rotZ+=rotFrac;
            break;
        case 's':
            tY+=frac;
            rotZ-=rotFrac;
            break;
        case 'a':
            tX+=frac;
            rotX-=rotFrac*3;
            rotY+=rotFrac/2;
            break;
        case 'd':
            tX-=frac;
            rotX+=rotFrac*3;
            rotY-=rotFrac/2;
            break;
        case 'y':
           rotX-=rotFrac;
            break;
       case 'h':
            rotX+=rotFrac;
            break;
        case 'v':
            rotY+=rotFrac;
            break;
        case 'j':
            rotY-=rotFrac;
            break;
        case 'g':
            START=true;
            break;
        case 'm':
            START=false;
            break;
        case 'o':
            cosX-=frac*cos(rotX*rad);
            cosY+=frac*cos(rotY*rad);
            cosZ-=frac*cos(rotZ*rad);
            cout<<"Front : "<<cosX<<" "<<cosY<<" "<<cosZ<<endl;
            break;
        case 'l':
            cosX+=frac*cos(rotX*rad);
            cosY-=frac*cos(rotY*rad);
            cosZ+=frac*cos(rotZ*rad);
            cout<<"Back : "<<cosX<<" "<<cosY<<" "<<cosZ<<endl;
            break;

    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowPosition(0,0);
	glutInitWindowSize(1366,720);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    glutCreateWindow("GLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    //PlaySound("starwars.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);

    sndPlaySound("sound.wav",SND_ASYNC|SND_LOOP);

    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();

    return EXIT_SUCCESS;
}
