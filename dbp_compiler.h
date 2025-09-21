#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "json.hpp"

namespace DBH {
using json = nlohmann::json;

class Sprite {
private:
    std::string name;
    json blocks;
    int blockIdCounter = 0;
    
    std::string generateBlockId() {
        return std::to_string(++blockIdCounter);
    }

public:
    Sprite(const std::string& spriteName) : name(spriteName) {
        blocks = json::object();
    }

    std::string getName() const {
        return name;
    }

    void move(int steps) {
        std::string blockId = generateBlockId();
        blocks[blockId] = {
            {"opcode", "motion_movesteps"},
            {"next", nullptr},
            {"parent", nullptr},
            {"inputs", {
                {"STEPS", {
                    1,
                    {4, std::to_string(steps)}
                }}
            }},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true},
            {"x", 0},
            {"y", 0}
        };
    }

    void turnRight(int degrees) {
        std::string blockId = generateBlockId();
        blocks[blockId] = {
            {"opcode", "motion_turnright"},
            {"next", nullptr},
            {"parent", nullptr},
            {"inputs", {
                {"DEGREES", {
                    1,
                    {4, std::to_string(degrees)}
                }}
            }},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true},
            {"x", 0},
            {"y", 0}
        };
    }

    void say(const std::string& text) {
        std::string blockId = generateBlockId();
        blocks[blockId] = {
            {"opcode", "looks_say"},
            {"next", nullptr},
            {"parent", nullptr},
            {"inputs", {
                {"MESSAGE", {
                    1,
                    {10, text}
                }}
            }},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true},
            {"x", 0},
            {"y", 0}
        };
    }

    void show() {
        std::string blockId = generateBlockId();
        blocks[blockId] = {
            {"opcode", "looks_show"},
            {"next", nullptr},
            {"parent", nullptr},
            {"inputs", json::object()},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true},
            {"x", 0},
            {"y", 0}
        };
    }

    json toJSON() const {
        json spriteJson;
        spriteJson["isStage"] = false;
        spriteJson["name"] = name;
        spriteJson["variables"] = json::object();
        spriteJson["lists"] = json::object();
        spriteJson["broadcasts"] = json::object();
        spriteJson["blocks"] = blocks;
        spriteJson["comments"] = json::object();
        spriteJson["currentCostume"] = 0;
        
        spriteJson["costumes"] = json::array();
        json costume;
        costume["name"] = "костюм 1";
        costume["bitmapResolution"] = 1;
        costume["dataFormat"] = "svg";
        costume["assetId"] = "d752c24bbac45bab70074cc380cf0ff0";
        costume["md5ext"] = "d752c24bbac45bab70074cc380cf0ff0.svg";
        costume["rotationCenterX"] = 24;
        costume["rotationCenterY"] = 17;
        spriteJson["costumes"].push_back(costume);
        
        spriteJson["sounds"] = json::array();
        spriteJson["volume"] = 100;
        spriteJson["layerOrder"] = 1;
        spriteJson["visible"] = true;
        spriteJson["x"] = 0;
        spriteJson["y"] = 0;
        spriteJson["size"] = 100;
        spriteJson["direction"] = 90;
        spriteJson["draggable"] = false;
        spriteJson["rotationStyle"] = "all around";
        
        return spriteJson;
    }
};

class Project {
private:
    std::vector<Sprite> sprites;

public:
    Project(const std::string& name) {}
    
    void addSprite(const Sprite& sprite) {
        sprites.push_back(sprite);
    }

    bool saveToFile(const std::string& filename) {
        json projectJson;
        projectJson["targets"] = json::array();
        projectJson["monitors"] = json::array();
        projectJson["extensions"] = json::array();
        projectJson["meta"] = {
            {"semver", "3.0.0"},
            {"vm", "0.2.0"},
            {"agent", "DBH Compiler"}
        };
        
        json stage;
        stage["isStage"] = true;
        stage["name"] = "Stage";
        stage["variables"] = json::object();
        stage["lists"] = json::object();
        stage["broadcasts"] = json::object();
        stage["blocks"] = json::object();
        stage["comments"] = json::object();
        stage["currentCostume"] = 0;
        
        stage["costumes"] = json::array();
        json backdrop;
        backdrop["name"] = "фон 1";
        backdrop["bitmapResolution"] = 1;
        backdrop["dataFormat"] = "svg";
        backdrop["assetId"] = "2754c5c5e22c677d7f9fb8a2c6115462";
        backdrop["md5ext"] = "2754c5c5e22c677d7f9fb8a2c6115462.svg";
        backdrop["rotationCenterX"] = 282;
        backdrop["rotationCenterY"] = 237;
        stage["costumes"].push_back(backdrop);
        
        stage["sounds"] = json::array();
        stage["volume"] = 100;
        stage["layerOrder"] = 0;
        stage["tempo"] = 60;
        stage["videoTransparency"] = 50;
        stage["videoState"] = "on";
        stage["textToSpeechLanguage"] = nullptr;
        projectJson["targets"].push_back(stage);
        
        for (const auto& sprite : sprites) {
            projectJson["targets"].push_back(sprite.toJSON());
        }
        
        std::ofstream file(filename + ".json");
        file << projectJson.dump(4);
        return true;
    }
};
}
