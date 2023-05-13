#ifndef SEAT_H
#define SEAT_H


#include <fstream>
#include <iostream>

#include <glm/glm.hpp>


class Seat {
public:
    Seat(std::string name, glm::vec3 vec3);
    Seat(std::string name, float x, float y, float z);
    ~Seat();

    void AddSeatCoordinate();


private:
    glm::vec3 position_;
    std::string name_;
};

#endif // SEAT_H
