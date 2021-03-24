#include "targets/enter_size_calc.h"
#include "ast/all.h"

void og::enter_size_calc::do_add_node(cdk::add_node *const node, int lvl) {}
void og::enter_size_calc::do_and_node(cdk::and_node *const node, int lvl){}
void og::enter_size_calc::do_assignment_node(cdk::assignment_node *const node, int lvl) {}
void og::enter_size_calc::do_data_node(cdk::data_node *const node, int lvl) {}
void og::enter_size_calc::do_div_node(cdk::div_node *const node, int lvl) {}
void og::enter_size_calc::do_double_node(cdk::double_node *const node, int lvl) {}
void og::enter_size_calc::do_eq_node(cdk::eq_node *const node, int lvl) {}
void og::enter_size_calc::do_ge_node(cdk::ge_node *const node, int lvl) {}
void og::enter_size_calc::do_gt_node(cdk::gt_node *const node, int lvl) {}
void og::enter_size_calc::do_integer_node(cdk::integer_node *const node, int lvl) {}
void og::enter_size_calc::do_le_node(cdk::le_node *const node, int lvl) {}
void og::enter_size_calc::do_lt_node(cdk::lt_node *const node, int lvl) {}
void og::enter_size_calc::do_mod_node(cdk::mod_node *const node, int lvl) {}
void og::enter_size_calc::do_mul_node(cdk::mul_node *const node, int lvl) {}
void og::enter_size_calc::do_ne_node(cdk::ne_node *const node, int lvl) {}
void og::enter_size_calc::do_neg_node(cdk::neg_node *const node, int lvl) {}
void og::enter_size_calc::do_nil_node(cdk::nil_node *const node, int lvl) {}
void og::enter_size_calc::do_not_node(cdk::not_node *const node, int lvl) {}
void og::enter_size_calc::do_or_node(cdk::or_node *const node, int lvl) {}
void og::enter_size_calc::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {}
void og::enter_size_calc::do_string_node(cdk::string_node *const node, int lvl) {}
void og::enter_size_calc::do_sub_node(cdk::sub_node *const node, int lvl) {}
void og::enter_size_calc::do_variable_node(cdk::variable_node *const node, int lvl) {}
void og::enter_size_calc::do_address_of_node(og::address_of_node *const node, int lvl) {}
void og::enter_size_calc::do_break_node(og::break_node *const node, int lvl) {}
void og::enter_size_calc::do_continue_node(og::continue_node *const node, int lvl) {}
void og::enter_size_calc::do_evaluation_node(og::evaluation_node *const node, int lvl) {}
void og::enter_size_calc::do_func_call_node(og::func_call_node *const node, int lvl) {}
void og::enter_size_calc::do_func_decl_node(og::func_decl_node *const node, int lvl) {}
void og::enter_size_calc::do_identity_node(og::identity_node *const node, int lvl) {}
void og::enter_size_calc::do_input_node(og::input_node *const node, int lvl) {}
void og::enter_size_calc::do_nullptr_node(og::nullptr_node *const node, int lvl) {}
void og::enter_size_calc::do_ptr_index_node(og::ptr_index_node *const node, int lvl) {}
void og::enter_size_calc::do_return_node(og::return_node *const node, int lvl) {}
void og::enter_size_calc::do_sizeof_node(og::sizeof_node *const node, int lvl) {}
void og::enter_size_calc::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {}
void og::enter_size_calc::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {}
void og::enter_size_calc::do_tuple_node(og::tuple_node *const node, int lvl) {}
void og::enter_size_calc::do_write_node(og::write_node *const node, int lvl) {}

void og::enter_size_calc::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

void og::enter_size_calc::do_for_node(og::for_node *const node, int lvl) {
    node->inits()->accept(this, lvl);
}

void og::enter_size_calc::do_func_def_node(og::func_def_node *const node, int lvl) {
    node->block()->accept(this, lvl);
}

void og::enter_size_calc::do_block_node(og::block_node *const node, int lvl) {
    if (node->decls()) {
        node->decls()->accept(this, lvl);
    }
    if (node->instrs()) {
        node->instrs()->accept(this, lvl);
    }
}

void og::enter_size_calc::do_if_else_node(og::if_else_node *const node, int lvl) {
    node->thenblock()->accept(this, lvl);
    node->elseblock()->accept(this, lvl);
}

void og::enter_size_calc::do_if_node(og::if_node *const node, int lvl) {
    node->block()->accept(this, lvl);
}

void og::enter_size_calc::do_var_decl_node(og::var_decl_node *const node, int lvl) {
    _size += node->type()->size();
}

void og::enter_size_calc::do_var_def_node(og::var_def_node *const node, int lvl) {
    _size += node->type()->size();
}
