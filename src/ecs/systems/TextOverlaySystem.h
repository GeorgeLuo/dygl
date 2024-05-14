#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <fstream>

GLuint textureAtlasID;     // OpenGL texture ID
stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

void initFontSystem(const char *fontPath)
{
    // Load the font using stb_truetype
    unsigned char *ttf_buffer = new unsigned char[1 << 20]; // allocate enough memory for the font file
    // Increase the resolution: Double the dimensions for the temporary bitmap
    unsigned char *temp_bitmap = new unsigned char[1024 * 1024]; // temporary bitmap for alpha values, now 1024x1024
    // Load font file into memory
    std::ifstream fontFile(fontPath, std::ios::binary);
    fontFile.read((char *)ttf_buffer, 1 << 20);
    fontFile.close();
    // Increase the baked font size for higher resolution
    // Bake the font glyphs into a bitmap using stb_truetype
    stbtt_BakeFontBitmap(ttf_buffer, 0, 64.0f, temp_bitmap, 1024, 1024, 32, 96, cdata); // Adjusted for 1024x1024 bitmap, font size doubled
    // Convert the baked bitmap to RGBA format with an adjusted size
    std::vector<unsigned char> rgba_bitmap(1024 * 1024 * 4); // Adjusted for RGBA, now 1024x1024
    for (int i = 0; i < (1024 * 1024); ++i)
    {
        rgba_bitmap[i * 4 + 0] = 255;            // R
        rgba_bitmap[i * 4 + 1] = 255;            // G
        rgba_bitmap[i * 4 + 2] = 255;            // B
        rgba_bitmap[i * 4 + 3] = temp_bitmap[i]; // A from the alpha bitmap
    }
    // Upload the RGBA bitmap to OpenGL as a texture
    glGenTextures(1, &textureAtlasID);
    glBindTexture(GL_TEXTURE_2D, textureAtlasID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba_bitmap.data());
    // Set texture parameters for filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Clean up
    delete[] ttf_buffer;
    delete[] temp_bitmap;
}

class TextBlock
{
public:
private:
};

class TextOverlaySystem : public System
{
public:
    TextOverlaySystem(EntityManager &entityManager, ComponentManager &componentManager)
        : entityManager(entityManager), componentManager(componentManager)
    {
    }
    void Update(float deltaTime) override;
    void InsertText(float x, float y, float z, const std::string &text);
    void Initialize();

private:
    EntityManager &entityManager;
    ComponentManager &componentManager;
};

void TextOverlaySystem::Initialize()
{
    // initFontSystem("fonts/Nanum-Gothic-Coding/NanumGothicCoding-Regular.ttf");
    initFontSystem("fonts/arial/Arial.ttf");
}

void TextOverlaySystem::Update(float deltaTime)
{
}

void TextOverlaySystem::InsertText(float x, float y, float z, const std::string &text)
{
    Entity textEntity = entityManager.CreateEntity();

    componentManager.AddComponent(textEntity, ShaderComponent("shaders/vertex/textOverlay.vert", "shaders/fragment/textOverlay.frag"));

    std::vector<Vertex> vertices;
    float cursorX = x;
    for (unsigned char c : text)
    {
        if (c < 32 || c >= 128)
            continue; // basic ASCII handling
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(cdata, 1024, 1024, c - 32, &cursorX, &y, &q, 1); // 1=opengl & d3d10+,0=d3d9
        vertices.push_back(Vertex(q.x0, q.y0, z, q.s0, q.t0));
        vertices.push_back(Vertex(q.x1, q.y0, z, q.s1, q.t0));
        vertices.push_back(Vertex(q.x1, q.y1, z, q.s1, q.t1));
        vertices.push_back(Vertex(q.x0, q.y0, z, q.s0, q.t0));
        vertices.push_back(Vertex(q.x1, q.y1, z, q.s1, q.t1));
        vertices.push_back(Vertex(q.x0, q.y1, z, q.s0, q.t1));
    }

    // Setup OpenGL buffers
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW); // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Attach the components
    componentManager.AddComponent(textEntity, GeometryComponent(vertices));
    componentManager.AddComponent(textEntity, TransformComponent(x, y, z, 0.01f, 0.01f, 0.01f));
    componentManager.AddComponent(textEntity, RenderComponent(VAO, VBO, vertices.size()));
    componentManager.AddComponent(textEntity, TextureComponent(textureAtlasID));
    entityManager.PublishEntityCreation(textEntity);
}
