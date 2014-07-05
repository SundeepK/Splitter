#ifndef BODYLISTENER_H
#define BODYLISTENER_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "B2DWorld.h"
#include "B2BoxBuilder.h"
#include <functional>
#include <iostream>
#include "ActionController.h"
#include <string>
#include <math.h>
#include <SFML/OpenGL.hpp>
#include <GL/glut.h>


class BodyListener : public B2BodySplitCallback
{
    public:
        BodyListener(B2DWorld& world);
        virtual ~BodyListener();
        void onb2BodySplit(std::vector<B2BoxBuilder>& splitBodies, b2Body* body);
    protected:
    private:
    B2DWorld box2DWorld;
};

#endif // BODYLISTENER_H
