/*
============================================================
    LOST & FOUND ITEMS MANAGEMENT SYSTEM
============================================================

Crafted By:
   • Bezawit Dereje
   • Etsegenet Amsalu
   • Fenet Asmamaw
   • Kedest Eyasu

Last Updated : January 5, 2026
© 2026 Lost & Found Project Team. All rights reserved.

Project Overview:
This console-based Lost & Found Management System allows users
to record, manage, search, update, and store information about
lost and found items. It supports reporting lost/found items,
viewing stored records, updating details, deleting entries,
marking items as claimed, and sorting/filtering by various criteria.

The system uses structured data, file storage, and menu-driven
interaction to ensure data persistence, accuracy, and ease of use.
Input validation reduces errors, and binary file handling ensures
efficient storage between program executions.

Purpose:
Developed as a collaborative project to demonstrate practical
application of C++ concepts including structures, arrays,
functions, file handling, and control flow.

============================================================
*/





#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <cctype>


using namespace std;






// Item structure
struct Item {
    int id;
    string name;
    string category;
    string description;
    char date[12];   // YYYY-MM-DD
    string location;
    string status; // "Lost" or "Found"
    int matched;     // 0 = No, 1 = Yes
    int claimed;     // 0 = No, 1 = Yes
    int matchedItemID;
    string personName;
    string personContact;
};

const string CATEGORIES[] = {
    "Electronics",
    "Clothing",
    "Documents",
    "Accessories",
    "Bags",
    "Keys",
    "Other"
};

const int CATEGORY_COUNT = 7;






// Helper Functions 

void pause() {
    cout << "\nPress Enter to continue...";
    while (cin.get() != '\n'); // keep reading until Enter is pressed
}

string toLowerCase(const string &s) {
    string result = s;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = static_cast<char>(tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
}

bool containsSubstring(const string &str, const string &substr) {
    string lowerStr = toLowerCase(str);
    string lowerSub = toLowerCase(substr);
    return lowerStr.find(lowerSub) != string::npos;
}

void selectCategory(string& category) {
    int choice;

    while (true) {
        cout << "\nSelect Category:\n";
        for (int i = 0; i < CATEGORY_COUNT; i++) {
            cout << "  " << i + 1 << ". " << CATEGORIES[i] << "\n";
        }
        cout << "Choose (1-" << CATEGORY_COUNT << "): ";

        if (cin >> choice && choice >= 1 && choice <= CATEGORY_COUNT) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            category = CATEGORIES[choice - 1];
            break;
        } else {
            cout << "Invalid choice. Please select a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

void getInput(string &input, const string &prompt, bool optional = false) {
    while (true) {
        cout << prompt;
        getline(cin, input);

        if (!optional && input.empty()) {
            cout << "Input cannot be empty!\n";
        } else {
            break;
        }
    }
}

void getValidDate(const char* prompt, char* date) {
    string temp;
    while (true) {
        getInput(temp, prompt); // get string input

        //  Check format YYYY-MM-DD 
        if (temp.length() != 10 || temp[4] != '-' || temp[7] != '-') {
            cout << "Invalid date format! Use YYYY-MM-DD.\n";
            continue;
        }


        // Check all year digits
        if (!isdigit(temp[0]) || !isdigit(temp[1]) || !isdigit(temp[2]) || !isdigit(temp[3]) ||
            !isdigit(temp[5]) || !isdigit(temp[6]) ||
            !isdigit(temp[8]) || !isdigit(temp[9])) {
            cout << "Date contains invalid characters! Must be digits.\n";
            continue;
        }


        // --- Extract year, month, day ---
        int year = (temp[0]-'0')*1000 + (temp[1]-'0')*100 + (temp[2]-'0')*10 + (temp[3]-'0');
        int month = (temp[5]-'0')*10 + (temp[6]-'0');
        int day = (temp[8]-'0')*10 + (temp[9]-'0');

        // --- Validate month ---
        if (month < 1 || month > 12) {
            cout << "Invalid month! Must be 01-12.\n";
            continue;
        }

        // --- Validate day ---
        int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        // Leap year check
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            daysInMonth[1] = 29;

        if (day < 1 || day > daysInMonth[month-1]) {
            cout << "Invalid day for the given month!\n";
            continue;
        }

        // --- If all checks passed, copy to char array ---
        strncpy(date, temp.c_str(), 11); // 10 chars + null terminator
        date[11] = '\0';
        break; // exit loop
    }
}

void getStatus(string &status) {
    while (true) {
        cout << "Enter status (Lost/Found): ";
        getline(cin, status);

        string temp = toLowerCase(status);
        if (temp == "lost" || temp == "found") {
            temp[0] = toupper(temp[0]); // capitalize first letter
            status = temp;
            break;
        } else {
            cout << "Invalid status! Only 'Lost' or 'Found' allowed.\n";
        }
    }
}









//Display & Retrieval Helpers

void displayItem(const Item& item) {
    cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "ID:        " << item.id << "\n";
    cout << "Name:      " << item.name << "\n";
    cout << "Category:  " << item.category << "\n";
    cout << "Description:\n" << item.description << "\n";
    cout << "Date:      " << item.date << "\n";
    cout << "Location:  " << item.location << "\n";
    cout << "Status:    " << item.status << "\n";
    cout << "Matched:   " << (item.matched ? "Yes" : "No") << "\n";
    cout << "Claimed:   " << (item.claimed ? "Yes" : "No") << "\n";

    if (item.matchedItemID != -1)
        cout << "Matched With ID: " << item.matchedItemID << "\n";

    cout << "Person:    " << item.personName << "\n";
    cout << "Contact:   " << item.personContact << "\n";
    cout << "------------------------------------------------------------------------------------------------------------------------------\n\n";
}

void displayResults(Item items[], int results[], int count) {
    if (count == 0) {
        cout << "No items found matching criteria.\n";
        return;
    }

cout << "\n===========================================================================================================================\n";
    cout << "\n========== SEARCH / FILTER RESULTS ==========\n";

    for (int i = 0; i < count; i++) {
        int idx = results[i]; // always use results array

        cout << "ID: " << items[idx].id << "\n";
        cout << "Name: " << items[idx].name << "\n";
        cout << "Category: " << items[idx].category << "\n";
        cout << "Description: " << items[idx].description << "\n";
        cout << "Date: " << items[idx].date << "\n";
        cout << "Location: " << items[idx].location << "\n";
        cout << "Status: " << items[idx].status << "\n";
        cout << "Matched: " << (items[idx].matched ? "Yes" : "No") << "\n";
        cout << "Claimed: " << (items[idx].claimed ? "Yes" : "No") << "\n";
        if (items[idx].matchedItemID != -1)
            cout << "Matched With ID: " << items[idx].matchedItemID << "\n";
        cout << "Person: " << items[idx].personName << " | Contact: " << items[idx].personContact << "\n";
        cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    }
}

Item* getItemByID(Item items[], int itemCount, int id) {
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id)
            return &items[i]; // return address of the matching item
    }
    return NULL; // return nullptr if no item with the given ID is found
}






// Dynamic Array & Memory Management
void resizeArray(Item*& items, int& capacity) {
    int newCapacity = capacity * 2;
    Item* newItems = new Item[newCapacity]();
    for (int i = 0; i < capacity; i++)
        newItems[i] = items[i];
    delete[] items;
    items = newItems;
    capacity = newCapacity;
}







// File Operations

void saveToFile(fstream& file, Item* items, int itemCount, int nextID, const char* filename) {
    file.open(filename, ios::out | ios::binary);
    if (!file) {
        cout << "File can't be opened.\n";
        return;
    }

    // header
    file.write(reinterpret_cast<char*>(&nextID), sizeof(nextID));
    file.write(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

    for (int i = 0; i < itemCount; i++) {

        // id
        file.write(reinterpret_cast<char*>(&items[i].id), sizeof(items[i].id));

        // name
        size_t len = items[i].name.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].name.c_str(), len);

        // category
        len = items[i].category.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].category.c_str(), len);

        // description
        len = items[i].description.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].description.c_str(), len);

        // date (fixed array)
        file.write(items[i].date, sizeof(items[i].date));

        // location
        len = items[i].location.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].location.c_str(), len);

        // status
        len = items[i].status.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].status.c_str(), len);

        // flags
        file.write(reinterpret_cast<char*>(&items[i].matched), sizeof(items[i].matched));
        file.write(reinterpret_cast<char*>(&items[i].claimed), sizeof(items[i].claimed));
        file.write(reinterpret_cast<char*>(&items[i].matchedItemID), sizeof(items[i].matchedItemID));

        // person name
        len = items[i].personName.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].personName.c_str(), len);

        // person contact
        len = items[i].personContact.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].personContact.c_str(), len);
    }

    file.close();
}

void loadFromFile(fstream& file, Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    file.open(filename, ios::in | ios::binary);
    if (!file) {
        itemCount = 0;
        nextID = 100;
        return;
    }

    // Read header safely
    if (!file.read(reinterpret_cast<char*>(&nextID), sizeof(nextID)) ||
        !file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount))) {
        itemCount = 0;
        nextID = 100;
        file.close();
        return;
    }

    if (itemCount > capacity) {
        while (capacity < itemCount)
            capacity *= 2;

        delete[] items;
        items = new Item[capacity];
    }

    for (int i = 0; i < itemCount; i++) {

        // id
        file.read(reinterpret_cast<char*>(&items[i].id), sizeof(items[i].id));

        size_t len;
        char* buffer;

        // name
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].name.assign(buffer, len);
        delete[] buffer;

        // category
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].category.assign(buffer, len);
        delete[] buffer;

        // description
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].description.assign(buffer, len);
        delete[] buffer;

        // date
        file.read(items[i].date, sizeof(items[i].date));

        // location
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].location.assign(buffer, len);
        delete[] buffer;

        // status
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].status.assign(buffer, len);
        delete[] buffer;

        // flags
        file.read(reinterpret_cast<char*>(&items[i].matched), sizeof(items[i].matched));
        file.read(reinterpret_cast<char*>(&items[i].claimed), sizeof(items[i].claimed));
        file.read(reinterpret_cast<char*>(&items[i].matchedItemID), sizeof(items[i].matchedItemID));

        // person name
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].personName.assign(buffer, len);
        delete[] buffer;

        // person contact
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].personContact.assign(buffer, len);
        delete[] buffer;
    }

    file.close();
}


void viewFromFile(const char* filename, fstream& file) {
    int itemCount = 0;
    int capacity = 10;  // initial capacity
    int nextID = 100;

    Item* items = new Item[capacity];

    loadFromFile(file, items, itemCount, capacity, nextID, filename);

    if (itemCount == 0) {
        cout << "\nNo items to display.\n";
        delete[] items;
        return;
    }

cout << "\n===========================================================================================================================\n";
cout << "\n====================    ITEMS ON RECORD    ================================================================================\n";

    for (int i = 0; i < itemCount; i++) {
        cout << "Item " << i + 1 << ":\n";
        displayItem(items[i]);
        
    }

    delete[] items;
}

void clearAllItems(Item items[], int& itemCount, int& nextID, const char* filename) {
    string confirm;

    while (true) {
        cout << "Are you sure you want to delete ALL items? (Y/N): ";
        getline(cin, confirm);

        if (confirm == "Y" || confirm == "y") {
            ofstream out(filename, ios::binary | ios::trunc);
            if (!out) {
                cout << "Error clearing file!\n";
                return;
            }
            out.close();

            itemCount = 0;
            nextID = 100;
            cout << "All items cleared successfully.\n";
            return;
        }
        else if (confirm == "N" || confirm == "n") {
            cout << "Operation cancelled. No items were deleted.\n";
            return;
        }
        else {
            cout << "Invalid input. Please enter only Y or N.\n";
        }
    }
}











//Search & Filter Functions

int searchByName(Item items[], int itemCount, const string& name, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].name, name)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByCategory(Item items[], int itemCount, const string& category, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].category, category)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByDescription(Item items[], int itemCount, const string& description, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].description, description)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByLocation(Item items[], int itemCount, const string& location, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].location, location)) {
            results[count++] = i;
        }
    }
    return count;
}


int searchByDate(Item items[], int itemCount, const char* date, int results[]) {
    int count = 0;

    for (int i = 0; i < itemCount; i++) {
        if (strcmp(items[i].date, date) == 0) {
            results[count++] = i;
        }
    }

    return count; // number of matches
}

int searchByStatus(Item items[], int itemCount, const string& status, int results[]) {
    int count = 0;
    string lowerStatus = toLowerCase(status);

    for (int i = 0; i < itemCount; i++) {
        string itemStatus = toLowerCase(items[i].status);

        // Check if status matches AND item is unmatched
        if (itemStatus == lowerStatus && items[i].matched == 0) {
            results[count++] = i;
        }
    }

    return count;
}

int filterByMatched(Item items[], int itemCount, int matchedValue, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].matched == matchedValue) {
            results[count++] = i;
        }
    }
    return count;
}

int filterByClaimed(Item items[], int itemCount, int claimedValue, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].claimed == claimedValue) {
            results[count++] = i;
        }
    }
    return count;
}

void filterSearchMenu(Item items[], int itemCount) {
    int choice;
    string input;

    int* results = new int[itemCount]; // dynamic array for search results

    do {
        cout << "\n--- Filter / Search Items ---\n";
        cout << "1. By Name\n2. By Category\n3. By Description\n4. By Location\n";
        cout << "5. By Status\n6. By Matched / Unmatched\n7. By Claimed / Unclaimed\n";
        cout << "8. By Date\n"; 
        cout << "9. Back to Main Menu\n"; 
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int count = 0;

        switch (choice) {
            case 1:
                getInput(input, "Enter name: ");
                count = searchByName(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 2:
                getInput(input, "Enter category: ");
                count = searchByCategory(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 3:
                getInput(input, "Enter description: ");
                count = searchByDescription(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 4:
                getInput(input, "Enter location: ");
                count = searchByLocation(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 5:
                getStatus(input);
                count = searchByStatus(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 6: {
                int m;
                while (true) {
                    cout << "1. Matched\n2. Unmatched\nSelect: ";
                    if (!(cin >> m)) {
                        cout << "Invalid input! Please enter 1 or 2.\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (m == 1 || m == 2) break; // valid input
                    cout << "Invalid choice! Please enter 1 or 2.\n";
                }

                count = filterByMatched(items, itemCount, m == 1 ? 1 : 0, results);
                displayResults(items, results, count);
                break;
            }


            case 7: {
                int c;
                while (true) {
                    cout << "1. Claimed\n2. Unclaimed\nSelect: ";
                    if (!(cin >> c)) {
                        cout << "Invalid input! Please enter 1 or 2.\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (c == 1 || c == 2) break; // valid input
                    cout << "Invalid choice! Please enter 1 or 2.\n";
                }

                count = filterByClaimed(items, itemCount, c == 1 ? 1 : 0, results);
                displayResults(items, results, count);
                break;
            }


            case 8: { // Search by Date
                char date[12]; 
                getValidDate("Enter date (YYYY-MM-DD): ", date);

                count = searchByDate(items, itemCount, date, results);
                displayResults(items, results, count);
                break;
            }


            case 9: // Back to Main Menu
                delete[] results;
                return;

            default:
                cout << "Invalid choice! Please select 1-9.\n";
        }

    } while (true);
}






//Matching System

int* findPotentialMatches(Item items[], int itemCount, const Item& newItem, int& matchCount) {
    int* matchIndices = new int[itemCount];

    for (int i = 0; i < itemCount; i++) {
        // Skip already matched items or same status
        if (items[i].matched == 1 || items[i].status == newItem.status)
            continue;

        // Use case-insensitive matching for strings
        bool match =
            containsSubstring(items[i].name, newItem.name) ||
            containsSubstring(newItem.name, items[i].name) ||
            containsSubstring(items[i].category, newItem.category) ||
            containsSubstring(newItem.category, items[i].category) ||
            containsSubstring(items[i].description, newItem.description) ||
            containsSubstring(newItem.description, items[i].description) ||
            containsSubstring(items[i].location, newItem.location) ||
            containsSubstring(newItem.location, items[i].location);

        if (match) {
            matchIndices[matchCount++] = i;
        }
    }

    if (matchCount == 0) {
        delete[] matchIndices;
        return NULL;
    }

    return matchIndices;
}

void markAsMatched(Item& item1, Item& item2) {
    item1.matched = 1;
    item2.matched = 1;
    item1.matchedItemID = item2.id;
    item2.matchedItemID = item1.id;

    cout << "Items matched successfully!\n";
    cout << "Item " << item1.id << " matched with Item " << item2.id << "\n";
     cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    
}

bool markMatchByID(Item items[], int itemCount, Item& newItem, int matchID) {

    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == matchID) {
            markAsMatched(newItem, items[i]);
            return true;
        }
    }
    return false; // ID not found
}

void displayMatches(Item items[], int matchIndices[], int matchCount) {
    if (matchCount == 0 || matchIndices == NULL) {
        cout << "No potential matches found.\n";
        cout << "------------------------------------------------------------------------------------------------------------------------------\n";
        return;
    }

    for (int i = 0; i < matchCount; i++) {
        cout << "------------------------------------------------------------------------------------------------------------------------------\n";
        cout << "\n--- Potential Match " << i + 1 << " ---\n";
        displayItem(items[matchIndices[i]]);
        cout<<endl;
        cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    }
}

void searchForMatches(Item*& items, int itemCount, Item& newItem, int& nextID, const char* filename, fstream& file) {
    //  Ask user if they want to search for matches 
    char searchChoice;
    while (true) {
        string input;
        cout << "Do you want to search for matching items now? (Y/N): ";
        getline(cin, input); // read whole line

        // Validate that input is exactly 1 character
        if (input.length() == 1) {
            searchChoice = tolower(input[0]); // convert to lowercase
            if (searchChoice == 'y' || searchChoice == 'n') {
                break; // valid input
            }
        }

        cout << "Invalid input. Please enter only Y or N.\n";
    }

    if (searchChoice == 'y') {
        // Find potential matches 
        int matchCount = 0;
        int* matchIndices = findPotentialMatches(items, itemCount - 1, newItem, matchCount);

        // Display matches
        displayMatches(items, matchIndices, matchCount);

        if (matchCount > 0) {
            int choice;

            // Loop to allow match ID entry until 0 or a successful match 
            while (true) {
                cout << "Enter the ID of the item to mark as matched (0 to stop): ";
                if (!(cin >> choice)) { // input validation
                    cout << "Invalid input. Enter a number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear rest of line

                if (choice == 0) {
                    break; // user wants to stop entering match IDs
                }

                // Check if the entered ID exists in the match list
                bool valid = false;
                for (int i = 0; i < matchCount; i++) {
                    if (items[matchIndices[i]].id == choice) {
                        valid = true;
                        break;
                    }
                }

                if (valid) {
                    if (markMatchByID(items, itemCount, newItem, choice)) {
                        saveToFile(file, items, itemCount, nextID, filename);
                        break; // stop asking after a successful match
                    } else {
                        cout << "Failed to mark item ID " << choice << ".\n";
                    }
                } else {
                    cout << "Invalid match ID. Please enter a valid ID from the list.\n";
                }
            }
        } 

        delete[] matchIndices; // free memory
    }
}





//Add Item Operations
void addLostItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename,fstream &file) {
    if (itemCount == capacity)
        resizeArray(items, capacity);

    Item newItem;
    newItem.id = nextID++;

    getInput(newItem.name, "Enter Item Name: ");
    selectCategory(newItem.category);
    getInput(newItem.description, "Enter Description: ");
    getValidDate("Enter Date Found (YYYY-MM-DD): ", newItem.date);
    getInput(newItem.location, "Enter Location Found: ");
    getInput(newItem.personName, "Enter owner Name : ", true);
    getInput(newItem.personContact, "Enter owner Contact : ", true);


    newItem.status = "Lost";
    newItem.matched = 0;
    newItem.claimed = 0;
    newItem.matchedItemID = -1;

    items[itemCount++] = newItem;
    saveToFile(file, items, itemCount, nextID, filename);

    cout << "\nLost item added! ID: " << newItem.id << "\n";

   cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    // Call the new match search function
    searchForMatches(items, itemCount, items[itemCount - 1], nextID, filename,file);
    //items[itemCount - 1] - last item added
    pause();
}

void addFoundItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename,fstream &file) {
    if (itemCount == capacity)
        resizeArray(items, capacity);

    Item newItem;
    newItem.id = nextID++;

    getInput(newItem.name, "Enter Item Name: ");
    selectCategory(newItem.category);
    getInput(newItem.description, "Enter Description: ");
    getValidDate("Enter Date Found (YYYY-MM-DD): ", newItem.date);
    getInput(newItem.location, "Enter Location Found: ");
    getInput(newItem.personName, "Enter Finder Name (Optional): ", true);
    getInput(newItem.personContact, "Enter Finder Contact (Optional): ", true);


    newItem.status = "Found";
    newItem.matched = 0;
    newItem.claimed = 0;
    newItem.matchedItemID = -1;

    items[itemCount++] = newItem;

    saveToFile(file, items, itemCount, nextID, filename);

    cout << "\nFound item added! ID: " << newItem.id << "\n";
    cout << "------------------------------------------------------------------------------------------------------------------------------\n";

    // Reuse the search function
    searchForMatches(items, itemCount, items[itemCount - 1], nextID, filename,file);

    pause();
}







//Update Functions

void updateItemMenu(Item* item) {
    int choice;

    do {
        
        cout << "------------------------------------------------------------------------------------------------------------------------------\n";
		cout << "\n--- Update Menu for Item ID: " << item->id << " ---\n\n";
		
		cout << "Enter the field you want to update:\n\n";
		cout << "1. Name\n"; 
		cout << "2. Category\n"; 
		cout << "3. Description\n"; 
		cout << "4. Date\n"; 
		cout << "5. Location\n"; 
		cout << "6. Person Name\n"; 
		cout << "7. Person Contact\n"; 
		cout << "8. All Fields\n"; 
		cout << "9. Return to Main Menu\n";
		cout << "Select an option: ";


        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                getInput(item->name, "New Name: ");
                cout << "Name updated successfully!\n";
                break;
            case 2:
                selectCategory(item->category);
                cout << "Category updated successfully!\n";
                break;
            case 3:
                getInput(item->description, "New Description: ");
                cout << "Description updated successfully!\n";
                break;
            case 4:
                getValidDate("New Date (YYYY-MM-DD): ", item->date);
                cout << "Date updated successfully!\n";
                break;
            case 5:
                getInput(item->location, "New Location: ");
                cout << "Location updated successfully!\n";
                break;
            case 6:
                getInput(item->personName, "New Person Name: ", true);
                cout << "Person Name updated successfully!\n";
                break;
            case 7:
                getInput(item->personContact, "New Person Contact: ", true);
                cout << "Person Contact updated successfully!\n";
                break;
            case 8: // Update all fields
                getInput(item->name, "New Name: ");
                selectCategory(item->category);
                getInput(item->description, "New Description: ");
                getValidDate("New Date (YYYY-MM-DD): ", item->date);
                getInput(item->location, "New Location: ");
                getInput(item->personName, "New Person Name : ", true);
                getInput(item->personContact, "New Person Contact : ", true);
                cout << "All fields updated successfully!\n";
                break;
            case 9:
                return; // exit menu
            default:
                cout << "Invalid option. Please select 1-9.\n";
        }

    } while (true);
}

void updateItem(Item items[], int itemCount, const char* filename, int nextID,fstream &file) {
    if (itemCount == 0) {
        cout << "No items available to update.\n";
        return;
    }

    int id;
    cout << "Enter the ID of the item to update: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Item* item = NULL;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id) {
            item = &items[i];
            break;
        }
    }

    if (!item) {
        cout << "Item with ID " << id << " not found.\n";
        return;
    }

    cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    cout<< "Item details "<<endl;

    displayItem(*item); // Show current details
    updateItemMenu(item); // Let user update fields
    saveToFile(file, items, itemCount, nextID, filename);
}










//Delete Function

void deleteItem(Item*& items, int& itemCount, int& nextID, const char* filename, fstream& file) {
    if (itemCount == 0) {
        cout << "No items available to delete.\n";
        return;
    }

    // Get valid ID
    int id;
    while (true) {
        cout << "Enter the ID of the item to delete: ";
        if (cin >> id) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear rest of line
            break;
        } else {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear(); // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
        }
    }

    // Find the item
    int index = -1;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        cout << "Item with ID " << id << " not found.\n";
        return;
    }

    // Confirm deletion with single-character input
    char confirm;
    string input;
    while (true) {
        cout << "Are you sure you want to delete this item? (Y/N): ";
        getline(cin, input);

        if (input.length() == 1 &&
            (input[0] == 'Y' || input[0] == 'y' || input[0] == 'N' || input[0] == 'n')) {
            confirm = input[0];
            break; // Valid input
        } else {
            cout << "Invalid input! Please enter only 'Y' or 'N'.\n";
        }
    }

    if (confirm == 'N' || confirm == 'n') {
        cout << "Deletion cancelled.\n";
        return;
    }

    // Shift items to fill the gap
    for (int i = index; i < itemCount - 1; i++) {
        items[i] = items[i + 1];
    }
    itemCount--;

    // Save updated array to file
    saveToFile(file, items, itemCount, nextID, filename);

    cout << "Item deleted successfully!\n";
}







// Marking Functions
void markItemAsMatched(Item items[], int itemCount, int nextID, const char* filename, fstream& file) {
    if (itemCount < 2) {
        cout << "Not enough items to mark as matched.\n";
        return;
    }

    int id1, id2;

    // Get first item ID
    while (true) {
        cout << "Enter the ID of the first item to match: ";
        if (cin >> id1) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        } else {
            cout << "Invalid input! Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    // Get second item ID
    while (true) {
        cout << "Enter the ID of the second item to match: ";
        if (cin >> id2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        } else {
            cout << "Invalid input! Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    // Ensure IDs are different
    if (id1 == id2) {
        cout << "Cannot match an item with itself.\n";
        return;
    }

    // Find both items
    Item* item1 = NULL;
    Item* item2 = NULL;

    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id1) item1 = &items[i];
        if (items[i].id == id2) item2 = &items[i];
    }

    if (!item1 || !item2) {
        cout << "One or both item IDs not found.\n";
        return;
    }

    // Check that one is Lost and the other is Found
    if ((item1->status == "Lost" && item2->status == "Lost") ||
        (item1->status == "Found" && item2->status == "Found")) {
        cout << "Invalid match! You can only match a Lost item with a Found item.\n";
        return;
    }

    // Check if either item is already matched
    if (item1->matched || item2->matched) {
        cout << "One or both items are already matched.\n";
        return;
    }

    // Mark them as matched
    markAsMatched(*item1, *item2);

    // Save changes to file
    saveToFile(file, items, itemCount, nextID, filename);
}

void markAsClaimed(Item items[], int itemCount, const char* filename, int nextID, fstream& file) {
    int id;

    // Input validation loop
    while (true) {
        cout << "Enter the ID of the item to mark as claimed: ";
        if (cin >> id) {  // Check if input is an integer
            if (id <= 0) {
                cout << "ID must be a positive number. Try again.\n";
                continue;
            }
            break; // valid input
        } else {
            cout << "Invalid input. Please enter a numeric ID.\n";
            cin.clear(); // clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // remove leftover newline

    // Find the item
    Item* item = getItemByID(items, itemCount, id);

    if (!item) {
        cout << "Item not found.\n";
        return;
    }

    if (item->matched == 0) {
        cout << "Item cannot be claimed because it is not matched yet.\n";
        return;
    }

    if (item->claimed == 1) {
        cout << "Item is already claimed.\n";
        return;
    }

    // Mark the item as claimed
    item->claimed = 1;

    // Mark the matched item as claimed too
    if (item->matchedItemID != -1) {
        Item* matchedItem = getItemByID(items, itemCount, item->matchedItemID);
        if (matchedItem) matchedItem->claimed = 1;
    }

    saveToFile(file, items, itemCount, nextID, filename);
    cout << "Item marked as claimed successfully.\n";
}












// Sorting System
void swapItems(Item& a, Item& b) {
    Item temp = a;
    a = b;
    b = temp;
}

void sortByID(Item items[], int itemCount, bool ascending) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if ((ascending && items[j].id > items[j + 1].id) ||
                (!ascending && items[j].id < items[j + 1].id)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortByName(Item items[], int itemCount, bool ascending) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if ((ascending && items[j].name > items[j + 1].name) ||
                (!ascending && items[j].name < items[j + 1].name)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortByCategory(Item items[], int itemCount, bool ascending = true) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if ((ascending && items[j].category > items[j + 1].category) ||
                (!ascending && items[j].category < items[j + 1].category)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortByDate(Item items[], int itemCount, bool ascending) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if ((ascending && strcmp(items[j].date, items[j + 1].date) > 0) ||
                (!ascending && strcmp(items[j].date, items[j + 1].date) < 0)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortByStatus(Item items[], int itemCount, bool lostFirst) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            // Assuming "Lost" > "Found" in alphabetical order
            if ((lostFirst && items[j].status < items[j + 1].status) ||
                (!lostFirst && items[j].status > items[j + 1].status)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortMenu(Item items[], int itemCount, const char* filename, int nextID, fstream& file) {
    int choice;
    int order;

    while (true) {
        cout << "------------------------------------------------------------------------------------------------------------------------------\n";
        cout << "\n--- Sort Items ---\n";
        cout << "1. By ID\n";
        cout << "2. By Name\n";
        cout << "3. By Category\n";       // Added Category
        cout << "4. By Date\n";
        cout << "5. By Status\n";
        cout << "6. Back to Main Menu\n";

        // Get valid menu choice
        while (true) {
            cout << "Select an option: ";
            if (cin >> choice && choice >= 1 && choice <= 6) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            } else {
                cout << "Invalid input. Enter a number between 1 and 6.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        if (choice == 6) return; // Exit menu

        // Determine sorting order
        if (choice == 1 || choice == 2 || choice == 3) { // ID, Name, Category
            while (true) {
                cout << "1. Ascending\n2. Descending\nSelect order: ";
                if (cin >> order && (order == 1 || order == 2)) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                } else {
                    cout << "Invalid input. Enter 1 or 2.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        } else if (choice == 4) { // Date
            while (true) {
                cout << "1. Recent First\n2. Older First\nSelect order: ";
                if (cin >> order && (order == 1 || order == 2)) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                } else {
                    cout << "Invalid input. Enter 1 or 2.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        } else if (choice == 5) { // Status
            while (true) {
                cout << "1. Lost First\n2. Found First\nSelect order: ";
                if (cin >> order && (order == 1 || order == 2)) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                } else {
                    cout << "Invalid input. Enter 1 or 2.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }

        bool ascendingOrLostFirst;
        if (choice == 4) { // Date
            ascendingOrLostFirst = (order == 2); // Recent first = descending
        } else {
            ascendingOrLostFirst = (order == 1); // ID, Name, Category, Status
        }

        // Perform sorting
        switch (choice) {
            case 1: sortByID(items, itemCount, ascendingOrLostFirst); break;
            case 2: sortByName(items, itemCount, ascendingOrLostFirst); break;
            case 3: sortByCategory(items, itemCount, ascendingOrLostFirst); break; // New
            case 4: sortByDate(items, itemCount, ascendingOrLostFirst); break;
            case 5: sortByStatus(items, itemCount, ascendingOrLostFirst); break;
        }

        saveToFile(file, items, itemCount, nextID, filename);
        cout << "Items sorted successfully!\n";
    }
}












// Help and Welcome message Functions
void showHelp() {
    cout << "\n===========================================================================================================================\n";
    cout << "        LOST & FOUND MANAGER - HELP     \n";
    cout << "\n===========================================================================================================================\n\n";

    cout << "ABOUT THE APPLICATION\n";
    cout << "----------------------\n";
    cout << "This application helps manage lost and found items.\n";
    cout << "Users can report lost or found items, search for matches,\n";
    cout << "update item details, and track claimed items.\n\n";

    cout << "DATA STORAGE\n";
    cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "- All items are stored in a binary file (items.bin).\n";
    cout << "- Data is automatically saved after every change.\n";
    cout << "- Items persist even after closing the program.\n\n";

    cout << "MAIN MENU OPTIONS\n";
    cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "1. Report Lost Item\n";
    cout << "   - Add a new item that has been lost.\n";
    cout << "   - The system will try to find matching found items.\n\n";

    cout << "2. Report Found Item\n";
    cout << "   - Add an item that has been found.\n";
    cout << "   - The system will try to match it with lost items.\n\n";

    cout << "3. View Items\n";
    cout << "   - Display all stored items with full details.\n\n";

    cout << "4. Update Item\n";
    cout << "   - Modify item information using the item ID.\n\n";

    cout << "5. Filter / Search Items\n";
    cout << "   - Search items by name, category, description, location,\n";
    cout << "     status (Lost/Found), matched, or claimed.\n\n";

    cout << "6. Delete Item\n";
    cout << "   - Permanently remove an item using its ID.\n\n";

    cout << "7. Mark Item as Claimed\n";
    cout << "   - Mark a matched item as claimed by its owner.\n\n";

    cout << "8. Sort Items\n";
    cout << "   - Sort items by ID, name, date, or status.\n\n";

    cout << "9. Clear All Items\n";
    cout << "   - Delete all items from the system (requires confirmation).\n\n";

    cout << "10. Exit\n";
    cout << "    - Safely exit the application.\n\n";

    cout << "MATCHING SYSTEM\n";
   cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "- The app automatically suggests matches based on:\n";
    cout << "  name, category, description, and location.\n";
    cout << "- Matches can be confirmed manually.\n\n";

    cout << "IMPORTANT NOTES\n";
   cout << "------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "- Each item has a unique ID.\n";
    cout << "- Always use the ID when updating or deleting items.\n";
    cout << "- Claimed items cannot be claimed again.\n\n";

    cout << "\n===========================================================================================================================\n";
    pause();
}

void displayWelcomeMessage() {
	
    cout << "\n===========================================================================================================================\n";
    cout << "*           WELCOME TO THE LOST & FOUND ITEMS MANAGER      *\n";
    cout << "\n===========================================================================================================================\n";

    cout << "Track lost items, manage found items,\n";
    cout << "and help reunite what is lost with its owner!\n\n";

    cout << "Every item you log can make someone's day! \n";
    cout << "Let's get started!\n\n";
}








//Main Menu Controller
void mainMenu(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename, fstream& file) {
    int choice;

    do {
        cout << "\n===========================================================================================================================\n";
        cout << "         LOST & FOUND ITEMS MANAGER       \n";
        cout << "\n===========================================================================================================================\n";
        cout << "  1. How to Use\n";
        cout << "  2. Report Lost Item\n";
        cout << "  3. Report Found Item\n";
        cout << "  4. View Items\n";
        cout << "  5. Update Item\n";
        cout << "  6. Filter / Search Items\n";
        cout << "  7. Delete Item\n";
        cout << "  8. Mark Item as Claimed\n";
        cout << "  9. Mark Items as Matched\n";
        cout << " 10. Sort Items\n";
        cout << " 11. Clear All Items\n";
        cout << " 12. Exit\n";

        cout << "------------------------------------------------------------------------------------------------------------------------------\n";
        cout << "Select an option (1-12): ";

        cin >> choice;

        // Validate input
        if (cin.fail() || choice < 1 || choice > 12)
 {
            cout << "Invalid input! Please enter a number between 1 and 11.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: showHelp(); break;
            case 2: addLostItem(items, itemCount, capacity, nextID, filename, file); break;
            case 3: addFoundItem(items, itemCount, capacity, nextID, filename, file); break;
            case 4: viewFromFile(filename, file); break;
            case 5: updateItem(items, itemCount, filename, nextID, file); break;
            case 6: filterSearchMenu(items, itemCount); break;
            case 7: deleteItem(items, itemCount, nextID, filename, file); break;
            case 8: markAsClaimed(items, itemCount, filename, nextID, file); break;
            case 9: markItemAsMatched(items, itemCount, nextID, filename, file); break;
            case 10: sortMenu(items, itemCount, filename, nextID, file); break;
            case 11: clearAllItems(items, itemCount, nextID, filename); break;
            case 12: cout << "Exiting...\n"; break;
        }


  } while (choice != 12);
}






// Main Function
int main() {
    fstream file;
    const char* filename = "items.bin";

    int itemCount = 0, nextID = 100, capacity = 10;
    Item* items = new Item[capacity];

   
    loadFromFile(file, items, itemCount, capacity, nextID, filename);
    
    displayWelcomeMessage();
    pause();
//    cout << "Items loaded: " << itemCount << ", Next ID: " << nextID << "\n";

    mainMenu(items, itemCount, capacity, nextID, filename, file);

    delete[] items;
    return 0;
}


