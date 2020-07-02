#include <iostream>
#include <string>
#include <fstream>
#include <jpeglib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/rotate_vector.hpp"

using namespace std;

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture;
int heightTexture;

int widthDisplay = 1000;
int heightDisplay = 1000;

int prevWidthDisplay = 1000;
int prevHeightDisplay = 1000;

bool isFullscreen = false;

glm::vec3 cameraPosition;
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraGaze = glm::vec3(0.0, 0.0, 1.0);
glm::vec3 cameraCross = glm::vec3(-1.0, 0.0, 0.0);
GLfloat cameraSpeed = 0;

GLFWmonitor* primaryMonitor;
const GLFWvidmode* vidmode;

GLfloat fovy = 45;
GLfloat aspectRatio = 1;
GLfloat near = 0.1;
GLfloat far = 1000;
GLfloat heightFactor = 10.0f;

glm::vec3 lightPos;

int vertexCount;
glm::vec3* vertices;

bool readDataFromFile(const string& fileName, string &data) {
    fstream myfile;
    myfile.open(fileName.c_str(), std::ios::in);
    if (myfile.is_open()) {
        string curLine;
        while (getline(myfile, curLine)){
            data += curLine;
            if (!myfile.eof())
                data += "\n";
        }
        myfile.close();
    } else
        return false;
    return true;
}

GLuint initVertexShader(const string& filename) {
    string shaderSource;
    if (!readDataFromFile(filename, shaderSource)){
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }
    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);
    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
	return vs;
}

GLuint initFragmentShader(const string& filename) {
    string shaderSource;
    if (!readDataFromFile(filename, shaderSource)) {
        exit(-1);
    }
    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);
    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
	return fs;
}

void initShaders() {
  idProgramShader = glCreateProgram();
  idVertexShader   = initVertexShader("shader_flat.vert");
  idFragmentShader = initFragmentShader("shader_flat.frag");
  glAttachShader(idProgramShader, idVertexShader);
  glAttachShader(idProgramShader, idFragmentShader);
  glLinkProgram(idProgramShader);
}

void initTexture(char *filename,int *w, int *h, int temp) {
    int width, height;

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen( filename, "rb" );
    unsigned long location = 0;
    int i = 0, j = 0;

    if ( !infile )
    {
        return;
    }

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error( &jerr );
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress( &cinfo );
    /* this makes the library read from infile */
    jpeg_stdio_src( &cinfo, infile );
    /* reading the image header which contains image information */
    jpeg_read_header( &cinfo, TRUE );
    /* Start decompression jpeg here */
    jpeg_start_decompress( &cinfo );

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
    /* read one scan line at a time */
    while( cinfo.output_scanline < cinfo.image_height )
    {
        jpeg_read_scanlines( &cinfo, row_pointer, 1 );
        for( i=0; i<cinfo.image_width*cinfo.num_components;i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;

    glUniform1i(glGetUniformLocation(idProgramShader, temp == 0 ? "rgbTexture" : "rgbHeight"), temp);

    glGenTextures(1,&idJpegTexture);
    glActiveTexture(GL_TEXTURE0 + temp);
    glBindTexture(GL_TEXTURE_2D, idJpegTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);

    *w = width;
    *h = height;

    glGenerateMipmap(GL_TEXTURE_2D);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    free( row_pointer[0] );
    free( raw_image );
    fclose( infile );

}

void adjustShaderDetails(){
    glClearStencil(0);
    glClearDepth(1.0f);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    cameraPosition += (cameraGaze * cameraSpeed);
    glViewport(0, 0, widthDisplay, heightDisplay);
    glm::mat4 matrixProjection = glm::perspective(fovy, aspectRatio, near, far);
    glm::mat4 matrixView = glm::lookAt(cameraPosition, cameraPosition + cameraGaze*near, cameraUp);
    glm::mat4 matrixModel = glm::mat4(1.0f);
    glm::mat4 matrixModelViewProjection = matrixProjection * matrixView * matrixModel;
    glm::mat4 matrixNormal = glm::inverseTranspose(matrixView);	
    glUniformMatrix4fv(glGetUniformLocation(idProgramShader, "MVP"), 1, GL_FALSE, &matrixModelViewProjection[0][0]);
    glUniform3fv(glGetUniformLocation(idProgramShader, "cameraPosition"), 1, &cameraPosition[0]);
    glUniform1i(glGetUniformLocation(idProgramShader, "widthTexture"), widthTexture);
    glUniform1i(glGetUniformLocation(idProgramShader, "heightTexture"), heightTexture);
    glUniform1f(glGetUniformLocation(idProgramShader, "heightFactor"), heightFactor);
    glUniform3fv(glGetUniformLocation(idProgramShader, "lightPos"), 1, &lightPos[0]);
}

void createVertices(){
    vertexCount = 6 * widthTexture * heightTexture;
    vertices = new glm::vec3[vertexCount];
    glm::vec3 v0, v1, v2, v3;
    int k = 0;
    for(int i = 0; i < widthTexture; i++){
        for(int j = 0; j < heightTexture; j++){
            v0 = glm::vec3(i, 0, j);
            v1 = glm::vec3(i + 1, 0, j);
            v2 = glm::vec3(i + 1, 0, j + 1);
            v3 = glm::vec3(i, 0, j + 1);
            vertices[k] = v0;
            vertices[k + 1] = v1;
            vertices[k + 2] = v2;
            vertices[k + 3] = v0;
            vertices[k + 4] = v2,
            vertices[k + 5] = v3;
            k += 6;
        }
    }
}


static void keyPressCallback(GLFWwindow *win, int key, int scode, int act, int mods){
    bool adjustFullscreen = false;
    if(act == GLFW_PRESS || act == GLFW_REPEAT) {
        switch(key){
            case GLFW_KEY_R:
                heightFactor += 0.5;
                break;
            case GLFW_KEY_F:
                heightFactor -= 0.5;
                break;
            case GLFW_KEY_A:
                cameraCross = glm::rotate(cameraCross, 0.05f, cameraUp);
                cameraGaze = glm::rotate(cameraGaze, 0.05f, cameraUp);
                glm::normalize(cameraUp);
                glm::normalize(cameraGaze);
                glm::normalize(cameraCross);
                break;
            case GLFW_KEY_S:
                cameraUp = glm::rotate(cameraUp, 0.05f, cameraCross);
                cameraGaze = glm::rotate(cameraGaze, 0.05f, cameraCross);
                glm::normalize(cameraUp);
                glm::normalize(cameraGaze);
                glm::normalize(cameraCross);
                break;
            case GLFW_KEY_D:
                cameraCross = glm::rotate(cameraCross, -0.05f, cameraUp);
                cameraGaze = glm::rotate(cameraGaze, -0.05f, cameraUp);
                glm::normalize(cameraUp);
                glm::normalize(cameraGaze);
                glm::normalize(cameraCross);
                break;
            case GLFW_KEY_W:
                cameraUp = glm::rotate(cameraUp, -0.05f, cameraCross);
                cameraGaze = glm::rotate(cameraGaze, -0.05f, cameraCross);
                glm::normalize(cameraUp);
                glm::normalize(cameraGaze);
                glm::normalize(cameraCross);
                break;
            case GLFW_KEY_Y:
                cameraSpeed += 0.01;
                break;
            case GLFW_KEY_H:
                cameraSpeed -= 0.01;
                break;
            case GLFW_KEY_X:
                cameraSpeed = 0;
                break;
            case GLFW_KEY_P:
                adjustFullscreen = true;
                break;
            case GLFW_KEY_I:
				cameraUp = glm::vec3(0.0, 1.0, 0.0);
				cameraGaze = glm::vec3(0.0, 0.0, 1.0);
				cameraCross = glm::vec3(-1.0, 0.0, 0.0);
                cameraPosition = glm::vec3(widthTexture/2, widthTexture/10, -widthTexture/4);
  				lightPos = glm::vec3(widthTexture/2, 100, heightTexture/2);
                cameraSpeed = 0;
                fovy = 45;
                aspectRatio = 1;
                near = 0.1;
                far = 1000;
                heightFactor = 10;
                break;
            case GLFW_KEY_Q:
            	cameraPosition += cameraCross;
                break;
            case GLFW_KEY_E:
            	cameraPosition -= cameraCross;
                break;
            case GLFW_KEY_T:
            	lightPos.y += 5;
                break;
            case GLFW_KEY_G:
            	lightPos.y -= 5;
                break;
            case GLFW_KEY_UP:
            	lightPos.z += 5;
                break;
            case GLFW_KEY_DOWN:
            	lightPos.z -= 5;
                break;
            case GLFW_KEY_LEFT:
            	lightPos.x += 5;
                break;
            case GLFW_KEY_RIGHT:
            	lightPos.x -= 5;
                break;
        }
    }
    if(adjustFullscreen){
        adjustFullscreen = false;
        if(isFullscreen){
            isFullscreen = false;
            glfwSetWindowMonitor(win, nullptr, 0, 0, prevWidthDisplay, prevHeightDisplay, 0);
        } else {
            isFullscreen = true;
            prevWidthDisplay = widthDisplay;
            prevHeightDisplay = heightDisplay;
            glfwSetWindowMonitor(win, primaryMonitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
        }
    }
}

static void changeWindowSizeCallBack(GLFWwindow *win, int width, int height) {
    widthDisplay = width;
    heightDisplay = height;
}

int main(int argc, char * argv[]) {

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(widthDisplay, heightDisplay, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    glfwTerminate();
    exit(-1);
  }

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], &widthTexture, &heightTexture, 1);
  initTexture(argv[2], &widthTexture, &heightTexture, 0);

  cameraPosition = glm::vec3(widthTexture/2, widthTexture/10, -widthTexture/4);
  lightPos = glm::vec3(widthTexture/2, 100, heightTexture/2);

  glfwSetKeyCallback(win, keyPressCallback);
  glfwSetWindowSizeCallback(win, changeWindowSizeCallBack);
  createVertices();
  glEnable(GL_DEPTH_TEST);
  primaryMonitor = glfwGetPrimaryMonitor();
  vidmode = glfwGetVideoMode(primaryMonitor);

  while (!glfwWindowShouldClose(win)) {
    adjustShaderDetails();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}
