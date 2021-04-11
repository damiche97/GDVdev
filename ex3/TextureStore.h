#ifndef TEXTURE_STORE_H
#define TEXTURE_STORE_H

#include <vector>

#include <GL/glew.h>
#include "imageloader.h"

class TextureStore {
  private:
    std::vector<Image *> bitmaps;
    std::vector<GLuint> handles;

  public:
    ~TextureStore();

    GLuint loadTexture(const char * const filePath);
};

#endif
