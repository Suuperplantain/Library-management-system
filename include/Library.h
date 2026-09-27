#pragma once

#include <map>
#include <string>
#include <vector>

enum UserRole { ADMIN, FACULTY, STUDENT };

void saveBooks(const std::vector<std::vector<std::string>> &books);

class Library {
private:
    static int next_id;
    int current_user_id;
    std::string current_username;
    std::string current_password;
    UserRole current_role;
    bool is_logged_in;
    std::map<std::string, UserRole> roleMap = {
        {"ADMIN", ADMIN},
        {"FACULTY", FACULTY},
        {"STUDENT", STUDENT},
    };

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
    double calculateLateFees(const std::string &dueDate, UserRole role);
    bool checkFileExists(const std::string &filename);
    std::string cleanString(const std::string &input);
};
