/////////////////////////////////////////////////////////////////////
// Server.cpp -   Demonstrate one way communication				   //                              //
// and file transfer from server side, It has the publisher fuctinality. 
// which converts .cpp and.h files given by client to .html files
// Komal Gujarathi  -	SUID -  211778351					       //
// Phone - 3157446116											   //
/////////////////////////////////////////////////////////////////////

#include<iostream>
#include "Server.h"
#include "../CodePublisher/codePublisher.h"
#include "../Analyzer/Executive.h"
#include "../DepAnal/DepAnal.h"
#include "../TypeTable/TypeTable.h"
#include "../FileMgr/FileMgr.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/DataStore.h"
#include<cstdio>
#include<ctime>
#include<fstream>
//----< this defines processing to frame messages >------------------

HttpMessage ClientHandler::readMessage(Socket& socket){ 
	connectionClosed_ = false; HttpMessage msg;
  while (true){
    std::string attribString = socket.recvString('\n');
    if (attribString.size() > 1){
      HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
      msg.addAttribute(attrib); } else{ break;}
  }// If client is done, connection breaks and recvString returns empty string
  if (msg.attributes().size() == 0){ connectionClosed_ = true; return msg; }
  if (msg.attributes()[0].first == "POST"){
	  std::string command = msg.findValue("COMMAND");
	  if (command == "CHECKIN"){ return parseFileMessage(socket, msg); }
	  else if(command == "QUIT"){ //quit command says that all files have been uploaded, so dependecny anal can be done
		  std::cout << "\nAll Client Files received\n";
		  std::string category = msg.findValue("CATEGORY");
		  generateHtmlFiles(msg); }
	  else if (command == "GETNAMES") {
		  std::string fromAddr = msg.findValue("fromAddr");
		  std::string filesString = getFiles(msg.findValue("CATEGORY"));
		  msg.removeAttribute("toAddr");
		  msg.removeAttribute("fromAddr");
		  msg.addAttribute(HttpMessage::Attribute("toAddr", fromAddr));
		  msg.addAttribute(HttpMessage::Attribute("fromAddr", "localhost::8080"));
		  msg.addAttribute(HttpMessage::Attribute("FILESSTRING", filesString));
		  sender->postHMessage(msg); }
	  else if (command == "GETGIVENFILE"){ std::cout << "\nClient Requested to send file\n";
		  std::string fromAddr = msg.findValue("fromAddr");
		  std::string path = iisPath + "/" + msg.findValue("CATEGORY");
		  std::vector<std::string> vecFiles = FileSystem::Directory::getFiles(path, "*.HTML");
		  std::vector<std::string> vecFiles1 = FileSystem::Directory::getFiles(path, "*.css");
		  std::vector<std::string> vecFiles2 = FileSystem::Directory::getFiles(path, "*.js");
		  vecFiles.insert(vecFiles.end(),vecFiles1.begin(), vecFiles1.end());
		  vecFiles.insert(vecFiles.end(), vecFiles2.begin(), vecFiles2.end());
		  for each (auto file in vecFiles) {
			  SocketSystem ss;
			  SocketConnecter si;
			  while (!si.connect("localhost", 8081)){ ::Sleep(100); }
			  sendFile(msg.findValue("CATEGORY"), path+"/"+file, si,fromAddr); }
		  msg.removeAttribute("toAddr");
		  msg.removeAttribute("fromAddr");
		  msg.addAttribute(HttpMessage::Attribute("toAddr", fromAddr));
		  msg.addAttribute(HttpMessage::Attribute("fromAddr", "localhost::8080")); }
	  else if (command == "DELETEFILE") { deleteHtmlFile(msg); }
  } return msg;
}



//----< send file using socket >-------------------------------------
/*

* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/

bool ClientHandler::sendFile(std::string category, const std::string& filename, Socket& socket,std::string fromAddr)
{
	FileSystem::FileInfo fi(filename);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(filename);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
	{
		std::cout << "\nUNABLE TO OPEN FILE\n" << filename << "\n";
		return false;
	}
	std::size_t found = filename.find_last_of("/\\");
	std::string fname = filename.substr(found + 1);

	HttpMessage msg = sender->makeMessage(1, "",fromAddr);
	msg.addAttribute(HttpMessage::Attribute("FILE", fname));
	msg.addAttribute(HttpMessage::Attribute("CATEGORY", category));
	msg.addAttribute(HttpMessage::Attribute("COMMAND", "GETGIVENFILE"));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sender->sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	std::cout << "\nServer sent File:\n" << fname << "\n";
	return true;
}


// --------------< returns list of server files >--------------------
std::string ClientHandler::getFiles(std::string category)
{
	std::cout << "\nCategory: " << category;

	std::string stringFiles = ",";
	std::string path = iisPath + "/" + category;
	//std::string path = "../HTMLFiles/" + category;
	std::vector<std::string> vecFiles = FileSystem::Directory::getFiles(path, "*.HTML");

	if (vecFiles.size()>0)
		stringFiles.clear();
	for each (auto str in vecFiles)
	{
		stringFiles.append(str);
		stringFiles.append(",");
	}

	return stringFiles;

}
//delete html files
void ClientHandler::deleteHtmlFile(HttpMessage& msg)
{
	std::string category = msg.findValue("CATEGORY");
	std::string file = msg.findValue("FILE");
	std::string fileStr = iisPath + "/" + category + "/" + file;

	std::cout << "\nDeleting file: \n" + file;
	if (std::remove(fileStr.c_str()) != 0)
		perror("\nError Deleting Flie");
	else
		puts("\nFile Successfully deleted\n");
}

//generate html files
void ClientHandler::generateHtmlFiles( HttpMessage& msg)
{
	std::string category = msg.findValue("CATEGORY");
	CodePublisher publisher;
	using DepStore = std::unordered_map<std::string, std::set<std::string>>;
	CodeAnalysis::CodeAnalysisExecutive exec;
	using Files = std::unordered_map<std::string, std::vector<std::string>>;
	try {
		std::string path1 = iisPath + "/" + category;
		bool succeeded = exec.ProcessArguments(path1);
		exec.getSourceFiles();
		exec.stopLogger();
		exec.processSourceCode(false);
		Files fm = exec.getFileMap();
		//creating typetable
		TypeTable tt;
		tt.createTypeTable();
		Store typeTable = tt.getTypeTable();
		//dependency analysis
		DepAnal danal;
		danal.tokenizeForDependencies(fm, typeTable);
		DepStore dependencies = danal.getDependencies();
		publisher.processFiles(fm, dependencies, category,iisPath); 
	}
	catch (std::exception e)
	{
		std::cout << "Exception occurred while converting files to HTML";
	}

}


//parse the files received from client
HttpMessage ClientHandler::parseFileMessage(Socket&socket, HttpMessage msg)
{
	std::string filename = msg.findValue("FILE");
	std::string directory = msg.findValue("CATEGORY");
	if (filename != "")
	{
		size_t contentSize;
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "")
			contentSize = Converter<size_t>::toValue(sizeString);
		else
			return msg;

		readFile(directory, filename, contentSize, socket);
	}

	if (filename != "")
	{
		// construct message body

		msg.removeAttribute("content-length");
		std::string bodyString = "<file>" + filename + "</file>";
		std::string sizeString = Converter<size_t>::toString(bodyString.size());
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addBody(bodyString);
	}
	else
	{
		size_t numBytes = 0;
		size_t pos = msg.findAttribute("content-length");
		if (pos < msg.attributes().size())
		{
			numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
		}
	}
	return msg;
}
//----< read a binary file from socket and save >--------------------
/*
 * This function expects the sender to have already send a file message, 
 * and when this function is running, continuosly send bytes until
 * fileSize bytes have been sent.
 */
bool ClientHandler::readFile(std::string category,const std::string& filename, size_t fileSize, Socket& socket)
{
	
	std::string dir1 = iisPath + "/"+category;
	FileSystem::Directory::create(dir1);
	std::string fqname = dir1 + "/" + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	
	if (!file.isGood())
  {
    Show::write("\n\n  can't open file " + fqname);
    return false;
  }

  const size_t BlockSize = 2048;
  Socket::byte buffer[BlockSize];

  size_t bytesToRead;
  while (true){
    if (fileSize > BlockSize)
      bytesToRead = BlockSize;
    else
      bytesToRead = fileSize;
    socket.recv(bytesToRead, buffer);
    FileSystem::Block blk;
    for (size_t i = 0; i < bytesToRead; ++i)
      blk.push_back(buffer[i]);
    file.putBlock(blk);
    if (fileSize < BlockSize)
      break;
    fileSize -= BlockSize;
  }
  file.close();
  std::cout << "\nServer Received File:\n " << filename << std::endl;
  return true;
}
//----< receiver functionality is defined by this function >---------

void ClientHandler::operator()(Socket socket)
{
  /*
   * There is a potential race condition due to the use of connectionClosed_.
   * If two clients are sending files at the same time they may make changes
   * to this member in ways that are incompatible with one another.  This
   * race is relatively benign in that it simply causes the readMessage to 
   * be called one extra time.
   *
   * The race is easy to fix by changing the socket listener to pass in a 
   * copy of the clienthandler to the clienthandling thread it created.  
   * I've briefly tested this and it seems to work.  However, I did not want
   * to change the socket classes this late in your project cycle so I didn't
   * attempt to fix this.
   */
  while (true)
  {
    HttpMessage msg = readMessage(socket);
	if (connectionClosed_ || msg.bodyString() == "quit")
    {
      Show::write("\n\n  clienthandler thread is terminating");
      break;
    }
    msgQ_.enQ(msg);
  }
}

// -----------< starts server receiver >------------------

void ServerSender::startReceiver()
{
	try
	{
		SocketSystem ss;
		SocketListener sl(port_, Socket::IP6);

		sl.start(serverReceiver);
		std::cout.flush();
		std::cin.get();
	}
	catch (std::exception& ex)
	{
		Show::write("  Exception caught:");
		Show::write(std::string("\n  ") + ex.what() + "\n\n");
	}
}



//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage ServerSender::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{

	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

void ServerSender::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}

//-----------< process the request received from server >----------------

void ServerSender::processRequest(SocketConnecter& si,HttpMessage &request)
{
	using namespace Logging;
	try
	{
		while (!si.connect(ip_, 8081))
		{
			Show::write("\n server waiting to connect");
			::Sleep(100);
		}
		std::string command = request.findValue("COMMAND");
		if ( command == "TESTMSG")
		{
			Sleep(100);
			std::cout << "\nTest Message From Server\n";
		}
		else if (command == "GETFILES")
		{
			Sleep(100);

			std::cout << "\nServer Sending Files in A given Category To Client\n";	
		}
		sendMessage(request, si);
	}
	catch (std::exception& exc)
	{
		std::cout << "Exception caugut" << exc.what();
	}
}

// ---------< starts server sender >----------------

void ServerSender::startSender()
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (true)
		{
			auto request = queue.deQ();
			processRequest(si, request);
		}
	}
	catch (std::exception& ex)
	{
		Show::write("  Exception caught:");
		Show::write(std::string("\n  ") + ex.what() + "\n\n");
	}
}

//----< post message from WPF application >---------------

void ServerSender::postHMessage(const HttpMessage& msg)
{
	queue.enQ(msg);
}

void ServerSender::testMessages()
{
	HttpMessage messge = makeMessage(1, "Test Message", "localhost::8081");
	messge.addAttribute(HttpMessage::attribute("COMMAND", "TESTMSG"));
	messge.addAttribute(HttpMessage::attribute("MESSAGE", "Testing Servr Client Communication..."));
	postHMessage(messge);
	std::cout << "\nTest Message Sent\n";
}


// -----------< starts server sender on C++ thread >------------------

void ServerSender::startSenderThread()
{
	std::thread senderThread(&ServerSender::startSender, this);
	senderThread.join();
}

// -----------< starts server receiver on C++ thread >------------------


void ServerSender::startReceiverThread()
{
	std::thread receiverThread(&ServerSender::startReceiver, this);
	receiverThread.detach();
}

//----< test stub >--------------------------------------------------
#ifdef SERVER

int main(int argc, char *argv[])
{
	CodeAnalysis::CodeAnalysisExecutive exec;

  ::SetConsoleTitle(L"HttpMessage Server - Runs Forever");
  bool succeeded = exec.ProcessCommandLine(argc, argv);
  
  if (!succeeded) {
	  return 1;
  }
  std::string iisPath = argv[1];
  std::cout << "\nPath: " << iisPath;
  
  Show::attach(&std::cout);
  Show::start();

  std::cout << "\nDemonstrating server sender\n";

  BlockingQueue<HttpMessage> msgQ;
  ServerSender server_(msgQ,iisPath);
  server_.startReceiverThread();
  server_.startSenderThread();

}
#endif