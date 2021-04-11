#pragma once
#include "imageloader.h"
#include <string>

#include <array>
#include <memory>

enum class SkyboxSide: std::size_t {
  NegX = 0,
  NegY = 1,
  NegZ = 2,
  PosX = 3,
  PosY = 4,
  PosZ = 5
};

class Skybox {
  private:
    std::array<GLuint, 6> skybox {};
    std::array<std::unique_ptr<Image>, 6> images {};

    unsigned int skybox_to_use;

    /**
     * Retrieve image pointer with compile time index checking.
     */
    template<SkyboxSide side>
    std::unique_ptr<Image> & getImage() {
      return std::get<
        static_cast<std::size_t>(side)
      >(images);
    }

    /**
     * Retrieve skybox side texture with compile time index checking.
     */
    template<SkyboxSide side>
    GLuint & getTexture() {
      return std::get<
        static_cast<std::size_t>(side)
      >(skybox);
    }

    template<SkyboxSide side>
    void loadPart(std::string const & file) {
      getImage<side>() = std::unique_ptr<Image>(loadBMP(file.c_str()));
      getTexture<side>() = loadTexture(getImage<side>().get());
    }
 
  public:
    Skybox(unsigned int used_box);
    ~Skybox();

    void loadSkybox();

    void displaySkybox();
};

