#ifndef INSTRUMENT_H
#define INSTRUMENT_H
#include "Object.h"


class Instrument : public Object {
    enum Type {
        MONITOR,
        GROUNDED,
        HANGED
    };
public:
    Instrument();
    Instrument(Type type);

    void Update() override;

    glm::vec3 sound_direction;
    Type type = GROUNDED;
    

};

#endif // INSTRUMENT_H
