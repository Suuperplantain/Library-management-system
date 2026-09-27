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
