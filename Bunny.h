
/*#ifndef CENG477_HOMEWORK3_BUNNY_H
#define CENG477_HOMEWORK3_BUNNY_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    GLfloat x, y, z;
    Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}
};

struct Texture {
    GLfloat u, v;
    Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) {}
};

struct Normal {
    GLfloat x, y, z;
    Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}
};

struct Face {
    GLuint vIndex[3], tIndex[3], nIndex[3];
    Face(int v[], int t[], int n[]) {
        for (int i = 0; i < 3; ++i) {
            vIndex[i] = v[i];
            tIndex[i] = t[i];
            nIndex[i] = n[i];
        }
    }
};

class Bunny {
public:
    Bunny();
    ~Bunny();

    void draw();
    void update();
    bool loadModel(const std::string& fileName);
    void setupMesh();
    void initVBO();
    void updatePosition(); // Method to update the bunny's position
    void display(int programIndex); // Method to display the bunny
    void setShaderProgram(GLuint program);
    void setModelMatrixLocation(GLint loc);

    GLuint VAO, VBO, TBO, NBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
    std::vector<Normal> normals;
    std::vector<Face> faces;
    glm::mat4 projectionMatrix;
    glm::mat4 viewingMatrix;
    glm::mat4 modelingMatrix;
    glm::vec3 eyePos(0, 0, 0);
    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLint gInVertexLoc, gInNormalLoc;
    int gVertexDataSizeInBytes, gNormalDataSizeInBytes;


    double positionX, positionY, positionZ;
    double velocityX, velocityY, velocityZ;
    const double jumpVelocity;
    int numIndices;
    const double gravity = -0.01;
    GLint modelMatrixLoc; // Add this line



};

#endif //CENG477_HOMEWORK3_BUNNY_H
 */
