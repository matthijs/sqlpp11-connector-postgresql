#ifndef ASSERT_THROW_H
#define ASSERT_THROW_H

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

#endif
