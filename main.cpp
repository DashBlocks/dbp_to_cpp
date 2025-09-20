
#include "dbh_compiler.h"
#include <iostream>

int main() {
    std::cout << "DBH Compiler started!" << std::endl;
    

    DBH::Project game("MyScratchGame");
    

    DBH::Sprite cat("Cat");
    
    // LUSHi SPOSOB UBRAT BAG "//"
    // std::cout << "Creating sprite: " << cat.getName() << std::endl;
    
    std::cout << "Creating sprite: Cat" << std::endl;
    
    cat.move(100);
    cat.say("Hello!");
    
    game.addSprite(cat);
    
    if (game.saveToFile("my_game.sb3")) {
        std::cout << "Project saved successfully as my_game.sb3!" << std::endl;
    } else {
        std::cout << "Error saving project!" << std::endl;
        return 1;
    }
    
    return 0;
}