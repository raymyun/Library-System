#include "Strategy.hpp"
#include "User.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include "Library.hpp"
#include "Standard.hpp"

void Standard::run(Library* library_system) {}

void Standard::newList(Library* library) {
	Composition* newList = new Composition();
	
	std::string name;
	std::cout << "List name: ";
	std::getline(std::cin, name);
	newList->SetName(name);
	std::cin.clear();

	int input = 1;
	while (input != 0) {
		std::cout << "1. Output List" << std::endl;
		std::cout << "2. Add to List" << std::endl;
		std::cout << "0. Stop" << std::endl;

		std::cin >> input;
		if (input == 1) {
			library->DisplayAll();
		}
		else if (input == 2) {
			std::cout << "Enter an ID number or press 2 to enter another list: ";
			std::cin >> input;
			if (input != 2) {
				for (auto i : library->GetLibrary()) {
					if (i->GetID() == input) {
						Individual* entry = new Individual();
						entry->SetBook(library->FindBook(input));
						newList->Add(entry);
						std::cout << entry->GetBook()->GetTitle() << " was added to your list" << std::endl;
					}
				}
			}
			else {
				if (lists.size() != 0) {
					std::cout << "Enter name of list you would like to add: ";
					std::getline(std::cin, name);
					std::cin.clear();
					for (auto i : lists) {
						if (name == i->GetName()) {
							newList->Add(i);
							std::cout << name << " was added to your list" << std::endl;
						}
					}
				}
				else {
					std::cout << "Error: No Lists available" << std::endl;
				}
			}
		}
	}
}

void Standard::viewLists() {
	for (auto i : lists) {
		i->print(0);
	}
}

void Standard::recommend(std::vector<Book*> library) {
	std::cout << std::setw(20) << "Genre" << std::setw(50) << "Title" << std::setw(35) << "Author" << std::setw(7) << "ID" << std::endl;
	std::cout << "----------------------------------------------------------------------------------------------------------------" << std::endl;
	
	Favorites* alg1 = new Favorites();
        alg1->recommendation_algorithm(history, library);
	Popular* alg2 = new Popular();
	alg2->recommendation_algorithm(history, library);


	delete alg1;
	delete alg2;
}

void Standard::AddCheckedOut(Book* book) {
	checkedOut.push_back(book);
}

void Standard::AddHistory(std::pair<Book*, double> data) {
	history.insert(data);
}

void Standard::AddLists(Composition* list) {
	lists.push_back(list);
}

void Standard::checkoutBook(Book* b, std::vector<Book*>& library) {
	for (auto i : checkedOut) {
		if (i->GetID() == b->GetID()) {
			std::cout << "You already have this book checked out." << std::endl;
			return;
		}
	}
	checkedOut.push_back(b);
	std::cout << "Book with ID: " << b->GetID() << " has been successfully checked out to User." << std::endl;
}

void Standard::returnBook(Book* b, std::vector<Book*>& library, int index) {
	double rating = -1.0;
	checkedOut.erase(checkedOut.begin() + index);
	std::cout << "Book with ID: " << b->GetID() << " has been successfully returned to the Library." << std::endl;
	while (rating < 1.0 || rating > 5.0) {
		std::cout << "From 1 to 5 (1 being the worst, 5 being the best), please input your rating of this book: ";
		std::cin >> rating;
		std::cout << std::endl;
	}
	this->history.insert(std::pair<Book*, double>(b, rating));
	std::cout << "You have given the book with ID: " << b->GetID() << " a rating of " << rating << "." << std::endl;
	b->SetRating(((b->GetRating() * b->GetNumReviews()) + rating) / (b->GetNumReviews() + 1));
	b->SetNumReviews(b->GetNumReviews() + 1);
}

void Standard::displayBooks() {
    for(unsigned i = 0; i < checkedOut.size(); i++) {
       std::cout <<  checkedOut.at(i)->GetTitle() << std::endl;
    }
}
