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

I implemented 2 versions (one using processes -> `/processes/`, one using threads-> `/threads/`)

Difference between processes and threads:
    * Processes do not share memory, while threads do. So in my implementation with threads, one client is able to access elements that another client has stored 

So, in my `threads` implementation, I used an `std::shared_mutex` for read/ write access to the `std::unordered_map` so that different threads don't access that data structure simultaenously

I use a shared mutex instead of a regular one because I want readers to all be able to share access to the map. Only writers will block access to the map.

TODO:

* Enable quit command
