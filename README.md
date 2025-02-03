# Multi-Threaded Graph Search Server

## Overview
This project implements a **multi-threaded TCP/IP server** that computes the **shortest path** between graph nodes using **Breadth-First Search (BFS)**. Clients can connect to the server and request shortest paths dynamically. The server processes multiple client requests concurrently using **multi-threading**.

## Features
- **Graph stored in `db.csv`** for efficient access.
- **Multi-threaded server** to handle multiple clients simultaneously.
- **Shortest path computation** using BFS.
- **TCP/IP client-server communication** for dynamic interaction.
- **Caching mechanism** to store previous queries and reduce computation time.


## How to Compile and Run

### **1. Compile the Server**
```bash
g++ server/server.cpp -o server -pthread
```

### **2. Compile the Client**
```bash
g++ client/client.cpp -o client
```

### **3. Run the Server**
```bash
./server db.csv 4444
```
- `db.csv` is the dataset containing the graph.
- `4444` is the server's listening port.

### **4. Run the Client**
```bash
./client 127.0.0.1 4444 0 19382
```
- `127.0.0.1` is the server IP.
- `4444` is the port number.
- `0 19382` are the two node IDs for which the shortest path is requested.

## Example Input/Output
### **Client Request:**
```
>> Please choose one of the following options:
1. Find shortest path
2. Exit

>> Enter two node IDs: 0 19382
```
### **Server Response:**
```
Shortest path: 0 18427 19382
```

## Algorithm Implementation
- The server reads the **graph structure** from `db.csv`.
- It **parses** the data to store it in a suitable in-memory representation.
- When a client requests a shortest path, the server executes **BFS**.
- If the request has been **previously computed**, it returns the cached result.

## Future Improvements
- Optimize BFS for larger graphs using **priority queues**.
- Implement **Dijkstra’s Algorithm** for weighted graphs.
- Improve **error handling** and input validation.
- Add **graph visualization tools**.
- Implement **secure authentication** for client connections.

## Author
Developed by Ronen Yakobov.

## References
- [BFS Algorithm](https://en.wikipedia.org/wiki/Breadth-first_search)
- [Socket Programming](https://beej.us/guide/bgnet/)
- [Multi-Threading in C++](https://www.geeksforgeeks.org/multithreading-in-cpp/)

