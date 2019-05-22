#ifndef IDGENERATOR_I_H
#define IDGENERATOR_I_H

#include "idgenerator.h"

class IdGeneratorI : public record::idGenerator
{
public:

    virtual long getNextId(const Ice::Current&);
    virtual record::recordIds getNextIds(int idNum, const Ice::Current&);
};

#endif
