#pragma once

#include "../jarbes_kernel/core/metatron_graph.hpp"
#include <string>

namespace sysp {

class MetatronBuilder {

public:

    MetatronGraph build_from_ast(const std::string &file);

};

}
