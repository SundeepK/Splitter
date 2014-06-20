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

void drawCircle(sf::RenderWindow& App, sf::Vector2f pos, sf::Color color)
{
    sf::CircleShape entryCircle(4);
    entryCircle.setFillColor(color);
    entryCircle.setPosition(sf::Vector2f(pos.x-entryCircle.getRadius(), pos.y-entryCircle.getRadius()));
    App.draw(entryCircle);

}

int isCCW(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3)
{
    return p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
}

B2BoxBuilder getBox2dBuilder(std::vector<b2Vec2> points, b2Body* body)
{
    B2BoxBuilder builder(points, body);
    return builder;
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
//    settings.majorVersion = 3;
//    settings.minorVersion = 0;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow App(sf::VideoMode(1280, 800,32), "Splitter test",sf::Style::Default, settings);
    App.setKeyRepeatEnabled(true);
    App.setFramerateLimit(60);

    sf::VertexArray sliceLine(sf::Lines, 2);
    bool isleftPressed = false;

    B2DWorld box2DWorld(9.8f);
    SFMLB2dDebugDraw drawer(App);
    box2DWorld.setDebugDraw(drawer);
    drawer.SetFlags(b2Draw::e_shapeBit);

    //box
    B2BoxBuilder builder(50,50);
    builder
    .bodyType(b2_dynamicBody)
    .setPosition(b2Vec2(50,10))
    .setDensity(1.0f);
    b2Body* b = box2DWorld.createB2Body(&builder);
    b->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);
    b->ApplyTorque(500,true);

    std::vector<b2Vec2> sides;

    sides.push_back (b2Vec2(-1.5,-0.5));
    sides.push_back (b2Vec2(0.5,-1));
    sides.push_back(b2Vec2(1,-0.5));
    sides.push_back(b2Vec2(1,1.5));
    sides.push_back(b2Vec2(0.5,1.5));
    sides.push_back(b2Vec2(-0.5,1.3));


    B2BoxBuilder eightSides(sides ,b);
    eightSides.setPosition(b2Vec2(100,100));
    b2Body* b3 = box2DWorld.createB2Body(&eightSides);


    B2BoxBuilder secondndbox(50,50);
    secondndbox
    .bodyType(b2_dynamicBody)
    .setPosition(b2Vec2(120,10))
    .setDensity(1.0f);
    b2Body* b2 = box2DWorld.createB2Body(&secondndbox);
    b2->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);

    //ground
    B2BoxBuilder groundShapebuilder(1200, 50);
    groundShapebuilder
    .bodyType(b2_staticBody)
    .setPosition(b2Vec2(0,700))
    .setDensity(1.0f);
    box2DWorld.createB2Body(&groundShapebuilder);

    sf::Clock deltaClock;

    box2DWorld.registerBodySplitCallback([&box2DWorld](std::vector<B2BoxBuilder> splitBodies, b2Body* body) -> void
    {

        for(auto builder : splitBodies)
        {
            box2DWorld.createB2Body(&builder);
        }
        box2DWorld.deleteBody(body);
    });

    while (App.isOpen())
    {

        sf::Event event;
        while( App.pollEvent(event))
        {

            if(isleftPressed)
            {
                sliceLine[1].position = (sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
                sliceLine[1].color =     sf::Color::Red;
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sliceLine[0].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    sliceLine[0].color =     sf::Color::Red;
                    sliceLine[1].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    sliceLine[1].color =     sf::Color::Red;

                    isleftPressed = true;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    box2DWorld.clearIntersects();
                    sliceLine[1].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    sliceLine[1].color =     sf::Color::Red;
                    isleftPressed = false;
                    box2DWorld.rayCast(sliceLine[0].position, sliceLine[1].position);
                    box2DWorld.rayCast(sliceLine[1].position, sliceLine[0].position);
                }
            }
        }


        box2DWorld.update(deltaClock.restart().asSeconds());

        App.draw(sliceLine);
        App.display();
        App.clear();


    }

}


