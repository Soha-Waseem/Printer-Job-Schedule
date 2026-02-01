#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream> 

using namespace std;

struct Job {
    string jobName;
    int pages;
    int priority;
    Job* next;
};

Job* tail = NULL;
int totalPagesProcessed = 0;
int totalJobsProcessed = 0;
int sizeQ = 0;


bool isEmpty() {
    return tail == NULL;
}

// ---------- Load From File ----------
void loadFromFile() {
    ifstream file("queue.txt");
    if (!file) {
        cout << "No saved queue found. Starting fresh.\n";
        return;
    }

    string label;
    if (!(file >> label >> totalJobsProcessed) || label != "JobsProcessed") return;
    if (!(file >> label >> totalPagesProcessed) || label != "PagesPrinted") return;
    int count;
    if (!(file >> label >> count) || label != "QueueSize") return;

    file.ignore(); 

    for (int i = 0; i < count; i++) {
        string line;
        if (!getline(file, line) || line.empty()) continue;

        Job* job = new Job;


        size_t pos = line.find_last_of(' ');          // last space (before priority)
        size_t pos2 = line.find_last_of(' ', pos-1);  // space before pages
        job->pages = stoi(line.substr(pos2+1, pos-pos2-1));
        job->priority = stoi(line.substr(pos+1));
        job->jobName = line.substr(0, pos2);
       

        job->next = NULL;

        if (isEmpty()) {
            job->next = job;
            tail = job;
        } else {
            job->next = tail->next;
            tail->next = job;
            tail = job;
        }
        sizeQ++;
    }

    file.close();
}

// ---------- Save To File ----------
void saveToFile() {
    ofstream file("queue.txt");
    if (!file) {
        cout << "Error saving to file.\n";
        return;
    }

    file << "JobsProcessed " << totalJobsProcessed << endl;
    file << "PagesPrinted " << totalPagesProcessed << endl;
    file << "QueueSize " << sizeQ << endl;

    if (!isEmpty()) {
        Job* temp = tail->next;
        do {
            file << temp->jobName << " "
                 << temp->pages << " "
                 << temp->priority << endl;
            temp = temp->next;
        } while (temp != tail->next);
    }
    file.close();
}

// ---------- Add Job ----------
void addJob() {
    string name;
    int pages, priority = 0, setPriority = 0;

    cout << "Enter job name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter number of pages: ";
    if (!(cin >> pages) || pages <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid pages number.\n";
        return;
    }

    cout << "Set priority? (1=Yes, 0=No): ";
    if (!(cin >> setPriority) || (setPriority != 0 && setPriority != 1)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input for priority setting.\n";
        return;
    }

    if (setPriority == 1) {
        cout << "Enter priority: ";
        if (!(cin >> priority)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid priority.\n";
            return;
        }
    }

    clock_t start = clock();

    Job* newJob = new Job{name, pages, priority, NULL};

    if (isEmpty()) {
        newJob->next = newJob;
        tail = newJob;
    } else {
        Job* curr = tail->next;
        Job* prev = tail;
        bool inserted = false;

        do {
            if (newJob->priority > curr->priority) {
                prev->next = newJob;
                newJob->next = curr;
                if (curr == tail->next)
                    tail->next = newJob;
                inserted = true;
                break;
            }
            prev = curr;
            curr = curr->next;
        } while (curr != tail->next);

        if (!inserted) {
            newJob->next = tail->next;
            tail->next = newJob;
            tail = newJob;
        }
    }

    sizeQ++;

    clock_t end = clock();
    cout << "Job added successfully.\n";
    cout << "Time taken: " << (end - start) * 1000000.0 / CLOCKS_PER_SEC << " microseconds\n";
}

// ---------- Process Job ----------
void processJob() {
    if (isEmpty()) {
        cout << "Queue is empty.\n";
        return;
    }

    clock_t start = clock();

    Job* head = tail->next;
    cout << "Processing job: " << head->jobName << endl;

    totalJobsProcessed++;
    totalPagesProcessed += head->pages;

    if (head == tail)
        tail = NULL;
    else
        tail->next = head->next;

    delete head;
    sizeQ--;

    clock_t end = clock();
    cout << "Time taken: " << (end - start) * 1000000.0 / CLOCKS_PER_SEC << " microseconds\n";
}

// ---------- Delete Job ----------
void deleteJob() {
    if (isEmpty()) {
        cout << "Queue is empty.\n";
        return;
    }

    string name;
    cout << "Enter job name to delete: ";
    cin.ignore();
    getline(cin, name);

    Job* curr = tail->next;
    Job* prev = tail;

    do {
        if (curr->jobName == name) {
            if (curr == tail && curr == tail->next)
                tail = NULL;
            else {
                prev->next = curr->next;
                if (curr == tail) tail = prev;
            }
            delete curr;
            sizeQ--;
            cout << "Job deleted.\n";
            return;
        }
        prev = curr;
        curr = curr->next;
    } while (curr != tail->next);

    cout << "Job not found.\n";
}

// ---------- Search Job ----------
void searchJob() {
    if (isEmpty()) {
        cout << "Queue is empty.\n";
        return;
    }

    string name;
    cout << "Enter job name to search: ";
    cin.ignore();
    getline(cin, name);

    Job* curr = tail->next;
    do {
        if (curr->jobName == name) {
            cout << "Job found: " << curr->jobName
                 << " | Pages: " << curr->pages
                 << " | Priority: " << curr->priority << endl;
            return;
        }
        curr = curr->next;
    } while (curr != tail->next);

    cout << "Job not found.\n";
}

// ---------- View Queue ----------
void viewQueue() {
    if (isEmpty()) {
        cout << "Queue is empty.\n";
        return;
    }

    Job* temp = tail->next;
    cout << "\n--- CURRENT QUEUE ---\n";
    do {
        cout << temp->jobName
             << " | Pages: " << temp->pages
             << " | Priority: " << temp->priority << endl;
        temp = temp->next;
    } while (temp != tail->next);
}

// ---------- Status ----------
void status() {
    cout << "\n--- PRINTER STATUS ---\n";
    cout << "Total Jobs Processed: " << totalJobsProcessed << endl;
    cout << "Total Pages Printed: " << totalPagesProcessed << endl;
    cout << "Jobs Remaining: " << sizeQ << endl;
}

// ---------- Free Memory ----------
void freeQueue() {
    if (isEmpty()) return;

    Job* head = tail->next;
    tail->next = NULL;
    while (head) {
        Job* temp = head;
        head = head->next;
        delete temp;
    }
    tail = NULL;
}

// ---------- MAIN ----------
int main() {
    loadFromFile();

    int choice;
    do {
        cout << "\n===== PRINTER JOB SCHEDULER =====\n";
        cout << "1. Add Job\n";
        cout << "2. Process Job\n";
        cout << "3. View Queue\n";
        cout << "4. Printer Status\n";
        cout << "5. Delete Job\n";
        cout << "6. Search Job\n";
        cout << "7. Exit\n";
        cout << "Enter choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        switch (choice) {
            case 1: addJob(); break;
            case 2: processJob(); break;
            case 3: viewQueue(); break;
            case 4: status(); break;
            case 5: deleteJob(); break;
            case 6: searchJob(); break;
            case 7:
                saveToFile();
                freeQueue();
                cout << "Data saved. Exiting...\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }
    } while (choice != 7);

    return 0;
}