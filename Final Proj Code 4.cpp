#include <iostream>
#include <string>   // For string handling
#include <cstdlib>  // For system("cls")    
#include <limits>   // For numeric_limits<streamsize>
#include <iomanip>  // For fixed, setw and setprecision
#include <fstream>  // For file handling    
using namespace std;

struct ParkingLog {
    string licensePlate;    // Vehicle's license plate
    string entryTime;       // Entry time of the vehicle
    string exitTime;        // Exit time of the vehicle
    float fee;              // Parking fee
};

void clearScreen();                                                                            // Declares the function to clear the console screen
void pauseProgram();                                                                           // Declares the function to pause the program                                         
void printMenu(int totalSpaces, int occupiedSpaces);                                           // Declares the function to print the menu                                                                          // Declares the function to clear the console screen
void vehicleEntry(ParkingLog logs[], int &occupiedSpaces, int totalSpaces, int &logCount);     // Declares the function for vehicle entry
void vehicleExit(ParkingLog logs[], int &occupiedSpaces, int totalSpaces, int &logCount);      // Declares the function for vehicle exit
void viewLogs(ParkingLog logs[], int logCount);                                                // Declares the function to view parking logs
void saveLogsToFile(ParkingLog logs[], int logCount);                                          // Declares the function to save logs to a file
void printLogHeader(ostream &out);                                                             // Declares the function to print log header to file
void printCentered(ostream &out, string text, int width = 40);                                 // Declares the function to print centered text
string formatFee(const ParkingLog &log);                                                       // Declares the function to format fee             
string formatExitTime(const ParkingLog &log);                                                  // Declares the function to format exit time

int main() {
    int totalSpaces = 100;          // Total parking spaces
    int occupiedSpaces = 0;         // Currently occupied spaces
    ParkingLog logs[100];           // Array to store parking logs
    int logCount = 0;               // Count of parking logs
    int choice = 0;                 // User menu choice
                 

    while (choice != 4) {                              // Loop until user chooses to exit 
        clearScreen();                                 // Clear console screen
        printMenu(totalSpaces, occupiedSpaces);        // Reprint the menu
        cin >> choice;                                 // User input for menu choice
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        if (cin.fail()) {                              // If input is not a number
            cin.clear();                               // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input
            cout << "\nInvalid input! Please enter a number (1-5).\n";
            pauseProgram();                            // Pause before re-showing menu
            continue;                                  // Skip to next loop iteration
        }
    
            switch (choice) {
                case 1: // Vehicle Entry
                    vehicleEntry(logs, occupiedSpaces, totalSpaces, logCount);   // Call vehicle entry function
                    pauseProgram();
                    break;
                case 2: // Vehicle Exit
                    vehicleExit(logs, occupiedSpaces, totalSpaces, logCount);    // Call vehicle exit function
                    pauseProgram();
                    break;
                case 3: // View Logs
                    viewLogs(logs, logCount);                                    // Call view logs function
                    pauseProgram();
                    break;
                case 4: // Exit
                    saveLogsToFile(logs, logCount);                              // Call save logs to file function
                    cout << "Exiting the program. Goodbye!\n";                   // Save logs before exiting
                    return 0;
                case 5: // Save Logs to File
                    saveLogsToFile(logs, logCount);                             // Call save logs to file function
                    pauseProgram();
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
                    pauseProgram();
                    break;
            }
    }

    return 0;
}

// Function to clear the console screen
void clearScreen() {
        system("cls");
}

// Function to print centered text
void printCentered(ostream &out, string text, int width) {
    int pad = (width - text.length()) / 2;
    out << string(pad, ' ') << text << endl;
}

// Function to pause the program until user presses Enter
void pauseProgram() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clears buffer
}

// Function to print the menu options
void printMenu(int totalSpaces, int occupiedSpaces) {
    cout << "===============================\n";
    printCentered(cout, "<EPEECT Parking Management>", 40);
    cout << "===============================\n";
    cout << "Available Spaces: " << (totalSpaces - occupiedSpaces) << "/" << totalSpaces << "\n"
         << "-------------------------------\n"
         << "1. Vehicle Entry\n"
         << "2. Vehicle Exit\n"
         << "3. View Parking Logs & Invoices\n"
         << "4. Exit\n"
         << "5. Save Logs to File\n"
         << "-------------------------------\n"
         << "Enter your choice: ";
    }

// Function to find vehicle index by license plate
int findVehicle(ParkingLog logs[], int logCount, string plate) {
    for (int i = 0; i < logCount; ++i) {
        if (logs[i].licensePlate == plate) {
            return i;
        }
    }
    return -1; // Return -1 if not found
}


// Function to convert time string "HH:MM" to hours in float
float timeToHours(string time) {
    int hours, minutes;
    sscanf(time.c_str(), "%d:%d", &hours, &minutes);
    return hours + minutes / 60.0;
}

// Function to calculate parking fee
float calculateParkingFee(float duration, float ratePerHour, float overtimeRate, float overnightRate, float lostcardfee) {
    float fee = (duration <= 3) ? ratePerHour * duration : (3 * ratePerHour + (duration - 3) * overtimeRate);
    fee += overnightRate + lostcardfee;
    return fee;
}

// Function to handle vehicle entry
void vehicleEntry(ParkingLog logs[], int &occupiedSpaces, int totalSpaces, int &logCount) {
    string plate, entryTime;
    if (occupiedSpaces < totalSpaces) {
        // License plate input
        cout << "\nEnter License Plate: ";
        getline(cin, plate);
        logs[logCount].licensePlate = plate;    // Store license plate

        // Entry time input
        cout << "Enter Entry Time (HH:MM): ";
        getline(cin, entryTime);

        logs[logCount].entryTime = entryTime;
        logs[logCount].fee = 0;
        occupiedSpaces++;
        logCount++;
        cout << "Vehicle entered successfully.\n";
        cout << "Slots remaining: " << (totalSpaces - occupiedSpaces) << "\n";
    } else {
        cout << "ERROR! Parking Full. No available spaces.\n";
    }
}

 // Function to handle vehicle exit
void vehicleExit(ParkingLog logs[], int &occupiedSpaces, int totalSpaces, int &logCount) { 
    string plate, exitTime;
    char isCard, isOvernight;           // Card and Overnight
    // Parking rates
    float ratePerHour = 20.0;       // Rate per hour
    float overtimeRate = 30.0;      // Overtime parking rate
    float overnightRate = 0;    // Overnight parking rate
    float lostCardFee = 0;      // Lost Card compensation fee
 
    cout << "\nEnter License Plate: ";
        getline(cin, plate);
    int index = findVehicle(logs, logCount, plate);  // Find the index of the vehicle
    if (index == -1) {
        cout << "ERROR: Vehicle not found in logs.\n";
        return;
    }
    // Exit time input
    cout << "Enter Exit Time (HH:MM): ";
    getline(cin, exitTime);
    logs[index].exitTime = exitTime;  // Store exit time
        
    cout << "Do you have your parking card? (Y/N): ";
    cin >> isCard;
    cout << "Was the car parked overnight? (Y/N): ";
    cin >> isOvernight;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');    // Clear input buffer
    
    if (isOvernight == 'Y' || isOvernight == 'y') {overnightRate = 200.0;    }  // Set overnight rate if applicable
    if (isCard == 'N' || isCard == 'n') {lostCardFee = 200.0;   }              // Set lost card fee if applicable  

    float duration = timeToHours(logs[index].exitTime) - timeToHours(logs[index].entryTime);
            if (duration < 0) duration += 24; // Adjust for overnight parking
    float fee = calculateParkingFee(duration, ratePerHour, overtimeRate, overnightRate, lostCardFee);

    logs[index].fee = fee;  // Store calculated fee
    cout << "Entry Time: " << logs[index].entryTime << endl;
    cout << "Exit Time: " << logs[index].exitTime << endl;
    cout << "Parking fee: ₱" << fee << endl;
    cout << "Vehicle has exited successfully.\n";
    occupiedSpaces--; // Decrease occupied spaces
    cout << "Slots remaining: " << (totalSpaces - occupiedSpaces) << "\n";
}

// Helper to print log table header
void printLogHeader(ostream &out) {
    out << left 
        << setw(5)  << "#" 
        << setw(15) << "License Plate"
        << setw(15) << "Entry Time"
        << setw(15) << "Exit Time"
        << setw(10) << "Fee (₱)"
        << endl;
    out << string(60, '-') << endl;
}

// Helper to format exit time
string formatExitTime(const ParkingLog &log) {
    return log.exitTime.empty() ? "[Still Parked]" : log.exitTime;
}

// Helper to format fee
string formatFee(const ParkingLog &log) {
    if (log.exitTime.empty()) return "—";
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.2f", log.fee);
    return string(buffer);
}

// Function to view parking logs on console
void viewLogs(ParkingLog logs[], int logCount) {
    cout << "\n=====================================================\n";
    printCentered(cout, "<EPEECT Parking Management>");
    cout << "=====================================================\n";

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
             << setw(10) << formatFee(logs[i])
             << endl;
    }

    cout << string(60, '-') << endl;
}

// Function to save logs to a file
void saveLogsToFile(ParkingLog logs[], int logCount) {
    ofstream file("ParkingLogs.txt", ios::out);
    if (!file) {
        cout << "Error: Could not create log file.\n";
        return;
    }

    file << "==============================\n";
    printCentered(file, "<EPEECT Parking Management>", 40);
    file << "==============================\n\n";

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
                 << setw(10) << formatFee(logs[i])
                 << endl;
        }
        file << string(60, '-') << endl;
    }

    file.close();
    cout << "\nParking logs saved successfully to 'ParkingLogs.txt'.\n";
}