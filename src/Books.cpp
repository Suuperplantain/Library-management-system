#include "Library.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

void saveBooks(const vector<vector<string>> &books)
{
    ofstream booksOut("books.txt");
    if (!booksOut.is_open())
    {
        cerr << "Error: Could not open books file for writing!" << endl;
        return;
    }

    booksOut << "ID,Title,Author,Year,Copies\n";
    for (const auto &book : books)
    {
        booksOut << book[0] << ", \"" << book[1] << "\", \"" << book[2] << "\", "
                 << book[3] << ", " << book[4] << "\n";
    }
    booksOut.close();
}

void Library::displayBooks(bool returnToMenu)
{
    ifstream booksFile("books.txt");
    if (!booksFile.is_open())
    {
        cerr << "Error: Could not open books file!" << endl;
        return;
    }

    cout << "\n=========== Library Book Collection ===========\n\n";

    string line;

    getline(booksFile, line);
    if (line.find("ID") == string::npos)
    {
        booksFile.clear();
        booksFile.seekg(0);
    }

    int count = 0;
    while (getline(booksFile, line))
    {
        vector<string> parts;
        string current;
        bool inQuotes = false;

        for (char c : line)
        {
            if (c == '\"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes)
            {
                parts.push_back(cleanString(current));
                current.clear();
            }
            else
            {
                current += c;
            }
        }
        parts.push_back(cleanString(current));

        if (parts.size() >= 5)
        {
            count++;
            cout << "---------------------------------------------\n";
            cout << " Book #" << count << "\n";
            cout << "---------------------------------------------\n";
            cout << " ID:              " << parts[0] << "\n";
            cout << " Title:           " << parts[1] << "\n";
            cout << " Author:          " << parts[2] << "\n";
            cout << " Year Published:  " << parts[3] << "\n";
            cout << " Available Copies:" << parts[4] << "\n\n";
        }
        else
        {
            cerr << "Warning: Invalid book record format - " << line << endl;
        }
    }

    booksFile.close();

    if (count == 0)
    {
        cout << "No books found in the library.\n";
    }

    cout << "=============================================\n";

    if (current_role == ADMIN)
    {
        int choice;
        cout << "\n****** What would you like to do? ******\n";
        cout << "1. Edit a book\n";
        cout << "2. Remove a book\n";
        cout << "3. Add a new book\n";
        cout << "4. Return to main menu\n";
        cout << "Enter your choice (1-4): ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            editBook();
            break;
        case 2:
            removeBook();
            break;
        case 3:
            addBook();
            break;
        case 4:
            showUserMenu();
            break;
        default:
            cerr << "Invalid choice. Returning to menu." << endl;
            showUserMenu();
        }
    }
    else if (returnToMenu)
    {
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
        showUserMenu();
    }
}

void Library::searchBooks()
{
    string searchTitle;
    cout << "Enter book title to search: ";
    cin.ignore();
    getline(cin, searchTitle);

    ifstream booksFile("books.txt");
    if (!booksFile.is_open())
    {
        cerr << "Error: Could not open books file!" << endl;
        return;
    }

    cout << "\n=== Search Results ===\n";
    string line;
    bool found = false;

    while (getline(booksFile, line))
    {
        stringstream ss(line);
        string part;
        vector<string> parts;

        while (getline(ss, part, ','))
        {
            parts.push_back(cleanString(part));
        }

        if (parts.size() >= 5)
        {
            string title = parts[1];
            string titleLower = title;
            transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::tolower);

            string searchLower = searchTitle;
            transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

            if (titleLower.find(searchLower) != string::npos)
            {
                cout << "ID: " << parts[0] << endl;
                cout << "Title: " << title << endl;
                cout << "Author: " << parts[2] << endl;
                cout << "Year: " << parts[3] << endl;
                cout << "Available Copies: " << parts[4] << endl;
                cout << "--------------------------------" << endl;
                found = true;
            }
        }
    }

    booksFile.close();

    if (!found)
    {
        cout << "No matching books found." << endl;
    }

    cout << "Press Enter to continue...";
    cin.get();
    showUserMenu();
}

void Library::addBook()
{
    if (current_role != ADMIN)
    {
        cerr << "Error: You don't have permission to add books." << endl;
        return;
    }

    ifstream inFile("books.txt");
    string line;
    getline(inFile, line);
    int maxId = 0;

    while (getline(inFile, line))
    {
        stringstream ss(line);
        string idStr;
        getline(ss, idStr, ',');
        int id = stoi(cleanString(idStr));
        if (id > maxId)
            maxId = id;
    }
    inFile.close();

    int newId = maxId + 1;
    string title, author;
    int year, stock;

    cout << "Adding new book with ID " << newId << endl;
    cout << "Enter the title of the book: ";
    cin.ignore();
    getline(cin, title);
    cout << "Enter the author of the book: ";
    getline(cin, author);
    cout << "Enter the year of publication: ";
    cin >> year;
    cout << "Enter the number of copies available: ";
    cin >> stock;

    ofstream outFile("books.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << newId << ", \"" << title << "\", \"" << author << "\", " << year << ", " << stock << "\n";
        outFile.close();
        cout << "Book successfully added to the library!" << endl;
    }
    else
    {
        cerr << "Error: Could not open books file for writing!" << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
    showUserMenu();
}

void Library::editBook()
{
    if (current_role != ADMIN)
    {
        cerr << "Error: You don't have permission to edit books." << endl;
        return;
    }

    ifstream inFile("books.txt");
    if (!inFile.is_open())
    {
        cerr << "Error: Could not open books file!" << endl;
        return;
    }

    vector<vector<string>> books;
    string line;

    getline(inFile, line);

    while (getline(inFile, line))
    {
        vector<string> parts;
        string current;
        bool inQuotes = false;

        for (char c : line)
        {
            if (c == '\"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes)
            {
                parts.push_back(cleanString(current));
                current.clear();
            }
            else
            {
                current += c;
            }
        }
        parts.push_back(cleanString(current));

        if (parts.size() >= 5)
        {
            books.push_back(parts);
        }
    }
    inFile.close();

    cout << "\n=========== Library Book Collection ===========\n\n";
    for (size_t i = 0; i < books.size(); i++)
    {
        cout << "---------------------------------------------\n";
        cout << " Book #" << i + 1 << "\n";
        cout << "---------------------------------------------\n";
        cout << " ID:              " << books[i][0] << "\n";
        cout << " Title:           " << books[i][1] << "\n";
        cout << " Author:          " << books[i][2] << "\n";
        cout << " Year Published:  " << books[i][3] << "\n";
        cout << " Available Copies:" << books[i][4] << "\n\n";
    }
    cout << "=============================================\n";

    int bookId;
    cout << "Enter the ID of the book you want to edit: ";
    cin >> bookId;
    cin.ignore();

    bool found = false;
    for (auto &book : books)
    {
        if (stoi(book[0]) == bookId)
        {
            found = true;
            cout << "\nCurrent Book details:\n";
            cout << "ID: " << book[0] << endl;
            cout << "Title: " << book[1] << endl;
            cout << "Author: " << book[2] << endl;
            cout << "Year: " << book[3] << endl;
            cout << "Stock: " << book[4] << endl;

            int choice;
            cout << "\nWhich field would you like to edit?\n";
            cout << "1. Title\n";
            cout << "2. Author\n";
            cout << "3. Year\n";
            cout << "4. Stock\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice)
            {
            case 1:
                cout << "Enter the new title: ";
                getline(cin, book[1]);
                break;
            case 2:
                cout << "Enter the new author: ";
                getline(cin, book[2]);
                break;
            case 3:
                cout << "Enter the new year: ";
                cin >> book[3];
                break;
            case 4:
                cout << "Enter the new stock: ";
                cin >> book[4];
                break;
            default:
                cerr << "Invalid choice. No changes made." << endl;
                showUserMenu();
                return;
            }

            cout << "\nBook details successfully updated!\n";
            break;
        }
    }

    if (!found)
    {
        cerr << "Book with ID " << bookId << " not found.\n";
        showUserMenu();
        return;
    }

    ofstream outFile("books.txt");
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open books file for writing!" << endl;
        showUserMenu();
        return;
    }

    outFile << "ID,Title,Author,Year,Copies\n";
    for (const auto &book : books)
    {
        outFile << book[0] << ", \"" << book[1] << "\", \"" << book[2] << "\", "
                << book[3] << ", " << book[4] << "\n";
    }
    outFile.close();

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
    showUserMenu();
}

void Library::removeBook()
{
    if (current_role != ADMIN)
    {
        cerr << "Error: You don't have permission to remove books." << endl;
        return;
    }

    ifstream inFile("books.txt");
    if (!inFile.is_open())
    {
        cerr << "Error: Could not open books file!" << endl;
        return;
    }

    vector<vector<string>> books;
    string line;

    getline(inFile, line);

    while (getline(inFile, line))
    {
        vector<string> parts;
        string current;
        bool inQuotes = false;

        for (char c : line)
        {
            if (c == '\"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes)
            {
                parts.push_back(cleanString(current));
                current.clear();
            }
            else
            {
                current += c;
            }
        }
        parts.push_back(cleanString(current));

        if (parts.size() >= 5)
        {
            books.push_back(parts);
        }
    }
    inFile.close();

    cout << "\n=========== Library Book Collection ===========\n\n";
    for (size_t i = 0; i < books.size(); i++)
    {
        cout << "---------------------------------------------\n";
        cout << " Book #" << i + 1 << "\n";
        cout << "---------------------------------------------\n";
        cout << " ID:              " << books[i][0] << "\n";
        cout << " Title:           " << books[i][1] << "\n";
        cout << " Author:          " << books[i][2] << "\n";
        cout << " Year Published:  " << books[i][3] << "\n";
        cout << " Available Copies:" << books[i][4] << "\n\n";
    }
    cout << "=============================================\n";

    int bookId;
    cout << "Enter the ID of the book you want to remove: ";
    cin >> bookId;
    cin.ignore();

    bool found = false;
    for (auto it = books.begin(); it != books.end(); ++it)
    {
        if (stoi((*it)[0]) == bookId)
        {
            found = true;
            string bookTitle = (*it)[1];
            books.erase(it);
            cout << "\nBook \"" << bookTitle << "\" (ID: " << bookId << ") removed successfully!\n";
            break;
        }
    }

    if (!found)
    {
        cerr << "Book with ID " << bookId << " not found.\n";
        showUserMenu();
        return;
    }

    ofstream outFile("books.txt");
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open books file for writing!" << endl;
        showUserMenu();
        return;
    }

    outFile << "ID,Title,Author,Year,Copies\n";
    for (const auto &book : books)
    {
        outFile << book[0] << ", \"" << book[1] << "\", \"" << book[2] << "\", "
                << book[3] << ", " << book[4] << "\n";
    }
    outFile.close();

    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
    showUserMenu();
}
