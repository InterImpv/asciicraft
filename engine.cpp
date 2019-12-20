#include "engine.hpp"
#include "perlin.hpp"

EGame::EGame()
{
	printf("Loading game...\n");
	
	srand(time(NULL));
	isLoaded = true;
		
	/* this is hardcoded for now */
	/* BLOCKS */
	_blocks = new GBlock[BLOCK_MAX];
	//air
	_blocks[AIR].id = AIR;
	_blocks[AIR].tile = '*';
	_blocks[AIR].canPass = true;
	_blocks[AIR].isTransparent = true;
	//barrier
	_blocks[BARRIER].id = BARRIER;
	_blocks[BARRIER].tile = 'X';
	_blocks[AIR].isTransparent = true;
	_blocks[BARRIER].img_x = 1;
	_blocks[BARRIER].img_y = 0;
	//stone
	_blocks[STONE].id = STONE;
	_blocks[STONE].tile = 'H';
	_blocks[STONE].img_x = 2;
	_blocks[STONE].img_y = 0;
	//cobble
	_blocks[COBBLESTONE].id = COBBLESTONE;
	_blocks[COBBLESTONE].tile = '%';
	_blocks[COBBLESTONE].img_x = 3;
	_blocks[COBBLESTONE].img_y = 0;
	//stone bricks
	_blocks[STONEBRICKS].id = STONEBRICKS;
	_blocks[STONEBRICKS].tile = '=';
	_blocks[STONEBRICKS].img_x = 4;
	_blocks[STONEBRICKS].img_y = 0;
	//sandstone
	_blocks[SANDSTONE].id = SANDSTONE;
	_blocks[SANDSTONE].tile = '%';
	_blocks[SANDSTONE].img_x = 3;
	_blocks[SANDSTONE].img_y = 1;
	//sand bricks
	_blocks[SANDBRICKS].id = SANDBRICKS;
	_blocks[SANDBRICKS].tile = '=';
	_blocks[SANDBRICKS].img_x = 4;
	_blocks[SANDBRICKS].img_y = 1;
	//wood
	_blocks[WOOD].id = WOOD;
	_blocks[WOOD].tile = '=';
	_blocks[WOOD].img_x = 5;
	_blocks[WOOD].img_y = 0;
	//tree
	_blocks[TREE].id = TREE;
	_blocks[TREE].tile = 'T';
	_blocks[TREE].img_x = 0;
	_blocks[TREE].img_y = 1;
	
	/* FLOORS */
	_floors = new GFloor[BLOCK_MAX];
	//air
	_floors[AIR].id = AIR;
	_floors[AIR].tile = '*';
	_floors[AIR].img_x = 0;
	_floors[AIR].img_y = 0;
	//grass
	_floors[GRASS].id = GRASS;
	_floors[GRASS].tile = ',';
	_floors[GRASS].img_x = 1;
	_floors[GRASS].img_y = 0;
	//dirt
	_floors[DIRT].id = DIRT;
	_floors[DIRT].tile = '.';
	_floors[DIRT].img_x = 2;
	_floors[DIRT].img_y = 0;
	//gravel
	_floors[GRAVEL].id = GRAVEL;
	_floors[GRAVEL].tile = ':';
	_floors[GRAVEL].img_x = 3;
	_floors[GRAVEL].img_y = 0;
	//sand
	_floors[SAND].id = SAND;
	_floors[SAND].tile = ':';
	_floors[SAND].img_x = 4;
	_floors[SAND].img_y = 0;
	//water
	_floors[WATER].id = WATER;
	_floors[WATER].tile = '~';
	_floors[WATER].img_x = 5;
	_floors[WATER].img_y = 0;
	//lava
	_floors[LAVA].id = LAVA;
	_floors[LAVA].tile = '~';
	_floors[LAVA].img_x = 6;
	_floors[LAVA].img_y = 0;
	//clay
	_floors[CLAY].id = CLAY;
	_floors[CLAY].tile = ';';
	_floors[CLAY].img_x = 7;
	_floors[CLAY].img_y = 0;
	//woodplanks
	_floors[WOODPLANKS].id = WOODPLANKS;
	_floors[WOODPLANKS].tile = '+';
	_floors[WOODPLANKS].img_x = 8;
	_floors[WOODPLANKS].img_y = 0;
	
	/* screen and map sizes */
	set_scrwh(768, 768);
	mapWidth = 256;
	mapHeight = 256;
	
	/* world gen params*/
	smoothTries = 2;
	
	noiseScaleFactor = 16;
	noiseScale = 0.05;
	
	/* game window */
	window.create(sf::VideoMode(ScreenWidth, ScreenHeight), "LittleBuild");
	gameWin.reset(sf::FloatRect(0, 0, VIEW_SIZE, VIEW_SIZE));
	window.setView(gameWin);
	
	/* voxels */
	if (!_Tblocks.loadFromFile("textures/blocks.png"))
        isLoaded = false;
    if (!_Tfloors.loadFromFile("textures/floors.png"))
        isLoaded = false;
        
    /* entities */
	if (!_Tentities.loadFromFile("textures/entities.png"))
        isLoaded = false;
        
    /* font && text */
	if (!gameFont.loadFromFile("textures/font.ttf"))
        isLoaded = false;
        
    text_buf.setFont(gameFont);
    text_buf.setCharacterSize(16);
    text_buf.setFillColor(sf::Color::White);
    text_buf.setPosition(0, 0);
		
	/* map && world gen*/
	map = new GVoxel *[mapWidth];
	for (int i = 0; i < mapWidth; i++)
		map[i] = new GVoxel[mapWidth];
			
	map_gen();
	
	printf("Game loaded!\n");
}

EGame::~EGame()
{
	delete [] _blocks;
	delete [] _floors;
	delete [] map;
	printf("Exit!\n");
}

void EGame::set_scrwh(int width, int height) 
{
	ScreenWidth = width;
	ScreenHeight = height;
}
int EGame::get_scrw() const{	return ScreenWidth;	}
int EGame::get_scrh() const{	return ScreenHeight; }
int EGame::get_mapw() const{	return mapWidth;	}
int EGame::get_maph() const{	return mapHeight; }

void EGame::unload(){	isLoaded = false; }
bool EGame::status(){	return isLoaded; }

GVoxel **EGame::get_map() const{	return map; }
GBlock *EGame::get_blocks() const{	return _blocks; }
GFloor *EGame::get_floors() const{	return _floors; }

void EGame::set_block(const int x, const int y, unsigned int id)
{
	if (id > BLOCK_MAX)
		return;
	
	if ((x > 0 && x < get_mapw() - 1) && (y > 0 && y < get_maph() - 1))
		map[y][x]._Gblock = _blocks[id];
	else
		return;
}

void EGame::set_floor(const int x, const int y, unsigned int id)
{
	if (id > BLOCK_MAX)
		return;
		
	if ((x > 0 && x < get_mapw() - 1) && (y > 0 && y < get_maph() - 1))
		map[y][x]._Gfloor = _floors[id];
	else
		return;
}

void EGame::set_voxel(const int x, const int y, GVoxel voxel)
{
	if ((x > 0 && x < get_mapw() - 1) && (y > 0 && y < get_maph() - 1))
		map[y][x] = voxel;
	else
		return;
}

int EGame::check_block(const int x, const int y)
{
	if ((x >= 0 && x <= get_mapw() - 1) &&
		(y >= 0 && y <= get_maph() - 1))
		return map[y][x]._Gblock.id;
	else
		return -1;
}

int EGame::check_floor(const int x, const int y)
{
	if ((x >= 0 && x <= get_mapw() - 1) &&
		(y >= 0 && y <= get_maph() - 1))
		return map[y][x]._Gfloor.id;
	else
		return -1;
}

GBlock *EGame::get_block(const int x, const int y)
{
	if ((x >= 0 && x <= get_mapw() - 1) &&
		(y >= 0 && y <= get_maph() - 1))
		return &map[y][x]._Gblock;
	else
		return nullptr;
}

GFloor *EGame::get_floor(const int x, const int y)
{
	if ((x >= 0 && x <= get_mapw() - 1) &&
		(y >= 0 && y <= get_maph() - 1))
		return &map[y][x]._Gfloor;
	else
		return nullptr;
}

GVoxel *EGame::get_voxel(const int x, const int y)
{
	if ((x >= 0 && x <= get_mapw() - 1) &&
		(y >= 0 && y <= get_maph() - 1))
		return &map[y][x];
	else
		return nullptr;
}

bool EGame::check_collision(const int x, const int y)
{
	if ((x >= 0 && x <= get_mapw() - 1) &&
		(y >= 0 && y <= get_maph() - 1))
		{
		return map[y][x]._Gblock.canPass;
	}
	else
		return true;
}

void EGame::map_gen()
{	
	printf("\tGenerating map...\n");
	clock_t startT = clock();
	
	//fills map with grass
	for (int i = 0; i < get_maph(); i++)
	{
		for (int j = 0; j < get_mapw(); j++)
		{
			map[i][j]._Gblock = _blocks[AIR];
			map[i][j]._Gfloor = _floors[GRAVEL];
		}
	}
	
	//generates map border
	for (int i = 0; i < get_maph(); i++)
	{		
		for (int j = 0; j < get_mapw(); j++)	
		{
			map[0][j]._Gblock = _blocks[BARRIER];
			map[i][0]._Gblock = _blocks[BARRIER];
			map[get_maph() - 1][j]._Gblock = _blocks[BARRIER];
			map[i][get_mapw() - 1]._Gblock = _blocks[BARRIER];
		}
	}
	
	/* initialize perlin noise */
	PerlinNoise noiseGen(time(NULL));
	
	std::random_device scaleValue;
    std::uniform_real_distribution<double> dist(-noiseScale, noiseScale);
	
	double noiseValue;
	for (int i = 1; i < get_maph() - 1; i++)
	{
		for (int j = 1; j < get_mapw() - 1; j++)
		{
			noiseValue = noiseGen.noise((double)j / noiseScaleFactor,
										(double)i / noiseScaleFactor,
										abs(dist(scaleValue)));
			
			/* adds slight noisiness to the landscape*/
			noiseValue += dist(scaleValue);
			
			//water						
			if (noiseValue <= 0.32)
			{
				map[i][j]._Gblock = _blocks[AIR];
				map[i][j]._Gfloor = _floors[WATER];
			}
			//sand
			if (noiseValue > 0.32 && noiseValue <= 0.35)
			{
				map[i][j]._Gblock = _blocks[AIR];
				map[i][j]._Gfloor = _floors[SAND];
			}
			//clay
			if (noiseValue > 0.318 && noiseValue <= 0.32)
			{
				map[i][j]._Gblock = _blocks[AIR];
				map[i][j]._Gfloor = _floors[CLAY];
			}
			//grass
			if (noiseValue > 0.35 && noiseValue <= 0.7)
			{
				map[i][j]._Gblock = _blocks[AIR];
				map[i][j]._Gfloor = _floors[GRASS];
			}
			//trees
			if (noiseValue >= 0.525 && noiseValue <= 0.65)
			{
				if (rand() % 2 < 1)
				{
					map[i][j]._Gblock = _blocks[TREE];
					map[i][j]._Gfloor = _floors[GRASS];
				}
			}
			//gravel
			if (noiseValue > 0.7 && noiseValue <= 0.725)
			{
				map[i][j]._Gblock = _blocks[AIR];
				map[i][j]._Gfloor = _floors[GRAVEL];
			}
			//stone
			if (noiseValue > 0.725)
			{
				map[i][j]._Gblock = _blocks[STONE];
				map[i][j]._Gfloor = _floors[GRAVEL];
			}
		}
	}
	gen_smooth();
	
	clock_t endT = clock();
	printf("Done in %.8lfs!\n", (double)(endT - startT)/CLOCKS_PER_SEC);
}
void EGame::gen_smooth()
{
	for (int k = 0; k < (int)smoothTries; k++)
	{
		for (int i = 1; i < get_maph() - 1; i++)
		{
			for (int j = 1; j < get_mapw() - 1; j++)
			{
				//checks for lone stone blocks
				if (check_block(j, i) == STONE)
				{
					if (((check_block(j + 1, i) == AIR) &&
						( check_block(j - 1, i) == AIR)) &&
						((check_block(j, i + 1) == AIR) &&
						( check_block(j, i - 1) == AIR)))
						{
							set_block(j, i, AIR);
							set_floor(j, i, GRASS);
						}
				}
				
				if (check_block(j, i) == AIR)
				{
					if (((check_block(j + 1, i) == STONE) &&
						( check_block(j - 1, i) == STONE)) &&
						((check_block(j, i + 1) == STONE) &&
						( check_block(j, i - 1) == STONE)))
						{
							set_block(j, i, STONE);
							set_floor(j, i, GRAVEL);
						}
				}
						
				//creating beaches
				if (check_floor(j, i) == WATER)
				{
					if (check_floor(j + 1, i) == GRASS)
						set_floor(j + 1, i, SAND);
					if (check_floor(j - 1, i) == GRASS)
						set_floor(j - 1, i, SAND);
					if (check_floor(j, i + 1) == GRASS)
						set_floor(j, i + 1, SAND);
					if (check_floor(j, i - 1) == GRASS)
						set_floor(j, i - 1, SAND);
				}
				
				if (check_floor(j, i) == WATER)
				{
					if (((check_floor(j + 1, i) == SAND) &&
						( check_floor(j - 1, i) == SAND)) &&
						((check_floor(j, i + 1) == SAND) &&
						( check_floor(j, i - 1) == SAND)))
							set_floor(j, i, SAND);
				}
				
				if (check_floor(j, i) == SAND ||
					check_floor(j, i) == GRAVEL ||
					check_floor(j, i) == CLAY)
				{
					if (((check_floor(j + 1, i) == WATER) &&
						( check_floor(j - 1, i) == WATER)) ||
						((check_floor(j, i + 1) == WATER) &&
						( check_floor(j, i - 1) == WATER)))
							set_floor(j, i, WATER);	
				}
				
				//checks for lone gravel blocks
				if (check_floor(j, i) == GRAVEL)
				{
					if (((check_floor(j + 1, i) == GRASS) &&
						( check_floor(j - 1, i) == GRASS)) &&
						((check_floor(j, i + 1) == GRASS) &&
						( check_floor(j, i - 1) == GRASS)))
						{
							set_block(j, i, AIR);
							set_floor(j, i, GRASS);
						}
				}
				
				//checks for lone sand blocks
				if (check_floor(j, i) == SAND)
				{
					if (((check_floor(j + 1, i) == GRASS) &&
						( check_floor(j - 1, i) == GRASS)) &&
						((check_floor(j, i + 1) == GRASS) &&
						( check_floor(j, i - 1) == GRASS)))
						{
							set_block(j, i, AIR);
							set_floor(j, i, GRASS);
						}
				}
			}
		}
	}
}

void EGame::render()
{
	clock_t startT = clock();
			
	int xstart = player->get_x() - ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX)) + timeT;
	int ystart = player->get_y() - ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX)) + timeT;
	int xend = player->get_x() + ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX)) - timeT;
	int yend = player->get_y() + ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX)) - timeT;
	
	int xview = player->get_x();
	int yview = player->get_y();
	
	if (player->get_x() <= 10)
		xview = 10;
	if (player->get_x() >= get_mapw() - 11)
		xview = get_mapw() - 11;
	if (player->get_y() <= 10)
		yview = 10;
	if (player->get_y() >= get_maph() - 11)
		yview = get_maph() - 11;
	
	if (xstart < 0)
		xstart = 0;
	if (ystart < 0)
		ystart = 0;
	if (xend >= get_mapw())
		xend = get_mapw() - 1;
	if (yend >= get_maph())
		yend = get_maph() - 1;
		
	gameWin.setCenter(BLOCK_PIX * (xview + 0.5), BLOCK_PIX * (yview + 0.5));
	
	window.setView(gameWin);
    window.clear(sf::Color::Black);
    
    /* map render */
	sf::Sprite _Btemp(_Tblocks);
	sf::Sprite _Ftemp(_Tfloors);
	
	for (int i = ystart; i < yend + 1; i++)
	{
		for (int j = xstart; j < xend + 1; j++)
		{		
			if (check_block(j, i) <= AIR)
			{
				if (get_floor(j, i)->dug)
				{									
					_Ftemp.setTextureRect(sf::IntRect(BLOCK_PIX * get_voxel(j, i)->_Gfloor.img_x,
													  BLOCK_PIX * get_voxel(j, i)->_Gfloor.img_y, 
													  BLOCK_PIX, BLOCK_PIX));
													  
					_Ftemp.setPosition(j * BLOCK_PIX, i * BLOCK_PIX);
					this->window.draw(_Ftemp);
					
					_Ftemp.setTextureRect(sf::IntRect(240,
													  240, 
													  BLOCK_PIX, BLOCK_PIX));
													  
					_Ftemp.setPosition(j * BLOCK_PIX, i * BLOCK_PIX);
					this->window.draw(_Ftemp);
				}
				else
				{
					_Ftemp.setTextureRect(sf::IntRect(BLOCK_PIX * get_voxel(j, i)->_Gfloor.img_x,
													  BLOCK_PIX * get_voxel(j, i)->_Gfloor.img_y, 
													  BLOCK_PIX, BLOCK_PIX));
													  
					_Ftemp.setPosition(j * BLOCK_PIX, i * BLOCK_PIX);
					this->window.draw(_Ftemp);
				}
			}
			else
			{
				if (!get_block(j, i)->isTransparent)
				{
					_Ftemp.setTextureRect(sf::IntRect(BLOCK_PIX * get_voxel(j, i)->_Gfloor.img_x,
													  BLOCK_PIX * get_voxel(j, i)->_Gfloor.img_y, 
													  BLOCK_PIX, BLOCK_PIX));
												  
					_Ftemp.setPosition(j * BLOCK_PIX, i * BLOCK_PIX);
				}
				
				_Btemp.setTextureRect(sf::IntRect(BLOCK_PIX * get_voxel(j, i)->_Gblock.img_x,
												  BLOCK_PIX * get_voxel(j, i)->_Gblock.img_y, 
												  BLOCK_PIX, BLOCK_PIX));
												  
				_Btemp.setPosition(j * BLOCK_PIX, i * BLOCK_PIX);
				this->window.draw(_Ftemp);
				this->window.draw(_Btemp);
			}
		}
	}
	
	/* entities && player */
	for (std::list<GEntity>::iterator _ent_it = _entities.begin(); _ent_it != _entities.end(); ++_ent_it)
	{		
		_ent_it->render();
		
		if (!_ent_it->isAlive())
			_entities.erase(_ent_it--);
	}
	player->render();
	
	window.display();
	
	clock_t endT = clock();
	printf("\t\tFrame rendered in %.4lfs!\n", (double)(endT - startT)/CLOCKS_PER_SEC);
}

void EGame::map_physics()
{	
	clock_t startT = clock();
	
	/* day/night cycle */
	if (clockT < TICKS_IN_HOUR)
		clockT++;
	else
	{	
		if (timeT < HOURS_IN_DAY && day_night)
		{
			timeT++;
			clockT = 0;
		}
		else if (timeT >= HOURS_IN_DAY)
			day_night = false;
			
		if (timeT > 0 && !day_night)
		{
			timeT--;
			clockT = 0;
		}
		else if (timeT < 0)
			day_night = true;
	}
	
	int xstart = player->get_x() - ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	int ystart = player->get_y() - ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	int xend = player->get_x() + ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	int yend = player->get_y() + ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	
	for (int i = ystart; i < yend + 1; i++)
	{
		for (int j = xstart; j < xend + 1; j++)
		{
			sim_liquid(j, i);
			sim_flora(j, i);
		}
	}
	
	for (std::list<GEntity>::iterator _ent_it = _entities.begin(); _ent_it != _entities.end(); ++_ent_it)
	{
		_ent_it->simulate();
				
		if (!_ent_it->isAlive())
			_entities.erase(_ent_it--);
	}
	
	player->simulate();
	
	clock_t endT = clock();
	printf("\t\tFrame simulated in %.4lfs!\n", (double)(endT - startT)/CLOCKS_PER_SEC);
}

void EGame::sim_liquid(const int x, const int y)
{
	if ((x < 0 && x > get_mapw() - 1) &&
		(y < 0 && y > get_maph() - 1))
		return;
	
	//water
	if (check_floor(x, y) == WATER)
	{
		if (get_floor(x + 1, y)->dug)
			set_floor(x + 1, y, WATER);
		if (get_floor(x - 1, y)->dug)
			set_floor(x - 1, y, WATER);
		if (get_floor(x, y + 1)->dug)
			set_floor(x, y + 1, WATER);
		if (get_floor(x, y - 1)->dug)
			set_floor(x, y - 1, WATER);
			
		if (check_floor(x + 1, y) == LAVA)
		{
			set_block(x + 1, y, STONE);
			set_floor(x + 1, y, GRAVEL);
		}
		if (check_floor(x - 1, y) == LAVA)
		{
			set_block(x - 1, y, STONE);
			set_floor(x - 1, y, GRAVEL);
		}
		if (check_floor(x, y + 1) == LAVA)
		{
			set_block(x, y + 1, STONE);
			set_floor(x, y + 1, STONE);
		}
		if (check_floor(x, y - 1) == LAVA)
		{
			set_block(x, y - 1, STONE);
			set_floor(x, y - 1, STONE);
		}
	}
	
	//lava
	if (check_floor(x, y) == LAVA)
	{
		if (get_floor(x + 1, y)->dug)
			set_floor(x + 1, y, LAVA);
		if (get_floor(x - 1, y)->dug)
			set_floor(x - 1, y, LAVA);
		if (get_floor(x, y + 1)->dug)
			set_floor(x, y + 1, LAVA);
		if (get_floor(x, y - 1)->dug)
			set_floor(x, y - 1, LAVA);
		
		if (check_floor(x + 1, y) == GRASS)
			set_floor(x + 1, y, DIRT);
		if (check_floor(x - 1, y) == GRASS)
			set_floor(x - 1, y, DIRT);
		if (check_floor(x, y + 1) == GRASS)
			set_floor(x, y + 1, DIRT);
		if (check_floor(x, y - 1) == GRASS)
			set_floor(x, y - 1, DIRT);
	}
}

void EGame::sim_flora(const int x, const int y)
{
	if ((x < 0 && x > get_mapw() - 1) &&
		(y < 0 && y > get_maph() - 1))
		return;
		
	//grass
	if (check_block(x, y) == AIR && check_floor(x, y) == DIRT)
	{
		if (rand() % 100 <= 1 && !get_floor(x, y)->dug)
			set_floor(x, y, GRASS);
	}
	if (check_block(x, y) != AIR && check_block(x, y) != TREE &&
		check_floor(x, y) == GRASS)
	{
		if (rand() % 10 <= 1)
			set_floor(x, y, DIRT);
	}
	
	//trees
	if (check_block(x, y) == TREE)
	{
		if (rand() % 10000 <= 1)
		{
			int randX = -1 + rand() % 3,
				randY = -1 + rand() % 3;
				
			if (check_block(x + randX, y + randY) == AIR &&
				check_floor(x + randX, y + randY) == GRASS)
				set_block(x + randX, y + randY, TREE);
		}
	}
}
