//Daniel Dahan 318840196
//Oleg Muraviov 321163446
//Ronen Yakoubov 207688581 
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

struct Graph {
    unordered_map<int,vector<int>> adjList;
    
};

//Global variables for caching
const int CACHE_SIZE = 10;
unordered_map<string,vector<int>> cache;
queue<string> cacheOrder;
mutex cacheMutex;

//Read from the file
Graph readGraphFromFile(const string& fileName){
    ifstream file(fileName);
    Graph graph;

    if(file.is_open()) {
        int node1, node2;
        while (file >> node1 >> node2){
            graph.adjList[node1].push_back(node2);
            graph.adjList[node2].push_back(node1);
        
        }
        file.close();
    }  else{
        cerr << "Error: Unable to open file " << fileName << endl;
    }
    return graph;
}

// Function to perform Breadth First Search to find shortest path
vector<int> BFS(const Graph& graph, int start, int end){
    queue<vector<int>> q; // creates queue of paths
    q.push({start});
    vector<bool> visited(graph.adjList.size(), false); // tracking if visited in particular vertice

    while (!q.empty()){
        vector<int> path = q.front();
        q.pop();
        int node = path.back();

        if(node == end) //  if the last node is the destination (stopping condition)
            return path;

        if(!visited[node]){
            visited[node] = true;
            for(int neighbor : graph.adjList.at(node)){ //  make a newPath with each neigbor
                if(!visited[neighbor]) {
                    vector<int> newPath = path;
                    newPath.push_back(neighbor);
                    q.push(newPath);
                    // q.push(newPath);


                }
            }
        }
    }
    return {}; //no path found
}

void handleClient(int clientSocket, const Graph& graph){
    char buffer[256];
    int n;

    while (true){
        // Receive data from client
        n = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (n <= 0){
            cerr << "Error: Connection closed by client" << endl;
            close(clientSocket);
            return;
        }
        // Proccesing client request
        int start, end;
        sscanf(buffer, "%d %d", &start, &end);

        // Check cach
        string key = to_string(start) + "-" + to_string(end);
        vector<int> cachedPath;
        bool useCache = false; // flag for checking if cached path is found

        {
            lock_guard<mutex> lock(cacheMutex);


            
            if (cache.find(key) != cache.end()) {
                cachedPath = cache[key];
                useCache = true;
            }
        }

        // calculate path or use cache
        vector<int> path;
        if(!useCache){
            path = BFS(graph, start, end);

            {
                lock_guard<mutex> lock(cacheMutex);
                cache[key] = path;
                cacheOrder.push(key);
                if(cacheOrder.size() > CACHE_SIZE){
                    string oldestKey = cacheOrder.front();
                    cacheOrder.pop();
                    cache.erase(oldestKey);
                }
            }
        }  else {
            path = cachedPath;
        }

        // send response to client
        if(path.empty()){
            send(clientSocket, "No path found", sizeof("No path found"), 0);

        } else{
            string response;
            for(int node : path){
                response += to_string(node) + " ";
            }
            send(clientSocket, response.c_str(), response.size(), 0);

        }
    }
}

void runServer(int port, const string& filename){

    //Read graph from file
    Graph graph = readGraphFromFile(filename);

    //create sever socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); //adress domain, indecate TCP socket, 0
    if(serverSocket < 0){
        cerr << "Error: Unable to create server socket" << endl;
        return;
    }

    //Bind sever socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; //IPv4 adress domain
    serverAddr.sin_port = htons(port); //4444
    serverAddr.sin_addr.s_addr = INADDR_ANY; //will listen to all avilable connections

    int n = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(n<0){
        cerr << "Error unable to bind server sockt" << endl;
        close(serverSocket);
        return;
    }

    //Listen for connections
    listen(serverSocket, 5);
    cout << "Server listening to port "<< port <<endl;

    //Accepet and handle client connections
    while(true){
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);


        if(clientSocket <0){
            cerr << "Error: Unable to accept client connection" << endl;
            continue;
        }
        cout << "Client connected" << endl;

        //Handle client in a seprate thread
        thread clientThread(handleClient, clientSocket, ref(graph));
        clientThread.detach(); //Detach thread to allow it to run independtly
    }

    //close server socket
    close(serverSocket);
}



//main
int main(int argc, char* argv[]){   
    if(argc != 3){
        cerr << "Usage: " << argv[0] << " <filename> <port>" << endl;
        return 1;
    }

    string filename = argv[1];
    int port = atoi(argv[2]);


    runServer(port, filename);

    return 0;
}