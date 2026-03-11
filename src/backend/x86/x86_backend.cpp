#include "x86_backend.hpp"

#include <iostream>

namespace sysp::backend::x86 {

using sysp::metatron::Node;
using sysp::metatron::Graph;

Backend::Backend()
{}

void Backend::generate(const Graph& graph, std::ostream& out)
{
    out << "section .text\n";
    out << "global _start\n";
    out << "_start:\n";

    for (auto& node_ptr : graph.nodes()) {

        Node* node = node_ptr.get();

        switch (node->opcode) {

            case sysp::ir::Opcode::Add:
                out << "    add rax, rbx\n";
                break;

            case sysp::ir::Opcode::Sub:
                out << "    sub rax, rbx\n";
                break;

            case sysp::ir::Opcode::Mul:
                out << "    imul rax, rbx\n";
                break;

            case sysp::ir::Opcode::Div:
                out << "    idiv rbx\n";
                break;

            case sysp::ir::Opcode::ConstInt:
                out << "    mov rax, 1\n";
                break;

            default:
                out << "    nop\n";
                break;
        }
    }

    out << "    mov rax, 60\n";
    out << "    xor rdi, rdi\n";
    out << "    syscall\n";
}

}
