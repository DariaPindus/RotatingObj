#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <string> 
#include <png.h>

#define VIEW_NEAR      1.0
#define VIEW_FAR     20.0

int angle = 0;		//variable for rotation, animation
int shift = 0;		//the angle of shift relatively to the start
GLUquadric *quad;
int width = 500;
int height = 500;
int timer_step;	//step of timer
int count = 0;
int timeout;		//by the end of this time the function will stop
int working_time = 0;
int aspect;
int planetorb = 0;	//angle of motion of orbiting object
bool first = true;
int name_counter = 0;
const std::string ext = ".png";

struct RGB
{
	int red;
	int green;
	int blue;

	void SetColor() {
		glColor3f (this->red, this->green, this->blue);
	}
};
std::vector<RGB> colors_vector_;


void CreateDisk(int shift, RGB color) {
	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_LINE); 
	glPushMatrix ();
    //glTranslatef (0., 0., 1.);
	color.SetColor();
	glRotatef(shift, 0, 1, 0);
    glRotatef(count, 0, 1, 0);
 
    gluDisk (quad, 0., 0.5, 50, 1);
    glPopMatrix ();
}

void CheckRot() {
	if (angle > 360)
		angle -= 360;
	if (planetorb > 360)
		planetorb -= 360;
}

void animate() {
	CheckRot();
	count += 15;
	planetorb += 25;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //gluPerspective(5.0f, aspect, VIEW_NEAR, VIEW_FAR);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, aspect, .5, 50);
	glMatrixMode(GL_MODELVIEW); //select the modelview matrix.
    glLoadIdentity ();
    gluLookAt(0,0,4,
              0,0,0,
              0,1,0);

    glPushMatrix();
    
    CreateDisk(20, colors_vector_[0]);
    CreateDisk(60, colors_vector_[1]);
    CreateDisk(90, colors_vector_[2]);    

    glRotatef((GLfloat)planetorb,0.0,1.0,0.0);
    glTranslatef(0.5,0.0,0.0);
    glColor3ub(30, 120, 30);          // set color to green
    glutWireSphere(0.1,20,20); 
    glPopMatrix();

    glutSwapBuffers();
}

void InirColors(){
  RGB one = {0, 0, 1};
  RGB two = {0, 1, 0};
  RGB three = {1, 0, 1};
  RGB four = {1, 1, 1};
  colors_vector_.push_back(one);
  colors_vector_.push_back(two);
  colors_vector_.push_back(three);
  colors_vector_.push_back(four);
}


void screenshoot(const std::string& name) {

    FILE *fp = fopen(name.c_str(), "wb");
    if (!fp) {
       return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
    }

    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        png_destroy_write_struct(&png_ptr, NULL);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, NULL);
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, png_info, width, height, 8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    unsigned char data[width*height*3], argb_data[width*height*4];
    unsigned char *rows[height];

//    render();
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, argb_data);

    for (int i = 0; i < height; ++i) {
        rows[height - i - 1] = data + (i*width*3);
        for (int j = 0; j < width; ++j) {
            int i1 = (i*width+j)*3;
            int i2 = (i*width+j)*4;
            data[i1++] = argb_data[++i2];
            data[i1++] = argb_data[++i2];
            data[i1++] = argb_data[++i2];
        }
    }


    png_set_rows(png_ptr, png_info, rows);
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png_ptr, png_info);
    
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
}

std::string setStringName() {
    std::string pre = "img";
    std::string numb;
    if (name_counter >= 10) {
        if (name_counter >= 100) {
            numb = std::to_string(name_counter++);
        } 
        else {
            numb = "0" + std::to_string(name_counter++);
        }
    }
    else 
        numb = "00" + std::to_string(name_counter++);

    return pre + numb + ext;
}


void timerFunc(int value) {
	if (working_time >= timeout) {
		system("ffmpeg -framerate 10 -i img%03d.png -c:v libx264  -pix_fmt yuv420p out.mp4");
        exit(0);
    }
    animate();
    working_time += timer_step;
    first = false;
    screenshoot(setStringName());
    glutPostRedisplay();
    glutTimerFunc(timer_step, timerFunc, value);
}

int main(int argc, char **argv)
{
	aspect = (GLfloat)width/(GLfloat)height;
    timeout = 4000;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);  // double buffered window
    glutCreateWindow ("Orbit Demo");                        // Graphics Window Title
    glutReshapeWindow(width,height);                             // Graphics Window Size                                       // lighting and material properties
    InirColors();
    timer_step = 500;
    glFlush();
    glutDisplayFunc (display);                              // graphics display function                                 // updates object motions
    glutTimerFunc(timer_step, timerFunc, 0);
    glutMainLoop();                                     // enters the GLUT event processing loop
}