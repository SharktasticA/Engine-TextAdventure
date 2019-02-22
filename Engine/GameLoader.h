/*
    Game file XML parser class
    Khalid Ali 2019
    http://khalidali.co.uk
*/

#ifndef ENGINE_TEXTADVENTURE_GAMELOADER_H
#define ENGINE_TEXTADVENTURE_GAMELOADER_H

#include "../Structures/Game.h"
#include "../Structures/Place.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

//Inteprets game file XML into a workable format
class GameLoader
{
private:
    //Loaded game XML file
    xml_document doc;

    //Attempts to load game <Details> data from game file
    void loadGameData(void)
    {
        try
        {
            //Initialise Game object and give it <Details> content
            game = new Game;
            game->gameName = doc.child("Game").attribute("name").as_string();
            game->travelMethod = doc.child("Game").child("Details").child("TravelMethod").child_value();
            game->distanceUnit = doc.child("Game").child("Details").child("DistanceUnit").child_value();
        }
        catch(...) { throw "FATAL ERROR: cannot load <Details> from game file!"; }
    }

    //Attempts to load game <Map> data from game file
    void loadMapData(void)
    {
        try
        {
            // 1) Construct all individual place objects via Area XML
            // names without mapping for the moment. Just store adjacent
            // neighbour data in two arrays since all data needs to be
            // read before pointer links can be established

            //All individual places
            vector<Place*> places;
            //Four possible neighbour names for each place
            vector<string[4]> adjacentNames;
            //Four possible neighbour travel dsitances for each place
            vector<int[4]> adjacentCosts;

            for (xml_node area = doc.child("Game").child("Area"); area; area = area.next_sibling("Area"))
            {
                places.push_back(new Place(area.attribute("name").as_string()));

                string adjName[4];
                adjName[0] = area.child("NorthName").child_value();
                adjName[1] = area.child("EastName").child_value();
                adjName[2] = area.child("SouthName").child_value();
                adjName[3]  = area.child("WestName").child_value();
                adjacentNames.push_back(adjName);

                int adjDist[4];
                adjDist[0] = (int)area.child("NorthDistance").child_value()[0];
                adjDist[1] = (int)area.child("EastDistance").child_value()[0];
                adjDist[2] = (int)area.child("SouthDistance").child_value()[0];
                adjDist[3]  = (int)area.child("WestDistance").child_value()[0];
                adjacentCosts.push_back(adjDist);
            }

            // 2) For each individual place, find the neighbour objects
            // based on their name and create the (up to four-way) doubly
            // linked list from it. If the place has no neighbour for a direction,
            // nullptr is used to indicate lack of elsewhere in the program.

            for (int i = 0; i < places.size(); i++)
            {
                vector<PlaceData*> relatives;

                for (int j = 0; j < 4; j++)
                {
                    if (adjacentNames[i][j] != "")
                    {
                        for (Place* place : places) if (place->GetName() == adjacentNames[i][j])
                                relatives.push_back(new PlaceData(place, adjacentCosts[i][j]));
                    }
                    else relatives.push_back(new PlaceData(nullptr, 0));
                }

                places[i]->MapPlaces(relatives[0], relatives[1], relatives[2], relatives[3]);
            }
        }
        catch(...) { throw "FATAL ERROR: cannot load <Map> from game file!"; }
    }

    //
    void loadEnemyData(void)
    {

    }

public:
    //
    Game* game;

    GameLoader(string loc)
    {
        //Attempt XML document parse to see if we can proceed
        xml_parse_result rst = doc.load_file(loc.c_str());
        if (!rst) throw "FATAL ERROR: cannot load game file " + loc + "!";

        //Attempt load data from XML document
        loadGameData();
        loadMapData();
    }
};

#endif //ENGINE_TEXTADVENTURE_GAMELOADER_H