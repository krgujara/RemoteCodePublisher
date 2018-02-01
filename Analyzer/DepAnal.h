#pragma once
// DepAnal.h

#include "../Parser/ActionsAndRules.h"
#include <iostream>

namespace CodeAnalysis
{
  class TypeAnal
  {
  public:
    using SPtr = std::shared_ptr<ASTNode*>;

    TypeAnal();
    void doTypeAnal();
  private:
    void DFS(ASTNode* pNode);
    AbstrSynTree& ASTref_;
    ScopeStack<ASTNode*> scopeStack_;
    Scanner::Toker& toker_;
  };

  inline TypeAnal::TypeAnal() : 
    ASTref_(Repository::getInstance()->AST()),
    scopeStack_(Repository::getInstance()->scopeStack()),
    toker_(*(Repository::getInstance()->Toker()))
  {
  }

  inline void TypeAnal::DFS(ASTNode* pNode)
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

  inline void TypeAnal::doTypeAnal()
  {
    std::cout << "\n  starting type analysis:\n";
    ASTNode* pRoot = ASTref_.root();
    DFS(pRoot);
  }
}