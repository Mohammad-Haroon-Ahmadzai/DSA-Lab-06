/*
 * CS-250: Data Structures and Algorithms | Lab 6
 * Task 4: Waiting Room Management System (Emergency Ward)
 *
 * Array-based circular queue of patient IDs.
 * IDs are auto-generated starting from 1 and are never reused.
 */

#include <iostream>
#include <climits>
using namespace std;

class WaitingRoom
{
    static const int CAPACITY = 100;
    int queueArr[CAPACITY];
    int frontIdx;
    int count;
    int nextId; // next auto-generated patient ID

    int rearIndex() const { return (frontIdx + count) % CAPACITY; }

public:
    WaitingRoom() : frontIdx(0), count(0), nextId(1) {}

    bool isFull() const { return count == CAPACITY; }
    bool isEmpty() const { return count == 0; }

    // Assigns an auto-generated ID and adds the patient at the rear
    int RegisterPatient()
    {
        if (isFull())
        {
            cout << "Waiting room is full! Cannot register more patients.\n";
            return -1;
        }
        int id = nextId++;
        queueArr[rearIndex()] = id;
        count++;
        cout << "Patient registered with ID " << id << ". Patients waiting: " << count << "\n";
        return id;
    }

    // Calls the patient at the front of the queue (first come, first served)
    int ServePatient()
    {
        if (isEmpty())
        {
            cout << "No patients are waiting.\n";
            return -1;
        }
        int id = queueArr[frontIdx];
        frontIdx = (frontIdx + 1) % CAPACITY;
        count--;
        cout << "Now serving patient with ID " << id << ". Patients waiting: " << count << "\n";
        return id;
    }

    // Cancels all appointments (doctor goes to lunch). IDs keep increasing afterwards.
    void CancelAll()
    {
        int cancelled = count;
        frontIdx = 0;
        count = 0;
        cout << "All appointments cancelled (" << cancelled << " patient(s) removed).\n";
    }

    // True if nobody is waiting
    bool CanDoctorGoHome() const { return isEmpty(); }

    // Shows waiting IDs in sorted (ascending) order
    void ShowAllPatient() const
    {
        if (isEmpty()) { cout << "No patients are waiting.\n"; return; }

        int tmp[CAPACITY];
        for (int i = 0; i < count; i++) tmp[i] = queueArr[(frontIdx + i) % CAPACITY];

        // insertion sort on the copy (the queue itself is not modified)
        for (int i = 1; i < count; i++)
        {
            int key = tmp[i], j = i - 1;
            while (j >= 0 && tmp[j] > key) { tmp[j + 1] = tmp[j]; j--; }
            tmp[j + 1] = key;
        }

        cout << "Waiting patients (" << count << "): ";
        for (int i = 0; i < count; i++) cout << tmp[i] << (i + 1 < count ? ", " : "\n");
    }
};

int main()
{
    WaitingRoom room;
    int choice;

    do
    {
        cout << "\n===== Emergency Ward Waiting Room =====\n"
             << "1. Register Patient\n"
             << "2. Serve Patient\n"
             << "3. Cancel All Appointments\n"
             << "4. Can Doctor Go Home?\n"
             << "5. Show All Waiting Patients\n"
             << "0. Exit\n"
             << "Enter choice: ";
        if (!(cin >> choice)) break;

        switch (choice)
        {
        case 1: room.RegisterPatient(); break;
        case 2: room.ServePatient(); break;
        case 3: room.CancelAll(); break;
        case 4: cout << (room.CanDoctorGoHome() ? "true - no one is waiting, the doctor can go home.\n"
                                                : "false - patients are still waiting.\n"); break;
        case 5: room.ShowAllPatient(); break;
        case 0: cout << "Goodbye!\n"; break;
        default: cout << "Invalid choice, try again.\n";
        }
    } while (choice != 0);

    return 0;
}
