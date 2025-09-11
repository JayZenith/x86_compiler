#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"


class LLVMGenerator {
public:
    LLVMGenerator(const std::vector<std::unique_ptr<Node>>& prog,
              llvm::LLVMContext& ctx,
              llvm::Module* mod,
              llvm::IRBuilder<>& builder)
        : m_prog(prog), context(ctx), module(mod), builder(builder) {}

    void codegen_program() {
        auto intTy = llvm::Type::getInt32Ty(context);
        auto funcType = llvm::FunctionType::get(intTy, false);
        auto mainFunc = llvm::Function::Create(funcType,
                                               llvm::Function::ExternalLinkage,
                                               "main",
                                               module);
        auto entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
        builder.SetInsertPoint(entry);

        // Walk AST nodes
        for (const auto& stmt : m_prog) {
            gen_stmt(stmt.get());
        }

        builder.CreateRet(llvm::ConstantInt::get(intTy, 0));
    }

private:
    const std::vector<std::unique_ptr<Node>>& m_prog;
    llvm::LLVMContext& context;
    llvm::Module* module;
    llvm::IRBuilder<>& builder;

    std::unordered_map<std::string, llvm::Value*> m_vars;

    void gen_stmt(Node* node) {
        switch (node->type) {
            case NodeType::Let: {
                auto let_node = static_cast<NodeLet*>(node);
                llvm::Value* val = gen_expr(let_node->expr.get());
                // allocate stack variable
                llvm::AllocaInst* var = builder.CreateAlloca(builder.getInt32Ty(), nullptr, let_node->name);
                builder.CreateStore(val, var);
                m_vars[let_node->name] = var;
                break;
            }
            case NodeType::Exit: {
                auto exit_node = static_cast<NodeExit*>(node);
                llvm::Value* val = gen_expr(exit_node->expr.get());
                // exit code: just return the value from main
                builder.CreateRet(val);
                break;
            }
            default: break;
        }
    }

    llvm::Value* gen_expr(Node* node) {
        switch (node->type) {
            case NodeType::IntLit: {
                auto lit = static_cast<NodeIntLit*>(node);
                return llvm::ConstantInt::get(builder.getInt32Ty(), lit->value);
            }
            case NodeType::Ident: {
                auto ident = static_cast<NodeIdent*>(node);
                llvm::Value* ptr = m_vars[ident->name];
                llvm::PointerType* ptrType = llvm::cast<llvm::PointerType>(ptr->getType());
                llvm::Type* elemType = ptrType->getNonOpaquePointerElementType();
                return builder.CreateLoad(elemType, ptr, ident->name + "_val");
            }

            case NodeType::BinExpr: {
                auto bin = static_cast<NodeBinExpr*>(node);
                llvm::Value* lhs = gen_expr(bin->lhs.get());
                llvm::Value* rhs = gen_expr(bin->rhs.get());
                if (bin->op == "+") return builder.CreateAdd(lhs, rhs, "addtmp");
                if (bin->op == "-") return builder.CreateSub(lhs, rhs, "subtmp");
                if (bin->op == "*") return builder.CreateMul(lhs, rhs, "multmp");
                if (bin->op == "/") return builder.CreateSDiv(lhs, rhs, "divtmp");
            }
            default: return nullptr;
        }
    }
};
