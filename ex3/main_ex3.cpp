#include <llvm/Support/raw_ostream.h>
#include "Verifier.h"

int main(int argc, char **argv) {
    if (argc > 3) {
        llvm::errs() << "Expected an argument - IR file name\n";
        exit(1);
    }
    doMain(argv[1]);
}
