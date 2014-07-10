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
#include "TemplateHasher.h"
#include <unordered_map>
#include <algorithm>
#include "B2vecHasher.h"
#include "TextureMapper.h"

B2BoxBuilder getBox2dBuilder(std::vector<b2Vec2> points, b2Body* body) {
    B2BoxBuilder builder(points, body);
    return builder;
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow App(sf::VideoMode(1280, 800,32), "Splitter test",sf::Style::Default, settings);
    App.setKeyRepeatEnabled(true);
    App.setFramerateLimit(60);


    glDisable(GL_LIGHTING);

    // Configure the viewport (the same size as the window)
    glViewport(0, 0, App.getSize().x, App.getSize().y);

    glMatrixMode(GL_PROJECTION);
    glOrtho(0,1280,800,0,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);                  // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);             // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);

    GLuint texture = 0;
    {
        sf::Image image;
        if (!image.loadFromFile("textures/1.png"))
            return EXIT_FAILURE;
        image.flipVertically();
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.getSize().x, image.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    glBindTexture(GL_TEXTURE_2D,texture);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);


    sf::VertexArray sliceLine(sf::Lines, 2);
    bool isleftPressed = false;

    B2DWorld box2DWorld(9.8f);

    //box
    B2BoxBuilder builder(70,70);
    builder
    .bodyType(b2_dynamicBody)
    .setPosition(b2Vec2(200,100))
    .setDensity(1.0f);
    b2Body* b = box2DWorld.createB2Body(&builder);
    b->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);

    std::vector<b2Vec2> texs;
    texs.push_back(b2Vec2(0.0f,1.f));
    texs.push_back(b2Vec2(1.0f,1.0f));
    texs.push_back(b2Vec2(1.0f,0.0f));
    texs.push_back(b2Vec2(0.0f,0.0f));
    Texcoords* b1textcoords = new Texcoords;
    b1textcoords->textCoords = texs;

    b->SetUserData(b1textcoords);



//    B2BoxBuilder eightSides(sides ,b);
//    eightSides.setPosition(b2Vec2(100,100));
//    b2Body* b3 = box2DWorld.createB2Body(&eightSides);


    B2BoxBuilder secondndbox(70,70);
    secondndbox
    .bodyType(b2_dynamicBody)
    .setPosition(b2Vec2(120,10))
    .setRestitution(0.3f)
    .setDensity(1.0f);
    b2Body* b2 = box2DWorld.createB2Body(&secondndbox);
    b2->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);
    Texcoords* b1textcoords2 = new Texcoords;
    b1textcoords2->textCoords = texs;
    b2->SetUserData(b1textcoords2);

    //ground
    B2BoxBuilder groundShapebuilder(1200, 50);
    groundShapebuilder
    .bodyType(b2_staticBody)
    .setPosition(b2Vec2(0,400))
    .setDensity(1.0f);
    box2DWorld.createB2Body(&groundShapebuilder);

    TextureMapper textureMapper(Box2DConstants::WORLD_SCALE);

    sf::Clock deltaClock;

    box2DWorld.registerBodySplitCallback([&box2DWorld, &textureMapper](std::vector<B2BoxBuilder> splitBodies, b2Body* body) -> void {
        if(body->GetMass() < 0.1f) return;

        Texcoords *textures   = (Texcoords*) body->GetUserData();

        for(auto builder : splitBodies) {
            b2Body* newB =     box2DWorld.createB2Body(&builder);
            Texcoords *texturesForNewBody = new Texcoords();
            std::vector<b2Vec2> newBOdyTexCoords =  textureMapper.mapSplitBody(newB, body,textures->textCoords);
            texturesForNewBody->textCoords = newBOdyTexCoords;
            newB->SetUserData(texturesForNewBody);
        }


        if(textures) {
            delete textures;
        }
        box2DWorld.deleteBody(body);
    });

    while (App.isOpen()) {

        sf::Event event;
        while( App.pollEvent(event)) {

            if(event.type == sf::Event::Closed){
                        App.close();
            }

            if(isleftPressed) {
                sliceLine[1].position = (sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
                sliceLine[1].color =     sf::Color::Red;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sliceLine[0].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    sliceLine[0].color =     sf::Color::Red;
                    sliceLine[1].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    sliceLine[1].color =     sf::Color::Red;

                    isleftPressed = true;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    box2DWorld.clearIntersects();
                    sliceLine[1].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    sliceLine[1].color =     sf::Color::Red;
                    isleftPressed = false;
                    if( !(sliceLine[0].position == sliceLine[1].position)) {
                        box2DWorld.rayCast(sliceLine[0].position, sliceLine[1].position);
                        box2DWorld.rayCast(sliceLine[1].position, sliceLine[0].position);
                    }

                }
            }
        }

        box2DWorld.update(deltaClock.restart().asSeconds());

        App.pushGLStates();
        App.clear(sf::Color::Black);
        App.draw(sliceLine);
        App.popGLStates();


        App.setActive(true);

        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        //draw opengl here
        glColor3f(1.0f,1.0f,1.0f);


        const float M2P=30;
        for (b2Body * bn = box2DWorld.GetBodyList(); bn != NULL; bn = bn->GetNext ()) {

            b2PolygonShape* shape =((b2PolygonShape*)bn->GetFixtureList()->GetShape());
            std::vector<b2Vec2> points(shape->GetVertexCount());
            for(int i=0; i<shape->GetVertexCount(); i++) {
                points[i]=(shape)->GetVertex(i);
            }

            glPushMatrix();
            b2Vec2 center = bn->GetWorldCenter();
            float angle = bn->GetAngle();
            glTranslatef(center.x*M2P,center.y*M2P,0);
            glRotatef(angle*180.0/M_PI,0,0,1);

            Texcoords *tex   = (Texcoords*) bn->GetUserData();
            glBegin(GL_POLYGON);//begin drawing of polygon
            for(int i=0; i<shape->GetVertexCount(); i++) {
                if(tex) {
                    glTexCoord2d(tex->textCoords[i].x, tex->textCoords[i].y);
                }
                glVertex2f(points[i].x*M2P,points[i].y*M2P);
            }
            glEnd();//end drawing of polygon
            glPopMatrix();

        }


        App.pushGLStates();
        //draw sfml stuff here
        App.popGLStates();
        App.display();
    }

    for (b2Body * bn = box2DWorld.GetBodyList(); bn != NULL; bn = bn->GetNext ()) {
        Texcoords *tex   = (Texcoords*) bn->GetUserData();
        if(tex){
           delete tex;
        }
    }

    return 0;

}


