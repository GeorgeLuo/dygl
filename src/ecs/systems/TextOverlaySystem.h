#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include "DisplayTextEvent.h"
#include "TextBlockComponent.h"
#include "KeyboardEntryEvent.h"
#include "Text.h"

class TextOverlaySystem : public System
{
public:
    std::vector<Entity> focus;

    std::string inputBufferText;
    TextOverlaySystem(EntityManager &entityManager, ComponentManager &componentManager)
        : entityManager(entityManager), componentManager(componentManager)
    {
    }
    void Update(float deltaTime) override;

    void ClearBlock(const std::string blockname);
    void ClearBlock(Entity entity);

    Entity AddTextBlock(const std::string blockname, float x, float y, float z, float scaleX = 0.002f, float scaleY = 0.002f);
    Entity GetEntityByBlockname(const std::string blockname);

    Entity AppendText(const std::string blockname, const std::string &text);
    void AppendText(Entity entity, const std::string &text);
    Entity CreateOrReplaceText(const std::string blockname, const std::string text, float x, float y, float z);
    void DeleteFromText(Entity entity, int numChars = 1);

    void PublishBlock(Entity entity);

    void HandleDisplayTextEvent(const DisplayTextEvent &event);
    void HandleKeyboardEntryEvent(const KeyboardEntryEvent &event);

    void InputChar(int character, const std::string blockname);
    void AddListener(EventBus &bus);
    void Initialize(const std::string fontfile);

private:
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
    for (auto entity : this->entities)
    {
        // reset the vertices for the entity's textblock component
    }
}

Entity TextOverlaySystem::AddTextBlock(const std::string blockname, float x, float y, float z, float scaleX, float scaleY)
{
    Entity textEntity = entityManager.CreateEntity();
    System::AddEntity(textEntity);

    TextBlockComponent component = TextBlockComponent(blockname);

    componentManager.AddComponent<TextBlockComponent>(textEntity, component);

    componentManager.AddComponent(textEntity, ShaderComponent("shaders/vertex/textOverlay.vert", "shaders/fragment/textOverlay.frag"));
    componentManager.AddComponent(textEntity, GeometryComponent(std::vector<Vertex>()));
    componentManager.AddComponent(textEntity, TransformComponent(x, y, z, scaleX, scaleY));
    componentManager.AddComponent(textEntity, TextureComponent(textureAtlasID));

    entityManager.PublishEntityCreation(textEntity);
    return textEntity;
}

void TextOverlaySystem::AddListener(EventBus &bus)
{
    bus.subscribe<DisplayTextEvent>([this](const DisplayTextEvent &event)
                                    { this->HandleDisplayTextEvent(event); });

    bus.subscribe<KeyboardEntryEvent>([this](const KeyboardEntryEvent &event)
                                      { this->HandleKeyboardEntryEvent(event); });
}

void TextOverlaySystem::HandleKeyboardEntryEvent(const KeyboardEntryEvent &event)
{
    if (event.entryType == ENTER)
    {
        focus.clear();
        return;
    }

    // check if any entity is in focus
    if (focus.size() == 0)
    {
        // if not assume we are free typing
        // create a new free type text block and add to focus
        Entity entity = AddTextBlock("free_type", 0.0f, 0.0f, 0.0f);
        focus.push_back(entity);
    }

    // append text to entities in focus
    for (Entity entity : focus)
    {
        if (event.entryType == CHARACTER)
        {
            AppendText(entity, event.text);
        }
        else if (event.entryType == DELETE)
        {
            DeleteFromText(entity);
        }
    }
}

void TextOverlaySystem::HandleDisplayTextEvent(const DisplayTextEvent &event)
{
    if (focus.size() == 0)
    {
        Entity entity;
        if (event.replace)
        {
            entity = CreateOrReplaceText(event.blockname, event.text, event.x, event.y, event.z);
        }
        else
        {
            entity = GetEntityByBlockname(event.blockname);
            if (entity == INVALID_ENTITY)
            {
                entity = CreateOrReplaceText(event.blockname, event.text, event.x, event.y, event.z);
            }
            else
            {
                entity = AppendText(event.blockname, event.text);
            }
        }
        if (event.focus)
        {
            focus.push_back(entity);
        }
    }
    else
    {
        if (event.focus)
        {
            for (Entity entity : focus)
            {
                AppendText(entity, event.text);
            }
        }
        else
        {
            CreateOrReplaceText(event.blockname, event.text, event.x, event.y, event.z);
        }
    }
}

Entity TextOverlaySystem::CreateOrReplaceText(const std::string blockname, const std::string text, float x, float y, float z)
{
    Entity entity = GetEntityByBlockname(blockname);
    if (entity == INVALID_ENTITY)
    {
        entity = AddTextBlock(blockname, x, y, z);
    }
    else
    {
        ClearBlock(blockname);
    }
    TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
    component.textblock = text;
    PublishBlock(entity);

    return entity;
}

void TextOverlaySystem::ClearBlock(const std::string blockname)
{
    Entity entity = GetEntityByBlockname(blockname);
    ClearBlock(entity);
}

void TextOverlaySystem::ClearBlock(Entity entity)
{
    TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
    component.textblock.clear();
}

// TODO: use a map here
Entity TextOverlaySystem::GetEntityByBlockname(const std::string blockname)
{
    // find the block by blockname
    for (auto entity : this->entities)
    {
        TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
        if (component.blockname == blockname)
        {
            return entity;
        }
    }
    return INVALID_ENTITY;
}

void TextOverlaySystem::InputChar(int character, const std::string blockname)
{
    std::string strChar = getChar(character);
    if (strChar == "\n")
    {
        ClearBlock(blockname);
    }
    else
    {
        AppendText(blockname, strChar);
    }
}

void TextOverlaySystem::DeleteFromText(Entity entity, int numChars)
{
    TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);

    if (numChars > 0 && numChars <= component.textblock.size())
    {
        component.textblock.erase(component.textblock.size() - numChars, numChars);
    }
    else if (numChars > component.textblock.size())
    {
        component.textblock.clear();
    }

    PublishBlock(entity);
}

Entity TextOverlaySystem::AppendText(const std::string blockname, const std::string &text)
{
    for (auto entity : this->entities)
    {
        TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
        if (component.blockname == blockname)
        {
            component.textblock += text;
            PublishBlock(entity);
            return entity;
        }
    }
    return INVALID_ENTITY;
}

void TextOverlaySystem::AppendText(Entity entity, const std::string &text)
{
    TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
    {
        component.textblock += text;
        PublishBlock(entity);
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
