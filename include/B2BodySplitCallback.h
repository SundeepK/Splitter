#ifndef ONB2BODYSPLIT_H
#define ONB2BODYSPLIT_H

#include <Box2D.h>
#include "B2BoxBuilder.h"

class B2BodySplitCallback
{
    public:
        B2BodySplitCallback();
        virtual ~B2BodySplitCallback();
        virtual void onb2BodySplit(std::vector<B2BoxBuilder> splitBodies, b2Body* body) = 0;
    protected:
    private:
};

#endif // ONB2BODYSPLIT_H
