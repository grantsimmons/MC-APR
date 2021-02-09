#ifndef CELLGRAPH_H
#define CELLGRAPH_H

#include <string>
#include <vector>

#include <json.hpp>

#include "CellMap.h"
#include "CellUnit.h"

class CellGraph {
    public:

		using json = nlohmann::json;

        CellGraph();

        void parse_netlist(std::string netlist_json_file, CellMap& cell_map);

        int generate_graph();

        std::vector<CellUnit*>& get_netlist();

        void print_netlist();

        void print_graph();

    private:

        json yosys_netlist;

        std::vector<CellUnit*> internal_netlist;
        
};

#endif
