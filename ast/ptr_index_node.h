#ifndef __OG_AST_PTR_INDEX_NODE_H__
#define __OG_AST_PTR_INDEX_NODE_H__

#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/expression_node.h>

namespace og {

  /**
   * Class for describing the pointer index of operator.
   */
  class ptr_index_node: public cdk::lvalue_node {
    cdk::expression_node *_ptr;
    cdk::expression_node *_index;

  public:
    ptr_index_node(int lineno, cdk::expression_node *ptr, cdk::expression_node *index) :
        cdk::lvalue_node(lineno), _ptr(ptr), _index(index) {
    }

  public:
    cdk::expression_node *ptr() {
      return _ptr;
    }

    cdk::expression_node *index() {
      return _index;
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_ptr_index_node(this, level);
    }

  };

} // og

#endif