#pragma once

#include "../memory/arena.hpp"
#include "registers.hpp"
#include "lcache.hpp"

struct HoneyCell {

    uint16_t id;

    RegisterFile registers;

    LCache cache;

    Arena arena;

};
