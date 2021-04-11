#include "TextureStore.h"

TextureStore::~TextureStore() {
	glDeleteTextures(handles.size(), handles.data());

  for (auto bitmap_ptr : bitmaps) {
    delete bitmap_ptr;
  }
}

GLuint TextureStore::loadTexture(const char * const filePath) {
  bitmaps.push_back(loadBMP(filePath));
  handles.push_back(::loadTexture(bitmaps.back()));

  return handles.back();
}
