#pragma once
#include <System.h>
#include "Text.h"
#include "DisplayTextEvent.h"
#include "TextBlockModification.h"
#include "InFocusComponent.h"
#include "SystemLogger.h"
#include "GameStateComponent.h"

/**
 * KeyboardInputSystem is responsible for taking keyboard input values (as int)
 * from the game app and determining the downstream actions that are effected.
 *
 * The KeyboardInputSystem is not responsible for execution of the actions,
 * only telling the subsequent systems via component changes.
 *
 * The associated entities here are all keyboard based text entities, which means
 * entities with text buffers. Some may be rendered, others just contain the data.
 */

enum KeyboardActionType
{
    PRESS
};

struct KeyboardAction
{
public:
    GameMode mode;

    KeyboardActionType keyboardActionType;

    int character;
    bool shiftPressed, ctrlPressed, altPressed;

    KeyboardAction() = default;
    KeyboardAction(GameMode mode, KeyboardActionType keyboardActionType, int character, bool shiftPressed, bool ctrlPressed, bool altPressed) : mode(mode), keyboardActionType(keyboardActionType), character(character), shiftPressed(shiftPressed), ctrlPressed(ctrlPressed), altPressed(ctrlPressed){};
};

class KeyboardInputSystem : public System
{
public:
    KeyboardInputSystem(EntityManager &entityManager, ComponentManager &componentManager, SystemLogger *logger);
    void KeyPress(int character, bool shiftPressed, bool ctrlPressed, bool altPressed);
    void Update(float deltaTime) override;

private:
    ConcurrentQueue<KeyboardAction> keyboardActionQueue;

    void inputChar(GameMode modeAtInput, TextBlockModificationType entryType, int character, bool shiftPressed, bool ctrlPressed, bool altPressed);

    EntityManager &entityManager;
    ComponentManager &componentManager;
};

KeyboardInputSystem::KeyboardInputSystem(EntityManager &entityManager, ComponentManager &componentManager, SystemLogger *logger) : System(logger), entityManager(entityManager), componentManager(componentManager) {}

void KeyboardInputSystem::KeyPress(int character, bool shiftPressed, bool ctrlPressed, bool altPressed)
{
    auto mode = componentManager.GetComponent<GameStateComponent>(componentManager.GetEntityWithComponent<GameStateComponent>()).gameMode;
    keyboardActionQueue.Push(KeyboardAction(mode, PRESS, character, shiftPressed, ctrlPressed, altPressed));
}

void KeyboardInputSystem::Update(float deltaTime)
{
    auto mode = componentManager.GetComponent<GameStateComponent>(componentManager.GetEntityWithComponent<GameStateComponent>()).gameMode;

    KeyboardAction keyboardAction;
    while (keyboardActionQueue.TryPop(keyboardAction))
    {
        if (keyboardAction.keyboardActionType == PRESS)
        {
            std::string displayAs;
            switch (keyboardAction.character)
            {
            case GLFW_KEY_BACKSPACE:
                inputChar(mode, DELETE, INT_MIN, keyboardAction.shiftPressed, keyboardAction.ctrlPressed, keyboardAction.altPressed);
                displayAs = "DELETE";
                break;
            default:
                inputChar(mode, CHARACTER, keyboardAction.character, keyboardAction.shiftPressed, keyboardAction.ctrlPressed, keyboardAction.altPressed);

                // Map the key to a character, considering shift modifier for uppercase
                displayAs = keyboardAction.shiftPressed ? getShiftedChar(keyboardAction.character) : getChar(keyboardAction.character);
                if (displayAs == "\n")
                {
                    displayAs = "ENTER";
                }
                if (displayAs == " ")
                {
                    displayAs = "SPACEBAR";
                }
                break;
            }

            // Log the keypress event
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2)
                << "pressed (" << displayAs << ")"
                << (keyboardAction.shiftPressed ? " with Shift" : "")
                << (keyboardAction.ctrlPressed ? " with Ctrl" : "")
                << (keyboardAction.altPressed ? " with Alt" : "");

            std::string formattedString = oss.str();
            // tell text overlay system to update the input_logger block via queue(?)
        }
    }
}

void KeyboardInputSystem::inputChar(GameMode modeAtInput, TextBlockModificationType entryType, int character, bool shiftPressed, bool ctrlPressed, bool altPressed)
{
    auto &mode = componentManager.GetComponent<GameStateComponent>(componentManager.GetEntityWithComponent<GameStateComponent>());

    std::string c;
    if (entryType == CHARACTER)
        c = getChar(character);

    // quick switch mode
    if (shiftPressed && c == " ")
    {
        switch (mode.gameMode)
        {
        case FREE_TYPE:
            mode.gameMode = ENTITY_MANAGEMENT;
            break;
        case ENTITY_MANAGEMENT:
            mode.gameMode = ROAM;
            break;
        case ROAM:
            mode.gameMode = FREE_TYPE;
            break;
        }

        std::string formattedString = "switched to " + modeToString(mode.gameMode) + " mode";
        Log(formattedString, "log_input_logger");
        return;
    }
    else
        // determine if it's a meta command
        if (ctrlPressed && shiftPressed)
        {
            if (c == "e")
            {
                mode = ENTITY_MANAGEMENT;
                // tell text overlay system to display we're in meta mode, entity management
                return;
            }
            else if (c == "t")
            {
                mode = ROAM;
                // tell text overlay system we're in travel mode
                return;
            }
            else if (c == "f")
            {
                mode = FREE_TYPE;
                return;
            }
        }

    if (mode.gameMode == FREE_TYPE && modeAtInput == FREE_TYPE)
    {
        // enter means lock text block from appends
        // TODO: should probably handle this at the top of method
        if (c == "\n")
        {
            entryType = ENTER;
            auto entities = componentManager.GetEntitiesWithComponents<InFocusComponent, TextBlockComponent>();
            for (auto entity : entities)
            {
                componentManager.RemoveComponent<InFocusComponent>(entity);
            }
            return;
        }

        std::string realC = shiftPressed ? getShiftedChar(character) : getChar(character);

        auto entities = componentManager.GetEntitiesWithComponents<InFocusComponent, TextBlockComponent>();
        if (entities.size() == 0)
        {
            // create a new free type text block
            Entity freeTypeEntity = entityManager.CreateEntity();
            TextBlockComponent component = TextBlockComponent("free_type");
            componentManager.AddComponent<TextBlockComponent>(freeTypeEntity, component);
            componentManager.AddComponent(freeTypeEntity, InFocusComponent());

            // TODO: clean up the publish creation logic
            entityManager.PublishEntityCreation(freeTypeEntity);
            entities.insert(freeTypeEntity);
        }

        for (Entity entity : entities)
        {
            if (componentManager.HasComponent<TextBlockComponent>(entity))
            {
                TextBlockComponent &textBlockComponent = componentManager.GetComponent<TextBlockComponent>(entity);
                textBlockComponent.queuedModifications.push(TextBlockModification(entryType, realC));
            }
        }
    }
    else if (mode.gameMode == ENTITY_MANAGEMENT)
    {
        // shift + n = new non-text visible entity, EntityManagementSubMode.NEW_ENTITY
        // if ENTER in EntityManagementSubMode.NEW_ENTITY mode, create copy of last (base) entity in center
    }
}
