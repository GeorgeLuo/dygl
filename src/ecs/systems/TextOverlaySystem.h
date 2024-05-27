#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "DisplayTextEvent.h"
#include "TextBlockComponent.h"
#include "TextBlockModification.h"
#include "Text.h"
#include "BoundingBoxComponent.h"

/**
 * TextOverlaySystem is responsible for finalizing text based entities for
 * pre-rendering. Systems that precede this may include the keyboard input
 * or message system, and the text overlay system rectifies all the changes
 * from these interfaces.
 *
 * The owned entities of this system will be all text based entities.
 */

// TODO: add expiration feature

// TODO: use commands when writing through entity and component managers

// TODO: make the text hitbox an entire rectangle

class TextOverlaySystem : public System
{
public:
    TextOverlaySystem(EntityManager &entityManager, ComponentManager &componentManager)
        : entityManager(entityManager), componentManager(componentManager)
    {
    }
    void Update(float deltaTime) override;
    void Initialize(const std::string fontfile);

private:
    void publishBlock(Entity entity);
    EntityManager &entityManager;
    ComponentManager &componentManager;
};

void TextOverlaySystem::Initialize(const std::string fontfile)
{
    initFontSystem(fontfile.c_str());
    // initFontSystem("fonts/arial/Arial.ttf");
}

void TextOverlaySystem::Update(float deltaTime)
{
    auto entities = componentManager.GetEntitiesWithComponents<TextBlockComponent>();
    for (Entity entity : entities)
    {
        auto &textBlockComponent = componentManager.GetComponent<TextBlockComponent>(entity);

        bool publish;
        if (textBlockComponent.queuedModifications.size())
        {
            publish = true;
            // handle the modified text blocks
            TextBlockModification modification;
            while (!textBlockComponent.queuedModifications.empty())
            {
                modification = textBlockComponent.queuedModifications.front();
                textBlockComponent.queuedModifications.pop();

                if (modification.entryType == CHARACTER)
                {
                    textBlockComponent.textblock += modification.text;
                }
                else if (modification.entryType == REPLACE)
                {
                    textBlockComponent.textblock = modification.text;
                }
                else if (modification.entryType == DELETE)
                {
                    if (textBlockComponent.textblock.size() >= 1)
                        textBlockComponent.textblock.erase(textBlockComponent.textblock.size() - 1, 1);
                }
            }
        }
        if (!componentManager.HasComponent<ShaderComponent>(entity))
        {
            componentManager.AddComponent(entity, ShaderComponent("shaders/vertex/textOverlay.vert", "shaders/fragment/textOverlay.frag"));
            publish = true;
        }
        if (!componentManager.HasComponent<GeometryComponent>(entity))
        {
            componentManager.AddComponent(entity, GeometryComponent(std::vector<Vertex>()));
            publish = true;
        }
        if (!componentManager.HasComponent<TextureComponent>(entity))
        {
            componentManager.AddComponent(entity, TextureComponent(textureAtlasID));
            publish = true;
        }
        if (!componentManager.HasComponent<TransformComponent>(entity))
        {
            // componentManager.AddComponent(entity, TransformComponent(0.0f, 0.0f, 0.0f, 0.004f, 0.004f));
            componentManager.AddComponent(entity, TransformComponent(0.0f, 0.0f, 0.0f));
            // componentManager.AddComponent(entity, TransformComponent(0.0f, 0.0f, 0.0f, scale, scale));

            // componentManager.AddComponent(entity, TransformComponent(0.0f, 0.0f, 0.0f));
            publish = true;
        }
        if (publish)
        {
            publishBlock(entity);
        }
    }
}

void TextOverlaySystem::publishBlock(Entity entity)
{
    TextBlockComponent &block = componentManager.GetComponent<TextBlockComponent>(entity);
    TransformComponent &transform = componentManager.GetComponent<TransformComponent>(entity);

    std::vector<Vertex> vertices;
    float cursorX = transform.position[0];
    float cursorY = transform.position[1];
    float z = transform.position[2];

    float startX = cursorX;

    float maxWidth = 1600.0f; // Set the default maximum width for the text block
    if (componentManager.HasComponent<BoundingBoxComponent>(entity))
    {
        maxWidth = componentManager.GetComponent<BoundingBoxComponent>(entity).width;
    }

    // TODO : IMPORTANT Y height calculations are incorrect, max and min are not set

    // Initialize bounding box values
    float minX = cursorX;
    float minY = cursorY;
    float maxX = cursorX;
    float maxY = cursorY;

    // Calculate render scale
    float renderScale = scale / 120.0f;

    for (unsigned char c : block.textblock)
    {
        // Handle new line character
        if (c == '\n')
        {
            cursorX = startX;
            cursorY += lineHeight * renderScale; // Increment cursorY to move down to the next line
            continue;
        }

        if (c < 32 || c >= 128)
            continue; // Skip non-ASCII characters

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(cdata, 1024, 1024, c - 32, &cursorX, &cursorY, &q, 1); // 1=opengl & d3d10+, 0=d3d9

        float charWidth = (q.x1 - q.x0) * renderScale;

        // Check if the character exceeds the maximum width
        if (cursorX + charWidth > startX + maxWidth)
        {
            cursorX = startX;
            cursorY += lineHeight; // Increment cursorY to move down to the next line

            // Recalculate the quad after wrapping
            stbtt_GetBakedQuad(cdata, 1024, 1024, c - 32, &cursorX, &cursorY, &q, 1);
        }

        // Correctly set up vertices and texture coordinates
        vertices.push_back(Vertex(q.x0 * renderScale, -q.y0 * renderScale, z, q.s0, q.t0));
        vertices.push_back(Vertex(q.x1 * renderScale, -q.y0 * renderScale, z, q.s1, q.t0));
        vertices.push_back(Vertex(q.x1 * renderScale, -q.y1 * renderScale, z, q.s1, q.t1));
        vertices.push_back(Vertex(q.x0 * renderScale, -q.y0 * renderScale, z, q.s0, q.t0));
        vertices.push_back(Vertex(q.x1 * renderScale, -q.y1 * renderScale, z, q.s1, q.t1));
        vertices.push_back(Vertex(q.x0 * renderScale, -q.y1 * renderScale, z, q.s0, q.t1));

        // Update bounding box dimensions based on the actual character positions
        if (q.x1 * renderScale > maxX)
            maxX = q.x1 * renderScale;
        if (-q.y1 * renderScale > maxY)
            maxY = -q.y1 * renderScale;
        if (q.x0 * renderScale < minX)
            minX = q.x0 * renderScale;
        if (-q.y0 * renderScale < minY)
            minY = -q.y0 * renderScale;

        // Increment cursorX to the end of the current character
        cursorX += charWidth;
    }

    // Update the GeometryComponent with the new vertices
    GeometryComponent &geometry = componentManager.GetComponent<GeometryComponent>(entity);
    geometry.vertices = vertices;
    geometry.dirty = true;

    // Calculate final height of the bounding box based on rendered text
    float finalHeight = maxY - minY;

    // Create or update the BoundingBoxComponent based on the calculated bounds
    if (!componentManager.HasComponent<BoundingBoxComponent>(entity))
    {
        componentManager.AddComponent(entity, BoundingBoxComponent(minX, minY, maxWidth, finalHeight));
    }
    else
    {
        BoundingBoxComponent &bbox = componentManager.GetComponent<BoundingBoxComponent>(entity);
        bbox.x = minX;
        bbox.y = minY;
        bbox.width = maxWidth; // Keep the width from the BoundingBoxComponent
        bbox.height = finalHeight; // Update the height based on the rendered text
    }
}
