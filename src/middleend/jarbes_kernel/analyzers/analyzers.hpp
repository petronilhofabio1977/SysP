#pragma once

#include "../core/metatron_graph.hpp"

bool check_use_before_production(const MetatronGraph &graph);
bool detect_cycle(const MetatronGraph &graph);
bool detect_data_race(const MetatronGraph &graph);
