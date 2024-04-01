#ifndef INC_8CHOCCHIP_MAINMENU_H
#define INC_8CHOCCHIP_MAINMENU_H

#include "SFML/Graphics.hpp"

#include <unordered_map>
#include <thread>
#include <utility>

class MainMenu {
private:
    sf::RenderWindow window;

    std::vector<std::string>& romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>>& romFiles;
    std::vector<std::thread*>& windows;
public:
    MainMenu(std::unordered_map<std::string *, std::vector<std::string>>& romFiles,
             std::vector<std::string>& romDirectories, std::vector<std::thread *>& windows,
             std::string configFilePath);
};


#endif // INC_8CHOCCHIP_MAINMENU_H
