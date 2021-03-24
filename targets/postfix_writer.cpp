#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/enter_size_calc.h"
#include "ast/all.h"  // all.h is automatically generated

//---------------------------------------------------------------------------

void og::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void og::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_globalVar) {
    _pf.SINT(node->value());
  }
  else {
    _pf.INT(node->value());
  }
}

void og::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_globalVar) {
    _pf.SDOUBLE(node->value());
  }
  else {
    _pf.DOUBLE(node->value());
  }
}

void og::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  _pf.RODATA();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  _pf.SSTRING(node->value());

  if (_isTuple) {
    if (_globalVar) {
      _pf.DATA();
      _pf.ALIGN();
      _pf.SADDR(mklbl(lbl1));
    }
    else {
      _pf.TEXT();
      _pf.ALIGN();
      _pf.ADDR(mklbl(lbl1));
    }
  }
  else if (_globalVar) {
    _pf.DATA();
    _pf.ALIGN();
    if (_publicVar) {
      _pf.GLOBAL(_varLabel, _pf.OBJ());
    }
    _pf.LABEL(_varLabel);
    _pf.SADDR(mklbl(lbl1));
    _pf.TEXT();
    _pf.ALIGN();
  }
  else {
    _pf.TEXT();
    _pf.ALIGN();
    _pf.ADDR(mklbl(lbl1));
  }
}

void og::postfix_writer::do_tuple_node(og::tuple_node *const node, int lvl) {
  for (int i = node->expressions()->size() - 1; i >= 0; i--) {
    node->expressions()->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_identity_node(og::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl);
}

void og::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->argument()->accept(this, lvl);
  
  if (node->is_typed(cdk::TYPE_INT)) {
    _pf.NEG();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DNEG();
  }
  else {
    throw std::string("ERROR: CANNOT HAPPEN!");
  }
  
}

void og::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  node->argument()->accept(this, lvl);
  _pf.NOT();
}

//---------------------------------------------------------------------------

//TODO: pointer arithmetic
void og::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DSUB();
  }
  else {
    _pf.SUB();
  }
}

void og::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    std::shared_ptr<cdk::reference_type> p = std::dynamic_pointer_cast<cdk::reference_type>(node->type());
    _pf.INT(p->referenced()->size());
    _pf.MUL();  
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    std::shared_ptr<cdk::reference_type> p = std::dynamic_pointer_cast<cdk::reference_type>(node->type());
    _pf.INT(p->referenced()->size());
    _pf.MUL();  
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DADD();
  }
  else {
    _pf.ADD();
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DMUL();
  }
  else {
    _pf.MUL();
  }
}

void og::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DDIV();
  }
  else {
    _pf.DIV();
  };
}

void og::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->right()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LT();
}

void og::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->right()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }
  
  _pf.LE();
}

void og::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->right()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GE();
}

void og::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->right()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GT();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->right()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.NE();
}

void og::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->right()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.EQ();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  std::string and_end = mklbl(++_lbl);

  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.JZ(and_end);
  _pf.TRASH(4);

  _pf.ALIGN();
  _pf.LABEL(and_end);

}

void og::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  std::string or_end = mklbl(++_lbl);
  
  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.JNZ(or_end);
  _pf.TRASH(4);

  node->right()->accept(this, lvl);

  _pf.ALIGN();
  _pf.LABEL(or_end);

}

//---------------------------------------------------------------------------

void og::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<og::symbol> symbol = _symtab.find(node->name());

  if (!symbol) {
    throw std::string("Symbol " + node->name() + " not found");    
  }
  if (symbol->global()) {
    _pf.ADDR(node->name());
  }
  else {
    _pf.LOCAL(symbol->offset());
  }
}

void og::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_STRUCT) && !_isTupleIndex) {
    size_t pos = node->type()->size();
    std::shared_ptr<cdk::structured_type> type = std::dynamic_pointer_cast<cdk::structured_type>(node->type());

    if (type == nullptr) {
      throw std::string("Failed cast in rvalue node");
    }

    for (int i = type->components().size() - 1; i >= 0; i--) {
      pos -= type->component(i)->size();
      node->lvalue()->accept(this, lvl);
      _pf.INT(pos);
      _pf.ADD();
      if (type->component(i)->name() == cdk::TYPE_DOUBLE) {
        _pf.LDDOUBLE();
      } else {
        _pf.LDINT();
      }
    }
  } else {
    node->lvalue()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _pf.LDDOUBLE();    
    } else if (!node->is_typed(cdk::TYPE_STRUCT)) {
      _pf.LDINT();
    }
  }
}

void og::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->rvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DUP64();
  }
  else {
    _pf.DUP32();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE) && node->rvalue()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.STDOUBLE();
  }
  else {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_evaluation_node(og::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->argument()->accept(this, lvl);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.TRASH(4);
  } 
  else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4);
  } 
  else if (node->argument()->is_typed(cdk::TYPE_POINTER)) {
    _pf.TRASH(4);
  } 
  else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.TRASH(8);
  }
  else if (node->argument()->is_typed(cdk::TYPE_STRUCT)) {
    _pf.TRASH(node->argument()->type()->size());
  }
  else if (node->argument()->is_typed(cdk::TYPE_VOID)) {
  }
  else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void og::postfix_writer::do_for_node(og::for_node * const node, int lvl) {
  std::string forCond = mklbl(++_lbl);
  std::string forEnd = mklbl(++_lbl);
  std::string forIncr = mklbl(++_lbl);

  _continue.push_back(forIncr);
  _break.push_back(forEnd);

  _symtab.push();

  node->inits()->accept(this, lvl);
  _pf.LABEL(forCond);
  node->condition()->accept(this, lvl);
  _pf.JZ(forEnd);
  node->block()->accept(this, lvl);
  _pf.LABEL(forIncr);
  node->incrs()->accept(this, lvl);
  _pf.JMP(forCond);
  _pf.LABEL(forEnd);

  _symtab.pop();

  _continue.pop_back();
  _break.pop_back();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_if_node(og::if_node * const node, int lvl) {
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_if_else_node(og::if_else_node * const node, int lvl) {
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1)); 
  node->elseblock()->accept(this, lvl);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

void og::postfix_writer::do_block_node(og::block_node *const node, int lvl) {
  _symtab.push();
  if (node->decls())
    node->decls()->accept(this, lvl);
  if (node->instrs())
    node->instrs()->accept(this, lvl);
  _symtab.pop();
}

void og::postfix_writer::do_break_node(og::break_node *const node, int lvl) {
  if (_break.empty())
    throw std::string("break not in for");
    
  _pf.JMP(_break.back());
}

void og::postfix_writer::do_continue_node(og::continue_node *const node, int lvl) {
  if (_continue.empty())
    throw std::string("continue not in for");
    
  _pf.JMP(_continue.back());
}

void og::postfix_writer::do_func_call_node(og::func_call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  size_t argSize = 0;

  std::shared_ptr<og::symbol> symbol = _symtab.find(node->id());

  if (symbol->type()->name() == cdk::TYPE_STRUCT) {
    _pf.INT(symbol->type()->size());
    _pf.ALLOC();
  }

  for (int i = node->args()->size() - 1; i >= 0; i--) {
    node->args()->node(i)->accept(this, lvl);
    cdk::typed_node *typed_node = dynamic_cast<cdk::typed_node*>(node->args()->node(i));
    argSize += typed_node->type()->size();
    if (typed_node->is_typed(cdk::TYPE_INT) && symbol->arg_types().at(i)->name() == cdk::TYPE_DOUBLE) {
      _pf.I2D();
    }
  }

  if (symbol->type()->name() == cdk::TYPE_STRUCT) {
    _pf.SP();
    _pf.INT(argSize);
    _pf.ADD();
    argSize += 4;
  }

  _pf.CALL(node->id());

  if (argSize) {
    _pf.TRASH(argSize);
  }


  if (!node->is_typed(cdk::TYPE_VOID) && !node->is_typed(cdk::TYPE_STRUCT)) {
    if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _pf.LDFVAL64();
    } else {
      _pf.LDFVAL32();
    }
  }
}

void og::postfix_writer::do_func_decl_node(og::func_decl_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  pushFunctionToDeclare(node->id());
}

void og::postfix_writer::do_func_def_node(og::func_def_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;


  _currentFunction = _symtab.find(node->id());
  if (!_currentFunction)
    throw std::string("Symbol not found");

  og::enter_size_calc stackSize(_compiler);
  node->accept(&stackSize, lvl);

  _globalVar = false;
  _localOffset = 0;
  _argOffset = 8;

  if (_currentFunction->type()->name() == cdk::TYPE_STRUCT) {
    _argOffset += 4;
  }

  _symtab.push();

  _pf.TEXT();
  _pf.ALIGN();

  if (node->qualifier() == 1)
    _pf.GLOBAL(node->id(), _pf.FUNC());

  _pf.LABEL(node->id());
  _pf.ENTER(stackSize.size());

  if (node->args()) {
    _argVar = true;
    node->args()->accept(this, lvl);
    _argVar = false;
  }
  _returned = false;
  node->block()->accept(this, lvl);

  if (!_returned) {
    if (node->type()->name() != cdk::TYPE_VOID)
      throw std::string("No return in non procedure function");
    _pf.LEAVE();
    _pf.RET();
  }

  _globalVar = true;
  _currentFunction = nullptr;
  _symtab.pop();
  popFunctionToDeclare(node->id());
}

void og::postfix_writer::do_input_node(og::input_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  if (node->is_typed(cdk::TYPE_INT)) {
    pushFunctionToDeclare("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    pushFunctionToDeclare("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  } else {
    throw std::string("Unsupported type in input");
  }
}

void og::postfix_writer::copy_tuple(cdk::expression_node *const node, int lvl) {
  og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node);
  std::shared_ptr<cdk::structured_type> structType = std::dynamic_pointer_cast<cdk::structured_type>(_currentFunction->type());

  size_t position = 0;

  if (tuple == nullptr || structType == nullptr) {
    throw std::string("Failed cast in copy tuple");
  }

  for (size_t i = 0; i < tuple->expressions()->size(); i++) {
    cdk::expression_node *expr = dynamic_cast<cdk::expression_node*>(tuple->expressions()->node(i));
    
    if (expr == nullptr) {
      throw std::string("Failed cast in copy tuple");
    }
    
    expr->accept(this, lvl);
    if (structType->component(i)->name() == cdk::TYPE_DOUBLE && expr->is_typed(cdk::TYPE_INT)) {
      _pf.I2D();
    }
    _pf.LOCAL(8);
    _pf.LDINT();
    _pf.INT(position);
    _pf.ADD();
    
    if (structType->component(i)->name() == cdk::TYPE_DOUBLE) {
      _pf.STDOUBLE();
      position += 8;
    } else {
      _pf.STINT();
      position += 4;
    }
  }
}

void og::postfix_writer::do_return_node(og::return_node *const node, int lvl) {
  if (node->values()->is_typed(cdk::TYPE_STRUCT)) {
    copy_tuple(node->values(), lvl);
  } else {
    node->values()->accept(this, lvl);
    if (_currentFunction->type()->name() == cdk::TYPE_DOUBLE) {
      if (node->values()->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
      }
      _pf.STFVAL64();
    } else {
      _pf.STFVAL32();
    }
  }

  _returned = true;
  _pf.LEAVE();
  _pf.RET();
}

void og::postfix_writer::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->argument()->type()->size());
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_var_decl_node(og::var_decl_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  std::string id = node->ids()[0]; //TODO: fix me
  std::shared_ptr<og::symbol> symbol =  _symtab.find_local(id);

  if (!symbol) {
    throw std::string("Unable to find" + id);
  } 

  // If node refers to a global declaration
  if (_globalVar) {
    symbol->global(true);
    if (node->qualifier() == 2) {
      pushFunctionToDeclare(id);
    }
    else {
      _pf.BSS();
      _pf.ALIGN();
      if (node->qualifier() == 1) {
        _pf.GLOBAL(id, "OBJ");
      }
      _pf.LABEL(id);
      _pf.SALLOC(node->type()->size());
      _pf.TEXT();
      _pf.ALIGN();
    }
  }
  // If node refers to an argument of a function
  else if (_argVar) {
    symbol->global(false);
    symbol->offset(_argOffset);
    _argOffset += symbol->type()->size();
  }
  // If node refers to a local declaration
  else {
    symbol->global(false);
    _localOffset -= node->type()->size();
    symbol->offset(_localOffset);
  }
}

void og::postfix_writer::do_var_def_node(og::var_def_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS
  _varLabel = node->ids()[0];
  std::shared_ptr<og::symbol> symbol;

  // If node refers to a global definition
  if (_globalVar) {

    // Multiple definitions (auto a, b, c = 1, 2, 3)
    if (node->is_typed(cdk::TYPE_STRUCT) && node->ids().size() != 1) {
      if (node->qualifier() == 1) {
        _publicVar = true;
      }
      og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node->args());
      if (tuple == nullptr) {
        throw std::string("Failed cast in var def node");
      }
      for (size_t i = 0; i < node->ids().size(); i++) {
        _varLabel = node->ids()[i];
        symbol = _symtab.find_local(_varLabel);
        if (!symbol) {
          throw std::string("Unable to find " + _varLabel);
        }
        cdk::typed_node *tn = dynamic_cast<cdk::typed_node*>(tuple->expressions()->node(i));
        if (tn == nullptr) {
          throw new std::string("Failed cast in var def");
        }
        if (tn->is_typed(cdk::TYPE_STRING)) {
          tn->accept(this, lvl);
        }
        else {
          _pf.DATA();
          _pf.ALIGN();
          if (_publicVar) {
            _pf.GLOBAL(_varLabel, _pf.OBJ());
          }
          _pf.LABEL(_varLabel);
          tn->accept(this, lvl);
        }
      }
      return;
    }

    // Tuple definition (auto a = 1, 2, 3)
    if (node->is_typed(cdk::TYPE_STRUCT) && node->ids().size() == 1) {
      _isTuple = true;
      if (node->qualifier() == 1) {
        _publicVar = true;
      }
      _pf.DATA();
      _pf.ALIGN();
      if (_publicVar) {
        _pf.GLOBAL(_varLabel, _pf.OBJ());
      }
      _pf.LABEL(_varLabel);
      og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node->args());
      if (tuple == nullptr) {
        throw std::string("Failed cast in var def node");
      }
      for (size_t i = 0; i < tuple->expressions()->nodes().size(); i++) {
        cdk::typed_node *tn = dynamic_cast<cdk::typed_node*>(tuple->expressions()->node(i));
        if (tn == nullptr) {
          throw new std::string("Failed cast in var def");
        }
        if (tn->is_typed(cdk::TYPE_STRING)) {
          tn->accept(this, lvl);
        }
        else {
          tn->accept(this, lvl);
        }
      }
      _isTuple = false;
      return;
    }

    // Single definition
    symbol = _symtab.find_local(_varLabel);
    symbol->global(true);
    if (node->is_typed(cdk::TYPE_STRING)) {
      if (node->qualifier() == 1) {
        _publicVar = true;
      }
      node->args()->accept(this, lvl);
    }
    else {
      _pf.DATA();
      _pf.ALIGN();
      if (node->qualifier() == 1) {
        _pf.GLOBAL(_varLabel, _pf.OBJ());
      }
      _pf.LABEL(_varLabel);
      if (node->is_typed(cdk::TYPE_DOUBLE) && node->args()->is_typed(cdk::TYPE_INT)) {
        cdk::integer_node *intNode = dynamic_cast<cdk::integer_node*>(node->args());
        cdk::double_node *doubleNode = new cdk::double_node(intNode->lineno(), intNode->value());
        doubleNode->accept(this, lvl);
      }
      else {
        node->args()->accept(this, lvl);
      }
    }
  }

  // If node refers to a local definition
  else {

    // Multiple definitions (auto a, b, c = 1, 2, 3)
    if (node->is_typed(cdk::TYPE_STRUCT) && node->ids().size() != 1) {
      if (node->qualifier() == 1) {
        _publicVar = true;
      }
      og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node->args());
      if (tuple == nullptr) {
        throw std::string("Failed cast in var def node");
      }
      tuple->accept(this, lvl);
      std::shared_ptr<cdk::structured_type> st = std::dynamic_pointer_cast<cdk::structured_type>(tuple->type());
      if (st == nullptr) {
        throw std::string("Failed cast in var def node");
      }
      for (size_t i = 0; i < node->ids().size(); i++) {
        _varLabel = node->ids()[i];
        symbol = _symtab.find_local(_varLabel);
        if (!symbol) {
          throw std::string("Unable to find " + _varLabel);
        }
        symbol->global(false);
        _localOffset -= st->component(i)->size();
        symbol->offset(_localOffset);
        _pf.LOCAL(symbol->offset());
        if (st->component(i)->name() == cdk::TYPE_DOUBLE) {
          _pf.STDOUBLE();
        }
        else {
          _pf.STINT();
        }
      }
      return;
    }

    // Tuple definition (auto a = 1, 2, 3)
    if (node->is_typed(cdk::TYPE_STRUCT) && node->ids().size() == 1) {
      _isTuple = true;
      symbol = _symtab.find_local(_varLabel);
      if (!symbol) {
        throw std::string("Unable to find " + _varLabel);
      }
      symbol->global(false);
      _localOffset -= node->type()->size();
      symbol->offset(_localOffset);
      int cumulativeOffset = symbol->offset();
      og::tuple_node *tuple = dynamic_cast<og::tuple_node*>(node->args());
      if (tuple == nullptr) {
        throw std::string("Failed cast in var def node");
      } 
      tuple->accept(this, lvl);
      std::shared_ptr<cdk::structured_type> st = std::dynamic_pointer_cast<cdk::structured_type>(tuple->type());
      if (st == nullptr) {
        throw std::string("Failed cast in var def node");
      }
      for (size_t i = 0; i < st->components().size(); i++) {
        _pf.LOCAL(cumulativeOffset);
        if (st->component(i)->name() == cdk::TYPE_DOUBLE) {
          _pf.STDOUBLE();
        }
        else {
          _pf.STINT();
        }
        cumulativeOffset += st->component(i)->size();
      }

      _isTuple = false;
      return;
    }

    // Single definition
    else {
      symbol = _symtab.find_local(_varLabel);
      if (!symbol) {
        throw std::string("Unable to find " + _varLabel);
      }
      symbol->global(false);
      _localOffset -= node->type()->size();
      symbol->offset(_localOffset);
      node->args()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_DOUBLE) && node->args()->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
      }
      _pf.LOCAL(symbol->offset());
      if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.STDOUBLE();
      }
      else {
        _pf.STINT();
      }
    }
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_write_node(og::write_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->expressions()->accept(this, lvl);

  if (node->expressions()->is_typed(cdk::TYPE_INT)) {
    pushFunctionToDeclare("printi");
    _pf.CALL("printi");
    _pf.TRASH(4);
  } else if (node->expressions()->is_typed(cdk::TYPE_DOUBLE)) {
    pushFunctionToDeclare("printd");
    _pf.CALL("printd");
    _pf.TRASH(8);
  } else if (node->expressions()->is_typed(cdk::TYPE_STRING)) {
    pushFunctionToDeclare("prints");
    _pf.CALL("prints");
    _pf.TRASH(4);
  } else if (node->expressions()->is_typed(cdk::TYPE_STRUCT)) {
    std::shared_ptr<cdk::structured_type> type = std::dynamic_pointer_cast<cdk::structured_type>(node->expressions()->type());

    if (type == nullptr) {
      throw std::string("Failed cast in write node");
    }

    for (size_t i = 0; i < type->components().size(); i++) {
      if (type->component(i)->name() == cdk::TYPE_INT) {
        pushFunctionToDeclare("printi");
        _pf.CALL("printi");
        _pf.TRASH(4);
      } else if (type->component(i)->name() == cdk::TYPE_DOUBLE) {
        pushFunctionToDeclare("printd");
        _pf.CALL("printd");
        _pf.TRASH(8);
      } else if (type->component(i)->name() == cdk::TYPE_STRING) {
        pushFunctionToDeclare("prints");
        _pf.CALL("prints");
        _pf.TRASH(4);
      } else {
        throw std::string("ERROR: Invalid type in write");
        exit(1);
      }
    }

  } else {
    throw std::string("ERROR: Invalid type in write");
    exit(1);
  }

  if (node->newline()) {
    pushFunctionToDeclare("println");
    _pf.CALL("println");
  }
}

void og::postfix_writer::do_address_of_node(og::address_of_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
}

void og::postfix_writer::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(0);
}

void og::postfix_writer::do_stack_alloc_node(og::stack_alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<cdk::reference_type> type = std::dynamic_pointer_cast<cdk::reference_type>(node->type());

  node->argument()->accept(this, lvl);
  _pf.INT(type->referenced()->size());
  _pf.MUL();
  _pf.ALLOC();
  _pf.SP();
}

void og::postfix_writer::do_ptr_index_node(og::ptr_index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->ptr()->accept(this, lvl);
  node->index()->accept(this, lvl);
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
}

// NO GOOD
// NEEDS WORK
void og::postfix_writer::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int i = node->index()->value() - 1;
  size_t pos = 0;
  std::shared_ptr<cdk::structured_type> type = std::dynamic_pointer_cast<cdk::structured_type>(node->tuple()->type());
  
  if (type == nullptr) {
    throw std::string("Failed cast in tuple index node");
  }
  
  _isTupleIndex = true;
  node->tuple()->accept(this, lvl);
  _isTupleIndex = false;
  for (int j = 0; j < i; j++) {
    pos += type->component(j)->size();
  }
  _pf.INT(pos);
  _pf.ADD();
}
