/////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Demo for CSE687 Project #4, Spring 2015               //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - MockChannel reads from sendQ and writes to recvQ                      //
//                                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015  
// Extended By - Komal Gujarthi, to support the msg transfer from GUI to Server

/////////////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "MockChannel.h"
#include <string>
#include <iostream>
#include "../MsgClient/Client.h"
#include "../HttpMessage/HttpMessage.h"

/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads messages from Sendr and writes messages to Recvr
//
class MockChannel : public IMockChannel
{
public:
	MockChannel(std::string ip, int port);
	void start();
	void postMessage(const std::string& msg, const std::string& body);
	std::string getMessage() {
		return client_.getMessage();
	}
private:
	ClientSender client_;
	bool stop_ = false;
};

//----< pass pointers to Sender and Receiver >-------------------------------

MockChannel::MockChannel(std::string ip, int port) : client_() {}

//----< creates thread to read from sendQ and echo back to the recvQ >-------

void MockChannel::start()
{
	std::cout << "\n  MockChannel starting up";
	client_.start();
}
//msg has values - CHECKIN, GETFILES, EXTRACT
void MockChannel::postMessage(const std::string& command, const std::string& msg)
{
	std::cout << "\nClient Fired Request\n";
	if (command == "SENDMESSAGE")
	{
		HttpMessage messge = client_.makeMessage(1, "", "localhost::8081", "localhost::8080");

		messge.addAttribute(HttpMessage::attribute("COMMAND", command));
		messge.addBody(msg);
		messge.addAttribute(HttpMessage::attribute("MESSAGE", msg));
		std::cout << "\n\n Command : " + command + "\n  body: " + msg + "\n";
		std::cout << "\n HttpMESSAGE: " + messge.toString();
		std::thread t2(
			[&]() {client_.postHMessage(messge); });
		t2.detach();
		//client_.postHMessage(messge);
	}
	else if (command == "CHECKIN")
	{
		std::cout << "Checkkkk";
		//file transfer message
		HttpMessage messge = client_.makeMessage(1, "", "localhost::8081", "localhost::8080");
		messge.addAttribute(HttpMessage::attribute("COMMAND", "CHECKIN"));
		messge.addAttribute(HttpMessage::attribute("FILES", msg));
		messge.addAttribute(HttpMessage::attribute("CATEGORY", "CATEGORY1"));
		
		std::thread t3(
			[&]() {client_.postHMessage(messge); });
		t3.detach();
	}
}

IMockChannel* ObjectFactory::createMockChannel(std::string ip, int port)
{
	return new MockChannel(ip, port);
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
	ObjectFactory objFact;
	ISendr* pSendr = objFact.createSendr();
	IRecvr* pRecvr = objFact.createRecvr();
	IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
	pMockChannel->start();
	pSendr->postMessage("Hello World");
	pSendr->postMessage("CSE687 - Object Oriented Design");
	Message msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pSendr->postMessage("stopping");
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pMockChannel->stop();
	pSendr->postMessage("quit");
	std::cin.get();
}
#endif
