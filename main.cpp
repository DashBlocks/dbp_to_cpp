#include "dbh_compiler.h"
#include <iostream>

int main() {
    DBH::Project game("MyGame");
    DBH::Sprite cat("Спрайт 1");
    
    cat.move(10);
    cat.turnRight(15);
    cat.say("Привет!");
    cat.show();
    
    game.addSprite(cat);
    
    if (game.saveToFile("test")) {
        std::cout << "Project saved as test.json!" << std::endl;
    }
    
    return 0;
}