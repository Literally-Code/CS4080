package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

class LoxClass implements LoxCallable {
  final String name;
  final List<LoxClass> superclasses;
  private final Map<String, LoxFunction> methods;

  LoxClass(String name, List<LoxClass> superclasses,
           Map<String, LoxFunction> methods) {
    this.superclasses = superclasses;
    this.name = name;
    this.methods = methods;
  }
  
  // Return the method at the top of the hierarchy.  This is also the
  // method whose arity is used when the class is called.
  LoxFunction findMethod(String name) {
    for (LoxClass superclass : superclasses) {
      LoxFunction method = superclass.findMethod(name);
      if (method != null) return method;
    }

    return methods.get(name);
  }

  // Build a bound chain whose first callable is the highest implementation.
  // Each function's "inner" binding points to the next implementation below
  // it in the inheritance hierarchy.
  LoxFunction findMethod(LoxInstance instance, String name) {
    List<LoxFunction> methods = new ArrayList<>();
    collectMethods(name, methods);

    LoxFunction inner = null;
    for (int i = methods.size() - 1; i >= 0; i--) {
      inner = methods.get(i).bind(instance, inner);
    }

    return inner;
  }

  private void collectMethods(String name, List<LoxFunction> methods) {
    for (LoxClass superclass : superclasses) {
      superclass.collectMethods(name, methods);
    }

    if (this.methods.containsKey(name)) {
      methods.add(this.methods.get(name));
    }
  }

  @Override
  public String toString() {
    return name;
  }
  @Override
  public Object call(Interpreter interpreter,
                     List<Object> arguments) {
    LoxInstance instance = new LoxInstance(this);
    LoxFunction initializer = findMethod(instance, "init");
    if (initializer != null) {
      initializer.call(interpreter, arguments);
    }

    return instance;
  }

  @Override
  public int arity() {
    LoxFunction initializer = findMethod("init");
    if (initializer == null) return 0;
    return initializer.arity();
  }
}
