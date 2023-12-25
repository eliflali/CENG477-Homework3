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

//#include "dataTypes.h"
//#include "fileOperations.h"
//#include "Shaders.h"
//#include "Callbacks.h"
//#include "gameLogic.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;
GLuint gBunnyShaderProgram;
int gWidth, gHeight;

GLint modelingMatrixLoc;
GLint viewingMatrixLoc;
GLint projectionMatrixLoc;
GLint eyePosLoc;

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;
glm::vec3 eyePos(0, 0, 0);

int activeProgramIndex = 0;




GLuint gVertexAttribBuffer, gIndexBuffer;
GLint gInVertexLoc, gInNormalLoc;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;


// Function declarations
void initializeGame();
void updateGame();
void renderGame();
void handleInput(GLFWwindow* window);
void checkCollisions();
// ... Other necessary functions
struct Vertex
{
    Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Texture
{
    Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
    GLfloat u, v;
};

struct Normal
{
    Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Face
{
    Face(int v[], int t[], int n[]) {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];
        tIndex[0] = t[0];
        tIndex[1] = t[1];
        tIndex[2] = t[2];
        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    GLuint vIndex[3], tIndex[3], nIndex[3];
};
struct Bunny {
    glm::vec3 position;
    float speed;
    // Other bunny properties...
};

Bunny bunny;

vector<Vertex> gVertices;
vector<Texture> gTextures;
vector<Normal> gNormals;
vector<Face> gFaces;

bool ParseObj(const string& fileName)
{
    fstream myfile;

    // Open the input
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            if (curLine.length() >= 2)
            {
                if (curLine[0] == 'v')
                {
                    if (curLine[1] == 't') // texture
                    {
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') // normal
                    {
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else // vertex
                    {
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                else if (curLine[0] == 'f') // face
                {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    str >> vIndex[0]; str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1]; str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2]; str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    assert(vIndex[0] == nIndex[0] &&
                           vIndex[1] == nIndex[1] &&
                           vIndex[2] == nIndex[2]); // a limitation for now

                    // make indices start from 0
                    for (int c = 0; c < 3; ++c)
                    {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    gFaces.push_back(Face(vIndex, tIndex, nIndex));
                }
                else
                {
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }

            //data += curLine;
            if (!myfile.eof())
            {
                //data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    /*
    for (int i = 0; i < gVertices.size(); ++i)
    {
        Vector3 n;

        for (int j = 0; j < gFaces.size(); ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                if (gFaces[j].vIndex[k] == i)
                {
                    // face j contains vertex i
                    Vector3 a(gVertices[gFaces[j].vIndex[0]].x,
                              gVertices[gFaces[j].vIndex[0]].y,
                              gVertices[gFaces[j].vIndex[0]].z);

                    Vector3 b(gVertices[gFaces[j].vIndex[1]].x,
                              gVertices[gFaces[j].vIndex[1]].y,
                              gVertices[gFaces[j].vIndex[1]].z);

                    Vector3 c(gVertices[gFaces[j].vIndex[2]].x,
                              gVertices[gFaces[j].vIndex[2]].y,
                              gVertices[gFaces[j].vIndex[2]].z);

                    Vector3 ab = b - a;
                    Vector3 ac = c - a;
                    Vector3 normalFromThisFace = (ab.cross(ac)).getNormalized();
                    n += normalFromThisFace;
                }

            }
        }

        n.normalize();

        gNormals.push_back(Normal(n.x, n.y, n.z));
    }
    */

    assert(gVertices.size() == gNormals.size());

    return true;
}

bool ReadDataFromFile(
        const string& fileName, ///< [in]  Name of the shader file
        string& data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        //moveBunnyLeft();
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        //moveBunnyRight();
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        //restart();
    }
    /*else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        glShadeModel(GL_SMOOTH);
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }*/
}

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);

    // Use perspective projection
    float fovyRad = (float)(90.0 / 180.0) * M_PI;
    projectionMatrix = glm::perspective(fovyRad, w / (float)h, 1.0f, 100.0f);

    // Assume default camera position and orientation (camera is at
    // (0, 0, 0) with looking at -z direction and its up vector pointing
    // at +y direction)
    //
    //viewingMatrix = glm::mat4(1);
    viewingMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

}


GLuint createVS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*)shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = { 0 };
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

    return vs;
}

GLuint createFS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*)shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = { 0 };
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

    return fs;
}

void initShaders()
{
    // Create the program
    std::cout<<"create program -> 376" << std::endl;
    gBunnyShaderProgram = glCreateProgram();

    // Create the shaders for program
    std::cout<<"create shaders -> 380" << std::endl;
    GLuint vertexShader = createVS("./vertexShader.vert");
    GLuint fragmentShader = createFS("./fragmentShader.frag");

    // Attach the shaders to the program
    std::cout<<"attach shaders -> 385" << std::endl;
    glAttachShader(gBunnyShaderProgram, vertexShader);
    glAttachShader(gBunnyShaderProgram, fragmentShader);

    // Link the program
    std::cout<<"link program -> 390" << std::endl;
    glLinkProgram(gBunnyShaderProgram);


    GLint isLinked;
    std::cout<<"link status -> 394" << std::endl;
    glGetProgramiv(gBunnyShaderProgram, GL_LINK_STATUS, &isLinked);

    if (isLinked != GL_TRUE)
    {
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(gBunnyShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(gBunnyShaderProgram, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(gBunnyShaderProgram);

            // Use the infoLog as you see fit.
            std::cerr << "Shader Program Linking Failed:\n" << &infoLog[0] << std::endl;

            // In this simple program, we'll just leave
            return;
        }
    }

    std::cout<<"matrix locations -> 403" << std::endl;
    modelingMatrixLoc = glGetUniformLocation(gBunnyShaderProgram, "modelingMatrix");
    viewingMatrixLoc = glGetUniformLocation(gBunnyShaderProgram, "viewingMatrix");
    projectionMatrixLoc = glGetUniformLocation(gBunnyShaderProgram, "projectionMatrix");
    eyePosLoc = glGetUniformLocation(gBunnyShaderProgram, "eyePos");

    std::cout<<"init game finish -> 409" << std::endl;

}

void initVBO()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);
    cout << "vao = " << vao << endl;

    glEnableVertexAttribArray(0); // Vertex position attribute
    glEnableVertexAttribArray(1); // Normal attribute
    assert(glGetError() == GL_NONE);

    glGenBuffers(1, &gVertexAttribBuffer); // Buffer for vertex attributes
    glGenBuffers(1, &gIndexBuffer); // Buffer for indices

    assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

    // Bind buffers
    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    // Calculate data sizes
    gVertexDataSizeInBytes = gVertices.size() * sizeof(Vertex);
    gNormalDataSizeInBytes = gNormals.size() * sizeof(Normal);
    int indexDataSizeInBytes = gFaces.size() * sizeof(Face);

    // Transfer vertex data to GPU
    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, gVertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, gNormals.data());

    // Transfer index data to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, gFaces.data(), GL_STATIC_DRAW);

    // Set attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

    // Unbind the VAO to prevent accidental changes
    //glBindVertexArray(0);

    std::cout << "init VBO finish line 454" << std::endl;
}


void drawModel()
{
    //glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

    glDrawElements(GL_TRIANGLES, gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}

void display()
{
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    static float angle = 0;
    float angleRad = (float)(angle / 180.0) * M_PI;

    // Compute the modeling matrix
    glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -3.f));
    glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));
    glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(0.0, 0.0, 1.0));
    modelingMatrix = matT * matRz * matR; // starting from right side, rotate around Y to turn back, then rotate around Z some more at each frame, then translate.
    /*for(int i=0;i<4;i++)
    {
        for (int j=0;j<4;j++)
        {
            std::cout<<modelingMatrix[i][i]<<std::endl;
        }
    }
    std::cout<<"gBunnyShaderProgram"<<std::endl;
    std::cout<<gBunnyShaderProgram<<std::endl;*/
    // Set the active program and the values of its uniform variables
    glUseProgram(gBunnyShaderProgram);
    /*for(int i=0;i<4;i++)
    {
        for (int j=0;j<4;j++)
        {
            std::cout<<projectionMatrix[i][i]<<std::endl;
        }
    }*/
    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glUniformMatrix4fv(viewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelingMatrix));
    glUniform3fv(eyePosLoc, 1, glm::value_ptr(eyePos));

    // Draw the scene
    drawModel();

    angle += 0.9;


}
void gameLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window)) {
        //Game rendering and logic:
        // Check for errors
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }

        // Clear the screen to red for debugging
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //handleInput(window);

        //updateGame();

        //renderGame();
        display();

        //glUseProgram(shaderProgram);
        // Bind VAO
        //glBindVertexArray(vao);

        // Draw call
        //glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}
int main() {
    // Initialize GLFW and create a window
    GLFWwindow* window; // Initialize window
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // uncomment this if on MacOS (sefim burayı sen commentle)

    int width = 1000, height = 800;
    window = glfwCreateWindow(width, height, "Bunny Run", NULL, NULL);
    std::cout<<window<<std::endl;

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    char rendererInfo[512] = { 0 };
    strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
    strcat(rendererInfo, " - "); // Use strcpy_s on Windows, strcpy on Linux
    strcat(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
    glfwSetWindowTitle(window, rendererInfo);


    initializeGame();
    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, width, height); // need to call this once ourselves

    // Main game loop
    gameLoop(window);
    // Cleanup and close
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void initializeGame() {
    ParseObj("./hw3_support_files/bunny.obj");

    bunny.position = glm::vec3(0, 0, 0); // Starting position random again
    bunny.speed = 0.1f; // Set initial speed random value

    glEnable(GL_DEPTH_TEST);
    initShaders();
    initVBO();
}

void updateGame() {
    // Update game logic
    //updateBunnySpeed();
    //updateBunnyPosition();
    //checkCollisions();
    // ... Other game updates
}

void renderGame() {
    // Render the game objects
}

void handleInput(GLFWwindow* window) {
    // Handle keyboard input for bunny movement
}

void updateBunnySpeed() {
    // Increase bunny speed gradually
}

void updateBunnyPosition() {
    // Update bunny position based on speed and input
}

void checkCollisions() {
    // Check for collisions with checkpoints
}
