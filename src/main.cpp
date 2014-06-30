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


struct Texcoords {
    public:
    std::vector<b2Vec2> textCoords;
};

void drawCircle(sf::RenderWindow& App, sf::Vector2f pos, sf::Color color) {
    sf::CircleShape entryCircle(4);
    entryCircle.setFillColor(color);
    entryCircle.setPosition(sf::Vector2f(pos.x-entryCircle.getRadius(), pos.y-entryCircle.getRadius()));
    App.draw(entryCircle);

}

int isCCW(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3) {
    return p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
}

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
    glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                   // Black Background
    glClearDepth(1.0f);                         // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);

    GLuint texture = 0;
    {
        sf::Image image;
        if (!image.loadFromFile("1.png"))
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

    B2DWorld box2DWorld(50.8f);
    SFMLB2dDebugDraw drawer(App);
    box2DWorld.setDebugDraw(drawer);
    drawer.SetFlags(b2Draw::e_shapeBit);

    //box
    B2BoxBuilder builder(50,50);
    builder
    .bodyType(b2_dynamicBody)
    .setPosition(b2Vec2(200,100))
    .setDensity(1.0f);
    b2Body* b = box2DWorld.createB2Body(&builder);
    b->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);

    std::vector<b2Vec2> texs;
    texs.push_back(b2Vec2(0.0f,0.5f));
    texs.push_back(b2Vec2(0.5f,0.5f));
    texs.push_back(b2Vec2(0.5f,0.0f));
    texs.push_back(b2Vec2(0.0f,0.0f));
    Texcoords* b1textcoords = new Texcoords;
    b1textcoords->textCoords = texs;

    b->SetUserData(b1textcoords);


    std::vector<b2Vec2> sides;

    sides.push_back (b2Vec2(-1.5,-0.5));
    sides.push_back (b2Vec2(0.5,-1));
    sides.push_back(b2Vec2(1,-0.5));
    sides.push_back(b2Vec2(1,1.5));
    sides.push_back(b2Vec2(0.5,1.5));
    sides.push_back(b2Vec2(-0.5,1.3));


//    B2BoxBuilder eightSides(sides ,b);
//    eightSides.setPosition(b2Vec2(100,100));
//    b2Body* b3 = box2DWorld.createB2Body(&eightSides);


    B2BoxBuilder secondndbox(50,50);
    secondndbox
    .bodyType(b2_dynamicBody)
    .setPosition(b2Vec2(120,10))
    .setRestitution(0.3f)
    .setDensity(1.0f);
    b2Body* b2 = box2DWorld.createB2Body(&secondndbox);
    b2->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);

    //ground
    B2BoxBuilder groundShapebuilder(1200, 50);
    groundShapebuilder
    .bodyType(b2_staticBody)
    .setPosition(b2Vec2(0,400))
    .setDensity(1.0f);
    box2DWorld.createB2Body(&groundShapebuilder);

    sf::Clock deltaClock;

    std::vector<b2Body*> splitbs;

   box2DWorld.registerBodySplitCallback([&box2DWorld, &splitbs](std::vector<B2BoxBuilder> splitBodies, b2Body* body) -> void {
        if(body->GetMass() < 0.1f) return;
        for(auto builder : splitBodies) {
            b2Body* b =     box2DWorld.createB2Body(&builder);
        //    splitbs.push_back(b);
        }

        Texcoords *textures   = (Texcoords*) body->GetUserData();
        if(textures){
             delete textures;
        }
        box2DWorld.deleteBody(body);


    });



    while (App.isOpen()) {

        sf::Event event;
        while( App.pollEvent(event)) {

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

}














//
//
//
//
//
//#include <SFML/Graphics.hpp>
//#include <SFML/System.hpp>
//#include <SFML/Window.hpp>
//
//#include "B2DWorld.h"
//#include "B2BoxBuilder.h"
//#include <functional>
//#include <iostream>
//#include "ActionController.h"
//#include <string>
//#include <math.h>
//#include <SFML/OpenGL.hpp>
//#include <GL/glut.h>
//
//void drawCircle(sf::RenderWindow& App, sf::Vector2f pos, sf::Color color) {
//    sf::CircleShape entryCircle(4);
//    entryCircle.setFillColor(color);
//    entryCircle.setPosition(sf::Vector2f(pos.x-entryCircle.getRadius(), pos.y-entryCircle.getRadius()));
//    App.draw(entryCircle);
//
//}
//
//int isCCW(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3) {
//    return p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
//}
//
//B2BoxBuilder getBox2dBuilder(std::vector<b2Vec2> points, b2Body* body) {
//    B2BoxBuilder builder(points, body);
//    return builder;
//}
//
//int main() {
//    sf::ContextSettings settings;
//    settings.depthBits = 24;
//    settings.stencilBits = 8;
//    settings.antialiasingLevel = 4;
//    settings.majorVersion = 3;
//    settings.minorVersion = 0;
//    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
//    sf::RenderWindow App(sf::VideoMode(1280, 800,32), "Splitter test",sf::Style::Default, settings);
//    App.setKeyRepeatEnabled(true);
//    App.setFramerateLimit(60);
//
//    sf::VertexArray sliceLine(sf::Lines, 2);
//    bool isleftPressed = false;
//
//    B2DWorld box2DWorld(9.8f);
//    SFMLB2dDebugDraw drawer(App);
//    box2DWorld.setDebugDraw(drawer);
//    drawer.SetFlags(b2Draw::e_shapeBit);
//
//    //box
//    B2BoxBuilder builder(50,50);
//    builder
//    .bodyType(b2_dynamicBody)
//    .setPosition(b2Vec2(90,90))
//    .setRestitution(1.0f)
//    .setDensity(1.0f);
//    b2Body* b = box2DWorld.createB2Body(&builder);
//    b->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);
//    b->ApplyTorque(500,true);
//
//    std::vector<b2Vec2> sides;
//
//    sides.push_back (b2Vec2(-1.5,-0.5));
//    sides.push_back (b2Vec2(0.5,-1));
//    sides.push_back(b2Vec2(1,-0.5));
//    sides.push_back(b2Vec2(1,1.5));
//    sides.push_back(b2Vec2(0.5,1.5));
//    sides.push_back(b2Vec2(-0.5,1.3));
//
//
//    B2BoxBuilder eightSides(sides ,b);
//    eightSides.setPosition(b2Vec2(100,100));
//    b2Body* b3 = box2DWorld.createB2Body(&eightSides);
//
//
//    B2BoxBuilder secondndbox(50,50);
//    secondndbox
//    .bodyType(b2_dynamicBody)
//    .setPosition(b2Vec2(120,10))
//    .setDensity(1.0f);
//    b2Body* b2 = box2DWorld.createB2Body(&secondndbox);
//    b2->ApplyLinearImpulse( b2Vec2(0.1f,0.1f), b->GetWorldCenter(), true);
//
//    //ground
//    B2BoxBuilder groundShapebuilder(1200, 50);
//    groundShapebuilder
//    .bodyType(b2_staticBody)
//    .setPosition(b2Vec2(0,700))
//    .setDensity(1.0f);
//    box2DWorld.createB2Body(&groundShapebuilder);
//
//    sf::Clock deltaClock;
//
//    std::vector< sf::RectangleShape> sprites;
//    std::vector<b2Body*> splitbs;
//
//
//    sf::RectangleShape rect(sf::Vector2f(20,20));
//    rect.setPosition(sf::Vector2f(b->GetPosition().x,b->GetPosition().y));
//
//
//    sf::Texture texture;
//    if (!texture.loadFromFile("1.png"))
//    {}
//
//    GLuint texture = 0;
//    {
//        sf::Image image;
//        if (!image.loadFromFile("1.png"))
//            return EXIT_FAILURE;
//        image.flipVertically();
//        glGenTextures(1, &texture);
//        glBindTexture(GL_TEXTURE_2D, texture);
//        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.getSize().x, image.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    }
//
//    glBindTexture(GL_TEXTURE_2D,texture);
//    glDisableClientState(GL_NORMAL_ARRAY);
//    glDisableClientState(GL_COLOR_ARRAY);
//
//    rect.setTexture(&texture);
//
//    box2DWorld.registerBodySplitCallback([&box2DWorld, &splitbs, &texture](std::vector<B2BoxBuilder> splitBodies, b2Body* body) -> void {
//        if(body->GetMass() < 0.1f) return;
//        for(auto builder : splitBodies) {
//            b2Body* b =     box2DWorld.createB2Body(&builder);
//            sf::RectangleShape* sprite = new  sf::RectangleShape(sf::Vector2f(50,50));
//            sprite->setTexture(&texture);
//            b->SetUserData(sprite);
//            splitbs.push_back(b);
//        }
//
//        box2DWorld.deleteBody(body);
//
//
//    });
//
//        glDisable(GL_LIGHTING);
//
//    // Configure the viewport (the same size as the window)
//    glViewport(0, 0, App.getSize().x, App.getSize().y);
//
//    glMatrixMode(GL_PROJECTION);
//    glOrtho(0,1280,800,0,-1,1);
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//
//   glEnable(GL_TEXTURE_2D);
//    glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
//    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                   // Black Background
//    glClearDepth(1.0f);                         // Depth Buffer Setup
//    glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
//    glDepthFunc(GL_LEQUAL);
//
//    sf::VertexArray quad(sf::Quads, 4);
//
//    // define it as a rectangle, located at (10, 10) and with size 100x100
//    quad[0].position = sf::Vector2f(10, 10);
//    quad[1].position = sf::Vector2f(110, 10);
//    quad[2].position = sf::Vector2f(110, 110);
//    quad[3].position = sf::Vector2f(10, 110);
//
//    // define its texture area to be a 25x50 rectangle starting at (0, 0)
//    quad[0].texCoords = sf::Vector2f(0, 0);
//    quad[1].texCoords = sf::Vector2f(200, 0);
//    quad[2].texCoords = sf::Vector2f(200, 200);
//    quad[3].texCoords = sf::Vector2f(0, 200);
//
//
//    while (App.isOpen()) {
//
//        sf::Event event;
//        while( App.pollEvent(event)) {
//
//            if(isleftPressed) {
//                sliceLine[1].position = (sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
//                sliceLine[1].color =     sf::Color::Red;
//            }
//
//            if (event.type == sf::Event::MouseButtonPressed) {
//                if (event.mouseButton.button == sf::Mouse::Left) {
//                    sliceLine[0].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
//                    sliceLine[0].color =     sf::Color::Red;
//                    sliceLine[1].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
//                    sliceLine[1].color =     sf::Color::Red;
//
//                    isleftPressed = true;
//                }
//            }
//
//            if (event.type == sf::Event::MouseButtonReleased) {
//                if (event.mouseButton.button == sf::Mouse::Left) {
//                    box2DWorld.clearIntersects();
//                    sliceLine[1].position = (sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
//                    sliceLine[1].color =     sf::Color::Red;
//                    isleftPressed = false;
//                    if( !(sliceLine[0].position == sliceLine[1].position)) {
//                        box2DWorld.rayCast(sliceLine[0].position, sliceLine[1].position);
//                        box2DWorld.rayCast(sliceLine[1].position, sliceLine[0].position);
//                    }
//
//                }
//            }
//        }
//
//
//        box2DWorld.update(deltaClock.restart().asSeconds());
//
//        for (b2Body * bn = box2DWorld.GetBodyList(); bn != NULL; bn = bn->GetNext ()) {
//            if (b->GetType () == b2_staticBody) {
//                continue;
//            }
//
//            sf::RectangleShape *c   = (sf::RectangleShape*) bn->GetUserData();
//            if(c !=NULL){
//              b2Vec2 vec = ((bn->GetPosition()));
//                sf::Vector2f sfv(vec.x*30, vec.y*30);
//                c->setPosition(sfv);
//                App.draw(*c);
//            }
//        }
//
//        App.draw(sliceLine);
//        App.draw(quad,&texture);
//        App.display();
//        App.clear();
//
//
//    }
//
//    for(auto b : splitbs) {
//        sf::Sprite *c   = (sf::Sprite*) b->GetUserData();
//        delete c;
//    }
//
//}





