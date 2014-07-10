Splitter
========

Splitter is a box2d slicing engine used to split shapes into smaller ones. It comes with a simple texture mapping class that can texture your split shapes for you, given the original box2d body and texture co-ordinates. Also, its under MIT liscence. Still under active development.


![ScreenShot](https://raw.githubusercontent.com/SundeepK/Splitter/master/images/splitter_test_image.png)

## Dependencies
Splitter needs a few things to build and run:
- C++ compiler (gcc for instance)
- Opengl
- SFML
- Box2d
- Codeblocks (if you wish to open the project file)
- coffee

## Build instructions
To build the demo sandbox test that comes with Splitter, you will need to do the following:

```bash

/Splitter $ mkdir build
/Splitter $ cd build/
/Splitter/build $ cmake ..
/Splitter/build $ make
```
Then simply run the Splitter example.

## Usage
Splitter extends b2RayCastCallback, so you will only need to perform 2 raycasts to get box2d bodies to split, (Check the example main class for details).

```CPP
    //example usage
    Splitter splitter;
    m_world.RayCast(&splitter, point1, point2);
    m_world.RayCast(&splitter, point2, point1);

```

Splitter allows you to register both a callback class (which much extend B2BodySplitCallback) or a function to recieve callbacks when a box2d body has been split. You can then use TextureMapper class to get a vector of b2Vecs of your texture coords (see example main for more details). This callback will give you builder objects to which you pass your world to construct your new split Box2d bodies and the original box2d body that was split. Below is a simple example:

```CPP

    //example callback
    splitter.registerBodySplitCallback([&box2dWorld](std::vector<B2BoxBuilder> splitBodies, b2Body* parentBody) -> void {
       TextureMapper textureMapper(30.0f); //world scale
       Texcoords *parentBodyTexCoords   = (Texcoords*) parentBody->GetUserData();
       std::vector<b2Vec2> texCoords = parentBodyTexCoords->textCoords;
       
        for(auto builder : splitBodies) {
            b2Body* newSplitBody = builder.build(box2dWorld); //your box2d world
            Texcoords *texturesForNewBody = new Texcoords();
            std::vector<b2Vec2> newBodyTexCoords =  textureMapper.mapSplitBody(newSplitBody, parentBody, texCoords);
            texturesForNewBody->textCoords = newBodyTexCoords;
            newSplitBody->SetUserData(texturesForNewBody);
        }

        box2dWorld.deleteBody(body);
        delete parentBodyTexCoords; 
    });

```

To use TextureMapper class, you must make sure that the original body was textured correcly before. With it's verticies mapping in the same order as it's texture coordinates.

That's all there is to it.

The test texture used in the main example and screenshot is taken from here http://www.isc.tamu.edu/~lewing/linux/index.html. 




