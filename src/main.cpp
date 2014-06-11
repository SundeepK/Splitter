#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <Box2D/Box2D.h>
#include "SFMLB2DebugDraw.h"


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

    b2World m_world(b2Vec2(0.0f,9.8f));
    SFMLB2dDebugDraw drawer(App);
    drawer.SetFlags(b2Draw::e_shapeBit);

    //build a simple box
    m_world.SetDebugDraw(&drawer);
    b2BodyDef myBodyDef;
    myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
    myBodyDef.position.Set(20.0f/Box2DConstants::WORLD_SCALE+10/2.0f, 20.0f/Box2DConstants::WORLD_SCALE+10.0f/2.0f); //set the starting position
    myBodyDef.angle = 0; //set the starting angle
    b2Body* dynamicBody = m_world.CreateBody(&myBodyDef);
    b2PolygonShape boxShape;
    boxShape.SetAsBox(20.0f/Box2DConstants::WORLD_SCALE/2.0f,20.0f/Box2DConstants::WORLD_SCALE/2.0f);
    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    dynamicBody->CreateFixture(&boxFixtureDef);

    //floor
    b2BodyDef ground;
    ground.type = b2_staticBody; //this will be a dynamic body
    ground.position.Set((30.0f/Box2DConstants::WORLD_SCALE)+70.0f/2.0f, (50.0f/Box2DConstants::WORLD_SCALE)+30.0f/2.0f); //set the starting position
    ground.angle = 0; //set the starting angle
    b2Body* groundBody = m_world.CreateBody(&ground);
    b2PolygonShape groundbox;
    groundbox.SetAsBox((700.0f/Box2DConstants::WORLD_SCALE)/2.0f,(30.0f/Box2DConstants::WORLD_SCALE)/2.0f);
    b2FixtureDef groundFixture;
    groundFixture.shape = &groundbox;
    groundFixture.density = 1;
    groundBody->CreateFixture(&groundFixture);

    while (App.isOpen())
    {

        sf::Event event;
        while( App.pollEvent(event))
        {

        if(isleftPressed) {
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
                 sliceLine[1].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                 sliceLine[1].color =     sf::Color::Red;
                 isleftPressed = false;

                }
            }
        }

        m_world.Step(1/60.0f,8,3);
	    m_world.DrawDebugData();

        App.draw(sliceLine);
        App.display();
        App.clear();


    }

}


