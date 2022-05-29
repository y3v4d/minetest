#include "direction.h"

const vec3i _dir_to_vec3i[6] = {
    {  0,  0, -1  },
    {  0,  0,  1  },
    {  1,  0,  0  },
    { -1,  0,  0  },
    {  0,  1,  0  },
    {  0, -1,  0  }
};

const char* _dir_to_str[6] = {
    "FRONT",
    "BACK",
    "RIGHT",
    "LEFT",
    "TOP",
    "BOTTOM"
};