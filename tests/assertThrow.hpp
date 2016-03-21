#pragma once

#define assert_throw(code, exception){\
    bool exceptionThrown = false;\
    try\
    {\
        code;\
    }\
    catch(const exception &e) \
    {\
        exceptionThrown = true;\
    }\
    assert(exceptionThrown);\
}
