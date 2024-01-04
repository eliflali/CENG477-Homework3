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
GLuint cubeProgram;

GLint modelingMatrixLoc[2];
GLint viewingMatrixLoc[2];
GLint projectionMatrixLoc[2];
GLint eyePosLoc[2];

GLint groundModelingMatrixLoc;
GLint groundViewingMatrixLoc;
GLint groundProjectionMatrixLoc;
GLint groundEyePosLoc;

GLint scaleLocation;
GLint offsetLocation;
GLint color1Location;
GLint color2Location;

int colorRandomizer[3];


/*cubeModelingMatrixLoc = glGetUniformLocation(cubeProgram, "modelingMatrix");
    cubeViewingMatrixLoc = glGetUniformLocation(cubeProgram, "viewingMatrix");
    cubeProjectionMatrixLoc = glGetUniformLocation(cubeProgram, "projectionMatrix");
    cubeEyePosLoc = glGetUniformLocation(cubeProgram, "eyePos");
    cubeScaleLocation = glGetUniformLocation(cubeProgram, "scale");
    cubeOffsetLocation = glGetUniformLocation(cubeProgram, "offset");
    cubeColor1Location = glGetUniformLocation(cubeProgram, "color");*/
GLint cubeModelingMatrixLoc;
GLint cubeViewingMatrixLoc;
GLint cubeProjectionMatrixLoc;
GLint cubeEyePosLoc;
GLint cubeScaleLocation;
GLint cubeOffsetLocation;
GLint cubeColor1Location;
GLint cubeLightColorLocation;
GLint cubeLightPosLocation;



glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;
glm::vec3 eyePos(0, 2, 0);
glm::vec3 offset(0.0,-2.0,-2.0);
glm::vec3 Coffset(0.0,-2.0,-2.0);

glm::mat4 groundProjectionMatrix;
glm::mat4 groundViewingMatrix;
glm::mat4 groundModelingMatrix;

glm::mat4 cubeProjectionMatrix;
glm::mat4 cubeViewingMatrix;
glm::mat4 cubeModelingMatrix;



GLuint vao;
GLuint vaoG;
GLuint vaoCube;

int activeProgramIndex = 0;

const double gravity = -0.0025;

void cubeRand()
{
    int randomYellow = rand() %3;
    for(int i = 0; i < 3; i++)
    {
        if(i == randomYellow)
        {
            colorRandomizer[i] = 1;
        }
        else
        {
            colorRandomizer[i] = 0;
        }
    }
}

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

    const double jumpVelocity = 0.05;

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
struct Cube{
    vector<Vertex> gVertices;
    vector<Texture> gTextures;
    vector<Normal> gNormals;
    vector<Face> gFaces;

    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLint gInVertexLoc, gInNormalLoc;
    int gVertexDataSizeInBytes, gNormalDataSizeInBytes;
};
Cube cube;
Cube cubeClones[5];
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
bool ParseCube(const string& fileName){
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
                        cube.gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') { // Normal vector
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        cube.gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else { // Vertex position
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        cube.gVertices.push_back(Vertex(c1, c2, c3));
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
                    cube.gFaces.push_back(Face(vIndex, tIndex, nIndex));
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
    assert(cube.gVertices.size() == cube.gNormals.size());

    return true;
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
	// Create the programs
// Create the programs

	gProgram[0] = glCreateProgram();
	gProgram[1] = glCreateProgram();

	// Create the shaders for both programs

	GLuint vs1 = createVS("vert.glsl");
	GLuint fs1 = createFS("frag.glsl");

	GLuint vs2 = createVS("vert2.glsl");
	GLuint fs2 = createFS("frag2.glsl");

	// Attach the shaders to the programs

	glAttachShader(gProgram[0], vs1);
	glAttachShader(gProgram[0], fs1);

	glAttachShader(gProgram[1], vs2);
	glAttachShader(gProgram[1], fs2);

	// Link the programs

	glLinkProgram(gProgram[0]);
	GLint status;
	glGetProgramiv(gProgram[0], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[1]);
	glGetProgramiv(gProgram[1], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	// Get the locations of the uniform variables from both programs

	for (int i = 0; i < 2; ++i)
	{
		modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
		viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
		projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
		eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
	}
    //ground time
    groundProgram = glCreateProgram();
    GLuint groundVS = createVS("groundVert.glsl");
    GLuint groundFS = createFS("groundFragment.glsl");
    glAttachShader(groundProgram, groundVS);
    glAttachShader(groundProgram, groundFS);
    glLinkProgram(groundProgram);
    glGetProgramiv(groundProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        cout << "Program link failed" << endl;
        exit(-1);
    }
    groundModelingMatrixLoc = glGetUniformLocation(groundProgram, "modelingMatrix");
    groundViewingMatrixLoc = glGetUniformLocation(groundProgram, "viewingMatrix");
    groundProjectionMatrixLoc = glGetUniformLocation(groundProgram, "projectionMatrix");
    groundEyePosLoc = glGetUniformLocation(groundProgram, "eyePos");
    scaleLocation = glGetUniformLocation(groundProgram, "scale");
    offsetLocation = glGetUniformLocation(groundProgram, "offset");
    color1Location = glGetUniformLocation(groundProgram, "color1");
    color2Location = glGetUniformLocation(groundProgram, "color2");
    //now for cube
    cubeProgram = glCreateProgram();
    GLuint cubeVS = createVS("cubeVert.glsl");
    GLuint cubeFS = createFS("cubeFrag.glsl");
    glAttachShader(cubeProgram, cubeVS);
    glAttachShader(cubeProgram, cubeFS);
    glLinkProgram(cubeProgram);
    glGetProgramiv(cubeProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        cout << "Program link failed" << endl;
        exit(-1);
    }
    cubeModelingMatrixLoc = glGetUniformLocation(cubeProgram, "modelingMatrix");
    cubeViewingMatrixLoc = glGetUniformLocation(cubeProgram, "viewingMatrix");
    cubeProjectionMatrixLoc = glGetUniformLocation(cubeProgram, "projectionMatrix");
    cubeEyePosLoc = glGetUniformLocation(cubeProgram, "eyePos");
    cubeColor1Location = glGetUniformLocation(cubeProgram, "objectColor");
    cubeLightPosLocation = glGetUniformLocation(cubeProgram, "lightPos");
    cubeScaleLocation = glGetUniformLocation(cubeProgram, "scale");
    cubeOffsetLocation = glGetUniformLocation(cubeProgram, "offset");
    cubeLightColorLocation = glGetUniformLocation(cubeProgram, "lightColor");
}

void initVBO()
{
	glGenVertexArrays(1, &vao);
	assert(vao > 0);
	glBindVertexArray(vao);
	cout << "vao = " << vao << endl;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &bunny.gVertexAttribBuffer);
	glGenBuffers(1, &bunny.gIndexBuffer);

	assert(bunny.gVertexAttribBuffer > 0 && bunny.gIndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, bunny.gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  bunny.gIndexBuffer);

	 bunny.gVertexDataSizeInBytes =  bunny.gVertices.size() * 3 * sizeof(GLfloat);
	 bunny.gNormalDataSizeInBytes =  bunny.gNormals.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes =  bunny.gFaces.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[ bunny.gVertices.size() * 3];
	GLfloat* normalData = new GLfloat[ bunny.gNormals.size() * 3];
	GLuint* indexData = new GLuint[ bunny.gFaces.size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int i = 0; i <  bunny.gVertices.size(); ++i)
	{
		vertexData[3 * i] =  bunny.gVertices[i].x;
		vertexData[3 * i + 1] =  bunny.gVertices[i].y;
		vertexData[3 * i + 2] =  bunny.gVertices[i].z;

	}

	std::cout << "minX = " << minX << std::endl;
	std::cout << "maxX = " << maxX << std::endl;
	std::cout << "minY = " << minY << std::endl;
	std::cout << "maxY = " << maxY << std::endl;
	std::cout << "minZ = " << minZ << std::endl;
	std::cout << "maxZ = " << maxZ << std::endl;

	for (int i = 0; i <  bunny.gNormals.size(); ++i)
	{
		normalData[3 * i] =  bunny.gNormals[i].x;
		normalData[3 * i + 1] =  bunny.gNormals[i].y;
		normalData[3 * i + 2] =  bunny.gNormals[i].z;
	}

	for (int i = 0; i <  bunny.gFaces.size(); ++i)
	{
		indexData[3 * i] =  bunny.gFaces[i].vIndex[0];
		indexData[3 * i + 1] =  bunny.gFaces[i].vIndex[1];
		indexData[3 * i + 2] =  bunny.gFaces[i].vIndex[2];
	}


	glBufferData(GL_ARRAY_BUFFER,  bunny.gVertexDataSizeInBytes +  bunny.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  bunny.gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER,  bunny.gVertexDataSizeInBytes,  bunny.gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying to GPU memory; can free now from CPU memory
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( bunny.gVertexDataSizeInBytes));

    //NOW SAME FOR QUAD
    glGenVertexArrays(1, &vaoG);
    assert(vaoG > 0);
    glBindVertexArray(vaoG);
    cout << "vaoG = " << vaoG << endl;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);
    glGenBuffers(1, &quad.gVertexAttribBuffer);
    glGenBuffers(1, &quad.gIndexBuffer);
    assert(quad.gVertexAttribBuffer > 0 && quad.gIndexBuffer > 0);
    glBindBuffer(GL_ARRAY_BUFFER, quad.gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  quad.gIndexBuffer);
    quad.gVertexDataSizeInBytes =  quad.gVertices.size() * 3 * sizeof(GLfloat);
    quad.gNormalDataSizeInBytes =  quad.gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytesG =  quad.gFaces.size() * 3 * sizeof(GLuint);
    GLfloat* vertexDataG = new GLfloat[ quad.gVertices.size() * 3];
    GLfloat* normalDataG = new GLfloat[ quad.gNormals.size() * 3];
    GLuint* indexDataG = new GLuint[ quad.gFaces.size() * 3];
    for(int i = 0; i < quad.gVertices.size(); ++i) {
        vertexDataG[3 * i] = quad.gVertices[i].x;
        vertexDataG[3 * i + 1] = quad.gVertices[i].y;
        vertexDataG[3 * i + 2] = quad.gVertices[i].z;
    }
    for(int i = 0; i < quad.gNormals.size(); ++i) {
        normalDataG[3 * i] = quad.gNormals[i].x;
        normalDataG[3 * i + 1] = quad.gNormals[i].y;
        normalDataG[3 * i + 2] = quad.gNormals[i].z;
    }
    for(int i = 0; i < quad.gFaces.size(); ++i) {
        indexDataG[3 * i] = quad.gFaces[i].vIndex[0];
        indexDataG[3 * i + 1] = quad.gFaces[i].vIndex[1];
        indexDataG[3 * i + 2] = quad.gFaces[i].vIndex[2];
    }
    glBufferData(GL_ARRAY_BUFFER, quad.gVertexDataSizeInBytes + quad.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, quad.gVertexDataSizeInBytes, vertexDataG);
    glBufferSubData(GL_ARRAY_BUFFER, quad.gVertexDataSizeInBytes, quad.gNormalDataSizeInBytes, normalDataG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexDataSizeInBytesG, indexDataG, GL_STATIC_DRAW);
    // done copying to GPU memory; can free now from CPU memory
    delete[] vertexDataG;
    delete[] normalDataG;
    delete[] indexDataG;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(quad.gVertexDataSizeInBytes));
 
    //NOW SAME FOR CUBE
    glGenVertexArrays(1, &vaoCube);
    assert(vaoCube > 0);
    glBindVertexArray(vaoCube);
    cout << "vaoCube = " << vaoCube << endl;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);
    glGenBuffers(1, &cube.gVertexAttribBuffer);
    glGenBuffers(1, &cube.gIndexBuffer);
    assert(cube.gVertexAttribBuffer > 0 && cube.gIndexBuffer > 0);
    glBindBuffer(GL_ARRAY_BUFFER, cube.gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  cube.gIndexBuffer);
    cube.gVertexDataSizeInBytes =  cube.gVertices.size() * 3 * sizeof(GLfloat); 
    cube.gNormalDataSizeInBytes =  cube.gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytesC =  cube.gFaces.size() * 3 * sizeof(GLuint);
    GLfloat* vertexDataC = new GLfloat[ cube.gVertices.size() * 3];
    GLfloat* normalDataC = new GLfloat[ cube.gNormals.size() * 3];
    GLint* indexDataC = new GLint[ cube.gFaces.size() * 3];
    for(int i = 0; i < cube.gVertices.size(); ++i) {
        vertexDataC[3 * i] = cube.gVertices[i].x;
        vertexDataC[3 * i + 1] = cube.gVertices[i].y;
        vertexDataC[3 * i + 2] = cube.gVertices[i].z;
    }
    for(int i = 0; i < cube.gNormals.size(); ++i) {
        normalDataC[3 * i] = cube.gNormals[i].x;
        normalDataC[3 * i + 1] = cube.gNormals[i].y;
        normalDataC[3 * i + 2] = cube.gNormals[i].z;
    }
    for(int i = 0; i < cube.gFaces.size(); ++i) {
        indexDataC[3 * i] = cube.gFaces[i].vIndex[0];
        indexDataC[3 * i + 1] = cube.gFaces[i].vIndex[1];
        indexDataC[3 * i + 2] = cube.gFaces[i].vIndex[2];
    }
    glBufferData(GL_ARRAY_BUFFER, cube.gVertexDataSizeInBytes + cube.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cube.gVertexDataSizeInBytes, vertexDataC);
    glBufferSubData(GL_ARRAY_BUFFER, cube.gVertexDataSizeInBytes, cube.gNormalDataSizeInBytes, normalDataC);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexDataSizeInBytesC, indexDataC, GL_STATIC_DRAW);
    // done copying to GPU memory; can free now from CPU memory
    delete[] vertexDataC;
    delete[] normalDataC;
    delete[] indexDataC;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube.gVertexDataSizeInBytes));
}
void init()
{
    //ParseObj("armadillo.obj");
    ParseObj("hw3_support_files/bunny.obj");
    ParseQuad("hw3_support_files/quad.obj");
    ParseCube("hw3_support_files/cube.obj");
    std::cout << cube.gVertices.size() << std::endl;
    std::cout << cube.gNormals.size() << std::endl;
    std::cout << cube.gFaces.size() << std::endl;
    std::cout << cube.gTextures.size() << std::endl;
    std::cout << cube.gFaces[0].vIndex[0] << std::endl;
    std::cout << cube.gVertexAttribBuffer << std::endl;
    /* 
    std::cout << quad.gNormals.size() << std::endl;
    std::cout << quad.gVertices.size() << std::endl;
    std::cout << quad.gFaces.size() << std::endl;
    std::cout << quad.gTextures.size() << std::endl;
    std::cout << quad.gFaces[0].vIndex[0] << std::endl;
    std::cout << quad.gVertexAttribBuffer << std::endl; */
    glEnable(GL_DEPTH_TEST);
    initShaders();
    initVBO();
    cubeRand();

    std::cout << "bunny shader initialized" <<std::endl; 

    std::cout << "ground shader initialized" <<std::endl;
}

void drawBunnyModel()
{
    //glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(bunny.gVertexDataSizeInBytes));
    //initBunnyVBO();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, bunny.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void drawGroundModel()
{
    glBindVertexArray(vaoG);
    glDrawElements(GL_TRIANGLES, quad.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
    static float angle = 0;   // Static angle variable to keep track of rotation
    bunnyJump();
    // Convert angle to radians for rotation
    float angleRad = (float)(angle / 180.0) * M_PI;
    // Compute the modeling matrix (transformation matrix for the object)
    glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, -1.0, 2.8)); // Translation
    glm::mat4 matThop = glm::translate(glm::mat4(1.0), glm::vec3(0.f, bunny.positionY, -4.0)); //bunny hop
    glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(0.15, 0.15, 0.15));      // Scaling
    glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-90. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0)); // Rotation around Y
    //glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(0.0, 0.0, 1.0)); // Rotation around Z
    //Translatipn -> Rotation -> Scaling
    modelingMatrix = matT* matThop * matR *matS ; // Combine transformations
    /*  */
    // Set the active shader program and update its uniform variables
    glUseProgram(gProgram[activeProgramIndex]);
    glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
    glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
    // Draw the model
    drawBunnyModel();
    // Update the angle for the next frame
    angle += 0.9;
}
void displayQuad(){

	float angleRad = (float)(10 / 180.0) * M_PI;
   
	// Compute the modeling matrix 
	glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, -2.f, -2.f));
	glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(3.0, 1.0, 1000.0));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(1.0, 0.0, 0.0));
	groundModelingMatrix = matT * matS * matR * matRz; // starting from right side, rotate around Y to turn back, then rotate around Z some more at each frame, then translate.

	
    glUseProgram(groundProgram);

// Set the uniform values
    //GLint color1Location = glGetUniformLocation(groundProgram, "color1");
    //make color1 fuchsia
    glUniform3f(color1Location, 1.f,0.f,0.6f);
    //GLint color2Location = glGetUniformLocation(groundProgram, "color2");
    //make color2 turquoise
    glUniform3f(color2Location, 0.f,1.f,0.8f);
    //GLint scaleLocation = glGetUniformLocation(groundProgram, "scale");
    glUniform1f(scaleLocation,1);

    //GLint offsetLocation = glGetUniformLocation(groundProgram, "offset");

    glUniform3f(offsetLocation,offset.x,offset.y,offset.z);
    glUniformMatrix4fv(groundProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(groundViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(groundModelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(groundModelingMatrix));
    glUniform3fv(groundEyePosLoc, 0.7, glm::value_ptr(eyePos));
	// Draw the scene

	drawGroundModel();
    offset.z -= 0.5;

	//angle += 0.9;
}

void drawCube(){
    glDrawElements(GL_TRIANGLES, cube.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}

void displayCube(){
    float angleRad = (float)(10 / 180.0) * M_PI;
    glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(5, -2.f, -100.f));
	glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(.5, 5.0,0.3));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(1.0, 0.0, 0.0));

	cubeModelingMatrix = matT * matS ; // starting from right side, rotate around Y to turn back, then rotate around Z some more at each frame, then translate.
    for(int i =0 ; i<3 ; i++){
        cubeModelingMatrix = glm::translate(cubeModelingMatrix, glm::vec3(-5.f, 0.f, 0.f));
        if(colorRandomizer[i] == 0){
            //red
            glUniform3f(cubeColor1Location, 1.f,0.f,0.1f);
        }
        else{
            //make it yellow
            glUniform3f(cubeColor1Location, 1.f,1.f,0.1f);
        }
        glUniform3f(cubeLightPosLocation, 5.f,5.f,5.f);
        glUniform3f(cubeLightColorLocation, 1.f,1.f,1.f);
        glUniform1f(cubeScaleLocation,1);
        glUniform3f(cubeOffsetLocation,Coffset.x,Coffset.y,Coffset.z);
        glUniformMatrix4fv(cubeProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(cubeViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
        glUniformMatrix4fv(cubeModelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(cubeModelingMatrix));
        glUniform3fv(cubeEyePosLoc, 0.7, glm::value_ptr(eyePos));
        drawCube();
        std::cout << Coffset.z << "offset cubess"<< std::endl;
        if(Coffset.z >= 97.2){
            Coffset.z = -2 ;
            cubeRand();

        }
    }
            Coffset.z += 0.5;

}

void display()
{
    glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    displayBunny();
    displayQuad();
    glUseProgram(cubeProgram);
    glBindVertexArray(vaoCube);
    displayCube();
    glBindVertexArray(0);
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
    viewingMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0,2, 0));
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
    strcpy(rendererInfo, " - "); // Use strcpy_s on Windows, strcpy on Linux
    strcpy(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
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