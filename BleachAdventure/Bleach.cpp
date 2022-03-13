#include<iostream>
#include<cstring>
#include<SDL.h>
#include<SDL_image.h>
//********************************************
const int Wi = 1200;
const int He = 670;
const int GoFrame = 7, StayFrame = 9, DeFendFrame = 9;
int posY,nY=0;
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
SDL_Rect Go[GoFrame + 1], Stay[StayFrame + 1], BackGround,Defend[DeFendFrame + 1], Scr = { 0,0,Wi,He };
SDL_RendererFlip flipType = SDL_FLIP_NONE;
//********************************************
//test github
class cTexture
{
public:
	cTexture();
	~cTexture();
	bool loadTexture(std::string path);
	void free();
	void setColor(Uint8 r, Uint8 g, Uint8 b);
	void setAlpha(Uint8 alpha);
	void setBlendMode(SDL_BlendMode blmode);
	void rendertex(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) {
		SDL_Rect ren = { x,y,TextureWidth,TextureHeight };
		if (clip != NULL)
		{
			ren.w = clip->w;
			ren.h = clip->h;
		}
		SDL_RenderCopyEx(renderer, cT, clip, &ren, angle, center, flip);
	}
	void renback(SDL_Rect* clip) {
		SDL_RenderCopy(renderer, cT, clip, &Scr);
	}
	int getWidth();
	int getHeight();
private:
	int TextureWidth, TextureHeight;
	SDL_Texture* cT;
};
cTexture mainTexture, backTexture;
class Animation
{
public:
	static const int picWidth = 75;
	static const int picHeight = 70;
	static const int picVel = 2;
	Animation();
	void handleEvent(SDL_Event* e, bool& go, SDL_RendererFlip& flipType, bool& runright, bool& runleft, bool& defend, bool& dfdown, int &Defend_Fr);
	void move(int& frame);
	void render(SDL_Rect* clip, SDL_RendererFlip& fliptype);
	int getX();
	int getY();
private:
	int picX, picY;
	int velX, velY;
};
Animation run;
//********************************************
cTexture::cTexture() {
	TextureWidth = 0;
	TextureHeight = 0;
	cT = NULL;
}
cTexture::~cTexture() {
	free();
}
void cTexture::free() {
	if (cT != NULL)
	{
		SDL_DestroyTexture(cT);
		cT = NULL;
		TextureWidth = 0;
		TextureHeight = 0;
	}
}
bool cTexture::loadTexture(std::string path) {
	free();
	SDL_Texture* newTex = NULL;
	SDL_Surface* loadedSur = IMG_Load(path.c_str());
	if (loadedSur == NULL) {
		std::cout << "Error: Can't load the image!" << std::endl << SDL_GetError() << std::endl;
	}
	else {
		Uint32 color_key = SDL_MapRGB(loadedSur->format, 7, 121, 141);
		SDL_SetColorKey(loadedSur, SDL_TRUE, color_key);
		newTex = SDL_CreateTextureFromSurface(renderer, loadedSur);
		if (newTex == NULL) {
			std::cout << "Error: Can't create texture!" << std::endl << SDL_GetError();
		}
		else {
			TextureWidth = loadedSur->w;
			TextureHeight = loadedSur->h;
		}
		SDL_FreeSurface(loadedSur);
	}
	cT = newTex;
	return cT != NULL;
}
void cTexture::setColor(Uint8 r, Uint8 g, Uint8 b) {
	SDL_SetTextureColorMod(cT, r, g, b);
}
void cTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(cT, alpha);
}
void cTexture::setBlendMode(SDL_BlendMode blmode) {
	SDL_SetTextureBlendMode(cT, blmode);
}
int cTexture::getWidth() {
	return TextureWidth;
}
int cTexture::getHeight() {
	return TextureHeight;
}
//---------------------------------------------
Animation::Animation() {
	picX = (Wi - picWidth) / 2;
	picY = (He - picHeight) / 2;
	velX = 0;
	velY = 0;
}
void Animation::handleEvent(SDL_Event* e, bool& go, SDL_RendererFlip& flipType, bool& runright, bool& runleft, bool& defend, bool&dfdown, int &Defend_Fr) {
	if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_a:
			velX -= picVel;
			go = true;
			//if (!left) {
			//	flipType = SDL_FLIP_HORIZONTAL;
			//	left = true;
			//}
			runleft = true;
			break;
		case SDLK_d:
			velX += picVel;
			go = true;
			//if (!runleft) { flipType = SDL_FLIP_NONE; left = false; }
			runright = true;
			break;
		case SDLK_s:
			defend = true;
			nY ++;
			break;
		}
		if (velX == 0) go = false;
		else if (velX == 2) flipType = SDL_FLIP_NONE; 
		else flipType = SDL_FLIP_HORIZONTAL;
		if (nY == 1) {
			posY = run.getY() - 15; 
		}
 	}
	else if (e->type == SDL_KEYUP && e->key.repeat == 0)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_a:
			velX += picVel;
			if (velX == 0) go = false;
			else go = true;
			runleft = false;
			break;
		case SDLK_d:
			velX -= picVel;
			if (velX == 0) go = false;
			else go = true;
			runright = false;
			break;
		case SDLK_s:
			Defend_Fr = 0;
			nY = 0;
			dfdown = true;
			defend = false;
			break;
		}
		if (!runleft && velX==2) flipType = SDL_FLIP_NONE;
		if (!runright && velX == -2) flipType = SDL_FLIP_HORIZONTAL;
	}
}
void Animation::move(int& frame) {
	picX += velX;
	frame++;
	if (picX < 0 || (picX + picWidth)>Wi) picX -= velX;
}
void Animation::render(SDL_Rect* clip, SDL_RendererFlip& flipType) {
	mainTexture.rendertex(picX, picY, clip, 0, NULL, flipType);
}
int Animation::getX() {
	return picX;
}
int Animation::getY() {
	return picY;
}
//********************************************
void init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Error: SDL can't be initialized!" << std::endl << SDL_GetError() << std::endl;
	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!";
	}
	window = SDL_CreateWindow("BleachAventure!!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Wi, He, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cout << "Can't create window" << std::endl << SDL_GetError() << std::endl;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		std::cout << "Can't create renderer" << std::endl << SDL_GetError() << std::endl;
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	int iF = IMG_INIT_PNG;
	if (!(IMG_Init(iF) & iF)) {
		std::cout << "SDL_image could not initialize!" << std::endl << IMG_GetError() << std::endl;
	}
}
void loadMedia() {
	if (!mainTexture.loadTexture("environment/Ichigo Kurosaki.png")) {
		std::cout << "Unable to load the image" << std::endl;
	}
	if (!backTexture.loadTexture("environment/Bleach Stage.png")) {
		std::cout << "Unable to load the background image" << std::endl;
	}
	else {
		int k = -75;
		for (int i = 0; i < 2; i++) {
			k += 85;
			Go[i].x = k;
			Go[i].y = 130;
			Go[i].w = 75;
			Go[i].h = 45;
		}
		Go[2].x = 175;
		Go[2].y = 130;
		Go[2].w = 75;
		Go[2].h = 45;

		Go[3].x = 260;
		Go[3].y = 130;
		Go[3].w = 80;
		Go[3].h = 45;

		Go[4].x = 345;
		Go[4].y = 130;
		Go[4].w = 75;
		Go[4].h = 45;

		Go[5].x = 430;
		Go[5].y = 130;
		Go[5].w = 75;
		Go[5].h = 45;

		Go[6].x = 515;
		Go[6].y = 130;
		Go[6].w = 75;
		Go[6].h = 45;

		Go[7].x = 600;
		Go[7].y = 130;
		Go[7].w = 75;
		Go[7].h = 45;

		int t = -65;
		for (int i = 0; i < 2; i++) {
			t += 75;
			Stay[i].x = t;
			Stay[i].y = 45;
			Stay[i].w = 70;
			Stay[i].h = 45;
		}
		Stay[2] = Stay[1];
		Stay[3] = Stay[1];

		Stay[4].x = 170;
		Stay[4].y = 45;
		Stay[4].w = 70;
		Stay[4].h = 45;

		Stay[5] = Stay[4];
		Stay[6] = Stay[4];

		Stay[7].x = 255;
		Stay[7].y = 45;
		Stay[7].w = 70;
		Stay[7].h = 45;
		Stay[8] = Stay[7];
		Stay[9] = Stay[7];

		BackGround.x = 2;
		BackGround.y = 2;
		BackGround.w = 320;
		BackGround.h = 470;

		Defend[0].x = 375;
		Defend[0].y = 45;
		Defend[0].w = 60;
		Defend[0].h = 45;

		Defend[1].x = 375;
		Defend[1].y = 45;
		Defend[1].w = 60;
		Defend[1].h = 45;

		Defend[2].x = 375;
		Defend[2].y = 45;
		Defend[2].w = 60;
		Defend[2].h = 45;

		Defend[3].x = 375;
		Defend[3].y = 45;
		Defend[3].w = 60;
		Defend[3].h = 45;

		Defend[4].x = 375;
		Defend[4].y = 45;
		Defend[4].w = 60;
		Defend[4].h = 45;

		Defend[5].x = 375;
		Defend[5].y = 45;
		Defend[5].w = 60;
		Defend[5].h = 45;

		Defend[6].x = 375;
		Defend[6].y = 45;
		Defend[6].w = 60;
		Defend[6].h = 45;

		Defend[7].x = 375;
		Defend[7].y = 45;
		Defend[7].w = 60;
		Defend[7].h = 45;

		Defend[8].x = 440;
		Defend[8].y = 30;
		Defend[8].w = 50;
		Defend[8].h = 60;

		Defend[9].x = 498;
		Defend[9].y = 46;
		Defend[9].w = 60;
		Defend[9].h = 45;
	}
}
void close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = NULL;
	window = NULL;
	backTexture.free();
	mainTexture.free();
	SDL_Quit();
	IMG_Quit();
}
void makeanimation(int& Stay_Fr, int& Run_Fr, int &Defend_Fr, bool go, bool defend, bool &dfdown) {
	SDL_Rect* current;
	if (defend) {
		if (nY <= 8) { current = &Defend[nY - 1]; nY++; }
		else current = &Defend[8];
		if (current == &Defend[8]) {
			mainTexture.rendertex(run.getX(), posY, current, 0.0, NULL, flipType);
		}
		else mainTexture.rendertex(run.getX(), run.getY(), current, 0.0, NULL, flipType);
	}
	else if (go) {
		run.move(Run_Fr);
		current = &Go[Run_Fr / 8];
		if (Run_Fr / GoFrame >= 8) Run_Fr = 0;
		run.render(current, flipType);
		SDL_Delay(5);
	}
	else if (dfdown) {
		current = &Defend[9];
		mainTexture.rendertex(run.getX(), run.getY(), current, 0.0, NULL, flipType);
		dfdown = false;
	}
	else if (!go&&!defend&&!dfdown) {
		current = &Stay[Stay_Fr / 10];
		Stay_Fr++;
		if (Stay_Fr / StayFrame >= 10) Stay_Fr = 0;
		mainTexture.rendertex(run.getX(), run.getY(), current, 0.0, NULL, flipType);
		SDL_Delay(3);
	}
}
//********************************************
int main(int argc, char* argv[]) {
	init();
	loadMedia();
	bool quit = false;
	bool go = false, runright = false, runleft = false, defend = false, dfdown = false;
	SDL_Event e;
	int Run_Fr = 0;
	int Stay_Fr = 0;
	int Defend_Fr = 0;
	while (!quit) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) quit = true;
			run.handleEvent(&e, go, flipType, runright, runleft, defend, dfdown, Defend_Fr );
		}
		SDL_RenderClear(renderer);
		backTexture.renback(&BackGround);
		makeanimation(Stay_Fr, Run_Fr, Defend_Fr, go, defend, dfdown);
		SDL_RenderPresent(renderer);
	}
	close();
	return 0;
}
