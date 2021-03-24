#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

//---------------------------------------------------------------------------

void og::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void og::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void og::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}
void og::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}
void og::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void og::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void og::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void og::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  node->lvalue()->accept(this, lvl);
  reset_new_symbol();

  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_evaluation_node(og::evaluation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_for_node(og::for_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("inits", lvl + 2);
  node->inits()->accept(this, lvl + 4);
  closeTag("inits", lvl + 2);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("incrs", lvl + 2);
  node->incrs()->accept(this, lvl + 4);
  closeTag("incrs", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_if_node(og::if_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_if_else_node(og::if_else_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_block_node(og::block_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("declarations", lvl + 2);
  if (node->decls() != nullptr) node->decls()->accept(this, lvl + 4);
  closeTag("declarations", lvl + 2);
  openTag("instructions", lvl + 2);
  if (node->instrs() != nullptr) node->instrs()->accept(this, lvl + 4);
  closeTag("instructions", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_break_node(og::break_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void og::xml_writer::do_continue_node(og::continue_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void og::xml_writer::do_func_call_node(og::func_call_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " id='" << node->id() << "'>" << std::endl;
  openTag("args", lvl + 2);
  if(node->args()!=nullptr) node->args()->accept(this, lvl + 4);
  closeTag("args", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_func_decl_node(og::func_decl_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label()
  << " qualifier='"; 
  switch (node->qualifier()) {
    case 0:
      os() << "private";
      break;
    case 1:
      os() << "public";
      break;
    case 2:
      os() << "require";
      break;
    default:
      break;
  }
  os() << "' id='" << node->id()
  << "' type='" << (node->type() != nullptr ? cdk::to_string(node->type()) : "procedure") << "'>" << std::endl;
  openTag("args", lvl + 2);
  if(node->args() != nullptr) node->args()->accept(this, lvl + 4);
  closeTag("args", lvl + 2);  
  closeTag(node, lvl);
}
void og::xml_writer::do_func_def_node(og::func_def_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() 
  << " qualifier='";
  switch (node->qualifier()) {
    case 0:
      os() << "private";
      break;
    case 1:
      os() << "public";
      break;
    case 2:
      os() << "require";
      break;
    default:
      break;
  } 
  os () << "' id='" << node->id()
  << "' type='" << (node->type() != nullptr ? cdk::to_string(node->type()) : "procedure") << "'>" << std::endl;
  openTag("args", lvl + 2);
  if(node->args() != nullptr) node->args()->accept(this, lvl + 4);
  closeTag("args", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_input_node(og::input_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void og::xml_writer::do_return_node(og::return_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("values", lvl + 2);
  node->values()->accept(this, lvl + 4);
  closeTag("values", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}
void og::xml_writer::do_var_decl_node(og::var_decl_node *const node, int lvl) {
  os() << std::string(lvl, ' ')
       << "<" << node->label()
       << " qualifier='";
  switch (node->qualifier()) {
    case 0:
      os() << "private";
      break;
    case 1:
      os() << "public";
      break;
    case 2:
      os() << "require";
      break;
    default:
      break;
  }     
  os() << "' type='" << cdk::to_string(node->type()) 
       << "'>" << std::endl;
  openTag("ids", lvl + 2);
  for (std::string id: node->ids()) {
    os() << std::string(lvl + 4, ' ') << "<id>" << id << "</id>" << std::endl;
  }
  closeTag("ids", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_var_def_node(og::var_def_node *const node, int lvl) {
  os() << std::string(lvl, ' ')
      << "<" << node->label()
      << " qualifier='";
  switch (node->qualifier()) {
    case 0:
      os() << "private";
      break;
    case 1:
      os() << "public";
      break;
    case 2:
      os() << "require";
      break;
    default:
      break;
  }  
  os() << "' type='" << cdk::to_string(node->type()) 
       << "'>" << std::endl;
  openTag("ids", lvl + 2);
  for (std::string id : node->ids()) {
    os() << std::string(lvl + 4, ' ') << "<id>" << id << "</id>" << std::endl;
  } 
  closeTag("ids", lvl + 2);
  openTag("args", lvl + 2);
  node->args()->accept(this, lvl + 4);
  closeTag("args", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_write_node(og::write_node *const node, int lvl) {
  os() << std::string(lvl, ' ')
       << "<" << node->label()
       << " newline='" << std::boolalpha << node->newline()
       << "'>" << std::endl;
  openTag("expressions", lvl + 2);
  node->expressions()->accept(this, lvl + 4);
  closeTag("expressions", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_address_of_node(og::address_of_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("value", lvl + 2);
  node->lvalue()->accept(this, lvl + 4);
  closeTag("value", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void og::xml_writer::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}
void og::xml_writer::do_ptr_index_node(og::ptr_index_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("ptr", lvl + 2);
  node->ptr()->accept(this, lvl + 4);
  closeTag("ptr", lvl + 2);
  openTag("index", lvl + 2);
  node->index()->accept(this, lvl + 4);
  closeTag("index", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("tuple", lvl + 2);
  node->tuple()->accept(this, lvl + 4);
  closeTag("tuple", lvl + 2);
  openTag("index", lvl + 2);
  node->index()->accept(this, lvl + 4);
  closeTag("index", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_tuple_node(og::tuple_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("expressions", lvl + 2);
  node->expressions()->accept(this, lvl + 4);
  closeTag("expressions", lvl + 2);
  closeTag(node, lvl);
}
void og::xml_writer::do_identity_node(og::identity_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}