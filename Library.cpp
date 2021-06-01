#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Admin.hpp"
#include "Library.hpp"
using namespace std;

std::vector<Book*> Library::GetLibrary() {return library;}
Library::Library() {
	currUser = nullptr;
}

Library::~Library() {
	delete currUser;
	currUser == nullptr;
	for(unsigned i = 0; i < userList.size(); i++) {
		delete userList.at(i);
		userList.at(i) = nullptr;
	}
	for(unsigned i = 0; i < library.size(); i++) {
		delete library.at(i);
		library.at(i) = nullptr;
	}
}

void Library::DisplayGenre(string genre) {
	for (int i = 0; i < library.size(); i++) {
		if (library.at(i)->GetGenre() == genre) {
			cout << library.at(i)->GetTitle() << " by " << library.at(i)->GetAuthor();
		}
	}
}

void Library::DisplayAll() {
	cout << right << setw(20) << "Genre" << right << setw(50) << "Title" << right << setw(35) << "Author" << right << setw(7) << "ID" << endl;
	cout << "-----------------------------------------------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < library.size(); i++) {
		 cout << setw(20) << library.at(i)->GetGenre() << setw(50) <<  library.at(i)->GetTitle()
          << setw(35) << library.at(i)->GetAuthor() << setw(7)  << library.at(i)->GetID() << endl;
	}
}

Book* Library::FindBook(int id) {
	for (auto i : library) {
		if (i->GetID() == id) {
			return i;
		}
	}
	return nullptr;
}

bool Library::loadBooks() {
	ifstream fin;
	fin.open("booklists.csv");
	if(fin.fail())
		return false;
	
	string title = "";
	string author = "";
	string genre = "";
	string ID = "";
	string rating = "";
	string review = "";	
	
	while(getline(fin, title, ',')) {
		getline(fin, author, ',');
		getline(fin, genre, ',');
		getline(fin, ID, ',');
		getline(fin, rating, ',');
		getline(fin, review);

		int bookID = stoi(ID);
		double bookRate = stoi(rating);
		int bookRev = stoi(review);
	
		Book* tmp = new Book(title, author, genre, bookID, bookRate, bookRev);
		library.push_back(tmp);
	}

	fin.close();
	return true;
}

bool Library::loadUsers() {
	ifstream fin;
	fin.open("login.csv");
	if(fin.fail()) {
		return false;
	}
	
	string name;
	string pass;
	string admin;
	fin >> name;

	while(!fin.eof()) {
		fin >> pass;
		fin >> admin;
		if(admin == "1") {
			Admin* tmp = new Admin(name, pass);
			userList.push_back(tmp);
		} else {
			Standard* tmp = new Standard(name, pass);
			userList.push_back(tmp);
		}
		fin >> name;
	}
	fin.close();

	return true;
}

void Library::login() {
	string nameInput;
	string passInput;
	cout << "Please enter your username or 'n' to create a new login: ";
	cin >> nameInput;
	
	if(nameInput == "n")
		createNewUser();
	else {
		cout << "Please enter your password: ";
		cin >> passInput;
		cout << endl;

		currUser = getUser(nameInput, passInput);
		if(currUser == nullptr) {
			cout << "Invalid username or password.\n" << endl;
		} else {
			if (PopulateUser() == false)
				cout << "Error loading information\n" << endl;
			else
				cout << "Welcome " << nameInput << "!\n" << endl;
		}
	}
}

void Library::createNewUser() {
	string newUsername;
	string newPassword;
	cout << "Please create a username: ";
	cin >> newUsername;
	cout << "Please create a password: ";
	cin >> newPassword;

	Standard* newUser = new Standard(newUsername, newPassword);
	userList.push_back(newUser);
	currUser = newUser;

	ofstream fout;
	fout.open("login.csv", fstream::app);
	if(fout.fail()) {
                return;
        }
	fout <<  newUsername << " " << newPassword << " 0\n";
	fout.close();	

	cout << "New Standard user " << newUsername << " has been created.\n" << endl;
}

bool Library::PopulateUser() {
	ifstream fin;
	fin.open("UserFiles/" + currUser->getUserID() + ".txt");
	if (!fin.is_open()) {
		cout << "Error loading user" << endl;
		return false;
	}
	
	string inputLine;
	istringstream in1;
	istringstream in2;
	int input1;
	double input2;

	//Populate checked out books
	getline(fin, inputLine);
	if (inputLine != "") {
		while (inputLine != "$") {
			in1.str(inputLine);
			while (in1 >> input1) {
				currUser->AddCheckedOut(FindBook(input1));
			}
			getline(fin, inputLine);
		}
	}
	else {
		getline(fin, inputLine);
	}

	//Populate history map
	getline(fin, inputLine);
	if (inputLine != "") {
		while (inputLine != "$") {
			in2.str(inputLine);
			while (in2 >> input1) {
				in2 >> input2;
				pair<Book*, double> histVal;

				histVal.first = FindBook(input1);
				histVal.second = input2;

				currUser->AddHistory(histVal);
			}
			getline(fin, inputLine);
		}
	}
	else {
		getline(fin, inputLine);
	}

	//Populate book lists
	while (!fin.eof()) {
		currUser->AddLists(CreateList(fin, 0));
	}
	vector<Composition*> temp = currUser->GetLists();
	temp.pop_back();
	currUser->SetLists(temp);

	fin.close();
	return true;
}

Composition* Library::CreateList(ifstream& fin, bool key) {
	string list_name;
	string input;
	string trash;
	int tempID;

	if (key == 0) {
		fin >> trash;
		if (trash == "") {
			return nullptr;
		}
		fin.ignore();
	}
	getline(fin, list_name);	

	Composition* temp = new Composition();
	temp->SetName(list_name);

	getline(fin, input);
	while (input != "}") {
		if (input == "/") {
			temp->Add(CreateList(fin, 1));
		}
		else {
			istringstream in;
			in.str(input);
			in >> tempID;

			Individual* tempBook = new Individual();
			tempBook->SetBook(FindBook(tempID));

			temp->Add(tempBook);
		}
		getline(fin, input);
	}
	return temp;
}

void Library::printMenu() {
	cout << "Menu" << endl;
  	cout << "- Display Library ('d')" << endl;
	cout << "- View Checked Out Books ('o')" << endl;
  	cout << "- Checkout Book ('c')" << endl;
  	cout << "- Return Book ('r')" << endl;
	cout << "- Recommend Books ('m')" << endl;
	cout << "- Create List ('l')" << endl;
	cout << "- View Lists ('v')" << endl;
	if(currUser->getUserType() == "admin") {
                cout << "- Add Book to Library ('a')" << endl;
                cout << "- Remove Book from Library ('e')" << endl;
        }
	cout << "- Quit ('q')" << endl;
	cout << "Choose an action: " << endl;
}

void Library::start() {
	if(currUser == nullptr)
        	return;
        
    	char input;
	printMenu();
	bool isAdmin = false;
	if(currUser->getUserType() == "admin") {
		isAdmin = true;
	}

    	cin >> input;
    	while (input != 'q') {
        	if (input == 'd')
            		DisplayAll();
		else if (input == 'o')
			View();
        	else if (input == 'c')
			Checkout();
        	else if (input == 'r')
            		Return();
		else if (input == 'm')
			Recommend();
		else if (input == 'l')
			CreateList();
		else if (input == 'v')
			ViewLists();
		else if (input == 'a' && isAdmin)
			AddBook();
		else if (input == 'e' && isAdmin)
			RemoveBook();
        
        	cout << endl;
		printMenu();
		cin.ignore();
        	cin >> input;
        	if(input != 'q')
        	    cout << endl;
    	}
	StoreLibrary();	
	CreateFile();
    	currUser = nullptr;
    	cout << "Bye!" << endl;
}

void Library::CreateList() {
	cout << endl;
	currUser->newList(this);
}

void Library::ViewLists() {
	cout << endl;
	currUser->viewLists();
}

void Library::View() {
	vector<Book*> books = currUser->GetCheckedOut();
	
	cout << setw(20) << "Genre" << setw(50) << "Title" << setw(35) << "Author" << setw(7) << "ID" << endl;
	cout << "----------------------------------------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < books.size(); i++) {
		cout << setw(20) << books.at(i)->GetGenre() <<  setw(50) << books.at(i)->GetTitle()
		     << setw(35) << books.at(i)->GetAuthor() << setw(7) << books.at(i)->GetID() << endl;
	}
}

void Library::CreateFile() {
	ofstream fout; 
	fout.open("UserFiles/" + currUser->getUserID() + ".txt");

	for (auto i : currUser->GetCheckedOut()) {
		fout << i->GetID() << " ";	
	}
	fout << "\n$\n";

	for (auto i : currUser->GetHistory()) {
		fout << i.first->GetID() << " " << i.second << " ";
	}
	fout << "\n$\n";

	vector<Composition*> lists = currUser->GetLists();
	if (lists.empty() != true) {
		for (auto i : lists) {
			i->file(fout);
		}
	}
	fout.close();
}

void Library::Checkout() {
	int bookID = 0;
	cout << "Enter ID of book to check out: ";
	cin >> bookID;
	Book* b = FindBook(bookID);
	if (b == nullptr) {
		cout << "Book with ID: " << bookID << " was not found in the Library." << endl;
		return;
	}
	currUser->checkoutBook(b, library);
}

void Library::Return() {
	int bookID = 0;
	int index = 0;
        cout << "Enter ID of book to return: ";
        cin >> bookID;
        for (auto i : currUser->GetCheckedOut()) {
		if (bookID == i->GetID()) {
			currUser->returnBook(i, library, index);
			return;
		}
		++index;
	}
        cout << "Book with ID: " << bookID << " was not found in the User's list of Checked out Books." << endl;
}

void Library::Recommend() {
	currUser->recommend(library);
}

void Library::StoreLibrary() {
	ofstream fout;
	fout.open("booklists.csv");
	for (auto i : library) {
		fout << i->GetTitle() << "," << i->GetAuthor() << "," << i->GetGenre() << "," << i->GetID() << "," << i->GetRating() << "," << i->GetNumReviews() << "\n";
	}
	fout.close();
}

void Library::AddBook() {
	string title = "";
	string author = "";
	string genre = "";
	string ID = "";
	cout << "Enter book title: ";
	cin.ignore();
	getline(cin, title);
	cout << "Enter book author: ";
	getline(cin, author);
	cout << "Enter book genre: ";
	getline(cin, genre);
	cout << "Enter book ID: ";
	cin >> ID;
	int bookID = stoi(ID);

	while(bookID < library.size()) {
		cout << "Book with that ID already exists. Please enter a new ID: ";
		cin >> ID;
		bookID = stoi(ID);
	}

	Book* tmp = new Book(title, author, genre, bookID, 0.0, 0);
	currUser->addBook(tmp, library);
}

void Library::RemoveBook() {
	int bookID = 0;
	cout << "Enter ID of book to remove: ";
	cin >> bookID;
	Book* tmp = FindBook(bookID);
	currUser->remBook(tmp, library);
}

User* Library::getUser(const string& name, const string& pw) {
	for(unsigned i = 0; i < userList.size(); i++) {
		if(userList.at(i)->check(name, pw))
			return userList.at(i);
	}
	return nullptr;
}

string Library::GetCurrentUsername() {
	return currUser->getUserID();
}
