#pragma once
/////////////////////////////////////////////////////////////////////////
// Client.cpp - Demonstrates simple one-way HTTP messaging          //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016    
// Exteded By : Komal Gujarathi
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
*
* It's purpose is to use HTtp style messaging and Sockts to demonstrate
*Project #4.
*/
/*
* Required Files:
*   Client.cpp, Server.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*
* Public Interfaces - ClientReceiver
* startReceiving(); - to start to receive files from Server
* readFile();
* parseFileMessage(Socket&socket, HttpMessage msg);

* ClientSender
*
* startReceiving(BlockingQueue<HttpMessage>&msgQ);
* readFile(); read FIle to be sent to server
* parseFileMessage(); 
* postHMessages()  - function used to enque msgs from client side
*Build Process:
*--------------
*Using Visual Studio Command Prompt:
*
* version 1. CLient is able to Send And receive MEssages Files, filenames, 
* delete fiels from server.
*
* version 2. Test Messages are Created to Demonstarte the client Side of the project.
* Seperate Send Queue and Receive Queue is used to do msg and file transfer. 
* I did demonstrate automated test on client side and server side. Since I cannot 
* keep both GUI and Client as a startup project, I have not kept GUI as startup project.
*/
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>

using namespace Logging;
using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;


/////////////////////////////////////////////////////////////////////
// ClientReceiver class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You need to be careful using data members of this class
//   because each client handler thread gets a reference to this 
//   instance so you may get unwanted sharing.
// - I may change the SocketListener semantics (this summer) to pass
//   instances of this class by value.
// - that would mean that all ClientHandlers would need either copy or
//   move semantics.
//  Receiver
//

class ClientReceiver
{
public:
	ClientReceiver(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	void startReceiving(BlockingQueue<HttpMessage>&msgQ);
	bool readFile(std::string category, const std::string& filename, size_t fileSize, Socket& socket);
	HttpMessage parseFileMessage(Socket&socket, HttpMessage msg);
	void lauchBrowserForArgumentInCommandLine(std::string filename);
private:
	std::string clientpath_ = "../Downloads/";
	bool connectionClosed_;
	HttpMessage getParseMessage(Socket& socket);
	HttpMessage readMessage(Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
	int listenPort = 8081;
};




/////////////////////////////////////////////////////////////////////
// Client class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class ClientSender
{
public:
	using EndPoint = std::string;
	void processRequest(SocketConnecter& si, std::string msg, std::string path,std::string category,std::string body, HttpMessage &hmsg);
	ClientSender() :clientReceiver(rcvQueue) {}
	void execute(); 
	bool Connect(SocketConnecter& si, std::string ipAddress, int port);
	void postHMessage(const HttpMessage& msg); //Function used by the client to enque msgs to Blocking queue to be sent to server
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& myep, const EndPoint& ep);
	void testFileUpload();
	void testSendMessage(); //test messages 
	void testGetName();
	void testGetFiles();
	void testFileUpload2();
	void testFileUpload3();
	void testDeleteFile();
	std::string getMessage(); //used  by channel to get msgs from
	void start();
	void startSenderThread();
	void startReceiverThread();
	void startReceiver();

private:
	BlockingQueue<HttpMessage> rcvQueue;
	BlockingQueue<HttpMessage> sendingQueue;
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(std::string directory, const std::string& fqname , Socket& socket);
	ClientReceiver clientReceiver;
};

