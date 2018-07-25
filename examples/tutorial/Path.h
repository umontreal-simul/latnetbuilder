#ifndef EXAMPLES__PATH_H
#define EXAMPLES__PATH_H

#include "netbuilder/Path.h"
void SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES()
{
    #ifdef CHECKING_EXAMPLES // if the executable will be called in the build directory
        NetBuilder::SET_PATH_TO_LATNETBUILDER_DIR("../../progs/bin");
    #else // if the executable will be called in the share/doc/latnetbuilder/examples/tutorial directory
        NetBuilder::SET_PATH_TO_LATNETBUILDER_DIR("../../../../../bin");
    #endif
}

#endif