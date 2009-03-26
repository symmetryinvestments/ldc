#ifndef LDC_IR_IRFUNCTION_H
#define LDC_IR_IRFUNCTION_H

#include "gen/llvm.h"
#include "ir/ir.h"
#include "ir/irlandingpad.h"

#include <vector>
#include <stack>
#include <map>

struct Statement;
struct EnclosingHandler;

// scope statements that can be target of jumps
// includes loops, switch, case, labels
struct IRTargetScope
{
    // generating statement
    Statement* s;
    
    // the try of a TryFinally that encloses the loop
    EnclosingHandler* enclosinghandler;
    
    llvm::BasicBlock* breakTarget;
    llvm::BasicBlock* continueTarget;

    IRTargetScope();
    IRTargetScope(Statement* s, EnclosingHandler* enclosinghandler, llvm::BasicBlock* continueTarget, llvm::BasicBlock* breakTarget);
};

// represents a function
struct IrFunction : IrBase
{
    llvm::Function* func;
    llvm::Instruction* allocapoint;
    FuncDeclaration* decl;
    TypeFunction* type;

    bool queued;
    bool defined;
    
    llvm::Value* retArg; // return in ptr arg
    llvm::Value* thisArg; // class/struct 'this' arg
    llvm::Value* nestArg; // nested function 'this' arg
    
    llvm::Value* nestedVar; // nested var alloca
    
    llvm::Value* _arguments;
    llvm::Value* _argptr;
    
    llvm::DISubprogram diSubprogram;

    // pushes a unique label scope of the given name
    void pushUniqueLabelScope(const char* name);
    // pops a label scope
    void popLabelScope();

    // gets the string under which the label's BB
    // is stored in the labelToBB map.
    // essentially prefixes ident by the strings in labelScopes
    std::string getScopedLabelName(const char* ident);

    // label to basic block lookup
    typedef std::map<std::string, llvm::BasicBlock*> LabelToBBMap;
    LabelToBBMap labelToBB;

    // landing pads for try statements
    IRLandingPad landingPad;

    // loop blocks
    typedef std::vector<IRTargetScope> TargetScopeVec;
    TargetScopeVec targetScopes;

    // constructor
    IrFunction(FuncDeclaration* fd);

    // annotations
    void setNeverInline();
    void setAlwaysInline();

private:
    // prefix for labels and gotos
    // used for allowing labels to be emitted twice
    std::vector<std::string> labelScopes;

    // next unique id stack
    std::stack<int> nextUnique;
};

#endif
