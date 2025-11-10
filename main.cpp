#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>

using namespace std;

const string DB_FILENAME = "inventory.txt";
const char DELIM = '|';

struct Item {
    int id;
    string name;
    string category;
    int quantity;
    double price;
};

// Utility: parse line (id|name|category|quantity|price)
Item parseLine(const string &line) {
    Item it{};
    stringstream ss(line);
    string token;
    vector<string> parts;

    while (getline(ss, token, DELIM))
        parts.push_back(token);
    if (parts.size() >= 5) {
        it.id = stoi(parts[0]);
        it.name = parts[1];
        it.category = parts[2];
        it.quantity = stoi(parts[3]);
        it.price = stod(parts[4]);
    }
    return it;
}

string toLine(const Item &it) {
    return to_string(it.id) + string(1, DELIM) + it.name + string(1, DELIM) +
           it.category + string(1, DELIM) + to_string(it.quantity) +
           string(1, DELIM) + to_string(it.price);
}

vector<Item> loadRecords() {
    vector<Item> items;
    ifstream fin(DB_FILENAME);
    if (!fin.is_open())
        return items;

    string line;
    while (getline(fin, line)) {
        if (line.empty())
            continue;
        items.push_back(parseLine(line));
    }
    fin.close();
    return items;
}

bool saveRecords(const vector<Item> &items) {
    ofstream fout(DB_FILENAME, ios::trunc);
    if (!fout.is_open())
        return false;
    for (const auto &it : items)
        fout << toLine(it) << '\n';
    fout.close();
    return true;
}

int getNextID(const vector<Item> &items) {
    int mx = 0;
    for (const auto &it : items)
        if (it.id > mx)
            mx = it.id;
    return mx + 1;
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void addItem(vector<Item> &items) {
    Item it;
    it.id = getNextID(items);
    cout << "\n--- Add New Product ---\n";
    cout << "Product name: ";
    cin.ignore();
    getline(cin, it.name);
    cout << "Category: ";
    getline(cin, it.category);
    cout << "Quantity: ";
    while (!(cin >> it.quantity) || it.quantity < 0) {
        cout << "Enter a valid non-negative integer for quantity: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Price: ";
    while (!(cin >> it.price) || it.price < 0) {
        cout << "Enter a valid non-negative number for price: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    items.push_back(it);
    if (saveRecords(items))
        cout << "Product added successfully. ID = " << it.id << '\n';
    else
        cout << "Error: could not save record.\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void displayAll(const vector<Item> &items) {
    cout << "\n--- Inventory List ---\n";
    if (items.empty()) {
        cout << "No products found.\n";
        return;
    }
    cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(15) << "Category"
         << setw(10) << "Qty" << setw(10) << "Price\n";
    cout << string(70, '-') << '\n';
    for (const auto &it : items) {
        cout << left << setw(6) << it.id << setw(25) << it.name << setw(15) << it.category
             << setw(10) << it.quantity << setw(10) << fixed << setprecision(2) << it.price << '\n';
    }
}

void searchByID(const vector<Item> &items) {
    cout << "\nEnter Product ID to search: ";
    int id;
    if (!(cin >> id)) {
        cout << "Invalid input.\n";
        cin.clear();
        cin.ignore();
        return;
    }
    for (const auto &it : items) {
        if (it.id == id) {
            cout << "\nFound:\n";
            cout << "ID: " << it.id << "\nName: " << it.name << "\nCategory: " << it.category
                 << "\nQuantity: " << it.quantity << "\nPrice: " << it.price << '\n';
            return;
        }
    }
    cout << "Product with ID " << id << " not found.\n";
}

void searchByName(const vector<Item> &items) {
    cout << "\nEnter name or substring to search: ";
    cin.ignore();
    string q;
    getline(cin, q);
    bool found = false;

    for (const auto &it : items) {
        string lowerName = it.name;
        string lowerQ = q;
        transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        transform(lowerQ.begin(), lowerQ.end(), lowerQ.begin(), ::tolower);
        if (lowerName.find(lowerQ) != string::npos) {
            if (!found) {
                cout << "\nMatches:\n";
                cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(15) << "Category"
                     << setw(10) << "Qty" << setw(10) << "Price\n";
                cout << string(70, '-') << '\n';
            }
            cout << left << setw(6) << it.id << setw(25) << it.name << setw(15) << it.category
                 << setw(10) << it.quantity << setw(10) << fixed << setprecision(2) << it.price << '\n';
            found = true;
        }
    }
    if (!found)
        cout << "No matching products found.\n";
}

void updateProduct(vector<Item> &items) {
    cout << "\nEnter Product ID to update: ";
    int id;
    if (!(cin >> id)) {
        cout << "Invalid input.\n";
        cin.clear();
        cin.ignore();
        return;
    }
    for (auto &it : items) {
        if (it.id == id) {
            cout << "\nEditing product (leave blank to keep current)\n";
            cin.ignore();
            cout << "Current name: " << it.name << "\nNew name: ";
            string s;
            getline(cin, s);
            if (!s.empty())
                it.name = s;
            cout << "Current category: " << it.category << "\nNew category: ";
            getline(cin, s);
            if (!s.empty())
                it.category = s;
            cout << "Current quantity: " << it.quantity << "\nNew quantity (enter -1 to keep): ";
            int q;
            if (cin >> q) {
                if (q >= 0)
                    it.quantity = q;
            } else {
                cin.clear();
                cin.ignore();
            }
            cout << "Current price: " << it.price << "\nNew price (enter -1 to keep): ";
            double p;
            if (cin >> p) {
                if (p >= 0)
                    it.price = p;
            } else {
                cin.clear();
                cin.ignore();
            }
            if (saveRecords(items))
                cout << "Product updated.\n";
            else
                cout << "Error saving.\n";
            return;
        }
    }
    cout << "Product not found.\n";
}

void changeQuantity(vector<Item> &items, int delta, const string &action) {
    cout << "\nEnter Product ID to " << action << ": ";
    int id;
    if (!(cin >> id)) {
        cout << "Invalid input.\n";
        cin.clear();
        cin.ignore();
        return;
    }
    for (auto &it : items) {
        if (it.id == id) {
            if (delta < 0 && it.quantity + delta < 0) {
                cout << "Not enough stock. Current qty: " << it.quantity << '\n';
                return;
            }
            it.quantity += delta;
            if (saveRecords(items))
                cout << action << " successful. New qty: " << it.quantity << '\n';
            else
                cout << "Error saving.\n";
            return;
        }
    }
    cout << "Product not found.\n";
}

void deleteProduct(vector<Item> &items) {
    cout << "\nEnter Product ID to delete: ";
    int id;
    if (!(cin >> id)) {
        cout << "Invalid input.\n";
        cin.clear();
        cin.ignore();
        return;
    }
    auto it = remove_if(items.begin(), items.end(), [id](const Item &p)
                        { return p.id == id; });
    if (it == items.end()) {
        cout << "Product not found.\n";
        return;
    }
    items.erase(it, items.end());
    if (saveRecords(items))
        cout << "Product deleted.\n";
    else
        cout << "Error saving.\n";
}

int main() {
    vector<Item> items = loadRecords();
    int choice = 0;
    while (true) {
        cout << "\n====== Inventory Management System ======\n";
        cout << "1. Add Product\n2. Display All Products\n3. Search by ID\n4. Search by Name\n5. Update Product Info\n6. Sell Product (decrease qty)\n7. Restock Product (increase qty)\n8. Delete Product\n9. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: addItem(items); break;
            case 2: displayAll(items); pause(); break;
            case 3: searchByID(items); pause(); break;
            case 4: searchByName(items); pause(); break;
            case 5: updateProduct(items); pause(); break;
            case 6: changeQuantity(items, -1, "sell"); pause(); break;
            case 7: {
                cout << "Enter amount to restock: ";
                int amt;
                if (!(cin >> amt) || amt <= 0) {
                    cout << "Invalid amount.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cout << "Enter Product ID to restock: ";
                    int id;
                    if (!(cin >> id)) {
                        cout << "Invalid ID.\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    } else {
                        bool ok = false;
                        for (auto &it : items) {
                            if (it.id == id) {
                                it.quantity += amt;
                                ok = true;
                                break;
                            }
                        }
                        if (!ok)
                            cout << "Product not found.\n";
                        else if (saveRecords(items))
                            cout << "Restocked successfully.\n";
                        else
                            cout << "Error saving.\n";
                    }
                }
                pause();
                break;
            }
            case 8: deleteProduct(items); pause(); break;
            case 9: cout << "Exiting...\n"; return 0;
            default: cout << "Invalid choice.\n"; break;
        }
    }
}
