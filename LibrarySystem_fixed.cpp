#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <chrono>
#include <thread>
using namespace std;

enum UserRole
{
    ADMIN,
    FACULTY,
    STUDENT
};

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

class Library
{
private:
    static int next_id;
    int current_user_id;
    string current_username;
    string current_password;
    UserRole current_role;
    bool is_logged_in;

    map<string, UserRole> roleMap = {
        {"ADMIN", ADMIN},
        {"FACULTY", FACULTY},
        {"STUDENT", STUDENT}};

public:
    Library();

    void signup();
    void login();
    void logout();
    void displayBooks(bool returnToMenu = true);
    void searchBooks();
    void addBook();
    void editBook();
    void removeBook();
    void borrowBook();
    void returnBook();
    void checkLateFees();
    void viewBorrowedBooks(bool returnToMenu = true);
    void showMainMenu();
    void showUserMenu();
    void createDefaultFiles();
    double calculateLateFees(const string &dueDate, UserRole role);
    bool checkFileExists(const string &filename);
    string cleanString(const string &input);
};

int Library::next_id = 15;

Library::Library()
{
    current_user_id = 0;
    current_username = "";
    current_password = "";
    current_role = STUDENT;
    is_logged_in = false;
}

string Library::cleanString(const string &input)
{
    string cleaned;
    for (char c : input)
    {
        if (c != '\"' && c != '\'' && c != '\\')
        {
            cleaned += c;
        }
    }
    cleaned.erase(0, cleaned.find_first_not_of(" \t"));
    cleaned.erase(cleaned.find_last_not_of(" \t") + 1);
    return cleaned;
}

bool Library::checkFileExists(const string &filename)
{
    ifstream file(filename);
    return file.good();
}

void Library::createDefaultFiles()
{
    ofstream booksFile("books.txt");
    if (booksFile.is_open())
    {
        booksFile << "ID,Title,Author,Year,Copies\n";

        booksFile << "1,\"C++ Programming Basics\",\"John Doe\",2019,5\n";
        booksFile << "2,\"Data Structures and Algorithms\",\"Jane Smith\",2020,3\n";
        booksFile << "3,\"Introduction to Machine Learning\",\"Alice Brown\",2021,2\n";
        booksFile << "4,\"Database Management Systems\",\"Robert White\",2018,4\n";
        booksFile << "5,\"The C++ Standard Library\",\"Bjarne Stroustrup\",2022,6\n";
        booksFile << "6,\"Artificial Intelligence: A Modern Approach\",\"Stuart Russell\",2021,3\n";
        booksFile << "7,\"Operating Systems Concepts\",\"Abraham Silberschatz\",2017,5\n";
        booksFile << "8,\"Computer Networks\",\"Andrew Tanenbaum\",2019,2\n";
        booksFile << "9,\"Clean Code\",\"Robert C. Martin\",2008,4\n";
        booksFile << "10,\"The Pragmatic Programmer\",\"Andy Hunt\",1999,2\n";
        booksFile << "11,\"Design Patterns\",\"Erich Gamma\",1994,3\n";
        booksFile << "12,\"C++ Concurrency in Action\",\"Anthony Williams\",2020,2\n";
        booksFile << "13,\"Python for Data Analysis\",\"Wes McKinney\",2018,4\n";
        booksFile << "14,\"Learning React\",\"Alex Banks\",2022,6\n";
        booksFile << "15,\"Introduction to Algorithms\",\"Thomas H. Cormen\",2009,5\n";
        booksFile << "16,\"Hands-On Machine Learning\",\"Aurélien Géron\",2023,3\n";
        booksFile << "17,\"Deep Learning\",\"Ian Goodfellow\",2016,2\n";
        booksFile << "18,\"Head First Design Patterns\",\"Eric Freeman\",2004,4\n";
        booksFile << "19,\"Computer Organization and Design\",\"David A. Patterson\",2021,3\n";
        booksFile << "20,\"Modern Operating Systems\",\"Andrew S. Tanenbaum\",2018,5\n";

        booksFile.close();
    }

    ofstream peopleFile("People.txt");
    if (peopleFile.is_open())
    {
        peopleFile << "\"ID\", \"Name\", \"Role\", \"Books Borrowed\", \"Time Borrowed\", \"Due Date\", \"Late Fees\"\n";
        peopleFile << "\"1\", \"Dr. Emily Carter\", \"Faculty\", \"None\", \"N/A\", \"N/A\", \"$0\"\n";
        peopleFile << "\"2\", \"Prof. Robert Greene\", \"Faculty\", \"None\", \"N/A\", \"N/A\", \"$0\"\n";
        peopleFile.close();
    }

    ofstream usersFile("users.txt");
    if (usersFile.is_open())
    {
        usersFile << "1, \"admin\", \"ADMIN\", \"admin123\"\n";
        usersFile << "2, \"faculty1\", \"FACULTY\", \"faculty123\"\n";
        usersFile << "3, \"student1\", \"STUDENT\", \"student123\"\n";
        usersFile.close();
    }
}

void Library::signup()
{
    string username, password;

    cout << "Create your Username: ";
    cin.ignore();
    getline(cin, username);
    username = cleanString(username);

    cout << "Enter your password: ";
    getline(cin, password);
    password = cleanString(password);

    ofstream usersFile("users.txt", ios::app);
    ofstream peopleFile("People.txt", ios::app);

    if (!usersFile.is_open() || !peopleFile.is_open())
    {
        cerr << "Error: Could not open user files!\n";
        return;
    }

    usersFile << next_id << ", \"" << username << "\", \"STUDENT\", \"" << password << "\"\n";

    peopleFile << "\"" << next_id << "\", \"" << username << "\", \"Student\", \"None\", \"N/A\", \"N/A\", \"$0\"\n";

    current_user_id = next_id;
    current_username = username;
    current_password = password;
    current_role = STUDENT;
    is_logged_in = true;

    cout << "\nAccount created successfully!\n";
    cout << "Your ID is " << next_id << "\n";
    next_id++;

    usersFile.close();
    peopleFile.close();
    showUserMenu();
}

void Library::login()
{
    string username, password;
    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    ifstream usersFile("users.txt");
    if (!usersFile.is_open())
    {
        cerr << "Error: Could not open users file!\n";
        return;
    }

    string line;
    while (getline(usersFile, line))
    {
        vector<string> parts;
        stringstream ss(line);
        string part;

        while (getline(ss, part, ','))
        {
            parts.push_back(cleanString(part));
        }

        if (parts.size() >= 4)
        {
            string file_username = parts[1];
            string file_password = parts[3];

            if (file_username == username && file_password == password)
            {
                current_user_id = stoi(parts[0]);
                current_username = username;
                current_password = password;

                string role_str = parts[2];
                if (role_str == "ADMIN")
                    current_role = ADMIN;
                else if (role_str == "FACULTY")
                    current_role = FACULTY;
                else
                    current_role = STUDENT;

                is_logged_in = true;
                cout << "\nLogin successful! Welcome " << username << "!\n";
                usersFile.close();
                showUserMenu();
                return;
            }
        }
    }

    usersFile.close();
    cerr << "Invalid username or password!\n";
}

void Library::logout()
{
    cout << " Logging out...." << endl;
    cout << " You have successfully logged out!!" << endl;
    cout << "**********Goodbye**********" << endl;

    current_user_id = 0;
    current_username = "";
    current_password = "";
    is_logged_in = false;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    this_thread::sleep_for(chrono::seconds(1));
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
                int id = stoi(parts[0]);
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

    tm dueTm = {0};
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

int main()
{
    Library lib;
    lib.showMainMenu();
    return 0;
}
