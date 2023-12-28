
#ifndef CENG477_HOMEWORK3_GROUND_H
#define CENG477_HOMEWORK3_GROUND_H
#ifndef GROUND_H
#define GROUND_H

#include <GL/glew.h>
#include <vector>

struct Vertex {
    GLfloat x, y, z;
};

class Ground {
public:
    Ground();
    ~Ground();

    void init();
    void draw();

private:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    void setupMesh();
};

#endif // GROUND_H

#endif //CENG477_HOMEWORK3_GROUND_H
