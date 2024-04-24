#pragma once

struct EntityDeletionMessage {
    int id;

    EntityDeletionMessage(int id) : id(id) {}
};