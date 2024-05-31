/*
* OpenGL Burger Time
*
*/
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <array>
#include <memory>
#include <math.h>		
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Glut.h"
#include "stb_image.h"



class platform {
public:
	float startX;
	float endX;
	float posY;
	platform(float sx, float ex, float y) {
		startX = sx;
		endX = ex;
		posY = y;
	}
};

class ladder {
public:
	float startX;
	float endX;
	float topY;
	float botY;

	ladder(float sx, float ex, float by, float ty) {
		startX = sx;
		endX = ex;
		topY = ty;
		botY = by;
	}
};
class player {
public:
	float posX;
	float posY;
	float width = 8;
	float height = 8;
	player(float x, float y) {
		this->posX = x;
		this->posY = y;
	};
	// Function to check collision with a ladder
	bool isCollidingWithLadder(const ladder& l) const {
		// Check if the player's bounding box overlaps with the ladder's bounding box

		float playerFeetY = posY - height / 2;

		// Check if the player's feet are within the ladder's bounds
		if (posX >= l.startX && posX <= l.endX &&
			playerFeetY <= l.topY && playerFeetY >= l.botY) {
			return true; // Collision with ladder detected
		}
		return false; // No collision with ladder
	}
	bool isCollidingWithPlatform(const platform& p) const {

		float playerFeetY = posY - height / 2;

		if (posX >= p.startX && posX <= p.endX &&
			playerFeetY <= p.posY + 2 && playerFeetY >= p.posY -2) {
			return true; // Collision with platform detected
		}
		return false; // No collision platform ladder
	}
};

class droppableObj {
public:
	float posX;
	float posY;
	float part1x;
	float part2x;
	float part3x;
	float part4x;
	float part1y;
	float part2y;
	float part3y;
	float part4y;
	bool falling = false;
	bool canfall = true;
	int texType;
	int texPart;
	droppableObj(float x, float y, bool cf)
	{
		this->posX = x;
		this->posY = y;
		this->part1x = x;
		this->part1y = y;
		this->part2x = x + 5;
		this->part2y = y;
		this->part3x = x + 10;
		this->part3y = y;
		this->part4x = x + 15;
		this->part4y = y;
		this->canfall = cf;
	};
	void setNext(std::shared_ptr<droppableObj> nextObj) {
		next = nextObj;
	}
	std::shared_ptr<droppableObj> next;
};

const double Xmin = 0.0, Xmax = 150.0;
const double Ymin = 0.0, Ymax = 180.0;


//object inits
droppableObj burg1top = droppableObj(5, 90,true);
droppableObj burg1bot = droppableObj(5, 30, false);
droppableObj burg1pat = droppableObj(5, 60, true);
droppableObj burg2top = droppableObj(35, 120, true);
droppableObj burg2bot = droppableObj(35, 60, true);
droppableObj burg2pat = droppableObj(35, 90, true);
droppableObj burg3top = droppableObj(102, 120, true);
droppableObj burg3bot = droppableObj(102, 60, true);
droppableObj burg3pat = droppableObj(102, 90, true);
droppableObj burg4top = droppableObj(132, 90, true);
droppableObj burg4bot = droppableObj(132, 30, false);
droppableObj burg4pat = droppableObj(132, 60, true);
player playerchar = player(90, 64);


std::vector<ladder> ladders;
std::vector<platform> platforms;

//Textures
GLuint playerTex;
GLuint bun1p1;
GLuint bun1p2;
GLuint bun1p3;
GLuint bun1p4;
GLuint bun2p1;
GLuint bun2p2;
GLuint bun2p3;
GLuint bun2p4;
GLuint patty1;
GLuint patty2;
GLuint patty3;
GLuint patty4;


void drawChar(float cx, float cy,int charType,int charState, player &mychar)
{
	int width = mychar.width;
	int height = mychar.height;
	glEnable(GL_TEXTURE_2D);
	switch (charType)
	{
		case 0: {
			glBindTexture(GL_TEXTURE_2D, playerTex);
			glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glTexCoord2f(0, 1);
			glVertex2f(cx - width / 2, cy + height / 2);
			glTexCoord2f(1, 1);
			glVertex2f(cx + width / 2, cy + height / 2);
			glTexCoord2f(1, 0);
			glVertex2f(cx + width / 2, cy - height / 2);
			glTexCoord2f(0, 0);
			glVertex2f(cx - width / 2, cy - height / 2);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
void drawPlate(float x, float y)
{
	float startx = x;
	float topy = y;
	float boty = y - 3;
	glBegin(GL_LINE_LOOP);
	glColor3f(1, 1, 1);
	glVertex2f(startx - 1, topy);
	glVertex2f(startx, topy);
	glVertex2f(startx, boty);
	glVertex2f(startx + 24, boty);
	glVertex2f(startx + 24, topy);
	glVertex2f(startx + 25, topy);
	glVertex2f(startx + 25, boty - 1);
	glVertex2f(startx - 1, boty - 1);
	glEnd();
}
void drawPlatform(float sx, float ex, float y)
{
	float startx = sx;
	float endx = ex;
	float topy = y;
	float boty = y - 1.5;
	glBegin(GL_QUADS);
	glColor3f(0.5, 0.5, 1);
	glVertex2f(startx, topy);
	glVertex2f(endx, topy);
	glVertex2f(endx, boty);
	glVertex2f(startx, boty);
	glEnd();
}
void drawLadder(float sx, float ex, int by, int ty)
{
	float startx = sx;
	float endx = ex;
	float topy = ty;
	float boty = by;
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	for (float i = boty; i <= topy; i += 2)
	{
		glVertex2f(startx, i);
		glVertex2f(endx, i);
		glVertex2f(endx, i-1);
		glVertex2f(startx, i-1);
	}
	glEnd();
}
void drawDroppable(droppableObj &obj, GLuint tex1, GLuint tex2, GLuint tex3, GLuint tex4)
{
	int width = 6;
	int height = 4;
	float cx = obj.posX + width / 2;
	float initialY = obj.posY;
	if (obj.falling)
	{
		obj.part1y -= 0.0025;
		obj.part2y -= 0.0025;
		obj.part3y -= 0.0025;
		obj.part4y -= 0.0025;
	}
	float y1 = obj.part1y;
	float y2 = obj.part2y;
	float y3 = obj.part3y;
	float y4 = obj.part4y;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	if (playerchar.posY - playerchar.height / 2 == initialY && abs(int(playerchar.posX - cx)) <= width / 2 && y1> (initialY - height/2) && obj.canfall)
	{
		y1 -= 0.0025;
		obj.part1y = y1;
	}
	glTexCoord2f(0, 1);
	glVertex2f(cx - width / 2, y1 + height /2 );
	glTexCoord2f(1, 1);
	glVertex2f(cx + width / 2, y1 + height / 2);
	glTexCoord2f(1, 0);
	glVertex2f(cx + width / 2, y1 - height/2);
	glTexCoord2f(0, 0);
	glVertex2f(cx - width / 2, y1 - height/2);
	glDisable(GL_TEXTURE_2D);
	glEnd();
	cx += width;
	//part2
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	if (playerchar.posY - playerchar.height / 2 == initialY && abs(int(playerchar.posX - cx)) <= width / 2 && y2 > (initialY - height / 2) && obj.canfall)
	{
		y2 -= 0.0025;
		obj.part2y = y2;
	}
	glTexCoord2f(0, 1);
	glVertex2f(cx - width / 2, y2 + height / 2);
	glTexCoord2f(1, 1);
	glVertex2f(cx + width / 2, y2 + height / 2);
	glTexCoord2f(1, 0);
	glVertex2f(cx + width / 2, y2 - height / 2);
	glTexCoord2f(0, 0);
	glVertex2f(cx - width / 2, y2 - height / 2);
	glDisable(GL_TEXTURE_2D);
	glEnd();
	cx += width;
	//part3
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex3);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	if (playerchar.posY - playerchar.height / 2 == initialY && abs(int(playerchar.posX - cx)) <= width / 2 && y3 > (initialY - height / 2) && obj.canfall)
	{
		y3 -= 0.0025;
		obj.part3y = y3;
	}
	glTexCoord2f(0, 1);
	glVertex2f(cx - width / 2, y3 + height / 2);
	glTexCoord2f(1, 1);
	glVertex2f(cx + width / 2, y3 + height / 2);
	glTexCoord2f(1, 0);
	glVertex2f(cx + width / 2, y3 - height / 2);
	glTexCoord2f(0, 0);
	glVertex2f(cx - width / 2, y3 - height / 2);
	glDisable(GL_TEXTURE_2D);
	glEnd();
	cx += width;
	//part4
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex4);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	if (playerchar.posY - playerchar.height / 2 == initialY && abs(int(playerchar.posX - cx)) <= width / 2 && y4 > (initialY - height / 2) && obj.canfall)
	{
		y4 -= 0.0025;
		obj.part4y = y4;
	}
	glTexCoord2f(0, 1);
	glVertex2f(cx - width / 2, y4 + height / 2);
	glTexCoord2f(1, 1);
	glVertex2f(cx + width / 2, y4 + height / 2);
	glTexCoord2f(1, 0);
	glVertex2f(cx + width / 2, y4 - height / 2);
	glTexCoord2f(0, 0);
	glVertex2f(cx - width / 2, y4 - height / 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	if ((initialY - y1) >= 2 && (initialY - y2) >= 2 && (initialY - y3) >= 2 && (initialY - y4) >= 2 && obj.canfall)
	{
		obj.falling = true;
	}
}

void myKeyboardFunc(unsigned char key, int x, int y)
{


}

void mySpecialKeyFunc(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_RIGHT:
		{
			float platformstart;
			float platformend;
			float platformy;
			bool collisionDetected = false;
			for (const platform& p : platforms) {
				if (playerchar.isCollidingWithPlatform(p)) {
					collisionDetected = true;
					platformstart = p.startX;
					platformend = p.endX;
					platformy = p.posY;
					break; // Exit the loop as soon as a collision is detected
				}
			}
			if (collisionDetected && playerchar.posX + playerchar.width / 2 < platformend) {
				playerchar.posX += 1;
				playerchar.posY = platformy + playerchar.height / 2;
			}
			break;
		}
		case GLUT_KEY_LEFT:
		{
			float platformstart;
			float platformend;
			float platformy;
			bool collisionDetected = false;
			for (const platform& p : platforms) {
				if (playerchar.isCollidingWithPlatform(p)) {
					collisionDetected = true;
					platformstart = p.startX;
					platformend = p.endX;
					platformy = p.posY;
					break; // Exit the loop as soon as a collision is detected
				}
			}
			if (collisionDetected && playerchar.posX - playerchar.width / 2 > platformstart) {
				playerchar.posX -= 1;
				playerchar.posY = platformy + playerchar.height/2;
			}
			break;
		}
		case GLUT_KEY_UP:
		{
			float laddertop;
			float ladderbot;
			bool collisionDetected = false;
			for (const ladder& l : ladders) {
				if (playerchar.isCollidingWithLadder(l)) {
					collisionDetected = true;
					laddertop = l.topY;
					ladderbot = l.botY;
					break; // Exit the loop as soon as a collision is detected
				}
			}
			if (collisionDetected && playerchar.posY - playerchar.height / 2 < laddertop) {
				playerchar.posY += 1;
			}
			break;
		}
		case GLUT_KEY_DOWN:
		{
			float laddertop;
			float ladderbot;
			bool collisionDetected = false;
			for (const ladder& l : ladders) {
				if (playerchar.isCollidingWithLadder(l)) {
					collisionDetected = true;
					laddertop = l.topY;
					ladderbot = l.botY;
					break; // Exit the loop as soon as a collision is detected
				}
			}
			if (collisionDetected && playerchar.posY - playerchar.height/2 > ladderbot) {
				playerchar.posY -= 1;
			}
			break;
		}
	}
}

GLuint loadTexture(const char* filename) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);

	if (image) {
		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image);
	}
	else {
		printf("Failed to load texture: %s\n", filename);
		stbi_image_free(image);
		return 0;
	}

	return textureID;
}


//SCENE
//SCENE
//SCENE
//SCENE
///////////////////////////////////////////////
void drawScene(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawChar(playerchar.posX, playerchar.posY,0,0, playerchar);
	drawDroppable(burg1top, bun1p1, bun1p2, bun1p3, bun1p4);
	drawDroppable(burg1pat, patty1, patty2, patty3, patty4);
	drawDroppable(burg1bot, bun2p1, bun2p2, bun2p3, bun2p4);

	drawDroppable(burg2top, bun1p1, bun1p2, bun1p3, bun1p4);
	drawDroppable(burg2pat, patty1, patty2, patty3, patty4);
	drawDroppable(burg2bot, bun2p1, bun2p2, bun2p3, bun2p4);

	drawDroppable(burg3top, bun1p1, bun1p2, bun1p3, bun1p4);
	drawDroppable(burg3pat, patty1, patty2, patty3, patty4);
	drawDroppable(burg3bot, bun2p1, bun2p2, bun2p3, bun2p4);

	drawDroppable(burg4top, bun1p1, bun1p2, bun1p3, bun1p4);
	drawDroppable(burg4pat, patty1, patty2, patty3, patty4);
	drawDroppable(burg4bot, bun2p1, bun2p2, bun2p3, bun2p4);

	drawPlate(5, 30);
	drawPlate(35, 5);
	drawPlate(102, 5);
	drawPlate(132, 30);

	drawPlatform(62, 99, 5);
	drawPlatform(3, 158, 60);
	drawPlatform(3, 158, 90);
	drawPlatform(3, 158, 120);
	drawPlatform(32, 128, 30);

	drawLadder(65, 75, 5, 30);
	drawLadder(87, 97, 5, 30);
	drawLadder(42, 52, 30, 90);
	drawLadder(12, 22, 60, 120);
	drawLadder(109, 119, 30, 90);
	drawLadder(139, 149, 60, 120);
	drawLadder(76, 86, 60, 120);

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

/// 
/// //////////////////////////////////////////////////////////////////
/// 
void initRendering()
{
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);
}

void resizeWindow(int w, int h)
{
	double scale, center;
	double windowXmin, windowXmax, windowYmin, windowYmax;

	glViewport(0, 0, w, h);

	w = (w == 0) ? 1 : w;
	h = (h == 0) ? 1 : h;
	if ((Xmax - Xmin) / w < (Ymax - Ymin) / h) {
		scale = ((Ymax - Ymin) / h) / ((Xmax - Xmin) / w);
		center = (Xmax + Xmin) / 2;
		windowXmin = center - (center - Xmin) * scale;
		windowXmax = center + (Xmax - center) * scale;
		windowYmin = Ymin;
		windowYmax = Ymax;
	}
	else {
		scale = ((Xmax - Xmin) / w) / ((Ymax - Ymin) / h);
		center = (Ymax + Ymin) / 2;
		windowYmin = center - (center - Ymin) * scale;
		windowYmax = center + (Ymax - center) * scale;
		windowXmin = Xmin;
		windowXmax = Xmax;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(windowXmin, windowXmax, windowYmin, windowYmax, -1, 1);

}

int main(int argc, char** argv)
{
	// Add ladder objects to the vector
	ladders.push_back(ladder(65, 75, 5, 30));
	ladders.push_back(ladder(87, 97, 5, 30));
	ladders.push_back(ladder(42, 52, 30, 90));
	ladders.push_back(ladder(12, 22, 60, 120));
	ladders.push_back(ladder(109, 119, 30, 90));
	ladders.push_back(ladder(139, 149, 60, 120));
	ladders.push_back(ladder(76, 86, 60, 120));
	// Add platform objects to the vector
	platforms.push_back(platform(62, 99, 5));
	platforms.push_back(platform(3, 158, 60));
	platforms.push_back(platform(3, 158, 90));
	platforms.push_back(platform(3, 158, 120));
	platforms.push_back(platform(32, 128, 30));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(10, 60);
	glutInitWindowSize(720, 720);
	
	glutCreateWindow("Burger time");
	playerTex = loadTexture("texture/char/chef_d_0.png");
	bun1p1 = loadTexture("texture/obj/bun1p1.png");
	bun1p2 = loadTexture("texture/obj/bun1p2.png");
	bun1p3 = loadTexture("texture/obj/bun1p3.png");
	bun1p4 = loadTexture("texture/obj/bun1p4.png");
	bun2p1 = loadTexture("texture/obj/bun2p1.png");
	bun2p2 = loadTexture("texture/obj/bun2p2.png");
	bun2p3 = loadTexture("texture/obj/bun2p3.png");
	bun2p4 = loadTexture("texture/obj/bun2p4.png");
	patty1 = loadTexture("texture/obj/patty1.png");
	patty2 = loadTexture("texture/obj/patty2.png");
	patty3 = loadTexture("texture/obj/patty3.png");
	patty4 = loadTexture("texture/obj/patty4.png");
	if (playerTex == 0) {
		return 1;
	}
	initRendering();
	glutKeyboardFunc(myKeyboardFunc);
	glutSpecialFunc(mySpecialKeyFunc);
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(drawScene);
	glutMainLoop();
	return(0);
}

