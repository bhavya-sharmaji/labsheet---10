// Lab Sheet-09: Smart Digital Expense Tracker with Budget Prediction
// Beginner-friendly menu-driven C++ project.
// Data is saved in expenses.txt and user details in users.txt.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <limits>
using namespace std;

struct Expense {
    int id;
    string date;       // Format: YYYY-MM-DD
    string category;
    string description;
    double amount;
};

struct User {
    string username;
    string password;
};

vector<Expense> expenses;
string currentUser;
double monthlyBudget = 0.0;

// ---------- Utility functions ----------
string currentMonth() {
    time_t now = time(0);
    tm *localTime = localtime(&now);
    stringstream month;
    month << (localTime->tm_year + 1900) << "-"
          << setw(2) << setfill('0') << (localTime->tm_mon + 1);
    return month.str();
}

string todayDate() {
    time_t now = time(0);
    tm *localTime = localtime(&now);
    stringstream date;
    date << (localTime->tm_year + 1900) << "-"
         << setw(2) << setfill('0') << (localTime->tm_mon + 1) << "-"
         << setw(2) << setfill('0') << localTime->tm_mday;
    return date.str();
}

string userExpenseFile() {
    return currentUser + "_expenses.txt";
}

string userBudgetFile() {
    return currentUser + "_budget.txt";
}

int nextExpenseId() {
    int largestId = 0;
    for (const Expense &expense : expenses) {
        if (expense.id > largestId) largestId = expense.id;
    }
    return largestId + 1;
}

// ---------- Module 1: File saving and loading ----------
void loadExpenses() {
    expenses.clear();
    ifstream file(userExpenseFile());
    Expense expense;
    while (file >> expense.id) {
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(file, expense.date);
        getline(file, expense.category);
        getline(file, expense.description);
        file >> expense.amount;
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        if (file) expenses.push_back(expense);
    }
    file.close();
}

void saveExpenses() {
    ofstream file(userExpenseFile());
    for (const Expense &expense : expenses) {
        file << expense.id << '\n'
             << expense.date << '\n'
             << expense.category << '\n'
             << expense.description << '\n'
             << fixed << setprecision(2) << expense.amount << '\n';
    }
    file.close();
}

void loadBudget() {
    ifstream file(userBudgetFile());
    if (!(file >> monthlyBudget)) monthlyBudget = 0.0;
    file.close();
}

void saveBudget() {
    ofstream file(userBudgetFile());
    file << fixed << setprecision(2) << monthlyBudget;
    file.close();
}

// ---------- Module 2: User registration and login ----------
bool registerUser() {
    string username, password;
    cout << "\n--- User Registration ---\n";
    cout << "Create username (no spaces): ";
    cin >> username;
    cout << "Create password (no spaces): ";
    cin >> password;

    ifstream input("users.txt");
    string savedUser, savedPassword;
    while (input >> savedUser >> savedPassword) {
        if (savedUser == username) {
            cout << "This username already exists.\n";
            return false;
        }
    }
    input.close();

    ofstream output("users.txt", ios::app);
    output << username << " " << password << '\n';
    output.close();

    cout << "Registration successful. Please login.\n";
    return true;
}

bool loginUser() {
    string username, password;
    cout << "\n--- Login ---\n";
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    ifstream input("users.txt");
    string savedUser, savedPassword;
    while (input >> savedUser >> savedPassword) {
        if (savedUser == username && savedPassword == password) {
            currentUser = username;
            input.close();
            loadExpenses();
            loadBudget();
            cout << "Login successful. Welcome, " << currentUser << "!\n";
            return true;
        }
    }
    input.close();
    cout << "Incorrect username or password.\n";
    return false;
}

// ---------- Module 3: Budget setting and calculation ----------
double monthlySpending() {
    double total = 0.0;
    string month = currentMonth();
    for (const Expense &expense : expenses) {
        if (expense.date.size() >= 7 && expense.date.substr(0, 7) == month)
            total += expense.amount;
    }
    return total;
}

void showBudgetWarning() {
    if (monthlyBudget <= 0) {
        cout << "No monthly budget set. Please set your budget first.\n";
        return;
    }

    double spent = monthlySpending();
    double remaining = monthlyBudget - spent;

    cout << fixed << setprecision(2);
    cout << "Monthly budget: Rs. " << monthlyBudget << '\n';
    cout << "Spent this month: Rs. " << spent << '\n';

    if (remaining < 0)
        cout << "WARNING: Budget exceeded by Rs. " << -remaining << "!\n";
    else if (spent >= monthlyBudget * 0.80)
        cout << "Warning: You have used 80% or more of your budget. Remaining: Rs. "
             << remaining << '\n';
    else
        cout << "You are within your budget. Remaining: Rs. " << remaining << '\n';
}

void setBudget() {
    cout << "Enter your monthly budget (Rs.): ";
    double amount;
    cin >> amount;
    if (!cin || amount <= 0) {
        cout << "Please enter a valid positive amount.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    monthlyBudget = amount;
    saveBudget();
    cout << "Monthly budget saved successfully.\n";
    showBudgetWarning();
}

// ---------- Module 4: Add and view expenses ----------
void addExpense() {
    Expense expense;
    expense.id = nextExpenseId();

    cout << "\n--- Add Daily Expense ---\n";
    cout << "Enter date (YYYY-MM-DD) or type T for today: ";
    string dateInput;
    cin >> dateInput;
    if (dateInput == "T" || dateInput == "t")
        expense.date = todayDate();
    else
        expense.date = dateInput;

    cout << "Category (Food/Travel/Study/Shopping/Bills/Other): ";
    cin >> ws;
    getline(cin, expense.category);

    cout << "Description: ";
    getline(cin, expense.description);

    cout << "Amount (Rs.): ";
    cin >> expense.amount;
    if (!cin || expense.amount <= 0) {
        cout << "Invalid amount. Expense was not added.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    expenses.push_back(expense);
    saveExpenses();
    cout << "Expense added and saved successfully. Expense ID: " << expense.id << '\n';
    showBudgetWarning();
}

void viewExpenseHistory() {
    cout << "\n--- Expense History ---\n";
    if (expenses.empty()) {
        cout << "No expenses found.\n";
        return;
    }

    cout << left << setw(6) << "ID" << setw(13) << "Date"
         << setw(16) << "Category" << setw(25) << "Description"
         << "Amount (Rs.)\n";
    cout << string(75, '-') << '\n';

    cout << fixed << setprecision(2);
    for (const Expense &expense : expenses) {
        cout << left << setw(6) << expense.id
             << setw(13) << expense.date
             << setw(16) << expense.category
             << setw(25) << expense.description
             << expense.amount << '\n';
    }
}

// ---------- Module 5: Category-wise expense ----------
void categoryWiseExpense() {
    cout << "\n--- Category-wise Expense ---\n";
    if (expenses.empty()) {
        cout << "No expenses found.\n";
        return;
    }

    vector<string> categories;
    vector<double> totals;

    for (const Expense &expense : expenses) {
        auto position = find(categories.begin(), categories.end(), expense.category);
        if (position == categories.end()) {
            categories.push_back(expense.category);
            totals.push_back(expense.amount);
        } else {
            int index = static_cast<int>(position - categories.begin());
            totals[index] += expense.amount;
        }
    }

    cout << fixed << setprecision(2);
    for (size_t i = 0; i < categories.size(); i++)
        cout << categories[i] << " : Rs. " << totals[i] << '\n';
}

// ---------- Module 6: Monthly report and prediction ----------
void monthlyReport() {
    string month;
    cout << "Enter month (YYYY-MM), or type C for current month: ";
    cin >> month;
    if (month == "C" || month == "c") month = currentMonth();

    double total = 0.0;
    int count = 0;
    for (const Expense &expense : expenses) {
        if (expense.date.size() >= 7 && expense.date.substr(0, 7) == month) {
            total += expense.amount;
            count++;
        }
    }

    cout << fixed << setprecision(2);
    cout << "\n--- Monthly Expense Report: " << month << " ---\n";
    cout << "Number of expenses: " << count << '\n';
    cout << "Total spent: Rs. " << total << '\n';

    if (month == currentMonth() && monthlyBudget > 0) {
        time_t now = time(0);
        tm *localTime = localtime(&now);
        int daysPassed = localTime->tm_mday;
        int daysInMonth = 31;
        int year = localTime->tm_year + 1900;
        int mon = localTime->tm_mon + 1;
        if (mon == 4 || mon == 6 || mon == 9 || mon == 11) daysInMonth = 30;
        else if (mon == 2) {
            bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
            daysInMonth = leap ? 29 : 28;
        }

        double predicted = (total / daysPassed) * daysInMonth;
        cout << "Monthly budget: Rs. " << monthlyBudget << '\n';
        cout << "Predicted month-end spending: Rs. " << predicted << '\n';
        if (predicted > monthlyBudget)
            cout << "Prediction: You may exceed your monthly budget.\n";
        else
            cout << "Prediction: You are likely to stay within your budget.\n";
    } else if (month == currentMonth()) {
        cout << "Set a monthly budget to see budget prediction.\n";
    }
}

// ---------- Module 7: Search and delete ----------
void searchExpense() {
    int id;
    cout << "Enter expense ID to search: ";
    cin >> id;

    for (const Expense &expense : expenses) {
        if (expense.id == id) {
            cout << "Expense found:\n";
            cout << "ID: " << expense.id << "\nDate: " << expense.date
                 << "\nCategory: " << expense.category
                 << "\nDescription: " << expense.description
                 << "\nAmount: Rs. " << fixed << setprecision(2) << expense.amount << '\n';
            return;
        }
    }
    cout << "Expense ID not found.\n";
}

void deleteExpense() {
    int id;
    cout << "Enter expense ID to delete: ";
    cin >> id;

    auto position = find_if(expenses.begin(), expenses.end(),
        [id](const Expense &expense) { return expense.id == id; });

    if (position == expenses.end()) {
        cout << "Expense ID not found.\n";
        return;
    }

    expenses.erase(position);
    saveExpenses();
    cout << "Expense deleted successfully.\n";
}

// ---------- Main menu ----------
void showMenu() {
    int choice;
    do {
        cout << "\n========== SMART DIGITAL EXPENSE TRACKER ==========\n";
        cout << "1. Add Daily Expense\n";
        cout << "2. View Expense History\n";
        cout << "3. Category-wise Expense\n";
        cout << "4. Monthly Expense Report and Prediction\n";
        cout << "5. Set Monthly Budget\n";
        cout << "6. Budget Warning\n";
        cout << "7. Search Expense\n";
        cout << "8. Delete Expense\n";
        cout << "9. Logout and Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addExpense(); break;
            case 2: viewExpenseHistory(); break;
            case 3: categoryWiseExpense(); break;
            case 4: monthlyReport(); break;
            case 5: setBudget(); break;
            case 6: showBudgetWarning(); break;
            case 7: searchExpense(); break;
            case 8: deleteExpense(); break;
            case 9: cout << "Logged out. Thank you!\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);
}

int main() {
    int choice;
    cout << "===============================================\n";
    cout << "       SMART DIGITAL EXPENSE TRACKER\n";
    cout << "===============================================\n";

    do {
        cout << "\n1. Register\n2. Login\n3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            if (loginUser()) showMenu();
        } else if (choice == 3) {
            cout << "Goodbye!\n";
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);

    return 0;
}
