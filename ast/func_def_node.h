#ifndef __OG_AST_FUNC_DEF_NODE_H__
#define __OG_AST_FUNC_DEF_NODE_H__

#include "func_decl_node.h"
#include "block_node.h"
#include <cdk/types/basic_type.h>
#include <memory>

namespace og {

  /**
   * Class for describing function and procedure definition nodes.
   */
  class func_def_node: public og::func_decl_node {
    og::block_node *_block;

  public:
    inline func_def_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &id, cdk::sequence_node *args, og::block_node *block) :
        og::func_decl_node(lineno, qualifier, type, id, args), _block(block) {
    }

  public:
    inline og::block_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_func_def_node(this, level);
    }

  };

} // og

#endif