#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include <filesystem>
#include <fstream>

#include "Extractor.hh"

using namespace llvm;

std::vector<std::string> runConstraints(const std::filesystem::path& fileName, bool debug) {

    LLVMContext Context;
    SMDiagnostic Err;
    StringRef FileName(fileName.c_str());

    std::unique_ptr<Module> Mod = parseAssemblyFile(FileName, Err, Context);

    if (!Mod) {
        Err.print(fileName.c_str(), errs());
        return {};
    }

    Extractor extractor;
    extractor.initialize();
//    z3::fixedpoint *Solver = extractor.getSolver();
    z3::context &C = extractor.getContext();

    InstMapTy InstMap;
    unsigned int Counter = 0;
    for (auto &F: *Mod) {
        for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
            InstMap[&*I] = Counter++;
        }
    }

    for (auto &F: *Mod) {
        for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; I++) {
            extractor.extractConstraints(InstMap, &*I);
        }
    }
    if (debug) {
        extractor.print(InstMap);
    }

    std::cout << "Potential divide-by-zero points:" << std::endl;
    std::vector<std::string> result;
    for (auto &Entry: InstMap) {
        z3::check_result R = extractor.query(Entry.second);
        if (R == z3::sat) {
            std::string tmp = toString(Entry.first);
            result.push_back(tmp);
            if (debug) {
                std::cout << tmp << std::endl;
            }
        }
    }
    return result;
}
