
#ifndef CENG477_HOMEWORK3_GAMEOBJECT_H
#define CENG477_HOMEWORK3_GAMEOBJECT_H

#include <vector>
#include <GL/glew.h>

// Vertex structure
struct Vertex {
    GLfloat x, y, z;
    // Add other vertex attributes like normals, texture coordinates, etc., if needed
};

class DrawableObject {
public:
    DrawableObject(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    virtual ~DrawableObject();

    void draw(int numberOfIndices);

protected:
    GLuint VAO, VBO, EBO;
};

class Bunny : public DrawableObject {
public:
    Bunny(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

    void update();

private:
    double positionX;
    double positionY;
    double positionZ;

    double velocityX;
    double velocityY;
    double velocityZ;

    const double jumpVelocity;
};

class Ground : public DrawableObject {
public:
    Ground();

    void update();

private:
    GLfloat groundVertices[20]; // 4 vertices * 5 attributes each
    GLuint groundIndices[6];    // 2 triangles * 3 indices each
};

#endif //CENG477_HOMEWORK3_GAMEOBJECT_H
