gcc -o client1 client1.c -lpthread
cp client1 client2
cp client1 client3
./client1 localhost 8080
