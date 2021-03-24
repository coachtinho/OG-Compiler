#ifndef __OG_AST_FUNC_CALL_NODE_H__
#define __OG_AST_FUNC_CALL_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace og {

  /**
   * Class for describing function and procedure calls nodes.
   */
  class func_call_node: public cdk::expression_node {
    std::string _id;
    cdk::sequence_node *_args;

  public:
    inline func_call_node(int lineno, const std::string &id, cdk::sequence_node *args) :
        cdk::expression_node(lineno), _id(id), _args(args) {
    }

  public:

    inline std::string id() {
      return _id;
    }

    inline cdk::sequence_node *args() {
      return _args;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_func_call_node(this, level);
    }

  };

} // og

#endif