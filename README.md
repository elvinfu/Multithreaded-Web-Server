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
Example usage:
````
./http333d 5555 ../projdocs unit_test_indices/*
````

Once you have the web server running, type your search query in the search bar and the top results will appear.

To shut down the web server gracefully, open another terminal window and run the following command:
````
ps -u
````
Then, locate the process ID (pid) of the web server and execute:
````
kill <pid>
````
