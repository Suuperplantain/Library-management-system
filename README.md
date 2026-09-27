# Library Management System

A menu-driven C++ application for managing a small library. It has separate admin, faculty, and student flows for catalogue management, borrowing, returns, and late-fee checks. Records are stored in local text files.

## Build and run

You need a C++17 compiler. With GCC or MinGW, build it directly:

```bash
g++ -std=c++17 -I include src/*.cpp -o library_management_system
```

Run the executable from the repository directory so the generated data files stay alongside the project:

```bash
./library_management_system
```

Alternatively, use CMake 3.16 or later:

```bash
cmake -S . -B build
cmake --build build
```

The application creates sample `books.txt`, `People.txt`, and `users.txt` files on first run. These runtime files are ignored by Git.

## Source layout

```text
include/Library.h          Shared application state and declarations
src/main.cpp               Program entry point
src/Library.cpp            Construction, shared helpers, initial data
src/Authentication.cpp     Signup, login, and logout
src/Books.cpp              Catalogue operations
src/Borrowing.cpp          Borrowing, returns, and fees
src/Menus.cpp              Main and role-specific menus
```

## Scope and limitations

The application uses local text files and a terminal interface. Authentication is for demonstration only: passwords are stored in plaintext, so don't use real credentials or treat this as production-ready security.
