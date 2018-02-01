#pragma once
///////////////////////////////////////////////////////////////
// CodePublisher.h - Package to convert the Cpp files        //  
//to HTMl pages to publish                                     // 
//                                                           //
// Language:    Visual C++ 2015                              //
// Platform:    Windows 10 Macbook                           //
// Author:      Komal Gujarathi, Syracuse University         //
//              (315) 744 6116, krgujara@syr.edu             //
///////////////////////////////////////////////////////////////

/*
Package Operations:
===================
Package to convert Cpp and .h files to .Html files
Creates a repository to publish all files from a specifies folder
Maintanence Information:
========================

Public Interfaces:
==================
convertCppToHtml converts one Cpp file To its corresponding Html
processFiles - takes all files as input and sends each file to convertCppToHtml for processing
createRepository - creates index Page


Required files:
---------------
Analyzer/Executive.h"
TypeTable.h , TypeTable.cpp
DependencyAnalysis.h and dependencyAnalysis.cpp

Build Process:
--------------
Using Visual Studio Command Prompt:

Revision History:
-----------------
ver 1. created on 5-mar-2017
ver 2. added a function to add repository
*/
#include<set>
#include<unordered_map>
class CodePublisher
{

public:
	void convertCppToHtml(std::string file, std::set < std::string > dependsOn, std::string category, std::string iispath);
	void processFiles(std::unordered_map < std::string, std::vector < std::string >> files, std::unordered_map < std::string, std::set < std::string>> dependencies, std::string category, std::string iispath);
	void createRepository(std::unordered_map < std::string, std::vector < std::string >> files, std::string path);
};

