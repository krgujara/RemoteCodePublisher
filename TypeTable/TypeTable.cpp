///////////////////////////////////////////////////////////////
// TypeTable.cpp - Creates TypeTable for dependency analysis //                                 //
//                                                           //
// Language:    Visual C++ 2015                              //
// Platform:    Windows 10 Macbook                           //
// Author:      Komal Gujarathi, Syracuse University         //
//              (315) 744 6116, krgujara@syr.edu             //
///////////////////////////////////////////////////////////////

#include "TypeTable.h"

//-----<Function which returns typetable>------
Store TypeTable::getTypeTable()
{
	return typetablestore;
}

void TypeTable::add(std::string _type, TypeInfo _typeinfo)
{
	typetablestore[_type].push_back(_typeinfo);
}

//-----<Displays typetable>------
void TypeTable::displayTypeTable()
{

	std::cout << "\n------------TYPE TABLE----------\n";
	Keys keys = setOfTypes();
	for (Type key : keys)
	{
		std::cout << "\n\n";
		std::cout << "\n\nKEY(Type name):   " << key;

		std::cout << "\nVALUE :  \n{";
		for (std::vector<TypeInfo>::iterator it = typetablestore[key].begin(); it != typetablestore[key].end(); ++it)
		{
			std::cout << "\nFile Name : " << it->getFileName();
			std::cout << "\nNamespace : " << it->getNameSpace();
			std::cout << "\nType      : " << it->getTypeName();

			std::cout << "\n";
		}

		std::cout << "\n}";
	}
}

//getterr method
std::string TypeInfo::getFileName()
{
	return _filename;
}


//getterr method
std::string TypeInfo::getNameSpace()
{
	return _namespace;
}

void TypeInfo::setFileName(std::string s)
{
	_filename = s;
}

void TypeInfo::setNameSpace(std::string s)
{
	_namespace = s;
}

void TypeInfo::setTypeName(std::string s)
{
	_typename = s;
}

//getterr method
std::string TypeInfo::getTypeName()
{
	return _typename;
}




//-----<returns all the key names in typetable>------
std::vector<std::string> TypeTable::setOfTypes()
{

	Keys keys;
	for (Item item : typetablestore)
	{
		keys.push_back(item.first);
	}
	return keys;
}

//-----<returns the size of typetable>------
size_t TypeTable::count()
{
	return typetablestore.size();
}



#ifdef TYPETABLE_TEST

int main(int argc, char* argv[])
{
	using namespace CodeAnalysis;
	CodeAnalysisExecutive exec;

	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded)
		{
			return 1;
		}
		exec.setDisplayModes();
		exec.startLogger(std::cout);

		std::ostringstream tOut("CodeAnalysis");
		exec.showCommandLineArguments(argc, argv);

		exec.getSourceFiles();
		exec.processSourceCode(true);

		TypeTable ta;
		ta.createTypeTable();
		Store typeTable = ta.getTypeTable();
		ta.displayTypeTable();
	}
	catch (std::exception& except)
	{
		exec.flushLogger();
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		exec.stopLogger();
		return 1;
	}
	return 0;
}
#endif