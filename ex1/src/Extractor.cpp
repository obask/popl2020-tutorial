#include "Extractor.h"

#include "llvm/IR/Instruction.h"

void Extractor::initialize() {
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

    z3::expr kill_rule = z3::forall(X, I, J, z3::implies(Def(X, I) && Def(X, J),/* -: */Kill(X, J)));


//    PS disable intellij type hints to make it more readable
}

void Extractor::addDef(const InstMapTy &InstMap, Value *X, Instruction *L) {
  if (InstMap.find(X) == InstMap.end())
    return;
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
  Solver->add_fact(Def, Arr);
}

void Extractor::addUse(const InstMapTy &InstMap, Value *X, Instruction *L) {
  if (Constant *C = dyn_cast<Constant>(X))
    return;
  if (InstMap.find(X) == InstMap.end())
    return;
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
  Solver->add_fact(Use, Arr);
}

void Extractor::addDiv(const InstMapTy &InstMap, Value *X, Instruction *L) {
  if (Constant *C = dyn_cast<Constant>(X))
    return;
  if (InstMap.find(X) == InstMap.end())
    return;
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(L)};
  Solver->add_fact(Div, Arr);
}

void Extractor::addTaint(const InstMapTy &InstMap, Instruction *L) {
  unsigned int Arr[1] = {InstMap.at(L)};
  Solver->add_fact(Taint, Arr);
}

void Extractor::addSanitizer(const InstMapTy &InstMap, Instruction *L) {
  unsigned int Arr[1] = {InstMap.at(L)};
  Solver->add_fact(Sanitizer, Arr);
}

void Extractor::addGen(const InstMapTy &InstMap, Instruction *X, Value *Y) {
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(Y)};
  Solver->add_fact(Gen, Arr);
}

void Extractor::addNext(const InstMapTy &InstMap, Instruction *X,
                        Instruction *Y) {
  unsigned int Arr[2] = {InstMap.at(X), InstMap.at(Y)};
  Solver->add_fact(Next, Arr);
};

/*
 * Implement the following function that collects Datalog facts for each
 * instruction.
 */
void Extractor::extractConstraints(const InstMapTy &InstMap, Instruction *I) {
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
     */

    if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
        // do nothing, alloca is just a declaration.
    } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
        Value *From = SI->getValueOperand();
        Value *To = SI->getPointerOperand();
        addGen(InstMap, SI, SI);
        addDef(InstMap, To, SI);
        addUse(InstMap, From, SI);
    } else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
        // TODO: Extract facts from LoadInst
    } else if (BinaryOperator *BI = dyn_cast<BinaryOperator>(I)) {
        // TODO: Extract facts from BinaryOperation
        if (BI->isIntDivRem()) {
            //
        }
    } else if (CallInst *CI = dyn_cast<CallInst>(I)) {
        // TODO: Extract facts from CallInst
        if (!CI->getCalledFunction()->getReturnType()->isVoidTy()) {
//
        }
    } else if (CastInst *CI = dyn_cast<CastInst>(I)) {
        // TODO: Extract facts from CastInst
    } else if (CmpInst *CI = dyn_cast<CmpInst>(I)) {
        // TODO: Extract facts from CmpInst
    }
}
