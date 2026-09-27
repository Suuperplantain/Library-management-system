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
