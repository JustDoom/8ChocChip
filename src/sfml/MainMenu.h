#ifndef INC_8CHOCCHIP_MAINMENU_H
#define INC_8CHOCCHIP_MAINMENU_H

#include <thread>
#include <unordered_map>
#include <utility>

#include "SFML/Graphics.hpp"

#include "InputHandler.h"

class MainMenu {
private:
    sf::RenderWindow window;
    std::vector<std::thread*>& windows;

    std::vector<std::string>& romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>>& romFiles;

    InputHandler inputHandler;
public:
    MainMenu(std::unordered_map<std::string *, std::vector<std::string>>& romFiles,
             std::vector<std::string>& romDirectories, std::vector<std::thread *>& windows,
             std::string configFilePath);
};


#endif // INC_8CHOCCHIP_MAINMENU_H
