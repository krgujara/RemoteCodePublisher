///////////////////////////////////////////////////////////////
// DepAnal.cpp - Creates dependency analysis from type table  //                                 //
//                                                           //
// Language:    Visual C++ 2015                              //
// Platform:    Windows 10 Macbook                           //
// Author:      Komal Gujarathi, Syracuse University         //
//              (315) 744 6116, krgujara@syr.edu             //
///////////////////////////////////////////////////////////////

#include "DepAnal.h"

//----< helper: is text a substring of str? >--------------------
bool contains_str(const std::string& str, const std::string& text)
{
	if (str.find(text) < str.length())
		return true;
	return false;
}

//-------------<Tokenise the files to find if the type appears in aly of the files and checking it with all the types that are stored in typetable>------------
void DepAnal::tokenizeForDependencies(FileMap files, std::unordered_map<Type, std::vector<TypeInfo>> typeTable)
{
	std::vector<std::string> vec;

	// Loop over all file patterns
	for (auto item : files)
	{
		// Loop over all files
		for (auto file : item.second) {
			std::ifstream in(file);
			vec.clear();

			if (!in.good()) {
				continue;
			}

			Scanner::Toker token;
			token.returnComments(false);
			token.attach(&in);

			while (in.good())
			{
				std::string tok = token.getTok();
				if (tok == "\n")
					continue;
				vec.push_back(tok);
			}

			analyzeDependencies(vec, file, typeTable);
		}
	}

}

//-----<Displays dependency analysis result>-----------
void DepAnal::displayDependencies()
{
	std::cout << "\n\n\n----Dependency Analysis has : " << depAnalStore.size() << "Entries\n\n" << std::endl;


	for (auto item : depAnalStore)
	{
		std::cout << "\n" << item.first << "   DEPENDS ON   ";
		for (std::set<std::string>::iterator it = item.second.begin(); it != item.second.end(); ++it)
		{
			std::cout << "\n" << *it;
		}

		std::cout << "\n\n";
	}

}

//-------<Function to return the dependency analysis result>------------
std::unordered_map<std::string, std::set<std::string>> DepAnal::getDependencies()
{
	return depAnalStore;
}

//--------<Function to analyse dependencies>----------
void DepAnal::analyzeDependencies(std::vector<std::string> vec, std::string file,
	std::unordered_map<Type, std::vector<TypeInfo>> typetable)
{
	//for all tokens in the file
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		//found
		std::unordered_map<Type, std::vector<TypeInfo>>::iterator found = typetable.find(*it);
		if (found != typetable.end())
		{
			//match found

			// File1 depends on File2 (File2 -primary, File1- secondary)
			for (auto typeInfo : found->second)
			{
				//A file cannot be dependent on itself
				if (typeInfo.getFileName() != file) {
					depAnalStore[file].insert(typeInfo.getFileName());
				}
			}
		}

	}
}


#ifdef DEPANAL_TEST


int main(int argc, char* argv[])
{
	using namespace CodeAnalysis;

	//starting to execute
	CodeAnalysisExecutive exec;

	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded)
		{
			return 1;
		}
		exec.getSourceFiles();
		exec.processSourceCode(true);

		TypeTable ta;
		ta.createTypeTable();
		std::unordered_map<Type, std::vector<TypeInfo>> typeTable = ta.getTypeTable();
		DepAnal da;

		FileMap files = exec.getFileMap();

		da.tokenizeForDependencies(files, typeTable);

		da.displayDependencies();

		std::unordered_map<std::string, std::set<std::string>> dependencies = da.getDependencies();

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