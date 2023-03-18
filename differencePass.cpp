#include <cstdio>
#include <iostream>
#include <set>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"

// Yu Cong | E0925519 | benyucong@gmail.com

using namespace llvm;

std::string getOpLabel(const llvm::Value *op);
std::string getSimpleNodeLabel(BasicBlock *Node);

// Helper method for converting the name of a LLVM type to a string
static std::string LLVMTypeAsString(const Type *T)
{
    std::string TypeName;
    raw_string_ostream N(TypeName);
    T->print(N);
    return N.str();
}

class GVNames : public ModulePass
{
public:
    GVNames() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M)
    {
        for (Module::const_global_iterator GI = M.global_begin(),
                                           GE = M.global_end();
             GI != GE; ++GI)
        {
            outs() << "Found global named \"" << GI->getName()
                   << "\": type = " << LLVMTypeAsString(GI->getType()) << "\n";
        }
        return false;
    }

    // The address of this member is used to uniquely identify the class. This is
    // used by LLVM's own RTTI mechanism.
    static char ID;
};

char GVNames::ID = 0;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        errs() << "Usage: " << argv[0] << " <IR file>\n";
        return 1;
    }
    // Parse the input LLVM IR file into a module.
    SMDiagnostic Err;
    LLVMContext Context;
    std::unique_ptr<Module> Mod(parseIRFile(argv[1], Err, Context));
    if (!Mod)
    {
        Err.print(argv[0], errs());
        return 1;
    }
}