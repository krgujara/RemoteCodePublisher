///////////////////////////////////////////////////////////////
// CodePublisher.cpp - Package to convert the Cpp files      //  
//to HTMl pages to publish                                   // 
//                                                           //
// Language:    Visual C++ 2015                              //
// Platform:    Windows 10 Macbook                           //
// Author:      Komal Gujarathi, Syracuse University         //
//              (315) 744 6116, krgujara@syr.edu             //
///////////////////////////////////////////////////////////////

#include "codePublisher.h"
#include "../Analyzer/Executive.h"
#include <fstream>
#include <iostream>
#include <set>
#include<unordered_map>
#include "../Tokenizer/Tokenizer.h"
#include "../DepAnal/DepAnal.h"
#include "../TypeTable/TypeTable.h"

////////////////////////////////////////////////
//function to convert .Cpp files to .Html files
void CodePublisher::convertCppToHtml(std::string file, std::set<std::string> dependencies, std::string category, std::string iispath){
	static int id = 0;
	std::string fileName;
	std::ifstream sourceFile(file);
	if (sourceFile.is_open()) { 
		std::string path = iispath + "/"+category+"/";
		std::size_t found = file.rfind("\\");
		if (found != std::string::npos)
			for (size_t i = found + 1; i < file.length(); i++) fileName += file[i];
		path.append(fileName+".html");
		fileName.clear();
		std::ofstream HTMLfile(path);
		if (HTMLfile.is_open()) { std::string htmlFileContents;
			htmlFileContents.append("<html><head><link rel = \"stylesheet\" type = \"text/css\" href = \"style.css\">");
			htmlFileContents.append("<script type = \"text/javascript\" src = \"script.js\"></script></head><body><h3>");
			htmlFileContents.append(file);
			htmlFileContents.append("</h3><hr/><div class = \"indent\"><h4>Dependencies:</h4>");
			for (auto it = dependencies.begin(); it != dependencies.end(); it++) {
				std::string path = "../"+category+"/"; //forming the path to store HTML files
				std::size_t found = (*it).rfind("\\");
				fileName = "";
				if (found != std::string::npos)
					for (size_t i = found + 1; i < (*it).length(); i++) fileName += (*it)[i];
				path.append(fileName + ".html");  //creating the HTML path corresponding to each dependant file
				htmlFileContents.append("<a href = \""+path);
				htmlFileContents.append("\"> " + (*it) + "</a><br>"); }
			htmlFileContents.append("</div><hr/><pre>");
			Scanner::Toker toker;
			toker.returnComments();
			toker.attach(&sourceFile);
			do { std::string tok = toker.getTok();
				if (tok == "}") htmlFileContents.append(" </div>"); //this is required for collapse expand
				if (tok == "<") htmlFileContents.append("&lt;");      //replace < with &lt; as HTMLparser cannt process < > signs
				else if (tok == ">") htmlFileContents.append("&gt;"); //replaces > with &gt;
				else htmlFileContents.append(tok + " ");
				if (tok == "{") {
					std::string toggle = "toggleText" + std::to_string(id++);
					htmlFileContents.append("<a id = \"displayText\" href = \"javascript:toggle(");
					htmlFileContents.append("'" + toggle + "'");
					htmlFileContents.append("); \">+</a><div id = \"");
					htmlFileContents.append(toggle + "\" style = \"display: none\">");}
			} while (sourceFile.good());
			htmlFileContents.append("</pre></body></html>");
			HTMLfile << htmlFileContents;
			HTMLfile.close();}
		else std::cout << "\nCannot open the HTML File";
		sourceFile.close();}
	else std::cout << "Unable to open files";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//function to convert to each file into corresponding html files along wiht insertion of dependencies 
void CodePublisher::processFiles(std::unordered_map<std::string, std::vector<std::string>> fm, std::unordered_map < std::string, std::set < std::string>> dependencies,std::string category, std::string iispath)
{
	for (auto it = fm.begin(); it != fm.end(); it++)
	{
		std::cout << " ";
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			std::set<std::string> dependsOn;
			dependsOn = dependencies[*it2];

			convertCppToHtml(*it2, dependsOn,category, iispath);
		}
	}
}

///////////////////////////////////////////////////////////////
//funciton to create the start up (index ) page for set of files 
void CodePublisher::createRepository(std::unordered_map<std::string, std::vector<std::string>> fm, std::string path)
{
	std::string contents = "";
	std::ofstream HTMLfile(path);
	if (HTMLfile.is_open())
	{
		contents.append("<html><head><style>h2: {color: red;}</style></head><body><center><H2>Code Publisher for");
		contents.append(path);
		contents.append("</H2></center><div><hr>");
		for (auto it = fm.begin(); it != fm.end(); it++)
		{
			for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
				contents.append("<a href =\"");

				std::string fileName = "";
				std::string path = "../HTMLFiles/"; //forming the path to store links to all HTML files
				std::size_t found = (*it2).rfind("\\");
				if (found != std::string::npos)
					for (size_t i = found + 1; i < (*it2).length(); i++)
						fileName += (*it2)[i];
				path.append(fileName);
				path.append(".html");

				contents.append(path);
				contents.append("\">");
				contents.append(fileName);
				contents.append("</a><br>");
				fileName.clear();

			}
		}
		contents.append("</body></html>");
		HTMLfile << contents;

		HTMLfile.close();
	}
	else
	{
		std::cout << "\nCannot open file ";
	}
}


#ifdef CODE_PUBLISHER
int main(int argc, char *argv[]) {

	std::string line;
	CodeAnalysis::CodeAnalysisExecutive exec;
	using Files = std::unordered_map<std::string, std::vector<std::string>>;
	using DepStore = std::unordered_map<std::string, std::set<std::string>>;

	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded)
		{
			return 1;
		}
		exec.getSourceFiles();
		exec.processSourceCode(true);
		Files fm = exec.getFileMap();

		CodePublisher publisher;

		TypeTable tt;
		tt.createTypeTable();

		//std::cout << "\nType tabe Created\n";
		Store typeTable = tt.getTypeTable();

		DepAnal danal;
		//FileMap files = exec.getFileMap();
		danal.tokenizeForDependencies(fm, typeTable);
		//danal.displayDependencies();
		DepStore dependencies = danal.getDependencies();

		publisher.processFiles(fm, dependencies);


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


