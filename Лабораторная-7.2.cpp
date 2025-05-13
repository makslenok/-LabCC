#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <iostream>

class Entity {
protected:
    std::string name;
    int health;
    int damage;

public:
    Entity(const std::string& n, int h, int d)
        : name(n), health(h), damage(d) {
    }

    ~Entity() {}

    virtual void displayInfo() {
        std::cout << "Name: " << Entity::name << ", health: " << Entity::health << ", Damage: " << Entity::damage << std::endl;
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getDamage() const { return damage; }
    void takeDamage(int amount) { health -= amount; }
    bool isAlive() const { return health > 0; }
};

class Hero : public Entity {
public:
    Hero(const std::string& n, int h, int d)
        : Entity(n, h, d) {
    }

    ~Hero() {}

    void attack(Entity& target)
    {
        int attackDamage = damage;
        if (rand() % 100 < 25)
        {
            attackDamage *= 2;
            std::cout << "Strong attack! " << name << " attacks " << target.getName() << " for " << damage * 2 << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        target.takeDamage(attackDamage);
    }

    void heal(int amount) {
        health += amount;
        if (health > 200) { health = 200; }
        std::cout << name << " healed for " << amount << " health point\n";
    }

    void displayInfo() override
    {
        if (health < 0) { health = 0; }
        std::cout << "Hero: " << name << ", health: " << health << std::endl;
    }
};

class Monster : public Entity
{
public:
    Monster(const std::string& n, int h, int d)
        : Entity(n, h, d) {
    }

    ~Monster() {}

    void attack(Entity& target)
    {
        int attackDamage = damage;
        if (rand() % 100 < 20)
        {
            attackDamage *= 2;
            std::cout << "Strong attack! " << name << " attacks " << target.getName() 
                << " for " << damage * 2 << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        target.takeDamage(attackDamage);
    }

    void displayInfo() override
    {
        std::cout << "Monster: " << name << ", health: " << health
            << ", damage: " << damage << std::endl;
    }
};

std::vector<Monster> monsters;
std::mutex monstersMutex;
std::mutex fightMutex;

void generator() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::lock_guard<std::mutex> lock(monstersMutex);
        for (int i = 1; i <= 100; ++i) {
            std::string monsterName = "Goblin #" + std::to_string(i);
            monsters.push_back(Monster(monsterName, 50, 25));
        };
        std::cout << "New monster generated!\n";
    }
}
void fight(Hero& hero, Monster& monster) {
    std::cout << "\nBattle: " << hero.getName() << " vs " << monster.getName() << "!\n";
    while (hero.isAlive() && monster.isAlive()) {
        std::lock_guard<std::mutex> lock(fightMutex);

        hero.attack(monster);
        if (!monster.isAlive()) {
            std::cout << monster.getName() << " defeated!\n";
            break;
        }

        monster.attack(hero);
        if (!hero.isAlive()) {
            std::cout << hero.getName() << " defeated!\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (hero.isAlive()) {
        hero.heal(15);
    }
}

int main() {
    Hero hero("Knight", 80, 25);

    std::thread monsterGenerator(generator);
    monsterGenerator.detach();

    while (hero.isAlive()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock(monstersMutex);
        if (!monsters.empty()) {
            Monster currentMonster = monsters[0];
            monsters.erase(monsters.begin());
            std::thread fightThread(fight, std::ref(hero), std::ref(currentMonster));
            fightThread.join();
            std::cout << "\nHero indicators:\n";
            hero.displayInfo();
        }
    }
    std::cout << "Game Over!\n";
    return 0;
}
