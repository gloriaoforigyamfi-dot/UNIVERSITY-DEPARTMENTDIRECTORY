#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

struct Staff {
    int staffID;
    string name;
    string department;
    string position;
    string email;
};

struct Node {
    Staff data;
    Node* left;
    Node* right;

    Node(const Staff& s) : data(s), left(nullptr), right(nullptr) {}
};

class DepartmentDirectory {
private:
    Node* root;
    int count;

    Node* insertHelper(Node* node, const Staff& s, bool& inserted) {
        if (node == nullptr) {
            inserted = true;
            return new Node(s);
        }
        if (s.staffID < node->data.staffID)
            node->left = insertHelper(node->left, s, inserted);
        else if (s.staffID > node->data.staffID)
            node->right = insertHelper(node->right, s, inserted);
        else
            inserted = false; // Duplicate
        return node;
    }

    Node* findMin(Node* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    Node* deleteHelper(Node* node, int id, bool& deleted) {
        if (node == nullptr) return nullptr;
        if (id < node->data.staffID)
            node->left = deleteHelper(node->left, id, deleted);
        else if (id > node->data.staffID)
            node->right = deleteHelper(node->right, id, deleted);
        else {
            deleted = true;

            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            }

            if (!node->left) {
                Node* temp = node->right;
                delete node;
                return temp;
            }
            if (!node->right) {
                Node* temp = node->left;
                delete node;
                return temp;
            }

            Node* successor = findMin(node->right);
            node->data = successor->data;
            node->right = deleteHelper(node->right, successor->data.staffID, deleted);
        }
        return node;
    }

    Node* searchHelper(Node* node, int id) {
        while (node) {
            if (id == node->data.staffID) return node;
            node = (id < node->data.staffID) ? node->left : node->right;
        }
        return nullptr;
    }

    void inorder(Node* node) const {
        if (!node) return;
        inorder(node->left);
        printStaff(node->data);
        inorder(node->right);
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    int countNodes(Node* node) const {
        if (!node) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

    int readInt(const string& prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [Error] Invalid input. Please enter an integer.\n";
        }
    }

    string readString(const string& prompt) {
        string value;
        while (true) {
            cout << prompt;
            getline(cin, value);

            size_t start = value.find_first_not_of(" \t");
            size_t end = value.find_last_not_of(" \t");
            if (start != string::npos) {
                value = value.substr(start, end - start + 1);
            } else {
                value = "";
            }
            if (!value.empty()) return value;
            cout << "  [Error] Input cannot be empty.\n";
        }
    }

    void printStaff(const Staff& s) const {
        cout << left
             << setw(8)  << s.staffID
             << setw(22) << s.name
             << setw(20) << s.department
             << setw(20) << s.position
             << s.email << "\n";
    }

    void printHeader() const {
        cout << left
             << setw(8)  << "ID"
             << setw(22) << "Name"
             << setw(20) << "Department"
             << setw(20) << "Position"
             << "Email" << "\n";
        cout << string(90, '-') << "\n";
    }

public:
    DepartmentDirectory() : root(nullptr), count(0) {}
    ~DepartmentDirectory() { destroy(root); }

    bool insertStaff(const Staff& s) {
        bool inserted = false;
        root = insertHelper(root, s, inserted);
        if (inserted) count++;
        return inserted;
    }

    Staff* searchStaff(int id) {
        Node* found = searchHelper(root, id);
        return found ? &found->data : nullptr;
    }

    bool deleteStaff(int id) {
        bool deleted = false;
        root = deleteHelper(root, id, deleted);
        if (deleted) count--;
        return deleted;
    }

    bool updateStaff(int id) {
        Node* node = searchHelper(root, id);
        if (!node) {
            cout << "  [Error] Staff ID " << id << " not found.\n";
            return false;
        }
        cout << "\n  Current record:\n";
        printHeader();
        printStaff(node->data);
        cout << "\n  Enter new details (leave current by typing new value):\n";
        node->data.name       = readString("  New Name       : ");
        node->data.department = readString("  New Department : ");
        node->data.position   = readString("  New Position   : ");
        node->data.email      = readString("  New Email      : ");
        cout << "  [OK] Staff record updated.\n";
        return true;
    }

    void displayAll() const {
        if (!root) {
            cout << "  [Info] Directory is empty.\n";
            return;
        }
        cout << "\n  ---- Staff Directory (sorted by ID) ----\n";
        printHeader();
        inorder(root);
        cout << string(90, '-') << "\n";
        cout << "  Total staff: " << count << "\n";
    }

    bool findMinID(int& outID, string& outName) {
        if (!root) return false;
        Node* m = findMin(root);
        outID = m->data.staffID;
        outName = m->data.name;
        return true;
    }

    bool findMaxID(int& outID, string& outName) {
        if (!root) return false;
        Node* m = root;
        while (m->right) m = m->right;
        outID = m->data.staffID;
        outName = m->data.name;
        return true;
    }

    bool isEmpty() const { return root == nullptr; }
    int size() const { return count; }
};


bool isValidEmail(const string& email) {
    size_t at = email.find('@');
    size_t dot = email.find('.', at == string::npos ? 0 : at);
    return at != string::npos && dot != string::npos && at > 0 && dot > at + 1;
}

int readPositiveInt(const string& prompt) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (v > 0) return v;
            cout << "  [Error] ID must be a positive integer.\n";
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [Error] Invalid input. Please enter a positive integer.\n";
        }
    }
}

string readNonEmpty(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        size_t start = s.find_first_not_of(" \t");
        size_t end = s.find_last_not_of(" \t");
        if (start != string::npos) {
            s = s.substr(start, end - start + 1);
            return s;
        }
        cout << "  [Error] Input cannot be empty.\n";
    }
}

string readValidEmail(const string& prompt) {
    string e;
    while (true) {
        cout << prompt;
        getline(cin, e);
        if (isValidEmail(e)) return e;
        cout << "  [Error] Invalid email format. Try again.\n";
    }
}


void addStaffMenu(DepartmentDirectory& dir) {
    cout << "\n  --- Add Staff ---\n";
    Staff s;
    s.staffID    = readPositiveInt("  Staff ID    : ");
    s.name       = readNonEmpty  ("  Name        : ");
    s.department = readNonEmpty  ("  Department  : ");
    s.position   = readNonEmpty  ("  Position    : ");
    s.email      = readValidEmail("  Email       : ");

    if (dir.insertStaff(s))
        cout << "  [OK] Staff added successfully.\n";
    else
        cout << "  [Error] Duplicate Staff ID (" << s.staffID << ").\n";
}

void searchStaffMenu(DepartmentDirectory& dir) {
    cout << "\n  --- Search Staff ---\n";
    if (dir.isEmpty()) {
        cout << "  [Info] Directory is empty.\n";
        return;
    }
    int id = readPositiveInt("  Enter Staff ID: ");
    Staff* s = dir.searchStaff(id);
    if (!s) {
        cout << "  [Error] Staff ID " << id << " not found.\n";
        return;
    }
    cout << "\n  Record found:\n";
    cout << left << setw(15) << "  ID"         << ": " << s->staffID    << "\n";
    cout << left << setw(15) << "  Name"       << ": " << s->name       << "\n";
    cout << left << setw(15) << "  Department" << ": " << s->department << "\n";
    cout << left << setw(15) << "  Position"   << ": " << s->position   << "\n";
    cout << left << setw(15) << "  Email"      << ": " << s->email      << "\n";
}

void deleteStaffMenu(DepartmentDirectory& dir) {
    cout << "\n  --- Delete Staff ---\n";
    if (dir.isEmpty()) {
        cout << "  [Info] Directory is empty.\n";
        return;
    }
    int id = readPositiveInt("  Enter Staff ID to delete: ");
    if (dir.deleteStaff(id))
        cout << "  [OK] Staff ID " << id << " deleted.\n";
    else
        cout << "  [Error] Staff ID " << id << " not found.\n";
}

void updateStaffMenu(DepartmentDirectory& dir) {
    cout << "\n  --- Update Staff ---\n";
    if (dir.isEmpty()) {
        cout << "  [Info] Directory is empty.\n";
        return;
    }
    int id = readPositiveInt("  Enter Staff ID to update: ");
    dir.updateStaff(id);
}

void displayAllMenu(const DepartmentDirectory& dir) {
    cout << "\n  --- Display All Staff ---\n";
    dir.displayAll();
}

void findMinMaxMenu(DepartmentDirectory& dir) {
    cout << "\n  --- Find Minimum / Maximum ID ---\n";
    if (dir.isEmpty()) {
        cout << "  [Info] Directory is empty.\n";
        return;
    }
    int minID, maxID;
    string minName, maxName;
    if (dir.findMinID(minID, minName))
        cout << "  Minimum ID: " << minID << "  (" << minName << ")\n";
    if (dir.findMaxID(maxID, maxName))
        cout << "  Maximum ID: " << maxID << "  (" << maxName << ")\n";
}


int main() {
    DepartmentDirectory dir;
    int choice;

    cout << "=========================================================\n";
    cout << "       UNIVERSITY DEPARTMENT DIRECTORY SYSTEM            \n";
    cout << "=========================================================\n";

    do {
        cout << "\n  ------ Main Menu ------\n";
        cout << "  1. Insert Staff\n";
        cout << "  2. Search Staff by ID\n";
        cout << "  3. Delete Staff\n";
        cout << "  4. Update Staff\n";
        cout << "  5. Display All Staff\n";
        cout << "  6. Find Minimum ID\n";
        cout << "  7. Find Maximum ID\n";
        cout << "  8. Count Staff\n";
        cout << "  0. Exit\n";

        choice = readPositiveInt("  Select option: ");

        switch (choice) {
            case 1: addStaffMenu(dir);        break;
            case 2: searchStaffMenu(dir);     break;
            case 3: deleteStaffMenu(dir);     break;
            case 4: updateStaffMenu(dir);     break;
            case 5: displayAllMenu(dir);      break;
            case 6: case 7: findMinMaxMenu(dir); break;
            case 8:
                cout << "  Total staff in directory: " << dir.size() << "\n";
                break;
            case 0:
                cout << "\n  Exiting system. Goodbye!\n";
                break;
            default:
                cout << "  [Error] Invalid option. Choose 0-8.\n";
        }
    } while (choice != 0);

    return 0;
}
