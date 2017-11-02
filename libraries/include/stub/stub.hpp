/*
    author: pli
    date: 2017.10.17
    Stub class for test.
*/

#ifndef _STUB_H_
#define _STUB_H_

#include <memory>

class Stub {
public:
    Stub();
    virtual ~Stub();

    void start();
};
typedef std::shared_ptr<Stub> StubPtr;

#endif
