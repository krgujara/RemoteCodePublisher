#pragma once
///////////////////////////////////////////////////////////////
// DepAnal.h - Creates Dependency Analysis                   //                                 //
//                                                           //
// Language:    Visual C++ 2015                              //
// Platform:    Windows 10 Macbook                           //
// Author:      Komal Gujarathi, Syracuse University         //
//              (315) 744 6116, krgujara@syr.edu             //
///////////////////////////////////////////////////////////////


/*
Package Operations:
===================
Dependency Alalysis package takes the typetable as an input
and finds out the dependencies among the packages.
Dependenecy Analysis holds the unordered map of  which stores the key value pair
key indicating the File Name
value is the Set of all the other files on wihch that particular files
depend on

package Prologue:
================
Provides the dependency results from the type table information
And displays dependencies

Maintanence Information:
========================

Required files:
---------------
Parser/ActionsAndRules.h , Parser/ActionsAndRules.cpp
AbstractSyntaxTree/AbstrSynTree.h , AbstractSyntaxTree/AbstrSynTree.cpp
Analyzer/Executive.h Analyzer/Executive.cpp
TypeTable.h, TypeTable.cpp

Build Process:
--------------
Using Visual Studio Command Prompt:

Revision History:
-----------------
ver 1. created on 2-Mar-2017
*/

#include "../Parser/ActionsAndRules.h"
#include <iostream>
#include <set>
#include <fstream>
#include <string>
#include "../TypeTable/TypeTable.h"


using namespace CodeAnalysis;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, std::vector<std::string>>;
using SPtr = std::shared_ptr<ASTNode*>;


//------<Class for finding dependcy analysis, basically a store which stores information about dependencies>---------
class DepAnal
{
public:

	DepAnal();
	void doDepAnal();
	void tokenizeForDependencies(FileMap files, std::unordered_map<Type, std::vector<TypeInfo>> typeTable);
	void analyzeDependencies(std::vector<std::string> vec, std::string file, std::unordered_map<Type, std::vector<TypeInfo>> typetable);
	void displayDependencies();
	std::unordered_map<std::string, std::set<std::string>> getDependencies();
private:
	void DFS(ASTNode* pNode);
	AbstrSynTree& ASTref_;
	ScopeStack<ASTNode*> scopeStack_;
	Scanner::Toker& toker_;

	std::unordered_map<std::string, std::set<std::string>> depAnalStore;
};

//-----------<initializes the AST, Scopestack and Toker>--------------
inline DepAnal::DepAnal() :
	ASTref_(Repository::getInstance()->AST()),
	scopeStack_(Repository::getInstance()->scopeStack()),
	toker_(*(Repository::getInstance()->Toker()))
{
}

//--------<Recursive depth first search>------
inline void DepAnal::DFS(ASTNode* pNode)
{
	static std::string path = "";
	if (pNode->path_ != path)
	{
		std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
		path = pNode->path_;
	}
	std::cout << "\n  " << pNode->name_;
	std::cout << ", " << pNode->type_;
	for (auto pChild : pNode->children_)
		DFS(pChild);
}

//------<Function which does type analysis>--------
inline void DepAnal::doDepAnal()
{
	std::cout << "\n  starting type analysis:\n";
	ASTNode* pRoot = ASTref_.root();
	DFS(pRoot);
}
