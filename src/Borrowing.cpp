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

void Library::borrowBook()
{
    if (!is_logged_in)
    {
        cerr << "Error: Login required\n";
        return;
    }

    displayBooks(false);

    int bookId;
    const int maxTries = 3;
    int tries = 0;

    do
    {
        cout << "Enter book ID to borrow: ";
        if (!(cin >> bookId) || bookId <= 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid ID. Enter a positive number";
            if (++tries < maxTries)
            {
                cerr << " (" << maxTries - tries << " tries remaining): ";
            }
            else
            {
                cerr << ". Returning to menu.\n";
                showUserMenu();
                return;
            }
        }
        else
        {
            break;
        }
    } while (tries < maxTries);

    ifstream booksFile("books.txt");
    if (!booksFile.is_open())
    {
        cerr << "Error: Could not open books file! Please try again later.\n";
        showUserMenu();
        return;
    }

    vector<vector<string>> books;
    string line;
    bool bookFound = false;
    string bookTitle;
    int availableCopies = 0;

    getline(booksFile, line);

    while (getline(booksFile, line))
    {
        vector<string> bookData;
        stringstream ss(line);
        string part;
        bool inQuotes = false;
        string current;

        // Split on commas outside quoted fields so titles such as "A, B" stay intact.
        for (char c : line)
        {
            if (c == '\"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes)
            {
                bookData.push_back(cleanString(current));
                current.clear();
            }
            else
            {
                current += c;
            }
        }
        bookData.push_back(cleanString(current));

        if (bookData.size() >= 5)
        {
            try
            {
                books.push_back(bookData);
                if (stoi(bookData[0]) == bookId)
                {
                    bookFound = true;
                    bookTitle = bookData[1];
                    availableCopies = stoi(bookData[4]);
                    if (availableCopies <= 0)
                    {
                        cerr << "No copies available of this book.\n";
                        booksFile.close();
                        showUserMenu();
                        return;
                    }
                }
            }
            catch (const invalid_argument &)
            {
                cerr << "Warning: Invalid book entry skipped.\n";
                continue;
            }
        }
    }
    booksFile.close();

    if (!bookFound)
    {
        cerr << "Error: Book with ID " << bookId << " not found in the library.\n";
        showUserMenu();
        return;
    }

    vector<vector<string>> people;
    bool userFound = false;
    ifstream peopleIn("People.txt");
    if (!peopleIn.is_open())
    {
        cerr << "Error: Could not open user records file!\n";
        showUserMenu();
        return;
    }

    getline(peopleIn, line);

    while (getline(peopleIn, line))
    {
        vector<string> person;
        stringstream ss(line);
        string part;
        bool inQuotes = false;
        string current;

        for (char c : line)
        {
            if (c == '\"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes)
            {
                person.push_back(cleanString(current));
                current.clear();
            }
            else
            {
                current += c;
            }
        }
        person.push_back(cleanString(current));

        if (person.size() >= 7)
        {
            if (stoi(person[0]) == current_user_id)
            {
                userFound = true;
                if (person[3] == "None")
                {
                    person[3] = bookTitle + " (" + to_string(bookId) + ")";
                }
                else
                {
                    if (person[3].find("(" + to_string(bookId) + ")") != string::npos)
                    {
                        cerr << "Error: You have already borrowed this book.\n";
                        peopleIn.close();
                        showUserMenu();
                        return;
                    }
                    person[3] += ", " + bookTitle + " (" + to_string(bookId) + ")";
                }

                time_t now = time(0);
                if (now == -1)
                {
                    cerr << "Error getting current time.\n";
                    peopleIn.close();
                    showUserMenu();
                    return;
                }

                tm due;
#ifdef _WIN32
                if (localtime_s(&due, &now) != 0)
                {
                    cerr << "Error converting time.\n";
                    peopleIn.close();
                    showUserMenu();
                    return;
                }
#else
                due = *localtime(&now);
                if (!localtime(&now))
                {
                    cerr << "Error converting time.\n";
                    peopleIn.close();
                    showUserMenu();
                    return;
                }
#endif

                due.tm_mday += (current_role == FACULTY) ? 60 : 30;
                if (mktime(&due) == -1)
                {
                    cerr << "Error calculating due date.\n";
                    peopleIn.close();
                    showUserMenu();
                    return;
                }

                char dateStr[20];
                if (!strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &due))
                {
                    cerr << "Error formatting date.\n";
                    peopleIn.close();
                    showUserMenu();
                    return;
                }
                person[4] = dateStr;
                person[5] = dateStr;
            }
            people.push_back(person);
        }
    }
    peopleIn.close();

    if (!userFound)
    {
        time_t now = time(0);
        if (now == -1)
        {
            cerr << "Error getting current time.\n";
            showUserMenu();
            return;
        }

        tm due;
#ifdef _WIN32
        if (localtime_s(&due, &now) != 0)
        {
            cerr << "Error converting time.\n";
            showUserMenu();
            return;
        }
#else
        due = *localtime(&now);
        if (!localtime(&now))
        {
            cerr << "Error converting time.\n";
            showUserMenu();
            return;
        }
#endif

        due.tm_mday += (current_role == FACULTY) ? 60 : 30;
        if (mktime(&due) == -1)
        {
            cerr << "Error calculating due date.\n";
            showUserMenu();
            return;
        }

        char dateStr[20];
        if (!strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &due))
        {
            cerr << "Error formatting date.\n";
            showUserMenu();
            return;
        }

        vector<string> newUser = {
            to_string(current_user_id),
            current_username,
            (current_role == FACULTY) ? "Faculty" : "Student",
            bookTitle + " (" + to_string(bookId) + ")",
            dateStr,
            dateStr,
            "$0"};
        people.push_back(newUser);
    }

    for (auto &book : books)
    {
        if (stoi(book[0]) == bookId)
        {
            book[4] = to_string(stoi(book[4]) - 1);
            break;
        }
    }

    // Persist the reduced stock before recording the member's new loan.
    saveBooks(books);

    ofstream peopleOut("People.txt");
    if (!peopleOut.is_open())
    {
        cerr << "Error: Could not save user records! Changes not saved.\n";
        showUserMenu();
        return;
    }

    peopleOut << "\"ID\", \"Name\", \"Role\", \"Books Borrowed\", \"Time Borrowed\", \"Due Date\", \"Late Fees\"\n";
    for (const auto &person : people)
    {
        peopleOut << "\"" << person[0] << "\", \"" << person[1] << "\", \"" << person[2] << "\", \""
                  << person[3] << "\", \"" << person[4] << "\", \"" << person[5] << "\", \"" << person[6] << "\"\n";
    }
    peopleOut.close();

    cout << "Successfully borrowed: " << bookTitle << "\n";
    cout << "Due date: " << people.back()[5] << "\n";
    showUserMenu();
}

void Library::returnBook()
{
    if (!is_logged_in)
    {
        cerr << "Error: You must be logged in to return books." << endl;
        return;
    }

    viewBorrowedBooks(false);
    int bookId;
    cout << "Enter the ID of the book you want to return: ";
    cin >> bookId;
    cin.ignore();

    ifstream booksFile("books.txt");
    vector<vector<string>> books;
    string line;
    bool bookFound = false;
    string bookTitle;

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
            try
            {
                static_cast<void>(stoi(parts[0]));
                books.push_back(parts);
            }
            catch (const invalid_argument &)
            {
                cerr << "Warning: invalid book entry skipped.\n";
                continue;
            }
            if (stoi(parts[0]) == bookId)
            {
                bookFound = true;
                bookTitle = parts[1];
            }
        }
    }
    booksFile.close();

    if (!bookFound)
    {
        cerr << "Book with ID " << bookId << " not found in the library database." << endl;
        return;
    }

    ifstream peopleIn("People.txt");
    vector<vector<string>> people;
    bool hasBorrowed = false;

    getline(peopleIn, line);

    while (getline(peopleIn, line))
    {
        stringstream ss(line);
        string part;
        vector<string> parts;
        bool inQuotes = false;
        string current;

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

        if (parts.size() >= 7)
        {
            if (stoi(parts[0]) == current_user_id)
            {
                string borrowedBooks = parts[3];
                string bookPattern = bookTitle + " (" + to_string(bookId) + ")";

                size_t pos = borrowedBooks.find(bookPattern);
                if (pos != string::npos)
                {
                    hasBorrowed = true;
                    if (borrowedBooks == bookPattern)
                    {
                        parts[3] = "None";
                        parts[4] = "N/A";
                        parts[5] = "N/A";
                    }
                    else
                    {
                        if (pos > 0 && borrowedBooks[pos - 2] == ',')
                        {
                            borrowedBooks.erase(pos - 2, bookPattern.length() + 2);
                        }
                        else if (pos + bookPattern.length() < borrowedBooks.length() &&
                                 borrowedBooks[pos + bookPattern.length()] == ',')
                        {
                            borrowedBooks.erase(pos, bookPattern.length() + 2);
                        }
                        else
                        {
                            borrowedBooks.erase(pos, bookPattern.length());
                        }
                        parts[3] = borrowedBooks;
                    }
                }
            }
            people.push_back(parts);
        }
    }
    peopleIn.close();

    if (!hasBorrowed)
    {
        cerr << "You have not borrowed book \"" << bookTitle << "\" (ID: " << bookId << ")." << endl;
        return;
    }

    ofstream peopleOut("People.txt");
    peopleOut << "\"ID\", \"Name\", \"Role\", \"Books Borrowed\", \"Time Borrowed\", \"Due Date\", \"Late Fees\"\n";
    for (const auto &person : people)
    {
        peopleOut << "\"" << person[0] << "\", \"" << person[1] << "\", \"" << person[2] << "\", \""
                  << person[3] << "\", \"" << person[4] << "\", \"" << person[5] << "\", \"" << person[6] << "\"\n";
    }
    peopleOut.close();

    for (auto &book : books)
    {
        if (stoi(book[0]) == bookId)
        {
            book[4] = to_string(stoi(book[4]) + 1);
            break;
        }
    }

    ofstream booksOut("books.txt");
    for (const auto &book : books)
    {
        booksOut << book[0] << ", \"" << book[1] << "\", \"" << book[2] << "\", "
                 << book[3] << ", " << book[4] << "\n";
    }
    booksOut.close();

    cout << "\nYou have successfully returned \"" << bookTitle << "\"!" << endl;
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
    showUserMenu();
}

double Library::calculateLateFees(const string &dueDate, UserRole role)
{
    if (dueDate == "N/A" || dueDate.empty())
    {
        return 0.0;
    }

    int year, month, day;
    if (sscanf(dueDate.c_str(), "%d-%d-%d", &year, &month, &day) != 3)
    {
        cerr << "Warning: Invalid due date format: " << dueDate << "\n";
        return 0.0;
    }

    time_t now = time(0);
    if (now == -1)
    {
        cerr << "Error getting current time.\n";
        return 0.0;
    }

    tm currentTm;
#ifdef _WIN32
    if (localtime_s(&currentTm, &now) != 0)
    {
        cerr << "Error converting current time.\n";
        return 0.0;
    }
#else
    tm *temp = localtime(&now);
    if (!temp)
    {
        cerr << "Error converting current time.\n";
        return 0.0;
    }
    currentTm = *temp;
#endif

    tm dueTm{};
    dueTm.tm_year = year - 1900;
    dueTm.tm_mon = month - 1;
    dueTm.tm_mday = day;

    if (dueTm.tm_year < 0 || dueTm.tm_mon < 0 || dueTm.tm_mon > 11 || dueTm.tm_mday < 1 || dueTm.tm_mday > 31)
    {
        cerr << "Warning: Invalid due date components: " << dueDate << "\n";
        return 0.0;
    }

    time_t dueTime = mktime(&dueTm);
    if (dueTime == -1)
    {
        cerr << "Error converting due date.\n";
        return 0.0;
    }

    double secondsLate = difftime(now, dueTime);
    if (secondsLate <= 0)
    {
        return 0.0;
    }

    const int secondsPerDay = 60 * 60 * 24;
    int daysLate = static_cast<int>(secondsLate / secondsPerDay);

    double feePerDay = (role == FACULTY) ? 0.50 : 1.00;
    double fee = daysLate * feePerDay;

    return max(0.0, fee);
}

void Library::checkLateFees()
{
    if (current_role != ADMIN)
    {
        cerr << "Error: You don't have permission to view all late fees." << endl;
        return;
    }

    ifstream peopleFile("People.txt");
    if (!peopleFile.is_open())
    {
        cerr << "Error: Could not open people file!" << endl;
        return;
    }

    cout << "\n=== Users with Late Fees ===\n";
    string line;
    bool headerShown = false;

    getline(peopleFile, line);

    while (getline(peopleFile, line))
    {
        stringstream ss(line);
        string part;
        vector<string> parts;
        bool inQuotes = false;
        string current;

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

        if (parts.size() >= 7)
        {
            string dueDate = parts[5];
            string roleStr = parts[2];
            UserRole role = (roleStr == "Faculty") ? FACULTY : STUDENT;

            double lateFees = calculateLateFees(dueDate, role);
            if (lateFees > 0)
            {
                if (!headerShown)
                {
                    cout << "ID\tName\t\t\tLate Fees\n";
                    cout << "----------------------------------------\n";
                    headerShown = true;
                }

                cout << parts[0] << "\t" << parts[1];
                if (parts[1].length() < 8)
                    cout << "\t\t\t";
                else if (parts[1].length() < 16)
                    cout << "\t\t";
                else
                    cout << "\t";
                cout << "$" << lateFees << endl;
            }
        }
    }

    peopleFile.close();

    if (!headerShown)
    {
        cout << "No users have late fees at this time." << endl;
    }

    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
    showUserMenu();
}

void Library::viewBorrowedBooks(bool returnToMenu)
{
    if (!is_logged_in)
    {
        cerr << "Error: You must be logged in to view borrowed books." << endl;
        return;
    }

    ifstream peopleFile("People.txt");
    if (!peopleFile.is_open())
    {
        cerr << "Error: Could not open people file!" << endl;
        return;
    }

    cout << "\n=== Your Borrowed Books ===\n";
    string line;
    bool found = false;

    getline(peopleFile, line);

    while (getline(peopleFile, line))
    {
        stringstream ss(line);
        string part;
        vector<string> parts;
        bool inQuotes = false;
        string current;

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

        if (parts.size() >= 7 && stoi(parts[0]) == current_user_id)
        {
            found = true;
            cout << "Borrowed Books: " << parts[3] << endl;
            cout << "Borrow Date: " << parts[4] << endl;
            cout << "Due Date: " << parts[5] << endl;

            string roleStr = parts[2];
            UserRole role = (roleStr == "Faculty") ? FACULTY : STUDENT;
            double lateFees = calculateLateFees(parts[5], role);

            if (lateFees > 0)
            {
                cout << "Late Fees: $" << lateFees << endl;
            }
            break;
        }
    }

    peopleFile.close();

    if (!found)
    {
        cout << "You have not borrowed any books." << endl;
    }
    if (returnToMenu)
    {
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
        showUserMenu();
    }
}
