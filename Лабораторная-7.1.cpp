#include <memory>

class Entity {
private:
    std::string name;
    int health;
    int level;

public:
    Entity(const std::string& n, int h, int l)
        : name(n), health(h), level(l) {}

    ~Entity() {}

    void displayInfo() const {
        std::cout << "Name: " << Entity::name << ", health: " << Entity::health << ", Level: " << Entity::level << std::endl;
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getLevel() const { return level; }
};

template <typename T>
class Queue {
    std::vector<T> entities;
public:
    void addEntity(T entity) {
        entities.push_back(entity);
    }

    void display() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }

    const std::vector<T>& getEntities() const {
        return entities;
    }
};

void saveToFile(const Queue<std::shared_ptr<Entity>>& queue, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing.");
    }
    for (const auto& entity : queue.getEntities()) {
        file << entity->getName() << " " << entity->getHealth() << " " << entity->getLevel() << std::endl;
    }
}

void loadFromFile(Queue<std::shared_ptr<Entity>>& queue, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    std::string name;
    int HP, level;

    while (file >> name >> HP >> level) {
        queue.addEntity(std::make_shared<Entity>(name, HP, level));
    };
}

int main() {
    try
    {
        Queue<std::shared_ptr<Entity>> manager;
        manager.addEntity(std::make_shared<Entity>("Hero", 100, 5));
        manager.addEntity(std::make_shared<Entity>("Enemy", 200, 1));
        saveToFile(manager, "game_save.txt");

        Queue<std::shared_ptr<Entity>> loadedManager;
        loadFromFile(loadedManager, "game_save.txt");
        loadedManager.display();

        std::cout << "\nList of entities:\n";
        loadedManager.display();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
