#ifndef SPEAKER_H
#define SPEAKER_H
#include "Object.h"


class Speaker : public Object {
    enum Type {
        MONITOR,
        GROUNDED,
        HANGED
    };
public:
    Speaker();
    Speaker(Type type);
    ~Speaker();

    void Update() override;

    glm::vec3 sound_direction;
    Type type = GROUNDED;
    

};

#endif // SPEAKER_H
