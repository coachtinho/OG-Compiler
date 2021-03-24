#ifndef __OG_AST_VAR_DECL_NODE_H__
#define __OG_AST_VAR_DECL_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <memory>

namespace og {

  /**
   * Class for describing variable declaration nodes.
   */
  class var_decl_node: public cdk::typed_node {
    int _qualifier;
    std::vector<std::string> _ids;

  public:
    inline var_decl_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type_ptr, std::vector<std::string> ids) :
        cdk::typed_node(lineno), _qualifier(qualifier), _ids(ids) {
            type(type_ptr);
    }

  public:

    inline std::vector<std::string> ids() {
      return _ids;
    }

    inline int qualifier() {
      return _qualifier;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_var_decl_node(this, level);
    }

  };

} // og

#endif