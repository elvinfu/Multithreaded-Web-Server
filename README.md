# Multithreaded-Web-Server
Implementation of my own multithreaded web server called http333d. The server is designed to handle multiple client connections concurrently using a thread pool. It supports serving static files and performing search functionality.

## Usage
To compile the web server, run the following command:
````
make
````

To run the web server, use the following command:
````
./http333d <port> <document_root> <index_files...>
````
<port>: The port number on which the web server should listen for incoming connections.
<document_root>: The root directory of the web server's document tree.
<index_files...>: One or more index files to use for directory listings.
Example usage:
````
./http333d 5555 ../projdocs unit_test_indices/*
````

To shut down the web server gracefully, open another terminal window and run the following command:
````
ps -u
````
Then, locate the process ID (pid) of the web server and execute:
````
kill <pid>
````
