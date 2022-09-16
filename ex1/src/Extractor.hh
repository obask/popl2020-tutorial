#include "llvm/IR/Instruction.h"

#include "z3++.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include <set>
#include <map>
#include <iostream>

#include "Utils.hh"

using namespace llvm;

using InstMapTy = std::map<Value *, unsigned int>;
using DefMapTy = std::map<Value *, std::set<Value *>>;

class Extractor
{
public:
  Extractor()
  {
    Solver = new z3::fixedpoint(C);
    Params = new z3::params(C);
    Params->set("engine", "datalog");
    Solver->set(*Params);
  }

  ~Extractor()
  {
    delete Solver;
    delete Params;
  }

  void initialize();
  z3::fixedpoint *getSolver() { return Solver; }
  z3::context &getContext() { return C; }
  z3::check_result query(unsigned int N)
  {
    z3::expr Q = Alarm(C.bv_val(N, 32));
    return Solver->query(Q);
  }

  void addDef(const InstMapTy &InstMap, Value *X, Instruction *L)
  {
    if (InstMap.find(X) == InstMap.end())
      return;
    unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
    Solver->add_fact(Def, Arr);
  }

  void addUse(const InstMapTy &InstMap, Value *X, Instruction *L)
  {
    if (Constant *C = dyn_cast<Constant>(X))
      return;
    if (InstMap.find(X) == InstMap.end())
      return;
    unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
    Solver->add_fact(Use, Arr);
  }

  void addDiv(const InstMapTy &InstMap, Value *X, Instruction *L)
  {
    if (Constant *C = dyn_cast<Constant>(X))
      return;
    if (InstMap.find(X) == InstMap.end())
      return;
    unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
    Solver->add_fact(Div, Arr);
  }

  void addTaint(const InstMapTy &InstMap, Instruction *L)
  {
    unsigned int Arr[1] = {InstMap.at(L)};
    Solver->add_fact(Taint, Arr);
  }

  void addSanitizer(const InstMapTy &InstMap, Instruction *L)
  {
    unsigned int Arr[1] = {InstMap.at(L)};
    Solver->add_fact(Sanitizer, Arr);
  }

  void addGen(const InstMapTy &InstMap, Instruction *X, Value *Y)
  {
    unsigned int Arr[2] = {InstMap.at(X), InstMap.at(Y)};
    Solver->add_fact(Gen, Arr);
  }

  void addNext(const InstMapTy &InstMap, Instruction *X,
                          Instruction *Y)
  {
    unsigned int Arr[2] = {InstMap.at(X), InstMap.at(Y)};
    Solver->add_fact(Next, Arr);
  };

  void extractConstraints(const InstMapTy &InstMap, Instruction *I);

  static void printTuple(const std::string &Name, Value *V1, Value *V2)
  {
    std::cout << Name << "(\"" << toString(V1) << "\", \"" << toString(V2)
              << "\")" << std::endl;
  }

  void print(InstMapTy &InstMap)
  {
    std::cout << "=== Reaching Definition (Out) ===" << std::endl;
    for (auto &V1 : InstMap)
    {
      for (auto &V2 : InstMap)
      {
        z3::expr Q = Out(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat)
        {
          printTuple("Out", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Kill ===" << std::endl;
    for (auto &V1 : InstMap)
    {
      for (auto &V2 : InstMap)
      {
        z3::expr Q = Kill(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat)
        {
          printTuple("Kill", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Def ===" << std::endl;
    for (auto &V1 : InstMap)
    {
      for (auto &V2 : InstMap)
      {
        z3::expr Q = Def(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat)
        {
          printTuple("Def", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Use ===" << std::endl;
    for (auto &V1 : InstMap)
    {
      for (auto &V2 : InstMap)
      {
        z3::expr Q = Use(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat)
        {
          printTuple("Use", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Edge ===" << std::endl;
    for (auto &V1 : InstMap)
    {
      for (auto &V2 : InstMap)
      {
        z3::expr Q = Edge(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat)
        {
          printTuple("Edge", V1.first, V2.first);
        }
      }
    }
    std::cout << "=== Path ===" << std::endl;
    for (auto &V1 : InstMap)
    {
      for (auto &V2 : InstMap)
      {
        z3::expr Q = Path(C.bv_val(V1.second, 32), C.bv_val(V2.second, 32));
        if (Solver->query(Q) == z3::sat)
        {
          printTuple("Path", V1.first, V2.first);
        }
      }
    }
  }

private:
  std::map<Value *, std::set<Value *>> DefMap;

  z3::context C;
  z3::fixedpoint *Solver;
  z3::params *Params;
  z3::check_result Result;
  z3::sort LLVMInst = C.bv_sort(32);

public:
  /* Relations for Def and Use */
  z3::func_decl Def = C.function("Def", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Use = C.function("Use", LLVMInst, LLVMInst, C.bool_sort());

  /* Relations for Reaching Definition */
  z3::func_decl Kill = C.function("Kill", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Gen = C.function("Gen", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Next = C.function("Next", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl In = C.function("In", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Out = C.function("Out", LLVMInst, LLVMInst, C.bool_sort());

  /* Relations for Taint Analysis */
  z3::func_decl Taint = C.function("Taint", LLVMInst, C.bool_sort());
  z3::func_decl Edge = C.function("Edge", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Path = C.function("Path", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Sanitizer = C.function("Sanitizer", LLVMInst, C.bool_sort());
  z3::func_decl Div = C.function("Div", LLVMInst, LLVMInst, C.bool_sort());
  z3::func_decl Alarm = C.function("Alarm", LLVMInst, C.bool_sort());
};

void Extractor::initialize()
{
  /* Relations for Def and Use */
  Solver->register_relation(Def);
  Solver->register_relation(Use);

  /* Relations for Reaching Definition Analysis */
  Solver->register_relation(Kill);
  Solver->register_relation(Gen);
  Solver->register_relation(Next);
  Solver->register_relation(In);
  Solver->register_relation(Out);

  /* Relations for Taint Analysis */
  Solver->register_relation(Taint);
  Solver->register_relation(Edge);
  Solver->register_relation(Path);
  Solver->register_relation(Sanitizer);
  Solver->register_relation(Div);
  Solver->register_relation(Alarm);

  /*
   * Add your code here:
   * Define your analysis rules for taint analysis and add the rules to the
   * solver.
   */

  z3::expr X = C.bv_const("X", 32);
  z3::expr I = C.bv_const("I", 32);
  z3::expr J = C.bv_const("J", 32);

  z3::expr kill_rule = z3::forall(X, I, J, z3::implies(Def(X, I) && Def(X, J), /* -: */ Kill(X, J)));

}

/*
 * Implement the following function that collects Datalog facts for each
 * instruction.
 */
void Extractor::extractConstraints(const InstMapTy &InstMap, Instruction *I)
{
  /**
   * TODO: For each predecessor P of instruction I,
   *       add a new fact in the `next` relation.
   */

  /**
   * TODO:
   *
   *   For each of the instruction add appropriate facts:
   *     Add `def` and `use` relations.
   *   For `BinaryOperator` instructions involving division:
   *     Add a fact for the `div` relation.
   *   For `CallInst` instructions:
   *     Add a `def` relation only if it returns a non-void value.
   *     If its a call to tainted input,
   *       Add appropriate fact to `taint` relation.
   *     If its a call to sanitize,
   *       Add appropriate fact to `sanitizer` relation.
   *
   * NOTE: Many Values may be used in a single instruction,
   *       but at most one Value can be defined in one instruction.
   * NOTE: You can use `isTaintedInput()` and `isSanitizer()` function
   *       to check if a particular CallInst is a tainted input
   *       or sanitize respectively.
   * NOTE: Make sure to use getPredecessors instruction from Utils.cpp
   * 
   * Def(X,Y): Variable X is defined at instruction Y
   * Use(X,Y): Variable X is used at instruction Y
   * Next(X,Y): Instruction Y is an immediate successor of instruction X
   * Gen(X,Y): Definition Y is generated by instruction X
   * Kill(X,Y): Definition X is killed by instruction Y
   * Out(X, Y) :- Definition Y may reach the program point just after instruction X
   * In(X, Y) :- Definition Y may reach the program point just before instruction X
   * 
   * 
   * ## Taint analysis
   * 
   * Def(X,Y): Variable X is defined at instruction Y
   * Use(X,Y): Variable X is used at instruction Y
   * In(X,Y): Instruction Y may reach the program point just before instruction X
   * Taint(X): There exists a function call at instruction X that reads a tainted input
   * Sanitizer(X): There exists a function call at instruction X that sanitizes a tainted input
   * Div(X,Y): There exists a division operation at instruction Y whose divisor is variable X
   * 
   * Edge(I, J) :- There exists an immediate data-flow from instruction I to J
   * Path(I, J) :- There exists a transitive tainted data-flow from instruction I to J
   * Alarm( I ) :- There exists a potential exploitable divide-by-zero error at instruction I  
   * 


   * 
   */

  if (AllocaInst *AI = dyn_cast<AllocaInst>(I))
  {
    // do nothing, alloca is just a declaration.
  }
  else if (StoreInst *SI = dyn_cast<StoreInst>(I))
  {
    Value *From = SI->getValueOperand();
    Value *To = SI->getPointerOperand();
    addGen(InstMap, SI, SI);
    addDef(InstMap, To, SI);
    addUse(InstMap, From, SI);
  }
  else if (LoadInst *LI = dyn_cast<LoadInst>(I))
  {
    // TODO: Extract facts from LoadInst
  }
  else if (BinaryOperator *BI = dyn_cast<BinaryOperator>(I))
  {
    // TODO: Extract facts from BinaryOperation
    if (BI->isIntDivRem())
    {
      //
    }
  }
  else if (CallInst *CI = dyn_cast<CallInst>(I))
  {
    // TODO: Extract facts from CallInst
    if (!CI->getCalledFunction()->getReturnType()->isVoidTy())
    {
      //
    }
  }
  else if (CastInst *CI = dyn_cast<CastInst>(I))
  {
    // TODO: Extract facts from CastInst
  }
  else if (CmpInst *CI = dyn_cast<CmpInst>(I))
  {
    // TODO: Extract facts from CmpInst
  }
}
