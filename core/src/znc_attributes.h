#pragma once

#ifndef __CONSTRUCTOR
    #define __CONSTRUCTOR __attribute__((constructor))
#endif

#ifndef __DESTRUCTOR
    #define __DESTRUCTOR __attribute__((destructor))
#endif

#ifndef __CONSTRUCTOR_PRIO
    #define __CONSTRUCTOR_PRIO(p) __attribute__((constructor(p)))
#endif