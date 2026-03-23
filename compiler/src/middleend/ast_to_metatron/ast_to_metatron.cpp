#include "ast_to_metatron.hpp"

#include "../ir/opcode.hpp"

namespace sysp::middleend {

static int node_counter = 0;

ASTToMetatron::ASTToMetatron()
{
}

sysp::metatron::Node* ASTToMetatron::convert_stmt(const sysp::ast::Stmt* stmt)
{
    if (auto s = dynamic_cast<const sysp::ast::IfStmt*>(stmt))
        return convert_if(s);

    if (auto s = dynamic_cast<const sysp::ast::WhileStmt*>(stmt))
        return convert_while(s);

    if (auto s = dynamic_cast<const sysp::ast::ReturnStmt*>(stmt))
        return convert_return(s);

    if (auto s = dynamic_cast<const sysp::ast::BreakStmt*>(stmt))
        return convert_break(s);

    if (auto s = dynamic_cast<const sysp::ast::ContinueStmt*>(stmt))
        return convert_continue(s);

    return nullptr;
}

sysp::metatron::Node* ASTToMetatron::convert_if(const sysp::ast::IfStmt*)
{
    return new sysp::metatron::Node(
        node_counter++,
        sysp::ir::Opcode::Branch
    );
}

sysp::metatron::Node* ASTToMetatron::convert_while(const sysp::ast::WhileStmt*)
{
    return new sysp::metatron::Node(
        node_counter++,
        sysp::ir::Opcode::Branch
    );
}

sysp::metatron::Node* ASTToMetatron::convert_return(const sysp::ast::ReturnStmt*)
{
    return new sysp::metatron::Node(
        node_counter++,
        sysp::ir::Opcode::Return
    );
}

sysp::metatron::Node* ASTToMetatron::convert_break(const sysp::ast::BreakStmt*)
{
    return new sysp::metatron::Node(
        node_counter++,
        sysp::ir::Opcode::Jump
    );
}

sysp::metatron::Node* ASTToMetatron::convert_continue(const sysp::ast::ContinueStmt*)
{
    return new sysp::metatron::Node(
        node_counter++,
        sysp::ir::Opcode::Jump
    );
}

}
