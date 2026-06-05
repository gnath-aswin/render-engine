#pragma once
#include <string>

class Texture {
private:
    unsigned int ID;
    std::string type;
    std::string path;

public:
    Texture(const char* imagePath, const std::string& textureType = "texture_diffuse");
    ~Texture();

    void bind(unsigned int unit = 0) const;

    unsigned int getID() const;
    const std::string& getType() const;
    const std::string& getPath() const;
};

