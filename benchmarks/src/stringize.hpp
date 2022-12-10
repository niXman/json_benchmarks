
#pragma once

#define __STRINGIZE_IMPL(x) #x
#define __STRINGIZE(x) __STRINGIZE_IMPL(x)
#define __STRINGIZE_VERSION(major, minor, bugfix) \
        __STRINGIZE(major) \
    "." __STRINGIZE(minor) \
    "." __STRINGIZE(bugfix)
