#pragma once
#include <string>
#include <fstream>
#include <glad.h> // Include GLAD
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <glfw3.h>
#include <vector>
#include <unordered_map>

GLuint textureAtlasID;     // OpenGL texture ID
stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

const std::unordered_map<int, std::string> shiftedKeys = {
    {GLFW_KEY_1, "!"},
    {GLFW_KEY_2, "@"},
    {GLFW_KEY_3, "#"},
    {GLFW_KEY_4, "$"},
    {GLFW_KEY_5, "%"},
    {GLFW_KEY_6, "^"},
    {GLFW_KEY_7, "&"},
    {GLFW_KEY_8, "*"},
    {GLFW_KEY_9, "("},
    {GLFW_KEY_0, ")"},
    {GLFW_KEY_MINUS, "_"},
    {GLFW_KEY_EQUAL, "+"},
    {GLFW_KEY_LEFT_BRACKET, "{"},
    {GLFW_KEY_RIGHT_BRACKET, "}"},
    {GLFW_KEY_BACKSLASH, "|"},
    {GLFW_KEY_SEMICOLON, ":"},
    {GLFW_KEY_APOSTROPHE, "\""},
    {GLFW_KEY_GRAVE_ACCENT, "~"},
    {GLFW_KEY_COMMA, "<"},
    {GLFW_KEY_PERIOD, ">"},
    {GLFW_KEY_SLASH, "?"}};

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

std::string getShiftedChar(int key)
{
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
    {
        // Convert letters to uppercase
        return std::string(1, static_cast<char>(key));
    }

    auto it = shiftedKeys.find(key);
    if (it != shiftedKeys.end())
    {
        return it->second;
    }

    return getChar(key);
}

// TODO: move this somewhere else, maybe params of initialize method

unsigned char *fontBuffer = nullptr;
float lineHeight = 0.0f;
float scale;

void initFontSystem(const char *fontPath)
{
    // Load the font using stb_truetype
    fontBuffer = new unsigned char[1 << 20];                     // Allocate enough memory for the font file
    unsigned char *temp_bitmap = new unsigned char[1024 * 1024]; // Temporary bitmap for alpha values

    // Load font file into memory
    std::ifstream fontFile(fontPath, std::ios::binary);
    fontFile.read((char *)fontBuffer, 1 << 20);
    fontFile.close();

    float pixelHeight = 128.0f;

    // Bake the font glyphs into a bitmap
    // stbtt_BakeFontBitmap(fontBuffer, 0, pixelHeight, temp_bitmap, 1024, 1024, 32, 96, cdata);
    if (stbtt_BakeFontBitmap(fontBuffer, 0, pixelHeight, temp_bitmap, 1024, 1024, 32, 96, cdata) <= 0)
    {
        std::cerr << "Failed to bake font bitmap." << std::endl;
        return;
    }

    // Convert the baked bitmap to RGBA format
    std::vector<unsigned char> rgba_bitmap(1024 * 1024 * 4);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Clean up
    delete[] temp_bitmap;

    stbtt_fontinfo fontInfo;
    stbtt_InitFont(&fontInfo, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0));

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

    // Scale ascent, descent, and lineGap based on font size
    float fontSize = 6.0f; // Adjust based on desired font size
    scale = stbtt_ScaleForPixelHeight(&fontInfo, pixelHeight);

    lineHeight = (ascent - descent + lineGap) * scale;

    // std::cout << "pixelHeight: " << pixelHeight << " fontSize: " << fontSize << " scale: " << scale << " lineHeight: " << lineHeight << std::endl;

    // TODO: maybe don't do this
    delete[] fontBuffer;
}
