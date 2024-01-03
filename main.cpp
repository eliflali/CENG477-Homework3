#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLuint gProgram[2];
GLuint groundProgram;

GLint modelingMatrixLoc[2];
GLint viewingMatrixLoc[2];
GLint projectionMatrixLoc[2];
GLint eyePosLoc[2];

GLint groundModelingMatrixLoc;
GLint groundViewingMatrixLoc;
GLint groundProjectionMatrixLoc;
GLint groundEyePosLoc;

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;
glm::vec3 eyePos(0, 0, 0);

glm::mat4 groundProjectionMatrix;
glm::mat4 groundViewingMatrix;
glm::mat4 groundModelingMatrix;


int activeProgramIndex = 0;

const double gravity = -0.01;

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

struct Face {
    Face(int v[], int t[], int n[]) {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];

        if (t != nullptr) {  // Check if texture indices are provided
            tIndex[0] = t[0];
            tIndex[1] = t[1];
            tIndex[2] = t[2];
        } else {
            tIndex[0] = tIndex[1] = tIndex[2] = 0;  // Safe default value
        }

        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    GLuint vIndex[3], tIndex[3], nIndex[3];
};


struct Bunny
{

    double positionX = 0;
    double positionY = 0;
    double positionZ = 0;

    double velocityX = 0;
    double velocityY = 0;
    double velocityZ = 0;

    const double jumpVelocity = 0.2;

    vector<Vertex> gVertices;
    vector<Texture> gTextures;
    vector<Normal> gNormals;
    vector<Face> gFaces;

    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLint gInVertexLoc, gInNormalLoc;
    int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

};
Bunny bunny;

// Assuming you have the same structures (Vertex, Texture, Normal, Face) as in your Bunny class
struct Quad {
    vector<Vertex> gVertices;
    vector<Texture> gTextures;
    vector<Normal> gNormals;
    vector<Face> gFaces;

    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLint gInVertexLoc, gInNormalLoc;
    int gVertexDataSizeInBytes, gNormalDataSizeInBytes;
};
Quad  quad;

bool ParseQuad (const string& fileName) {
    fstream myfile;

    // Open the input file
    myfile.open(fileName.c_str(), std::ios::in);

    // Check if the file is successfully opened
    if (myfile.is_open()) {
        string curLine;

        // Read the file line by line
        while (getline(myfile, curLine)) {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            // Check if the line is non-empty
            if (curLine.length() >= 2) {
                // Process vertex data
                if (curLine[0] == 'v') {
                    if (curLine[1] == 't') { // Texture coordinate
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        quad.gTextures.push_back(Texture(c1, c2));
                    } else if (curLine[1] == 'n') { // Normal vector
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        quad.gNormals.push_back(Normal(c1, c2, c3));
                    } else { // Vertex position
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        quad.gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                    // Process face data
                else if (curLine[0] == 'f') {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    // Parse indices of vertex/texture/normal for each vertex of the face
                    str >> vIndex[0];
                    str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1];
                    str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2];
                    str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    // Assert to check that vertex and normal indices are matching
                    assert(vIndex[0] == nIndex[0] &&
                           vIndex[1] == nIndex[1] &&
                           vIndex[2] == nIndex[2]); // a limitation for now

                    // Adjust indices to be 0-based instead of 1-based
                    for (int c = 0; c < 3; ++c) {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    // Add the face data
                    quad.gFaces.push_back(Face(vIndex, tIndex, nIndex));
                } else {
                    // Ignore lines that are not vertex, texture, normal, or face definitions
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }
        }

        myfile.close();
    } else {
        return false; // Return false if file couldn't be opened
    }
    assert(quad.gVertices.size() == quad.gNormals.size());

    return true;
}

// Function to parse a Wavefront .obj file
bool ParseObj(const string& fileName) {
    fstream myfile;

    // Open the input file
    myfile.open(fileName.c_str(), std::ios::in);

    // Check if the file is successfully opened
    if (myfile.is_open()) {
        string curLine;

        // Read the file line by line
        while (getline(myfile, curLine)) {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            // Check if the line is non-empty
            if (curLine.length() >= 2) {
                // Process vertex data
                if (curLine[0] == 'v') {
                    if (curLine[1] == 't') { // Texture coordinate
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        bunny.gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') { // Normal vector
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        bunny.gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else { // Vertex position
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        bunny.gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                    // Process face data
                else if (curLine[0] == 'f') {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    // Parse indices of vertex/texture/normal for each vertex of the face
                    str >> vIndex[0]; str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1]; str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2]; str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    // Assert to check that vertex and normal indices are matching
                    assert(vIndex[0] == nIndex[0] &&
                           vIndex[1] == nIndex[1] &&
                           vIndex[2] == nIndex[2]); // a limitation for now

                    // Adjust indices to be 0-based instead of 1-based
                    for (int c = 0; c < 3; ++c) {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    // Add the face data
                    bunny.gFaces.push_back(Face(vIndex, tIndex, nIndex));
                }
                else {
                    // Ignore lines that are not vertex, texture, normal, or face definitions
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }
        }

        myfile.close();
    } else {
        return false; // Return false if file couldn't be opened
    }

    /*
    // The commented section would calculate normals for each vertex
    // if it were not commented out. This is typically used when
    // the OBJ file doesn't contain normal data.
    */

    // Check that the number of vertices is equal to the number of normals
    assert(bunny.gVertices.size() == bunny.gNormals.size());

    return true; // Return true if parsing was successful
}

bool ReadDataFromFile(const string& fileName, string& data)     ///< [out] The contents of the file
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


// Function to create and compile a vertex shader
GLuint createVS(const char* shaderName) {
    string shaderSource;

    // Store the filename of the shader
    string filename(shaderName);

    // Read the shader source code from the file
    if (!ReadDataFromFile(filename, shaderSource)) {
        // If the shader file cannot be read, print an error message and exit
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    // Get the length of the shader source code
    GLint length = shaderSource.length();
    // Convert the shader source code to a C-style string
    const GLchar* shader = (const GLchar*)shaderSource.c_str();

    // Create a new vertex shader object
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    // Set the source code in the shader
    glShaderSource(vs, 1, &shader, &length);
    // Compile the shader
    glCompileShader(vs);

    // Allocate a buffer to store the compile log
    char errorLog[1024] = { 0 };
    // Get the compile log
    glGetShaderInfoLog(vs, 1024, &length, errorLog);
    // Print the compile log
    printf("VS error log: %s\n", errorLog);

    // Return the shader object
    return vs;
}

// Function to create and compile a fragment shader
GLuint createFS(const char* shaderName) {
    string shaderSource;

    // Store the filename of the shader
    string filename(shaderName);

    // Read the shader source code from the file
    if (!ReadDataFromFile(filename, shaderSource)) {
        // If the shader file cannot be read, print an error message and exit
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    // Get the length of the shader source code
    GLint length = shaderSource.length();
    // Convert the shader source code to a C-style string
    const GLchar* shader = (const GLchar*)shaderSource.c_str();

    // Create a new fragment shader object
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    // Set the source code in the shader
    glShaderSource(fs, 1, &shader, &length);
    // Compile the shader
    glCompileShader(fs);

    // Allocate a buffer to store the compile log
    char errorLog[1024] = { 0 };
    // Get the compile log
    glGetShaderInfoLog(fs, 1024, &length, errorLog);
    // Print the compile log
    printf("FS error log: %s\n", errorLog);

    // Return the shader object
    return fs;
}

// Function to initialize shaders
void initQuadShaders() {
    // Create OpenGL shader programs
    groundProgram = glCreateProgram();

    // Create and compile shaders for both programs
    GLuint vs1 = createVS("groundVert.glsl"); // Create vertex shader from "vert.glsl"
    GLuint fs1 = createFS("groundFragment.glsl"); // Create fragment shader from "frag.glsl"

    // Attach the shaders to the first program
    glAttachShader(groundProgram, vs1); // Attach vertex shader
    glAttachShader(groundProgram, fs1); // Attach fragment shader

    // Link the first program
    glLinkProgram(groundProgram); // Link shaders together into the first program
    GLint status;
    glGetProgramiv(groundProgram, GL_LINK_STATUS, &status); // Check for link success

    // If linking failed, print error and exit
    if (status != GL_TRUE) {
        cout << "Program link failed" << endl;
        exit(-1);
    }

    // Retrieve and store the locations of uniform variables for both programs
    groundModelingMatrixLoc = glGetUniformLocation(groundProgram, "groundModelingMatrix");
    groundViewingMatrixLoc = glGetUniformLocation(groundProgram, "viewingMatrix");
    groundProjectionMatrixLoc = glGetUniformLocation(groundProgram, "projectionMatrix");
    groundEyePosLoc = glGetUniformLocation(groundProgram, "eyePos");


}
void initBunnyShaders() {
    // Create OpenGL shader programs
    gProgram[0] = glCreateProgram();
    gProgram[1] = glCreateProgram();

    // Create and compile shaders for both programs
    GLuint vs1 = createVS("vert.glsl"); // Create vertex shader from "vert.glsl"
    GLuint fs1 = createFS("frag.glsl"); // Create fragment shader from "frag.glsl"

    GLuint vs2 = createVS("vert2.glsl"); // Create vertex shader from "vert2.glsl"
    GLuint fs2 = createFS("frag2.glsl"); // Create fragment shader from "frag2.glsl"

    // Attach the shaders to the first program
    glAttachShader(gProgram[0], vs1); // Attach vertex shader
    glAttachShader(gProgram[0], fs1); // Attach fragment shader

    // Attach the shaders to the second program
    glAttachShader(gProgram[1], vs2); // Attach vertex shader
    glAttachShader(gProgram[1], fs2); // Attach fragment shader

    // Link the first program
    glLinkProgram(gProgram[0]); // Link shaders together into the first program
    GLint status;
    glGetProgramiv(gProgram[0], GL_LINK_STATUS, &status); // Check for link success

    // If linking failed, print error and exit
    if (status != GL_TRUE) {
        cout << "Program link failed" << endl;
        exit(-1);
    }

    // Link the second program
    glLinkProgram(gProgram[1]); // Link shaders together into the second program
    glGetProgramiv(gProgram[1], GL_LINK_STATUS, &status); // Check for link success

    // If linking failed, print error and exit
    if (status != GL_TRUE) {
        cout << "Program link failed" << endl;
        exit(-1);
    }

    // Retrieve and store the locations of uniform variables for both programs
    for (int i = 0; i < 2; ++i) {
        modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
        viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
        projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
        eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
    }
}
void initGroundVBO(){
    assert(glGetError() == GL_NONE); // Check for OpenGL errors
    GLuint vao;

    // Generate a new Vertex Array Object (VAO) and bind it
    glGenVertexArrays(1, &vao);
    assert(vao > 0); // Check that VAO was successfully created
    glBindVertexArray(vao); // Bind the VAO
    cout << "vao = " << vao << endl;

    // Enable vertex attribute arrays at location 0 and 1
    glEnableVertexAttribArray(0); // Typically for vertex positions
    glEnableVertexAttribArray(1); // Typically for vertex normals
    assert(glGetError() == GL_NONE); // Check for OpenGL errors

    // Generate buffer IDs for vertex attribute buffer and index buffer
    glGenBuffers(1, &quad.gVertexAttribBuffer); // Buffer for vertex attributes
    glGenBuffers(1, &quad.gIndexBuffer); // Buffer for indices
    assert(quad.gVertexAttribBuffer > 0 && quad.gIndexBuffer > 0); // Check buffers were successfully created

    // Bind the vertex attribute buffer and the index buffer
    glBindBuffer(GL_ARRAY_BUFFER, quad.gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.gIndexBuffer);

    // Calculate the size in bytes of the vertex data and normals
    quad.gVertexDataSizeInBytes = quad.gVertices.size() * 3 * sizeof(GLfloat);
    quad.gNormalDataSizeInBytes = quad.gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytes = quad.gFaces.size() * 3 * sizeof(GLuint);

    // Allocate memory for vertex, normal, and index data
    GLfloat* vertexData = new GLfloat[quad.gVertices.size() * 3];
    GLfloat* normalData = new GLfloat[quad.gNormals.size() * 3];
    GLuint* indexData = new GLuint[quad.gFaces.size() * 3];

    // Variables to store the min and max values of the vertex positions
    float minX = 1e6, maxX = -1e6;
    float minY = 1e6, maxY = -1e6;
    float minZ = 1e6, maxZ = -1e6;

    // Copy vertex positions to the allocated array and find min/max values
    for(int i = 0; i < quad.gVertices.size(); ++i) {
        vertexData[3 * i] = quad.gVertices[i].x;
        vertexData[3 * i + 1] = quad.gVertices[i].y;
        vertexData[3 * i + 2] = quad.gVertices[i].z;

        // Updating min/max values for each coordinate
        minX = std::min(minX, quad.gVertices[i].x);
        maxX = std::max(maxX, quad.gVertices[i].x);
        minY = std::min(minY, quad.gVertices[i].y);
        maxY = std::max(maxY, quad.gVertices[i].y);
        minZ = std::min(minZ, quad.gVertices[i].z);
        maxZ = std::max(maxZ, quad.gVertices[i].z);
    }
    for(int i = 0; i < quad.gNormals.size(); ++i) {
        normalData[3 * i] = quad.gNormals[i].x;
        normalData[3 * i + 1] = quad.gNormals[i].y;
        normalData[3 * i + 2] = quad.gNormals[i].z;
    }
    for(int i = 0; i < quad.gFaces.size(); ++i) {
        indexData[3 * i] = quad.gFaces[i].vIndex[0];
        indexData[3 * i + 1] = quad.gFaces[i].vIndex[1];
        indexData[3 * i + 2] = quad.gFaces[i].vIndex[2];
    }
    //Upload vertex and normal data to the GPU
    glBufferData(GL_ARRAY_BUFFER, quad.gVertexDataSizeInBytes + quad.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, quad.gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, quad.gVertexDataSizeInBytes, quad.gNormalDataSizeInBytes, normalData);
    //Upload index data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);
    //Free the CPU-side memory as it's no longer needed
    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;
    //Define the layout of the vertex data in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // For vertex positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(quad.gVertexDataSizeInBytes)); // For vertex normals
}
void initBunnyVBO() {
    assert(glGetError() == GL_NONE); // Check for OpenGL errors
    GLuint vao;

    // Generate a new Vertex Array Object (VAO) and bind it
    glGenVertexArrays(1, &vao);
    assert(vao > 0); // Check that VAO was successfully created
    glBindVertexArray(vao); // Bind the VAO
    cout << "vao = " << vao << endl;

    // Enable vertex attribute arrays at location 0 and 1
    glEnableVertexAttribArray(0); // Typically for vertex positions
    glEnableVertexAttribArray(1); // Typically for vertex normals
    assert(glGetError() == GL_NONE); // Check for OpenGL errors

    // Generate buffer IDs for vertex attribute buffer and index buffer
    glGenBuffers(1, &bunny.gVertexAttribBuffer); // Buffer for vertex attributes
    glGenBuffers(1, &bunny.gIndexBuffer); // Buffer for indices
    assert(bunny.gVertexAttribBuffer > 0 && bunny.gIndexBuffer > 0); // Check buffers were successfully created

    // Bind the vertex attribute buffer and the index buffer
    glBindBuffer(GL_ARRAY_BUFFER, bunny.gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunny.gIndexBuffer);

    // Calculate the size in bytes of the vertex data and normals
    bunny.gVertexDataSizeInBytes = bunny.gVertices.size() * 3 * sizeof(GLfloat);
    bunny.gNormalDataSizeInBytes = bunny.gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytes = bunny.gFaces.size() * 3 * sizeof(GLuint);

    // Allocate memory for vertex, normal, and index data
    GLfloat* vertexData = new GLfloat[bunny.gVertices.size() * 3];
    GLfloat* normalData = new GLfloat[bunny.gNormals.size() * 3];
    GLuint* indexData = new GLuint[bunny.gFaces.size() * 3];

    // Variables to store the min and max values of the vertex positions
    float minX = 1e6, maxX = -1e6;
    float minY = 1e6, maxY = -1e6;
    float minZ = 1e6, maxZ = -1e6;

    // Copy vertex positions to the allocated array and find min/max values
    for (int i = 0; i < bunny.gVertices.size(); ++i) {
        vertexData[3 * i] = bunny.gVertices[i].x;
        vertexData[3 * i + 1] = bunny.gVertices[i].y;
        vertexData[3 * i + 2] = bunny.gVertices[i].z;

        // Updating min/max values for each coordinate
        minX = std::min(minX, bunny.gVertices[i].x);
        maxX = std::max(maxX, bunny.gVertices[i].x);
        minY = std::min(minY, bunny.gVertices[i].y);
        maxY = std::max(maxY, bunny.gVertices[i].y);
        minZ = std::min(minZ, bunny.gVertices[i].z);
        maxZ = std::max(maxZ, bunny.gVertices[i].z);
    }

    // Log min/max values for debugging
    std::cout << "minX = " << minX << std::endl;
    std::cout << "maxX = " << maxX << std::endl;
    std::cout << "minY = " << minY << std::endl;
    std::cout << "maxY = " << maxY << std::endl;
    std::cout << "minZ = " << minZ << std::endl;
    std::cout << "maxZ = " << maxZ << std::endl;

    // Copy normal data to the allocated array
    for (int i = 0; i < bunny.gNormals.size(); ++i) {
        normalData[3 * i] = bunny.gNormals[i].x;
        normalData[3 * i + 1] = bunny.gNormals[i].y;
        normalData[3 * i + 2] = bunny.gNormals[i].z;
    }

    // Copy index data for each face to the allocated array
    for (int i = 0; i < bunny.gFaces.size(); ++i) {
        indexData[3 * i] = bunny.gFaces[i].vIndex[0];
        indexData[3 * i + 1] = bunny.gFaces[i].vIndex[1];
        indexData[3 * i + 2] = bunny.gFaces[i].vIndex[2];
    }

    // Upload vertex and normal data to the GPU
    glBufferData(GL_ARRAY_BUFFER, bunny.gVertexDataSizeInBytes + bunny.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bunny.gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, bunny.gVertexDataSizeInBytes, bunny.gNormalDataSizeInBytes, normalData);

    // Upload index data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

    // Free the CPU-side memory as it's no longer needed
    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;

    // Define the layout of the vertex data in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // For vertex positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(bunny.gVertexDataSizeInBytes)); // For vertex normals
}

void init()
{
    //ParseObj("armadillo.obj");
    ParseObj("hw3_support_files/bunny.obj");
    ParseQuad("hw3_support_files/quad.obj");
    std::cout << quad.gNormals.size() << std::endl;
    std::cout << quad.gVertices.size() << std::endl;
    std::cout << quad.gFaces.size() << std::endl;
    std::cout << quad.gTextures.size() << std::endl;
    std::cout << quad.gFaces[0].vIndex[0] << std::endl;
    glEnable(GL_DEPTH_TEST);
    initBunnyShaders();
    initQuadShaders();
    std::cout << "bunny shader initialized" <<std::endl;

    std::cout << "ground shader initialized" <<std::endl;
    //initBunnyVBO();
    std::cout << "bunny vbo initialized" <<std::endl;
    //initGroundVBO();
    std::cout << "ground vbo initialized" <<std::endl;
}

void drawBunnyModel()
{
    //glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(bunny.gVertexDataSizeInBytes));
    //initBunnyVBO();
    std::cout << "draw bunny 615" <<std::endl;
    glDrawElements(GL_TRIANGLES, bunny.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}
void drawGroundModel()
{
    //glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(bunny.gVertexDataSizeInBytes));
    //initBunnyVBO();
    std::cout << "draw ground 615" <<std::endl;
    glDrawElements(GL_TRIANGLES, quad.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}


void bunnyJump()
{
    bunny.positionY += bunny.velocityY;
    bunny.velocityY += gravity;

    if(bunny.positionY  <= 0.0)
    {
        bunny.positionY  = 0.0;
        bunny.velocityY = bunny.jumpVelocity;
    }
}

void displayBunny() {
    // Set the clear color and clear depth and stencil buffers
    initBunnyVBO();

    std::cout << "display bunny 648" <<std::endl;
    static float angle = 0;   // Static angle variable to keep track of rotation
    bunnyJump();
    // Convert angle to radians for rotation
    float angleRad = (float)(angle / 180.0) * M_PI;
    std::cout << "display bunny 653" <<std::endl;
    // Compute the modeling matrix (transformation matrix for the object)
    glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, -6.0, -3.0)); // Translation
    glm::mat4 matThop = glm::translate(glm::mat4(1.0), glm::vec3(0.f, bunny.positionY, -4.0)); //bunny hop
    glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));      // Scaling
    glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-90. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0)); // Rotation around Y
    //glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(0.0, 0.0, 1.0)); // Rotation around Z
    std::cout << "display bunny 660" <<std::endl;
    //Translatipn -> Rotation -> Scaling
    modelingMatrix = matT* matThop * matR ; // Combine transformations
    std::cout << "display bunny 663" <<std::endl;

    // Set the active shader program and update its uniform variables
    glUseProgram(gProgram[activeProgramIndex]);
    glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
    glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
    std::cout << "display bunny 675" <<std::endl;
    // Draw the model
    drawBunnyModel();
    std::cout << "display bunny 678" <<std::endl;
    // Update the angle for the next frame
    angle += 0.9;
}
void displayQuad(){
    initGroundVBO();
    glClearColor(1, 1, 0, 1); // Set the clear color to black
    //Translatipn -> Rotation -> Scaling

    // Set the active shader program and update its uniform variables
    glUseProgram(groundProgram);

    // Set the position of the ground relative to the world origin
    glm::vec3 groundPosition = glm::vec3(0.0f, -1.0f, 0.0f); // Example: 1 unit below the origin

// Set the scale of the ground.
    glm::vec3 groundScale = glm::vec3(100.0f, 100.0f, 1.0f); // Example: 10 units wide and long

// Set the angle of slope for the road
    float slopeAngle = 10.0f; // Slope angle in degrees. Adjust as needed.

// Create rotation matrix for sloping the ground
    glm::mat4 matR = glm::rotate(glm::mat4(1.0f), glm::radians(slopeAngle), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotating along the X-axis

// Create the modeling matrix for the ground
    groundModelingMatrix = glm::translate(glm::mat4(1.0f), groundPosition) // First translate
                    * matR                                       // Then rotate
                    * glm::scale(glm::mat4(1.0f), groundScale);   // Finally, scale


    // Use the same projection, viewing, and modeling matrices as the bunny
    glUniformMatrix4fv(groundProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(groundViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    //glm::mat4 groundModelingMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.f, -1.0f, 0.f)); // Adjust as needed
    glUniformMatrix4fv(groundModelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(groundModelingMatrix));
    glUniform3fv(groundEyePosLoc, 1, glm::value_ptr(eyePos));

    // Set the offset and scale for the checkerboard pattern
    GLfloat offsetValue = 0.5f; // Adjust as needed
    GLfloat scaleValue = 10.0f; // Adjust as needed
    GLint offsetLocation = glGetUniformLocation(groundProgram, "offset");
    GLint scaleLocation = glGetUniformLocation(groundProgram, "scale");
    glUniform1f(offsetLocation, offsetValue);
    glUniform1f(scaleLocation, scaleValue);

    // Draw the ground model
    drawGroundModel();

}
void display()
{
    assert(glGetError() == GL_NONE); // Check for OpenGL errors
    glClearColor(0, 0, 0, 1); // Set the clear color to black
    glClearDepth(1.0f);       // Set the depth to the farthest
    glClearStencil(0);        // Set the stencil buffer to 0
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear buffers

    displayBunny();
    std::cout<<"bunny displayed"<<std::endl;
    displayQuad();
    assert(glGetError() == GL_NONE); // Check for OpenGL errors
    std::cout<<"ground displayed"<<std::endl;
}
void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

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
    //lookAt(eye, center, upvector)

}
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        activeProgramIndex = 0;
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        activeProgramIndex = 1;
    }
    else if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        glShadeModel(GL_FLAT);
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
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
    }
}

// Function to run the main rendering loop
void mainLoop(GLFWwindow* window) {
    // Loop until the window is instructed to close

    while (!glfwWindowShouldClose(window)) {
        // Call the display function to render the scene
        display();

        // Swap the front and back buffers
        // GLFW uses double buffering to avoid flickering and tearing artifacts
        glfwSwapBuffers(window);

        // Poll for and process events like keyboard and mouse input
        glfwPollEvents();
    }
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    std::cout << "line 819" << std::endl;
    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this if on MacOS
    std::cout << "line 833" << std::endl;
    int width = 1000, height = 800;
    window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    std::cout << "line 845" << std::endl;
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
    init();

    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, width, height); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}