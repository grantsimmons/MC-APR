#include "Net.h"

Net::Net(int net_number) : net_number(net_number) {

}

void Net::add_connection(CellUnit* new_cell) {
    connected_cells.push_back(new_cell);
}

int Net::get_net_number() {
    return net_number;
}

std::vector<CellUnit*> Net::get_connected_cells() {
    return connected_cells;
}
