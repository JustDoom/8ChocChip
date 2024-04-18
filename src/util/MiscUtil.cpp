#include "MiscUtil.h"

std::vector<sf::Keyboard::Key> MiscUtil::keys;
std::vector<sf::Mouse::Button> MiscUtil::mouse;

std::string MiscUtil::toLowerCase(const std::string& string) {
    std::string result;

    for (char ch: string) {
        // Convert each character to lowercase using tolower
        result += tolower(ch);
    }

    return result;
}

std::string MiscUtil::getFileFromPath(std::string path) {
    size_t lastSlashPos = path.find_last_of('/');

    if (lastSlashPos != std::string::npos) {
        return path.substr(lastSlashPos + 1);
    }

    return path;
}

void MiscUtil::addKey(sf::Keyboard::Key key) {
    MiscUtil::keys.emplace_back(key);
}

void MiscUtil::removeKey(sf::Keyboard::Key key) {
    auto it = std::find(MiscUtil::keys.begin(), MiscUtil::keys.end(), key);
    if (it != MiscUtil::keys.end()) {
        MiscUtil::keys.erase(it);
    }
}

bool MiscUtil::isPressed(sf::Keyboard::Key key) {
    return std::count(MiscUtil::keys.begin(), MiscUtil::keys.end(), key);
}

void MiscUtil::addButton(sf::Mouse::Button button) {
    MiscUtil::mouse.emplace_back(button);
}

void MiscUtil::removeButon(sf::Mouse::Button button) {
    auto it = std::find(MiscUtil::mouse.begin(), MiscUtil::mouse.end(), button);
    if (it != MiscUtil::mouse.end()) {
        MiscUtil::mouse.erase(it);
    }
}

bool MiscUtil::isClicked(sf::Mouse::Button button) {
    return std::count(MiscUtil::mouse.begin(), MiscUtil::mouse.end(), button);
}