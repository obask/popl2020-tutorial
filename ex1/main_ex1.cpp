#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include "Constraint.h"

int main(int argc, char **argv) {
    if (argc > 3) {
        llvm::errs() << "Expected an argument - IR file name\n";
        exit(1);
    }

//    if (argc == 3 && !strcmp(argv[1], "-d"))
//        Ext.print(InstMap);

    runConstraints(argv[1], true);
}
