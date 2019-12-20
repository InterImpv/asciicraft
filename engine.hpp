#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <string>
#include <list>
#include <random>

#ifndef ENGINE_H
#define ENGINE_H

#define BLOCK_MAX 32
#define BLOCK_PIX 16

#define MAX_SCALE 256.f
#define MIN_SCALE 4.f

#define VIEW_SIZE 336

#define TICKS_IN_HOUR 360
#define HOURS_IN_DAY 4

struct GBlock
{
	unsigned int id;
	unsigned char tile;
	bool canPass = false;
	bool isTransparent = false;
	
	unsigned int img_x = 0;
	unsigned int img_y = 0;
};

struct GFloor
{
	unsigned int id;
	unsigned char tile;
	bool dug = false;
	
	unsigned int img_x = 0;
	unsigned int img_y = 0;
};

struct GVoxel
{
	GBlock _Gblock;
	GFloor _Gfloor;
};

enum _GBLOCKS
{
	AIR = 0,
	
	BARRIER = 1,
	STONE = 2,
	COBBLESTONE = 3,
	STONEBRICKS = 4,
	WOOD = 5,
	TREE = 16,
	SANDSTONE = 19,
	SANDBRICKS = 20,
};

enum _GFLOORS
{
	GRASS = 1,
	DIRT = 2,
	GRAVEL = 3,
	SAND = 4,
	WATER = 5,
	LAVA = 6,
	CLAY = 7,
	WOODPLANKS = 8,
};

enum _GENTITIES
{
	PLAYER = 0,
	CHICKEN = 1,
	ZOMBIE = 2,
};

class GPlayer;
class GEntity;

class EGame
{
	private:
		int ScreenWidth;
		int ScreenHeight;
		int mapWidth;
		int mapHeight;
		
		unsigned int timeT = 0;
		unsigned int clockT = 0;
		bool day_night = true;
		
		/* world params */
		double noiseScale;
		double noiseScaleFactor;
		
		/* world gen tries */
		unsigned int smoothTries;
		
		/* misc */
		bool isLoaded = false;
		bool isInit = false;
		
		/* map && game blocks */
		GVoxel **map;
		GBlock *_blocks;
		GFloor *_floors;
		
	public:
		EGame();
		~EGame();
		
		void set_scrwh(int width, int height);
		int get_scrw() const;
		int get_scrh() const;
		
		void unload();
		bool status();
		
		/* map stuff */
		GVoxel **get_map() const;
		
		int get_mapw() const;
		int get_maph() const;
		
		void set_block(const int x, const int y, unsigned int id);
		void set_floor(const int x, const int y, unsigned int id);
		void set_voxel(const int x, const int y, GVoxel voxel);
		
		int check_block(const int x, const int y);
		int check_floor(const int x, const int y);
		bool check_collision(const int x, const int y);
		
		GBlock *get_block(const int x, const int y);
		GFloor *get_floor(const int x, const int y);
		GVoxel *get_voxel(const int x, const int y);
		
		GBlock *get_blocks() const;
		GFloor *get_floors() const;
				
		void map_gen();
		void map_physics();
		
		void gen_flora();
		void gen_smooth();
		
		void sim_liquid(const int x, const int y);
		void sim_flora(const int x, const int y);
		void sim_gravel(const int x, const int y);
		
		/* game renderer */
		void render();
		
		/* entities && player */
		GPlayer *player = nullptr;
		std::list<GEntity> _entities;
		
		/* event handler && game window */
		sf::Event eventHandler;
		sf::RenderWindow window;
		sf::View gameWin;
		
		/* game graphics */
		sf::Texture _Tblocks;
		sf::Texture _Tfloors;
		sf::Texture _Tentities;
		
		sf::Font gameFont;
		sf::Text text_buf;
};

class GEntity
{
	protected:	
		int x;
		int y;
		int dir_prev = 0;
		
		int hp;		//health points
		int fp;		//fatigue points
		int id;		//what kind of mob is this
		
		EGame *game = nullptr;
		
		sf::Sprite ent_sprite;
		
	public:
		GEntity();
		GEntity(EGame &game, int id);
	
		void set_x(const int x);
		void set_y(const int y);
		void set_xy(const int x, const int y);
		
		int get_x() const;
		int get_y() const;
		int get_hp() const;
		int get_fp() const;
		int get_id() const;
		
		bool isAlive();
		
		void simulate();
		
		void place(int x, int y);
		
		void render();
		void move(int dir);
};

class GPlayer : public GEntity
{
	protected:
		GVoxel tile;
		
		int xspawn = 0;
		int yspawn = 0;
		
	public:
		GPlayer();
		GPlayer(EGame &game);
		
		GVoxel *get_tile();
		int get_block() const;
		int get_floor() const;
		unsigned int dugBlock(int x, int y);
		unsigned int dugFloor(int x, int y);
				
		void render();
		void respawn();
		
		void mine(int dir);
		void build(int dir);
		void action();
		
		void simulate();
		void place();
};

class EMiner
{
	private:
		int x;
		int y;
		
		unsigned int tile;
		GVoxel voxel;
		
		bool active;
		
	public:	
		EMiner();
		EMiner(const int x, const int y, const unsigned int tile);
		
		void set_x(const int x);
		void set_y(const int y);
		void set_xy(const int x, const int y);
		void set_tiletype(const unsigned int tile);
		void set_voxel(GBlock block, GFloor floor);
		void activate();
		void deactivate();
		
		int get_x() const;
		int get_y() const;
		bool isActive() const;
		
		void mine(EGame &game);
};

#endif
