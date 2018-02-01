#pragma once
///////////////////////////////////////////////////////////////
// TypeTable.h - Creates TypeTable for dependency analysis   //                                 //
//                                                           //
// Language:    Visual C++ 2015                              //
// Platform:    Windows 10 Macbook                           //
// Author:      Komal Gujarathi, Syracuse University         //
//              (315) 744 6116, krgujara@syr.edu             //
///////////////////////////////////////////////////////////////


/*
Package Operations:
===================
Typetable is basically the map that stores the information
about the types like struct, class, enums, typedef.
Also it stores the information that in which package is this type
found, and also stores the namespace in which that type is found

TypeTable holds the unordered map which stores the key value pair
key indicating the Type
value is the vector of TypeInfo which is the collection of
information about that type - package, namespace, and type.

Public Interface
================
Creates and displays typetable from the given set of files

Maintanence Information:
========================

Required files:
---------------
Parser/ActionsAndRules.h"
AbstractSyntaxTree/AbstrSynTree.h"
Analyzer/Executive.h"

Build Process:
--------------
Using Visual Studio Command Prompt:

Revision History:
-----------------
ver 1. created on 28-feb-2017
*/
#include "../Parser/ActionsAndRules.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Analyzer/Executive.h"
#include <fstream>
#include <iostream>
#include <stack>
using namespace CodeAnalysis;
class TypeInfo;

using Type = std::string;
using Keys = std::vector<std::string>;
using Item = std::pair<Type, std::vector<TypeInfo>>;

using Store = std::unordered_map<Type, std::vector<TypeInfo>>;

//----< Class that is used by the class TypeTable to store the type information >-----
class TypeInfo
{

private:
	using FileName = std::string;
	using TypeName = std::string;
	using NameSpace = std::string;

	FileName _filename;
	TypeName _typename;
	NameSpace _namespace;
public:

	std::string getFileName();
	std::string getTypeName();
	std::string getNameSpace();
	void setFileName(std::string s);
	void setTypeName(std::string s);
	void setNameSpace(std::string s);
};

//-----<Class that creates the in - memory database for storing the data about that type which it collects from AST >------
class TypeTable
{

public:
	TypeTable();
	void createTypeTable();
	void add(std::string _type, TypeInfo _typeinfo);
	void displayTypeTable();
	size_t count();
	std::vector<std::string> setOfTypes();
	Store getTypeTable();

private:
	using SPtr = std::shared_ptr<ASTNode*>;
	void DFS(ASTNode* pNode);
	AbstrSynTree& ASTref_;
	ScopeStack<ASTNode*> scopeStack_;
	Scanner::Toker& toker_;

	Store typetablestore;

};


inline TypeTable::TypeTable() :
	ASTref_(Repository::getInstance()->AST()),
	scopeStack_(Repository::getInstance()->scopeStack()),
	toker_(*(Repository::getInstance()->Toker()))
{
}



//-----<DFS Traversal of the AST>------
inline void TypeTable::DFS(ASTNode* pNode)
{
	static std::stack<std::string>namespaceStack;
	static std::string path = "";
	if (pNode->path_ != path)
	{
		path = pNode->path_;
	}


	//store the current namespace information which can be used to 
	//locate the namespace in which
	//the type of the successor node in the AST.
	if (pNode->type_ == "namespace")
	{
		namespaceStack.push(pNode->name_);
	}

	//inserting data in the typetable
	if (pNode->type_ == "class" || pNode->type_ == "struct" || pNode->type_ == "enum" || pNode->type_ == "alias" || pNode->type_ == "typedef" || pNode->type_ == "function" || pNode->type_ == "namespace")
	{
		TypeInfo info;
		info.setFileName(pNode->path_);
		info.setTypeName(pNode->type_);
		info.setNameSpace(namespaceStack.top());
		if (pNode->name_ != "main")//main should not be in type table
			add(pNode->name_, info);

	}
	for (auto pChild : pNode->children_)
		DFS(pChild);

	if (pNode->type_ == "namespace")
	{
		namespaceStack.pop();
	}
}

//-<createTYpetable actually calls the DFS >------
inline void TypeTable::createTypeTable()
{
	//std::cout << "\n  Starting Type Analysis:\n";
	ASTNode* pRoot = ASTref_.root();
	DFS(pRoot);
	//displayTypeTable();
}
