/////////////////////////////////////////////////////////////////////
// Client.cpp - File to Demonstrate the client Side Functionality  //
// as per requirements of project 4								   //
//                                                                 //
// Komal Gujarathi  -	SUID -  211778351						   //
// Phone - 315-7446116											   //
// 																   //
/////////////////////////////////////////////////////////////////////

#include<iostream>
#include"Client.h"
#include "../FileSystem/FileSystem.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Logger/Logger.h"
#include<thread>
#include<vector>
#include<ctime>
#include <iostream>
#include<sstream>

using namespace Logging;

//----< factory for creating messages >------------------------------
/*
 * This function only creates one type of message for this demo.
 * - To do that the first argument is 1, e.g., index for the type of message to create.
 * - The body may be an empty string.
 * - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
 *   expects the receiver EndPoint for the toAddr attribute.
 */
HttpMessage ClientSender::makeMessage(size_t n, const std::string& body,const EndPoint &myep, const EndPoint& ep)
{
  HttpMessage msg;
  HttpMessage::Attribute attrib;
  switch (n)
  {
  case 1:
    msg.clear();
    msg.addAttribute(HttpMessage::attribute("POST", "Message"));
    msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
    msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
    msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myep));

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

void ClientSender::sendMessage(HttpMessage& msg, Socket& socket)
{
  std::string msgString = msg.toString();
  socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*

 * - Sends a message to tell receiver a file is coming.
 * - Then sends a stream of bytes until the entire file
 *   has been sent.
 * - Sends in binary mode which works for either text or binary.
 */
//bool MsgClient::sendFile(std::string directory, std::string& filename, Socket& socket)

bool ClientSender::sendFile(std::string category, const std::string& filename, Socket& socket)
{
  FileSystem::FileInfo fi(filename);
  size_t fileSize = fi.size();
  std::string sizeString = Converter<size_t>::toString(fileSize);
  FileSystem::File file(filename);
  file.open(FileSystem::File::in, FileSystem::File::binary);
  if (!file.isGood())
  {
	  std::cout << "\nUNABLE TO OPEN FILE\n"<< filename<<"\n";
	  return false;
  }
  std:: size_t found = filename.find_last_of("/\\");
  std::string fname = filename.substr(found+1);

  HttpMessage msg = makeMessage(1, "","localhost::8081" ,"localhost::8080");
  msg.addAttribute(HttpMessage::Attribute("FILE", fname));
  msg.addAttribute(HttpMessage::Attribute("CATEGORY",category));
  msg.addAttribute(HttpMessage::Attribute("COMMAND","CHECKIN"));
  msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
  sendMessage(msg, socket);
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
  std::cout << "\nClient sent File:" << fname << "\n";
  return true;
}
//----< this defines the behavior of the client >--------------------

void ClientSender::execute()
{
  try
  {
    SocketSystem ss;
    SocketConnecter si;
    while (true)
    {
		HttpMessage request = sendingQueue.deQ();
		std::string files,category,message;
		std::string command = request.findValue("COMMAND");
		std::string arg1, arg2;
		if (command == "CHECKIN") {
			files = request.findValue("FILES");
			arg1 = files;
			category = request.findValue("CATEGORY");
			arg2 = category;
		}
		else if (command == "SENDMESSAGE"|| command == "GETNAMES" || command == "GETGIVENFILE" || command == "DELETEFILE"){
			arg1 = "";
			arg2 = "";
		}

		else {
			std::cout << "\nCommand not Found\n";	
		}

		std::string body = request.bodyString();

		processRequest(si, command, arg1,arg2,body,request);
    }
  }
  catch (std::exception& exc)
  {
   
    std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
   
  }
}


//-----------< process the request received from server >----------------

void ClientSender::processRequest(SocketConnecter& si, std::string request, std::string arg1, std::string arg2, std::string body, HttpMessage& msgtosend){
	using namespace Logging;
	try{
		bool connected = Connect(si, "localhost", 8080);
		if (connected && request == "CHECKIN"){
			std::string filepaths = arg1;
			std::string category = arg2;
			std::istringstream all_files(filepaths);
			std::string file;
			while (std::getline(all_files, file, ',')){
				sendFile(category,file,si);
			}
			HttpMessage hMsg;
			hMsg = makeMessage(1, "done","localhost:8081", "localhost:8080");
			hMsg.addAttribute(HttpMessage::attribute("COMMAND", "QUIT"));
			hMsg.addAttribute(HttpMessage::attribute("CATEGORY", category));
			sendMessage(hMsg, si);
		}
		else if (connected && request == "SENDMESSAGE"){
			sendMessage(msgtosend,si);
		}
		else if (connected && request == "GETNAMES"){
			sendMessage(msgtosend,si);
		}
		else if (connected && request == "GETGIVENFILE"){
			sendMessage(msgtosend, si);
		}
		else if (connected && request == "DELETEFILE"){
			sendMessage(msgtosend,si);
		}
	}
	catch (std::exception& exc){
		std::cout << "Exception caugut"<<exc.what();
	}
}

//--------< connect to the server >-----------------

bool ClientSender::Connect(SocketConnecter& si, std::string ipAddress, int port)
{
	while (!si.connect(ipAddress, port))
	{
		std::cout << "\nClient waiting for Connection\n";
        ::Sleep(100);
	}
	return true;
}

//----< post message from WPF application >---------------

void ClientSender::postHMessage(const HttpMessage& msg)
{
	sendingQueue.enQ(msg);
}

//test msg to show file upload
void ClientSender::testFileUpload()
{
	std::cout<< "\n1. Sending Message to Upload Files to CATEGORY2\n";
	HttpMessage messge = makeMessage(1, "", "localhost::8081", "localhost::8080");
	messge.addAttribute(HttpMessage::attribute("COMMAND", "CHECKIN"));
	messge.addAttribute(HttpMessage::attribute("FILES", "../TestFiles/Logger.cpp,../TestFiles/Logger.h,../Parser/Parser.h,../Parser/Parser.cpp,../StylesAndScripts/script.js,../StylesAndScripts/style.css"));
	messge.addAttribute(HttpMessage::attribute("CATEGORY", "CATEGORY2"));
	postHMessage(messge);

}
//test messge to upload particular files in particular category
void ClientSender::testFileUpload2()
{
	std::cout << "\n1. Sending Message to Upload Files to CATEGORY1\n";
	HttpMessage messge = makeMessage(1, "", "localhost::8081", "localhost::8080");
	messge.addAttribute(HttpMessage::attribute("COMMAND", "CHECKIN"));
	messge.addAttribute(HttpMessage::attribute("FILES", "../TestFiles/Sockets.cpp,../TestFiles/Sockets.h,../StylesAndScripts/script.js,../StylesAndScripts/style.css"));
	messge.addAttribute(HttpMessage::attribute("CATEGORY", "CATEGORY1"));
	postHMessage(messge);

}

//test messge to upload particular files in particular category for deleting 
void ClientSender::testFileUpload3()
{
	std::cout << "\n1. Sending Message to Upload Files to CATEGORY3\n";
	HttpMessage messge = makeMessage(1, "", "localhost::8081", "localhost::8080");
	messge.addAttribute(HttpMessage::attribute("COMMAND", "CHECKIN"));
	messge.addAttribute(HttpMessage::attribute("FILES", "../TestFiles/Sockets.cpp,../TestFiles/Sockets.h,../StylesAndScripts/script.js,../StylesAndScripts/style.css"));
	messge.addAttribute(HttpMessage::attribute("CATEGORY", "CATEGORY3"));
	postHMessage(messge);

}


//----< WPF application will call this method to get message from receving queue. >---------------

std::string ClientSender::getMessage()
{
	HttpMessage result = rcvQueue.deQ();

	std::string files = result.findValue("FILESSTRING");
	std::cout << "files: " << files;
	return files;
}

//testing sending simple msgs

void ClientSender::testSendMessage() {

	std::cout << "\n2. Send Message Command\n";
	HttpMessage message1 = makeMessage(1, "", "localhost::8081", "localhost::8080");
	message1.addAttribute(HttpMessage::attribute("COMMAND", "SENDMESSAGE"));
	message1.addAttribute(HttpMessage::attribute("MESSAGE", "Hurrahh!!!!CLient Able To Send Messages!!"));
	postHMessage(message1);

}

//Test message to demonstrate that teh clint can receive all the file names 
//in the given category.

void ClientSender::testGetName()
{
	std::cout << "\n3. Get all Files in CATEGORY 1\n";
	HttpMessage message2 = makeMessage(1, "", "localhost::8081", "localhost::8080");
	message2.addAttribute(HttpMessage::attribute("COMMAND", "GETNAMES"));
	message2.addAttribute(HttpMessage::attribute("CATEGORY", "CATEGORY1"));
	postHMessage(message2);

}

//testing msgs to show that on request of the client, the serber sends all the 
//files required by client

void ClientSender::testGetFiles()
{
	std::cout << "\n4. Get Requested File\n";
	HttpMessage msg3 = makeMessage(1, "", "localhost::8081", "localhost::8080");
	msg3.addAttribute(HttpMessage::attribute("COMMAND", "GETGIVENFILE"));
	msg3.addAttribute(HttpMessage::attribute("FILENAME", "Sockets.cpp"));
	msg3.addAttribute(HttpMessage::attribute("CATEGORY", "CATEGORY1"));
	postHMessage(msg3);
	std::cout << "\nPlease check downloads folder\n";
}

//test messgae to show that client can delete particular file on server
void ClientSender::testDeleteFile()
{
	std::cout << "\n5. Delete 'Sockets.h.html' File in CATEGORY3\n";
	HttpMessage msg4 = makeMessage(1, "", "localhost::8081", "localhost::8080");
	msg4.addAttribute(HttpMessage::attribute("COMMAND", "DELETEFILE"));
	msg4.addAttribute(HttpMessage::attribute("FILE", "Sockets.h.html"));
	msg4.addAttribute(HttpMessage::attribute("CATEGORY", "CATEGORY3"));
	postHMessage(msg4);
}
// ---------< parse the string message received to HTTP >-----------------------

HttpMessage ClientReceiver::getParseMessage(Socket& socket)
{
	HttpMessage msg;
	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}
	return msg;
}
//----< this defines processing to frame messages >------------------

HttpMessage ClientReceiver::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg = getParseMessage(socket);
	// If client is done, connection breaks and recvString returns empty string

	if (msg.attributes().size() == 0)
	{
		connectionClosed_ = true;
		return msg;
	}
	// read body if POST - all messages in this demo are POSTs
	if (msg.attributes()[0].first == "POST")
	{
		std::string command = msg.findValue("COMMAND");
		if (command == "TESTMSG")
		{
			Sleep(100);
			std::cout << "\nClient Received Test Message from Server\n";
		}
		else if (command == "GETNAMES")
		{
			Sleep(100);
			std::cout << "\nServer Sent File Names: \n" << msg.findValue("FILESSTRING");
		}
		else if (command == "GETGIVENFILE")
		{
			Sleep(100);
			parseFileMessage(socket, msg);
			std::cout << "\nServer Sent Requested Files\n";
		}
	}
	return msg;
}

//parse the files reecived from server

HttpMessage ClientReceiver::parseFileMessage(Socket&socket, HttpMessage msg)
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

//last argument in command line argument is the index page for Requirement 
//to demonstarte the requirement to display any files specified in the 
//command line using the browser of your choice
void ClientReceiver::lauchBrowserForArgumentInCommandLine(std::string path)
{
	//launching the browser for the file specifies by taking this path
	//from command line arguments 
	std::string command("start \"\" \"" + path + "\"");
	std::system(command.c_str());

}

//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool ClientReceiver::readFile(std::string category, const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string dir = "../Downloads/" + category;

	std::string fqname = dir + "/" + filename;
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
	while (true)
	{
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
	std::cout << "\nClient Received File: " << filename << std::endl;
	return true;
}

//----< receiver functionality is defined by this function >---------

void ClientReceiver::operator()(Socket socket)
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
			Show::write("\n\n  Server Terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

void ClientReceiver::startReceiving(BlockingQueue<HttpMessage>&msgQ)
{

	try
	{
		SocketSystem ss;
		SocketListener sl(listenPort, Socket::IP6);
		ClientReceiver cp(msgQ);
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/

		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			if (msg.bodyString() == "quit")
			{
				break;
			}
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}

}

//-----< start receiver and initilize socket on particular port>
void ClientSender::startReceiver()
{
	try
	{
		SocketSystem ss;
		SocketListener sl(8081, Socket::IP6);
		sl.start(clientReceiver);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}


//wpf application calls thiss

void ClientSender::start()
{
	::SetConsoleTitle(L"Client");
	startSenderThread();
	startReceiverThread();
}
//----< start sender thread. >---------------

void ClientSender::startSenderThread()
{
	std::thread senderThread(&ClientSender::execute, this);
	senderThread.detach();
}

//----< start receiver thread. >---------------

void ClientSender::startReceiverThread()
{
	std::thread receiverThread(&ClientSender::startReceiver, this);
	receiverThread.detach();
}

//----< entry point >------

#ifdef CLIENT
int main(int argc, char *argv[])
{
  ::SetConsoleTitle(L"Clients Running on Threads");
  //MsgClient cl1;

  std::cout << "Demonstrating client";

  ClientSender c1;
  c1.testFileUpload3(); //Upluads Files in Category 3
  c1.testFileUpload2(); // Uploads files in Category 2
  c1.testFileUpload(); // Uploads Files in Category 1
  c1.testGetFiles();  //Command to Download requested files
  c1.testSendMessage(); //Command to test simple message sending Functionality
  c1.testSendMessage(); 
  c1.testGetName(); //Get All the file names in a particular category
  c1.testDeleteFile(); //Delete Reuested File from the Category 3

  std::thread t1(
	  [&]() { c1.execute(); } 

  );
  t1.detach();

  //for receiving messages
  BlockingQueue<HttpMessage> recvMsgQ;
  ClientReceiver recvr(recvMsgQ);
  std::thread t2([&]() {recvr.startReceiving(recvMsgQ); });
  t2.join();

  
}
#endif