#include <string>

#include "CellMap.h"
#include "CellGraph.h"
#include "CellPlacement.h"

int main () {
    CellMap cell_map;
    //cell_map.load_library("lib/mc/test_lib.json");
    cell_map.load_library("lib/mc/debug.json");

    CellGraph* cell_graph = new CellGraph();
    cell_graph->parse_netlist((std::string) "small_test.json", cell_map);
    cell_graph->print_netlist();

    CellPlacement* cell_placement = new CellPlacement(cell_graph->get_netlist(), 4, 2);
    cell_placement->place(cell_map["FILL"]);
    //cell_graph->print_netlist();

    return 0;
}
