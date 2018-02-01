#pragma once
/////////////////////////////////////////////////////////////////////////
// Server.h- Demonstrates simple one-way HTTP style messaging    //
//                 and file transfer                                   //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files. It has COde publisher Functionality. 
* IIS Server is ALso Set up. ALl server fiels get stored in IIS Folder.
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*   CodeAnalyzer.h CodePublisher.cpp
*   Code Publisher CodeAnalyzer.cpp
*   TypeTable.cpp, TypeTable.h
*   DepeAnal.cpp , DepenAnal.h 
*  Build Command:
*  --------------
*  devenv CppCli-WPF-App.sln
*  - this builds C++\CLI client application and native mock channel DLL

Public Interfaces - 
* Server Sender ( Sender Class of Server)
*  void processRequest(SocketConnecter& si,HttpMessage &message); //process the requests from client
*  void postHMessage(const HttpMessage& msg);
*  void startSenderThread();
*  void startReceiverThread();
*  BlockingQueue<HttpMessage>& getServerQ() { return queue; }
*  void sendMessage(HttpMessage& msg, Socket& socket);
*  HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep); //make messages to be sent

*ClientHandler (Receiver Class of Server)
*void generateHtmlFiles(HttpMessage &msg);
*void deleteHtmlFile(HttpMessage &msg);
*static std::string iispath;
*std::string getFiles(std::string category);
*std::string serverpath_ = "../ServerFiles/";
*std::string iisPath;
*bool connectionClosed_;
*HttpMessage readMessage(Socket& socket);
*bool readFile(std::string directory,const std::string& filename, size_t fileSize, Socket& socket);


Instructions to run - 
* IIS Virtual directory path needs to be given as the command line argument on the server side.
*
*  Maintenance History:
*  --------------------
Version 1. Server receives and sends files to client, as well as now able to send publish files
* version 2. Files get stored to the Virtual Diectory Set up as a path you give on command line.
*/

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <ctime>
using namespace Logging;

using Show = StaticLogger<1>;
using namespace Utilities;

/////////////////////////////////////////////////////////////////////
// ClientHandler class
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
class ServerSender;
class ClientHandler
{
public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	ClientHandler(BlockingQueue<HttpMessage>& msgQ, ServerSender &sndr, std::string iispath) : msgQ_(msgQ), sender(&sndr),iisPath(iispath) {
		std::cout << "Client Handler constructor";
	}
	void operator()(Socket socket);
	bool sendFile(std::string category, const std::string& filename, Socket& socket,std::string fromAddr);

	// --------------< returns list of server files >--------------------
	ServerSender *sender;
private:
	void generateHtmlFiles(HttpMessage &msg);
	void deleteHtmlFile(HttpMessage &msg);
	static std::string iispath;
	std::string getFiles(std::string category);
	std::string serverpath_ = "../ServerFiles/";
	std::string iisPath;
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(std::string directory,const std::string& filename, size_t fileSize, Socket& socket);
	HttpMessage parseFileMessage(Socket& socket, HttpMessage message);
	BlockingQueue<HttpMessage>& msgQ_;
};

/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class ServerSender
{
public:
	using EndPoint = std::string;
	ServerSender(BlockingQueue<HttpMessage>& q, std::string iisPath) : queue(q), serverReceiver(queue, *this,iisPath) {}
	void processRequest(SocketConnecter& si,HttpMessage &message);
	void testMessages();
	void postHMessage(const HttpMessage& msg);
	void startSenderThread();
	void startReceiverThread();
	BlockingQueue<HttpMessage>& getServerQ() { return queue; }
	void sendMessage(HttpMessage& msg, Socket& socket);
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);

private:
	void startReceiver();
	void startSender();
	int port_ = 8080;
	std::string ip_ = "localhost";
	BlockingQueue<HttpMessage> queue;
	ClientHandler serverReceiver;
};
