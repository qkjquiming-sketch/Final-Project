//+==========================================+
//      EPEECT PARKING MANAGEMENT SYSTEM
//+==========================================+

#include <iostream>
#include <string>  
#include <cstdlib>
#include <limits>  
#include <iomanip>  
#include <fstream> 
#include <filesystem>
#include <chrono>
#include <ctime> 
using namespace std;

//+==========================================+
//       STRUCTURES AND CONSTANTS
//+==========================================+

struct ParkingLog {
    string licensePlate;    // Vehicle's license plate
    string entryTime;       // Entry time of the vehicle
    string exitTime;        // Exit time of the vehicle
    float fee;              // Parking fee
};

constexpr int   TOTAL_SPACES    = 100;              // Total parking spaces
constexpr int   MAX_LOGS        = 100;              // Maximum number of parking logs
constexpr float RATE_PER_HOUR   = 20.0f;            // Standard rate per hour
constexpr float OVERTIME_RATE   = 30.0f;            // Overtime parking rate
constexpr float OVERNIGHT_RATE  = 200.0f;           // Overnight parking rate
constexpr float LOST_CARD_FEE   = 200.0f;           // Lost card compensation fee

//+==========================================+
//           FUNCTION DECLARATIONS
//+==========================================+
// HELPER FUNCTION DECLARATIONS
bool isValidTime(const string &time);                                                          // Declares the function to validate time format
float timeToHours(string time);                                                                // Declares the function to convert time to hours
float calculateParkingFee(float duration, float RATE_PER_HOUR, float OVERTIME_RATE, float overnightRate, float lostCardFee); // Declares the function to calculate parking fee
int  findVehicle(ParkingLog logs[], int logCount, string plate);                               // Declares the function to find vehicle by license plate
void printLogHeader(ostream &out);                                                             // Declares the function to print log header to file
string formatExitTime(const ParkingLog &log);                                                  // Declares the function to format exit time
string formatFee(const ParkingLog &log);                                                       // Declares the function to format fee
void clearScreen();                                                                            // Declares the function to clear the console screen
void pauseProgram();                                                                           // Declares the function to pause the program     
void printCentered(ostream &out, string text, int width = 45);                                 // Declares the function to print centered text    
// MAIN FUNCTION DECLARATIONS
void printMenu(int TOTAL_SPACES, int occupiedSpaces);                                          // Declares the function to print the menu                                                                         
void vehicleEntry(ParkingLog logs[], int &occupiedSpaces, int TOTAL_SPACES, int &logCount);    // Declares the function for vehicle entry
void vehicleExit(ParkingLog logs[], int &occupiedSpaces, int TOTAL_SPACES, int &logCount);     // Declares the function for vehicle exit
void viewLogs(ParkingLog logs[], int logCount);                                                // Declares the function to view parking logs
void saveLogsToFile(ParkingLog logs[], int logCount);                                          // Declares the function to save logs to a file        

//+==========================================+
//               MAIN FUNCTION
//+==========================================+

int main() {
    int occupiedSpaces = 0;     // Current occupied parking spaces
    ParkingLog logs[MAX_LOGS];  // Array to store parking logs
    int logCount = 0;           // Current number of logs
    int choice = 0;             // User menu choice

    // Main program loop
    while (choice != 4) {
        clearScreen();
        printMenu(TOTAL_SPACES, occupiedSpaces);
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input! Please enter a number (1-4).\n";
            pauseProgram();
            continue;
        }

        // Handle user choice
        switch (choice) {
            case 1: vehicleEntry(logs, occupiedSpaces, TOTAL_SPACES, logCount); pauseProgram(); break;  // Vehicle Entry
            case 2: vehicleExit(logs, occupiedSpaces, TOTAL_SPACES, logCount); pauseProgram(); break;   // Vehicle Exit
            case 3: viewLogs(logs, logCount); pauseProgram(); break;                                    // View Parking Logs
            case 4: saveLogsToFile(logs, logCount); cout << "Exiting the program. Goodbye!\n"; return 0;// Exit Program
            default: cout << "Invalid choice. Please try again.\n"; pauseProgram(); break;              // Invalid Choice
        }
    }

    return 0;
}
//+==========================================+
//       HELPER FUNCTIONS DEFINITIONS
//+==========================================+

// Clear console screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        if (system("clear") != 0) {
            cout << "\033[2J\033[1;H";
        }
    #endif
}

// Pause program until Enter is pressed
void pauseProgram() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Validate time format HH:MM
bool isValidTime(const string& time) {
    int h, m;
    if (sscanf(time.c_str(), "%d:%d", &h, &m) != 2) return false;
    return h >= 0 && h < 24 && m >= 0 && m < 60;
}

// Convert "HH:MM" string to float hours
float timeToHours(string time) {
    int hours, minutes;
    sscanf(time.c_str(), "%d:%d", &hours, &minutes);
    return hours + minutes / 60.0;
}

// Calculate parking fee
float calculateParkingFee(float duration, float RATE_PER_HOUR, float OVERTIME_RATE, float overnightRate, float lostCardFee) {
    float fee = (duration <= 3) ? RATE_PER_HOUR * duration : (3 * RATE_PER_HOUR + (duration - 3) * OVERTIME_RATE);
    fee += overnightRate + lostCardFee;
    return fee;
}

// Find vehicle index by license plate
int findVehicle(ParkingLog logs[], int logCount, string plate) {
    for (int i = 0; i < logCount; ++i) {
        if (logs[i].licensePlate == plate) return i;
    }
    return -1; // Not found
}

// Print log table header
void printLogHeader(ostream &out) {
    out << left 
        << setw(5)  << "#" 
        << setw(15) << "License Plate"
        << setw(15) << "Entry Time"
        << setw(15) << "Exit Time"
        << setw(15) << "Fee (Pesos)"
        << endl;
    out << string(60, '-') << endl;
}

// Format exit time for display
string formatExitTime(const ParkingLog &log) {
    return log.exitTime.empty() ? "[Still Parked]" : log.exitTime;
}

// Format fee for display
string formatFee(const ParkingLog &log) {
    if (log.exitTime.empty()) return "—";
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.2f", log.fee);
    return string(buffer);
}

// Center text
void printCentered(ostream &out, string text, int width) {
    int pad = max(0, (width - (int)text.length()) / 2);
    out << string(pad, ' ') << text << endl;
}

//+==========================================+
//         MAIN FUNCTIONS DEFINITIONS
//+==========================================+

// Print menu
void printMenu(int TOTAL_SPACES, int occupiedSpaces) {
    clearScreen();
    cout << "+==========================================+\n";
    printCentered(cout, "EPEECT PARKING MANAGEMENT SYSTEM", 45);
    cout << "+==========================================+\n";
    cout << " Available Spaces: " << setw(3) << (TOTAL_SPACES - occupiedSpaces)
         << " / " << TOTAL_SPACES << "\n";
    cout << "-----------------------------------------------\n";
    cout << " [1] Vehicle Entry\n";
    cout << " [2] Vehicle Exit\n";
    cout << " [3] View Parking Logs & Invoices\n";
    cout << " [4] Exit Program\n";
    cout << "-----------------------------------------------\n";
    cout << " Enter your choice: ";
}

// Vehicle entry
void vehicleEntry(ParkingLog logs[], int &occupiedSpaces, int TOTAL_SPACES, int &logCount) {
    string plate, entryTime;
    if (occupiedSpaces < TOTAL_SPACES) {
        cout << "\nEnter License Plate: ";
        getline(cin, plate);
        logs[logCount].licensePlate = plate;

        cout << "Enter Entry Time (HH:MM): ";
        getline(cin, entryTime);
        if (!isValidTime(entryTime)) {
            cout << "ERROR: Invalid time format. Please use HH:MM (24-hour format).\n";
            return;
        }

        logs[logCount].entryTime = entryTime;
        logs[logCount].fee = 0;
        occupiedSpaces++;
        logCount++;
        cout << "Vehicle entered successfully.\n";
        cout << "Slots remaining: " << (TOTAL_SPACES - occupiedSpaces) << "\n";
    } else {
        cout << "ERROR! Parking Full. No available spaces.\n";
    }
}

// Vehicle exit
void vehicleExit(ParkingLog logs[], int &occupiedSpaces, int TOTAL_SPACES, int &logCount) {
    string exitTime;

    if (occupiedSpaces == 0) {
        cout << "\nNo vehicles are currently parked.\n";
        return;
    }

    cout << "+==========================================+\n";
    printCentered(cout, "CURRENTLY PARKED VEHICLES", 45);
    cout << "+==========================================+\n";
    cout << left << setw(5) << "#" << setw(15) << "License Plate" << setw(15) << "Entry Time\n";
    cout << string(35, '-') << endl;

    int availableIndices[MAX_LOGS];
    int count = 0;

    for (int i = 0; i < logCount; i++) {    
        if (logs[i].exitTime.empty()) {
            cout << left << setw(5) << count + 1
                 << setw(15) << logs[i].licensePlate
                 << setw(15) << logs[i].entryTime << endl;
            availableIndices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "\nAll vehicles have already exited.\n";
        return;
    }

    int exitVehicle;
    cout << "\nSelect a vehicle to exit (1 - " << count << "): ";
    cin >> exitVehicle;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (cin.fail() || exitVehicle < 1 || exitVehicle > count) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "ERROR: Invalid selection.\n";
        return;
    }

    int index = availableIndices[exitVehicle - 1];
    cout << "Enter Exit Time (HH:MM): ";
    getline(cin, exitTime);
    if (!isValidTime(exitTime)) {
        cout << "ERROR: Invalid time format. Please use HH:MM (24-hour format).\n";
        return;
    }
    logs[index].exitTime = exitTime;

    char isCard, isOvernight;
    cout << "Do you have your parking card? (Y/N): ";
    cin >> isCard;
    cout << "Was the car parked overnight? (Y/N): ";
    cin >> isOvernight;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    float overnightRate = (isOvernight == 'Y' || isOvernight == 'y') ? OVERNIGHT_RATE : 0.0f;
    float lostCardFee   = (isCard == 'N' || isCard == 'n') ? LOST_CARD_FEE : 0.0f;

    float duration = timeToHours(logs[index].exitTime) - timeToHours(logs[index].entryTime);
    if (duration < 0) duration += 24;
    float fee = calculateParkingFee(duration, RATE_PER_HOUR, OVERTIME_RATE, overnightRate, lostCardFee);
    logs[index].fee = fee;

    cout << "+==========================================+\n";
    printCentered(cout, "EXIT SUMMARY", 45);
    cout << "+==========================================+\n";
    cout << " License Plate: " << logs[index].licensePlate << endl;
    cout << " Entry Time:    " << logs[index].entryTime << endl;
    cout << " Exit Time:     " << logs[index].exitTime << endl;
    cout << " Parking Fee:   " << fixed << setprecision(2) << fee << " Pesos" << endl;
    cout << "--------------------------------------------\n";
    cout << "Vehicle exited successfully!\n";
    occupiedSpaces--;
    cout << "Slots remaining: " << (TOTAL_SPACES - occupiedSpaces) << "\n";
}

// View parking logs
void viewLogs(ParkingLog logs[], int logCount) {
    cout << "+==========================================+\n";
    printCentered(cout, "EPEECT PARKING LOGS", 45);
    cout << "+==========================================+\n";

    if (logCount == 0) {
        cout << "No vehicles have been logged yet.\n";
        return;
    }
    printLogHeader(cout);
    for (int i = 0; i < logCount; ++i) {
        cout << left 
             << setw(5)  << i + 1
             << setw(15) << logs[i].licensePlate
             << setw(15) << logs[i].entryTime
             << setw(15) << formatExitTime(logs[i])
             << setw(15) << formatFee(logs[i])
             << endl;
    }
    cout << string(60, '-') << endl;
}

// Save logs to a file
void saveLogsToFile(ParkingLog logs[], int logCount) {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t now_time = system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&now_time);

    // Create a timestamped filename like ParkingLogs_2025-11-11_15-30.txt
    char filename[100];
    std::strftime(filename, sizeof(filename), "ParkingLogs_%Y-%m-%d_%H-%M.txt", &localTime);

    ofstream file(filename, ios::out);
    if (!file) {
        cout << "Error: Could not create log file.\n";
        return;
    }

    file << "+==========================================+\n";
    printCentered(file, "EPEECT PARKING LOGS", 45);
    file << "+==========================================+\n";

    if (logCount == 0) {
        file << "No vehicles have been logged yet.\n";
    } else {
        printLogHeader(file);
        for (int i = 0; i < logCount; ++i) {
            file << left
                 << setw(5)  << i + 1
                 << setw(15) << logs[i].licensePlate
                 << setw(15) << logs[i].entryTime
                 << setw(15) << formatExitTime(logs[i])
                 << setw(15) << formatFee(logs[i])
                 << endl;
        }
        file << string(60, '-') << endl;
    }

    file.close();
    cout << "\nParking logs saved successfully to '" << filename << "'.\n";
}
