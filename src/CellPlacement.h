#ifndef CELLPLACEMENT_H
#define CELLPLACEMENT_H

#include <vector>

#include "CellUnit.h"
#include "Coordinate.h"
#include "SubPlacement.h"

class CellPlacement {
    private:

        std::vector< std::vector<CellUnit*> > placement; //Output placed cells

        const std::vector<CellUnit*>& netlist; //Input Yosys-Synthesized Netlist

        int placement_x_max, placement_z_max;

        std::vector< std::vector<int> > flat_placement;

        struct CellCost {
            CellUnit* cell;
            int cost;
            CellCost(CellUnit* cell, int cost) : cell(cell), cost(cost){
            }
            CellCost() {
            }
        };
        struct CoordCost {
            Coordinate coord;
            int cost;
            CoordCost(Coordinate coord, int cost) : coord(coord), cost(cost){
            }
            CoordCost() {
            }
        };

    public:

        CellPlacement(std::vector<CellUnit*>& netlist);

        CellPlacement(std::vector<CellUnit*>& netlist, unsigned int placement_x_max, unsigned int placement_z_max);

        void place(LibraryCell* fill_cell);

        unsigned int get_placement_max_cell_width();

        unsigned int get_placement_max_cell_length();

        int get_x_max();

        int get_z_max();

        void initial_placement();

        void add_fill(LibraryCell* fill_cell);

        void swap(auto& cell1, auto& cell2);

        std::pair<SubPlacement*, SubPlacement*> split(SubPlacement* sub_placement);

        std::pair<SubPlacement*, SubPlacement*> run_kl(SubPlacement* in);

        void refine_placement();

        void print_placement();

        auto get_placement();

        void render_flat_placement();

        void print_flat_placement();
};

#endif
