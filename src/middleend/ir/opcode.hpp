#pragma once
#include <string>

namespace sysp::ir {

enum class Opcode {
    Nop,

    // ── Constants ────────────────────────────────────────────────
    ConstInt,
    ConstFloat,
    ConstStr,
    ConstBool,

    // ── Memory ───────────────────────────────────────────────────
    Alloc,      // allocate variable slot
    Load,       // load from variable
    Store,      // store to variable
    LoadField,  // struct field load
    StoreField, // struct field store
    LoadIndex,  // array index load
    StoreIndex, // array index store

    // ── Arithmetic ───────────────────────────────────────────────
    Add, Sub, Mul, Div, Mod,

    // ── Bitwise ──────────────────────────────────────────────────
    BitAnd, BitOr, BitXor, BitNot, Shl, Shr,

    // ── Comparisons ──────────────────────────────────────────────
    CmpEQ, CmpNE, CmpLT, CmpLE, CmpGT, CmpGE,

    // ── Logical ──────────────────────────────────────────────────
    And, Or, Not,

    // ── Control flow ─────────────────────────────────────────────
    Jump,           // unconditional jump
    Branch,         // conditional branch
    Return,         // function return
    Unreachable,    // dead code marker

    // ── Functions ────────────────────────────────────────────────
    Call,
    CallMethod,
    Phi,            // SSA phi node — merges values from different paths

    // ── Memory safety ────────────────────────────────────────────
    Move,           // ownership transfer
    Drop,           // explicit deallocation
    RegionEnter,    // enter a region scope
    RegionExit,     // exit a region scope

    // ── Concurrency ──────────────────────────────────────────────
    Spawn,
    Send,
    Recv,
    Await,

    // ── Type operations ──────────────────────────────────────────
    Cast,
    SizeOf,
};

inline std::string to_string(Opcode op) {
    switch (op) {
    case Opcode::Nop:          return "nop";
    case Opcode::ConstInt:     return "const_int";
    case Opcode::ConstFloat:   return "const_float";
    case Opcode::ConstStr:     return "const_str";
    case Opcode::ConstBool:    return "const_bool";
    case Opcode::Alloc:        return "alloc";
    case Opcode::Load:         return "load";
    case Opcode::Store:        return "store";
    case Opcode::LoadField:    return "load_field";
    case Opcode::StoreField:   return "store_field";
    case Opcode::LoadIndex:    return "load_index";
    case Opcode::StoreIndex:   return "store_index";
    case Opcode::Add:          return "add";
    case Opcode::Sub:          return "sub";
    case Opcode::Mul:          return "mul";
    case Opcode::Div:          return "div";
    case Opcode::Mod:          return "mod";
    case Opcode::BitAnd:       return "bit_and";
    case Opcode::BitOr:        return "bit_or";
    case Opcode::BitXor:       return "bit_xor";
    case Opcode::BitNot:       return "bit_not";
    case Opcode::Shl:          return "shl";
    case Opcode::Shr:          return "shr";
    case Opcode::CmpEQ:        return "cmpeq";
    case Opcode::CmpNE:        return "cmpne";
    case Opcode::CmpLT:        return "cmplt";
    case Opcode::CmpLE:        return "cmple";
    case Opcode::CmpGT:        return "cmpgt";
    case Opcode::CmpGE:        return "cmpge";
    case Opcode::And:          return "and";
    case Opcode::Or:           return "or";
    case Opcode::Not:          return "not";
    case Opcode::Jump:         return "jump";
    case Opcode::Branch:       return "branch";
    case Opcode::Return:       return "return";
    case Opcode::Unreachable:  return "unreachable";
    case Opcode::Call:         return "call";
    case Opcode::CallMethod:   return "call_method";
    case Opcode::Phi:          return "phi";
    case Opcode::Move:         return "move";
    case Opcode::Drop:         return "drop";
    case Opcode::RegionEnter:  return "region_enter";
    case Opcode::RegionExit:   return "region_exit";
    case Opcode::Spawn:        return "spawn";
    case Opcode::Send:         return "send";
    case Opcode::Recv:         return "recv";
    case Opcode::Await:        return "await";
    case Opcode::Cast:         return "cast";
    case Opcode::SizeOf:       return "sizeof";
    default:                   return "unknown";
    }
}

} // namespace sysp::ir
