#include "ConcurrentQueue.h"
#include "ECSApp.h"
#include "QueueCollection.h"
#include "EntityCreationMessageV2.h"
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
#include <json.hpp>

#include "civetweb.h"

using json = nlohmann::json;

struct EntityDefinition
{
    float x, y, z;
    std::string type;

    EntityDefinition(float x, float y, float z, std::string type)
        : x(x), y(y), z(z), type(type) {}
};

void parseCommandV2(const std::string &command, QueueCollection &queues)
{
    using json = nlohmann::json;

    json j = json::parse(command);
    EntityCreationMessageV2 msg;

    try
    {
        msg.id = j.at("id").get<int>();
    }
    catch (const json::type_error &e)
    {
        std::string idStr = j.at("id").get<std::string>();
        msg.id = std::stoi(idStr);
    }

    msg.shapeType = j.at("shapeType").get<std::string>();
    msg.transform.position = j["transform"]["position"].get<std::vector<float>>();
    msg.transform.rotation = j["transform"]["rotation"].get<std::vector<float>>();
    msg.transform.scale = j["transform"]["scale"].get<std::vector<float>>();
    msg.shaders.vertexShader = j["shaders"]["vertex"].get<std::string>();
    msg.shaders.fragmentShader = j["shaders"]["fragment"].get<std::string>();

    // Extracting uniform data
    for (auto &[key, val] : j["uniforms"].items())
    {
        msg.uniforms.floatUniforms[key] = val.get<std::vector<float>>();
    }

    // Extracting vertex data
    if (j.find("vertexData") != j.end())
    {
        if (j["vertexData"].find("positions") != j["vertexData"].end())
            msg.vertexData.positions = j["vertexData"]["positions"].get<std::vector<float>>();
        if (j["vertexData"].find("normals") != j["vertexData"].end())
            msg.vertexData.normals = j["vertexData"]["normals"].get<std::vector<float>>();
        if (j["vertexData"].find("texCoords") != j["vertexData"].end())
            msg.vertexData.texCoords = j["vertexData"]["texCoords"].get<std::vector<float>>();
        if (j["vertexData"].find("colors") != j["vertexData"].end())
            msg.vertexData.colors = j["vertexData"]["colors"].get<std::vector<float>>();
    }

    // Pushing the parsed message into the creation queue
    std::vector<EntityCreationMessageV2> creationMessages{msg};
    queues.entityCreationV2Queue.Push(creationMessages);

    // Additional commands like DELETE or COLOR can be similarly handled
}

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
    else
    {
        std::cout << "command: " << command << std::endl;
        parseCommandV2(command, queues);
    }
}

int handlePostRequest(struct mg_connection *conn, void *cbdata)
{
    const struct mg_request_info *req_info = mg_get_request_info(conn);
    if (req_info->content_length > 0)
    {
        std::string post_data(req_info->content_length, '\0');
        mg_read(conn, &post_data[0], req_info->content_length);

        try
        {
            // Assuming QueueCollection is globally accessible or passed as callback data
            QueueCollection &queues = *static_cast<QueueCollection *>(cbdata);
            // Parse and process the request data
            parseCommandV2(post_data, queues);

            // Respond to the request indicating success
            mg_printf(conn,
                      "HTTP/1.1 200 OK\r\n"
                      "Content-Type: application/json\r\n\r\n"
                      "{\"status\": \"success\"}\n");
        }
        catch (const std::exception &e)
        {
            // Handle parsing error or processing error
            mg_printf(conn,
                      "HTTP/1.1 400 Bad Request\r\n"
                      "Content-Type: application/json\r\n\r\n"
                      "{\"error\": \"%s\"}\n",
                      e.what());
        }
    }
    else
    {
        // Handle case with no content provided
        mg_printf(conn,
                  "HTTP/1.1 411 Length Required\r\n"
                  "Content-Type: application/json\r\n\r\n"
                  "{\"error\": \"No data provided\"}\n");
    }
    return 200; // Return an HTTP status code
}

void ServerThreadV2(QueueCollection &queues)
{
    const char *options[] = {
        "listening_ports", "8080",
        0};

    mg_callbacks callbacks{};
    memset(&callbacks, 0, sizeof(callbacks));

    struct mg_context *ctx = mg_start(&callbacks, nullptr, options);
    if (!ctx)
    {
        std::cerr << "Could not start CivetWeb server." << std::endl;
        return;
    }

    mg_set_request_handler(ctx, "/entity", handlePostRequest, static_cast<void *>(&queues));

    std::cout << "CivetWeb server started. Press Enter to stop.\n";
    std::cin.get();

    mg_stop(ctx);
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
    address.sin_port = htons(8081);

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
            std::cout << "Message Received: beg " << buffer << " end" << std::endl;
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

std::string createMockEntityCreationMessage(int id, const std::string &shapeType, std::vector<float> position)
{
    json j;

    j["id"] = id;
    j["shapeType"] = shapeType;

    // Set common transform attributes
    j["transform"] = {
        {"position", position},
        {"rotation", {0.0, 0.0, 0.0, 1.0}},
        {"scale", {1.0, 1.0, 1.0}}};

    // Set common shaders
    j["shaders"] = {
        {"vertex", "shaders/vertex/basicTransform.vert"},
        {"fragment", "shaders/fragment/uniformColor.frag"}};

    // Set specific uniforms based on shape type
    if (shapeType == "triangle")
    {
        j["uniforms"] = {
            {"vertices", {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f}}, // Triangle
            {"color", {1.0, 0.0, 0.0, 1.0}}                                          // RGBA for red
        };
    }
    else if (shapeType == "square")
    {
        j["uniforms"] = {
            {"vertices", {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f}}, // Square
            {"color", {0.0, 1.0, 0.0, 1.0}}                                                                                                   // RGBA for green
        };
    }
    else if (shapeType == "pyramid")
    {
        j["uniforms"] = {
            {"vertices", {-0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f}},
            {"color", {0.0, 1.0, 0.0, 1.0}}};
    }
    else if (shapeType == "cube")
    {
        j["uniforms"] = {
            {"vertices", {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5}},
            {"color", {0.0, 1.0, 0.0, 1.0}}};
    }
    else
    {
        throw std::runtime_error("Unsupported shape type");
    }

    return j.dump() + "\n"; // Serialize to string
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
    std::cout << "ack" << std::endl;
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
        {-0.75f, -0.75f, 0.0f, "cube"},
        {2.0f, 2.0f, 0.0f, "pyramid"},
        {2.0f, -2.0f, 0.0f, "square"},
        {1.5f, 1.0f, 0.0f, "triangle"},
        {-1.5f, 2.0f, 0.0f, "cube"}};
    // {-0.75f, 0.75f, 0.0f, "triangle"},
    // {0.75f, 0.75f, 0.0f, "square"},
    // {0.75f, -0.75f, 0.0f, "triangle"},
    // {-0.75f, -0.75f, 0.0f, "square"}};

    // Track active entities by id
    std::unordered_set<int> activeEntities;

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081); // Set to match server port
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
        int action = rand() % 2; // 0 for remove, 1 for add, 3 for v2

        // Containers for messages
        std::vector<EntityCreationMessage> creations;
        std::vector<EntityDeletionMessage> deletions;

        // Random action: Add or Remove entities
        if (action == 1)
        {
            // Attempt to add a random entity if not already active
            // int entityId = rand() % entityDefinitions.size();
            int entityId = rand() % entityDefinitions.size();

            if (activeEntities.find(entityId) == activeEntities.end())
            {
                // const auto &def = entityDefinitions[entityId];
                // activeEntities.insert(entityId);
                // std::string createMessage = createMockEntityCreationMessage(entityId, def.type, {def.x, def.y, def.z});

                // std::cout << createMessage << " length: " << createMessage.length() << std::endl;

                // send(sock, createMessage.c_str(), createMessage.length(), 0);
                // waitForAck(sock);
                // close(sock); // Close the socket after sending message
            }
        }
        else if (action == 0)
        {
            // Attempt to remove a random entity if there exists any
            if (!activeEntities.empty())
            {
                // int removeIndex = rand() % activeEntities.size();
                // auto it = activeEntities.begin();
                // std::advance(it, removeIndex);
                // int entityIdToRemove = *it; // Obtain the actual entityId to remove
                // deletions.push_back(EntityDeletionMessage(entityIdToRemove));
                // activeEntities.erase(it);

                // std::string entityCommand = deleteEntityCommand(entityIdToRemove); // Now correctly referencing the ID
                // send(sock, entityCommand.c_str(), entityCommand.length(), 0);
                // waitForAck(sock);
            }
        }
        // create mock entity with V2

        // int entityId = rand() % entityDefinitions.size();

        // std::string message = createMockEntityCreationMessage(99, entityDefinitions[entityId].type, {0.0, 0.0, 0.0});

        // std::cout << message << " length: " << message.length() << std::endl;

        // send(sock, message.c_str(), message.length(), 0);
        // waitForAck(sock);

        close(sock);                                                 // Close the socket after sending message
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Throttle message sending
    }
}

int main()
{
    QueueCollection queues;
    std::thread server2Thread(ServerThread, std::ref(queues));
    std::thread serverThread(ServerThreadV2, std::ref(queues));
    std::thread clientThread(ClientThread);
    OpenGLApp openglApp(queues); // Adjust constructor to accept QueueCollection
    openglApp.Initialize();
    openglApp.Run();
    server2Thread.join();
    serverThread.join();
    // Ensure the server thread has completed before exiting the program
    clientThread.join(); // This line is commented out along with the initiation of the client thread
    return 0;
}
