# MinKV Server

## What is MinKV Server
**MinKV Server** is a simple, lightweight, in-memory key–value storage server.
Clients communicate with the server using a simple, line-based DSL over TCP or TLS.

The server is:
- Concurrent and multithreaded
- Supports plain TCP and TLS connections
- Supports optional user authentication
- Designed to be simple and minimal

## Building
### Dependencies
The project depends on the following dependencies:
- **ASIO** (standalone)
- **OpenSSL**
- **sqlite3**
- **oneTBB**

Make sure all the developer files are installed on your system.

### Build Instructions
MinKV uses Cmake for building:
``` bash
mkdir build
cd build
cmake ..
make
```

## Configuration and Starting the Server
By default, MinKV loads its configuration from a file named ``config.cfg`` in the working directory.

To start the server with the default configuration:
```
./MinKV
```

A different config file can be used by providing it as an argument:
```
./MinKV my_config.cfg
```

## User Authentication
User credentials are stored in an SQLite3 database. By default, this database is named users.db.

### Adding Users
The recommended way of adding new users is to use built-in helper command:
```
./MinKV add-user users.db username
```
The program will ask the user for the password of the new user.
Afterward the command calculates the password hash correctly and inserts the user into the database.

For more advanced database administration, you can use the ``sqlite3`` CLI or any other database tool that supports sqlite3.

## Communication Protocol and Query DSL

### Command Format
Commands are sent to via tcp or tls to the server. Every command must be terminated with: ```\r\n ```

Commands can consist of the following three elements:
- The command identifier ``<cmd>``. (e.g ``GET``)
- Key identifiers ``<key>``: (e.g. ``key1``)
  - Key consist of alphanumerical characters or under scores
  - Keys mustn't contain spaces
- String literals ``<string>``: (e.g ``"This is a message\n"``)
  - Escape characters are supported

### Reply Format
All server replies are also terminated with ```\r\n```.

There are three types of replies, which should be self explanatorily:
```
OK
RESULT "Query result as string literal"
ERROR "Error message as string literal"
```

### List of Supported Commands
- ``GET <key>`` Returns the value associated with the key.
  If the key does not exist, an empty string (``""``) is returned.
- ``SET <key> <string>`` Sets the value of the key to the given string.
- ``DELETE <key>`` Deletes the value stored at the specified key.
- ``APPEND <key> <string>`` Appends the given string to the existing value.
- ``LOGIN <key> <string>`` Special command used for authentication.
  The key is interpreted as the username, and the string literal contains the password.
- ``CLOSE`` Requests for the server to close the connection

### Example of usage

```
LOGIN user "my super secret password"
OK

SET shopping_list "bread,butter"
OK

GET shopping_list
RESULT "bread,butter"

APPEND shopping_list ",eggs"
OK

GET shopping_list
RESULT "bread,butter,eggs"

CLOSE
OK
```

The ``\r\n`` at the end of each line are omitted for readability.

## TODO
The project is mostly feature complete, but there is still some work to be done:
 - [ ] Add a few more useful commands
 - [ ] Think of a better name (the hardest task)
 - [ ] More extensive testing
 - [ ] Added improved documentation