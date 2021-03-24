#ifndef __OG_TARGETS_SYMBOL_H__
#define __OG_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace og {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;

    bool _isFunc = false;
    int _qualifier;
    std::vector<std::shared_ptr<cdk::basic_type>> _arg_types;
    bool _isDefined = false;
    int _offset;
    bool _isGlobal = true;

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, int qualifier) :
        _type(type), _name(name), _qualifier(qualifier) {
    }

    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name,
          bool isFunc, int qualifier,
          std::vector<std::shared_ptr<cdk::basic_type>> arg_types) :
        _type(type), _name(name), _isFunc(isFunc), _qualifier(qualifier), _arg_types(arg_types) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    bool isFunc() {
      return _isFunc;
    }

    void isFunc(bool isFunc) {
      _isFunc = isFunc;
    }

    int qualifier() {
      return _qualifier;
    }

    void qualifier(int qualifier) {
      _qualifier = qualifier;
    }

    bool isDefined() {
      return _isDefined;
    }

    void isDefined(bool isDefined) {
      _isDefined = isDefined;
    }

    int offset() {
      return _offset;
    }

    void offset(int offset) {
      _offset = offset;
    }

    void global(bool b) {
      _isGlobal = b;
    }

    int global() {
      return _isGlobal;
    }

    std::vector<std::shared_ptr<cdk::basic_type>> arg_types() {
      return _arg_types;
    }
    
    void arg_types(std::vector<std::shared_ptr<cdk::basic_type>> types) {
      _arg_types = types;
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    
    void type(std::shared_ptr<cdk::basic_type> type) {
      _type = type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string &name() const {
      return _name;
    }
  };

} // og

#endif
