#pragma once

#include "../../frontend/ast/stmt.hpp"
#include "../metatron_graph/metatron_graph.hpp"

namespace sysp::middleend {

class ASTToMetatron {

public:

    ASTToMetatron();

    sysp::metatron::Node* convert_stmt(const sysp::ast::Stmt* stmt);

private:

    sysp::metatron::Node* convert_if(const sysp::ast::IfStmt* stmt);
    sysp::metatron::Node* convert_while(const sysp::ast::WhileStmt* stmt);
    sysp::metatron::Node* convert_return(const sysp::ast::ReturnStmt* stmt);
    sysp::metatron::Node* convert_break(const sysp::ast::BreakStmt* stmt);
    sysp::metatron::Node* convert_continue(const sysp::ast::ContinueStmt* stmt);

};

}
