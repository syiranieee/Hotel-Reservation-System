#define _CRT_SECURE_NO_WARNINGS 1
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <limits> 
#include <conio.h>
#include <ctime>   
#include <chrono>  
#include <sstream> 
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>

using namespace std;

// --- Database Configuration ---
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "";
const string schema = "hotelreservationsystem";


struct CurrentUser {
	int userId = 0;
	int specificId = 0;
	int roleId = 0;
	string username = "";
	string role = "";
	bool isLoggedIn = false; 
} currentUser;


// --- Function Prototypes ---
int getLastInsertID(sql::Connection* con);
string getHiddenPassword();
bool safeIntInput(int& input);
string getRoleStringById(sql::Connection* con, int roleId);
long long daysBetweenDates(const string& date1, const string& date2);
string getFloorByID(int roomId);
int getValidChoice(int min, int max);
void refreshUserPoints(sql::Connection* con, int& points, string& level);
void registerCustomer(sql::Connection* con);
void registerUser(sql::Connection* con, bool isStaff);
bool loginUser(sql::Connection* con);
void customerDashboard(sql::Connection* con);
void staffDashboard(sql::Connection* con);
void adminRoomMenu(sql::Connection* con);
void adminUserMenu(sql::Connection* con);
void adminBookingMenu(sql::Connection* con);
void syncRoomStatus(sql::Connection* con, int roomId);
void manageRooms(sql::Connection* con);
void viewAvailableRooms(sql::Connection* con, bool showPause = true);
void searchRoom(sql::Connection* con);
void updateRoomDetails(sql::Connection* con);
void deleteRoom(sql::Connection* con);
void createUserAdmin(sql::Connection* con);
void viewAllUsers(sql::Connection* con, bool showPause);
void searchUser(sql::Connection* con);
void updateUserDetails(sql::Connection* con);
void deleteUser(sql::Connection* con);
void bookRoom(sql::Connection* con);
void checkMyRewards(sql::Connection* con);
void generateReport(sql::Connection* con);
void viewAllBookings(sql::Connection* con, bool showPause);
void viewMyBookings(sql::Connection* con, bool showPause);
void updateBooking(sql::Connection* con);
void deleteBooking(sql::Connection* con); 
void searchBooking(sql::Connection* con);
void manageCheckInOut(sql::Connection* con);
void autoCleanupNoShow(sql::Connection* con);
void reportMenu(sql::Connection* con);
void roomReport(sql::Connection* con);
void bookingReport(sql::Connection* con);
void guestReport(sql::Connection* con);
void revenueAnalysisReport(sql::Connection* con);
void revenueBarChart(sql::Connection* con);
void saveReportToDB(sql::Connection* con, string type, int totalB, double totalR);
void roomPopularityReport(sql::Connection* con);

int getValidChoice(int min, int max) {
	int choice;
	while (true) {
		cout << "Enter Choice: ";
		if (safeIntInput(choice)) {
			if (choice >= min && choice <= max) {
				return choice;
			}
		}
		cout << "\n[!] Invalid input. Would you like to (1) Try again or (2) Exit to menu? ";
		int retry;
		if (safeIntInput(retry) && retry == 2) return -1;
		cout << "Please enter a valid number (" << min << "-" << max << ").\n";
	}
}

int main() {
	sql::Driver* driver;
	sql::Connection* con;
	try {
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema(schema);
		cout << "Database connection successful!" << endl;
		system("pause");

		while (true) {
			system("cls");
			cout << "===========================================\n";
			cout << "      THE HOTEL RESERVATION SYSTEM\n";
			cout << "===========================================\n";
			cout << " 1. Register\n";
			cout << " 2. Login\n";
			cout << " 3. Exit\n";

			int choice = getValidChoice(1, 3);
			if (choice == -1) continue;

			switch (choice) {
			case 1: registerCustomer(con); break;
			case 2:
				if (loginUser(con)) {
					if (currentUser.role == "Customer") customerDashboard(con);
					else staffDashboard(con);
				}
				break;
			case 3: delete con; return 0;
			}
		}
	}
	catch (sql::SQLException& e) {
		cout << "Could not connect to server. Error: " << e.what() << endl;
		system("pause");
		exit(1);
	}
	return 0;
}


bool safeIntInput(int& input) {
	if (cin >> input) {
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return true;
	}
	else {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return false;
	}
}


int getLastInsertID(sql::Connection* con) {
	sql::Statement* stmt = con->createStatement();
	sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	int id = 0;
	if (res->next()) {
		id = res->getInt(1);
	}
	delete res;
	delete stmt;
	return id;
}

string getHiddenPassword() {
	string password = "";
	char ch;
	while ((ch = _getch()) != 13) {
		if (ch == 8) {
			if (!password.empty()) {
				cout << "\b \b";
				password.pop_back();
			}
		}
		else {
			password.push_back(ch);
			cout << '*';
		}
	}
	cout << endl;
	return password;
}


string getRoleStringById(sql::Connection* con, int roleId) {
	try {
		sql::PreparedStatement* pstmt = con->prepareStatement("SELECT Position FROM ROLE WHERE RoleID = ?");
		pstmt->setInt(1, roleId);
		sql::ResultSet* res = pstmt->executeQuery();
		string role = "";
		if (res->next()) {
			role = res->getString("Position");
		}
		delete res;
		delete pstmt;
		return role;
	}
	catch (sql::SQLException& e) {
		return "Unknown";
	}
}

string getFloorByID(int roomId) {
	if (roomId >= 1 && roomId <= 9) return "1";
	else if (roomId >= 10 && roomId <= 18) return "2";
	else if (roomId >= 19 && roomId <= 27) return "3";
	else if (roomId >= 28 && roomId <= 36) return "4";
	else if (roomId >= 37 && roomId <= 45) return "5";
	else if (roomId >= 46 && roomId <= 54) return "6";
	else return "G";
}

long long daysBetweenDates(const string& date1, const string& date2) {		// date format: "YYYY-MM-DD"
	tm t1 = {};
	tm t2 = {};
	istringstream ss1(date1);
	istringstream ss2(date2);

	if (!(ss1 >> get_time(&t1, "%Y-%m-%d")) || !(ss2 >> get_time(&t2, "%Y-%m-%d"))) {
		return -1;
	}

	chrono::system_clock::time_point tp1 = chrono::system_clock::from_time_t(mktime(&t1));
	chrono::system_clock::time_point tp2 = chrono::system_clock::from_time_t(mktime(&t2));

	chrono::duration<long long, ratio<86400>> diff = chrono::duration_cast<chrono::duration<long long, ratio<86400>>>(tp2 - tp1);		// days

	return diff.count();
}


void registerCustomer(sql::Connection* con) {
	system("cls");
	cout << "===========================================\n";
	cout << "            REGISTRATION TYPE\n";
	cout << "===========================================\n";
	cout << " 1. Customer Registration\n";
	cout << " 2. Staff Registration\n";
	cout << " 3. Cancel\n";
	cout << "Enter Choice: ";

	int choice;
	if (safeIntInput(choice)) {
		switch (choice) {
		case 1:
			registerUser(con, false); // register as customer
			break;
		case 2:
			registerUser(con, true); // register as staff
			break;
		case 3:
			cout << "Registration cancelled.\n";
			system("pause");
			break;
		default:
			cout << "Invalid choice. Returning to main menu.\n";
			system("pause");
		}
	}
	else {
		cout << "Invalid input. Returning to main menu.\n";
		system("pause");
	}
}



void registerUser(sql::Connection* con, bool isStaff) {
	system("cls");
	string user, pass, name, ic, email, phone;
	string staffPosition = "", staffShiftTime = "";
	string rolePosition = "Customer";

	if (isStaff) {
		bool validRole = false;
		while (!validRole) {
			cout << "===========================================\n";
			cout << "        REGISTER NEW STAFF ACCOUNT\n";
			cout << "===========================================\n";
			cout << "Select Staff Position:\n";
			cout << " 1. Receptionist\n";
			cout << " 2. Manager\n";
			cout << " 3. Staff\n";
			cout << "Choice: ";

			int roleChoice;
			if (safeIntInput(roleChoice)) {
				if (roleChoice == 0) {
					return; 
				}
				else if (roleChoice == 1) {
					rolePosition = "Receptionist";
					validRole = true;
				}
				else if (roleChoice == 2) {
					rolePosition = "Manager";
					validRole = true;
				}
				else if (roleChoice == 3) {
					rolePosition = "Staff";
					validRole = true;
				}
				else {
					cout << "\n[!] Invalid choice. Please select 1, 2, or 3.\n";
					system("pause");
					system("cls");
				}
			}
			else {
				cout << "\n[!] Invalid input. Please enter a number.\n";
				system("pause");
				system("cls");
			}
		}
		staffPosition = rolePosition;
	}
	else {
		cout << "===========================================\n";
		cout << "      REGISTER NEW CUSTOMER ACCOUNT\n";
		cout << "===========================================\n";
	}

	cout << "Username: "; cin >> user;
	cout << "Password: "; pass = getHiddenPassword();


	while (true) {
		cout << "Phone Number (10-11 digits): "; cin >> phone;
		if (phone.length() >= 10 && phone.length() <= 11) break;
		cout << "[!] Invalid phone number. Length must be 10 or 11.\n";
	}


	while (true) {
		cout << "Email: "; cin >> email;
		if (email.find('@') != string::npos && email.find('.') != string::npos) break;
		cout << "[!] Invalid email format. Must contain '@' and a domain (e.g., .com).\n";
	}

	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	if (!isStaff) {
		cout << "Full Name: "; getline(cin, name);

		while (true) {
			cout << "IC Number (12 digits): "; cin >> ic;
			if (ic.length() == 12) break;
			cout << "[!] Invalid IC. Must be exactly 12 digits.\n";
		}
	}
	else {
		cout << "Staff Name: "; getline(cin, name);
		cout << "Staff Shift Time: "; getline(cin, staffShiftTime);

		while (true) {
			cout << "IC Number (Staff - 12 digits): "; cin >> ic;
			if (ic.length() == 12) break;
			cout << "[!] Invalid IC. Must be exactly 12 digits.\n";
		}
	}

	try {

		con->setAutoCommit(false);

		sql::PreparedStatement* pstmt;
		sql::ResultSet* res;

		int roleId = 0;
		pstmt = con->prepareStatement("SELECT RoleID FROM ROLE WHERE Position = ?");
		pstmt->setString(1, rolePosition);
		res = pstmt->executeQuery();
		if (res->next()) {
			roleId = res->getInt("RoleID");
		}
		delete res;
		delete pstmt;

		if (roleId == 0) {
			throw runtime_error("Role '" + rolePosition + "' not found in database.");
		}

		// check if username already exists
		pstmt = con->prepareStatement("SELECT User_ID FROM USER WHERE Username = ?");
		pstmt->setString(1, user);
		res = pstmt->executeQuery();
		if (res->next()) {
			cout << "\n[ERROR] Username '" << user << "' already exists!\n";
			delete res;
			delete pstmt;
			con->rollback(); // cancel transaction
			system("pause");
			return;
		}
		delete res;
		delete pstmt;

		pstmt = con->prepareStatement("INSERT INTO USER (Username, Password, RoleID, User_Email, User_PhoneNumber) VALUES (?, ?, ?, ?, ?)");
		pstmt->setString(1, user);
		pstmt->setString(2, pass);
		pstmt->setInt(3, roleId);
		pstmt->setString(4, email);
		pstmt->setString(5, phone);
		pstmt->executeUpdate();
		delete pstmt;

		int newUserId = getLastInsertID(con);

		if (!isStaff) {
			pstmt = con->prepareStatement("INSERT INTO CUSTOMER (User_Id, Name, IC_Number) VALUES (?, ?, ?)");
			pstmt->setInt(1, newUserId);
			pstmt->setString(2, name);
			pstmt->setString(3, ic);
			pstmt->executeUpdate();
			delete pstmt;

			int newCustomerId = getLastInsertID(con);

			pstmt = con->prepareStatement("INSERT INTO REWARD (Customer_ID, Points, Level) VALUES (?, 0, 'Bronze')");
			pstmt->setInt(1, newCustomerId);
			pstmt->executeUpdate();
			delete pstmt;

			cout << "\nCustomer registration successful!\n";
		}
		else {
			pstmt = con->prepareStatement("INSERT INTO STAFF (User_ID, StaffName, StaffPosition, StaffShiftTime) VALUES (?, ?, ?, ?)");
			pstmt->setInt(1, newUserId);
			pstmt->setString(2, name);
			pstmt->setString(3, staffPosition);
			pstmt->setString(4, staffShiftTime);
			pstmt->executeUpdate();
			delete pstmt;

			cout << "\nStaff registration successful!\n";
		}

		con->commit();
		con->setAutoCommit(true);
		cout << "User ID assigned: " << newUserId << "\n";
	}
	catch (sql::SQLException& e) {
		con->rollback(); // undo everything if database fails
		con->setAutoCommit(true);
		cout << "\n[DATABASE ERROR] Registration failed: " << e.what() << endl;
	}
	catch (exception& e) {
		con->rollback();
		con->setAutoCommit(true);
		cout << "\n[ERROR] " << e.what() << endl;
	}
	system("pause");
}

bool loginUser(sql::Connection* con) {
	int attempts = 0;
	const int MAX_ATTEMPTS = 3;

	while (attempts < MAX_ATTEMPTS) {
		system("cls");
		string user, pass;

		cout << "===========================================\n";
		cout << "                USER LOGIN                 \n";
		cout << "===========================================\n";
		cout << "Username: "; cin >> user;
		cout << "Password: "; pass = getHiddenPassword();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		try {
			sql::PreparedStatement* pstmt;
			sql::ResultSet* res;
				
			pstmt = con->prepareStatement("SELECT User_ID, RoleID, Username, Password FROM USER WHERE Username = ?");		// fetch user record by username
			pstmt->setString(1, user);
			res = pstmt->executeQuery();

			if (res->next()) {
				string storedPassword = res->getString("Password");

				if (pass == storedPassword) {
					currentUser.userId = res->getInt("User_ID");
					currentUser.roleId = res->getInt("RoleID");
					currentUser.username = res->getString("Username");
					currentUser.role = getRoleStringById(con, currentUser.roleId);

					delete res; delete pstmt;

					if (currentUser.role == "Customer") {
						pstmt = con->prepareStatement("SELECT Customer_ID FROM CUSTOMER WHERE User_Id = ?");		// get specific customer ID
						pstmt->setInt(1, currentUser.userId);														// set parameter
						res = pstmt->executeQuery();																// execute query
						if (res->next()) currentUser.specificId = res->getInt("Customer_ID");						// fetch customer ID
					}
					else {
						pstmt = con->prepareStatement("SELECT Staff_ID FROM STAFF WHERE User_ID = ?");				// get specific staff ID
						pstmt->setInt(1, currentUser.userId);
						res = pstmt->executeQuery();
						if (res->next()) currentUser.specificId = res->getInt("Staff_ID");
					}

					delete res; delete pstmt;
					currentUser.isLoggedIn = true;

					string dashboardType = (currentUser.role == "Customer") ? "Customer Dashboard" : "Staff Dashboard";

					cout << "\n[SUCCESS] Login Successful! Welcome, " << currentUser.username << ".\n";
					cout << "Logged in as: " << currentUser.role << " - Redirecting to " << dashboardType << ".\n\n";

					system("pause");
					return true;
				}
			}


			attempts++;
			delete res; delete pstmt;

			if (attempts < MAX_ATTEMPTS) {
				cout << "\n[!] Invalid Username or Password.\n";
				cout << "\n1. Try again";
				cout << "\n2. Return to Main Menu ";
				cout << "\nChoose an option: ";
				int retryChoice;
				if (safeIntInput(retryChoice) && retryChoice == 2) {
					return false; 
				}
			}
		}
		catch (sql::SQLException& e) {
			cout << "Error: " << e.what() << endl;
			system("pause");
			return false;
		}
	}


	cout << "\n===========================================\n";
	cout << " [!] SECURITY ALERT: TOO MANY ATTEMPTS\n";
	cout << "===========================================\n";
	cout << "You have reached the maximum limit of " << MAX_ATTEMPTS << " attempts.\n";
	cout << "Access denied. Returning to Main Menu.\n";
	system("pause");
	return false;
}

void refreshUserPoints(sql::Connection* con, int& points, string& level) {
	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"SELECT Points, Level FROM REWARD WHERE Customer_ID = ?"		// fetch reward points and level for current customer
		);
		pstmt->setInt(1, currentUser.specificId);
		sql::ResultSet* res = pstmt->executeQuery();

		if (res->next()) {								// if record found, update points and level
			points = res->getInt("Points");
			level = res->getString("Level");
		}
		delete res;
		delete pstmt;
	}
	catch (sql::SQLException& e) {

	}
}


void customerDashboard(sql::Connection* con) {
	while (currentUser.isLoggedIn) {
		system("cls");


		int pointsFromDB = 0;
		string levelFromDB = "Bronze";

		try {
			sql::PreparedStatement* pstmt = con->prepareStatement(
				"SELECT Points, Level FROM REWARD WHERE Customer_ID = ?"		// fetch reward points and level for current customer
			);

			pstmt->setInt(1, currentUser.specificId);
			sql::ResultSet* res = pstmt->executeQuery();

			if (res->next()) {
				pointsFromDB = res->getInt("Points");
				levelFromDB = res->getString("Level");
			}
			delete res;
			delete pstmt;
		}
		catch (sql::SQLException& e) {		// handle exception if needed

		}


		cout << "====================================================\n";
		cout << "                CUSTOMER DASHBOARD\n";
		cout << "====================================================\n";
		cout << " Welcome, " << currentUser.username << "!\n";
		cout << " Membership Tier : [" << levelFromDB << "]\n";
		cout << " Reward Points   : " << pointsFromDB << " pts\n";
		cout << "----------------------------------------------------\n";
		cout << " 1. Book a Room\n";
		cout << " 2. View Available Rooms\n";
		cout << " 3. Check Rewards & Benefits\n";
		cout << " 4. View My Booking History\n";
		cout << " 5. Cancel a Booking\n";
		cout << " 6. Logout\n";
		cout << "----------------------------------------------------\n";
		cout << "Enter your choice: ";

		int choice;
		if (safeIntInput(choice)) {
			switch (choice) {
			case 1:
				bookRoom(con);

				break;
			case 2:
				viewAvailableRooms(con, true);
				break;
			case 3:
				checkMyRewards(con);
				break;
			case 4:
				viewMyBookings(con, true);
				break;
			case 5:
				deleteBooking(con);
				break;
			case 6:
				currentUser.isLoggedIn = false;
				cout << "Logging out...\n";
				system("pause");
				break;
			default:
				cout << "Invalid choice. Please try again.\n";
				system("pause");
			}
		}
	}
}


void staffDashboard(sql::Connection* con) {

	autoCleanupNoShow(con);

	while (currentUser.isLoggedIn) {
		system("cls");
		string userTitle = (currentUser.username == "admin") ? "ADMINISTRATOR" : currentUser.role;			// Special title for admin user


		cout << "=============================================\n";
		cout << "              STAFF DASHBOARD                     ";
		cout << "\n=============================================\n";
		cout << "              Welcome " << currentUser.username << "\n";
		cout << "---------------------------------------------\n";
		cout << " 1. Manage Rooms\n";
		cout << " 2. Manage Users\n";
		cout << " 3. Manage Bookings\n";
		cout << " 4. Reports\n";
		cout << " 5. Logout\n";
		cout << "---------------------------------------------\n";
		cout << "Enter your choice: ";

		int choice;
		if (safeIntInput(choice)) {
			switch (choice) {
			case 1: adminRoomMenu(con); break;
			case 2: adminUserMenu(con); break;
			case 3: adminBookingMenu(con); break;
			case 4: reportMenu(con); break;
			case 5: currentUser.isLoggedIn = false; break;
			default: cout << "Invalid.\n"; system("pause");
			}
		}
		else {
			cout << "Invalid input. Please enter a number.\n"; system("pause");
		}
	}
}

void adminRoomMenu(sql::Connection* con) {
	bool inMenu = true;
	while (inMenu) {
		system("cls");
		cout << "===========================================\n";
		cout << "             MANAGE ROOMS\n";
		cout << "===========================================\n";
		cout << " 1. Add New Room\n";
		cout << " 2. View All Rooms\n";
		cout << " 3. Search Room\n";
		cout << " 4. Update Room\n";
		cout << " 5. Delete Room\n";
		cout << " 6. Back to Main Menu\n";
		cout << "-------------------------------------------\n";
		cout << "Enter your choice: ";

		int choice;
		if (safeIntInput(choice)) {
			switch (choice) {
			case 1: manageRooms(con); break;
			case 2: viewAvailableRooms(con); break;
			case 3: searchRoom(con); break;
			case 4: updateRoomDetails(con); break;
			case 5: deleteRoom(con); break;
			case 6: inMenu = false; break;
			default: cout << "Invalid.\n"; system("pause");
			}
		}
		else {
			cout << "Invalid input. Please enter a number.\n"; system("pause");
		}
	}
}

void adminUserMenu(sql::Connection* con) {
	bool inMenu = true;
	while (inMenu) {
		system("cls");
		cout << "===========================================\n";
		cout << "            MANAGE USERS\n";
		cout << "===========================================\n";
		cout << " 1. Add New User\n";
		cout << " 2. View All Users \n";
		cout << " 3. Search User\n";
		cout << " 4. Update User\n";
		cout << " 5. Delete User\n";
		cout << " 6. Back to Main Menu\n";
		cout << "-------------------------------------------\n";
		cout << "Enter your choice: ";

		int choice;
		if (safeIntInput(choice)) {
			switch (choice) {
			case 1: createUserAdmin(con); break;
			case 2: viewAllUsers(con, true); break;
			case 3: searchUser(con); break;
			case 4: updateUserDetails(con); break;
			case 5: deleteUser(con); break;
			case 6: inMenu = false; break;
			default: cout << "Invalid.\n"; system("pause");
			}
		}
		else {
			cout << "Invalid input. Please enter a number.\n"; system("pause");
		}
	}
}


void adminBookingMenu(sql::Connection* con) {
	bool inMenu = true;
	while (inMenu) {
		system("cls");
		cout << "===========================================\n";
		cout << "            MANAGE BOOKINGS\n";
		cout << "===========================================\n";
		cout << " 1. Add New Booking \n";
		cout << " 2. Search Booking\n";
		cout << " 3. Delete/Cancel Booking\n";
		cout << " 4. View All Bookings\n";
		cout << " 5. Check-In / Check-Out\n"; 
		cout << " 6. Back to Main Menu\n";
		cout << "-------------------------------------------\n";
		cout << "Enter your choice: ";

		int choice;
		if (safeIntInput(choice)) {
			switch (choice) {
			case 1: bookRoom(con); break;
			case 2: searchBooking(con); break;
			case 3: deleteBooking(con); break;
			case 4: viewAllBookings(con, true); break;
			case 5: manageCheckInOut(con); break; 
			case 6: inMenu = false; break;
			default: cout << "Invalid.\n"; system("pause");
			}
		}
		else {
			cout << "Invalid input. Please enter a number.\n"; system("pause");
		}
	}
}


// --- ROOM MANAGEMENT ---
void syncRoomStatus(sql::Connection* con, int roomId) {
	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"SELECT (R.TotalQuantity - COUNT(B.Booking_ID)) AS Remaining, R.RoomStatus "	//calculate remaining rooms
			"FROM ROOM R "																	
			"LEFT JOIN BOOKING B ON R.Room_ID = B.Room_ID AND B.BookingStatus = 'Active' "	//only consider active bookings
			"WHERE R.Room_ID = ? GROUP BY R.Room_ID"										//group by to get correct count
		);
		pstmt->setInt(1, roomId);
		sql::ResultSet* res = pstmt->executeQuery();

		if (res->next()) {
			int remaining = res->getInt("Remaining");
			string currentStatus = res->getString("RoomStatus");
			string newStatus = currentStatus;

			if (currentStatus != "Maintenance") {
				newStatus = (remaining <= 0) ? "Full" : "Available";
			}

			sql::PreparedStatement* updateStmt = con->prepareStatement(
				"UPDATE ROOM SET RoomStatus = ? WHERE Room_ID = ?");
			updateStmt->setString(1, newStatus);
			updateStmt->setInt(2, roomId);
			updateStmt->executeUpdate();
			delete updateStmt;
		}
		delete res; delete pstmt;
	}
	catch (sql::SQLException& e) {
		cout << "Error syncing room status: " << e.what() << endl;
	}
}


void manageRooms(sql::Connection* con) {
	system("cls");
	string type, desc, facilities;
	double price;
	int qty;

	cout << "===========================================\n";
	cout << "                ADD NEW ROOM               \n";
	cout << "===========================================\n";
	cout << "Enter Room Type: "; getline(cin, type);

	try {
		sql::PreparedStatement* checkPstmt = con->prepareStatement("SELECT RoomType FROM ROOM WHERE RoomType = ?"); //check if room type already exists
		checkPstmt->setString(1, type);
		sql::ResultSet* res = checkPstmt->executeQuery();

		if (res->next()) {
			cout << "\n[ERROR] Room Type '" << type << "' already exists in the system!\n";
			delete res;
			delete checkPstmt;
		}
		else {

			delete res;
			delete checkPstmt;

			cout << "Enter Price: "; cin >> price;
			cout << "Enter Total Units: "; cin >> qty;
			cin.ignore();
			cout << "Enter Description: "; getline(cin, desc);
			cout << "Enter Facilities: "; getline(cin, facilities);

			sql::PreparedStatement* pstmt = con->prepareStatement(
				"INSERT INTO ROOM (RoomType, RoomPrice, TotalQuantity, RoomStatus, RoomDescription, Facilities) "
				"VALUES (?, ?, ?, 'Available', ?, ?)");

			pstmt->setString(1, type);
			pstmt->setDouble(2, price);
			pstmt->setInt(3, qty);
			pstmt->setString(4, desc);
			pstmt->setString(5, facilities);

			pstmt->executeUpdate();
			cout << "\n[SUCCESS] Room Category " << type << " added successfully.\n";
			delete pstmt;
		}
	}
	catch (sql::SQLException& e) {
		cout << "Database Error: " << e.what() << endl;
	}
	system("pause");
}

void viewAvailableRooms(sql::Connection* con, bool showPause) {
	system("cls");
	bool isStaff = (currentUser.role != "Customer");
	try {
		sql::Statement* stmt = con->createStatement();
		if (!isStaff) {
			sql::ResultSet* res = stmt->executeQuery(
				"SELECT R.Room_ID, R.RoomType, R.RoomPrice, R.RoomDescription, R.Facilities, "			//query to show only available rooms for booking
				"(R.TotalQuantity - COUNT(B.Booking_ID)) AS LeftCount "
				"FROM ROOM R "
				"LEFT JOIN BOOKING B ON R.Room_ID = B.Room_ID AND B.BookingStatus = 'Confirmed' "		//leftCount to calculate available rooms (total - booked)
				"GROUP BY R.Room_ID HAVING LeftCount > 0"
			);
			cout << "===========================================================================================================================================================\n";
			cout << "                                                                AVAILABLE ROOMS FOR BOOKING                                                                \n";
			cout << "===========================================================================================================================================================\n";
			cout << left << setw(5) << "ID" << setw(20) << "Room Type" << setw(12) << "Price" << setw(60) << "Description" << "Facilities" << endl;
			cout << "-----------------------------------------------------------------------------------------------------------------------------------------------------------\n";
			while (res->next()) {
				cout << left << setw(5) << res->getInt("Room_ID") << setw(20) << res->getString("RoomType") << "RM" << setw(10) << fixed << setprecision(2) << res->getDouble("RoomPrice")
					<< setw(60) << res->getString("RoomDescription") << res->getString("Facilities") << endl;
			}
			delete res;
		}
		else {
			sql::ResultSet* res = stmt->executeQuery(
				"SELECT R.Room_ID, R.RoomType, R.RoomPrice, R.TotalQuantity, COUNT(B.Booking_ID) AS BookedCount, "
				"(R.TotalQuantity - COUNT(B.Booking_ID)) AS AvailableCount FROM ROOM R "
				"LEFT JOIN BOOKING B ON R.Room_ID = B.Room_ID AND B.BookingStatus = 'Confirmed' GROUP BY R.Room_ID"
			);
			cout << "============================================================================================\n";
			cout << "                                   ROOM INVENTORY STATUS                                    \n";
			cout << "============================================================================================\n";
			cout << left << setw(5) << "ID" << setw(20) << "Room Type" << setw(10) << "Price" << setw(10) << "Total" << setw(10) << "Booked" << setw(12) << "Available" << endl;
			cout << "--------------------------------------------------------------------------------------------\n";
			while (res->next()) {
				cout << left << setw(5) << res->getInt("Room_ID") << setw(20) << res->getString("RoomType") << "RM" << setw(8) << res->getDouble("RoomPrice")
					<< setw(10) << res->getInt("TotalQuantity") << setw(10) << res->getInt("BookedCount") << setw(12) << res->getInt("AvailableCount") << endl;
			}
			delete res;
		}
		delete stmt;
	}
	catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }

	if (showPause) 
		system("pause");
}

//  Search Room 
void searchRoom(sql::Connection* con) {
	system("cls");
	string keyword;
	cout << "====================================================================\n";
	cout << "                        HOTEL ROOM SEARCH                           \n";
	cout << "====================================================================\n";

	cout << "\nSearch by Room Type Keyword\n";
	cout << "Enter Room Type: ";

	if (cin.peek() == '\n') cin.ignore();

	getline(cin, keyword);

	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"SELECT * FROM ROOM WHERE RoomType LIKE ?");
		pstmt->setString(1, "%" + keyword + "%");
		sql::ResultSet* res = pstmt->executeQuery();

		bool found = false;


		cout << "\nSearch results for '" << keyword << "':\n";
		cout << "--------------------------------------------------------------------\n";
		cout << left << setw(5) << "ID" << setw(20) << "Room Type" << setw(15) << "Price (RM)" << "Status" << endl;
		cout << "--------------------------------------------------------------------\n";

		while (res->next()) {
			found = true;
			cout << left << setw(5) << res->getInt("Room_ID")
				<< setw(20) << res->getString("RoomType")
				<< "RM " << setw(12) << fixed << setprecision(2) << res->getDouble("RoomPrice")
				<< "[" << res->getString("RoomStatus") << "]" << endl;
		}

		if (!found) {
			cout << "\n [!] SORRY: No rooms matching '" << keyword << "' were found.\n";
			cout << "     Please check your spelling or try a different keyword.\n";
		}
		cout << "--------------------------------------------------------------------\n";

		delete res;
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		cout << "\n [DATABASE ERROR]: " << e.what() << endl;
	}

	cout << "\n";
	system("pause");
}

//Update Room Details
void updateRoomDetails(sql::Connection* con) {
	bool continueUpdating = true; 

	do {
		system("cls");
		int id;


		viewAvailableRooms(con, false);

		cout << "\n===========================================\n";
		cout << "                UPDATE ROOM                \n";
		cout << "===========================================\n";
		cout << "Enter Room Category ID to Update: ";
		if (!safeIntInput(id)) return;

		try {

			sql::PreparedStatement* checkPstmt = con->prepareStatement(
				"SELECT RoomType, RoomPrice, TotalQuantity, RoomStatus FROM ROOM WHERE Room_ID = ?"
			);
			checkPstmt->setInt(1, id);
			sql::ResultSet* res = checkPstmt->executeQuery();

			if (!res->next()) {
				cout << "\n[!] ERROR: Room ID " << id << " was not found.\n";
				delete res; delete checkPstmt;
				system("pause");
				return;
			}

			string currentType = res->getString("RoomType");
			string currentStatus = res->getString("RoomStatus");
			int currentTotal = res->getInt("TotalQuantity");
			delete res; delete checkPstmt;


			int currentlyBooked = 0;
			sql::PreparedStatement* bookPstmt = con->prepareStatement(
				"SELECT COUNT(*) FROM BOOKING WHERE Room_ID = ? AND BookingStatus = 'Confirmed'"
			);
			bookPstmt->setInt(1, id);
			sql::ResultSet* resBook = bookPstmt->executeQuery();
			if (resBook->next()) currentlyBooked = resBook->getInt(1);
			delete resBook; delete bookPstmt;

			cout << "\nSelected: " << currentType << " [" << currentStatus << "]";
			cout << "\n-------------------------------------------";
			cout << "\n 1. Update Price";
			cout << "\n 2. Update Total Quantity";
			cout << "\n 3. Room Maintenance";
			cout << "\n 4. Exit/Cancel";
			cout << "\n-------------------------------------------";
			cout << "\nChoice: ";

			int choice = getValidChoice(1, 4);
			if (choice == -1 || choice == 4) return;

			sql::PreparedStatement* pstmt = nullptr;

			if (choice == 1) {
				double p;
				cout << "Enter New Price: RM";
				cin >> p;
				pstmt = con->prepareStatement("UPDATE ROOM SET RoomPrice = ? WHERE Room_ID = ?");
				pstmt->setDouble(1, p);
				pstmt->setInt(2, id);
			}
			else if (choice == 2) {
				int subChoice, amount;
				cout << "\n--- Inventory Adjustment ---\n";
				cout << " 1. Increase Stock\n 2. Decrease Stock\nChoice: ";
				cin >> subChoice;
				if (subChoice == 1) {
					cout << "Add quantity: "; cin >> amount;
					pstmt = con->prepareStatement("UPDATE ROOM SET TotalQuantity = TotalQuantity + ? WHERE Room_ID = ?");
					pstmt->setInt(1, amount);
					pstmt->setInt(2, id);
				}
				else if (subChoice == 2) {
					cout << "Remove quantity: "; cin >> amount;
					if ((currentTotal - amount) < currentlyBooked) {
						cout << "\n[ERROR] Cannot remove. Rooms are currently occupied by guests.\n";
						system("pause"); continue;
					}
					pstmt = con->prepareStatement("UPDATE ROOM SET TotalQuantity = TotalQuantity - ? WHERE Room_ID = ?");
					pstmt->setInt(1, amount);
					pstmt->setInt(2, id);
				}
			}
			else if (choice == 3) {
				cout << "===========================================\n";
				cout << "                MAINTENANCE                \n";
				cout << "===========================================\n";


				cout << "Current Status: " << currentStatus << endl;
				cout << "-------------------------------------------\n";
				cout << " 1. Put Category to Maintenance (OFFLINE)\n";
				cout << " 2. Put Specific Units to Maintenance\n";
				cout << " 3. Restore Category to 'Available' (ONLINE)\n"; 
				cout << " 4. Cancel\n";
				cout << "-------------------------------------------\n";
				cout << "Choice: ";

				int mChoice; cin >> mChoice;

				if (mChoice == 1) {

					pstmt = con->prepareStatement("UPDATE ROOM SET RoomStatus = 'Maintenance' WHERE Room_ID = ?");
					pstmt->setInt(1, id);
					cout << "\n[!] ALERT: Category is now OFFLINE for maintenance.\n";
				}
				else if (mChoice == 2) {
					int mQty;
					cout << "How many units need maintenance?: "; cin >> mQty;


					if (mQty > (currentTotal - currentlyBooked)) {
						cout << "\n[ERROR] Only " << (currentTotal - currentlyBooked) << " vacant units available.\n";
						system("pause"); continue;
					}

					cout << "\n[SYSTEM] Work order created for " << mQty << " units.";
					cout << "\n[INFO] Category remains 'Available' in system for the remaining units.\n";
				}
				else if (mChoice == 3) {

					pstmt = con->prepareStatement("UPDATE ROOM SET RoomStatus = 'Available' WHERE Room_ID = ?");
					pstmt->setInt(1, id);
					cout << "\n[SUCCESS] Room category has been restored to 'Available'.\n";
				}
				else {
					cout << "\nOperation cancelled.\n";
				}
			}


			if (pstmt) {
				pstmt->executeUpdate();
				delete pstmt;
				syncRoomStatus(con, id);
				cout << "\n[SYSTEM] Update completed successfully.\n";
			}


			cout << "\nWould you like to update another room record?";
			cout << "\n[1] Yes, continue updating";
			cout << "\n[2] No, return to main menu";
			cout << "\nChoice: ";
			int loopChoice;
			cin >> loopChoice;

			if (loopChoice != 1) {
				continueUpdating = false;
			}

		}
		catch (sql::SQLException& e) {
			cout << "\n[DB ERROR] " << e.what() << endl;
			system("pause");
			continueUpdating = false;
		}

	} while (continueUpdating); 
}
// Delete Room

void deleteRoom(sql::Connection* con) {
	system("cls");
	int catId, choice;

	cout << "=========================================================" << endl;
	cout << "                       CURRENT ROOM                    " << endl;
	cout << "=========================================================" << endl;
	viewAvailableRooms(con, false);

	cout << "\n------------------- DELETE OPTIONS ---------------------" << endl;
	cout << "Enter the Room ID: ";
	if (!safeIntInput(catId)) return;


	cout << "\nWhat would you like to do with this ID?" << endl;
	cout << "1. Remove Units " << endl;
	cout << "2. Delete Entire Room Type " << endl;
	cout << "Enter choice: ";
	cin >> choice;

	if (choice == 1) {

		int qtyToDelete;
		cout << "How many units would you like to remove?: ";
		if (!(cin >> qtyToDelete) || qtyToDelete <= 0) {
			cout << "\n[ERROR] Invalid quantity.\n";
			return;
		}

		sql::PreparedStatement* pstmt = con->prepareStatement(
			"UPDATE ROOM SET TotalQuantity = TotalQuantity - ? WHERE Room_ID = ? AND TotalQuantity >= ?"
		);
		pstmt->setInt(1, qtyToDelete);
		pstmt->setInt(2, catId);
		pstmt->setInt(3, qtyToDelete);

		if (pstmt->executeUpdate() > 0)
			cout << "\n[SUCCESS] Units removed successfully.";
		else
			cout << "\n[FAIL] Not enough units or ID not found.";
		delete pstmt;

	}
	else if (choice == 2) {

		cout << "\n[WARNING] This will permanently remove the room type! Confirm? (y/n): ";
		char confirm;
		cin >> confirm;

		if (tolower(confirm) == 'y') {
			try {
				sql::PreparedStatement* pstmt = con->prepareStatement(
					"DELETE FROM ROOM WHERE Room_ID = ?"
				);
				pstmt->setInt(1, catId);

				if (pstmt->executeUpdate() > 0)
					cout << "\n[SUCCESS] Room type deleted from system.";
				else
					cout << "\n[FAIL] Category ID not found.";
				delete pstmt;
			}
			catch (sql::SQLException& e) {
				cout << "\n[ERROR] Cannot delete: Check if there are active bookings linked to this room.";
			}
		}
	}
	else {
		cout << "\nInvalid choice. Operation cancelled.";
	}

	cout << endl;
	system("pause");
}
// --- USER MANAGEMENT ---

void createUserAdmin(sql::Connection* con)
{
	system("cls");
	cout << "===========================================\n";
	cout << "            CREATE NEW USER\n";
	cout << "===========================================\n";
	cout << " 1. Customer\n";
	cout << " 2. Staff\n";          
	cout << " 3. Staff (Manager)\n";
	cout << "-------------------------------------------\n";
	cout << "Enter Choice: ";

	int choice;
	if (!safeIntInput(choice) || (choice < 1 || choice > 3)) {
		cout << "Invalid choice. Returning to menu.\n";
		system("pause");
		return;
	}

	if (choice == 1) {
		registerUser(con, false); 
	}
	else {

		registerUser(con, true);
	}
}



void viewAllUsers(sql::Connection* con, bool showPause) {
	system("cls");
	cout << "====================================================================================================\n";
	cout << "                                     VIEW ALL USER                                       \n";
	cout << "====================================================================================================\n";

	auto displayUsers = [&](const string& roleFilter, const string& sectionHeader) {
		cout << "\n>>> " << sectionHeader << "\n";
		cout << left << setw(6) << "UID" << setw(15) << "USERNAME" << setw(18) << "ROLE"
			<< setw(22) << "EMAIL" << setw(15) << "PHONE" << "NAME" << endl;
		cout << "----------------------------------------------------------------------------------------------------\n";
		try {

			string query = (roleFilter == "Customer") ?
				"SELECT u.User_ID, u.Username, r.Position, u.User_Email, u.User_PhoneNumber, c.Name "
				"FROM USER u JOIN ROLE r ON u.RoleID = r.RoleID JOIN CUSTOMER c ON u.User_ID = c.User_Id" :
				"SELECT u.User_ID, u.Username, r.Position, u.User_Email, u.User_PhoneNumber, s.StaffName "
				"FROM USER u JOIN ROLE r ON u.RoleID = r.RoleID JOIN STAFF s ON u.User_ID = s.User_ID "
				"WHERE LOWER(r.Position) != 'customer'";

			sql::Statement* stmt = con->createStatement();
			sql::ResultSet* res = stmt->executeQuery(query);
			while (res->next()) {
				cout << left << setw(6) << res->getInt(1)
					<< setw(15) << res->getString(2)
					<< setw(18) << res->getString(3)
					<< setw(22) << res->getString(4)
					<< setw(15) << res->getString(5)
					<< res->getString(6) << endl;
			}
			delete res; delete stmt;
		}
		catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
		cout << "----------------------------------------------------------------------------------------------------\n";
		};

	displayUsers("Staff", "STAFF & MANAGEMENT PROFILES");
	displayUsers("Customer", "REGISTERED GUEST PROFILES");

	if (showPause) system("pause");
}


void searchUser(sql::Connection* con) {
	system("cls");
	cout << "===========================================\n";
	cout << "              SEARCH USER\n";
	cout << "===========================================\n";

	string input;
	int userId = 0;
	string keyword;

	cout << "\nSearch by User ID or Name Keyword\n";
	cout << "Enter User ID or Name: ";
	getline(cin, input);

	bool isNumber = true;
	for (char c : input) {
		if (!isdigit(c)) {
			isNumber = false;
			break;
		}
	}

	if (isNumber && !input.empty()) {
		userId = stoi(input);
	}
	else {
		keyword = input;
	}

	try {
		sql::PreparedStatement* pstmt = nullptr;
		sql::ResultSet* res = nullptr;
		bool found = false;

		const string baseQuery =												//base query to fetch user details
			"SELECT u.User_ID, u.Username, r.Position, u.User_Email, "
			"c.Name AS CustomerName, s.StaffName "
			"FROM USER u "
			"JOIN ROLE r ON u.RoleID = r.RoleID "
			"LEFT JOIN CUSTOMER c ON u.User_ID = c.User_ID "					//left join to include both customer and staff details
			"LEFT JOIN STAFF s ON u.User_ID = s.User_ID ";						

		if (userId > 0) {
			pstmt = con->prepareStatement(
				baseQuery + "WHERE u.User_ID = ?");
			pstmt->setInt(1, userId);
		}
		else if (!keyword.empty()) {						//search by keyword
			string searchPattern = "%" + keyword + "%";		//wildcard pattern for LIKE search
			pstmt = con->prepareStatement(
				baseQuery +
				"WHERE u.Username LIKE ? "
				"OR c.Name LIKE ? "							//search by username or customer name
				"OR s.StaffName LIKE ?");					//search by username or name
			pstmt->setString(1, searchPattern);
			pstmt->setString(2, searchPattern);
			pstmt->setString(3, searchPattern);
		}
		else {
			cout << "\nInvalid input.\n";
			system("pause");
			return;
		}

		res = pstmt->executeQuery();

		while (res->next()) {
			found = true;
			cout << "\n-------------------------------------------\n";
			cout << "User ID   : " << res->getInt("User_ID") << endl;
			cout << "Username  : " << res->getString("Username") << endl;
			cout << "Position  : " << res->getString("Position") << endl;

			string position = res->getString("Position");
			if (position == "Customer") {
				cout << "Full Name : " << res->getString("CustomerName") << endl;
			}
			else {
				cout << "Full Name : " << res->getString("StaffName") << endl;
			}

			cout << "Email     : " << res->getString("User_Email") << endl;
		}

		if (!found) {
			cout << "\nNo user found with the provided keyword.\n";
		}

		if (res) delete res;
		if (pstmt) delete pstmt;
	}
	catch (sql::SQLException& e) {
		cout << "Database Error: " << e.what() << endl;
	}

	system("pause");
}


void updateUserDetails(sql::Connection* con) {
	bool inMenu = true;
	do {
		system("cls");
		cout << "===========================================\n";
		cout << "           UPDATE USER ACCOUNT       \n";
		cout << "===========================================\n";
		viewAllUsers(con, false);

		int userId;
		cout << "\nEnter User ID to Modify (or 0 to cancel): ";
		if (!safeIntInput(userId) || userId == 0) break;

		string userRole = "";
		try {
			sql::PreparedStatement* pstmt = con->prepareStatement(
				"SELECT r.Position FROM USER u JOIN ROLE r ON u.RoleID = r.RoleID WHERE u.User_ID = ?");		//fetch user role
			pstmt->setInt(1, userId);
			sql::ResultSet* res = pstmt->executeQuery();
			if (res->next()) userRole = res->getString("Position");
			delete res; delete pstmt;
		}
		catch (...) { break; }

		if (userRole == "") {
			cout << "[!] User ID not found.\n"; system("pause"); continue;
		}

		cout << "\nEditing Account for: " << userRole << " (ID: " << userId << ")\n";
		cout << "-------------------------------------------\n";
		cout << "1. Change Username\n";
		cout << "2. Update Email Address\n";
		cout << "3. Update Phone Number\n";
		cout << "4. Change Role/Position\n";
		cout << "0. Back\n";
		cout << "Choice: ";

		int choice;
		if (!safeIntInput(choice) || choice == 0) break;

		try {
			sql::PreparedStatement* pstmt = nullptr;
			string input;

			if (choice == 1) {
				cout << "New Username: "; cin >> input;
				pstmt = con->prepareStatement("UPDATE USER SET Username = ? WHERE User_ID = ?");	//update username
				pstmt->setString(1, input);
				pstmt->setInt(2, userId);
			}
			else if (choice == 2) {
				cout << "New Email: "; cin >> input;
				pstmt = con->prepareStatement("UPDATE USER SET User_Email = ? WHERE User_ID = ?");		//update email
				pstmt->setString(1, input);
				pstmt->setInt(2, userId);
			}
			else if (choice == 3) {
				cout << "New Phone: "; cin >> input;
				pstmt = con->prepareStatement("UPDATE USER SET User_PhoneNumber = ? WHERE User_ID = ?");	//update phone number
				pstmt->setString(1, input);
				pstmt->setInt(2, userId);
			}
			else if (choice == 4) {
				string newPos;
				cout << "Enter New Position (e.g. Manager/Receptionist/Customer): "; cin >> newPos;
				sql::PreparedStatement* rPstmt = con->prepareStatement("SELECT RoleID FROM ROLE WHERE Position = ?");	//fetch RoleID for new position
				rPstmt->setString(1, newPos);
				sql::ResultSet* rRes = rPstmt->executeQuery();
				if (rRes->next()) {
					pstmt = con->prepareStatement("UPDATE USER SET RoleID = ? WHERE User_ID = ?");
					pstmt->setInt(1, rRes->getInt("RoleID"));
					pstmt->setInt(2, userId);
				}
				else {
					cout << "[!] Invalid Position. Role not found in database.\n";
				}
				delete rRes; delete rPstmt;
			}

			if (pstmt) {
				pstmt->executeUpdate();
				cout << "\n[SUCCESS] Account details updated for User ID " << userId << ".\n";
				delete pstmt;
			}
		}
		catch (sql::SQLException& e) { cout << "DB Error: " << e.what() << endl; }

		system("pause");
	} while (inMenu);
}


void deleteUser(sql::Connection* con) {
	bool continueDeleting = true;

	do {
		system("cls");
		cout << "===========================================\n";
		cout << "              DELETE USER \n";
		cout << "===========================================\n";

		string keyword;
		cout << "Enter part of Username or Name (or type 'exit'): ";
		cin >> keyword;

		if (keyword == "exit") break;

		try {
			sql::PreparedStatement* pstmt = con->prepareStatement(					//search query to find users by username or name
				"SELECT u.User_ID, u.Username, r.Position "
				"FROM USER u "
				"JOIN ROLE r ON u.RoleID = r.RoleID "
				"LEFT JOIN CUSTOMER c ON u.User_ID = c.User_Id "
				"LEFT JOIN STAFF s ON u.User_ID = s.User_ID "
				"WHERE u.Username LIKE ? OR c.Name LIKE ? OR s.StaffName LIKE ?"
			);

			string searchPattern = "%" + keyword + "%";								
			pstmt->setString(1, searchPattern);
			pstmt->setString(2, searchPattern);
			pstmt->setString(3, searchPattern);

			sql::ResultSet* res = pstmt->executeQuery();								//list to hold found users

	
			struct FoundUser { int id; string uname; string role; };
			vector<FoundUser> list;

			cout << "\nSearch Results:\n";
			cout << left << setw(5) << "No" << setw(15) << "Username" << "Position" << endl;
			cout << "-------------------------------------------\n";

			int count = 1;
			while (res->next()) {
				FoundUser fu = { res->getInt("User_ID"), res->getString("Username"), res->getString("Position") };
				list.push_back(fu);
				cout << left << setw(5) << count << setw(15) << fu.uname << fu.role << endl;
				count++;
			}

			if (list.empty()) {
				cout << "[!] No user found matching '" << keyword << "'.\n";
			}
			else {
				cout << "-------------------------------------------\n";
				cout << "Select number to delete (0 to cancel): ";
				int choice;
				if (safeIntInput(choice) && choice > 0 && choice <= list.size()) {
					int targetId = list[choice - 1].id;
					string targetUname = list[choice - 1].uname;

					sql::PreparedStatement* checkStmt = con->prepareStatement(
						"SELECT COUNT(*) AS active_bookings FROM BOOKING WHERE User_ID = ? AND BookingStatus NOT IN ('Completed', 'Cancelled')"
					);
					checkStmt->setInt(1, targetId);
					sql::ResultSet* resBook = checkStmt->executeQuery();

					bool canDelete = true;
					if (resBook->next() && resBook->getInt("active_bookings") > 0) {
						canDelete = false;
						cout << "\n[FAIL] Cannot delete " << targetUname << ". User has active bookings.\n";
					}
					delete resBook; delete checkStmt;

					if (canDelete) {
						cout << "\n[WARNING] Confirm delete '" << targetUname << "'? (y/n): ";
						char confirm; cin >> confirm;
						if (tolower(confirm) == 'y') {
							sql::PreparedStatement* delStmt = con->prepareStatement("DELETE FROM USER WHERE User_ID = ?");		//delete user record
							delStmt->setInt(1, targetId);
							delStmt->executeUpdate();
							cout << "[SUCCESS] User removed.\n";
							delete delStmt;
						}
					}
				}
			}
			delete res; delete pstmt;		//cleanup
		}
		catch (sql::SQLException& e) {
			cout << "Database Error: " << e.what() << endl;
		}

		cout << "\n1. Search again\n0. Back to Menu\nChoice: ";
		int nextStep;
		if (!safeIntInput(nextStep) || nextStep == 0) continueDeleting = false;					//exit loop

	} while (continueDeleting);
}

void bookRoom(sql::Connection* con) {
	bool continueBooking = true;
	while (continueBooking) {
		system("cls");
		cout << "===========================================\n";
		cout << "              NEW RESERVATION              \n";
		cout << "===========================================\n";
		viewAvailableRooms(con, false);

		int targetUserId;

		if (currentUser.role != "Customer") {
			cout << "\nEnter Customer User ID for this booking: ";
			if (!safeIntInput(targetUserId)) {
				cout << "Invalid User ID input.\n";
				system("pause");
				return;
			}
		}
		else {
			targetUserId = currentUser.userId;
		}

		int targetCustomerId = 0;
		string targetRole = "";

		try {		//database operations

			sql::PreparedStatement* pstmt = con->prepareStatement(		//fetch customer ID and role
				"SELECT c.Customer_ID, r.Position FROM CUSTOMER c "
				"JOIN USER u ON c.User_Id = u.User_ID "
				"JOIN ROLE r ON u.RoleID = r.RoleID WHERE u.User_ID = ?"
			);

			pstmt->setInt(1, targetUserId);						
			sql::ResultSet* res = pstmt->executeQuery();		//execute query
			if (res->next()) {
				targetCustomerId = res->getInt("Customer_ID");	//get customer ID
				targetRole = res->getString("Position");
			}
			delete res; delete pstmt;

			if (targetCustomerId == 0 || targetRole != "Customer") {
				cout << "[ERROR] Invalid User ID or User is not registered as a Customer.\n";
				system("pause");
				return;
			}

			int representativeRoomId;
			cout << "\nEnter Room Category ID to Book: ";
			if (!safeIntInput(representativeRoomId)) return;		//get room category ID

			string roomType;
			double roomPrice = 0;
			int actualRoomId = 0;

			pstmt = con->prepareStatement(
				"SELECT RoomType, RoomPrice FROM ROOM WHERE Room_ID = ? AND RoomStatus='Available'"
			);
			pstmt->setInt(1, representativeRoomId);
			res = pstmt->executeQuery();

			if (res->next()) {
				roomType = res->getString("RoomType");
				roomPrice = res->getDouble("RoomPrice");
			}
			else {
				cout << "\n[ERROR] Room ID not found or not available.\n";
				delete res; delete pstmt; system("pause"); return;
			}
			delete res; delete pstmt;

			pstmt = con->prepareStatement(
				"SELECT Room_ID FROM ROOM WHERE RoomType = ? AND RoomStatus = 'Available' LIMIT 1"		//get an actual room number of that type
			);
			pstmt->setString(1, roomType);
			res = pstmt->executeQuery();
			if (res->next()) actualRoomId = res->getInt("Room_ID");			//get actual room ID
			else {
				cout << "\n[ERROR] No available rooms of selected type.\n";
				delete res; delete pstmt; system("pause"); return;
			}
			delete res; delete pstmt;


			int currentPoints = 0;
			pstmt = con->prepareStatement("SELECT Points FROM REWARD WHERE Customer_ID = ?");		//get current reward points
			pstmt->setInt(1, targetCustomerId);
			res = pstmt->executeQuery();
			if (res->next()) currentPoints = res->getInt("Points");
			delete res; delete pstmt;

			string checkIn, checkOut;
			cout << "Enter Check-In (YYYY-MM-DD): "; cin >> checkIn;
			cout << "Enter Check-Out (YYYY-MM-DD): "; cin >> checkOut;

			time_t t = time(0);					// get current time			
			tm* now_ptr = localtime(&t);		// convert to local time structure
			tm tm_now = *now_ptr;				

			stringstream ss_today;
			ss_today << (tm_now.tm_year + 1900) << "-"							
				<< setfill('0') << setw(2) << (tm_now.tm_mon + 1) << "-"
				<< setfill('0') << setw(2) << tm_now.tm_mday;

			long long days_till_checkin = daysBetweenDates(ss_today.str(), checkIn);		//days until check-in
			long long duration_days = daysBetweenDates(checkIn, checkOut);

			if (days_till_checkin < 2) {
				cout << "\n[ERROR] Policy Violation: Reservations must be made at least 2 days in advance.\n";
				cout << "Today is: " << ss_today.str() << ". Please select a date at least 2 days away.\n";
				system("pause");
				continue;
			}

			if (duration_days <= 0) {
				cout << "\n[ERROR] Invalid duration. Check-Out must be after Check-In.\n";
				system("pause");
				continue;
			}

			double basePrice = roomPrice * duration_days;
			if (duration_days >= 7) basePrice *= 0.90; 

			double finalPrice = basePrice;
			int pointsDeducted = 0;


			if (currentPoints >= 100) {
				cout << "\n[PROMO] You have " << currentPoints << " pts. Use 100 pts for 50% discount? (y/n): ";
				char apply; cin >> apply;
				if (tolower(apply) == 'y') {
					finalPrice = basePrice * 0.5;
					pointsDeducted = 100;
				}
			}

			cout << "\n-------------------------------------------";
			cout << "\nTotal Stay: " << duration_days << " night(s)";
			cout << "\nFinal Price: RM " << fixed << setprecision(2) << finalPrice;
			cout << "\n-------------------------------------------";
			cout << "\nConfirm Booking? (y/n): ";
			char confirm; cin >> confirm;

			if (tolower(confirm) == 'y') {

				pstmt = con->prepareStatement(
					"INSERT INTO BOOKING (User_ID, Room_ID, Checkin_Date, CheckOut_Date, TotalPrice, BookingStatus) "		//insert booking record
					"VALUES (?, ?, ?, ?, ?, 'Confirmed')"																
				);
				pstmt->setInt(1, targetUserId);
				pstmt->setInt(2, actualRoomId);
				pstmt->setString(3, checkIn);
				pstmt->setString(4, checkOut);
				pstmt->setDouble(5, finalPrice);
				pstmt->executeUpdate();
				delete pstmt;

				int newBookingId = getLastInsertID(con);


				pstmt = con->prepareStatement("UPDATE ROOM SET RoomStatus = 'Booked' WHERE Room_ID = ?");		//update room status
				pstmt->setInt(1, actualRoomId);
				pstmt->executeUpdate();
				delete pstmt;


				int pointsEarned = (int)(basePrice * 0.1);
				int newPoints = currentPoints - pointsDeducted + pointsEarned;
				string newLevel = (newPoints >= 1000) ? "Gold" : (newPoints >= 100) ? "Silver" : "Bronze";		//determine new membership level

				pstmt = con->prepareStatement("UPDATE REWARD SET Points = ?, Level = ? WHERE Customer_ID = ?");		//update reward points and level
				pstmt->setInt(1, newPoints);
				pstmt->setString(2, newLevel);
				pstmt->setInt(3, targetCustomerId);
				pstmt->executeUpdate();
				delete pstmt;


				int floor = representativeRoomId;
				int unit = (newBookingId % 10) + 1;
				int assignedRoomNumber = (floor * 100) + unit;

				cout << "\n===========================================\n";
				cout << "            RESERVATION SUCCESSFUL         \n";
				cout << "===========================================\n";
				cout << " Booking ID    : #" << newBookingId << endl;
				cout << " Room Assigned : " << assignedRoomNumber << " (Floor " << floor << ")" << endl;
				cout << " Points Earned : +" << pointsEarned << " pts" << endl;
				cout << " Current Tier  : [" << newLevel << "]" << endl;
				cout << " Total Paid    : RM " << fixed << setprecision(2) << finalPrice << endl;
				cout << "===========================================\n";

				syncRoomStatus(con, representativeRoomId);
			}
			else {
				cout << "Booking cancelled.\n";
			}

			cout << "\nWould you like to perform another booking? (y/n): ";
			char repeat; cin >> repeat;
			if (tolower(repeat) != 'y') continueBooking = false;

		}
		catch (sql::SQLException& e) {
			cout << "Database Booking Error: " << e.what() << endl;
			system("pause");
			continueBooking = false;
		}
	}
}

void checkMyRewards(sql::Connection* con) {
	system("cls");
	cout << "====================================================================\n";
	cout << "                             REWARDS                                \n";
	cout << "====================================================================\n\n";

	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"SELECT Points, Level FROM REWARD WHERE Customer_ID = ?"
		);
		pstmt->setInt(1, currentUser.specificId);
		sql::ResultSet* res = pstmt->executeQuery();

		if (res->next()) {
			int currentPoints = res->getInt("Points");
			string memberLevel = res->getString("Level");

			cout << "  [ MEMBER ACCOUNT SUMMARY ]\n";
			cout << "  ------------------------------------------\n";
			cout << "  Account Holder   : " << currentUser.username << endl; 
			cout << "  Membership Tier  : " << memberLevel << endl;
			cout << "  Available Points : " << currentPoints << " pts" << endl;
			cout << "  ------------------------------------------\n\n";

			cout << "  [ LOYALTY PROGRAM STRUCTURE ]\n";
			cout << "  +----------------+-----------------------+\n";
			cout << "  | TIER LEVEL     | POINT RANGE           |\n";
			cout << "  +----------------+-----------------------+\n";
			cout << "  | Bronze         | 0 - 99                |\n";
			cout << "  | Silver         | 100 - 999             |\n";
			cout << "  | Gold           | 1000+                 |\n";
			cout << "  +----------------+-----------------------+\n\n";

			cout << "  [ EXCLUSIVE PRIVILEGES ]\n";
			cout << "  * REDEMPTION: Deduct 100 pts to unlock a 50% discount on any booking.\n";
			cout << "  * LONG-STAY:  Automatic 10% discount applies for stays exceeding 6 nights.\n";
			cout << "  ____________________________________________________________________\n";

		}
		else {
			cout << "  [!] Status: No loyalty record associated with this account.\n";
			cout << "      Please contact the front desk to enroll in the program.\n";
		}

		delete res;
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		cerr << "  [CRITICAL ERROR]: Database connection failed.\n";
		cerr << "  Details: " << e.what() << endl;
	}

	cout << "\n  Press any key to return to dashboard...";
	(void)_getch(); 
}

void viewAllBookings(sql::Connection* con, bool showPause) {
	system("cls");
	cout << "======================================================================================================\n";
	cout << "                                        ALL BOOKINGS                                          \n";
	cout << "======================================================================================================\n";


	string query =
		"SELECT b.Booking_ID, IFNULL(c.Name, 'No Name') AS CustomerName, r.RoomType, "
		"b.CheckIn_Date, b.CheckOut_Date, b.TotalPrice, b.BookingStatus "
		"FROM BOOKING b "
		"LEFT JOIN USER u ON b.User_ID = u.User_ID "
		"LEFT JOIN CUSTOMER c ON u.User_ID = c.User_ID " 
		"LEFT JOIN ROOM r ON b.Room_ID = r.Room_ID "
		"ORDER BY b.Booking_ID DESC";

	try {
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res = stmt->executeQuery(query);

		cout << left << setfill(' ') << setw(8) << "ID"
			<< setw(20) << "Customer"
			<< setw(20) << "Room Type"
			<< setw(12) << "CheckIn"
			<< setw(12) << "CheckOut"
			<< setw(10) << "Price"
			<< "Status" << endl;
		cout << "------------------------------------------------------------------------------------------------------\n";

		bool dataExists = false;
		while (res->next()) {
			dataExists = true;
			cout << left << setfill(' ') << setw(8) << res->getInt("Booking_ID")
				<< setw(20) << res->getString("CustomerName") 
				<< setw(20) << res->getString("RoomType")
				<< setw(12) << res->getString("CheckIn_Date")
				<< setw(12) << res->getString("CheckOut_Date")
				<< "RM" << fixed << setprecision(2) << setw(8) << res->getDouble("TotalPrice")
				<< res->getString("BookingStatus") << endl;
		}

		if (!dataExists) {
			cout << "\n                     [!] No booking records found in database.\n";
		}

		delete res;
		delete stmt;
	}
	catch (sql::SQLException& e) {
		cout << "Error retrieving bookings: " << e.what() << endl;
	}

	cout << "------------------------------------------------------------------------------------------------------\n";
	if (showPause) system("pause");
}

void viewMyBookings(sql::Connection* con, bool showPause)
{
	system("cls");
	cout << "=======================================================================================================\n";
	cout << "                                       MY RESERVATION HISTORY                                          \n";
	cout << "=======================================================================================================\n";
	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"SELECT b.Booking_ID, b.Room_ID, r.RoomType, "
			"b.CheckIn_Date, b.CheckOut_Date, "
			"b.TotalPrice, b.BookingStatus "
			"FROM BOOKING b "
			"JOIN ROOM r ON b.Room_ID = r.Room_ID "
			"WHERE b.User_ID = ? "
			"ORDER BY b.Booking_ID DESC"
		);
		pstmt->setInt(1, currentUser.userId);
		sql::ResultSet* res = pstmt->executeQuery();

		cout << left << setfill(' ') << setw(8) << "ID"
			<< setw(12) << "ROOM NO."   // Added Room Number Column
			<< setw(18) << "TYPE"
			<< setw(12) << "CHECK-IN"
			<< setw(12) << "CHECK-OUT"
			<< setw(12) << "PRICE"
			<< "STATUS" << endl;
		cout << "-------------------------------------------------------------------------------------------------------\n";

		bool dataExists = false;
		while (res->next()) {
			dataExists = true;


			int catId = res->getInt("Room_ID");
			int displayRoomNo = (catId * 100) + (res->getInt("Booking_ID") % 10 + 1);

			cout << left << setfill(' ') << setw(8) << res->getInt("Booking_ID")
				<< "R-" << setw(10) << displayRoomNo  // Displaying assigned Room No
				<< setw(18) << res->getString("RoomType")
				<< setw(12) << res->getString("CheckIn_Date")
				<< setw(12) << res->getString("CheckOut_Date")
				<< "RM" << fixed << setprecision(2) << setw(10) << res->getDouble("TotalPrice")
				<< "[" << res->getString("BookingStatus") << "]" << endl;
		}

		if (!dataExists) {
			cout << "\n                     [!] You have no previous booking records.\n";
		}
		delete res;
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		cout << "Error retrieving your bookings: " << e.what() << endl;
	}
	cout << "-------------------------------------------------------------------------------------------------------\n";
	if (showPause) system("pause");
}

void updateBooking(sql::Connection* con)
{
	system("cls");
	cout << "===========================================\n";
	cout << "           UPDATE BOOKING STATUS\n";
	cout << "===========================================\n";
	int bookingId;
	cout << "Enter Booking ID to Update: ";
	if (!safeIntInput(bookingId)) {
		cout << "Invalid input.\n"; system("pause"); return;
	}
	cout << "Select New Status:\n";
	cout << "1. Confirmed\n2. Completed\n3. Cancelled\nChoice: ";
	int choice;
	if (!safeIntInput(choice) || (choice < 1 || choice > 3)) {								//validate choice
		cout << "Invalid choice.\n"; system("pause"); return;
	}
	string newStatus = (choice == 1) ? "Confirmed" : (choice == 2) ? "Completed" : "Cancelled";
	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"UPDATE BOOKING SET BookingStatus = ? WHERE Booking_ID = ?"							//update booking status query
		);
		pstmt->setString(1, newStatus);
		pstmt->setInt(2, bookingId);
		int rows = pstmt->executeUpdate();
		if (rows > 0) {
			cout << "\n[SUCCESS] Booking status updated to '" << newStatus << "'.\n";
		}
		else {
			cout << "\n[FAIL] Booking ID not found.\n";
		}
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		cout << "Error updating booking: " << e.what() << endl;
	}
	system("pause");
}



void searchBooking(sql::Connection* con) {
	bool continueSearching = true;

	do {
		system("cls");
		cout << "===========================================\n";
		cout << "              SEARCH BOOKING\n";
		cout << "===========================================\n";

		string input;
		int bookingId = 0;
		string keyword;

		cout << "Enter Booking ID or Customer Name keyword (or 0 to exit): ";

		if (cin.peek() == '\n') cin.ignore();
		getline(cin, input);

		if (input == "0") break; 


		bool isNumber = true;
		for (char c : input) {
			if (!isdigit(c)) {
				isNumber = false;
				break;
			}
		}

		if (isNumber && !input.empty()) {
			bookingId = stoi(input);
		}
		else {
			keyword = input;
		}

		try {
			sql::PreparedStatement* pstmt = nullptr;
			sql::ResultSet* res = nullptr;
			bool found = false;

			const string baseQuery =
				"SELECT b.Booking_ID, c.Name, r.RoomType, "
				"b.CheckIn_Date, b.CheckOut_Date, "
				"b.TotalPrice, b.BookingStatus "
				"FROM BOOKING b "
				"JOIN USER u ON b.User_ID = u.User_ID "
				"LEFT JOIN CUSTOMER c ON u.User_ID = c.User_ID "
				"JOIN ROOM r ON b.Room_ID = r.Room_ID ";

			if (bookingId > 0) {
				pstmt = con->prepareStatement(baseQuery + "WHERE b.Booking_ID = ?");
				pstmt->setInt(1, bookingId);
			}
			else if (!keyword.empty()) {
				string searchPattern = "%" + keyword + "%";
				pstmt = con->prepareStatement(baseQuery + "WHERE c.Name LIKE ? OR r.RoomType LIKE ?");
				pstmt->setString(1, searchPattern);
				pstmt->setString(2, searchPattern);
			}

			if (pstmt) {
				res = pstmt->executeQuery();
				while (res->next()) {
					found = true;
					cout << "\n-------------------------------------------\n";
					cout << "Booking ID : " << res->getInt("Booking_ID") << endl;
					cout << "Customer   : " << res->getString("Name") << endl;
					cout << "Room Type  : " << res->getString("RoomType") << endl;
					cout << "Check-In   : " << res->getString("CheckIn_Date") << endl;
					cout << "Check-Out  : " << res->getString("CheckOut_Date") << endl;
					cout << "Price      : RM " << fixed << setprecision(2) << res->getDouble("TotalPrice") << endl;
					cout << "Status     : " << res->getString("BookingStatus") << endl;
				}
			}

			if (!found) {
		
				cout << "\n[!] No booking found with the provided keyword/ID.\n";
				cout << "1. Try search again\n";
				cout << "0. Return to Dashboard\n";
				cout << "Choice: ";

				int retryChoice;
				if (safeIntInput(retryChoice) && retryChoice == 1) {
					if (res) delete res;
					if (pstmt) delete pstmt;
					continue; 
				}
				else {
					continueSearching = false; 
				}
			}
			else {
				cout << "\nSearch completed. Search again? (y/n): ";
				char again; cin >> again;
				if (again != 'y' && again != 'Y') continueSearching = false;
				cout << "Returning to search menu...\n";
				system("pause");
			}

			if (res) delete res;
			if (pstmt) delete pstmt;
		}
		catch (sql::SQLException& e) {
			cout << "Error: " << e.what() << endl;
			system("pause");
			break;
		}

	} while (continueSearching);
}




void deleteBooking(sql::Connection* con) {
	system("cls");
	cout << "====================================================================\n";
	cout << "                         BOOKING MANAGEMENT                         \n";
	cout << "====================================================================\n\n";

	// Display list based on user role
	if (currentUser.role == "Customer") {
		viewMyBookings(con, false);
	}
	else {
		viewAllBookings(con, false);
	}

	int bookingId;
	cout << "\n  Enter Booking Reference ID to Manage: ";
	if (!safeIntInput(bookingId)) {
		cout << "\n  [!] Invalid input format.\n";
		system("pause");
		return;
	}

	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* res;
		int roomId = 0;
		double totalPrice = 0.0;
		string checkInDateStr;
		string customerName;
		int customerId = 0;

		pstmt = con->prepareStatement(
			"SELECT c.Name AS CustomerName, c.Customer_ID, b.Room_ID, b.CheckIn_Date, b.TotalPrice, b.BookingStatus "
			"FROM BOOKING b "
			"JOIN CUSTOMER c ON b.User_ID = c.User_Id "
			"WHERE b.Booking_ID = ?"
		);
		pstmt->setInt(1, bookingId);
		res = pstmt->executeQuery();

		if (!res->next()) {
			cout << "\n  [ERROR] Record not found.\n";
			delete res; delete pstmt;
			system("pause"); return;
		}

		customerName = res->getString("CustomerName");
		customerId = res->getInt("Customer_ID");
		roomId = res->getInt("Room_ID");
		checkInDateStr = res->getString("CheckIn_Date");
		totalPrice = res->getDouble("TotalPrice");
		string currentStatus = res->getString("BookingStatus");
		delete res; delete pstmt;

		int managementChoice = 1;

		if (currentUser.role != "Customer") {
			cout << "\n--- Select Administrative Action ---\n";
			cout << "1. Cancel Booking \n";
			cout << "2. Delete Booking )\n";
			cout << "Enter Choice: ";
			if (!safeIntInput(managementChoice)) return;
		}

		if (managementChoice == 1) {
	
			if (currentStatus == "Completed") {
				cout << "\n [!] DENIED: This booking is already COMPLETED.\n";
				system("pause"); return;
			}
			if (currentStatus == "Cancelled") {
				cout << "\n [INFO] This booking has already been cancelled.\n";
				system("pause"); return;
			}


			time_t now_t = time(0);
			tm tm_now = *localtime(&now_t); 

			tm tm_checkIn = { 0 }; 
			istringstream ss_checkIn(checkInDateStr);
			ss_checkIn >> get_time(&tm_checkIn, "%Y-%m-%d");


			tm_checkIn.tm_hour = 15;
			tm_checkIn.tm_min = 0;
			tm_checkIn.tm_sec = 0;

			tm_checkIn.tm_isdst = -1;
			tm_now.tm_isdst = -1;

			time_t time1 = mktime(&tm_checkIn);
			time_t time2 = mktime(&tm_now);

			double hoursLeft = difftime(time1, time2) / 3600.0;
			double* hoursPtr = &hoursLeft; 

			double refund_pct = 0.0;

			if (*hoursPtr >= 48.0) refund_pct = 1.0;
			else if (*hoursPtr >= 12.0) refund_pct = 0.7;
			else if (*hoursPtr >= 3.0) refund_pct = 0.4;
			else if (*hoursPtr > 0.0) refund_pct = 0.1;
			else refund_pct = 0.0;

			double refund_amount = totalPrice * refund_pct;

			cout << "\n  +----------------------------------------------------------+\n";
			cout << "    CANCELLATION SUMMARY \n";
			cout << "    Time remaining  : " << fixed << setprecision(1) << *hoursPtr << " hours" << endl;
			cout << "    Guest Name      : " << customerName << endl;
			cout << "    Original Price  : RM " << fixed << setprecision(2) << totalPrice << endl;
			cout << "    REFUND RATE     : " << (refund_pct * 100) << "%" << endl;
			cout << "    REFUND TOTAL    : RM " << refund_amount << endl;
			cout << "  +----------------------------------------------------------+\n\n";

			cout << "  Confirm cancellation and release room? (y/n): ";
			char confirm; cin >> confirm;
			if (tolower(confirm) == 'y') {

				pstmt = con->prepareStatement("UPDATE BOOKING SET BookingStatus = 'Cancelled' WHERE Booking_ID = ?");
				pstmt->setInt(1, bookingId);
				pstmt->executeUpdate();
				delete pstmt;


				pstmt = con->prepareStatement("UPDATE ROOM SET RoomStatus = 'Available' WHERE Room_ID = ?");
				pstmt->setInt(1, roomId);
				pstmt->executeUpdate();
				delete pstmt;


				int pointsToDeduct = (int)(totalPrice * 0.1);
				pstmt = con->prepareStatement("UPDATE REWARD SET Points = IF(Points >= ?, Points - ?, 0) WHERE Customer_ID = ?");
				pstmt->setInt(1, pointsToDeduct);
				pstmt->setInt(2, pointsToDeduct);
				pstmt->setInt(3, customerId);
				pstmt->executeUpdate();
				delete pstmt;

				cout << "\n  [SUCCESS] Booking #" << bookingId << " has been cancelled.\n";
				cout << "  [INFO] " << pointsToDeduct << " reward points have been deducted.\n";
			}
		}
		else if (managementChoice == 2) {
			cout << "\n  [WARNING] Permanent delete will remove this record from the database.\n";
			cout << "  Proceed with deletion? (y/n): ";
			char confirm; cin >> confirm;
			if (tolower(confirm) == 'y') {
				pstmt = con->prepareStatement("DELETE FROM BOOKING WHERE Booking_ID = ?");
				pstmt->setInt(1, bookingId);
				pstmt->executeUpdate();
				delete pstmt;
				cout << "\n  [SUCCESS] Record #" << bookingId << " purged.\n";
			}
		}
	}
	catch (sql::SQLException& e) {
		cerr << "\n  [DATABASE ERROR]: " << e.what() << endl;
	}
	system("pause");
}

void manageCheckInOut(sql::Connection* con) {
	system("cls");
	cout << "================================================================================\n";
	cout << "                         CHECK-IN / CHECK-OUT TERMINAL                          \n";
	cout << "================================================================================\n\n";

	try {
		cout << " PENDING CHECK-INS (CONFIRMED BOOKINGS):\n";
		cout << " ------------------------------------------------------------------------------\n";

		cout << " " << left << setw(8) << "ID"
			<< setw(22) << "Guest Name"
			<< setw(20) << "Room Type"
			<< "Check-In Date" << endl;
		cout << " ------------------------------------------------------------------------------\n";

		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* resList = stmt->executeQuery(
			"SELECT b.Booking_ID, c.Name, r.RoomType, b.CheckIn_Date "
			"FROM BOOKING b "
			"JOIN ROOM r ON b.Room_ID = r.Room_ID "
			"JOIN CUSTOMER c ON b.User_ID = c.User_ID "
			"WHERE b.BookingStatus = 'Confirmed' "
			"ORDER BY b.CheckIn_Date ASC"
		);

		int count = 0;
		while (resList->next()) {
			cout << " " << left << setw(8) << resList->getInt("Booking_ID")
				<< setw(22) << resList->getString("Name")
				<< setw(20) << resList->getString("RoomType")
				<< resList->getString("CheckIn_Date") << endl;
			count++;
		}
		delete resList; delete stmt;

		if (count == 0) {
			cout << " [INFO] No pending arrivals found.\n";
		}
		cout << " ------------------------------------------------------------------------------\n";

		int bookingId;
		cout << "\n Enter Booking ID to process (or 0 to cancel): ";
		if (!safeIntInput(bookingId) || bookingId == 0) return;

		sql::PreparedStatement* pstmt = con->prepareStatement(
			"SELECT b.BookingStatus, r.RoomType, r.Room_ID FROM BOOKING b "
			"JOIN ROOM r ON b.Room_ID = r.Room_ID WHERE b.Booking_ID = ?"
		);
		pstmt->setInt(1, bookingId);
		sql::ResultSet* res = pstmt->executeQuery();

		if (res->next()) {
			string status = res->getString("BookingStatus");
			int roomId = res->getInt("Room_ID");

			cout << "\n +-------------------------------------------+\n";
			cout << "   Selected ID    : #" << bookingId << endl;
			cout << "   Current Status : [" << status << "]" << endl;
			cout << " +-------------------------------------------+\n";
			cout << "  1. Confirm Check-In (Guest Arrived)\n";
			cout << "  2. Confirm Check-Out (Guest Leaving)\n";
			cout << "  3. Cancel\n";
			cout << " ---------------------------------------------\n";
			cout << " Choice: ";
			int choice; cin >> choice;

			if (choice == 1 && status == "Confirmed") {
				pstmt = con->prepareStatement("UPDATE BOOKING SET BookingStatus = 'Checked-In' WHERE Booking_ID = ?");
				pstmt->setInt(1, bookingId);
				pstmt->executeUpdate();
				cout << "\n [SUCCESS] Guest checked in to Room #" << roomId << endl;

			}
			else if (choice == 2 && status == "Checked-In") {
				pstmt = con->prepareStatement("UPDATE BOOKING SET BookingStatus = 'Completed' WHERE Booking_ID = ?");
				pstmt->setInt(1, bookingId);
				pstmt->executeUpdate();
				cout << "\n [SUCCESS] Check-out completed. Room #" << roomId << " is now vacant.\n";

			}
			else if  (choice == 3) {
				cout << "\n [ERROR] Invalid operation for current status [" << status << "].\n";
			}
			else 
				cout << "\n Invalid input.\n";
		}
		else {
			cout << " [ERROR] Booking ID #" << bookingId << " not found.\n";
		}
		delete res; delete pstmt;
	}
	catch (sql::SQLException& e) { cout << " Database Error: " << e.what() << endl; }

	cout << "\n";
	system("pause");
}


void generateReport(sql::Connection* con) {
	reportMenu(con);
}

void autoCleanupNoShow(sql::Connection* con) {
	try {

		time_t t = time(0);
		tm* now = localtime(&t);
		stringstream ss_today;
		ss_today << (now->tm_year + 1900) << "-"
			<< setfill('0') << setw(2) << (now->tm_mon + 1) << "-"
			<< setfill('0') << setw(2) << now->tm_mday;

		sql::PreparedStatement* pstmt = con->prepareStatement(
			"UPDATE BOOKING SET BookingStatus = 'No-Show' "
			"WHERE BookingStatus = 'Confirmed' AND CheckIn_Date < ?"
		);
		pstmt->setString(1, ss_today.str());
		int affected = pstmt->executeUpdate();
		delete pstmt;

		if (affected > 0) {

			sql::Statement* stmt = con->createStatement();
			stmt->execute("UPDATE ROOM SET RoomStatus = 'Available' WHERE RoomStatus = 'Booked'");
			delete stmt;

			cout << "\n[SYSTEM] Auto-cleanup: " << affected << " expired bookings cleared.\n";
		}
	}
	catch (sql::SQLException& e) {

	}
}


// --- Reports Implementations ---

void reportMenu(sql::Connection* con) {
	bool inReportMenu = true;
	while (inReportMenu) {
		system("cls");
		cout << "====================================================\n";
		cout << "                    REPORT MENU            \n";
		cout << "====================================================\n";
		cout << "  [1] Room Inventory & Status Report\n";
		cout << "  [2] Financial Performance Summary\n";
		cout << "  [3] Guest Loyalty & Analytics\n";
		cout << "  [4] Monthly Revenue Trend Analysis\n";
		cout << "  [5] Monthly Revenue Visual Chart \n";
		cout << "  [6] Monthly Room Popularity Ranking\n";
		cout << "  [0] Return to Staff Dashboard\n";
		cout << "====================================================\n";
		cout << "Enter your choice: ";

		int choice;
		if (safeIntInput(choice)) {
			switch (choice) {
			case 1: roomReport(con); break;
			case 2: bookingReport(con); break;
			case 3: guestReport(con); break;
			case 4: revenueAnalysisReport(con); break;
			case 5: revenueBarChart(con); break;
			case 6: roomPopularityReport(con); break;
			case 0: inReportMenu = false; break;
			default:
				cout << "\n[!] Invalid selection. Please try again.\n";
				system("pause");
			}
		}
	}
}


void roomReport(sql::Connection* con) {
	system("cls");
	cout << "================================================================================\n";
	cout << "                     ROOM IVENTORY AND STATUS REPORT                          \n";
	cout << "================================================================================\n\n";

	try {
		sql::Statement* stmt = con->createStatement();

		string query =
			"SELECT R.RoomType, R.RoomStatus, R.TotalQuantity, "
			"(SELECT COUNT(*) FROM BOOKING B WHERE B.Room_ID = R.Room_ID AND B.BookingStatus = 'Confirmed') AS BookedCount "
			"FROM ROOM R ORDER BY R.RoomType";

		sql::ResultSet* res = stmt->executeQuery(query);

		cout << " [ CURRENT INVENTORY STATUS ]\n\n";

		while (res->next()) {
			string status = res->getString("RoomStatus");
			string icon = (status == "Available") ? "[OK]" : "[!]";

			int total = res->getInt("TotalQuantity");
			int booked = res->getInt("BookedCount");
			int available = total - booked; 

			cout << " " << icon << " CATEGORY: " << left << setw(20) << res->getString("RoomType") << endl;
			cout << "      - Room Status     : " << status << endl;
			cout << "      - Total Capacity  : " << total << " Units" << endl;
			cout << "      - Currently Booked: " << booked << " Units" << endl;
			cout << "      - Total Available : " << available << " Units" << endl;
			cout << "      ----------------------------------------------------------\n";
		}

		cout << "\n [ MAINTENANCE SUMMARY ]\n";
		cout << " * All available rooms have passed safety and housekeeping inspections.\n";
		cout << " * Management intervention required for categories marked with [!].\n";

		delete res; delete stmt;
	}
	catch (sql::SQLException& e) { cout << "DB Error: " << e.what() << endl; }
	system("pause");
}


void bookingReport(sql::Connection* con) {
	system("cls");
	cout << "================================================================================\n";
	cout << "                       FINANCIAL PERFORMANCE REPORT                      \n";
	cout << "================================================================================\n";
	cout << " Reporting Period : FY2025/2026\n";
	cout << " Document Status  : Final Audit\n";
	cout << "--------------------------------------------------------------------------------\n\n";

	try {
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* resSum = stmt->executeQuery("SELECT SUM(TotalPrice), COUNT(*) FROM BOOKING WHERE BookingStatus != 'Cancelled'");

		double totalRev = 0.0;
		int totalBookings = 0;
		if (resSum->next()) {
			totalRev = resSum->getDouble(1);
			totalBookings = resSum->getInt(2);
		}
		delete resSum;

		cout << " [1] GLOBAL REVENUE SUMMARY\n";
		cout << "     * Total Gross Revenue    : RM " << fixed << setprecision(2) << totalRev << endl;
		cout << "     * Confirmed Transactions : " << totalBookings << " Completed Bookings" << endl;
		cout << "     * Average Revenue/Guest  : RM " << (totalBookings > 0 ? totalRev / totalBookings : 0) << endl;
		cout << "\n";

		cout << " [2] REVENUE BY CATEGORY (Ranked by Performance)\n";
		sql::ResultSet* resType = stmt->executeQuery(
			"SELECT r.RoomType, COUNT(b.Booking_ID) as Bookings, SUM(b.TotalPrice) as Revenue "
			"FROM ROOM r JOIN BOOKING b ON r.Room_ID = b.Room_ID "
			"WHERE b.BookingStatus != 'Cancelled' "
			"GROUP BY r.RoomType ORDER BY Revenue DESC"
		);

		int rank = 1;
		while (resType->next()) {
			cout << "     " << rank << ". " << left << setw(20) << resType->getString("RoomType")
				<< " >> RM " << right << setw(10) << resType->getDouble("Revenue")
				<< " (" << resType->getInt("Bookings") << " stays)" << endl;
			rank++;
		}
		delete resType;

		cout << "\n [3] STRATEGIC ANALYSIS\n";
		if (totalRev > 5000)
			cout << "     - OVERALL STATUS: HIGH PERFORMANCE. Revenue targets achieved.\n";
		else
			cout << "     - OVERALL STATUS: STABLE. Suggesting more marketing for suites.\n";

		delete stmt;
	}
	catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
	system("pause");
}


void guestReport(sql::Connection* con) {
	system("cls");
	cout << "================================================================================\n";
	cout << "                      GUEST LOYALTY AND RETENTION ANALYTICS                       \n";
	cout << "================================================================================\n\n";

	try {
		sql::Statement* stmt = con->createStatement();

		// Count total guests
		sql::ResultSet* resCount = stmt->executeQuery("SELECT COUNT(*) FROM CUSTOMER");
		if (resCount->next()) {
			cout << " >> TOTAL REGISTERED MEMBERS: " << resCount->getInt(1) << " Active Accounts\n\n";
		}
		delete resCount;

		cout << " [ TOP TIER MEMBERS (VIP) ]\n";
		string query = "SELECT c.Name, r.Points, r.Level FROM CUSTOMER c "
			"JOIN REWARD r ON c.Customer_ID = r.Customer_ID "
			"ORDER BY r.Points DESC LIMIT 3";

		sql::ResultSet* res = stmt->executeQuery(query);
		while (res->next()) {
			cout << " * VIP GUEST : " << left << setw(25) << res->getString("Name")
				<< " | Rank: " << res->getString("Level")
				<< " (" << res->getInt("Points") << " pts)" << endl;
		}
		delete res;

		cout << "\n [ MARKETING RECOMMENDATION ]\n";
		cout << " - High-point guests should be prioritized for complimentary room upgrades.\n";
		cout << " - Loyal customers represent 60% of recurring revenue this month.\n";

		delete stmt;
	}
	catch (sql::SQLException& e) { cout << "Error: " << e.what() << endl; }
	system("pause");
}


void revenueAnalysisReport(sql::Connection* con) {
	system("cls");
	cout << "==========================================================\n";
	cout << "             MONTHLY REVENUE TREND ANALYSIS               \n";
	cout << "==========================================================\n\n";

	try {
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res = stmt->executeQuery(
			"SELECT YEAR(CheckIn_Date) AS Year, MONTH(CheckIn_Date) AS Month, SUM(TotalPrice) AS MonthlyRevenue "
			"FROM BOOKING WHERE BookingStatus != 'Cancelled' "
			"GROUP BY Year, Month ORDER BY Year, Month"
		);

		double previousRevenue = 0;
		string monthNames[] = { "", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

		while (res->next()) {
			int year = res->getInt("Year");
			int month = res->getInt("Month");
			double currentRevenue = res->getDouble("MonthlyRevenue");

			cout << " [" << monthNames[month] << " " << year << "] ";
			cout << " Revenue: RM " << fixed << setprecision(2) << setw(10) << currentRevenue;

			if (previousRevenue > 0) {
				double pct = ((currentRevenue - previousRevenue) / previousRevenue) * 100;

				if (pct >= 0) {
					cout << "   +" << setprecision(1) << pct << "% (Gained)";
				}
				else {
					cout << "   " << setprecision(1) << pct << "% (Dropped)";
				}
			}
			else {
				cout << "  -- (Starting Point)";
			}
			cout << "\n----------------------------------------------------------\n";
			previousRevenue = currentRevenue;
		}

		cout << "\n >>> Quick Note: December peaks are standard due to holidays.\n";
		cout << " >>> Action: Focus on 'Jan-Feb' marketing to boost sales.\n";

		delete res; delete stmt;
	}
	catch (sql::SQLException& e) {
		cout << "Database Error: " << e.what() << endl;
	}
	system("pause");
}

void revenueBarChart(sql::Connection* con) {
	system("cls");
	cout << "====================================================================\n";
	cout << "                MONTHLY REVENUE VISUALIZATION                     \n";
	cout << "====================================================================\n\n";

	try {
		sql::Statement* stmt = con->createStatement();

		sql::ResultSet* res = stmt->executeQuery(
			"SELECT YEAR(CheckIn_Date) AS Year, MONTH(CheckIn_Date) AS Month, SUM(TotalPrice) AS MonthlyRevenue "
			"FROM BOOKING "
			"WHERE BookingStatus != 'Cancelled' "
			"GROUP BY Year, Month "
			"ORDER BY Year ASC, Month ASC"
		);


		cout << "  Period    | Revenue (RM) | Visualization (Scale: * = RM500)\n";
		cout << "  ----------+--------------+--------------------------------------------\n";

		string monthNames[] = { "", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
								"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

		bool dataFound = false;
		while (res->next()) {
			dataFound = true;
			int year = res->getInt("Year");
			int monthNum = res->getInt("Month");
			double revenue = res->getDouble("MonthlyRevenue");

			cout << "  " << monthNames[monthNum] << " " << year << " | "
				<< right << setw(12) << fixed << setprecision(2) << revenue << " | ";

			int barLength = static_cast<int>(revenue / 300);

			if (barLength > 40) barLength = 40;

			for (int i = 0; i < barLength; i++) {
				cout << "*";
			}
			cout << endl;
		}

		if (!dataFound) {
			cout << "               [NO DATA AVAILABLE FOR REVENUE REPORT]                \n";
		}

		delete res;
		delete stmt;
	}
	catch (sql::SQLException& e) {
		cout << "\n  [DATABASE ERROR]: " << e.what() << endl;
	}

	cout << "  ----------+--------------+--------------------------------------------\n";
	cout << "  Note: Visualization is based on confirmed bookings only.\n\n";
	system("pause");
}

void roomPopularityReport(sql::Connection* con) {
	system("cls");
	int targetMonth, targetYear;

	cout << "==========================================================\n";
	cout << "                ROOM PERFORMANCE ANALYSIS             \n";
	cout << "==========================================================\n";
	cout << "Enter Year : "; cin >> targetYear;
	cout << "Enter Month (1-12): "; cin >> targetMonth;

	string monthNames[] = { "", "January", "February", "March", "April", "May", "June",
							"July", "August", "September", "October", "November", "December" };

	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"SELECT R.RoomType, COUNT(B.Booking_ID) AS TimesBooked, SUM(B.TotalPrice) AS RevenueGenerated "
			"FROM ROOM R "
			"LEFT JOIN BOOKING B ON R.Room_ID = B.Room_ID "
			"AND MONTH(B.CheckIn_Date) = ? AND YEAR(B.CheckIn_Date) = ? "
			"AND B.BookingStatus != 'Cancelled' "
			"GROUP BY R.RoomType "
			"ORDER BY TimesBooked DESC"
		);
		pstmt->setInt(1, targetMonth);
		pstmt->setInt(2, targetYear);
		sql::ResultSet* res = pstmt->executeQuery();

		system("cls");
		cout << "====================================================================\n";
		cout << "            ROOM TYPE DEMAND RANKING - " << monthNames[targetMonth] << " " << targetYear << "\n";
		cout << "====================================================================\n";
		cout << left << setw(8) << "RANK" << setw(20) << "ROOM TYPE" << setw(15) << "BOOKINGS" << "PERFORMANCE STATUS" << endl;
		cout << "--------------------------------------------------------------------\n";

		int rank = 1;
		bool dataFound = false;
		while (res->next()) {
			dataFound = true;
			int bookings = res->getInt("TimesBooked");
			string roomType = res->getString("RoomType");
			string status;

			if (rank == 1 && bookings > 0) status = "[ TOP PERFORMER ]";
			else if (bookings >= 5) status = "[ HIGH DEMAND ]";
			else if (bookings >= 2) status = "[ STABLE ]";
			else if (bookings == 1) status = "[ LOW INTEREST ]";
			else status = "[ UNDERPERFORMING / NO BOOKINGS ]";

			cout << left << setw(8) << rank
				<< setw(20) << roomType
				<< setw(15) << bookings
				<< status << endl;
			rank++;
		}

		if (!dataFound) {
			cout << "\n [!] No data available for the selected period.\n";
		}

		cout << "--------------------------------------------------------------------\n";
		cout << " RECOMMENDATION:\n";
		cout << " * Top Performers: Consider increasing seasonal pricing.\n";
		cout << " * Underperforming: Launch promotional discounts or 'Flash Sales'.\n";
		cout << "====================================================================\n";

		delete res; delete pstmt;
	}
	catch (sql::SQLException& e) {
		cout << "[DATABASE ERROR] " << e.what() << endl;
	}
	system("pause");
}

void saveReportToDB(sql::Connection* con, string type, int totalB, double totalR) {
	try {
		sql::PreparedStatement* pstmt = con->prepareStatement(
			"INSERT INTO REPORT (Staff_ID, Report_Type, Total_Bookings, Total_Revenue) VALUES (?, ?, ?, ?)"
		);
		pstmt->setInt(1, currentUser.specificId); 
		pstmt->setString(2, type);
		pstmt->setInt(3, totalB);
		pstmt->setDouble(4, totalR);
		pstmt->executeUpdate();
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		
	}
}



