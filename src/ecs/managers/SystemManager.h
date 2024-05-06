#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility> // For std::forward

class SystemManager
{
public:
    // Template method to add systems with variable constructor arguments
    template <typename T, typename... Args>
    void AddSystem(Args &&...args)
    {
        // Create a system of type T, forwarding the arguments
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        auto typeIdx = std::type_index(typeid(T));
        systems[typeIdx] = std::move(system);
    }

    template <typename T>
    T &GetSystem() const
    {
        auto typeIdx = std::type_index(typeid(T));
        auto it = systems.find(typeIdx);
        if (it != systems.end())
        {
            return *static_cast<T *>(it->second.get());
        }
        throw std::runtime_error("System not found: " + std::string(typeid(T).name()));
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<System>> systems;
};
