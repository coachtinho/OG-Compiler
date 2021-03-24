#ifndef __OG_AST_STACK_ALLOC_H__
#define __OG_AST_STACK_ALLOC_H__

#include <cdk/ast/unary_operation_node.h>
#include <cdk/ast/expression_node.h>

namespace og {

  /**
   * Class for describing stack alloc nodes.
   */
  class stack_alloc_node: public cdk::unary_operation_node {
  
  public:
    inline stack_alloc_node(int lineno, cdk::expression_node *argument) :
      cdk::unary_operation_node(lineno, argument) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_stack_alloc_node(this, level);
    }

  };

} // og

#endif