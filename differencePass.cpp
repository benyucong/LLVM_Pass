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

#define INF 0x3f3f3f3f

std::string getOpLabel(const llvm::Value *op);
std::string getSimpleNodeLabel(BasicBlock *Node);

std::unordered_map<std::string, int> min_vals;
std::unordered_map<std::string, int> max_vals;

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
    for (auto &F : *Mod)
    {
        if (strncmp(F.getName().str().c_str(), "main", 4) == 0)
        {
            for (BasicBlock &bb : F)
            {
                for (Instruction &i : bb)
                {
                    unsigned opcode = i.getOpcode();
                    switch (opcode)
                    {
                        // different cases
                    case llvm::Instruction::Alloca:
                    {
                        // result = alloc type
                        llvm::AllocaInst *allocainst = llvm::dyn_cast<llvm::AllocaInst>(&i);
                        llvm::Value *returnval = llvm::cast<llvm::Value>(allocainst);
                        std::string result = getOpLabel(returnval);
                        if (min_vals.count(result))
                        {
                            min_vals.erase(result);
                        }
                        if (max_vals.count(result))
                        {
                            max_vals.erase(result);
                        }

                        break;
                    }
                    case llvm::Instruction::Add:
                    {
                        // result = biop type operand0, operand1
                        llvm::BinaryOperator *binaryinst = llvm::dyn_cast<llvm::BinaryOperator>(&i);
                        llvm::Value *operand0 = binaryinst->getOperand(0);
                        llvm::Value *operand1 = binaryinst->getOperand(1);

                        std::string op0 = getOpLabel(operand0);
                        std::string op1 = getOpLabel(operand1);

                        llvm::Value *returnval = llvm::cast<llvm::Value>(binaryinst);
                        std::string result = getOpLabel(returnval);

                        int add0 = INF, add1 = INF;
                        bool is_op0_cons = op0[0] == '%' ? false : true;
                        bool is_op1_cons = op1[0] == '%' ? false : true;
                        if(is_op0_cons && is_op1_cons){
                            int res = std::stoi(op0) + std::stoi(op1);
                        }
                    }
                    }
                }
            }
        }
    }
}

std::string getSimpleNodeLabel(BasicBlock *Node)
{
    if (!Node->getName().empty())
        return Node->getName().str();
    std::string Str;
    raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}