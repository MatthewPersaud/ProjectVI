#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;

unsigned int GetSize();

int main()
{
	//connection variables
	WSADATA wsaData;
	SOCKET ClientSocket;
	sockaddr_in SvrAddr;
	unsigned int uiSize = 0;
	vector<string> ParamNames;
	char Rx[128];

	//setup connection
	//TODO Go through in detail later
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SvrAddr.sin_family = AF_INET;



	//change IP address to group member's IP for testing

	SvrAddr.sin_port = htons(27001);
	SvrAddr.sin_addr.s_addr = inet_addr("10.192.228.150");	//me



	connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));

	//get number of lines
	uiSize = GetSize();

	//loop through the following for the number of lines
	for (unsigned int l = 0; l < uiSize; l++)
	{
		//declare variable for input
		//and filename
		string strInput;
		//string fileName = "DataFile.txt";									//*
		//string fileName = "Telem_2023_1_24 16_12_58.txt";					//*
		string fileName = "Telem_czba-cykf_2023_1_24 16_24_34.txt";		//-
		//string fileName = "Telem_onroute_2023_2_2 20_6_11.txt";				//-

		//open file
		ifstream ifs(fileName);
		

		//read contents of datafile into strInput
		//up to the current spot on the mother loop
		//TODO this is inefficient, improve
		for (unsigned int iStart = 0; iStart < l; iStart++)
			getline(ifs, strInput);

		//TODO Test why this extra line is here
		getline(ifs, strInput);

		//If statement depending on where in the loop we are
		if (l > 0)
		{

			//communicate with
			size_t offset, preOffset;
			offset = preOffset = 0;
			unsigned int iParamIndex = 0;
			//while (offset != std::string::npos)
			while (iParamIndex != 8)
			{
				//Skip over reading first column, then read each subsequent column
				offset = strInput.find_first_of(',', preOffset + 1);
				string strTx = strInput.substr(preOffset + 1, offset - (preOffset + 1));

				//Send parameter name to server so that server can identify
				//which calculation to do
				send(ClientSocket, ParamNames[iParamIndex].c_str(), (int)ParamNames[iParamIndex].length(), 0);
				//receive confirmation from server (will always be "ACK")
				recv(ClientSocket, Rx, sizeof(Rx), 0);

				//Send to server for calculation of average
				send(ClientSocket, strTx.c_str(), (int)strTx.length(), 0);
				//Receive calculated average
				recv(ClientSocket, Rx, sizeof(Rx), 0);
				//print calculated average
				cout << ParamNames[iParamIndex] << " Avg: " << Rx << endl;

				//increment parameter index and starting position of reading
				preOffset = offset;
				iParamIndex++;
			}
		}

		//read in the names of the variables from the data file
		else
		{
			ParamNames.push_back("TIME STAMP");
			size_t offset, preOffset;
			offset = 0;
			preOffset = -1;
			//populate Paramnames with the column names for the variables, separated by commas
			while (offset != std::string::npos)
			{
				offset = strInput.find_first_of(',', preOffset + 1);
				string newParam = strInput.substr(preOffset + 1, offset - (preOffset + 1));
				ParamNames.push_back(newParam);
				preOffset = offset;
			}
		}
		//close file
		ifs.close();

	}

	//These three things let the server know that the file is done
	//sends "valuable header"
	send(ClientSocket, "*", 1, 0);
	//gets "valuable response"
	recv(ClientSocket, Rx, sizeof(Rx), 0);
	//sends "valuable data"
	send(ClientSocket, "*", 1, 0);


	//close connection
	closesocket(ClientSocket);
	WSACleanup();


	//system("pause");

	return 1;
}

//determine length of data file
unsigned int GetSize()
{
	//declare variables
	string strInput;
	unsigned int uiSize = 0;
	ifstream ifs("DataFile.txt");

	//while the file is open
	//read line by line, increment line number
	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			getline(ifs, strInput);
			uiSize++;
		}
	}

	//return number of lines
	return uiSize;
}