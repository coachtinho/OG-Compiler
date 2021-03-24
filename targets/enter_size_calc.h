#ifndef _OG_ENTER_SIZE_CALC_H__
#define _OG_ENTER_SIZE_CALC_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/ast/basic_node.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"
#include "targets/type_checker.h"

namespace og {
    class enter_size_calc : public basic_ast_visitor {
    private:
      int _size;

    public:
        enter_size_calc(std::shared_ptr<cdk::compiler> comp) :
            basic_ast_visitor(comp), _size(0) {}

    public:
        ~enter_size_calc() {
            os().flush();
        }

        size_t size() { 
            return _size; 
        }

        virtual void do_add_node(cdk::add_node *const node, int lvl);
        virtual void do_and_node(cdk::and_node *const node, int lvl);
        virtual void do_assignment_node(cdk::assignment_node *const node, int lvl);
        virtual void do_data_node(cdk::data_node *const node, int lvl);
        virtual void do_div_node(cdk::div_node *const node, int lvl);
        virtual void do_double_node(cdk::double_node *const node, int lvl);
        virtual void do_eq_node(cdk::eq_node *const node, int lvl);
        virtual void do_ge_node(cdk::ge_node *const node, int lvl);
        virtual void do_gt_node(cdk::gt_node *const node, int lvl);
        virtual void do_integer_node(cdk::integer_node *const node, int lvl);
        virtual void do_le_node(cdk::le_node *const node, int lvl);
        virtual void do_lt_node(cdk::lt_node *const node, int lvl);
        virtual void do_mod_node(cdk::mod_node *const node, int lvl);
        virtual void do_mul_node(cdk::mul_node *const node, int lvl);
        virtual void do_ne_node(cdk::ne_node *const node, int lvl);
        virtual void do_neg_node(cdk::neg_node *const node, int lvl);
        virtual void do_nil_node(cdk::nil_node *const node, int lvl);
        virtual void do_not_node(cdk::not_node *const node, int lvl);
        virtual void do_or_node(cdk::or_node *const node, int lvl);
        virtual void do_rvalue_node(cdk::rvalue_node *const node, int lvl);
        virtual void do_string_node(cdk::string_node *const node, int lvl);
        virtual void do_sub_node(cdk::sub_node *const node, int lvl);
        virtual void do_variable_node(cdk::variable_node *const node, int lvl);
        virtual void do_address_of_node(og::address_of_node *const node, int lvl);
        virtual void do_block_node(og::block_node *const node, int lvl);
        virtual void do_break_node(og::break_node *const node, int lvl);
        virtual void do_continue_node(og::continue_node *const node, int lvl);
        virtual void do_evaluation_node(og::evaluation_node *const node, int lvl);
        virtual void do_for_node(og::for_node *const node, int lvl);
        virtual void do_func_call_node(og::func_call_node *const node, int lvl);
        virtual void do_func_decl_node(og::func_decl_node *const node, int lvl);
        virtual void do_func_def_node(og::func_def_node *const node, int lvl);
        virtual void do_identity_node(og::identity_node *const node, int lvl);
        virtual void do_if_else_node(og::if_else_node *const node, int lvl);
        virtual void do_if_node(og::if_node *const node, int lvl);
        virtual void do_input_node(og::input_node *const node, int lvl);
        virtual void do_nullptr_node(og::nullptr_node *const node, int lvl);
        virtual void do_ptr_index_node(og::ptr_index_node *const node, int lvl);
        virtual void do_return_node(og::return_node *const node, int lvl);
        virtual void do_sizeof_node(og::sizeof_node *const node, int lvl);
        virtual void do_stack_alloc_node(og::stack_alloc_node *const node, int lvl);
        virtual void do_tuple_index_node(og::tuple_index_node *const node, int lvl);
        virtual void do_tuple_node(og::tuple_node *const node, int lvl);
        virtual void do_var_decl_node(og::var_decl_node *const node, int lvl);
        virtual void do_var_def_node(og::var_def_node *const node, int lvl);
        virtual void do_sequence_node(cdk::sequence_node *const node, int lvl);
        virtual void do_write_node(og::write_node *const node, int lvl);
    
    };
}

#endif