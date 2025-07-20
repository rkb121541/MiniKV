# MiniKV

A mini key-value storage system

* Currently, my program listens to TCP connections on a specified port (8080)

* It accepts one client connection at a time
    * So, it waits until a client connects and then prints "Accepted new client" on the server side
    * The rest of the clients that want to join have to wait and they get put in a queue; I specified there to be a maximum of 3 clients in the queue but it seems to support up to 4 (so 1 active client and 4 clients in the queue); I have to look into why that is the case

The server is activated by running:
```bash
g++ main.cpp
```
and then:
```bash
./a.out
```

We connect via the client by running in a new terminal:
```bash
telnet localhost 8080
```

The client is able to do these commands:

1. `PUT key value` to store the key with the given value

2. `GET key` to retrieve the value for the key

3. `DELETE key` to delete the key-value pair

The server responds with:

* `OK` for a successful `PUT`

* the value if `GET` is successful

* `NOT FOUND` if `GET` is unsuccessful

* `ERROR: Invalid command/ incorrect number of arguments` for improper commands

**TODO:**

* Add functionality for multiple clients simultaneously

* Save data to a database

* Implement authentication/ encryption

* Improve UI

