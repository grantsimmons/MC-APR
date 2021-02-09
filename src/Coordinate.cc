#include "Coordinate.h"

#include <iostream>

Coordinate::Coordinate(int x, int z) : x(x), z(z) {

}

Coordinate::Coordinate() {

}

//bool Coordinate::operator==(Coordinate compare) {
//    return (compare.x == this->x && compare.z == this->z);
//}

void Coordinate::print() {
	std::cout << "Coordinate:\nX: " << this->x << ", Z: " << this->z << std::endl;
}
