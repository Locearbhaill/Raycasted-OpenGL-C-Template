#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 // One degree in radians
// globals
int WIDTH = 1024;
int HEIGHT = 512;
float px, py; // player position
float pdx, pdy, pa; // players delta x, delta y and angle (a)

void drawPLayer()
{
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px,py);
    glVertex2i(px+pdx*5, py+pdy*5);
    glEnd();
}

// Define the map size in height(X), width(Y) and total blocks(S)
// 0 defines floor 
// 1 defines wall
// other numbers can correspond to different colors
int mapX = 8, mapY = 8, mapS = 64;
int map[]=
{
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,

};

void drawMap2D() // Takes map and draws 
{
    int x,y,xo,yo;
    for( y=0; y<mapY; y++ )
    {
        for( x=0; x<mapX; x++)
        {
            if( map[y*mapX+x]==1) {glColor3f(1,1,1);} if( map[y*mapX+x]==0) {glColor3f(0,0,0);} //add extra ifs to chck for color numbers!
            if( map[y*mapX+x]==2) {glColor3f(0,0,1);} 
            xo = x*mapS; yo=y*mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo     +1,        yo+1); //Adding and subtracting extra pixels gives that "outline" look as opposed to solid colors
            glVertex2i(xo     +1,   yo+mapS-1);
            glVertex2i(xo+mapS-1,   yo+mapS-1);
            glVertex2i(xo+mapS-1,        yo+1);
            glEnd();
        }
    }
}

float dist(float ax, float ay, float bx, float by, float ang)
{
    // Pythagoras finally gets used by me! 
    return ( sqrt ((bx-ax) * (bx-ax) + (by-ay) * (by-ay) ));
}


// These rays will be used to calculate the 3d display
void drawRays()
{   
    // rays, map x, map y, map position, depth of feild, rays x and y, ray angle, x and y offset
    // offsets will change varying on the map size
    // hard coding works for now.
    int rayCount = 60; // This can be changed to effectively define field of view (FOV) in degrees
    int r, mx, my, mp, dof; float rx, ry, ra, xo, yo, distanceFinal;
    ra = pa - DR*(rayCount/2);    if(ra < 0)     {ra += 2*PI; }   //ray angle = player angle - a degree amount
                        if(ra > 2*PI)  {ra -=2*PI; }
    for( r=0; r<rayCount; r++)
    {
        // Check for horizontal clipping with gridlines/walls
        dof = 0;
        float distanceH = 1000000, hx = px, hy=py; // default horizontal dist super high so it can only be smaller, horizontal x y saved
        float aTan=-1/tan(ra);

        if( ra>PI ) { ry = (((int)py >> 6) << 6) - 0.0001; rx=( py-ry ) * aTan + px; yo=-64; xo=-yo*aTan; } // facing down
        if( ra<PI ) { ry = (((int)py >> 6) << 6) + 64    ; rx=( py-ry ) * aTan + px; yo= 64; xo=-yo*aTan; } // facing up
        if( ra==0 || ra==PI) {rx = px; ry = py; dof = 8;} // straight left or right
        int mv = 0, mh = 0;
        while(dof<8)
        {
            mx = (int)(rx) >> 6; my = (int) (ry) >> 6; mp=my*mapX+mx;
            if( mp > 0 && mp<mapX*mapY && map[mp] >0 ) { mh = map[mp]; hx=rx; hy=ry; distanceH=dist(px, py, hx, hy, ra); dof=8; } //Hit wall 
            else { rx+=xo; ry+=yo; dof+= 1; } // Next line because wall not hit
        }

        // Check for vertical gridlines/walls
        dof = 0;
        float distanceV = 1000000, vx = px, vy=py; // default vertical dist super high so it can only be smaller, vertical x y saved
        float nTan=-tan(ra);

        if( ra>P2 && ra < P3 ) { rx = (((int)px >> 6) << 6) - 0.0001; ry=( px-rx ) * nTan + py; xo=-64; yo=-xo*nTan; } // facing left
        if( ra<P2 || ra > P3 ) { rx = (((int)px >> 6) << 6) + 64    ; ry=( px-rx ) * nTan + py; xo= 64; yo=-xo*nTan; } // facing right
        if( ra==0 || ra==PI) {rx = px; ry = py; dof = 8;} // straight up or down
        while(dof<8)
        {
            mx = (int)(rx) >> 6; my = (int) (ry) >> 6; mp=my*mapX+mx;
            if( mp > 0 && mp<mapX*mapY && map[mp] >0 ) { mv = map[mp]; vx=rx; vy=ry; distanceV=dist(px, py, vx, vy, ra);dof=8; } //Hit wall 
            else { rx+=xo; ry+=yo; dof+= 1; } // Next line because wall not hit
        }
        // set colors here to be slightly different
        // as a basic way of color shading/lighting
        // mv and mh will change corresponding to the values in map
        if(distanceV < distanceH) { rx = vx; ry = vy; distanceFinal = distanceV; glColor3f(0.9,0,0); if(mv==2) {glColor3f(0,0,0.9);}} 
        if(distanceV > distanceH) { rx = hx; ry = hy; distanceFinal = distanceH; glColor3f(0.7,0,0); if(mh==2) {glColor3f(0,0,0.7);}}
        

        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(px,py);
        glVertex2i(rx,ry);
        glEnd();

        // Drawing of the 3D scene
        // ca = cosine angle
        float ca =pa-ra; if(ca < 0) {ca += 2*PI; }  if(ca > 2*PI) {ca -=2*PI; } distanceFinal = distanceFinal * cos(ca); // fixes fisheye
        // lineH = line height
        float lineH = (mapS * 320 )/distanceFinal; if(lineH > 320) {lineH = 320;}// cubeSize * newScreenHeight / the ray's final distance. Capped at height
        // lineO = line offset (to centre the lines)
        float lineO = 160 - lineH/2;
        glLineWidth(8);
        glBegin(GL_LINES);
        // draw every 8th pixel 
        glVertex2i(r*8+530,lineO);
        // shift window over to the right
        glVertex2i(r*8+530, lineH+lineO);
        glEnd();






        ra += DR;   if(ra < 0) {ra += 2*PI; }  if(ra > 2*PI) {ra -=2*PI; }

    }

}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D(); // Needs to be called before draw player 
    drawPLayer();
    drawRays();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
    // Adding and subtracting tiny amounts because calcultions are in radians, not degrees!!
    if(key=='a') { pa -=0.1; if( pa<0    ){ pa+=2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5;  } //Reset if out of bounds of course
    if(key=='d') { pa +=0.1; if( pa>2*PI ){ pa-=2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5;  }
    if(key=='w') { px += pdx; py += pdy; }
    if(key=='s') { px -= pdx; py -= pdy; }
    glutPostRedisplay();
}

void init()
{
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,WIDTH,HEIGHT,0);
    px = 300; py = 300; pdx=cos(pa)*5; pdy=sin(pa)*5; //Calculate delta on init 
}

int main(int argc,char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Raycasted Thing!");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
}