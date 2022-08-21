#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include <fstream>

#include "Extractor.h"
#include "Constraint.h"

using namespace llvm;

int runConstraints(const char * fileName) {

    LLVMContext Context;
    SMDiagnostic Err;
    StringRef FileName(fileName);

    std::unique_ptr<Module> Mod = parseAssemblyFile(FileName, Err, Context);

    if (!Mod) {
        Err.print(fileName, errs());
        return 1;
    }

    Extractor Ext;
    Ext.initialize();
    z3::fixedpoint *Solver = Ext.getSolver();
    z3::context &C = Ext.getContext();

    InstMapTy InstMap;
    unsigned int Counter = 0;
    for (auto &F: *Mod) {
        for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
            InstMap[&*I] = Counter++;
        }
    }

    for (auto &F: *Mod) {
        for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
            Ext.extractConstraints(InstMap, &*I);
        }
    }

    std::cout << "Potential divide-by-zero points:" << std::endl;
    for (auto &Entry: InstMap) {
        z3::check_result R = Ext.query(Entry.second);
        if (R == z3::sat)
            std::cout << toString(Entry.first) << std::endl;
    }
    return 0;
}
