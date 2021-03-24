#ifndef __OG_AST_FUNC_DECL_NODE_H__
#define __OG_AST_FUNC_DECL_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/ast/typed_node.h>
#include <memory>

namespace og {

  /**
   * Class for describing function and procedure declaration nodes.
   */
  class func_decl_node: public cdk::typed_node {
    int _qualifier;
    std::string _id;
    cdk::sequence_node *_args;

  public:
    inline func_decl_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type_ptr, const std::string &id, cdk::sequence_node *args) :
        cdk::typed_node(lineno), _qualifier(qualifier), _id(id), _args(args) {
            type(type_ptr);
    }

  public:
    inline int qualifier() {
      return _qualifier;
    }

    inline std::string id() {
      return _id;
    }

    inline void id(std::string id) {
      _id = id;
    }

    inline cdk::sequence_node *args() {
      return _args;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_func_decl_node(this, level);
    }

  };

} // og

#endif