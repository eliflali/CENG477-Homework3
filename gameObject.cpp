class DrawableObject {
public:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> verticesOfObj;
    std::vector<GLuint> indicesOfObj;
    DrawableObject(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
        verticesOfObj = vertices;
        indicesOfObj = indices;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0); // Unbind VAO
    }

    ~DrawableObject() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void draw() {
        glBindVertexArray(VAO);
        // Assuming indices are used for drawing
        //gFaces.size() * 3
        glDrawElements(GL_TRIANGLES, indicesOfObj.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Other methods as needed
};

class Bunny : DrawableObject
{
public Bunny(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices){

}
    vector<Vertex> gVertices;
    vector<Texture> gTextures;
    vector<Normal> gNormals;
    vector<Face> gFaces;

    double positionX = 0;
    double positionY = 0;
    double positionZ = 0;

    double velocityX = 0;
    double velocityY = 0;
    double velocityZ = 0;

    const double jumpVelocity = 0.2;

};

Bunny bunny;

class Ground : DrawableObject
{
    public Ground(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices){

    }
    // Define the vertices for the ground quad
    GLfloat groundVertices[] = {
            // Positions            // Texture Coords (Optional)
            -10.0f, -0.1f,  10.0f,  0.0f, 1.0f, // Bottom Left
            10.0f, -0.1f,  10.0f,  1.0f, 1.0f, // Bottom Right
            10.0f, -0.1f, -10.0f,  1.0f, 0.0f, // Top Right
            -10.0f, -0.1f, -10.0f,  0.0f, 0.0f  // Top Left
    };

    // Define the indices for the quad (two triangles)
    GLuint groundIndices[] = {
            0, 1, 2, // First Triangle
            0, 2, 3  // Second Triangle
    };
};
