// dbh-compiler.h
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>

// JSON
#ifndef NLOHMANN_JSON_HPP
#define NLOHMANN_JSON_HPP
#include "json.hpp"
#endif

// ZIPI VSAKIE
#ifdef _WIN32
#include <windows.h>
#include <zip.h>
#else
#include <zip.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

namespace DBH {

using json = nlohmann::json;

namespace Utility {
    std::vector<uint8_t> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return {};
        return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
    }

    bool writeFile(const std::string& filename, const std::vector<uint8_t>& data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) return false;
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    }

    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
}

class Optimizer {
public:
    static std::vector<uint8_t> compressPNG(const std::vector<uint8_t>& input, int quality = 85) {
        std::vector<uint8_t> output;
        int width, height, channels;

        unsigned char* data = stbi_load_from_memory(input.data(), input.size(),
                                                   &width, &height, &channels, 0);
        if (data) {
            stbi_write_png_to_func([](void* context, void* data, int size) {
                auto* vec = static_cast<std::vector<uint8_t>*>(context);
                vec->insert(vec->end(), static_cast<uint8_t*>(data),
                           static_cast<uint8_t*>(data) + size);
            }, &output, width, height, channels, data, width * channels);

            stbi_image_free(data);
        }
        return output;
    }

    static std::vector<uint8_t> compressJPEG(const std::vector<uint8_t>& input, int quality = 75) {
        std::vector<uint8_t> output;
        int width, height, channels;

        unsigned char* data = stbi_load_from_memory(input.data(), input.size(),
                                                   &width, &height, &channels, 0);
        if (data) {
            stbi_write_jpg_to_func([](void* context, void* data, int size) {
                auto* vec = static_cast<std::vector<uint8_t>*>(context);
                vec->insert(vec->end(), static_cast<uint8_t*>(data),
                           static_cast<uint8_t*>(data) + size);
            }, &output, width, height, channels, data, quality);

            stbi_image_free(data);
        }
        return output;
    }

    static bool shouldConvertToJPEG(const std::vector<uint8_t>& imageData) {
        if (imageData.size() < 100) return false;

        int width, height, channels;
        unsigned char* data = stbi_load_from_memory(imageData.data(), imageData.size(),
                                                   &width, &height, &channels, 0);
        if (!data) return false;

        // Check PNG
        bool hasTransparency = false;
        if (channels == 4) {
            for (int i = 3; i < width * height * channels; i += channels) {
                if (data[i] < 255) {
                    hasTransparency = true;
                    break;
                }
            }
        }

        stbi_image_free(data);
        return !hasTransparency;
    }
};


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

    void move(int steps) {
        std::string blockId = generateBlockId();
        
        blocks[blockId] = {
            {"opcode", "motion_movesteps"},
            {"next", nullptr},
            {"parent", nullptr},
            {"inputs", {
                {"STEPS", {
                    {"name", std::to_string(steps)},
                    {"block", nullptr},
                    {"shadow", nullptr}
                }}
            }},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true}
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
                    {"name", std::to_string(degrees)},
                    {"block", nullptr},
                    {"shadow", nullptr}
                }}
            }},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true}
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
                    {"name", text},
                    {"block", nullptr},
                    {"shadow", nullptr}
                }}
            }},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true}
        };
    }

    void whenFlagClicked(std::function<void(Sprite&)> callback) {
        std::string eventId = generateBlockId();
        
        blocks[eventId] = {
            {"opcode", "event_whenflagclicked"},
            {"next", nullptr},
            {"parent", nullptr},
            {"inputs", json::object()},
            {"fields", json::object()},
            {"shadow", false},
            {"topLevel", true}
        };

        Sprite tempSprite("temp");
        callback(tempSprite);

        if (!tempSprite.blocks.empty()) {
            std::string firstChildId = tempSprite.blocks.begin().key();
            blocks[eventId]["next"] = firstChildId;
            
            for (auto& [id, block] : tempSprite.blocks.items()) {
                blocks[id] = block;
            }
        }
    }

    json toJSON() const {
        json spriteJson;
        spriteJson["name"] = name;
        spriteJson["blocks"] = blocks; 
        spriteJson["variables"] = json::object();
        spriteJson["lists"] = json::object();
        spriteJson["broadcasts"] = json::object();
        spriteJson["comments"] = json::object();
        spriteJson["currentCostume"] = 0;
        spriteJson["costumes"] = json::array();
        spriteJson["sounds"] = json::array();
        
        return spriteJson;
    }
};

class Stage : public Sprite {
private:
    std::vector<std::string> backdrops;

public:
    Stage() : Sprite("Stage") {}

    void addBackdrop(const std::string& filename) {
        backdrops.push_back(filename);
    }
};

class Project {
private:
    Stage stage;
    std::vector<Sprite> sprites;
    std::string projectName;
    bool compressImages = true;
    int qualityLevel = 85;

public:
    Project(const std::string& name) : projectName(name) {}

    void addSprite(const Sprite& sprite) {
        sprites.push_back(sprite);
    }

    void setCompression(bool enable, int quality = 85) {
        compressImages = enable;
        qualityLevel = quality;
    }

    bool saveToFile(const std::string& filename) {

        json projectJson;
        

        projectJson["targets"] = json::array();
        projectJson["monitors"] = json::array();
        projectJson["extensions"] = json::array();
        projectJson["meta"] = json::object();
        projectJson["meta"]["semver"] = "3.0.0";
        projectJson["meta"]["vm"] = "0.2.0";
        projectJson["meta"]["agent"] = "DBH Compiler";
        

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
        stage["sounds"] = json::array();
        stage["layerOrder"] = 0;
        stage["volume"] = 100;
        stage["tempo"] = 60;
        stage["videoTransparency"] = 50;
        stage["videoState"] = "on";
        stage["textToSpeechLanguage"] = nullptr;
        
        projectJson["targets"].push_back(stage);
        
        for (const auto& sprite : sprites) {
            json spriteJson = sprite.toJSON();
            spriteJson["isStage"] = false;
            spriteJson["layerOrder"] = 1;
            spriteJson["visible"] = true;
            spriteJson["x"] = 0;
            spriteJson["y"] = 0;
            spriteJson["size"] = 100;
            spriteJson["direction"] = 90;
            spriteJson["draggable"] = false;
            spriteJson["rotationStyle"] = "all around";
            
            projectJson["targets"].push_back(spriteJson);
        }
        

        int err = 0;
        zip_t* archive = zip_open(filename.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);
        if (!archive) {
            std::cerr << "Failed to create archive: " << err << std::endl;
            return false;
        }
        

        std::string projectStr = projectJson.dump();
        zip_source_t* source = zip_source_buffer(archive, projectStr.c_str(), projectStr.size(), 0);
        if (!source) {
            zip_close(archive);
            return false;
        }
        
        if (zip_file_add(archive, "project.json", source, ZIP_FL_OVERWRITE) < 0) {
            zip_source_free(source);
            zip_close(archive);
            return false;
        }
        
        if (zip_close(archive) < 0) {
            std::cerr << "Failed to close archive" << std::endl;
            return false;
        }
        
        return true;

    }   
};

class Decompiler {
public:
    static bool decompile(const std::string& sb3File, const std::string& outputDir) {
        //patom
        return true;
    }
};

}
