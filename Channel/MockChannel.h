#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
/////////////////////////////////////////////////////////////////////////////
// MockChannel.h - Demo for CSE687 Project #4, Spring 2015                 //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - MockChannel reads from sendQ and writes to recvQ                      //
//                                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015               //
/////////////////////////////////////////////////////////////////////////////

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
using Message = std::string;
struct IMockChannel
{
public:
	virtual std::string getMessage() = 0;
	virtual void postMessage(const std::string& command, const std::string& body) = 0;
	virtual void start() = 0;
};



extern "C" {
	struct ObjectFactory
	{
		//DLL_DECL ISendr* createSendr();
		//DLL_DECL IRecvr* createRecvr();
		DLL_DECL IMockChannel* createMockChannel(std::string ip, int port);
	};
}

#endif


