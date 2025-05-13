#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>

class User {
protected:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& n, int id, int aL)
        : name(n), id(id), accessLevel(aL) {
        if (name.empty()) {
            throw std::invalid_argument("User name cannot be empty");
        }
        if (accessLevel < 0) {
            throw std::invalid_argument("Access level cannot be negative");
        }
    }

    ~User() {};

    virtual void displayInfo() = 0;

    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAL() const { return accessLevel; }
};

class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& n, int i, int aL, const std::string& g)
        : User(n, i, aL), group(g) {
        if (group.empty()) {
            throw std::invalid_argument("Student's group cannot be empty");
        }
    }

    void displayInfo() override {
        std::cout << "Name: " << name << "\nID: " << id << "\nGroup: " << group << "\nAccess Level: " << accessLevel << "\n\n";
    }

    std::string getGroup() const { return group; }
};

class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& n, int i, int aL, const std::string& d)
        : User(n, i, aL), department(d) {
        if (department.empty()) {
            throw std::invalid_argument("Teacher's department cannot be empty");
        }
    }

    void displayInfo() override {
        std::cout << "Name: " << name << "\nID: " << id << "\nDepartment: " << department << "\nAccess Level: " << accessLevel << "\n\n";
    }

    std::string getDep() const { return department; }
};

class Administrator : public User {
public:
    Administrator(const std::string& n, int i, int aL)
        : User(n, i, aL) {
    }

    void displayInfo() override {
        std::cout << "Name: " << name << "\nID: " << id << "\nAccess Level: " << accessLevel << "\n\n";
    }
};

class Resource {
private:
    std::string resourceName;
    int requiredAL;

public:
    Resource(const std::string& n, int aL)
        : resourceName(n), requiredAL(aL) {
    }

    bool checkAccess(const User& user) const {
        return user.getAL() >= requiredAL;
    }

    std::string getResourceName() const { return resourceName; }
};

template <typename T>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<User>> users;
    std::vector<Resource> resources;

public:
    void addUser(std::unique_ptr<User> user) {
        users.push_back(std::move(user));
    }

    void addResource(const Resource& resource) {
        resources.push_back(resource);
    }

    std::vector<User*> searchUsers(const std::string& query) {
        std::vector<User*> results;

        for (const auto& user : users) {
            if (user->getName() == query || std::to_string(user->getId()) == query) {
                results.push_back(user.get());
            }
        }

        return results;
    }

    void displayUsers(const std::vector<User*>& usersToDisplay) {
        for (const auto& user : usersToDisplay) {
            std::cout << "Name: " << user->getName() << ", ID: " << user->getId() << std::endl;
        }
    }

    void checkAccessToResource(const Resource& resource) const {
        for (const auto& user : users) {
            if (resource.checkAccess(*user)) {
                std::cout << user->getName() << " has access to " << resource.getResourceName() << std::endl;
            }
            else {
                std::cout << user->getName() << " doesnt have access to " << resource.getResourceName() << std::endl;
            }
        }
    }

    void saveToFile(const std::string& name) const {
        std::ofstream outFile(name);
        if (!outFile.is_open()) throw std::runtime_error("Could not open file for writing.");

        for (const auto& user : users) {
            outFile << user->getName() << "," << user->getId() << ","
                << user->getAL();
            if (const Student* student = dynamic_cast<const Student*>(user.get())) {
                outFile << "," << student->getGroup();
            }
            else if (const Teacher* teacher = dynamic_cast<const Teacher*>(user.get())) {
                outFile << "," << teacher->getDep();
            }
            outFile << "\n";
        }
        outFile.close();
    }

    void loadFromFile(const std::string& name) {
        std::ifstream inFile(name);
        if (!inFile.is_open()) throw std::runtime_error("Could not open file for reading.");

        users.clear();
        resources.clear();


        std::string line;
        while (std::getline(inFile, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            if (pos1 == std::string::npos || pos2 == std::string::npos) continue;

            std::string name = line.substr(0, pos1);
            int id = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
            int accessLevel = std::stoi(line.substr(pos2 + 1, pos3 - pos2 - 1));

            if (accessLevel == 1 && pos3 != std::string::npos) {
                std::string group = line.substr(pos3 + 1);
                addUser(std::make_unique<Student>(name, id, accessLevel, group));
            }
            else if (accessLevel == 2 && pos3 != std::string::npos) {
                std::string department = line.substr(pos3 + 1);
                addUser(std::make_unique<Teacher>(name, id, accessLevel, department));
            }
            else if (accessLevel == 3) {
                addUser(std::make_unique<Administrator>(name, id, accessLevel));
            }
        }
        inFile.close();
    }

    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(), [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) {
            return a->getAL() < b->getAL();
            });
    }

    void displayAllUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
        }
    }
};

int main() {
    try {
        AccessControlSystem<User> acs;

        std::cout << "Adding users and resources...\n";
        acs.addUser(std::make_unique<Student>("Maria", 1, 1, "A22"));
        acs.addUser(std::make_unique<Teacher>("Alex", 2, 2, "Math"));
        acs.addUser(std::make_unique<Administrator>("Dmitry", 3, 3));
        acs.addUser(std::make_unique<Student>("Robert", 4, 1, "B23"));
        acs.addUser(std::make_unique<Student>("Michael", 5, 1, "C21"));
        acs.addUser(std::make_unique<Student>("Ted", 6, 1, "C21"));
        acs.addUser(std::make_unique<Teacher>("James", 7, 2, "Physics"));

        acs.displayAllUsers();

        //std::cout << "Checking errors...\n";
        //acs.addUser(std::make_unique<Student>("Maria", 1, -1, "A22"));
        //acs.addUser(std::make_unique<Teacher>("Alex", 3, 2, ""));
        //acs.addUser(std::make_unique<Student>("", 5, 1, ""));

        Resource server("Server", 3);
        Resource lab("Laboratory", 2);
        Resource library("Library", 1);

        acs.addResource(lab);
        acs.addResource(library);

        std::cout << "\nChecking access to resources:";
        std::cout << "\nto " << server.getResourceName() << "\n";
        acs.checkAccessToResource(server);
        std::cout << "\nto " << lab.getResourceName() << "\n";
        acs.checkAccessToResource(lab);
        std::cout << "\nto " << library.getResourceName() << "\n";
        acs.checkAccessToResource(library);

        std::cout << "\nSaving and loading data...\n";
        acs.saveToFile("users.txt");
        acs.loadFromFile("users.txt");

        std::cout << "\nSorting all users by access level...\n";
        acs.sortUsersByAccessLevel();;
        acs.displayAllUsers();

        std::string query;
        std::cout << "Enter name or ID to search: ";
        std::cin >> query;

        std::vector<User*> foundUsers = acs.searchUsers(query);

        if (!foundUsers.empty()) {
            std::cout << "Found users:\n";
            acs.displayUsers(foundUsers);
        }
        else {
            std::cout << "No users found." << std::endl;
        }


    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
