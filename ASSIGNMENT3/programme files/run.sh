rm Networking_Assignmenet3.db
g++ -o server server.c -lpthread -l sqlite3
./server 8080
