#include <cstdio>
#include <iostream>
#include <set>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <unordered_set>
#include <set>

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

std::unordered_map<std::string, int> min_vals;
std::unordered_map<std::string, int> max_vals;
std::unordered_set<std::string> var_names;
std::unordered_set<std::string> avoidBB;

int getMinVal(std::string op)
{
    if (op[0] != '%')
    {
        // if op is constant return the number
        return std::stoi(op);
    }
    return min_vals.count(op) ? min_vals[op] : -INF;
}

void update_MinVals(std::string res, int num)
{
    // if num is less or equal to -INF no need to update
    if (num <= -INF)
    {
        return;
    }
    min_vals[res] = num;
}

int getMaxVal(std::string op)
{
    if (op[0] != '%')
    {
        // if op is constant return the number
        return std::stoi(op);
    }
    return max_vals.count(op) ? max_vals[op] : INF;
}

void update_MaxVals(std::string res, int num)
{
    // if num is greater than or equal to INF no need to update
    if (num >= INF)
    {
        return;
    }
    max_vals[res] = num;
}

std::vector<int> getMinMaxPair(std::string op0, std::string op1, char cal)
{
    std::vector<int> ans;
    int op0_min = min_vals.count(op0) ? min_vals[op0] : -INF;
    int op0_max = max_vals.count(op0) ? max_vals[op0] : INF;

    int op1_min = min_vals.count(op1) ? min_vals[op1] : -INF;
    int op1_max = max_vals.count(op1) ? max_vals[op1] : INF;

    if (cal == '*')
    {
        for (int x : {op0_min, op0_max})
        {
            for (int y : {op1_min, op1_max})
            {
                ans.push_back(x * y);
            }
        }
    }
    else if (cal == '/')
    {
        for (int x : {op0_min, op0_max})
        {
            for (int y : {op1_min, op1_max})
            {
                ans.push_back(x / y);
            }
        }
    }
    else if (cal == '%')
    {
        for (int x : {op0_min, op0_max})
        {
            for (int y : {op1_min, op1_max})
            {
                ans.push_back(x % y);
            }
        }
    }
    else
    {
        errs() << "Wrong binary operator: " << cal << '\n';
        exit(-1);
    }
    std::sort(ans.begin(), ans.end());
    return {ans[0], *(ans.rbegin())};
}

std::vector<std::string> getInterval(std::string name)
{
    std::vector<std::string> ans;
    std::string _min = min_vals.count(name) ? std::to_string(min_vals[name]) : "-INF";
    std::string _max = max_vals.count(name) ? std::to_string(max_vals[name]) : "INF";
    return {_min, _max};
}

void cal_interval()
{
    for (auto i = var_names.begin(); i != var_names.end(); ++i)
    {
        std::vector<std::string> ends = getInterval(*i);
        llvm::outs() << "The variable " << *i << "'s interval is " << '[' << ends[0] << ", " << ends[1] << "]\n";
    }
}

std::string getOpLabel(const llvm::Value *op)
{
    if (!op->getName().empty())
    {
        return op->getName().str();
    }

    std::string str;
    raw_string_ostream OS(str);
    op->printAsOperand(OS, false);
    return OS.str();
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
                if (avoidBB.count(bb.getName().str()))
                {
                    continue;
                }
                std::string bb_name = getSimpleNodeLabel(&bb);
                bool jumpTrue = false, jumpFalse = false;
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
                        var_names.insert(result);
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
                        var_names.insert(result);

                        int _min = getMinVal(op0) + getMinVal(op1);
                        int _max = getMaxVal(op0) + getMaxVal(op1);

                        update_MinVals(result, _min);
                        update_MaxVals(result, _max);
                        break;
                    }
                    case llvm::Instruction::Sub:
                    {
                        llvm::BinaryOperator *binaryinst = llvm::dyn_cast<llvm::BinaryOperator>(&i);
                        llvm::Value *operand0 = binaryinst->getOperand(0);
                        llvm::Value *operand1 = binaryinst->getOperand(1);

                        std::string op0 = getOpLabel(operand0);
                        std::string op1 = getOpLabel(operand1);

                        llvm::Value *returnval = llvm::cast<llvm::Value>(binaryinst);
                        std::string result = getOpLabel(returnval);
                        var_names.insert(result);

                        int _min = getMinVal(op0) - getMaxVal(op1);
                        int _max = getMaxVal(op0) + getMinVal(op1);

                        update_MinVals(result, _min);
                        update_MaxVals(result, _max);
                        break;
                    }
                    case llvm::Instruction::Mul:
                    {
                        llvm::BinaryOperator *binaryinst = llvm::dyn_cast<llvm::BinaryOperator>(&i);
                        llvm::Value *operand0 = binaryinst->getOperand(0);
                        llvm::Value *operand1 = binaryinst->getOperand(1);

                        std::string op0 = getOpLabel(operand0);
                        std::string op1 = getOpLabel(operand1);

                        llvm::Value *returnval = llvm::cast<llvm::Value>(binaryinst);
                        std::string result = getOpLabel(returnval);
                        var_names.insert(result);

                        std::vector<int> min_max = getMinMaxPair(op0, op1, '*');
                        int _min = min_max[0];
                        int _max = min_max[1];
                        update_MinVals(result, _min);
                        update_MaxVals(result, _max);
                        break;
                    }
                    case llvm::Instruction::SDiv:
                    {
                        llvm::BinaryOperator *binaryinst = llvm::dyn_cast<llvm::BinaryOperator>(&i);
                        llvm::Value *operand0 = binaryinst->getOperand(0);
                        llvm::Value *operand1 = binaryinst->getOperand(1);

                        std::string op0 = getOpLabel(operand0);
                        std::string op1 = getOpLabel(operand1);

                        llvm::Value *returnval = llvm::cast<llvm::Value>(binaryinst);
                        std::string result = getOpLabel(returnval);
                        var_names.insert(result);

                        std::vector<int> min_max = getMinMaxPair(op0, op1, '/');
                        int _min = min_max[0];
                        int _max = min_max[1];
                        update_MinVals(result, _min);
                        update_MaxVals(result, _max);
                        break;
                    }
                    case llvm::Instruction::SRem:
                    {
                        llvm::BinaryOperator *binaryinst = llvm::dyn_cast<llvm::BinaryOperator>(&i);
                        llvm::Value *operand0 = binaryinst->getOperand(0);
                        llvm::Value *operand1 = binaryinst->getOperand(1);

                        std::string op0 = getOpLabel(operand0);
                        std::string op1 = getOpLabel(operand1);

                        llvm::Value *returnval = llvm::cast<llvm::Value>(binaryinst);
                        std::string result = getOpLabel(returnval);
                        var_names.insert(result);

                        std::vector<int> min_max = getMinMaxPair(op0, op1, '%');
                        int _min = min_max[0];
                        int _max = min_max[1];
                        update_MinVals(result, _min);
                        update_MaxVals(result, _max);
                        break;
                    }
                    case llvm::Instruction::Load:
                    {
                        llvm::LoadInst *loadinst = llvm::dyn_cast<llvm::LoadInst>(&i);
                        llvm::Value *operand = loadinst->getOperand(0);
                        std::string op = getOpLabel(operand);

                        llvm::Value *returnval = llvm::cast<llvm::Value>(loadinst);
                        std::string result = getOpLabel(returnval);
                        var_names.insert(result);

                        int _min = getMinVal(op);
                        int _max = getMaxVal(op);

                        update_MinVals(result, _min);
                        update_MaxVals(result, _max);
                        break;
                    }
                    case llvm::Instruction::Store:
                    {
                        // store op0 -> op1
                        llvm::StoreInst *storeinst = llvm::dyn_cast<llvm::StoreInst>(&i);
                        llvm::Value *operand0 = storeinst->getOperand(0);
                        llvm::Value *operand1 = storeinst->getOperand(1);

                        std::string op0 = getOpLabel(operand0);
                        std::string op1 = getOpLabel(operand1);

                        int _min = getMinVal(op0);
                        int _max = getMaxVal(op0);

                        update_MinVals(op1, _min);
                        update_MaxVals(op1, _max);
                        break;
                    }
                    case llvm::Instruction::ICmp:
                    {
                        llvm::ICmpInst *icmpinst = llvm::dyn_cast<llvm::ICmpInst>(&i);
                        llvm::Value *operand0 = icmpinst->getOperand(0);
                        llvm::Value *operand1 = icmpinst->getOperand(1);

                        std::string op0 = getOpLabel(operand0);
                        std::string op1 = getOpLabel(operand1);

                        int min0 = getMinVal(op0);
                        int max0 = getMaxVal(op0);

                        int min1 = getMinVal(op1);
                        int max1 = getMaxVal(op1);

                        llvm::CmpInst::Predicate predicate = icmpinst->getPredicate();
                        switch (predicate)
                        {
                        case llvm::CmpInst::ICMP_EQ:
                        {
                            if (min0 == max0 && min1 == max1)
                            {
                                // op0 and op1 are fixed vars
                                if (min0 == min1)
                                {
                                    jumpTrue = true;
                                }
                                else
                                {
                                    jumpFalse = true;
                                }
                            }
                            break;
                        }
                        case llvm::CmpInst::ICMP_NE:
                        {
                            if (min0 == max0 && min1 == max1)
                            {
                                // op0 and op1 are fixed vars
                                if (min0 != min1)
                                {
                                    jumpTrue = true;
                                }
                                else
                                {
                                    jumpFalse = true;
                                }
                            }
                            break;
                        }
                        case llvm::CmpInst::ICMP_SGT:
                        {
                            if (min0 > max1)
                            {
                                jumpTrue = true;
                            }
                            else if (max0 <= min1)
                            {
                                jumpFalse = true;
                            }
                            break;
                        }
                        case llvm::CmpInst::ICMP_SGE:
                        {
                            if (min0 >= max1)
                            {
                                jumpTrue = true;
                            }
                            else if (max0 < min1)
                            {
                                jumpFalse = true;
                            }
                            break;
                        }
                        case llvm::CmpInst::ICMP_SLT:
                        {
                            if (max0 < min1)
                            {
                                jumpTrue = true;
                            }
                            else if (min0 >= max1)
                            {
                                jumpFalse = true;
                            }
                            break;
                        }
                        case llvm::CmpInst::ICMP_SLE:
                        {
                            if (max0 <= min1)
                            {
                                jumpTrue = true;
                            }
                            else if (min0 > max1)
                            {
                                jumpFalse = true;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        break;
                    }
                    case llvm::Instruction::Br:
                    {
                        llvm::BranchInst *cbrinst = llvm::dyn_cast<llvm::BranchInst>(&i);
                        if (cbrinst->getNumSuccessors() == 2)
                        {
                            llvm::BasicBlock *trueBB = cbrinst->getSuccessor(0);
                            llvm::BasicBlock *falseBB = cbrinst->getSuccessor(1);
                            if (jumpTrue)
                            {
                                // avoid falseBB
                                std::string nameBB = getSimpleNodeLabel(falseBB);
                                falseBB->setName(nameBB);
                                avoidBB.insert(nameBB);
                            }
                            else if (jumpFalse)
                            {
                                // avoid trueBB
                                std::string nameBB = getSimpleNodeLabel(trueBB);
                                trueBB->setName(nameBB);
                                avoidBB.insert(nameBB);
                            }
                        }
                        break;
                    }
                    default:
                        break;
                    }
                }

                //
                llvm::outs() << bb_name << ": " << '\n';
                cal_interval();
            }
        }
    }
}
