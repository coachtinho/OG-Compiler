#ifndef __OG_TARGETS_POSTFIX_WRITER_H__
#define __OG_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>

namespace og {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<og::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    std::vector<std::string> _continue;
    std::vector<std::string> _break;
    std::shared_ptr<og::symbol> _currentFunction;
    std::vector<std::string> _functionsToDeclare;
    bool _returned = false;
    size_t _localOffset;
    size_t _argOffset;

    // Vars
    std::string _varLabel;
    bool _publicVar;
    bool _globalVar;
    bool _argVar;
    bool _isTuple;

    // Index
    bool _isTupleIndex;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<og::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), _publicVar(false), _globalVar(true), _argVar(false), _isTuple(false), _isTupleIndex(false) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  protected:
    void copy_tuple(cdk::expression_node *const node, int lvl);

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end
  public:
    void pushFunctionToDeclare(std::string id) {
      for (std::string func : _functionsToDeclare) {
        if (id == func)
          return;
      }
      _functionsToDeclare.push_back(id);
    }
    
    void popFunctionToDeclare(std::string id) {
      for (std::vector<std::string>::iterator i = _functionsToDeclare.begin(); i != _functionsToDeclare.end(); i++) {
        if (id == *i) {
          _functionsToDeclare.erase(i);
          break;
        }
      }
    }

    void listFunctionsToDeclare() {
      for (std::string id : _functionsToDeclare) {
        _pf.EXTERN(id);
      }
    }
  };

} // og

#endif
