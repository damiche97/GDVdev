#include "Skybox.h"

#include <iostream>

Skybox::Skybox(unsigned int used_box):skybox_to_use(used_box)
{ }

Skybox::~Skybox()
{
  std::cout << "Deleting textuures" << std::endl;
	glDeleteTextures(skybox.size(), skybox.data());
}

void Skybox::loadSkybox()
{
	std::string dest;
	switch (skybox_to_use) {
	case 1:
		dest = "Textures/skybox1/";
	case 2:
		dest = "Textures/skybox2/";
	case 3:
		dest = "Textures/skybox3/";
	}

	loadPart<SkyboxSide::NegX>(dest + "neg_x.bmp");
	loadPart<SkyboxSide::NegY>(dest + "neg_y.bmp");
	loadPart<SkyboxSide::NegZ>(dest + "neg_z.bmp");
	loadPart<SkyboxSide::PosX>(dest + "pos_x.bmp");
	loadPart<SkyboxSide::PosY>(dest + "pos_y.bmp");
	loadPart<SkyboxSide::PosZ>(dest + "pos_z.bmp");
}

void Skybox::displaySkybox()
{
	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

  glEnable(GL_DEPTH_CLAMP); // ask opengl not to clip skybox
  glDepthMask(false); // dont write depth values, so that the skybox is never rendered in front of anything

	// Just in case we set all vertices to white.
	glColor4f(1, 1, 1, 1);

	// Render the front quad (NegZ)
	glBindTexture(GL_TEXTURE_2D, getTexture<SkyboxSide::NegZ>());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex4f(0.5f, -0.5f, -0.5f, 0.f); // set 4th coordinate always to 0, so that the skybox is infinitely far away
	glTexCoord2f(1, 0); glVertex4f(-0.5f, -0.5f, -0.5f, 0.f);
	glTexCoord2f(1, 1); glVertex4f(-0.5f, 0.5f, -0.5f, 0.f);
	glTexCoord2f(0, 1); glVertex4f(0.5f, 0.5f, -0.5f, 0.f);
	glEnd();

	// Render the left quad (PosX)
	glBindTexture(GL_TEXTURE_2D, getTexture<SkyboxSide::PosX>());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex4f(0.5f, -0.5f, 0.5f, 0.f);
	glTexCoord2f(1, 0); glVertex4f(0.5f, -0.5f, -0.5f, 0.f);
	glTexCoord2f(1, 1); glVertex4f(0.5f, 0.5f, -0.5f, 0.f);
	glTexCoord2f(0, 1); glVertex4f(0.5f, 0.5f, 0.5f, 0.f);
	glEnd();

	// Render the back quad (PosZ)
	glBindTexture(GL_TEXTURE_2D, getTexture<SkyboxSide::PosZ>());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex4f(-0.5f, -0.5f, 0.5f, 0.f);
	glTexCoord2f(1, 0); glVertex4f(0.5f, -0.5f, 0.5f, 0.f);
	glTexCoord2f(1, 1); glVertex4f(0.5f, 0.5f, 0.5f, 0.f);
	glTexCoord2f(0, 1); glVertex4f(-0.5f, 0.5f, 0.5f, 0.f);

	glEnd();

	// Render the right quad (NegX)
	glBindTexture(GL_TEXTURE_2D, getTexture<SkyboxSide::NegX>());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex4f(-0.5f, -0.5f, -0.5f, 0.f);
	glTexCoord2f(1, 0); glVertex4f(-0.5f, -0.5f, 0.5f, 0.f);
	glTexCoord2f(1, 1); glVertex4f(-0.5f, 0.5f, 0.5f, 0.f);
	glTexCoord2f(0, 1); glVertex4f(-0.5f, 0.5f, -0.5f, 0.f);
	glEnd();

	// Render the top quad (PosY)
	glBindTexture(GL_TEXTURE_2D, getTexture<SkyboxSide::PosY>());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex4f(-0.5f, 0.5f, -0.5f, 0.f);
	glTexCoord2f(1, 0); glVertex4f(-0.5f, 0.5f, 0.5f, 0.f);
	glTexCoord2f(1, 1); glVertex4f(0.5f, 0.5f, 0.5f, 0.f);
	glTexCoord2f(0, 1); glVertex4f(0.5f, 0.5f, -0.5f, 0.f);
	glEnd();

	// Render the bottom quad (NegY)
	glBindTexture(GL_TEXTURE_2D, getTexture<SkyboxSide::NegY>());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex4f(0.5f, -0.5f, -0.5f, 0.f);
	glTexCoord2f(1, 0); glVertex4f(0.5f, -0.5f, 0.5f, 0.f);
	glTexCoord2f(1, 1); glVertex4f(-0.5f, -0.5f, 0.5f, 0.f);
	glTexCoord2f(0, 1); glVertex4f(-0.5f, -0.5f, -0.5f, 0.f);
	glEnd();

  glDepthMask(true);

	// Restore enable bits and matrix
	glPopAttrib();
	glPopMatrix();
}
