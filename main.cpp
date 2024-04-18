#include "ConcurrentQueue.h"
#include "ECSApp.h"
#include "QueueCollection.h"
#include <iostream>
#include <thread>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <vector>
// Note: For Windows, include Winsock2.h and link against Ws2_32.lib
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>

struct EntityDefinition
{
    float x, y, z;
    std::string type;

    EntityDefinition(float x, float y, float z, std::string type)
        : x(x), y(y), z(z), type(type) {}
};

void parseCommand(const std::string &command, QueueCollection &queues)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "CREATE")
    {
        // Parse parameters
        std::string type;
        float x, y, z;
        int id;
        iss >> id >> type >> x >> y >> z;
        // Add to entity creation queue in your system
        std::vector<EntityCreationMessage> creationMessages{EntityCreationMessage(id, type, x, y, z)};
        queues.entityCreationQueue.Push(creationMessages);
    }
    else if (cmd == "DELETE")
    {
        // Parse parameters
        int id;
        iss >> id;
        // Add to entity deletion queue in your system
        std::vector<EntityDeletionMessage> deletionMessages{EntityDeletionMessage(id)};
        queues.entityDeletionQueue.Push(deletionMessages);
    }
    else if (cmd == "COLOR")
    {
        // Parse parameters
        float r, g, b;
        iss >> r >> g >> b;
        // Add to color change queue or set color directly
        queues.colorQueue.Push(std::make_tuple(r, g, b));
    }
}

void ServerThread(QueueCollection &queues)
{
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started. Listening for connections..." << std::endl;

    // Outer loop added to continuously accept new connections
    while (true)
    {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            continue; // Don't exit the program, just wait for the next connection
        }

        // Process the connection
        char buffer[1024] = {0};
        while (true)
        {
            int bytes_read = read(new_socket, buffer, 1024);
            if (bytes_read <= 0)
            {
                std::cout << "Client disconnected or error." << std::endl;
                break; // Break from processing loop, not from accept loop
            }
            std::cout << "Message Received: " << buffer << std::endl;
            parseCommand(buffer, queues);
            send(new_socket, "ACK\n", 4, 0); // Send back an acknowledgement
        }
        close(new_socket); // Close the current connection before accepting a new one
    }
}

std::string createEntityCommand(int id, const std::string &type, float x, float y, float z)
{
    return "CREATE " + std::to_string(id) + " " + type + " " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
}

std::string deleteEntityCommand(int id)
{
    return "DELETE " + std::to_string(id) + "\n";
}

std::string colorChangeCommand(float r, float g, float b)
{
    return "COLOR " + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + "\n";
}

// Example parsing logic for one command
void parseCommand(const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "CREATE")
    {
        std::string type;
        float x, y, z;
        iss >> type >> x >> y >> z;
        // Create entity logic
    }
    else if (cmd == "DELETE")
    {
        int id;
        iss >> id;
        // Delete entity logic
    }
    else if (cmd == "COLOR")
    {
        float r, g, b;
        iss >> r >> g >> b;
        // Color change logic
    }
}

// Function to wait for an acknowledgement
void waitForAck(int sock)
{
    char ackBuffer[10];
    recv(sock, ackBuffer, sizeof(ackBuffer), 0);
}

void ClientThread()
{
    srand(static_cast<unsigned int>(time(nullptr))); // Seed for random number generation

    // Predefined entity positions and types
    std::vector<EntityDefinition> entityDefinitions = {
        {-0.75f, 0.75f, 0.0f, "pyramid"},
        {0.75f, 0.75f, 0.0f, "square"},
        {0.75f, -0.75f, 0.0f, "triangle"},
        {-0.75f, -0.75f, 0.0f, "cube"}};
        // {-0.75f, 0.75f, 0.0f, "triangle"},
        // {0.75f, 0.75f, 0.0f, "square"},
        // {0.75f, -0.75f, 0.0f, "triangle"},
        // {-0.75f, -0.75f, 0.0f, "square"}};


    // Track active entities by id
    std::unordered_set<int> activeEntities;

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080); // Set to match server port
    while (true)
    {
        // Creating socket file descriptor each loop to ensure fresh start
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            std::cout << "\n Socket creation error \n";
            continue; // Try again on the next loop
        }
        // Convert IPv4 and IPv6 addresses from text to binary form
        // Assuming the server is running on the same machine (localhost)
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        {
            std::cout << "\nInvalid address/ Address not supported \n";
            close(sock);
            continue; // Try again on the next loop
        }
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            std::cout << "\nConnection Failed. Retrying...\n";
            close(sock);
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait a bit before retrying
            continue;                                             // Try again on the next loop
        }
        // Connection is successful

        // Color
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;
        std::string colorCommand = colorChangeCommand(r, g, b);
        send(sock, colorCommand.c_str(), colorCommand.length(), 0);
        waitForAck(sock);

        // Decide randomly on actions: add or remove entities
        int action = rand() % 2; // 0 for remove, 1 for add

        // Containers for messages
        std::vector<EntityCreationMessage> creations;
        std::vector<EntityDeletionMessage> deletions;

        // Random action: Add or Remove entities
        if (action == 1)
        {
            // Attempt to add a random entity if not already active
            int entityId = rand() % entityDefinitions.size();
            if (activeEntities.find(entityId) == activeEntities.end())
            {
                const auto &def = entityDefinitions[entityId];
                creations.push_back(EntityCreationMessage(entityId, def.type, def.x, def.y, def.z));
                activeEntities.insert(entityId);

                std::string entityCommand = createEntityCommand(entityId, def.type, def.x, def.y, def.z);
                send(sock, entityCommand.c_str(), entityCommand.length(), 0);
                waitForAck(sock);
            }
        }
        else
        {
            // Attempt to remove a random entity if there exists any
            if (!activeEntities.empty())
            {
                int removeIndex = rand() % activeEntities.size();
                auto it = activeEntities.begin();
                std::advance(it, removeIndex);
                int entityIdToRemove = *it; // Obtain the actual entityId to remove
                deletions.push_back(EntityDeletionMessage(entityIdToRemove));
                activeEntities.erase(it);

                std::string entityCommand = deleteEntityCommand(entityIdToRemove); // Now correctly referencing the ID
                send(sock, entityCommand.c_str(), entityCommand.length(), 0);
                waitForAck(sock);
            }
        }
        close(sock);                                                  // Close the socket after sending message
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Throttle message sending
    }
}

int main()
{
    QueueCollection queues;
    std::thread serverThread(ServerThread, std::ref(queues));
    std::thread clientThread(ClientThread);
    OpenGLApp openglApp(queues); // Adjust constructor to accept QueueCollection
    openglApp.Initialize();
    openglApp.Run();
    serverThread.join();
    // Ensure the server thread has completed before exiting the program
    clientThread.join(); // This line is commented out along with the initiation of the client thread
    return 0;
}
