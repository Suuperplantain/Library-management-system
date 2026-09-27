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

void Library::showMainMenu()
{
    if (!checkFileExists("books.txt") || !checkFileExists("People.txt") || !checkFileExists("users.txt"))
    {
        createDefaultFiles();
    }

    while (true)
    {
        int choice;
        cout << "\n========== Library Management System ==========\n";
        cout << "1. Login\n";
        cout << "2. Sign Up\n";
        cout << "3. Exit\n";
        cout << "Enter your choice (1-3): ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            login();
            break;
        case 2:
            signup();
            break;
        case 3:
            cout << "Thank you for using the Library Management System. Goodbye!\n";
            exit(0);
        default:
            cerr << "Invalid choice. Please try again.\n";
        }
    }
}

void Library::showUserMenu()
{
    while (is_logged_in)
    {
        int choice;
        cout << "\n========== Library Management System ==========\n";
        cout << "Logged in as: " << current_username;

        if (current_role == ADMIN)
        {
            cout << " (Admin)\n";
            cout << "1. View All Books\n";
            cout << "2. Search for Books\n";
            cout << "3. Add a Book\n";
            cout << "4. Edit a Book\n";
            cout << "5. Remove a Book\n";
            cout << "6. View Users with Late Fees\n";
            cout << "7. View My Borrowed Books\n";
            cout << "8. Borrow a Book\n";
            cout << "9. Return a Book\n";
            cout << "10. Logout\n";
            cout << "11. Exit\n";
            cout << "Enter your choice (1-11): ";
        }
        else
        {
            cout << (current_role == FACULTY ? " (Faculty)\n" : " (Student)\n");
            cout << "1. View All Books\n";
            cout << "2. Search for Books\n";
            cout << "3. View My Borrowed Books\n";
            cout << "4. Borrow a Book\n";
            cout << "5. Return a Book\n";
            cout << "6. Logout\n";
            cout << "7. Exit\n";
            cout << "Enter your choice (1-7): ";
        }

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (current_role == ADMIN)
        {
            switch (choice)
            {
            case 1:
                displayBooks(false);
                break;
            case 2:
                searchBooks();
                break;
            case 3:
                addBook();
                break;
            case 4:
                editBook();
                break;
            case 5:
                removeBook();
                break;
            case 6:
                checkLateFees();
                break;
            case 7:
                viewBorrowedBooks();
                break;
            case 8:
                borrowBook();
                break;
            case 9:
                returnBook();
                break;
            case 10:
                logout();
                return;
            case 11:
                cout << "Goodbye!\n";
                exit(0);
            default:
                cerr << "Invalid choice. Try again.\n";
            }
        }
        else
        {
            switch (choice)
            {
            case 1:
                displayBooks(false);
                break;
            case 2:
                searchBooks();
                break;
            case 3:
                viewBorrowedBooks();
                break;
            case 4:
                borrowBook();
                break;
            case 5:
                returnBook();
                break;
            case 6:
                logout();
                return;
            case 7:
                cout << "Goodbye!\n";
                exit(0);
            default:
                cerr << "Invalid choice. Try again.\n";
            }
        }
    }

    showMainMenu();
}
