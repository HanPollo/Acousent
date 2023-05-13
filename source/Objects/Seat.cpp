#include "Seat.h"

Seat::Seat(std::string name, glm::vec3 vec3)
{
	position_ = vec3;
	name_ = name;
}

Seat::Seat(std::string name, float x, float y, float z)
{
	position_ = glm::vec3(x, y, z);
	name_ = name;
}

Seat::~Seat()
{
}

void Seat::AddSeatCoordinate()
{
	std::ofstream out;

	// std::ios::app is the open mode "append" meaning
	// new data will be written to the end of the file.
	out.open("SeatCoords.txt", std::ios::app);

	out << name_ << ": " << position_[0] << ", " << position_[1] << ", " << position_[2] << std::endl;
	out.close();
	std::cout << "Wrote" << std::endl;
}
