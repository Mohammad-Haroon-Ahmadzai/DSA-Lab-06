/*
 * CS-250: Data Structures and Algorithms | Lab 6
 * Task 3: Print Server Request Handling System
 *
 * Circular queue with dynamic resizing:
 *  - grows (x2) automatically when an enqueue finds the queue full
 *  - shrinks (/2) automatically when it becomes too sparse (size <= capacity/4)
 *  - never shrinks below MIN_CAPACITY and never loses any request data
 */

#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

struct PrintRequest
{
    int id;
    int arrivalTime;
    int processingTime;

    PrintRequest() : id(0), arrivalTime(0), processingTime(0) {}
    PrintRequest(int i, int a, int p) : id(i), arrivalTime(a), processingTime(p) {}
};

class CircularQueue
{
    PrintRequest* arr;
    int capacity;
    int frontIdx;
    int rearIdx; // index of the last element
    int count;
    static const int MIN_CAPACITY = 4;

public:
    CircularQueue(int cap = MIN_CAPACITY)
    {
        capacity = (cap < MIN_CAPACITY) ? MIN_CAPACITY : cap;
        arr = new PrintRequest[capacity];
        frontIdx = 0;
        rearIdx = -1;
        count = 0;
    }

    ~CircularQueue() { delete[] arr; }
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == capacity; }
    int size() const { return count; }
    int getCapacity() const { return capacity; }

    // Copies all elements into a new array in logical order (front at index 0)
    void resize(int newCapacity)
    {
        if (newCapacity < count || newCapacity < MIN_CAPACITY) return; // never lose data
        PrintRequest* bigger = new PrintRequest[newCapacity];
        for (int i = 0; i < count; i++)
            bigger[i] = arr[(frontIdx + i) % capacity];
        delete[] arr;
        arr = bigger;
        cout << "  [resize] capacity " << capacity << " -> " << newCapacity
             << " (" << count << " request(s) preserved)\n";
        capacity = newCapacity;
        frontIdx = 0;
        rearIdx = count - 1;
    }

    void enqueue(const PrintRequest& x)
    {
        if (isFull()) resize(capacity * 2); // auto-grow
        rearIdx = (rearIdx + 1) % capacity;
        arr[rearIdx] = x;
        count++;
    }

    PrintRequest dequeue()
    {
        if (isEmpty()) { cout << "Queue is empty!\n"; return PrintRequest(); }
        PrintRequest x = arr[frontIdx];
        frontIdx = (frontIdx + 1) % capacity;
        count--;
        if (capacity > MIN_CAPACITY && count <= capacity / 4) // auto-shrink
            resize(capacity / 2);
        return x;
    }

    PrintRequest front() const
    {
        if (isEmpty()) { cout << "Queue is empty!\n"; return PrintRequest(); }
        return arr[frontIdx];
    }

    // Prints the physical array (slots) plus front/rear markers
    void display() const
    {
        cout << "  Queue state: size=" << count << ", capacity=" << capacity
             << ", front=" << (isEmpty() ? -1 : frontIdx)
             << ", rear=" << (isEmpty() ? -1 : rearIdx) << "\n";
        cout << "  Slots: ";
        for (int i = 0; i < capacity; i++)
        {
            bool occupied = false;
            for (int k = 0; k < count; k++)
                if ((frontIdx + k) % capacity == i) { occupied = true; break; }
            if (occupied) cout << "[R" << arr[i].id << "] ";
            else cout << "[ -- ] ";
        }
        cout << "\n";
    }
};

int main()
{
    CircularQueue server(4);
    int serverTime = 0; // time at which the printer becomes free
    int processed = 0;
    long totalWait = 0;
    int choice;

    do
    {
        cout << "\n===== Print Server Request Handling System =====\n"
             << "1. Enqueue a print request\n"
             << "2. Dequeue (process next request)\n"
             << "3. Show front request\n"
             << "4. Display queue state\n"
             << "5. Check isEmpty() / isFull()\n"
             << "6. Show average waiting time\n"
             << "0. Exit\n"
             << "Enter choice: ";
        if (!(cin >> choice)) break;

        if (choice == 1)
        {
            int id, arrival, proc;
            cout << "Request ID: "; cin >> id;
            cout << "Arrival time: "; cin >> arrival;
            cout << "Processing time: "; cin >> proc;
            server.enqueue(PrintRequest(id, arrival, proc));
            cout << "Request R" << id << " enqueued.\n";
            server.display();
        }
        else if (choice == 2)
        {
            if (server.isEmpty()) { cout << "Queue is empty, nothing to process.\n"; continue; }
            PrintRequest r = server.dequeue();
            int start = (serverTime > r.arrivalTime) ? serverTime : r.arrivalTime;
            int wait = start - r.arrivalTime;
            serverTime = start + r.processingTime;
            totalWait += wait;
            processed++;
            cout << "Processing R" << r.id << ": starts at t=" << start << ", finishes at t=" << serverTime
                 << ", waiting time = " << wait << "\n";
            server.display();
        }
        else if (choice == 3)
        {
            if (server.isEmpty()) cout << "Queue is empty.\n";
            else
            {
                PrintRequest r = server.front();
                cout << "Front: R" << r.id << " (arrival=" << r.arrivalTime
                     << ", processing=" << r.processingTime << ")\n";
            }
        }
        else if (choice == 4)
        {
            server.display();
        }
        else if (choice == 5)
        {
            cout << boolalpha << "isEmpty() = " << server.isEmpty()
                 << ", isFull() = " << server.isFull() << "\n";
        }
        else if (choice == 6)
        {
            if (processed == 0) cout << "No requests processed yet.\n";
            else cout << fixed << setprecision(2) << "Processed " << processed << " request(s), total wait = "
                      << totalWait << ", average waiting time = " << (double)totalWait / processed << "\n";
        }
        else if (choice != 0)
        {
            cout << "Invalid choice, try again.\n";
        }
    } while (choice != 0);

    cout << "Goodbye!\n";
    return 0;
}
