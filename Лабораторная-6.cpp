
#include <iostream>
#include <vector>
#include <string>


template <typename T>
class Queue {
private:
    std::vector<T> data;
    size_t index = 0;

public:
    void push(const T& item) {
        data.push_back(item);
        std::cout << "Added: " << item << " to queue\n";
    }

    T pop() {
        if (data.empty()) {
            throw std::runtime_error("Queue is empty! Cannot pop.");
        }
        T removedItem = data[index];
        index++;

        if (index > data.size() / 2) {
            data.erase(data.begin(), data.begin() + index);
            index = 0;
        }
        return removedItem;
    }

    T front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[index];
    }

    bool empty() const {
        return index >= data.size();
    }
};


int main()
{
    try
    {
        Queue<int> intQueue;

        intQueue.push(5);
        intQueue.push(8);
        intQueue.push(1);
        intQueue.push(7);

        std::cout << intQueue.pop() << " deleted from queue" << std::endl;
        std::cout << intQueue.pop() << " deleted from queue" << std::endl;
        std::cout << intQueue.pop() << " deleted from queue" << std::endl;
        std::cout << intQueue.pop() << " deleted from queue" << std::endl;
        std::cout << intQueue.pop() << " deleted from queue" << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
