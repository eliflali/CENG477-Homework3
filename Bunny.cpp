/*
#include "Bunny.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>

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
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

Bunny::Bunny()
        : positionX(0), positionY(0), positionZ(0),
          velocityX(0), velocityY(0), velocityZ(0),
          jumpVelocity(0.2), numIndices(0), modelMatrixLoc(-1) {

    if (!loadModel("hw3_support_files/bunny.obj")) {
        std::cerr << "Failed to load bunny model" << std::endl;
        exit(EXIT_FAILURE);
    }

    setupMesh();
}

Bunny::~Bunny() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &TBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

bool Bunny::loadModel(const std::string& fileName) {
    std::fstream myfile;

    // Open the input file
    myfile.open(fileName.c_str(), std::ios::in);

    // Check if the file is successfully opened
    if (!myfile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << fileName << std::endl;
        return false;
    }

    std::string curLine;

    // Read the file line by line
    while (getline(myfile, curLine)) {
        std::stringstream str(curLine);
        GLfloat c1, c2, c3;
        std::string tmp;

        // Process vertex data
        if (curLine[0] == 'v') {
            if (curLine[1] == 't') { // Texture coordinate
                str >> tmp >> c1 >> c2;
                textures.push_back(Texture(c1, c2));
            }
            else if (curLine[1] == 'n') { // Normal vector
                str >> tmp >> c1 >> c2 >> c3;
                normals.push_back(Normal(c1, c2, c3));
            }
            else { // Vertex position
                str >> tmp >> c1 >> c2 >> c3;
                vertices.push_back(Vertex(c1, c2, c3));
            }
        }
            // Process face data
        else if (curLine[0] == 'f') {
            str >> tmp; // consume "f"
            int vIndex[3], tIndex[3], nIndex[3];
            char c;

            for (int i = 0; i < 3; ++i) {
                str >> vIndex[i] >> c >> tIndex[i] >> c >> nIndex[i];
                vIndex[i] -= 1; // Adjust for 0-based indexing
                tIndex[i] -= 1; // Adjust for 0-based indexing
                nIndex[i] -= 1; // Adjust for 0-based indexing
            }
            faces.push_back(Face(vIndex, tIndex, nIndex));
        }
    }

    myfile.close();

    // Check that the number of vertices is equal to the number of normals
    assert(vertices.size() == normals.size());

    return true;
}

void Bunny::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &TBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(Texture), &textures[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Normal), &normals[0], GL_STATIC_DRAW);

    std::vector<GLuint> indices;
    for (const auto& face : faces) {
        for (int i = 0; i < 3; ++i) {
            indices.push_back(face.vIndex[i]);
        }
    }
    numIndices = indices.size();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Vertex Positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Vertex Textures
    if (!textures.empty()) {
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)0);
    }

    // Vertex Normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Normal), (void*)0);

    glBindVertexArray(0);
}

void Bunny::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Bunny::update() {
    // Update bunny's position and velocity for jumping
    positionY += velocityY;
    velocityY += -0.01; // gravity

    if (positionY <= 0.0) {
        positionY = 0.0;
        velocityY = jumpVelocity;
    }
}

void Bunny::updatePosition() {
    // Update position based on velocity
    positionY += velocityY;
    // Apply gravity
    velocityY += gravity;

    // Reset position and velocity when touching the ground
    if (positionY <= 0.0f) {
        positionY = 0.0f;
        velocityY = jumpVelocity;
    }
}

void Bunny::display() {
    // Use the shader program
    glUseProgram(shaderProgram);

    // Calculate model matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

    // Set the model matrix uniform
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Bind VAO and render
    glBindVertexArray(VAO);
    // Assuming you know the number of vertices/indices
    glDrawElements(GL_TRIANGLES, faces.size()*3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Bunny::setShaderProgram(GLuint program) {
    shaderProgram = program;
}

void Bunny::setModelMatrixLocation(GLint loc) {
    modelMatrixLoc = loc;
}

void Bunny::initVBO() {
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
    glGenBuffers(1, &gVertexAttribBuffer); // Buffer for vertex attributes
    glGenBuffers(1, &gIndexBuffer); // Buffer for indices
    assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0); // Check buffers were successfully created

    // Bind the vertex attribute buffer and the index buffer
    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    // Calculate the size in bytes of the vertex data and normals
    gVertexDataSizeInBytes = vertices.size() * 3 * sizeof(GLfloat);
    gNormalDataSizeInBytes = normals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytes = faces.size() * 3 * sizeof(GLuint);

    // Allocate memory for vertex, normal, and index data
    GLfloat* vertexData = new GLfloat[vertices.size() * 3];
    GLfloat* normalData = new GLfloat[normals.size() * 3];
    GLuint* indexData = new GLuint[faces.size() * 3];

    // Variables to store the min and max values of the vertex positions
    float minX = 1e6, maxX = -1e6;
    float minY = 1e6, maxY = -1e6;
    float minZ = 1e6, maxZ = -1e6;

    // Copy vertex positions to the allocated array and find min/max values
    for (int i = 0; i < vertices.size(); ++i) {
        vertexData[3 * i] = vertices[i].x;
        vertexData[3 * i + 1] = vertices[i].y;
        vertexData[3 * i + 2] = vertices[i].z;

        // Updating min/max values for each coordinate
        minX = std::min(minX, vertices[i].x);
        maxX = std::max(maxX, vertices[i].x);
        minY = std::min(minY, vertices[i].y);
        maxY = std::max(maxY, vertices[i].y);
        minZ = std::min(minZ, vertices[i].z);
        maxZ = std::max(maxZ, vertices[i].z);
    }

    // Log min/max values for debugging
    std::cout << "minX = " << minX << std::endl;
    std::cout << "maxX = " << maxX << std::endl;
    std::cout << "minY = " << minY << std::endl;
    std::cout << "maxY = " << maxY << std::endl;
    std::cout << "minZ = " << minZ << std::endl;
    std::cout << "maxZ = " << maxZ << std::endl;

    // Copy normal data to the allocated array
    for (int i = 0; i < normals.size(); ++i) {
        normalData[3 * i] = normals[i].x;
        normalData[3 * i + 1] = normals[i].y;
        normalData[3 * i + 2] = normals[i].z;
    }

    // Copy index data for each face to the allocated array
    for (int i = 0; i < faces.size(); ++i) {
        indexData[3 * i] = faces[i].vIndex[0];
        indexData[3 * i + 1] = faces[i].vIndex[1];
        indexData[3 * i + 2] = faces[i].vIndex[2];
    }

    // Upload vertex and normal data to the GPU
    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);

    // Upload index data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

    // Free the CPU-side memory as it's no longer needed
    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;

    // Define the layout of the vertex data in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // For vertex positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes)); // For vertex normals
}
 */

