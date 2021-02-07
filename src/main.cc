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
#include <tuple>
#include <list>

#include <json.hpp>

using json = nlohmann::json;

class LibraryCell {
    private:
        const std::string name;
        const unsigned int cell_length, cell_width;
        const std::vector<std::vector<int>> cell_blocks;

    public:
        LibraryCell() : name("FILL"), cell_length(1), cell_width(1), cell_blocks({{0}}) {

        }

        LibraryCell(std::string name, unsigned int cell_length, unsigned int cell_width, std::vector<std::vector<int>> cell_blocks) : 
            name(name), 
            cell_length(cell_length),
            cell_width(cell_width),
            cell_blocks(cell_blocks) {

        }

        const std::string get_lib_cell_name() {
            return name;
        }
        
        const unsigned int get_cell_length() {
            return cell_length;
        }

        const unsigned int get_cell_width() {
            return cell_width;
        }

        const std::vector<std::vector<int>>& get_cell_blocks() {
            return this->cell_blocks;
        }
};

class CellUnit;
class Net {
    private:
        int net_number;
        std::vector<CellUnit*> connected_cells;

    public:
        Net(int net_number) : net_number(net_number) {

        }

        void add_connection(CellUnit* new_cell) {
            connected_cells.push_back(new_cell);
        }

        int get_net_number() {
            return net_number;
        }

        std::vector<CellUnit*> get_connected_cells() {
            return connected_cells;
        }
};

class CellUnit {
    public:
        LibraryCell* lib_cell;
        std::string cell_name;
        std::unordered_map<std::string, std::vector<int>> pins;
        std::unordered_map<std::string, std::vector<Net*>> connected_nets;
        std::vector<Net*> flat_nets;
        std::string partition_id;
        bool fixed = 0;

        CellUnit() {

        }

        CellUnit(std::string cell_name, LibraryCell* lib_cell, auto connections) : cell_name(cell_name), lib_cell(lib_cell), pins(connections) { //Assumes that all z-values are equal length in a cell

        }

        CellUnit(std::string cell_name, LibraryCell* lib_cell, auto& connected_nets, auto& flat_nets) : cell_name(cell_name), lib_cell(lib_cell), connected_nets(connected_nets), flat_nets(flat_nets) { //Assumes that all z-values are equal length in a cell

        }
        
        std::string get_name() {
            return cell_name;
        }

        void print_attributes() {
            std::cout << "Cell Name: " << cell_name << std::endl;
            std::cout << "Library Cell Name: " << lib_cell->get_lib_cell_name() << std::endl;
            std::cout << "Pin Connections: " << std::endl;
            for (auto key : connected_nets) {
                std::cout << key.first << ", ";
                for (auto val : key.second) {
                    std::cout << val->get_net_number();
                }
                std::cout << std::endl;
            }
            std::cout << "Partition ID: " << partition_id << std::endl;
        }

        std::string get_cell_name() {
            return this->cell_name;
        }

        std::string get_lib_cell_name() {
            return this->lib_cell->get_lib_cell_name();
        }

        void print() {
            auto& cell_blocks = lib_cell->get_cell_blocks();
            for (int cell_z = 0; cell_z < lib_cell->get_cell_length(); cell_z++) {
                for (int cell_x = 0; cell_x < lib_cell->get_cell_width(); cell_x++) {
                    std::cout << std::setfill('0') << std::setw(4) << cell_blocks[cell_z][cell_x] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }

        void set_partition_id(std::string new_partition_id) {
            this->partition_id = new_partition_id;
        }


};


template <typename T>
class subvector {
    public:
        using iterator = typename T::iterator;
        using reference = typename std::iterator_traits<iterator>::reference;

        subvector(iterator first, iterator last)
            : first(first), last(last)
        { } 

        reference operator[](size_t idx)
        {   
            return *std::next(first, idx);
        } 

        iterator begin() const { return first; }  
        iterator end() const { return last; }

        size_t size() {
            return (size_t) (last-first);
        }
    private:
        iterator first, last;
};

struct Coordinate {
    int x, z;
    Coordinate(int x, int z) : x(x), z(z) {
    }
    Coordinate() {

    }

    //bool operator==(Coordinate compare) {
    //    return (compare.x == this->x && compare.z == this->z);
    //}

    void print() {
        std::cout << "Coordinate:\nX: " << this->x << ", Z: " << this->z << std::endl;
    }
};

class SubPlacement {
    private:
    public:
        std::vector<subvector<std::vector<CellUnit*>>> sub_placement;
        Coordinate coord_ll, coord_ur;
        int last_split_direction;
        std::string partition_id;

        //SubPlacement(std::vector<std::vector<CellUnit*>>& vec, Coordinate coord_ll, Coordinate coord_ur, int last_split_direction) : coord_ll(coord_ll), coord_ur(coord_ur), last_split_direction(last_split_direction) {
        //    sub_placement.resize((coord_ur.z - coord_ll.z), subvector<std::vector<CellUnit*>>(vec[0].begin()+coord_ll.x, vec[0].begin()+coord_ur.x));
        //    for (std::size_t i = 0; i < (coord_ur.z - coord_ll.z); i++) {
        //        sub_placement[i] = subvector<std::vector<CellUnit*>>(vec[(coord_ll.z+i)].begin()+coord_ll.x, vec[(coord_ll.z+i)].begin()+coord_ur.x);
        //    }
        //}

        SubPlacement(std::vector<std::vector<CellUnit*>>& vec, Coordinate coord_ll, Coordinate coord_ur, int last_split_direction, std::string partition_id) : coord_ll(coord_ll), coord_ur(coord_ur), last_split_direction(last_split_direction), partition_id(partition_id) {
            sub_placement.resize((coord_ur.z - coord_ll.z), subvector<std::vector<CellUnit*>>(vec[0].begin()+coord_ll.x, vec[0].begin()+coord_ur.x));
            for (int i = 0; i < (coord_ur.z - coord_ll.z); i++) {
                sub_placement[i] = subvector<std::vector<CellUnit*>>(vec[(coord_ll.z+i)].begin()+coord_ll.x, vec[(coord_ll.z+i)].begin()+coord_ur.x);
            }
            for (auto& row : sub_placement) {
                for (auto& cell : row) {
                    if (cell != nullptr) {
                        cell->set_partition_id(partition_id);
                    }
                }
            }
        }

        auto& operator[] (std::size_t index) {
            return sub_placement[index];
        }

        auto& get(Coordinate coord) {
            return sub_placement[coord.z][coord.x];
        }

        void print_sub_placement() {
            std::cout << std::endl << "LL ";
            coord_ll.print();
            std::cout << "UR ";
            coord_ur.print();
            std::cout << "Horizontal Size: ";
            std::cout << this->horizontal_size() << std::endl;
            std::cout << "Vertical Size: ";
            std::cout << this->vertical_size() << std::endl;
            std::cout << "Last split direction: " << this->last_split_direction << std::endl;
            for (auto& row : sub_placement) {
                for (auto& cell : row) {
                    if (cell != nullptr) {
                        std::cout << std::setw(4) << cell->get_lib_cell_name() << " ";
                    }
                }
                std::cout << std::endl;
            }
        }

        int size() {
            return sub_placement.size();
        }

        int horizontal_size() {
            return coord_ur.x - coord_ll.x;
        }

        int vertical_size() {
            return coord_ur.z - coord_ll.z;
        }

};

class CellPlacement {
    private:
        std::vector< std::vector<CellUnit*> > placement; //Output placed cells
        const std::vector<CellUnit*>& netlist; //Input Yosys-Synthesized Netlist
        int placement_x_max, placement_z_max;

        std::vector< std::vector<int> > flat_placement;

        struct CoordCost {
            Coordinate coord;
            int cost;
            CoordCost(Coordinate coord, int cost) : coord(coord), cost(cost){
            }
            CoordCost() {
            }
        };

    public:
        CellPlacement(auto& netlist) :
            netlist(netlist), 
            placement_x_max(int(ceil(sqrt(netlist.size())))), 
            placement_z_max(int(ceil(sqrt(netlist.size())))) {
                this->placement.resize(placement_z_max, std::vector<CellUnit*>(placement_x_max, nullptr));
        }

        CellPlacement(auto& netlist, unsigned int placement_x_max, unsigned int placement_z_max) :
            netlist(netlist), 
            placement_x_max(placement_x_max), 
            placement_z_max(placement_z_max) {
                this->placement.resize(placement_z_max, std::vector<CellUnit*>(placement_x_max, nullptr));
        }

        void place(LibraryCell* fill_cell) {
            initial_placement();
            add_fill(fill_cell);
            print_placement();
            refine_placement();
            print_placement();
            //render_flat_placement();
            //print_flat_placement();
        }

        unsigned int get_placement_max_cell_width() {
            int max_cell_width = 0;
            for (auto& z : placement) {
                for (auto& x : z) {
                    if (x != nullptr) {
                        int cell_width = x->lib_cell->get_cell_width();
                        if (cell_width > max_cell_width) 
                            max_cell_width = cell_width;
                    }
                }
            }
            return max_cell_width;
        }

        unsigned int get_placement_max_cell_length() {
            int max_cell_length = 0;
            int z = 0;
            for (auto& z : placement) {
                for (auto& x : z) {
                    if (x != nullptr) {
                        int cell_length = x->lib_cell->get_cell_length();
                        if (cell_length > max_cell_length) 
                            max_cell_length = cell_length;
                    }
                }
            }
            return max_cell_length;
        }

        int get_x_max() {
            return this->placement_x_max;
        }

        int get_z_max() {
            return this->placement_z_max;
        }

        void initial_placement() {
            int curr_x = 0;
            int curr_z = 0;
            for (auto cell : netlist) {
                placement[curr_z][curr_x] = cell;
                curr_x++;
                if (curr_x == placement_x_max) {
                    curr_x = 0;
                    curr_z++;
                    if (curr_z == placement_z_max) {
                        std::cout << "ERROR: Ran out of room in initial placement. Expand the allowed placement grid" << std::endl;
                        exit(1);
                        //if (cell != placement.end()) {
                        //    std::cout << "ERROR: Ran out of room in initial placement!" << std::endl;
                        //    exit(1);
                        //}
                    }
                }
            }
        }

        void add_fill(LibraryCell* fill_cell) {
            std::cout << "Adding fill cells to design" << std::endl;
            for (int z = 0; z < placement_z_max; z++) {
                for (int x = 0; x < placement_x_max; x++) {
                    if(placement[z][x] == nullptr) {
                        placement[z][x] = new CellUnit((std::string) "FILL" + std::to_string(x) + std::to_string(z), fill_cell, (std::unordered_map<std::string, std::vector<int>>){});
                    }
                }
            }
        }

        void swap(auto& cell1, auto& cell2) {
            auto temp = cell1;
            cell1 = cell2;
            cell2 = temp;
        }

        std::pair<SubPlacement*, SubPlacement*> split(SubPlacement* sub_placement) { 
            //std::cout << "Splitting Sub Placement" << std::endl;
            // direction: 0 = horizontal, 1 = vertical
            if (sub_placement->last_split_direction == 0) { //Last split horizontally
                if (sub_placement->horizontal_size() > 1) {
                    //Vertical Split
                    std::string left_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ur.z);
                    SubPlacement* left = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ur.z}, 1, left_partition_id);

                    std::string right_partition_id = std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
                    SubPlacement* right = new SubPlacement(placement, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 1, right_partition_id);

                    return (std::pair<SubPlacement*, SubPlacement*>) {left, right};
                }
                else if (sub_placement->vertical_size() > 1) {
                    //Resort to horizontal split
                    std::string low_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string((sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2);
                    SubPlacement* low = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,(sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2}, 0, low_partition_id);

                    std::string high_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string((sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
                    SubPlacement* high = new SubPlacement(placement, {sub_placement->coord_ll.x,(sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 0, high_partition_id);

                    return (std::pair<SubPlacement*, SubPlacement*>) {low, high};
                }
                else if (sub_placement->vertical_size() == 1 && sub_placement->vertical_size() == 1) {
                    //Return nothing
                    //Done partitioning
                    return (std::pair<SubPlacement*, SubPlacement*>) {nullptr, nullptr};
                }
                else {
                    std::cout << "ERROR: Something went wrong durring graph partitioning" << std::endl;
                    exit(1);
                }
            }
            else if (sub_placement->last_split_direction == 1) { //Last split vertically
                if (sub_placement->vertical_size() > 1) {
                    //Horizontal split
                    std::string low_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string((sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2);
                    SubPlacement* low = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,(sub_placement->coord_ll.z+sub_placement->coord_ur.z)/2}, 0, low_partition_id);

                    std::string high_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string((sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
                    SubPlacement* high = new SubPlacement(placement, {sub_placement->coord_ll.x,(sub_placement->coord_ll.z + sub_placement->coord_ur.z)/2}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 0, high_partition_id);

                    return (std::pair<SubPlacement*, SubPlacement*>) {low, high};
                }
                else if (sub_placement->vertical_size() > 1) {
                    //Resort to vertical split
                    std::string left_partition_id = std::to_string(sub_placement->coord_ll.x) + std::to_string(sub_placement->coord_ll.z) + std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ur.z);
                    SubPlacement* left = new SubPlacement(placement, {sub_placement->coord_ll.x,sub_placement->coord_ll.z}, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ur.z}, 1, left_partition_id);

                    std::string right_partition_id = std::to_string((sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2) + std::to_string(sub_placement->coord_ll.z) + std::to_string(sub_placement->coord_ur.x) + std::to_string(sub_placement->coord_ur.z);
                    SubPlacement* right = new SubPlacement(placement, {(sub_placement->coord_ll.x + sub_placement->coord_ur.x)/2,sub_placement->coord_ll.z}, {sub_placement->coord_ur.x,sub_placement->coord_ur.z}, 1, right_partition_id);

                    return (std::pair<SubPlacement*, SubPlacement*>) {left, right};
                }
                else if (sub_placement->vertical_size() == 1 && sub_placement->vertical_size() == 1) {
                    //Return nothing
                    //Done partitioning
                    return (std::pair<SubPlacement*, SubPlacement*>) {nullptr, nullptr};
                }
                else {
                    std::cout << "ERROR: Something went wrong durring graph partitioning" << std::endl;
                    exit(1);
                }
            }
            else {
                std::cout << "ERROR: Invalid last_split: " << sub_placement->last_split_direction << std::endl;
                exit(1);
            }
        }

        std::pair<SubPlacement*, SubPlacement*> run_kl(SubPlacement* in) {
            auto[part1, part2] = split(in); //Assign new partition IDs

            if (part1 != nullptr && part2 != nullptr) {
                //Perform KL Algorithm on two subregions
                std::string partition_id_1 = part1->partition_id;
                std::string partition_id_2 = part2->partition_id;
                std::unordered_map<std::string, std::list<CoordCost>> cell_costs, swap_candidates;
                
                //Calculate cost of initial cut
                for (int z = 0; z < in->vertical_size(); z++) {
                    for (int x = 0; x < in->horizontal_size(); x++) {
                        Coordinate coord(x,z);
                        CellUnit* cell = in->get(coord);
                        std::string this_partition_id = cell->partition_id;
                        std::string other_partition_id;

                        if (this_partition_id == partition_id_1) {
                            other_partition_id = partition_id_2;
                        }
                        else if (this_partition_id == partition_id_2) {
                            other_partition_id = partition_id_1;
                        }
                        else {
                            std::cout <<"ERROR: Partition ID not found" << std::endl;
                            exit(1);
                        }

                        int cut_cost = 0;
                        
                        for (auto& net : cell->flat_nets) {
                            for (auto& connected_cell : net->get_connected_cells()) {
                                if (connected_cell->partition_id == other_partition_id) {
                                    cut_cost++;
                                }
                                else if (connected_cell->partition_id == this_partition_id) { //FIXME: Is this right?
                                    cut_cost--;
                                }
                            }
                        }
                        cell_costs[this_partition_id].push_back({coord,cut_cost});
                        if (cut_cost > 0) {
                            swap_candidates[this_partition_id].push_back({coord,cut_cost});
                        }
                    }
                }

                //std::unordered_map<std::string, CoordCost*> max;
                std::unordered_map<std::string, CoordCost> max;

                for (auto& part : swap_candidates) { //Iterate Map
                    std::cout << "Swap candidates for ";
                    std::cout << part.first << std::endl;
                    for (auto& coordcost : part.second) { //Iterate List
                        if (max.find(part.first) == max.end()) {
                            //Part ID not found
                            max[part.first] = coordcost;
                        } 
                        else {
                            //Part ID found
                            if (coordcost.cost > max[part.first].cost) {
                                max[part.first] = coordcost;
                                std::cout << "New Max = " << coordcost.cost << std::endl;
                            }
                        }
                        std::cout << "Cell: ";
                        std::cout << in->get(coordcost.coord)->get_cell_name() << std::endl;
                        std::cout << "  Cut Cost: " << coordcost.cost << std::endl;
                    }
                }

                //int correction_factor = 0;

                //int delta_g = sp_a_max_cost.second + sp_b_max_cost.second - correction;
                //if (delta_g > 0) {
                //    std::cout << "Swapping " << (*sp_a)[sp_a_max_cost.first.z][sp_a_max_cost.first.x]->get_cell_name() << " with " << (*sp_b)[sp_b_max_cost.first.z][sp_b_max_cost.first.x]->get_cell_name() << std::endl;
                //    swap((*sp_a)[sp_a_max_cost.first.z][sp_a_max_cost.first.x], (*sp_b)[sp_b_max_cost.first.z][sp_b_max_cost.first.x]);
                //}
                return std::pair<SubPlacement*, SubPlacement*>(part1, part2);
            }
            else if (part1 == nullptr && part2 == nullptr) {
                return std::pair<SubPlacement*, SubPlacement*>(nullptr, nullptr);
            }
            else {
                std::cout << "ERROR: Only one partition valid when splitting subplacement" << std::endl;
                exit(1);
            }
        }

        void refine_placement() {
            std::cout << "Refining placement..." << std::endl;

            std::deque<SubPlacement*> sp_queue;
            std::deque<SubPlacement*> final_queue; //Remove later, only for testing

            SubPlacement* top = new SubPlacement(placement, {0,0}, {placement_x_max, placement_z_max}, 0, std::to_string(0));

            sp_queue.push_back(top); //Seed sub_placement queue

            //Queue handler
            while(!sp_queue.empty()) {
                SubPlacement* curr_sp = sp_queue.front();

                auto[part1, part2] = run_kl(curr_sp); //Run KL Logic

                if (part1 == nullptr && part2 == nullptr) {
                    final_queue.push_front(part1);
                    final_queue.push_front(part2);
                }
                else {
                    sp_queue.push_back(part1);
                    sp_queue.push_back(part2);
                }

                //Remove current subregion pointer
                sp_queue.pop_front();
                delete curr_sp;
                //exit(1);
            }

        }

        void print_placement() {
            std::cout << "Placement:" << std::endl;
            for (auto& row : placement) {
                for (auto& cell : row) {
                    if (cell != nullptr) {
                        std::cout << std::setw(4) << cell->get_lib_cell_name() << " ";
                    }
                }
                std::cout << std::endl;
            }
        }

        auto get_placement() {
            return placement;
        }

        void render_flat_placement() {
            unsigned int max_cell_length = get_placement_max_cell_length();
            unsigned int max_cell_width = get_placement_max_cell_width();
            unsigned int initial_block_value = 0;
            
            flat_placement.resize(placement_z_max * max_cell_length, std::vector<int>(placement_x_max * max_cell_width, initial_block_value));

            for (int grid_z = 0; grid_z < placement_z_max; grid_z++) {
                for (int grid_x = 0; grid_x < placement_x_max; grid_x++) {
                    CellUnit* this_cell = placement[grid_z][grid_x];
                    if (this_cell != nullptr) { //Cell found
                        auto& cell_blocks = this_cell->lib_cell->get_cell_blocks();
                        for (int cell_z = 0; cell_z < max_cell_length; cell_z++) {
                            for (int cell_x = 0; cell_x < max_cell_width; cell_x++) {
                                if (cell_x < this_cell->lib_cell->get_cell_width() && cell_z < this_cell->lib_cell->get_cell_length()) { //Must make sure that iterators are within cell bounds
                                    //flat_map[grid_z * cell_map.all_cells_max_length + cell_z][grid_x * cell_map.all_cells_max_width + cell_x] = this_cell->cell_blocks[cell_z][cell_x];
                                    flat_placement[grid_z * max_cell_length + cell_z][grid_x * max_cell_width + cell_x] = cell_blocks[cell_z][cell_x];
                                } 
                                else { //Otherwise we will place a blank block here
                                    flat_placement[grid_z * max_cell_length + cell_z][grid_x * max_cell_width + cell_x] = initial_block_value;
                                }
                            }
                        }
                    }
                    else { //No cell found, must fill in entire cell with filler
                        for (int cell_z = 0; cell_z < max_cell_length; cell_z++) {
                            for (int cell_x = 0; cell_x < max_cell_width; cell_x++) {
                                flat_placement[grid_z * max_cell_length + cell_z][grid_x * max_cell_width + cell_x] = initial_block_value;
                            }
                        }
                    }
                }
            }
        }

        void print_flat_placement() {
            std::cout << "Size: " << flat_placement.size() << std::endl;
            std::cout << "Size (inner): " << flat_placement[0].size() << std::endl;
            std::cout << " X: ";
            for (int x = 0; x < flat_placement[0].size(); x++) {
                std::cout << std::setfill('0') << std::setw(4) << x << " ";
            }
            std::cout << "\n";
            for (int z = flat_placement.size() - 1; z >= 0; z--) { //Iterate backwards for physical layout
                std::vector<int> row = flat_placement[z];
                std::cout << std::setfill('0') << std::setw(2) << z << ": ";
                for (int x = 0; x < flat_placement[z].size(); x++) {
                    std::cout << std::setfill('0') << std::setw(4) << row[x] << " ";
                }
                std::cout << std::endl;
            }
        }
};

class CellMap {
    //CellMap maps a library cell name to a block layout specified by a given JSON file
    private:
    public:
        std::map< std::string, LibraryCell* > cell_map;
        int all_cells_max_width = 0; //Helper variable for flattening grid to block map
        int all_cells_max_length = 0; //Helper variable for flattening grid to block map

        CellMap() {
        }

        //Allows the user to access a cell layout with squre brackets. E.g. cell_map["NAND"]
        LibraryCell* operator[] (std::string cell_key) { //Return copy so that we can't overwrite a std cell entry
            LibraryCell* ret = nullptr;
            auto it = cell_map.find(cell_key);
            if ( it != cell_map.end() ) {
                // Found cell_key
                ret = it->second;
            } else {
                // Not found
                std::cout << "ERROR: Standard Cell entry \"" << cell_key << "\" not found in provided library file." << std::endl;
                exit(1);
            }
            return ret;
        }

        void load_library(std::string library_json_file) {
            // Read cell block layouts from given JSON
            std::ifstream f_lib_json(library_json_file);

            json lib;
            f_lib_json >> lib;

            for (auto& it : lib["cells"].items()) {
                std::string cell_name = it.key();
                json j = json::parse(it.value().dump());
                std::vector<std::vector<int>> single_cell_map = j.at("layout").get<std::vector<std::vector<int>>>();

                //Check dimensions
                int cell_max_length = single_cell_map.size();
                if (cell_max_length > this->all_cells_max_length) {
                    this->all_cells_max_length = cell_max_length;
                }

                //Get cell max width, check for uniformity
                int cell_max_width = 0;
                for (std::vector<int>& z_val : single_cell_map) {
                    cell_max_width = z_val.size() > cell_max_width ? z_val.size() : cell_max_width;
                    if (cell_max_width > all_cells_max_width) {
                        this->all_cells_max_width = cell_max_width;
                    }
                }

                //Resize for max_cell_width
                for (std::vector<int>& z_val : single_cell_map) {
                    z_val.resize(cell_max_width, 0);
                }

                std::reverse(single_cell_map.begin(), single_cell_map.end());
                this->cell_map[cell_name] = new LibraryCell(cell_name, cell_max_length, cell_max_width, single_cell_map);
            }
        }

};

class CellGraph {
    private:
        json yosys_netlist;

        std::vector<CellUnit*> internal_netlist;
        
    public:
        CellGraph() {
        }

        void parse_netlist(std::string netlist_json_file, CellMap& cell_map) {
            // Read Yosys Netlist from JSON
            std::ifstream netlist_json(netlist_json_file);
            netlist_json >> this->yosys_netlist;

            std::vector<std::string> module_names;
            for (auto& it : yosys_netlist["modules"].items()) {
                std::cout << it.key() << std::endl;
                module_names.push_back(it.key());
            }

            std::vector<Net*> nets;

            for (auto& module : module_names) {
                for (auto it : yosys_netlist["modules"][module]["cells"].items()) {
                    auto cell_json = json::parse((std::string)it.value().dump());

                    //Create and connect net to the cell
                    auto connections = cell_json.at("connections").get<std::unordered_map<std::string, std::vector<int>>>();
                    std::unordered_map<std::string, std::vector<Net*>> pin_map;
                    std::vector<Net*> flat_nets;
                    for (auto& conn : connections) {
                        std::vector<Net*> temp_nets;
                        for (auto net_num : conn.second) {
                            bool found = false;
                            for (auto& net : nets) {
                                if (net_num == net->get_net_number()) {
                                    found = true;
                                    temp_nets.push_back(net);
                                }
                            }
                            if (!found) {
                                Net* new_net = new Net(net_num);
                                temp_nets.push_back(new_net);
                                nets.push_back(new_net);
                            }
                        }
                        pin_map[conn.first] = temp_nets;
                        flat_nets.insert(flat_nets.end(), temp_nets.begin(), temp_nets.end());
                    }

                    CellUnit* cell = new CellUnit(
                        it.key(), 
                        cell_map[cell_json.at("type").get<std::string>()], 
                        pin_map,
                        flat_nets
                    );

                    //Connect cell to all connected nets
                    for (auto& net : flat_nets) {
                        net->add_connection(cell);
                    }

                    internal_netlist.push_back(cell);
                }
            }

            //TODO: Parse ports as well

        }

        int generate_graph() {
            return 0;
        }

        std::vector<CellUnit*>& get_netlist() {
            return internal_netlist;
        }

        void print_netlist() {
            for (auto& it : internal_netlist) {
                it->print_attributes();
            }
        }

        void print_graph() {
            std::cout << this->yosys_netlist["modules"]["booth"]["cells"] << std::endl;
        }

};

int main () {
    CellMap cell_map;
    cell_map.load_library("lib/mc/test_lib.json");

    CellGraph* cell_graph = new CellGraph();
    cell_graph->parse_netlist((std::string) "/mnt/c/git/MC-APR/test.json", cell_map);

    CellPlacement* cell_placement = new CellPlacement(cell_graph->get_netlist());
    cell_placement->place(cell_map["FILL"]);
    //cell_graph->print_netlist();

    return 0;
}
