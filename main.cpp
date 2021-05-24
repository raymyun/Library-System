#include <iostream>
#include "Library.hpp"

int main() {
	Library* lib = new Library();
	if(!lib->loadUsers()) {
		std::cerr << "ERROR: Cannot load users from login.csv" << std::endl;
		exit(1);
	}
	
	lib->login();

	return 0;
}
