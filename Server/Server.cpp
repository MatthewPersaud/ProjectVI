#include <windows.networking.sockets.h>
#include <iostream>
#include <thread>
#include <mutex>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

//Declare struct
struct StorageTypes
{
	//TODO add Sum variable to increase avg calc. efficiency

	unsigned int size = 0;
	float* pData;
};

//Function declarations
void UpdateData(unsigned int, float, StorageTypes*);
float CalcAvg(unsigned int, StorageTypes*);





void NewThread(SOCKET socket) {
	//Create array of structs (of Storage Types)
	StorageTypes threadData[7];

	SOCKET ConnectionSocket = socket;

	cout << "thread is continuing" << endl;

	char RxBuffer[128] = {};

	while (RxBuffer[0] != '*')
	{
		float fValue = 0;
		//allocate memory
		//TODO Check if this memory needs to be freed
		memset(RxBuffer, 0, sizeof(RxBuffer));
		//receive parameter name from client
		recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
		//send confirmation message back
		send(ConnectionSocket, "ACK", sizeof("ACK"), 0);

		//perform calculations based on RxString contents
		if (strcmp(RxBuffer, "ACCELERATION BODY X") == 0)
		{
			//allocate memory
			memset(RxBuffer, 0, sizeof(RxBuffer));
			//receive data from client
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			//convert size_t to float
			fValue = (float)atof(RxBuffer);
			//perform calculations
			UpdateData(0, fValue, threadData);
			fValue = CalcAvg(0, threadData);
		}
		else if (strcmp(RxBuffer, "ACCELERATION BODY Y") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(1, fValue, threadData);
			fValue = CalcAvg(1, threadData);
		}
		else if (strcmp(RxBuffer, "ACCELERATION BODY Z") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(2, fValue, threadData);
			fValue = CalcAvg(2, threadData);
		}
		else if (strcmp(RxBuffer, "TOTAL WEIGHT") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(3, fValue, threadData);
			fValue = CalcAvg(3, threadData);
		}
		else if (strcmp(RxBuffer, "PLANE ALTITUDE") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(4, fValue, threadData);
			fValue = CalcAvg(4, threadData);
		}
		else if (strcmp(RxBuffer, "ATTITUDE INDICATOR PICTH DEGREES") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(5, fValue, threadData);
			fValue = CalcAvg(5, threadData);
		}
		else if (strcmp(RxBuffer, "ATTITUDE INDICATOR BANK DEGREES") == 0)
		{
			memset(RxBuffer, 0, sizeof(RxBuffer));
			size_t result = recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			fValue = (float)atof(RxBuffer);
			UpdateData(6, fValue, threadData);
			fValue = CalcAvg(6, threadData);
		}
		else//TODO perform time calculations
		{
			//allocate memory
			memset(RxBuffer, 0, sizeof(RxBuffer));
			//receives data from client
			recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
			//set value to 0
			fValue = 0.0;
		}
		//create return buffer
		//TODO properly allocate
		char Tx[128];
		//put value into char array
		sprintf_s(Tx, "%f", fValue);
		//send char array back to client
		send(ConnectionSocket, Tx, sizeof(Tx), 0);
	}

	cout << "THREAD IS FINISHED" << endl;
	closesocket(ConnectionSocket);	//closes incoming socket

}



int main()
{
	//variable declarations
	WSADATA wsaData;
	SOCKET ServerSocket, ConnectionSocket;
	sockaddr_in SvrAddr;

	//TODO Check for eff.
	//Setting up socket communication
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == SOCKET_ERROR)
		return -1;

	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27001);
	bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));

	if (ServerSocket == SOCKET_ERROR)
		return -1;


	//REWORK?
	// 
	//----------------------------------
	listen(ServerSocket, 1);


	//wait for client connection
	//and error checking
	while (true) {

		cout << "Waiting for client connection\n" << endl;

		ConnectionSocket = SOCKET_ERROR;
		ConnectionSocket = accept(ServerSocket, NULL, NULL);

		//thread goes brrrrrrr
		thread (NewThread, ConnectionSocket).detach();

		cout << "Connection Established" << endl;
	}

	//----------------------------------

	//bruh
	
	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources

	return 1;
}

void UpdateData(unsigned int uiIndex, float value, StorageTypes* threadData)
{
	//On first function call
	if (threadData[uiIndex].size == 0)
	{
		//update value of data
		threadData[uiIndex].pData = new float[1];
		threadData[uiIndex].pData[0] = value;
		//set size to 1
		threadData[uiIndex].size = 1;
	}
	else
	{
		//TODO fix pointer shenanigans

		//create new array that is one larger than previous array
		float* pNewData = new float[threadData[uiIndex].size + 1];

		//populate new array with data from old array
		for (unsigned int x = 0; x < threadData[uiIndex].size; x++)
			pNewData[x] = threadData[uiIndex].pData[x];

		//add new value to last space of new array
		pNewData[threadData[uiIndex].size] = value;

		//empty old array
		delete[] threadData[uiIndex].pData;

		//repopulate old arrary with new array
		threadData[uiIndex].pData = pNewData;

		//increment size of old array
		threadData[uiIndex].size++;
	}
}

float CalcAvg(unsigned int uiIndex, StorageTypes* threadData)
{
	//declare variable
	float Avg = 0;

	//get sum
	for (unsigned int x = 0; x < threadData[uiIndex].size; x++)
		Avg += threadData[uiIndex].pData[x];

	//calculate average
	Avg = Avg / threadData[uiIndex].size;
	return Avg;
}