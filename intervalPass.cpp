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

#define INF 10000

std::unordered_map<std::string, int> min_vals;
std::unordered_map<std::string, int> max_vals;
std::set<std::string> var_names;
std::unordered_set<std::string> avoidBB;
std::unordered_set<std::string> avoidBBLoop;

int getMinVal(std::string op)
{
    if ((op[0] <= '9' && op[0] >= '0') || op[0] == '-')
    {
        // if op is constant return the number
        return std::stoi(op);
    }
    return min_vals.count(op) ? min_vals[op] : -INF;
}

void update_MinVals(std::string res, int num, bool if_flag)
{
    // if num is less or equal to -INF no need to update
    if (num <= -INF)
    {
        return;
    }
    if (if_flag)
    {
        if (min_vals.count(res) == 0 || min_vals[res] > num)
        {
            min_vals[res] = num;
        }
    }
    else
    {
        min_vals[res] = num;
    }
}

int getMaxVal(std::string op)
{
    if ((op[0] <= '9' && op[0] >= '0') || op[0] == '-')
    {
        // if op is constant return the number
        return std::stoi(op);
    }
    return max_vals.count(op) ? max_vals[op] : INF;
}

void update_MaxVals(std::string res, int num, bool if_flag)
{
    // if num is greater than or equal to INF no need to update
    if (num >= INF)
    {
        return;
    }
    if (if_flag)
    {
        if (max_vals.count(res) == 0 || max_vals[res] < num)
        {
            max_vals[res] = num;
        }
    }
    else
    {
        max_vals[res] = num;
    }
}

std::vector<int> getMinMaxPair(std::string op0, std::string op1, char cal)
{
    std::vector<int> ans;
    int a = getMinVal(op0);
    int b = getMaxVal(op0);

    int c = getMinVal(op1);
    int d = getMaxVal(op1);

    if (cal == '*')
    {
        for (int x : {a, b})
        {
            for (int y : {c, d})
            {
                ans.push_back(x * y);
            }
        }
    }
    else if (cal == '/')
    {
        if (c == 0 && d == 0)
        {
            return {-INF, INF};
        }
        else if (c >= 0)
        {
            for (int x : {a, b})
            {
                for (int y : {c, d})
                {
                    ans.push_back(x / y);
                }
            }
        }
        else if (d <= 0)
        {
            for (int x : {-b, -a})
            {
                for (int y : {-d, -c})
                {
                    ans.push_back(x / y);
                }
            }
        }
        else
        {
            for (int x : {a, b})
            {
                for (auto y : {c, d})
                {
                    ans.push_back(x / y);
                }
                ans.push_back(x * INF);
                ans.push_back(x * (-INF));
            }
        }
    }
    else if (cal == '%')
    {
        for (int x : {a, b})
        {
            for (int y : {c, d})
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
        if ((*i)[0] != '%')
        {
            llvm::outs() << "The variable " << *i << "'s interval is " << '[' << ends[0] << ", " << ends[1] << "]\n";
        }
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

void analyzeBB(Function::iterator loopHead, Function::iterator loopTail, std::unordered_set<std::string> &avoidBB)
{
    bool jumpTrue = false, jumpFalse = false;
    loopTail++;

    for (auto it = loopHead; it != loopTail; ++it)
    {
        BasicBlock *bb = &*it;
        std::string bb_name = bb->getName().str();

        if (avoidBB.count(bb->getName().str()))
        {
            continue;
        }

        bool if_flag = false;
        // std::cout << "bb name: "<<bb_name<<std::endl;
        if (bb_name.size() >= 7 && bb_name.substr(0, 2) == "if")
        {
            std::string bb_name_suffix;
            if (bb_name.size() > 7)
            {
                bb_name_suffix = bb_name.substr(8);
            }
            if (avoidBB.count("if.then" + bb_name_suffix) == 0 && avoidBB.count("if.else" + bb_name_suffix) == 0)
            {
                std::cout << "set if_flag" << std::endl;
                if_flag = true;
            }
        }

        for (Instruction &i : *bb)
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
                // if (min_vals.count(result))
                // {
                //     min_vals.erase(result);
                // }
                // if (max_vals.count(result))
                // {
                //     max_vals.erase(result);
                // }

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

                update_MinVals(result, _min, if_flag);
                update_MaxVals(result, _max, if_flag);

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
                int _max = getMaxVal(op0) - getMinVal(op1);

                update_MinVals(result, _min, if_flag);

                update_MaxVals(result, _max, if_flag);

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

                update_MinVals(result, _min, if_flag);

                update_MaxVals(result, _max, if_flag);

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

                update_MinVals(result, _min, if_flag);

                update_MaxVals(result, _max, if_flag);

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

                update_MinVals(result, _min, if_flag);

                update_MaxVals(result, _max, if_flag);

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

                update_MinVals(result, _min, if_flag);
                update_MaxVals(result, _max, if_flag);

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

                update_MinVals(op1, _min, if_flag);

                update_MaxVals(op1, _max, if_flag);

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
                        avoidBB.insert(nameBB);
                    }
                    else if (jumpFalse)
                    {
                        // avoid trueBB
                        std::string nameBB = getSimpleNodeLabel(trueBB);
                        avoidBB.insert(nameBB);
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

bool jumpBack(BasicBlock *curBB, BasicBlock *otherBB)
{
    std::string cur_str = getSimpleNodeLabel(curBB).substr(1);
    std::string other_str = getSimpleNodeLabel(otherBB).substr(1);

    int cur_num = std::stoi(cur_str);
    int other_num = std::stoi(other_str);
    if (other_num <= cur_num)
    {
        return true;
    }
    else
    {
        return false;
    }
}

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

    // Set each block's name first
    // for (auto &F : *Mod)
    // {
    //     if (strncmp(F.getName().str().c_str(), "main", 4) == 0)
    //     {
    //         for (BasicBlock &bb : F)
    //         {
    //             std::string bbName = getSimpleNodeLabel(&bb);
    //             bb.setName(bbName);
    //         }
    //     }
    // }

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

                BasicBlock *BB = &bb;
                Instruction *TermInst = BB->getTerminator();

                // Check if this basic block ends in a unconditional branch that jumps back to smaller num block
                if (BranchInst *BI = dyn_cast<BranchInst>(TermInst))
                {
                    BasicBlock *Succ = BI->getSuccessor(0);
                    Function::iterator loop_head = Function::iterator(Succ);
                    Function::iterator loop_end = Function::iterator(BB);
                    if (Succ->getName().str() == "while.cond")
                    {
                        // This is a while loop body, analyze it for multiple times
                        std::unordered_map<std::string, int> cur_min_vals = min_vals;
                        std::unordered_map<std::string, int> cur_max_vals = max_vals;

                        int loopTimes = 1000;
                        int threshHold = 900;
                        while (loopTimes--)
                        {
                            avoidBBLoop.clear();
                            analyzeBB(loop_head, loop_end, avoidBBLoop);

                            for (auto it = min_vals.begin(); it != min_vals.end(); it++)
                            {
                                std::string key = it->first;
                                if (it->second <= -threshHold && cur_min_vals.count(key))
                                {
                                    cur_min_vals.erase(key);
                                    continue;
                                }
                                cur_min_vals[key] = cur_min_vals.count(key) ? std::min(it->second, cur_min_vals[key]) : it->second;
                            }
                            for (auto it = max_vals.begin(); it != max_vals.end(); it++)
                            {
                                std::string key = it->first;
                                if (it->second >= threshHold && cur_max_vals.count(key))
                                {
                                    cur_max_vals.erase(key);
                                    continue;
                                }
                                cur_max_vals[key] = cur_max_vals.count(key) ? std::max(it->second, cur_max_vals[key]) : it->second;
                            }
                            min_vals = cur_min_vals;
                            max_vals = cur_max_vals;
                        }
                    }
                }
                Function::iterator curBB = Function::iterator(BB);
                analyzeBB(curBB, curBB, avoidBB);
                // llvm::outs() << bb_name << ": " << '\n';
                // cal_interval();
            }
        }
    }
    cal_interval();
}
