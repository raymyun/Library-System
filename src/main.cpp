#include <Db.h>
#include <Library.h>

#include <iostream>

int main() {
  std::string connStr =
      "mongodb+srv://libuser:libuser@libsystem.igbknrs.mongodb.net/"
      "?retryWrites=true&w=majority";

  Db *db = new Db(connStr);
  Library *lib = new Library(db);

  if (!lib->loadUsers()) {
    std::cerr << "ERROR: Cannot load users from login.csv" << std::endl;
    exit(1);
  }

  lib->login();
  lib->start();

  delete db;
  delete lib;
  return 0;
}