#ifndef _USRCONFIG_H_
#define _USRCONFIG_H_

// this file is used to override macros in btconfig.h
// to override a macro, undef and redefine it to new value like this:

#undef DEBUG_BCSP
#define DEBUG_BCSP 1

#undef DEBUG_HCI
#define DEBUG_HCI 1

#endif // _USRCONFIG_H_
