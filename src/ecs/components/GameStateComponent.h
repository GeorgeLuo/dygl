#pragma once
#include <string>

enum GameMode
{
    FREE_TYPE,
    ENTITY_MANAGEMENT,
    ROAM
};

enum Meta
{
    SHIFT,
    CTRL,
    ALT
};

std::string modeToString(GameMode mode)
{
    switch (mode)
    {
    case FREE_TYPE:
        return "FREE_TYPE";
    case ENTITY_MANAGEMENT:
        return "ENTITY_MANAGEMENT";
    case ROAM:
        return "ROAM";
    default:
        return "UNKNOWN";
    }
}

struct GameStateComponent
{
    GameMode gameMode;

    GameStateComponent(GameMode gameMode) : gameMode(gameMode) {}
    GameStateComponent() = default;
};