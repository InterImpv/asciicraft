#include "engine.hpp"

//entity
GEntity::GEntity()
{
	id = 1 + rand() % ZOMBIE;
	switch (id)
	{
		case CHICKEN:
			hp = 4;
			fp = 8;
			break;
		case ZOMBIE:
			hp = 10;
			fp = 20;
			dir_prev = rand() % 8;
			break;
	};
}

GEntity::GEntity(EGame &game, int id)
{	
	this->game = &game;
	
	ent_sprite.setTexture(this->game->_Tentities);
	ent_sprite.setTextureRect(sf::IntRect(BLOCK_PIX * id, 0, BLOCK_PIX, BLOCK_PIX));
	
	x = 1 + rand() % (this->game->get_mapw() - 2);
	y = 1 + rand() % (this->game->get_maph() - 2);
	
	this->id = id;
	switch (id)
	{
		case CHICKEN:
			hp = 4;
			fp = 8;
			break;
		case ZOMBIE:
			hp = 10;
			fp = 20;
			dir_prev = rand() % 8;
			break;
	};
}

void GEntity::set_x(const int x){	this->x = x;	}
void GEntity::set_y(const int y){	this->y = y;	}
void GEntity::set_xy(const int x, const int y)
{
	this->x = x;
	this->y = y;
}

int GEntity::get_x() const{	return x;	}
int GEntity::get_y() const{	return y;	}
int GEntity::get_hp() const{	return hp;	}
int GEntity::get_fp() const{	return fp;	}
int GEntity::get_id() const{	return id;	}

bool GEntity::isAlive(){	return (hp > 0) ? true : false;	}

void GEntity::place(int x, int y)
{
	this->x = x;
	this->y = y;
	//this->id = 1 + rand() % ZOMBIE;
        
	ent_sprite.setTexture(this->game->_Tentities);
	ent_sprite.setTextureRect(sf::IntRect(BLOCK_PIX * id, 0, BLOCK_PIX, BLOCK_PIX));
}

void GEntity::simulate()
{
	int dir = rand() % 8;
	int moveTries = 0;
	dir_prev = dir;
	
	if (!game->check_collision(x, y))
		hp--;
		
	switch(game->check_floor(x, y))
	{
		case WATER:
			if (fp > 0)
				fp--;
			else if (hp > 0){
				hp--;
			}
			break;
		case LAVA:
			if (fp > 0 && hp > 0)
			{
				fp--;
				hp--;
			}
			break;
			
		default:
			if (fp < 20)
				fp++;
			
			if (hp < 10)
				hp++;
				
			break;
	}
	
	switch(id)
	{
		case CHICKEN:
			while (moveTries < 4 && dir == dir_prev)
			{
				dir = rand() % 8;
				moveTries++;
			}
			move(dir);
			break;
			
		case ZOMBIE:
		{
			int dx = this->x - game->player->x;
			int dy = this->y - game->player->y;
			
			int distToPlayer = hypot(dx, dy);
			int dir = rand() % 8;
			
			if (distToPlayer < 16)
			{		
				if (dx <= 0 && dy >= 0)
					move(0);
				if (dx == -dy)
					move(1);
				if (dx <= 0 && dy <= 0)
					move(2);
				if (-dx == -dy)
					move(3);
				if (dx >= 0 && dy <= 0)
					move(4);
				if (-dx == dy)
					move(5);
				if (dx >= 0 && dy >= 0)
					move(6);
				if (dx == dy)
					move(7);
			}
			else
			{
				move(dir);
			}
			break;
		}
		
		default:
			dir = dir_prev + (-1 + rand() % 3);
			move(dir);
			break;
	};
}

void GEntity::render()
{
	ent_sprite.setPosition(x * BLOCK_PIX, y * BLOCK_PIX);
	game->window.draw(ent_sprite);
}

void GEntity::move(int dir)
{
	switch(dir)
	{
		case 0:		//E
			if (game->check_collision(x + 1, y))
				x++;
			break;
		case 1:		//SE
			if (game->check_collision(x + 1, y + 1))
			{
				x++;
				y++;
			}
			break;
		case 2:		//S
			if (game->check_collision(x, y + 1))
				y = y + 1;
			break;
		case 3:		//SW
			if (game->check_collision(x - 1, y + 1))
			{
				x--;
				y++;
			}
			break;
		case 4:		//W
			if (game->check_collision(x - 1, y))
				x--;
			break;
		case 5:		//NE
			if (game->check_collision(x - 1, y - 1))
			{
				x--;
				y--;
			}
			break;
		case 6:		//N
			if (game->check_collision(x, y - 1))
				y--;
			break;
		case 7:		//NE
			if (game->check_collision(x + 1, y - 1))
			{
				x++;
				y--;
			}
			break;
	};
}

//player
GPlayer::GPlayer()
{
	id = PLAYER;
	x = 1;
	y = 1;
	xspawn = x;
	yspawn = y;
	hp = 10;
	fp = 20;
}

GPlayer::GPlayer(EGame &game)
{	
	id = PLAYER;
	this->game = &game;
	this->game->player = this;	
        
	ent_sprite.setTexture(this->game->_Tentities);
	ent_sprite.setTextureRect(sf::IntRect(BLOCK_PIX * id, 0, BLOCK_PIX, BLOCK_PIX));
	
	/* world placement */
	unsigned int placementTries = 0;
	unsigned int checkSum;
	int density = this->game->get_mapw() * this->game->get_maph();
	
	x = this->game->get_mapw() / 4;
	y = this->game->get_maph() / 4;
	
	std::random_device xy_dev;
    std::uniform_real_distribution<double> x_rand(x, 3 * x);
    std::uniform_real_distribution<double> y_rand(y, 3 * y);
	
	while (placementTries < density)
	{
		checkSum = 0;
		
		x = x_rand(xy_dev);
		y = y_rand(xy_dev);
		
		for (int i = y - 1; i < y + 2; i++)
		{
			for (int j = x - 1; j < x + 2; j++)
			{
				if (this->game->check_floor(j, i) == GRASS &&
					this->game->check_block(j, i) == AIR)
					checkSum++;
			}
		}
		
		if (checkSum >= 8)
		{
			break;
		}
		else
		{
			this->game->map_gen();
			placementTries++;
		}
	}
	
	if (placementTries >= density)
	{
		x = x_rand(xy_dev);
		y = y_rand(xy_dev);
	}
	
	/* variables */
	xspawn = x;
	yspawn = y;
	hp = 10;
	fp = 20;
	tile._Gblock = this->game->get_blocks()[STONE];
	tile._Gfloor = this->game->get_floors()[GRAVEL];
}

GVoxel *GPlayer::get_tile(){	return &tile;	}
int GPlayer::get_block() const{	return tile._Gblock.id;	}
int GPlayer::get_floor() const{	return tile._Gfloor.id;	}

void GPlayer::respawn()
{
	x = xspawn;
	y = yspawn;
	hp = 10;
	fp = 20;
}

/*
 * 0 - E
 * 1 - SE
 * 2 - S
 * 3 - SW
 * 4 - W
 * 5 - NW
 * 6 - N
 * 7 - NE
 */

void GPlayer::action()
{
	if (game->eventHandler.type != sf::Event::KeyPressed)
		return;
	
	switch(game->eventHandler.key.code)
	{
		/* movement */
		case sf::Keyboard::Right:
			move(0);
			break;
		case sf::Keyboard::Down:
			move(2);
			break;
		case sf::Keyboard::Left:
			move(4);
			break;
		case sf::Keyboard::Up:
			move(6);
			break;
			
		case sf::Keyboard::Numpad6:
			move(0);
			break;
		case sf::Keyboard::Numpad3:
			move(1);
			break;
		case sf::Keyboard::Numpad2:
			move(2);
			break;
		case sf::Keyboard::Numpad1:
			move(3);
			break;
		case sf::Keyboard::Numpad4:
			move(4);
			break;
		case sf::Keyboard::Numpad7:
			move(5);
			break;
		case sf::Keyboard::Numpad8:
			move(6);
			break;
		case sf::Keyboard::Numpad9:
			move(7);
			break;
		
		/* mining */
		case sf::Keyboard::X:
		{
			game->window.waitEvent(game->eventHandler);
			while (game->eventHandler.type != sf::Event::KeyPressed)
				game->window.waitEvent(game->eventHandler);
			
			switch (game->eventHandler.key.code)
			{
				case sf::Keyboard::Right:
					mine(0);
					break;
				case sf::Keyboard::Down:
					mine(2);
					break;
				case sf::Keyboard::Left:
					mine(4);
					break;
				case sf::Keyboard::Up:
					mine(6);
					break;
					
				case sf::Keyboard::Numpad6:
					mine(0);
					break;
				case sf::Keyboard::Numpad3:
					mine(1);
					break;
				case sf::Keyboard::Numpad2:
					mine(2);
					break;
				case sf::Keyboard::Numpad1:
					mine(3);
					break;
				case sf::Keyboard::Numpad4:
					mine(4);
					break;
				case sf::Keyboard::Numpad7:
					mine(5);
					break;
				case sf::Keyboard::Numpad8:
					mine(6);
					break;
				case sf::Keyboard::Numpad9:
					mine(7);
					break;
			};
			break;
		}
		
		/* changing blocks */
		case sf::Keyboard::Add:
			if (tile._Gblock.id < WOOD)
				tile._Gblock = game->get_blocks()[++tile._Gblock.id];
			break;
		case sf::Keyboard::Subtract:
			if (tile._Gblock.id > AIR)
				tile._Gblock = game->get_blocks()[--tile._Gblock.id];
			break;
			
		/* changing floors */
		case sf::Keyboard::Multiply:
			if (tile._Gfloor.id < WOODPLANKS)
				tile._Gfloor = game->get_floors()[++tile._Gfloor.id];
			break;
		case sf::Keyboard::Divide:
			if (tile._Gfloor.id > GRASS)
				tile._Gfloor = game->get_floors()[--tile._Gfloor.id];
			break;
			
		/* spawning entities */
		case sf::Keyboard::G:
		{
			GEntity buf_ent_rand(*this->game, 1 + rand() % ZOMBIE);
			buf_ent_rand.place(x, y);
			game->_entities.push_back(buf_ent_rand);
			break;
		}
			
		/* building */
		case sf::Keyboard::Z:
		
			game->window.waitEvent(game->eventHandler);
			while (game->eventHandler.type != sf::Event::KeyPressed)
				game->window.waitEvent(game->eventHandler);
			
			switch (game->eventHandler.key.code)
			{
				case sf::Keyboard::Right:
					build(0);
					break;
				case sf::Keyboard::Down:
					build(2);
					break;
				case sf::Keyboard::Left:
					build(4);
					break;
				case sf::Keyboard::Up:
					build(6);
					break;
					
				case sf::Keyboard::Numpad6:
					build(0);
					break;
				case sf::Keyboard::Numpad3:
					build(1);
					break;
				case sf::Keyboard::Numpad2:
					build(2);
					break;
				case sf::Keyboard::Numpad1:
					build(3);
					break;
				case sf::Keyboard::Numpad4:
					build(4);
					break;
				case sf::Keyboard::Numpad7:
					build(5);
					break;
				case sf::Keyboard::Numpad8:
					build(6);
					break;
				case sf::Keyboard::Numpad9:
					build(7);
					break;
			};
			break;
	}
}

void GPlayer::mine(int dir)
{
	int dx = x;
	int dy = y;
	
	switch(dir)
	{
		case 0:		//E
			dx++;
			break;
		case 1:		//SE
			dx++;
			dy++;
			break;
		case 2:		//S
			dy++;
			break;
		case 3:		//SW
			dx--;
			dy++;
			break;
		case 4:		//W
			dx--;
			break;
		case 5:		//NE
			dx--;
			dy--;
			break;
		case 6:		//N
			dy--;
			break;
		case 7:		//NE
			dx++;
			dy--;
			break;
	};
	
	if (dx != x || dy != y)
	{
		if (game->check_block(dx, dy) != AIR)
			game->set_block(dx, dy, dugBlock(dx, dy));
		else
		{
			switch (game->get_floor(dx, dy)->id)
			{
				case DIRT:
				case SAND:
				case GRAVEL:
				case CLAY:
					game->get_floor(dx, dy)->dug = true;
					break;
					
				case GRASS:
					game->set_floor(dx, dy, dugFloor(dx, dy));
					break;
			};
		}
	}
}

unsigned int GPlayer::dugBlock(int x, int y)
{
	switch (game->check_block(x, y))
	{	
		case STONE:
		case WOOD:
		case TREE:
			return AIR;
		
		default:
			return AIR;
	}
}

unsigned int GPlayer::dugFloor(int x, int y)
{
	switch (game->check_floor(x, y))
	{	
		case GRASS:
			return DIRT;
		
		default:
			game->get_floor(x, y)->dug = true;
			return game->check_floor(x, y);
	}
}

void GPlayer::build(int dir)
{
	int dx = x;
	int dy = y;
	
	switch(dir)
	{
		case 0:		//E
			dx++;
			break;
		case 1:		//SE
			dx++;
			dy++;
			break;
		case 2:		//S
			dy++;
			break;
		case 3:		//SW
			dx--;
			dy++;
			break;
		case 4:		//W
			dx--;
			break;
		case 5:		//NE
			dx--;
			dy--;
			break;
		case 6:		//N
			dy--;
			break;
		case 7:		//NE
			dx++;
			dy--;
			break;
	};
	
	if ((dx != x || dy != y) && game->get_voxel(dx, dy)->_Gblock.id != BARRIER)
	{
		game->get_voxel(dx, dy)->_Gblock = tile._Gblock;
		game->get_voxel(dx, dy)->_Gfloor = tile._Gfloor;
	}
}

void GPlayer::simulate()
{	
	if (!isAlive())
	{
		respawn();
		return;
	}
	
	if (!game->check_collision(x, y))
		hp--;
		
	switch(game->check_floor(x, y))
	{
		case WATER:
			if (fp > 0)
				fp--;
			else if (hp > 0){
				hp--;
			}
			break;
		case LAVA:
			if (fp > 0 && hp > 0)
			{
				fp--;
				hp--;
			}
			break;
			
		default:
			if (fp < 20)
				fp++;
			
			if (hp < 10)
				hp++;
			break;
	}
}

void GPlayer::render()
{
	int xstart = x - ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	int ystart = y - ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	int xend = x + ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	int yend = y + ((VIEW_SIZE - BLOCK_PIX) / (2 * BLOCK_PIX));
	
	if (xstart < 0)
		xstart = 0;
	if (ystart < 0)
		ystart = 0;
	if (xend >= game->get_mapw())
		xend = game->get_mapw() - 1;
	if (yend >= game->get_maph())
		yend = game->get_maph() - 1;
		
	int xview = x - 10;
	int yview = y - 10;
	
	if (x <= 10)
		xview = 0;
	if (x >= game->get_mapw() - 11)
		xview = game->get_mapw() - 21;
	if (y <= 10)
		yview = 0;
	if (y >= game->get_maph() - 11)
		yview = game->get_maph() - 21;
	
	std::string buf;
	buf = "x :" + std::to_string(x) + "\ny: " + std::to_string(y) + "\n";
	buf += "hp :" + std::to_string(hp) + "\n";
	buf += "fp :" + std::to_string(fp) + "\n";
	buf += "bl :" + std::to_string(tile._Gblock.id) + "\n";
	buf += "fl :" + std::to_string(tile._Gfloor.id) + "\n";
	buf += "mc :" + std::to_string(game->_entities.size()) + "\n";
	
	game->text_buf.setString(buf);
	//game->text_buf.setPosition(xstart * BLOCK_PIX - (VIEW_SIZE - BLOCK_PIX)/2, ystart * BLOCK_PIX - (VIEW_SIZE - BLOCK_PIX)/2);
	game->text_buf.setPosition(xview * BLOCK_PIX, yview * BLOCK_PIX);
	
	ent_sprite.setPosition(x * BLOCK_PIX, y * BLOCK_PIX);
	//game->window.draw(this->dir_sprite);
	game->window.draw(this->ent_sprite);
	game->window.draw(game->text_buf);
}
