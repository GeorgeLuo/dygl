#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <fstream>
#include "DisplayTextEvent.h"
#include "TextBlockComponent.h"

GLuint textureAtlasID;     // OpenGL texture ID
stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

// Return char from keycode
const std::string getChar(int key)
{
    switch (key)
    {
    case GLFW_KEY_A:
        return "a";
    case GLFW_KEY_B:
        return "b";
    case GLFW_KEY_C:
        return "c";
    case GLFW_KEY_D:
        return "d";
    case GLFW_KEY_E:
        return "e";
    case GLFW_KEY_F:
        return "f";
    case GLFW_KEY_G:
        return "g";
    case GLFW_KEY_H:
        return "h";
    case GLFW_KEY_I:
        return "i";
    case GLFW_KEY_J:
        return "j";
    case GLFW_KEY_K:
        return "k";
    case GLFW_KEY_L:
        return "l";
    case GLFW_KEY_M:
        return "m";
    case GLFW_KEY_N:
        return "n";
    case GLFW_KEY_O:
        return "o";
    case GLFW_KEY_P:
        return "p";
    case GLFW_KEY_Q:
        return "q";
    case GLFW_KEY_R:
        return "r";
    case GLFW_KEY_S:
        return "s";
    case GLFW_KEY_T:
        return "t";
    case GLFW_KEY_U:
        return "u";
    case GLFW_KEY_V:
        return "v";
    case GLFW_KEY_W:
        return "w";
    case GLFW_KEY_X:
        return "x";
    case GLFW_KEY_Y:
        return "y";
    case GLFW_KEY_Z:
        return "z";
    case GLFW_KEY_0:
        return "0";
    case GLFW_KEY_1:
        return "1";
    case GLFW_KEY_2:
        return "2";
    case GLFW_KEY_3:
        return "3";
    case GLFW_KEY_4:
        return "4";
    case GLFW_KEY_5:
        return "5";
    case GLFW_KEY_6:
        return "6";
    case GLFW_KEY_7:
        return "7";
    case GLFW_KEY_8:
        return "8";
    case GLFW_KEY_9:
        return "9";
    case GLFW_KEY_SPACE:
        return " ";
    case GLFW_KEY_MINUS:
        return "-";
    case GLFW_KEY_EQUAL:
        return "=";
    case GLFW_KEY_LEFT_BRACKET:
        return "[";
    case GLFW_KEY_RIGHT_BRACKET:
        return "]";
    case GLFW_KEY_BACKSLASH:
        return "\\";
    case GLFW_KEY_SEMICOLON:
        return ";";
    case GLFW_KEY_APOSTROPHE:
        return "'";
    case GLFW_KEY_GRAVE_ACCENT:
        return "`";
    case GLFW_KEY_COMMA:
        return ",";
    case GLFW_KEY_PERIOD:
        return ".";
    case GLFW_KEY_SLASH:
        return "/";
    case GLFW_KEY_TAB:
        return "\t";
    case GLFW_KEY_ENTER:
        return "\n";
    default:
        return "";
    }
}

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

class TextChannel
{
public:
private:
};

class TextOverlaySystem : public System
{
public:
    std::string inputBufferText;
    TextOverlaySystem(EntityManager &entityManager, ComponentManager &componentManager)
        : entityManager(entityManager), componentManager(componentManager)
    {
    }
    void Update(float deltaTime) override;

    void ClearBlock(const std::string blockname);

    void AppendText(const std::string blockname, const std::string &text);
    void AddTextBlock(const std::string blockname, float x, float y, float z);
    void PublishBlock(Entity entity);

    void InputChar(int character);
    void AddListener(EventBus &bus);
    void Initialize();

private:
    EntityManager &entityManager;
    ComponentManager &componentManager;
};

void TextOverlaySystem::Initialize()
{
    initFontSystem("fonts/Nanum-Gothic-Coding/NanumGothicCoding-Regular.ttf");
    // initFontSystem("fonts/arial/Arial.ttf");
}

void TextOverlaySystem::Update(float deltaTime)
{
    for (auto entity : this->entities)
    {
        // reset the vertices for the entity's textblock component
    }
}

void TextOverlaySystem::AddTextBlock(const std::string blockname, float x, float y, float z)
{
    Entity textEntity = entityManager.CreateEntity();
    System::AddEntity(textEntity);

    TextBlockComponent component = TextBlockComponent(blockname);

    componentManager.AddComponent<TextBlockComponent>(textEntity, component);

    componentManager.AddComponent(textEntity, ShaderComponent("shaders/vertex/textOverlay.vert", "shaders/fragment/textOverlay.frag"));
    componentManager.AddComponent(textEntity, GeometryComponent(std::vector<Vertex>()));
    componentManager.AddComponent(textEntity, TransformComponent(x, y, z, 0.005f, 0.005f, 0.005f));
    componentManager.AddComponent(textEntity, TextureComponent(textureAtlasID));

    entityManager.PublishEntityCreation(textEntity);
}

void TextOverlaySystem::AddListener(EventBus &bus)
{
}

void TextOverlaySystem::ClearBlock(const std::string blockname)
{
    // find the block by blockname
    for (auto entity : this->entities)
    {
        TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
        if (component.blockname == blockname)
        {
            component.textblock.clear();
        }
    }
}

void TextOverlaySystem::InputChar(int character)
{
    std::string strChar = getChar(character);
    if (strChar == "\n")
    {
        ClearBlock("free_type");
    }
    else
    {
        AppendText("free_type", strChar);
    }
}

void TextOverlaySystem::AppendText(const std::string blockname, const std::string &text)
{
    for (auto entity : this->entities)
    {
        TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
        if (component.blockname == blockname)
        {
            component.textblock += text;
            PublishBlock(entity);
        }
    }
}

void TextOverlaySystem::PublishBlock(Entity entity)
{
    TextBlockComponent &block = componentManager.GetComponent<TextBlockComponent>(entity);
    TransformComponent &transform = componentManager.GetComponent<TransformComponent>(entity);

    std::vector<Vertex> vertices;
    float cursorX = transform.position[0];
    float cursorY = transform.position[1];
    float z = transform.position[2];

    for (unsigned char c : block.textblock)
    {
        if (c < 32 || c >= 128)
            continue; // Skip non-ASCII characters

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(cdata, 1024, 1024, c - 32, &cursorX, &cursorY, &q, 1); // 1=opengl & d3d10+, 0=d3d9

        // Correctly set up vertices and texture coordinates with negated Y values
        vertices.push_back(Vertex(q.x0, -q.y0, z, q.s0, q.t0)); // Bottom-left
        vertices.push_back(Vertex(q.x1, -q.y0, z, q.s1, q.t0)); // Bottom-right
        vertices.push_back(Vertex(q.x1, -q.y1, z, q.s1, q.t1)); // Top-right
        vertices.push_back(Vertex(q.x0, -q.y0, z, q.s0, q.t0)); // Bottom-left
        vertices.push_back(Vertex(q.x1, -q.y1, z, q.s1, q.t1)); // Top-right
        vertices.push_back(Vertex(q.x0, -q.y1, z, q.s0, q.t1)); // Top-left
    }

    // Update the GeometryComponent with the new vertices
    GeometryComponent &geometry = componentManager.GetComponent<GeometryComponent>(entity);
    geometry.vertices = vertices;
    geometry.dirty = true;
}
