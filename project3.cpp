#if defined(__APPLE__) && defined(__MACH__)
    #include <OpenGL/gl.h>
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

//angle of rotation
float xpos = -8, ypos = 5, zpos = 5, xrot = 0, yrot = 110;
float lastx, lasty;
bool resetMouse = true;
int screen_width = 800, screen_height = 500;

//editor setup
int edit = 0;
bool edit_mode = true;

//board setup
float space = 0.5;
int n = 25;
int nsq = 625;
int ncu = 15625;
bool board1[15625] = {};
bool board2[15625] = {};
bool board3[15625] = {};
bool tmp_board1[15625];
bool tmp_board2[15625];
bool tmp_board3[15625];
bool running = true;
bool run_once = false;
bool clear = false;
bool randomize = true;
bool show_board1 = true;
bool show_board2 = true;
bool show_board3 = true;
bool wireframe = true;
int refresh_rate = 60;
bool explode = false;
bool crossPlanes = true;

GLuint texture1; //the arrays for our textures
GLuint texture2;
GLuint texture3;
GLuint texture4;

bool lighting = true;
bool fog = false;
bool nurb = true;

//Handle movement of the edit circle
int getNextPosX (int i) {
    if ((i/n) % 2 ) {
        if (((i+1)%n)!=0)
            return i+1;
    } else if (((i)%n)!=0) {
        return i-1;
    }
    return i;
}
void editMovePosX () {
    edit = getNextPosX(edit);
}
int getNextNegX (int i) {
    if ((i/n) % 2) {
        if (((i)%n)!=0)
            return i-1;
    } else if (((i+1)%n)!=0) {
        return i+1;
    }
    return i;
}
void editMoveNegX () {
    edit = getNextNegX(edit);
}
int getNextNegY (int i) {
    if ((i/nsq) % 2 ) {
        if ((i%nsq) < (nsq-n))
            return (i + (2*(n-(i%n))-1));
    } else if ((i%nsq) >= n) {
        return (i - (2*(i%n)+1));
    }
    return i;
}
void editMoveNegY () {
    edit = getNextNegY(edit);
}
int getNextPosY (int i) {
    if ((i/nsq) % 2 ) {
        if ((i%nsq) >= n)
            return (i - (2*(i%n)+1));
    } else if ((i%nsq) < (nsq-n)) {
        return (i + (2*(n-(i%n))-1));
    }
    return i;
}
void editMovePosY () {
    edit = getNextPosY(edit);
}
int getNextPosZ (int i) {
    if (i+nsq < ncu)
        return (i + (2*(nsq-(i%nsq))-1));
    
    return i;
}
void editMovePosZ () {
    edit = getNextPosZ(edit);
}
int getNextNegZ (int i) {
    if (i-nsq >= 0)
        return (i - (2*(i%nsq)+1));
        
    return i;
}
void editMoveNegZ () {
    edit = getNextNegZ(edit);
}

void runGame (int val) {
    if (clear) {
        for (int i=0; i<ncu; i++) {
            board1[i] = 0;
            board2[i] = 0;
            board3[i] = 0;
        }
        clear = false;
    }
    if (randomize) {
        srand ( time(NULL) );
        int b = rand();
        for (int i=0; i<ncu; i++) {
            board1[i] = ((b & 15) == 0);
            board2[i] = board1[i];
            board3[i] = board1[i];
            b >>= 4;
            if (b == 0) b = rand();
        }
        randomize = false;
    }
    if (running || run_once) {
        run_once = false;
        for (int i=0; i<ncu; ++i) {
            // Get neighbor sum for each plane
            short sum1 = 0;
            short sum2 = 0;
            short sum3 = 0;
            // Primaries get shared
            short backMiddle = getNextNegZ(i);
            short frontMiddle = getNextPosZ(i);
            short leftMiddle = getNextNegX(i);
            short rightMiddle = getNextPosX(i);
            // Start with Z plane
            bool zplane [8] = { 
                board1[backMiddle],
                board1[frontMiddle],
                board1[leftMiddle],
                board1[rightMiddle],
                board1[getNextNegZ(leftMiddle)],
                board1[getNextPosZ(leftMiddle)],
                board1[getNextNegZ(rightMiddle)],
                board1[getNextPosZ(rightMiddle)]
            };
            
            // Then Y plane
            short topMiddle = getNextPosY(i);
            short bottomMiddle = getNextNegY(i);
            bool yplane [8] = {
                board2[topMiddle],
                board2[bottomMiddle],
                board2[leftMiddle],
                board2[rightMiddle],
                board2[getNextNegY(leftMiddle)],
                board2[getNextPosY(leftMiddle)],
                board2[getNextNegY(rightMiddle)],
                board2[getNextPosY(rightMiddle)]
            };
            
            // Then X plane
            bool xplane [8] = {
                board3[topMiddle],
                board3[bottomMiddle],
                board3[frontMiddle],
                board3[backMiddle],
                board3[getNextNegY(frontMiddle)],
                board3[getNextPosY(frontMiddle)],
                board3[getNextNegY(backMiddle)],
                board3[getNextPosY(backMiddle)]
            };

            
            for (int j=0; j<8; ++j) sum1 += zplane[j];
            for (int j=0; j<8; ++j) sum2 += yplane[j];
            for (int j=0; j<8; ++j) sum3 += xplane[j];
            
            short aliveCount = 0;
            
            if (sum1 == 3 || (sum1 == 2 && board1[i])) {
                tmp_board1[i] = true;
                aliveCount++;
            } else {
                tmp_board1[i] = false;
            }

            if (sum2 == 3 || (sum2 == 2 && board2[i])) {
                tmp_board2[i] = true;
                aliveCount++;
            } else {
                tmp_board2[i] = false;
            }
            
            if (sum3 == 3 || (sum3 == 2 && board3[i])) {
                tmp_board3[i] = true;
                aliveCount++;
            } else {
                tmp_board3[i] = false;
            }
            
            if (crossPlanes && aliveCount == 2) {
                if (tmp_board1[i])
                    if (tmp_board2[i])
                        tmp_board3[i] = true;
                    else
                        tmp_board2[i] = true;
                else
                    tmp_board1[i] = true;
            }
            
        }
        for (int i=0; i<ncu; i++) {
            board1[i] = tmp_board1[i];
            board2[i] = tmp_board2[i];
            board3[i] = tmp_board3[i];
        }
    }
    glutPostRedisplay();
    glutTimerFunc (refresh_rate,runGame,val+1);
}

GLuint LoadTexture( const char * filename, int width, int height ) {
    GLuint texture;
    unsigned char * data;
    FILE * file;
    
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to it’s array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //Here we are setting the parameter to repeat the texture instead of clamping the texture
    //to the edge of our shape. 
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );    

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture ); 
}

//draw the cubes that make up the board
void cube (void) {
    if (explode) {
        if (space < 2)
            space+=0.1;
        else {
            explode = false;
        }
        glScalef(1+(space-0.5),1+(space-0.5),1+(space-0.5));
    } else {
        if (space > 0.5) {
            space-=0.1;
            glScalef(1+(space-0.5),1+(space-0.5),1+(space-0.5));
        }
    }

    for (int i=0;i<ncu;i++)
    {    
    float x, y, z;
    x = 0;
    y = 0;
    z = 0;
    if((i%n)==0)
    {
        y = space;
        if ((i/nsq)%2) y *= -1;
        if ((i%nsq)==0) y = 0;
        z = 0;
    } else {
        y = 0;
        if ((i / n) % 2)
            z = -1*space;
        else
            z = space;
    }
    if ((i%nsq)==0) {
        x = space;
    }
    glTranslatef(x, y, z);
    glPushMatrix();
    srand(i);
    if (i == edit && edit_mode) {
        //draw the cursor
        if (board1[i] || board2[i] || board3[i]) {
            glColor3f(1,0,0);
        } else {
            glColor3f(0,1,0);
        }
        if (lighting) {
            glDisable( GL_TEXTURE_2D );
        }
        if (wireframe)
            glutWireCube(space);
        else
            glutSolidCube(space);
        if (lighting) {
            glEnable( GL_TEXTURE_2D );
        }
    } else {
        glColor3f(.5+rand()/(float)RAND_MAX,.5+rand()/(float)RAND_MAX,.5+rand()/(float)RAND_MAX);
        if (lighting) {
            GLfloat whiteMaterial[] = {1.0, 1.0, 1.0};
            if (i % 4 == 3) {
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, whiteMaterial);
                glBindTexture( GL_TEXTURE_2D, texture1 );
            } else if (i % 4 == 2) {
                glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, whiteMaterial);
                glBindTexture( GL_TEXTURE_2D, texture2 );
            } else if (i % 4 == 1) {
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteMaterial);
                glBindTexture( GL_TEXTURE_2D, texture3 );
            } else {
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteMaterial);
                glBindTexture( GL_TEXTURE_2D, texture4 );
            }
        }
        
        if (wireframe) {
            if (board1[i] && board2[i] && board3[i]) glutWireCube(space); //draw the cube
            else if ((board1[i] + board2[i] + board3[i]) == 2) glutWireTeapot(space);
            else if (board1[i] && show_board1) glutWireTorus(space/4,space/2,5,5); // draw the torus
            else if (board2[i] && show_board2) glutWireSphere(space/2,10,10); // draw the sphere
            else if (board3[i] && show_board3) glutWireCone(space/2,space/2,3,3); // draw the cone
        } else {
            if (board1[i] && board2[i] && board3[i]) glutSolidCube(space); //draw the cube
            else if ((board1[i] + board2[i] + board3[i]) == 2) glutSolidTeapot(space);
            else if (board1[i] && show_board1) glutSolidTorus(space/4,space/2,5,5); // draw the torus
            else if (board2[i] && show_board2) glutSolidSphere(space/2,10,10); // draw the sphere
            else if (board3[i] && show_board3) glutSolidCone(space/2,space/2,3,3); // draw the cone
        }
        
        if (lighting) {
            GLfloat blankMaterial[] = {0.0, 0.0, 0.0};
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blankMaterial);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, blankMaterial);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blankMaterial);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blankMaterial);
        }
    }
    glPopMatrix();
    }
}

void init (void) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    if (lighting) {
        glEnable(GL_LIGHTING);
    }
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
}

void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0); 
    glRotatef(yrot,0.0,1.0,0.0); 
    glTranslated(-xpos,-ypos,-zpos);
}

void display (void) {
    
    glClearColor (0.0,0.0,0.0,1.0); //clear the screen to black
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color buffer and the depth buffer
    glMatrixMode( GL_MODELVIEW );
    glColor4f(1.0f,0.0f,0.0f,1.0f);
    
    glEnable(GL_SCISSOR_TEST);
    glMatrixMode (GL_PROJECTION); //set the matrix to projection
    
    // Draw the primary view
    glViewport (0, 0, (GLsizei)screen_width, (GLsizei)screen_height); //set the viewport to the current window specifications
    glScissor(0, 0, (GLsizei)screen_width, (GLsizei)screen_height);
    glLoadIdentity ();
    
    if (lighting) {
        GLfloat AmbientLight[] = {0.1, 0.1, 0.2};
        glLightfv (GL_LIGHT0, GL_AMBIENT, AmbientLight);
        GLfloat DiffuseLight[] = {1, 1, 1};
        glLightfv (GL_LIGHT1, GL_DIFFUSE, DiffuseLight);
        GLfloat LightPosition[] = {xpos, ypos, zpos, 1};
        glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    }
    gluPerspective (60, (GLfloat)screen_width / (GLfloat)screen_height, 1.0, 100.0);
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //camera position, x,y,z, looking at x,y,z, Up Positions of the camera
    camera();
    glPushMatrix();
    if (lighting) {
        texture1 = LoadTexture( "texture.raw", 256, 256 );
        texture2 = LoadTexture( "/dev/urandom", 256, 256 );
        texture3 = LoadTexture( "water.raw", 500, 375 );
        texture4 = LoadTexture( "bubbles.raw", 200, 200 );
        glEnable( GL_TEXTURE_2D ); //enable 2D texturing
        glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
        glEnable(GL_TEXTURE_GEN_T);
        
        if (fog) {
            GLfloat fogColor[4]= {0.5f, 1.0f, 0.5f, 1};  // Fog Color
            glFogi(GL_FOG_MODE, GL_EXP);        // Fog Mode
            glFogfv(GL_FOG_COLOR, fogColor);    // Set Fog Color
            glFogf(GL_FOG_DENSITY, 1.0f);       // How Dense Will The Fog Be
            glHint(GL_FOG_HINT, GL_DONT_CARE);  // Fog Hint Value
            glFogf(GL_FOG_START, n*space);      // Fog Start Depth
            glFogf(GL_FOG_END,-n*space);          // Fog End Depth
        }
        
        if (nurb) {
            glPushMatrix();
            glRotatef(270,0.0,1.0,0.0);
            glTranslated(n*space/2,n*space/2,-n*space);
            GLfloat ctlpoints[4][4][3];
            GLUnurbsObj *theNurb;
            GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
            int u, v;
            for (u = 0; u < 4; u++) {
               for (v = 0; v < 4; v++) {
                  ctlpoints[u][v][0] = 2.0*((GLfloat)u - 1.5);
                  ctlpoints[u][v][1] = 2.0*((GLfloat)v - 1.5);

                  if ( (u == 1 || u == 2) && (v == 1 || v == 2))
                     ctlpoints[u][v][2] = 3.0;
                  else
                     ctlpoints[u][v][2] = -3.0;
               }
            }

            theNurb = gluNewNurbsRenderer();
            gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
            gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
            gluBeginSurface(theNurb);
            gluNurbsSurface(theNurb, 
                               8, knots, 8, knots,
                               4 * 3, 3, &ctlpoints[0][0][0], 
                               4, 4, GL_MAP2_VERTEX_3);
            gluEndSurface(theNurb);
            glPopMatrix();
        }
    }
    cube(); //call the cube drawing function
    glPopMatrix();
    if (lighting) {
        FreeTexture( texture1 );
        FreeTexture( texture2 );
        FreeTexture( texture3 );
        FreeTexture( texture4 );
    }
    glPushMatrix();
    glTranslated(space*((n/2)+1),space*(n/2),space*(n/2));
    glutWireCube(space*n);
    glPopMatrix();
    
    // Draw the secondary view
    glViewport (3*(screen_width/4), 3*(screen_height/4), screen_width/4, screen_width/4); //set the viewport to the current window specifications
    glScissor(3*(screen_width/4), 3*(screen_height/4), screen_width/4, screen_width/4);
    glLoadIdentity();
    glOrtho(-1, 13, -1, 13, -1, 13);
    glRotatef(90,0.0,1.0,0.0); 
    cube();
    glDisable(GL_SCISSOR_TEST);
    glutSwapBuffers(); //swap the buffers
}

void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)w, (GLsizei)h); //set the viewport to the current window specifications
    glMatrixMode (GL_PROJECTION); //set the matrix to projection
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0); //set the perspective (angle of sight, width, height, , depth)
    glMatrixMode (GL_MODELVIEW); //set the matrix back to model
}

void mouseMove(int x, int y)
{
    if(!resetMouse)
    {
            int diffx=x-lastx; //check the difference between the current x and the last x position
            int diffy=y-lasty; //check the difference between the current y and the last y position
            lastx=x; //set lastx to the current x position
            lasty=y; //set lasty to the current y position
            //set the xrot to xrot with the addition of the difference in the y position
            xrot += (float) diffy * 0.2;
            yrot += (float) diffx * 0.2;    //set the xrot to yrot with the addition of the difference in the x position
            
            if (xrot > 90) xrot = 90;
            if (xrot < -90) xrot = -90;
        
        resetMouse = true;
        glutWarpPointer(screen_width/2, 0);
    }
    else {
        resetMouse = false;
        lastx=x; //set lastx to the current x position
        lasty=y; //set lasty to the current y position
    }
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y) {
    if (key=='r')
    {
        xpos = -8;
        ypos = 5;
        zpos = 5;
        xrot = 0;
        yrot = 110;
        edit = 0;
        randomize = true;
        refresh_rate = 0;
    }
    if (key=='c')
    {
        clear = true;
    }
    
    if (key=='=')
    {
        // speed up
        if (refresh_rate >= 100)
            refresh_rate -= 100;
    }
    
    if (key=='-')
    {
        // slow down
        refresh_rate += 100;
    }
    
    if (key=='0')
    {
        // step
        run_once = true;
    }
    
    if (key=='1')
    {
        // toggle board1 visibility
        show_board1 = !show_board1;
    }
    if (key=='2')
    {
        // toggle board2 visibility
        show_board2 = !show_board2;
    }
    if (key=='3')
    {
        // toggle board1 visibility
        show_board3 = !show_board3;
    }
    
    if (key=='4') {
        // toggle classic game of life mode
        crossPlanes = !crossPlanes;
    }

    if (key=='z')
    {
        ypos --;
    }
    
    if (key==' ')
    {

        ypos ++;
    }

    if (key=='w')
    {
    float xrotrad, yrotrad;
    yrotrad = (yrot / 180 * 3.141592654f);
    xrotrad = (xrot / 180 * 3.141592654f); 
    xpos += float(sin(yrotrad));
    zpos -= float(cos(yrotrad));
    ypos -= float(sin(xrotrad));
    }

    if (key=='s')
    {
    float xrotrad, yrotrad;
    yrotrad = (yrot / 180 * 3.141592654f);
    xrotrad = (xrot / 180 * 3.141592654f); 
    xpos -= float(sin(yrotrad));
    zpos += float(cos(yrotrad));
    ypos += float(sin(xrotrad));
    }

    if (key=='d')
    {
        float yrotrad;
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos += float(cos(yrotrad));
        zpos += float(sin(yrotrad));
    }

    if (key=='a')
    {
        float yrotrad;
         yrotrad = (yrot / 180 * 3.141592654f);
         xpos -= float(cos(yrotrad));
         zpos -= float(sin(yrotrad));
    }
    if (key==27 || key=='q')
    {
    glutLeaveGameMode(); //set the resolution how it was
    exit(0); //quit the program
    }
    if (key=='e') {
        edit_mode = !edit_mode;
    }
    if (key=='/') {
        board1[edit] = !(board1[edit] || board2[edit] || board3[edit]);
        board2[edit] = board1[edit];
        board3[edit] = board1[edit];
        
    }
    if (key=='p') {
        running = !running;
    }
    
    if (key=='y') {
        wireframe = !wireframe;
    }
    
    if (key=='t') {
        int model = 0;
        glGetIntegerv(GL_SHADE_MODEL, &model);
        if (model == (int)GL_SMOOTH)
            glShadeModel (GL_FLAT);
        else
            glShadeModel (GL_SMOOTH);
    }
    
    if (key=='f') {
        if (fog) {
            fog = false;
            glDisable(GL_FOG);  // Disable GL_FOG
        } else {
            fog = true;
            glEnable(GL_FOG);   // Enables GL_FOG
        }
    }
    
    if (key=='n') {
        nurb = !nurb;
    }
    
    if (key=='l') {
        lighting = !lighting;
        if (lighting) {
            glEnable(GL_LIGHTING);
        } else {
            glDisable(GL_LIGHTING);
        }
    }
    
    if (key=='.') {
        editMovePosZ();
    }
    if (key==',') {
        editMoveNegZ();
    }
}

void keyboardSpecial (int key, int x, int y) {
    if (key==GLUT_KEY_UP)
    {
        editMovePosY();
    }
    if (key==GLUT_KEY_DOWN)
    {
        editMoveNegY();
    }
    if (key==GLUT_KEY_LEFT)
    {
        editMovePosX();
    }
    if (key==GLUT_KEY_RIGHT)
    {
        editMoveNegX();
    }
}

void clickMouse(int button, int state, int x, int y) {
    explode = true;
}

int main (int argc, char **argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH); //set the display to Double buffer, with depth
    glutGameModeString( "800x600:32@75" ); //the settings for fullscreen mode
    glutEnterGameMode(); //set glut to fullscreen using the settings in the line above
    init (); //call the init function
    glutDisplayFunc (display); //use the display function to draw everything
    glutIdleFunc (display); //update any variables in display, display can be changed to anyhing, as long as you move the variables to be updated, in this case, angle++;
    glutReshapeFunc (reshape); //reshape the window accordingly
    glutPassiveMotionFunc(mouseMove);
    glutKeyboardFunc (keyboard); //check the keyboard
    glutSpecialFunc(keyboardSpecial);
    glutMouseFunc(clickMouse);
    glutTimerFunc (refresh_rate,runGame,0);
    glutMainLoop (); //call the main loop
    return 0;
}