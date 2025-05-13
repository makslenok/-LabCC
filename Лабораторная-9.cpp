#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <memory>

class Character {
private:
    std::string name;
    int level, xp, hp, maxHp, attack, defense;

public:
    Character(std::string n = "Hero")
        : name(n), level(1), xp(0), hp(100), maxHp(100), attack(10), defense(5) {
    }

    Character(std::string n, int hp_, int atk_, int def_, int maxHp_, int lvl_, int exp_)
        : name(n), hp(hp_), attack(atk_), defense(def_), maxHp(maxHp_), level(lvl_), xp(exp_) {
    }

    void gainExp(int amount) {
        xp += amount;
        if (xp >= pow(10, level)) {
            xp -= pow(10, level);
            level++;
            std::cout << "You`ve reached new level: " << level << "!\n" << "Your stats are increased!\n";
            increaseStats();
        }
    }

    void heal() {
        hp += 20;
        if (hp > maxHp) hp = maxHp;
        std::cout << name << " healed for 20hp\n";
    }

    void takeDamage(int amount) {
        int damage = amount - defense;
        if (damage < 0) damage = 0;
        hp -= damage;
        std::cout << name << " takes " << damage << " damage.\n";
    }

    void increaseStats() {
        attack += 2;
        defense += 1;
        maxHp += 10;
        hp += 10;
    }

    void status() const {
        std::cout << "Character's stats:\n";
        showInfo();
    }

    void showInfo() const {
        std::cout << name << "\nLevel: " << level << "\nXP: " << xp << "\nHealth: " << hp << "/" << maxHp << "\nDamage: " << attack
            << "\nDefence: " << defense << "\n\n";
    }

    std::string getName() const { return name; }
    bool isAlive() const { return hp > 0; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHp; }
    int getLevel() const { return level; }
    int getExp() const { return xp; }
};

class Monster {
protected:
    std::string name;
    int hp, attack, defense;

public:
    Monster(std::string n, int h, int a, int d)
        : name(n), hp(h), attack(a), defense(d) {}

    ~Monster() {};

    void showInfo() const {
        std::cout << name << "\nHealth: " << hp << ", Damage: " << attack << ", Defence: " << defense << "\n";
    }

    void takeDamage(int dmg) {
        int damage = dmg - defense;
        if (damage < 0) damage = 0;
        hp -= damage;
        std::cout << name << " takes " << damage << " damage.\n";
    }

    bool isAlive() const { return hp > 0; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    std::string getName() const { return name; }
};

class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 40, 15, 5) {}
};

class Elemental : public Monster {
public:
    Elemental() : Monster("Elemental", 20, 10, 2) {}
};

template<typename T>
class Inventory {
private:
    std::vector<T> items;

public:
    void addItem(const T& item) {
        items.push_back(item);
    }

    void removeItem(const T& item) {
        auto it = std::find(items.begin(), items.end(), item);
        if (it != items.end()) items.erase(it);
    }

    bool contains(const T& item) const {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

    void showInventory() const {
        std::cout << "Inventory:\n";
        if (items.empty()) std::cout << "  (empty)\n";
        for (const auto& item : items) std::cout << "  - " << item << "\n";
    }
};

class Game {
private:
    Character player;
    Inventory<std::string> inventory;

    std::unique_ptr<Monster> spawnMonster() {
        int r = rand() % 4;
        if (r == 0 || r == 1 || r == 2) return std::make_unique<Goblin>();
        return std::make_unique<Elemental>();
    }

    void battle(std::unique_ptr<Monster> monster) {
        while (player.isAlive() && monster->isAlive()) {
            std::cout << "\n";
            player.showInfo();
            monster->showInfo();

            std::cout << "Attack or run? \n";
            std::string action;
            std::cin >> action;

            if (action == "run") {
                std::cout << "You escaped!\n";
                return;
            }

            monster->takeDamage(player.getAttack());
            if (!monster->isAlive()) break;
            player.takeDamage(monster->getAttack());
        }

        if (player.isAlive()) {
            std::cout << "You defeated the " << monster->getName() << "!\n";
            player.gainExp(5 + rand() % 6);
            inventory.addItem("Healing Potion");
        }
    }

    void saveGame() {
        std::ofstream out("game.txt");
        if (!out) {
            std::cerr << "Failed saving.\n";
            return;
        }

        out << player.getName() << "\n";
        out << player.getHP() << " " << player.getAttack() << " " << player.getDefense() << " "
            << player.getMaxHP() << " " << player.getLevel() << " " << player.getExp() << "\n";
        out << inventory.contains("Healing Potion") << " " << inventory.contains("Sword") << "\n";
        std::cout << "Game saved.\n";
    }

    void loadGame() {
        std::ifstream in("game.txt");
        if (!in) {
            std::cerr << "Failed loading. No such file.\n";
            return;
        }

        int lvl, xp, hp, maxHp, atk, def;
        std::string name;
        int hasPotion, hasSword;
        in.ignore();
        std::getline(in, name);
        in >> lvl >> xp >> hp >> maxHp >> atk >> def;
        in >> hasPotion >> hasSword;

        player = Character(name, lvl, xp, hp, maxHp, atk, def);
        inventory = Inventory<std::string>();
        if (hasPotion) inventory.addItem("Healing Potion");
        if (hasSword) inventory.addItem("Sword");

        std::cout << "Game loaded.\n";
        player.status();
    }

    void processAction() {

        std::string cmd;
        std::cin >> cmd;

        if (cmd == "run") {
            player.gainExp(1); 
            if (rand() % 2 == 0) {
                std::cout << "Monster appears!\n";
                auto monster = spawnMonster();
                battle(std::move(monster));
            }
        }
        else if (cmd == "heal") {
            if (inventory.contains("Healing Potion")) {
                player.heal();
                inventory.removeItem("Healing Potion");
            }
            else {
                std::cout << "No healing potion in the inventory!\n";
            }
        }

        else if (cmd == "inventory") {
            inventory.showInventory();
        }

        else if (cmd == "save") {
            saveGame();
        }

        else if (cmd == "load") {
            loadGame();
        }

        else if (cmd == "status") {
            player.status();
        }

        else if (cmd == "quit") {
            saveGame();
            exit(0);
        }

        else { std::cout << "Unknown command.\n"; }
    }

public:
    void start() {
        std::cout << "Load game or new game? (load/new): ";
        std::string choice;
        std::cin >> choice;

        if (choice == "load") {
            loadGame();
        }
        else if (choice == "new") {
            std::cout << "Enter your character's name: ";
            std::string name;
            std::cin.ignore();
            std::getline(std::cin, name);
            player = Character(name);
            inventory.addItem("Healing potion");
            inventory.addItem("Sword");
        }
        else { std::cout << "Unknown command.\n"; }

        std::cout << "Game started. Commands: run, heal, inventory, save, load, status, quit\n";

        while (true) {
            processAction();
            if (!player.isAlive()) {
                std::cout << "You died. Game over.\n";
                break;
            }
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    Game game;
    game.start();
    return 0;
}
