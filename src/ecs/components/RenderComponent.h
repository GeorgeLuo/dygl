// struct RenderComponent
// {
//     unsigned int VAO;
//     unsigned int VBO;
//     // Add constructor and destructor for managing OpenGL resources
// };

// struct RenderComponent
// {
//     unsigned int VAO;
//     unsigned int VBO;
//     GLsizei vertexCount; // Stores the number of vertices to be drawn

//     RenderComponent() = default;
    
//     // Constructor
//     RenderComponent(unsigned int vao, unsigned int vbo, GLsizei vertexCount)
//         : VAO(vao), VBO(vbo), vertexCount(vertexCount) {}

//     // Destructor to clean up resources, if necessary
//     // ~RenderComponent()
//     // {
//     //     glDeleteVertexArrays(1, &VAO);
//     //     glDeleteBuffers(1, &VBO);
//     // }
// };

struct RenderComponent
{
    unsigned int VAO;
    unsigned int VBO;
    GLsizei vertexCount; // Stores the number of vertices to be drawn
    GLsizeiptr bufferSize; // Stores the size of the buffer in bytes

    RenderComponent() = default;

    // Constructor
    RenderComponent(unsigned int vao, unsigned int vbo, GLsizei vertexCount, GLsizeiptr bufferSize)
        : VAO(vao), VBO(vbo), vertexCount(vertexCount), bufferSize(bufferSize) {}

    // Destructor to clean up resources, if necessary
    // ~RenderComponent()
    // {
    //     glDeleteVertexArrays(1, &VAO);
    //     glDeleteBuffers(1, &VBO);
    // }
};
