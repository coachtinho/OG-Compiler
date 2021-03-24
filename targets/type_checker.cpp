#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void og::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}

void og::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void og::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(4, cdk::TYPE_STRING));
}

void og::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
}

void og::type_checker::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->type(cdk::make_reference_type(4, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC)));
}

void og::type_checker::do_tuple_node(og::tuple_node *const node, int lvl) {
  ASSERT_UNSPEC

  std::vector<std::shared_ptr<cdk::basic_type>> *types = new std::vector<std::shared_ptr<cdk::basic_type>>();
  for (size_t i = 0; i < node->expressions()->size(); i++) {
    node->expressions()->node(i)->accept(this, lvl);
    cdk::typed_node *tn = dynamic_cast<cdk::typed_node*>(node->expressions()->node(i)); 
    if (tn == nullptr) {
      throw std::string("Tuple nodes are not types nodes");
    }
    types->push_back(tn->type());
  }

  if (node->expressions()->size() == 1) {
    cdk::typed_node *typed_node = dynamic_cast<cdk::typed_node*>(node->expressions()->node(0));
    node->type(typed_node->type());
  } else {
    node->type(cdk::make_structured_type(*types));
  }
}

//---------------------------------------------------------------------------

void og::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT) || node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->argument()->type());
  } else {
    throw std::string("Invalid type for argument in unary operation");
  } 
}

void og::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void og::type_checker::do_identity_node(og::identity_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void og::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(node->argument()->type());
  } else {
    throw std::string("Invalid type for argument in not");
  }
}

void og::type_checker::do_address_of_node(og::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::make_reference_type(4, node->lvalue()->type()));
}

void og::type_checker::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::make_reference_type(4, cdk::make_primitive_type(0, cdk::TYPE_UNSPEC)));
  } else {
    throw std::string("Invalid type for argument in stack alloc");
  }
}

void og::type_checker::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->argument()->accept(this, lvl + 2);
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void og::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_POINTER)) {
    throw  std::string("wrong type in left argument of binary expression");
  } 

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_POINTER)) {
    throw std::string("wrong type in right argument of binary expression");
  }

}

void og::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->left()->type());
  }
  else {
    throw std::string("Invalid types in addition");
  }
}

void og::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
  
  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    std::shared_ptr<cdk::reference_type> left = std::dynamic_pointer_cast<cdk::reference_type>(node->left()->type());
    std::shared_ptr<cdk::reference_type> right = std::dynamic_pointer_cast<cdk::reference_type>(node->right()->type());
    if (left == nullptr || right == nullptr) {
      throw std::string("Failed cast in sub node");
    }
    if (left->referenced() == right->referenced()) {
      node->type() = cdk::make_primitive_type(4, cdk::TYPE_INT);
    }
    else {
      throw std::string("Incompatible types in pointer subtraction");
    }
  }
  else {
    throw std::string("Invalid types in subtraction");
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->left()->type());
  }
  else {
    throw std::string("Invalid types in multiplication");
  }
}

void og::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->left()->type());
  }
  else {
    throw std::string("Invalid types in multiplication");
  }
}

void og::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else {
    throw std::string("Invalid types in modulus");
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (!node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in left argument of less than");
  }
  else if (!node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in right argument of less than");  
  }
  else {
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  }

}

void og::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (!node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in left argument of less equal");
  }
  else if (!node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in right argument of less equal");  
  }
  else {
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  }
}

void og::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (!node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in left argument of greater equal");
  }
  else if (!node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in right argument of greater equal");  
  }
  else {
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  }
}

void og::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (!node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in left argument of greater than");
  }
  else if (!node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in right argument of greater than");  
  }
  else {
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void og::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (!node->left()->is_typed(cdk::TYPE_INT) || !node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in and");
  }
  else {
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  }
}

void og::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  processBinaryExpression(node, lvl);

  if (!node->left()->is_typed(cdk::TYPE_INT) || !node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Invalid types in or");
  }
  else {
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<og::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    std::cout << "aqui " + id << std::endl;
    throw id;
  }
}

void og::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "Undeclared variable '" + id + "'";
  }
}

//TODO: testar se os referenced type sao compativeis
void og::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl);
  node->rvalue()->accept(this, lvl + 2);

  if (!node->lvalue()->is_typed(cdk::TYPE_INT) 
      && !node->lvalue()->is_typed(cdk::TYPE_DOUBLE) 
      && !node->lvalue()->is_typed(cdk::TYPE_STRING)
      && !node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
    throw std::string("Invalid type in left argument of assignment");
  }

  if (!node->rvalue()->is_typed(cdk::TYPE_INT) 
      && !node->rvalue()->is_typed(cdk::TYPE_DOUBLE) 
      && !node->rvalue()->is_typed(cdk::TYPE_STRING)
      && !node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
    throw std::string("Invalid type in right argument of assignment");
  }

  if (node->lvalue()->type()->name() == node->rvalue()->type()->name()) {
    if (node->lvalue()->is_typed(cdk::TYPE_POINTER) && node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      std::shared_ptr<cdk::reference_type> p1 = std::dynamic_pointer_cast<cdk::reference_type>(node->lvalue()->type());
      std::shared_ptr<cdk::reference_type> p2 = std::dynamic_pointer_cast<cdk::reference_type>(node->rvalue()->type());
      if (p1 == nullptr || p2 == nullptr) {
        throw std::string("Failed cast in assignment node");
      }
      if (p1->referenced()->name() == cdk::TYPE_UNSPEC && p2->referenced()->name() != cdk::TYPE_UNSPEC) {
        node->lvalue()->type(node->rvalue()->type());
      }
      if (p2->referenced()->name() == cdk::TYPE_UNSPEC && p1->referenced()->name() != cdk::TYPE_UNSPEC) {
        node->rvalue()->type(node->lvalue()->type());
      }
    }
    node->type(node->lvalue()->type());
  }
  else {
    if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE) && node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(node->lvalue()->type());
    }
    else {
      throw std::string("Conflicting types in assignment left: " + cdk::to_string(node->lvalue()->type()) + " right: " + cdk::to_string(node->rvalue()->type()));
    }
  }

}

//---------------------------------------------------------------------------

void og::type_checker::do_evaluation_node(og::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void og::type_checker::do_for_node(og::for_node *const node, int lvl) {
  node->inits()->accept(this, lvl + 2);
  node->condition()->accept(this, lvl + 2);
  node->incrs()->accept(this, lvl + 2);
  node->block()->accept(this, lvl + 2);
}

void og::type_checker::do_if_node(og::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 2);
  node->block()->accept(this, lvl + 2);
}

void og::type_checker::do_if_else_node(og::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 2);
  node->thenblock()->accept(this, lvl + 2);
  node->elseblock()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void og::type_checker::do_input_node(og::input_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->type(cdk::make_primitive_type(0, cdk::TYPE_UNSPEC));
}

//TODO: CHECK IF ONE OF THE TYPES OF A TUPLE IS NOT A POINTER
void og::type_checker::do_write_node(og::write_node *const node, int lvl) {
  node->expressions()->accept(this, lvl + 2);
  if (node->expressions()->is_typed(cdk::TYPE_POINTER)) {
    throw std::string("Cannot write pointers");
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_ptr_index_node(og::ptr_index_node *const node, int lvl) { 
  ASSERT_UNSPEC
  
  node->ptr()->accept(this, lvl + 2);
  if (!node->ptr()->is_typed(cdk::TYPE_POINTER)) {
    throw std::string("Wrong type for pointer in pointer index");
  }

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT)) {
    throw std::string("Wrong type for index in pointer index");
  }

  std::shared_ptr<cdk::reference_type> p = std::dynamic_pointer_cast<cdk::reference_type>(node->ptr()->type()); 
  if (p == nullptr) {
    throw std::string("Failed cast in ptr index node");
  }

  node->type(p->referenced());
}

void og::type_checker::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->tuple()->accept(this, lvl + 2);
  node->index()->accept(this, lvl + 2);
  if (!node->tuple()->is_typed(cdk::TYPE_STRUCT)) {
    throw std::string("Indexed not a tuple");
  }
  else {
    std::shared_ptr<cdk::structured_type> p = std::dynamic_pointer_cast<cdk::structured_type>(node->tuple()->type()); 
    if (p == nullptr) {
      throw std::string("Failed cast in ptr index node");
    }
    node->type(p->component(node->index()->value() - 1));
  }
}

//---------------------------------------------------------------------------

void og::type_checker::do_func_call_node(og::func_call_node *const node, int lvl) {
  ASSERT_UNSPEC;

  std::shared_ptr<og::symbol> symbol = _symtab.find(node->id());
  if (!symbol) {
    throw std::string("Function " + node->id() + " not declared");
  } else if (!symbol->isFunc()) {
    throw std::string(node->id() + " is not a function");
  } else if (symbol->type()->name() == cdk::TYPE_UNSPEC) {
    throw std::string("Auto function not defined");
  } else if (node->args()->size() != symbol->arg_types().size()) {
    throw std::string("Incompatible number of arguments");
  }

  node->args()->accept(this, lvl);
  for (size_t i = 0; i < node->args()->size(); i++) {
    cdk::typed_node *expr = dynamic_cast<cdk::typed_node*>(node->args()->node(i));
    if (expr == nullptr) {
      throw std::string("Failed cast in func call node");
    }
    std::shared_ptr<cdk::basic_type> type = symbol->arg_types().at(i);

    if (expr->type()->name() != type->name()) {
      if (!(type->name() == cdk::TYPE_DOUBLE && expr->is_typed(cdk::TYPE_INT))) {
        throw std::string("Incompatible arg types");
      }
    } else if (type->name() == cdk::TYPE_POINTER) {
      std::shared_ptr<cdk::reference_type> p1 = std::dynamic_pointer_cast<cdk::reference_type>(type);
      std::shared_ptr<cdk::reference_type> p2 = std::dynamic_pointer_cast<cdk::reference_type>(expr->type());
      if (p1 == nullptr || p2 == nullptr) {
        throw std::string("Failed cast in func call node");
      }
      if (p1->referenced()->name() != p2->referenced()->name()) {
        throw std::string("Incompatible arg pointer subtypes");
      }
    }
  }
  
  node->type(symbol->type());
}

void og::type_checker::do_func_decl_node(og::func_decl_node *const node, int lvl) {
  std::shared_ptr<og::symbol> symbol = _symtab.find(node->id());

  if (!symbol) {
    if(node->id() == "og") {
      node->id("_main");
    }
    else if(node->id() == "_main") {
      node->id("._main");
    }

    std::vector<std::shared_ptr<cdk::basic_type>> types;

    for (size_t i = 0; i < node->args()->size(); i++) {
      og::var_decl_node *var_decl = dynamic_cast<og::var_decl_node*>(node->args()->node(i));
      types.push_back(var_decl->type());
    }
    
    symbol = std::make_shared<og::symbol>(node->type(), node->id(), true, node->qualifier(), types);
    _symtab.insert(node->id(), symbol);
  } else {
    if (symbol->type() == node->type() && symbol->arg_types().size() == node->args()->size()) {
      for (size_t j = 0; j < node->args()->size(); j++) {
        std::shared_ptr<cdk::basic_type> type = symbol->arg_types().at(j);
        og::var_decl_node *var_decl = dynamic_cast<og::var_decl_node*>(node->args()->node(j));

        if (var_decl == nullptr) {
          throw std::string("Failed cast in func decl node");
        }

        if (type->name() != var_decl->type()->name()) {
          throw std::string("Redeclaration of function");
        } else if (type->name() == cdk::TYPE_POINTER) {
          std::shared_ptr<cdk::reference_type> p1 = std::dynamic_pointer_cast<cdk::reference_type>(type);
          std::shared_ptr<cdk::reference_type> p2 = std::dynamic_pointer_cast<cdk::reference_type>(var_decl->type());
          if (p1 == nullptr || p2 == nullptr) {
            throw std::string("Failed cast in func decl node");
          }
          if (p1->referenced()->name() != p2->referenced()->name())
            throw std::string("Incompatible arg pointer subtypes");
        }
      }
    } else {
      throw std::string("Redeclaration of function");
    }
  }
}

void og::type_checker::do_func_def_node(og::func_def_node *const node, int lvl) {
  std::shared_ptr<og::symbol> symbol = _symtab.find(node->id());
  if (!symbol) {
    do_func_decl_node(node, lvl);
    symbol = _symtab.find(node->id());
  } else if (!symbol->isFunc()) {
    throw std::string("Symbol already declared");
  } else if (symbol->isDefined()
      || node->type() != symbol->type()
      || node->args()->size() != symbol->arg_types().size()) {
    throw std::string("Redefinition of function");
  }
  
  for (size_t i = 0; i < node->args()->size(); i++) {
    og::var_decl_node *var_decl = dynamic_cast<og::var_decl_node*>(node->args()->node(i));
    std::shared_ptr<cdk::basic_type> type = symbol->arg_types().at(i);

    if (var_decl->type() != type) {
      throw std::string("Incompatible arg types");
    } else if (type->name() == cdk::TYPE_POINTER) {
      std::shared_ptr<cdk::reference_type> p1 = std::dynamic_pointer_cast<cdk::reference_type>(type);
      std::shared_ptr<cdk::reference_type> p2 = std::dynamic_pointer_cast<cdk::reference_type>(var_decl->type());
      if (p1 == nullptr || p2 == nullptr) {
        throw std::string("Failed cast in func def node");
      }
      if (p1->referenced()->name() != p2->referenced()->name())
        throw std::string("Incompatible arg pointer subtypes");
    }
  }

  // args and block visited only to set types of auto definitions
  // symbol table will be filled later by ASSERT_SAFE_EXPRESSIONS in postfix_writer
  _symtab.push();
  _currentFunction = symbol;
  node->args()->accept(this, lvl);
  node->block()->accept(this, lvl);
  _currentFunction = nullptr;
  _symtab.pop();

  symbol->isDefined(true);
  node->type(symbol->type());
}

void og::type_checker::do_return_node(og::return_node *const node, int lvl) {
  if (!_currentFunction) {
    throw std::string("return not in function");
  }
  node->values()->accept(this, lvl + 2);
  if (_currentFunction->type()->name() != cdk::TYPE_VOID) {
    if (_currentFunction->type()->name() == cdk::TYPE_UNSPEC) {
      _currentFunction->type(node->values()->type());
    } else if (_currentFunction->type()->name() == cdk::TYPE_STRUCT && node->values()->is_typed(cdk::TYPE_STRUCT)) {
      std::shared_ptr<cdk::structured_type> t1 = std::dynamic_pointer_cast<cdk::structured_type>(_currentFunction->type());
      std::shared_ptr<cdk::structured_type> t2 = std::dynamic_pointer_cast<cdk::structured_type>(node->values()->type());

      if (t1 == nullptr || t2 == nullptr) {
        throw std::string("Failed cast in return node");
      }
      
      if (t1->components().size() != t2->components().size()) {
        throw std::string("Incompatible sizes in return");
      }

      for (size_t i = 0; i < t1->components().size(); i++) {
        if (t1->components()[i]->name() != t2->components()[i]->name()) {
          if (!(t1->components()[i]->name() == cdk::TYPE_DOUBLE && t2->components()[i]->name() == cdk::TYPE_INT)) {
            throw std::string("Incompatible type in return");
          }
        } else if (t1->components()[i]->name() == cdk::TYPE_POINTER) {
          std::shared_ptr<cdk::reference_type> p1 = std::dynamic_pointer_cast<cdk::reference_type>(t1->components()[i]);
          std::shared_ptr<cdk::reference_type> p2 = std::dynamic_pointer_cast<cdk::reference_type>(t2->components()[i]);

          if (p1 == nullptr || p2 == nullptr) {
            throw std::string("Failed cast in return node");
          }

          if (p1->referenced()->name() != p2->referenced()->name()) {
            throw std::string("Incompatible type in return");
          }
        }
      }
    } else if (_currentFunction->type()->name() != node->values()->type()->name()) {
      if (!(_currentFunction->type()->name() == cdk::TYPE_DOUBLE && node->values()->is_typed(cdk::TYPE_INT))) {
        throw std::string("Incompatible type in return");
      }
    } else if (_currentFunction->type()->name() == cdk::TYPE_POINTER)  {
      //TODO verificar subtipos
    }
  } else {
    throw std::string("Found return in procedure");
  }
}

void og::type_checker::saveSymbol(std::string &name, std::shared_ptr<cdk::basic_type> type , int qualifier) {
  std::shared_ptr<og::symbol> symbol = _symtab.find_local(name);
  if (!symbol) {
    symbol = std::make_shared<og::symbol>(type, name, qualifier);
    _symtab.insert(name, symbol);
  }
  else {
    throw std::string("Variable " + name + " redeclared");
  }
}

void og::type_checker::do_var_decl_node(og::var_decl_node *const node, int lvl) {
  saveSymbol(node->ids()[0], node->type(), node->qualifier());
}

void og::type_checker::do_var_def_node(og::var_def_node *const node, int lvl) {

  node->args()->accept(this, lvl + 2);
  
  if ((node->is_typed(cdk::TYPE_UNSPEC) && node->args()->is_typed(cdk::TYPE_STRUCT)) || node->is_typed(cdk::TYPE_STRUCT)) {
    node->type(node->args()->type());
    std::shared_ptr<cdk::structured_type> p = std::dynamic_pointer_cast<cdk::structured_type>(node->args()->type());
    if (p == nullptr) {
      throw std::string("Failed first cast in var def node");
    }
    if (node->ids().size() == p->components().size()) {
      for (unsigned int i = 0; i < node->ids().size(); i++) {
        saveSymbol(node->ids()[i], p->component(i), node->qualifier());
      }
    }
    else if (node->ids().size() == 1) {
      saveSymbol(node->ids()[0], node->type(), node->qualifier());
    }
    else if (p->components().size() == 1) {
      for (unsigned int i = 0; i < node->ids().size(); i++) {
        saveSymbol(node->ids()[i], p->component(0), node->qualifier());
      }
    }
    else {
      throw std::string("Invalid cardinality in variable definition");
    }
  }
  else {
    if (node->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(node->args()->type());
    }
    if ((node->type() != node->args()->type()) && !(node->is_typed(cdk::TYPE_DOUBLE) && node->args()->is_typed(cdk::TYPE_INT))) {
      throw std::string("Conflicting types in variable definition");
    }
    if (node->is_typed(cdk::TYPE_POINTER) && node->args()->is_typed(cdk::TYPE_POINTER)) {
      std::shared_ptr<cdk::reference_type> p1 = std::dynamic_pointer_cast<cdk::reference_type>(node->type());
      std::shared_ptr<cdk::reference_type> p2 = std::dynamic_pointer_cast<cdk::reference_type>(node->args()->type());
      if (p1 == nullptr || p2 == nullptr) {
        throw std::string("Failed second cast in var def node");
      }
      if (p1->referenced()->name() == cdk::TYPE_UNSPEC && p2->referenced()->name() != cdk::TYPE_UNSPEC) {
        node->type(node->args()->type());
      }
      if (p2->referenced()->name() == cdk::TYPE_UNSPEC && p1->referenced()->name() != cdk::TYPE_UNSPEC) {
        node->args()->type(node->type());
      }
    }
    saveSymbol(node->ids()[0], node->type(), node->qualifier());
  }
}

//The only purpose of this typechecker is to aid ENTER size calc and to check return types
void og::type_checker::do_block_node(og::block_node *const node, int lvl) {
  if (node->decls())
    node->decls()->accept(this, lvl);
  if (node->instrs())
    node->instrs()->accept(this, lvl);
}

void og::type_checker::do_break_node(og::break_node *const node, int lvl) {
}

void og::type_checker::do_continue_node(og::continue_node *const node, int lvl) {
}
