#pragma once
#include <glm/glm.hpp>
#include <string>

class Shader {
  private:
      unsigned int ID;

      void checkCompileErrors(unsigned int shader, std::string type);

  public:

      Shader(const char* vertexPath, const char* fragmentPath);

      void use() const;
 
      // Uniform helpers
      void setBool(const std::string& name, bool value) const;
      void setInt(const std::string& name, int value) const;
      void setFloat(const std::string& name, float value) const;
      void setVec3(const std::string& name, float x, float y, float z) const;
      void setVec4(const std::string& name, float x, float y, float z, float w) const;
      void setMat4(const std::string& name, const glm::mat4& matrix, const std::string& entity_name) const;

      void dynamicColorUse() const;

      unsigned int getID() const;

};
