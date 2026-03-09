#pragma once

#include <vector>
#include <memory>
#include <string>

struct MetatronNode {

    int id;
    std::string label;

    std::vector<MetatronNode*> inputs;
    std::vector<MetatronNode*> outputs;

};

class MetatronGraph {

public:

    MetatronGraph();

    MetatronNode* create_node(const std::string& label);

    void connect(MetatronNode* a, MetatronNode* b);

    const std::vector<std::unique_ptr<MetatronNode>>& nodes() const;

private:

    int next_id = 0;

    std::vector<std::unique_ptr<MetatronNode>> node_list;

};
