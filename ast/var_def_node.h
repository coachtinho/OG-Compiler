#ifndef __OG_AST_VAR_DEF_NODE_H__
#define __OG_AST_VAR_DEF_NODE_H__

#include "var_decl_node.h"
#include <cdk/ast/expression_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/ast/sequence_node.h>
#include <memory>

namespace og {

  /**
   * Class for describing variable definition nodes.
   */
  class var_def_node: public og::var_decl_node {
    cdk::expression_node *_args;

  public:
    inline var_def_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type, std::vector<std::string> ids, cdk::expression_node *args) :
        og::var_decl_node(lineno, qualifier, type, ids), _args(args) {
    }

  public:
    inline cdk::expression_node *args() {
      return _args;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_var_def_node(this, level);
    }

  };

} // og

#endif