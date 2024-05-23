#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "DisplayTextEvent.h"
#include "TextBlockComponent.h"
#include "TextBlockModification.h"
#include "Text.h"

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
            componentManager.AddComponent(entity, TransformComponent(0.0f, 0.0f, 0.0f, 0.0018f, 0.0018f));
            publish = true;
        }
        if (publish)
        {
            publishBlock(entity);
        }
    }
}

// TODO: audit this, probably okay does not use eventbus
void TextOverlaySystem::publishBlock(Entity entity)
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
