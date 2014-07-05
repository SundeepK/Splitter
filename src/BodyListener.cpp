#include "BodyListener.h"

BodyListener::BodyListener(B2DWorld& world) : box2DWorld(world)
{
    //ctor
}

BodyListener::~BodyListener()
{
    //dtor
}

void BodyListener::onb2BodySplit(std::vector<B2BoxBuilder>& splitBodies, b2Body* body){

// if(body->GetMass() < 0.1f) return;
//
//        Texcoords *textures   = (Texcoords*) body->GetUserData();
//
//        b2PolygonShape* shape =((b2PolygonShape*)body->GetFixtureList()->GetShape());
//        std::vector<b2Vec2> points(shape->GetVertexCount());
//        int count = shape->GetVertexCount();
//
//        for(auto builder : splitBodies) {
//            b2Body* newB =     box2DWorld.createB2Body(&builder);
//               Texcoords *texturesForNewBody = new Texcoords();
//
//            for(int i=0; i< count; i++) {
//                b2PolygonShape* newShape =((b2PolygonShape*)newB->GetFixtureList()->GetShape());
//
//                int i2 = i + 1 < count ? i + 1 : 0;
//                b2Vec2 edge1 = (shape)->GetVertex(i);
//                b2Vec2 edge2 = (shape)->GetVertex(i2);
//
//                 b2Vec2 newVert =(newShape)->GetVertex(i);
//
//                 if(newVert.x == edge1.x && newVert.y == edge1.y){
//                   std::cout << "same vert texcoords  " <<  textures->textCoords[i].x << "," << textures->textCoords[i].y << std::endl;
//                   std::cout <<  std::endl;
//
//                  texturesForNewBody->textCoords.push_back(textures->textCoords[i]);
//                 }
//                 else if(isCCW( edge1, newVert, edge2) == 0){
//                   float length = getLength(edge1, edge2);
//                   float newVertLength = getLength(newVert, edge2);
//
//                   float percetageThrough = newVertLength / length;
//
//                   std::cout << "percentagle thoguht " <<  percetageThrough << std::endl;
//                   std::cout <<  std::endl;
//
//                   b2Vec2 newTex1 ( edge1.x  * 1.0f -newVertLength, edge1.y  * 1.0f -newVertLength);
//                   b2Vec2 newTex2 ( edge2.x  * newVertLength, edge2.y * newVertLength);
//                   texturesForNewBody->textCoords.push_back(newTex1 + newTex2);
//                    newB->SetUserData(texturesForNewBody);
//
//                 }
//            }
//
//
//            //    splitbs.push_back(b);
//        }
//
//        if(textures) {
//            delete textures;
//        }
//        box2DWorld.deleteBody(body);
//
//

}

