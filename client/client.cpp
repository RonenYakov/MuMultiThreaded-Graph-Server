//Daniel Dahan 318840196
//Oleg Muraviov 321163446
//Ronen Yakoubov 207688581 
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

// Function to send request to server and receive response
void sendRequest(const char* serverIP, int port, int start, int end){
    //Create client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); //adress domain, indecate TCP socket, 0
    if(clientSocket<0){
        cerr <<"Error: unable to create client socket" << endl;
        return;
    }

    //Connect to srever
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; //IPv4 adress domain
    serverAddr.sin_port = htons(port); //4444
    serverAddr.sin_addr.s_addr = INADDR_ANY; //will listen to all avilable connections

    int n = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(n<0){
        cerr << "Error unable to connect server" << endl;
        close(clientSocket);
        return;
    }

    //send request to srver
    char buffer[256];
    sprintf(buffer, "%d %d",start, end);
    send(clientSocket, buffer, sizeof(buffer), 0);

    //Receive response from server
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout <<buffer << endl; //Shortest path

    //close client sock
    close(clientSocket);
}



//main
int main(int argc, char* argv[]) {
    if(argc != 5){
        cerr << "Usage: " << argv[0] << " <serverIP> <port> <start> <end>" << endl;
        return 1;
    }

    const char* serverIP = argv[1];
    int port = atoi(argv[2]);
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);


    sendRequest(serverIP,port, start, end);

    return 0;
};
