#include "PlayMode.hpp"
#include <cmath>

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include "load_save_png.hpp"
#include <random>
using namespace std;

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

	{ //use tiles 0-16 as some weird dot pattern thing:
		std::array< uint8_t, 8*8 > distance;
		for (uint32_t y = 0; y < 8; ++y) {
			for (uint32_t x = 0; x < 8; ++x) {
				float d = glm::length(glm::vec2((x + 0.5f) - 4.0f, (y + 0.5f) - 4.0f));
				d /= glm::length(glm::vec2(4.0f, 4.0f));
				distance[x+8*y] = uint8_t(std::max(0,std::min(255,int32_t( 255.0f * d ))));
			}
		}
		for (uint32_t index = 0; index < 16; ++index) {
			PPU466::Tile tile;
			uint8_t t = uint8_t((255 * index) / 16);
			for (uint32_t y = 0; y < 8; ++y) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (uint32_t x = 0; x < 8; ++x) {
					uint8_t d = distance[x+8*y];
					if (d > t) {
						bit0 |= (1 << x);
					} else {
						bit1 |= (1 << x);
					}
				}
				tile.bit0[y] = bit0;
				tile.bit1[y] = bit1;
			}
			ppu.tile_table[index] = tile;
		}
	}

	glm::uvec2 sizeSprite(8,8);	

	//load the player sprite and its palette
	string playerName = "../Asset/player.png";
	vector<glm::u8vec4> playerData;
	load_png(playerName, &sizeSprite, &playerData, LowerLeftOrigin);

	//load the zombie sprite and its palette
	string zombieName = "../Asset/zombie_test.png";
	vector<glm::u8vec4> zombieData;
	load_png(zombieName, &sizeSprite, &zombieData, LowerLeftOrigin);

	//load the tower sprite and its palette
	string towerName = "../Asset/tower.png";
	vector<glm::u8vec4> towerData;
	load_png(towerName, &sizeSprite, &towerData, LowerLeftOrigin);
	

	//palette for player
	ppu.palette_table[7] = {
		//manually using eyedropper tool
		glm::u8vec4(172, 50, 50, 255),
		glm::u8vec4(238, 195,154, 255),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		
	};
	//palette for zombie
	ppu.palette_table[4] = {
		glm::u8vec4(55, 148, 110, 255),
		glm::u8vec4(185, 0, 0, 255),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};
	//palette for tower
	ppu.palette_table[5] = {
		glm::u8vec4(102, 298, 29, 255),
		glm::u8vec4(40, 37, 19, 255),
		glm::u8vec4(223, 113, 38, 255),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};


/**************************************************************************************
*    Title: transfering png data(vector) to tile data
*    Credit: inspired by Haoze(Jacky) Sun
*    Comment: We are discussing about how to transfer a vector data to tile(using bit),
				he come out with this idea and I also use this logic for my work
***************************************************************************************/



//create tiles for zombies
for(int i = 0; i < 8; i++){
	for(int j = 0; j < 8; j++){
		int index = i*8 + j;
		if(i == 0 && j == 0){
			//cout <<"r:" <<  (int)zombieData[index].r  << " "<< "g: " << (int)zombieData[index].g <<" " << "b: "<< (int)zombieData[index].b << " " <<"a: "<< (int)zombieData[index].a << endl;
			//cout <<"r:" <<  (int)ppu.palette_table[7][3].r << endl << "g: " << (int)ppu.palette_table[7][3].g << endl << "b: "<< (int)ppu.palette_table[7][3].b << endl <<"a: "<< (int)ppu.palette_table[7][3].a << endl;
		}
		if(zombieData[index] == ppu.palette_table[4][0]){
			// if(j == 0 && i == 0){
			// 	cout << "green" << endl;
			// }
			zombieTile.bit0[i] &= ~(1 << j);
			zombieTile.bit1[i] &= ~(1 << j);
		}
		else if(zombieData[index] == ppu.palette_table[4][1]){
			// if(j == 0 && i == 0){
			// 	cout << "red" << endl;
			// }
			zombieTile.bit0[i] |= (1 << j);
			zombieTile.bit1[i] &= ~(1 << j);
		}
		else if(zombieData[index] == ppu.palette_table[4][2]){
			// if(j == 0 && i == 0){
			// 	cout << "black" << endl;
			// }
			zombieTile.bit0[i] &= ~(1 << j);
			zombieTile.bit1[i] |= (1 << j);
		}
		else if(zombieData[index] == ppu.palette_table[4][3]){
			// if(j == 0 && i == 0){
			// 	cout << "transparent" << endl;
			// }
			zombieTile.bit0[i] |= (1 << j);
			zombieTile.bit1[i] |= (1 << j);
		}
	}
}

//create tiles for player
for(int i = 0; i < 8; i++){
	for(int j = 0; j < 8; j++){
		int index = i*8 + j;
		if(i == 0 && j == 0){
			//cout <<"r:" <<  (int)zombieData[index].r  << " "<< "g: " << (int)zombieData[index].g <<" " << "b: "<< (int)zombieData[index].b << " " <<"a: "<< (int)zombieData[index].a << endl;
			//cout <<"r:" <<  (int)ppu.palette_table[7][3].r << endl << "g: " << (int)ppu.palette_table[7][3].g << endl << "b: "<< (int)ppu.palette_table[7][3].b << endl <<"a: "<< (int)ppu.palette_table[7][3].a << endl;
		}
		if(playerData[index] == ppu.palette_table[7][0]){
			// if(j == 0 && i == 0){
			// 	cout << "green" << endl;
			// }
			playerTile.bit0[i] &= ~(1 << j);
			playerTile.bit1[i] &= ~(1 << j);
		}
		else if(playerData[index] == ppu.palette_table[7][1]){
			// if(j == 0 && i == 0){
			// 	cout << "red" << endl;
			// }
			playerTile.bit0[i] |= (1 << j);
			playerTile.bit1[i] &= ~(1 << j);
		}
		else if(playerData[index] == ppu.palette_table[7][2]){
			// if(j == 0 && i == 0){
			// 	cout << "black" << endl;
			// }
			playerTile.bit0[i] &= ~(1 << j);
			playerTile.bit1[i] |= (1 << j);
		}
		else if(playerData[index] == ppu.palette_table[7][3]){
			// if(j == 0 && i == 0){
			// 	cout << "transparent" << endl;
			// }
			playerTile.bit0[i] |= (1 << j);
			playerTile.bit1[i] |= (1 << j);
		}
	}
}

//create tile for tower
for(int i = 0; i < 8; i++){
	for(int j = 0; j < 8; j++){
		int index = i*8 + j;
		if(i == 0 && j == 0){
			//cout <<"r:" <<  (int)zombieData[index].r  << " "<< "g: " << (int)zombieData[index].g <<" " << "b: "<< (int)zombieData[index].b << " " <<"a: "<< (int)zombieData[index].a << endl;
			//cout <<"r:" <<  (int)ppu.palette_table[7][3].r << endl << "g: " << (int)ppu.palette_table[7][3].g << endl << "b: "<< (int)ppu.palette_table[7][3].b << endl <<"a: "<< (int)ppu.palette_table[7][3].a << endl;
		}
		if(towerData[index] == ppu.palette_table[5][0]){
			// if(j == 0 && i == 0){
			// 	cout << "green" << endl;
			// }
			towerTile.bit0[i] &= ~(1 << j);
			towerTile.bit1[i] &= ~(1 << j);
		}
		else if(towerData[index] == ppu.palette_table[5][1]){
			// if(j == 0 && i == 0){
			// 	cout << "red" << endl;
			// }
			towerTile.bit0[i] |= (1 << j);
			towerTile.bit1[i] &= ~(1 << j);
		}
		else if(towerData[index] == ppu.palette_table[5][2]){
			// if(j == 0 && i == 0){
			// 	cout << "black" << endl;
			// }
			towerTile.bit0[i] &= ~(1 << j);
			towerTile.bit1[i] |= (1 << j);
		}
		else if(towerData[index] == ppu.palette_table[5][3]){
			// if(j == 0 && i == 0){
			// 	cout << "transparent" << endl;
			// }
			towerTile.bit0[i] |= (1 << j);
			towerTile.bit1[i] |= (1 << j);
		}
	}
}
ppu.tile_table[32] = playerTile;
ppu.tile_table[33] = zombieTile;
ppu.tile_table[34] = towerTile;



	// use sprite 32 as a "player":
	// ppu.tile_table[32].bit0 = {
	// 	0b01111110,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b01111110,
	// };
	// ppu.tile_table[32].bit1 = {
	// 	0b00000000,
	// 	0b00000000,
	// 	0b00011000,
	// 	0b00100100,
	// 	0b00000000,
	// 	0b00100100,
	// 	0b00000000,
	// 	0b00000000,
	// };

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	
	// //used for the player:
	// ppu.palette_table[7] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0xff, 0xff, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

};

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	 //background_fade += elapsed / 10.0f;
	// background_fade -= std::floor(background_fade);

	// constexpr float PlayerSpeed = 30.0f;
	// if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	// if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	// if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	// if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	startlag ++;

	//zombie goes down; if the zombie reach the bottom, stop moving
	constexpr float ZombieDownSpeed = 10.0f;
	
	//determine if the game is over or not
	if (flag){
		constexpr float PlayerSpeed = 30.0f;
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;
		zombieDown -= ZombieDownSpeed * elapsed;
	startlag ++;
	}
	
	

	//check collision
	for(int i = 35; i < 40; i++){
		
		if(ppu.sprites[i].y < 10){
			flag = false;
		}
		int distance = abs(ppu.sprites[i].y - ppu.sprites[0].y);

		//if collide, zombie goes to the back;
		if(distance == 0 && startlag >=200){ 
			back = i;
		}
		
		
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
		0xff
	);

	//tilemap gets recomputed every frame as some weird plasma thing:
	//NOTE: don't do this in your game! actually make a map or something :-)
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			//Hardcoded
			ppu.background[x+PPU466::BackgroundWidth*y] =0b0000011100001111;
		}
	}

	//background scroll:
	ppu.background_position.x = int32_t(-0.5f * player_at.x);
	ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//player sprite:
	ppu.sprites[0].x = int8_t(player_at.x+10);
	ppu.sprites[0].y = int8_t(player_at.y+10);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	
	//setup the tower sprites
	for(int i = 1; i < 35; i++){
		ppu.sprites[i].x = int8_t(1)+(int8_t)(i-2)*8;
		ppu.sprites[i].y = int8_t(1);
		ppu.sprites[i].index = 34;
		ppu.sprites[i].attributes = 5;
	}
	//setup the zombies
	for(int i = 35; i < 40; i++){
		ppu.sprites[i].x = int8_t(1)+8* (int8_t)((i-35)*5);
		ppu.sprites[i].y = int8_t(90)+ 8* (int8_t)(i-20) + (int8_t)zombieDown;
		ppu.sprites[i].index = 33;
		ppu.sprites[i].attributes = 4;
		if(i == back){
			ppu.sprites[i].y += int8_t(80);
		}
	}
	



	//some other misc sprites:
	// for (uint32_t i = 1; i < 63; ++i) {
	// 	float amt = (i + 2.0f * background_fade) / 62.0f;
	// 	ppu.sprites[i].x = int8_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].y = int8_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].index = 32;
	// 	ppu.sprites[i].attributes = 6;
	// 	if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	// }

	//--- actually draw ---
	ppu.draw(drawable_size);
}
