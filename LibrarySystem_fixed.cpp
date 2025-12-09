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

// =========================
// ENUM: UserRole
// =========================
// Represents the roles a user can have in the library system.
// - ADMIN: Full control over the system, including managing books and users.
// - FACULTY: Can borrow books with extended due dates and view their borrowed books.
// - STUDENT: Can borrow books with standard due dates and view their borrowed books.
enum UserRole
{
    ADMIN,
    FACULTY,
    STUDENT
};

// =========================
// FUNCTION: saveBooks
// =========================
// Saves the updated list of books to the books.txt file.
void saveBooks(const vector<vector<string>> &books)
{
    ofstream booksOut("books.txt");
    if (!booksOut.is_open())
    {
        cerr << "Error: Could not open books file for writing!" << endl;
        return;
    }

    booksOut << "ID,Title,Author,Year,Copies\n"; // Optional header
    for (const auto &book : books)
    {
        booksOut << book[0] << ", \"" << book[1] << "\", \"" << book[2] << "\", "
                 << book[3] << ", " << book[4] << "\n";
    }
    booksOut.close();
}

// =========================
// CLASS: Library
// =========================
// Encapsulates all the functionality of the library management system.
// Includes user authentication, book management, and utility functions.
// Maintains the state of the currently logged-in user and their role.
class Library
{
private:
    static int next_id;      // Tracks the next available user ID for new accounts.
    int current_user_id;     // Stores the ID of the currently logged-in user.
    string current_username; // Stores the username of the currently logged-in user.
    string current_password; // Stores the password of the currently logged-in user.
    UserRole current_role;   // Stores the role of the currently logged-in user.
    bool is_logged_in;       // Indicates whether a user is currently logged in.

    // A map to associate role names (strings) with their corresponding UserRole enum values.
    map<string, UserRole> roleMap = {
        {"ADMIN", ADMIN},
        {"FACULTY", FACULTY},
        {"STUDENT", STUDENT}};

public:
    // =========================
    // CONSTRUCTOR: Library
    // =========================
    // Initializes the library system with default values.
    // By default, no user is logged in, and the role is set to STUDENT.
    Library();

    // =========================
    // FUNCTION DECLARATIONS
    // =========================
    void signup();                                                  // Allows a new user to create an account.
    void login();                                                   // Authenticates a user by verifying their credentials.
    void logout();                                                  // Resets the current user state and ends the session.
    void displayBooks(bool returnToMenu = true);                    // Lists all the books available in the library.
    void searchBooks();                                             // Allows users to search for books by title.
    void addBook();                                                 // Allows admin users to add new books to the library.
    void editBook();                                                // Allows admin users to modify the details of an existing book.
    void removeBook();                                              // Allows admin users to delete a book from the library.
    void borrowBook();                                              // Allows logged-in users to borrow a book.
    void returnBook();                                              // Allows logged-in users to return a borrowed book.
    void checkLateFees();                                           // Allows admin users to view all users with late fees.
    void viewBorrowedBooks(bool returnToMenu = true);               // Displays the books currently borrowed by the logged-in user.
    void showMainMenu();                                            // Displays the main menu for users who are not logged in.
    void showUserMenu();                                            // Displays the menu for logged-in users.
    void createDefaultFiles();                                      // Initializes the system by creating default database files.
    double calculateLateFees(const string &dueDate, UserRole role); // Calculates lateness fees for a user.
    bool checkFileExists(const string &filename);                   // Checks if a file exists in the system.
    string cleanString(const string &input);                        // Removes unnecessary spaces and special characters from a string.
};

// =========================
// STATIC VARIABLE: next_id
// =========================
// Shared across all instances of the Library class.
// Used to assign unique IDs to new users.
int Library::next_id = 15;

Library::Library()
{
    current_user_id = 0;
    current_username = "";
    current_password = "";
    current_role = STUDENT;
    is_logged_in = false;
}

// =========================
// FUNCTION: cleanString
// =========================
// Help from copilot
// Removes unnecessary spaces and special characters from a string.
// Ensures that input strings are properly formatted before processing.

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

// =========================
// FUNCTION: checkFileExists
// =========================
// Checks if a file exists in the system.
// Returns true if the file exists, otherwise false.

bool Library::checkFileExists(const string &filename)
{
    ifstream file(filename);
    return file.good();
}

// =========================
// FUNCTION: createDefaultFiles
// =========================
// Initializes the system by creating default database files.
// Creates sample data for books, users, and borrowed records if the files do not exist.
// Scenario:
// - If the system is run for the first time, this function ensures that the required files are created.
void Library::createDefaultFiles()
{
    // Create books.txt
    ofstream booksFile("books.txt");
    if (booksFile.is_open())
    {
        booksFile << "ID,Title,Author,Year,Copies\n"; // Optional header

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

    // Create People.txt
    ofstream peopleFile("People.txt");
    if (peopleFile.is_open())
    {
        peopleFile << "\"ID\", \"Name\", \"Role\", \"Books Borrowed\", \"Time Borrowed\", \"Due Date\", \"Late Fees\"\n";
        peopleFile << "\"1\", \"Dr. Emily Carter\", \"Faculty\", \"None\", \"N/A\", \"N/A\", \"$0\"\n";
        peopleFile << "\"2\", \"Prof. Robert Greene\", \"Faculty\", \"None\", \"N/A\", \"N/A\", \"$0\"\n";
        peopleFile.close();
    }

    // Create users.txt
    ofstream usersFile("users.txt");
    if (usersFile.is_open())
    {
        usersFile << "1, \"admin\", \"ADMIN\", \"admin123\"\n";
        usersFile << "2, \"faculty1\", \"FACULTY\", \"faculty123\"\n";
        usersFile << "3, \"student1\", \"STUDENT\", \"student123\"\n";
        usersFile.close();
    }
}

// =========================
// FUNCTION: signup
// =========================
// Allows a new user to create an account.
// Assigns a unique ID to the user and saves their credentials in the system.
// Scenario:
// - A student wants to create an account to borrow books.
void Library::signup()
{
    string username, password;

    // Get username/password
    cout << "Create your Username: ";
    cin.ignore();
    getline(cin, username);
    username = cleanString(username);

    cout << "Enter your password: ";
    getline(cin, password);
    password = cleanString(password);

    // Open files
    ofstream usersFile("users.txt", ios::app);
    ofstream peopleFile("People.txt", ios::app);

    if (!usersFile.is_open() || !peopleFile.is_open())
    {
        cerr << "Error: Could not open user files!\n";
        return;
    }

    // Write to users.txt
    usersFile << next_id << ", \"" << username << "\", \"STUDENT\", \"" << password << "\"\n";

    // Write FRESH record to People.txt
    peopleFile << "\"" << next_id << "\", \"" << username << "\", \"Student\", \"None\", \"N/A\", \"N/A\", \"$0\"\n";

    // Update system state
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

// =========================
// FUNCTION: login
// =========================
// Authenticates a user by verifying their credentials.
// Sets the current user state upon successful login.
// Scenario:
// - A faculty member logs in to borrow books with extended due dates.
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

// =========================
// FUNCTION: logout
// =========================
// Resets the current user state and ends the session.
// Scenario:
// - A user logs out after completing their tasks.
void Library::logout()
{
    cout << " Logging out...." << endl;
    cout << " You have successfully logged out!!" << endl;
    cout << "**********Goodbye**********" << endl;

    current_user_id = 0;
    current_username = "";
    current_password = "";
    is_logged_in = false;
    // fix from copilot, preventing the main menu from showing up after logging out
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    this_thread::sleep_for(chrono::seconds(1));
}

// =========================
// FUNCTION: displayBooks
// =========================
// Displays all the books available in the library.
// Admin users are given additional options to manage books.
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

    // Check for and skip header
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

// =========================
// FUNCTION: searchBooks
// =========================
// Allows users to search for books by title.
// Displays matching books if found, otherwise notifies the user.
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

// =========================
// FUNCTION: addBook
// =========================
// Allows admin users to add new books to the library.
// Prompts the admin to enter book details such as title, author, year, and stock.
void Library::addBook()
{
    if (current_role != ADMIN)
    {
        cerr << "Error: You don't have permission to add books." << endl;
        return;
    }

    // Find the highest existing ID
    ifstream inFile("books.txt");
    string line;
    getline(inFile, line); // bug fixed with aid of ChatGPT: skip header
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

// =========================
// FUNCTION: editBook
// =========================
// Allows admin users to edit the details of an existing book.
// Admins can modify the title, author, year, or stock of a book.
void Library::editBook()
{
    if (current_role != ADMIN)
    {
        cerr << "Error: You don't have permission to edit books." << endl;
        return;
    }

    // Read all books into memory
    ifstream inFile("books.txt");
    if (!inFile.is_open())
    {
        cerr << "Error: Could not open books file!" << endl;
        return;
    }

    vector<vector<string>> books;
    string line;

    // Skip header
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

    // Display books without returning to menu
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

    // Get book ID to edit
    int bookId;
    cout << "Enter the ID of the book you want to edit: ";
    cin >> bookId;
    cin.ignore(); // Clear newline

    // Find the book
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
            cin.ignore(); // Clear newline

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

    // Write all books back to file
    ofstream outFile("books.txt");
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open books file for writing!" << endl;
        showUserMenu();
        return;
    }

    outFile << "ID,Title,Author,Year,Copies\n"; // Write header
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

// =========================
// FUNCTION: removeBook
// =========================
// Allows admin users to remove a book from the library.
// Admins are prompted to select a book by its ID for removal.
void Library::removeBook()
{
    if (current_role != ADMIN)
    {
        cerr << "Error: You don't have permission to remove books." << endl;
        return;
    }

    // Read all books into memory
    ifstream inFile("books.txt");
    if (!inFile.is_open())
    {
        cerr << "Error: Could not open books file!" << endl;
        return;
    }

    vector<vector<string>> books;
    string line;

    // Skip header
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

    // Display books without returning to menu
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

    // Get book ID to remove
    int bookId;
    cout << "Enter the ID of the book you want to remove: ";
    cin >> bookId;
    cin.ignore(); // Clear newline

    // Find and remove the book
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

    // Write remaining books back to file
    ofstream outFile("books.txt");
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open books file for writing!" << endl;
        showUserMenu();
        return;
    }

    outFile << "ID,Title,Author,Year,Copies\n"; // Write header
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

// =========================
// FUNCTION: borrowBook
// =========================
// Allows logged-in users to borrow a book from the library.
// Updates the user's borrowed books and adjusts the stock of the book.
void Library::borrowBook()
{
    if (!is_logged_in)
    {
        cerr << "Error: Login required\n";
        return;
    }

    displayBooks(false);

    // Get book ID with robust input validation
    // Edit made by DeepSeek AI: Added comprehensive input validation with maximum tries
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

    // Load book data with improved error handling
    // Edit made by DeepSeek AI: Added more detailed error messages
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

    // Skip header
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

    // Update People.txt with improved date handling
    // Edit made by DeepSeek AI: Added cross-platform date handling and error checking///////////////////////////////////
    vector<vector<string>> people;
    bool userFound = false;
    ifstream peopleIn("People.txt");
    if (!peopleIn.is_open())
    {
        cerr << "Error: Could not open user records file!\n";
        showUserMenu();
        return;
    }

    // Skip header
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
                    // Check if already borrowed
                    if (person[3].find("(" + to_string(bookId) + ")") != string::npos)
                    {
                        cerr << "Error: You have already borrowed this book.\n";
                        peopleIn.close();
                        showUserMenu();
                        return;
                    }
                    person[3] += ", " + bookTitle + " (" + to_string(bookId) + ")";
                }
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Set dates with improved error handling, By DeepSeek AI///////////////////////////////////////////////////////
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

    // Create new record if needed
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Update books quantity
    for (auto &book : books)
    {
        if (stoi(book[0]) == bookId)
        {
            book[4] = to_string(stoi(book[4]) - 1);
            break;
        }
    }

    // Save files with error checking
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
    cout << "Due date: " << people.back()[5] << "\n"; // Show due date to user
    showUserMenu();
}

// =========================
// FUNCTION: returnBook
// =========================
// Allows logged-in users to return a borrowed book.
// Updates the user's borrowed books and adjusts the stock of the book.
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
    cin.ignore(); // bug fixed with aid of ChatGPT: clear newline after input

    // Check if book exists in library
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
                int id = stoi(parts[0]); // bug fixed with aid of ChatGPT: safe stoi usage
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

    // Check if user has borrowed this book
    ifstream peopleIn("People.txt");
    vector<vector<string>> people;
    bool hasBorrowed = false;

    // Skip header
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
                    // Remove the book from borrowed list
                    if (borrowedBooks == bookPattern)
                    {
                        parts[3] = "None";
                        parts[4] = "N/A";
                        parts[5] = "N/A";
                    }
                    else
                    {
                        // Handle cases where the book is first, last, or in middle of list
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

    // Write updated people data
    ofstream peopleOut("People.txt");
    peopleOut << "\"ID\", \"Name\", \"Role\", \"Books Borrowed\", \"Time Borrowed\", \"Due Date\", \"Late Fees\"\n";
    for (const auto &person : people)
    {
        peopleOut << "\"" << person[0] << "\", \"" << person[1] << "\", \"" << person[2] << "\", \""
                  << person[3] << "\", \"" << person[4] << "\", \"" << person[5] << "\", \"" << person[6] << "\"\n";
    }
    peopleOut.close();

    // Update book stock
    for (auto &book : books)
    {
        if (stoi(book[0]) == bookId)
        {
            book[4] = to_string(stoi(book[4]) + 1);
            break;
        }
    }

    // Write updated books data
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

// =========================
// FUNCTION: calculateLateFees
// =========================
// Calculates the late fees for a borrowed book based on the current date and the due date.
// Late fee is $1 per day for students and $0.50 per day for faculty.
// function from DeepSeek AI
double Library::calculateLateFees(const string &dueDate, UserRole role)
{
    if (dueDate == "N/A" || dueDate.empty())
    {
        return 0.0;
    }

    // Edit made by DeepSeek AI: Added more robust date parsing
    int year, month, day;
    if (sscanf(dueDate.c_str(), "%d-%d-%d", &year, &month, &day) != 3)
    {
        cerr << "Warning: Invalid due date format: " << dueDate << "\n";
        return 0.0;
    }

    // Edit made by DeepSeek AI: Added comprehensive time handling with error checking
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

    // Set up due date struct with validation
    tm dueTm = {0};
    dueTm.tm_year = year - 1900;
    dueTm.tm_mon = month - 1;
    dueTm.tm_mday = day;

    // Edit made by DeepSeek AI: Validate date components
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

    // Calculate difference in seconds
    double secondsLate = difftime(now, dueTime);
    if (secondsLate <= 0)
    {
        return 0.0;
    }

    // Convert to days
    const int secondsPerDay = 60 * 60 * 24;
    int daysLate = static_cast<int>(secondsLate / secondsPerDay);

    // Edit made by DeepSeek AI: Added minimum fee calculation
    double feePerDay = (role == FACULTY) ? 0.50 : 1.00;
    double fee = daysLate * feePerDay;

    // Ensure minimum fee is $0.00 (no negative fees)
    return max(0.0, fee);
}

// =========================
// FUNCTION: checkLateFees
// =========================
// Allows admin users to view all users with late fees.
// Dynamically calculates lateness fees and updates the People.txt file.
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

    // Skip header
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

// =========================
// FUNCTION: viewBorrowedBooks
// =========================
// Displays the books currently borrowed by the logged-in user.
// Includes details such as borrow date, due date, and lateness fees (if any).
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

    // Skip header
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

            // Calculate and display late fees if any
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

// =========================
// FUNCTION: showMainMenu
// =========================
// Displays the main menu for users who are not logged in.
// Provides options to log in, sign up, or exit the system.
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

// =========================
// FUNCTION: showUserMenu
// =========================
// Displays the menu for logged-in users based on their role.
// Admins, faculty, and students have different options available.
void Library::showUserMenu()
{
    while (is_logged_in) // Only show menu while logged in
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
                displayBooks(false); // bug fixed with aid of ChatGPT: prevent re-entry into admin menu loop
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
                logout(); // FIX FROM Copilot AI: Added return to break out of menu loop completely
                return;   // Exit the menu completely after logout
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
                displayBooks(false); // bug fixed with aid of ChatGPT: prevent re-entry into admin menu loop
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
                logout(); // FIX FROM Copilot AI: Added return to break out of menu loop completely
                return;   // Exit the menu completely after logout
            case 7:
                cout << "Goodbye!\n";
                exit(0);
            default:
                cerr << "Invalid choice. Try again.\n";
            }
        }
    }

    // If we get here, the user is no longer logged in
    showMainMenu(); // Return to main menu
}
// =========================
// FUNCTION: main
// =========================
// Entry point of the program.
// Initializes the library system and displays the main menu.
int main()
{
    Library lib;
    lib.showMainMenu();
    return 0;
}
