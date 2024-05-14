// struct RenderComponent
// {
//     unsigned int VAO;
//     unsigned int VBO;
//     // Add constructor and destructor for managing OpenGL resources
// };

struct RenderComponent
{
    unsigned int VAO;
    unsigned int VBO;
    GLsizei vertexCount; // Stores the number of vertices to be drawn

    RenderComponent() = default;
    
    // Constructor
    RenderComponent(unsigned int vao, unsigned int vbo, GLsizei vertexCount)
        : VAO(vao), VBO(vbo), vertexCount(vertexCount) {}

    // Destructor to clean up resources, if necessary
    // ~RenderComponent()
    // {
    //     glDeleteVertexArrays(1, &VAO);
    //     glDeleteBuffers(1, &VBO);
    // }
};
