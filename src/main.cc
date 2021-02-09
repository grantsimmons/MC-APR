#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <math.h>
#include <deque>
#include <list>

#include <json.hpp>

#include <string>

#include "CellMap.h"
#include "CellGraph.h"
#include "CellPlacement.h"
#include "CellUnit.h"

int main () {
    CellMap cell_map;
    cell_map.load_library("lib/mc/test_lib.json");

    CellGraph* cell_graph = new CellGraph();
    cell_graph->parse_netlist((std::string) "test.json", cell_map);

    CellPlacement* cell_placement = new CellPlacement(cell_graph->get_netlist());
    cell_placement->place(cell_map["FILL"]);
    //cell_graph->print_netlist();

    return 0;
}
