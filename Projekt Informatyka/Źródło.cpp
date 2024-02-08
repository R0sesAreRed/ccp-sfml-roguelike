#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <random>
#include <windows.h>
#include <conio.h>
#include <string>

using namespace std;


//zmienne globalne:

sf::RenderWindow window(sf::VideoMode(800, 600), "Roguelike v1.0");


const short int rs = 13; //room size
int roomx, roomy;
int hcx = 123, hcy = 5, hrx = 0, hry = 10, chrx = hrx, chry = hry; //hero coordiante x, y , hero room x, y, changed hero room x, y


int aix = 0, aiy = 0, aic;
int behp = 8, bedmg = 10, bespd = 2; //podstawowe statystyki przeciwnik�w

int			help = 0, hero = 4; //wyb�r postaci i obs�uga ekwipunku
bool			change = 0;
short int	menuhandler = 0;

int setrand = time(NULL);
int r(int a, int b) //random
{
	mt19937 generator(setrand);
	uniform_int_distribution<int> dist(a, b);
	setrand++;
	return dist(generator);
}

struct enemy //struktura przeciwnik�w
{
	bool type;
	int hp;
	int dmg;
	int spd;
	int item;
	int exp;
	short color;
	string letter;
	int sprite_no;
	bool stunned;
};
enemy en[500] = {};


class Board
{
public:
	int	walls[130][130] = {};
	int	units[130][130] = {};
	int	items[130][130] = {};
};
Board board;

class Player
{
public:
	int hp, dmg, armor, mr, mana, spd, movesleft, lvlupexp = 300;
	int chp = 1, cmana, cexp = 0, lvl = 1, floor_no = 1; //c - current
	const int bhp = 20, bdmg = 5, barmor = 5, bmr = 5, bmana = 0, bspd = 2, branddmg = 0; //base stats, for balancing purposes, const

	void assign_class_stats(int a, int b, int c, int d, int e, int f)
	{
		hp = bhp + a, dmg = bdmg + b, armor = barmor + c, mr = bmr + d, mana = bmana + e, spd = bspd + f, chp = hp, cmana = mana;
	}
};
Player p;

class Walls //�ciany i pod�oga
{
private:
	int w[rs] = { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4 }; //przej�cie mi�dzy pokojami
	int p0[12][12] = { //pok�j 1 (3-1 - �ciana, 0 - pod�oga)
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{4, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 4},
		{4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4},
		{4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
		{4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4},
		{4, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 4},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4}
	};
	int p1[12][12] = { // pok�j 2
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
		{4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4},
	};

public:
	void roomgen(int a, int b, int c[12][12]) //uzupe�nianie segmentu planszy pokojem
	{
		for (int i = 0; i < 12; i++)
		{
			for (int j = 0; j < 12; j++)
			{
				board.walls[a * rs + i][b * rs + j] = c[i][j];
			}
		}
	}

	void wallgenv(int a, int b) //generacja poziomyh przej�� mi�dzy pokojami
	{
		if (b > 0)
		{
			switch (r(0, 3))
			{
			case 0: case 1: case 2:
			{
				for (int i = 0; i < 12; i++)
				{
					board.walls[a * rs + i][b * rs - 1] = w[i];
				}
			}break;
			case 3:
			{
				for (int i = 0; i < 12; i++)
				{
					for (int j = -2; j < 1; j++)
					{
						board.walls[a * rs + i][b * rs + j] = 4;
					}
				}
			}break;
			}
		}
	}

	void wallgenh(int a, int b) //generacja pionowych przej�� im�dzy pokojami
	{
		if (a > 0)
		{
			switch (r(0, 3))
			{
			case 0: case 1: case 2:
			{
				for (int i = 0; i < rs; i++)
				{
					board.walls[a * rs - 1][b * rs + i] = w[i];
				}

			}break;
			case 3:
			{
				for (int i = 0; i < rs; i++)
				{
					for (int j = -2; j < 1; j++)
					{
						board.walls[a * rs + j][b * rs + i] = 4;
					}
				}
			}break;
			}
		}

	}
	void gen()  //generowanie planszy
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				switch (r(0, 1))
				{
				case 0:
				{
					roomgen(i, j, p0);
				}break;
				case 1:
				{
					roomgen(i, j, p1);
				}break;
				}
				wallgenh(i, j);
				wallgenv(i, j);
			}
		}
		for (int i = 0; i < 129; i++) //zewnetrzne �ciany
		{
			board.walls[0][i] = 5;
			board.walls[128][i] = 5;
			board.walls[129][i] = 5;
			board.walls[i][0] = 5;
			board.walls[i][128] = 5;
			board.walls[i][129] = 5;
		}
		board.walls[7][123] = 10; //schody
	}
};
Walls walls;

class Units //przeciwnicy
{
public:

	void loottable(int a, int b) //tabele przedmoit�w przeciwnik�w
	{
		switch (a)
		{
		case 0:
		{
			switch (r(0, 2))
			{
			case 0:
			{
				en[b].item = 1;
			}break;
			case 1:
			{
				en[b].item = 2;
			}break;
			default: {};
			}
		}break;
		case 1:
		{
			switch (r(0, 3))
			{
			case 0:
			{
				en[b].item = 1;
			}break;
			case 1:
			{
				en[b].item = 2;
			}break;
			case 2:
			{
				en[b].item = 3;
			}
			default: {};
			}
		}break;
		}
	}

	void gen() //generowanie przeciwnikow
	{
		int a = 3, b, rx, ry, ru; //random x, random y, random unit
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (!(i == 9 && j == 0))
				{
					b = r(3, 5);
					for (int k = 0; k < b; k++)
					{
						rx = r(1, 10);
						ry = r(1, 10);
						while (board.units[i * rs + rx][j * rs + ry] != 0 || board.walls[i * rs + rx][j * rs + ry] != 0)
						{
							rx = r(1, 10);
							ry = r(1, 10);
						}
						ru = r(3, 6);
						board.units[i * rs + rx][j * rs + ry] = a;

						switch (ru)
						{
						case 3: //Goblin
						{
							en[a].hp = behp - r(2, 4);
							en[a].dmg = bedmg + r(0, 2);
							en[a].spd = bespd + 1;
							en[a].exp = 30;
							en[a].type = 0;
							en[a].letter = " g";
							en[a].sprite_no = 2;
							en[a].color = 10;
							loottable(0, a);
						}break;
						case 4: //Spider
						{
							en[a].hp = behp - r(4, 6);
							en[a].dmg = bedmg + r(0, 1);
							en[a].spd = bespd;
							en[a].exp = 30;
							en[a].type = 0;
							en[a].letter = " S";
							en[a].sprite_no = 3;
							en[a].color = 14;
							loottable(0, a);
						}break;
						case 5: //fiend
						{
							en[a].hp = behp;
							en[a].dmg = bedmg;
							en[a].spd = bespd;
							en[a].exp = 30;
							en[a].type = 0;
							en[a].letter = " F";
							en[a].sprite_no = 4;
							en[a].color = 4;
							loottable(0, a);
						}break;
						case 6: //golem
						{
							en[a].hp = behp + r(5, 7);
							en[a].dmg = bedmg + r(2, 4);
							en[a].spd = bespd - 1;
							en[a].exp = 60;
							en[a].type = 0;
							en[a].letter = " G";
							en[a].sprite_no = 5;
							en[a].color = 8;
							loottable(1, a);
						}break;
						}
						a++;
					}
				}
			}
		}
	}
};
Units units;

class Equipment
{
public:
	int	content[5][4] = { {0, 0, 0, 0 }, {0, 1, 3, 2}, {3, 0, 0, 0}, {1, 0 , 0, 1}, {0, 0, 0, 0} }; //ekwipunek
	int x = 0, y = 0, choicex = 10, choicey = 10;
};
Equipment eq;

class Minimap
{
public:
	int minimaptech[10][10] = {}; //minimapa2
	char map[10][10]; //minimapa
	void mmap() //generowanie minimapy
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				map[i][j] = ((board.walls[i * rs][j * rs + 5]) == 0 ? 0 : 1 ) + ((board.walls[i * rs + 5][(j * rs) + 11] == 0 ? 0 : 1) * 2) + ((board.walls[(i * rs) + 11][j * rs + 5] == 0 ? 0 : 1) * 4) + ((board.walls[i * rs + 5][j * rs] == 0 ? 0 : 1) * 8);
			}
		}
	}
};
Minimap minimap;

struct Item
{
	int hp;
	int dmg;
	int armor;
	int mr;
	int mana;
	int spd;
	bool consumable;
	int sprite_no;
	int eqslot;
	short rarity;
	string opis;
};
struct tempbuff
{
	int hp;
	int dmg;
	int armor;
	int mr;
	int	hmana;
	int spd;
	int randdmg;
	void reset()
	{
		p.hp -= hp; hp = 0;
		if (p.chp > p.hp) p.chp = p.hp;
		p.armor -= armor; armor = 0;
		p.mr -= mr; mr = 0;
		p.mana -= hmana; hmana = 0;
		if (p.cmana > p.mana) p.cmana = p.mana;
		p.spd -= spd; spd = 0;
		p.dmg -= dmg; dmg = 0;
	}
};
tempbuff nextturn = { 0, 0, 0, 0, 0, 0, 0 };
tempbuff nextattack = { 0, 0, 0, 0, 0, 0, 0 };
tempbuff nextroom = { 0, 0, 0, 0, 0, 0, 0 };

Item none{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, };
Item hpdrop{ 15, 0, 0, 0, 0, 0, 1, 10, 0, 1, };
Item chainmail{ 0, 0, 5, 0, 0, 0, 0, 11, 2, 1, "Kolczuga. Zalozenie jej zwieksza pancerz o 5" };
Item hpamulet{ 10, 0, 0, 0, 0, 0, 0, 12, 4, 1, "Amulet zdrowia. Załozenie go zwieksza zdrowie o 10" };
Item item[4] = { none, hpdrop, hpamulet, chainmail };
void dropitem(int a, int b, int d)
{
	int c = 0;
	while (d > 0)
	{
		for (int i = -c; i <= c; i++)
		{
			for (int j = -c; j <= c; j++)
			{
				if (board.items[a + i][b + j] == 0 && board.walls[a + i][b + j] == 0)
				{
					board.items[a + i][b + j] = d;
					d = 0;
				}
			}
		}
		c++;
	}
}

class TextureType
{
public:
	int type = 1;
	sf::Texture texture;
	TextureType(string _texture) {
		texture.loadFromFile("assest/" + _texture + ".png");
	}
};

TextureType texturetype[] = {
	TextureType("player"),	//0
	TextureType("wall"),	//1
	TextureType("enemy1"),	//2
	TextureType("enemy2"),	//3
	TextureType("enemy3"),	//4
	TextureType("enemy4"),	//5
	TextureType("item1"),	//6
	TextureType("item2"),	//7
	TextureType("item3"),	//8
	TextureType("item"),	//9
	TextureType("item1s"),	//10
	TextureType("item2s"),	//11
	TextureType("item3s"),	//12
	TextureType("item8"),	//13
	TextureType("item9"),	//14
	TextureType("item10"),	//15
	TextureType("floor"),	//16
	TextureType("spelloverlay"), //17
	TextureType("emptyspelloverlay"), //18
	TextureType("eq_image"), //19
	TextureType("highlight"), //20
	TextureType("highlight2") //21
};

TextureType map_texture[] = {
	TextureType("map0"),
	TextureType("map1"),
	TextureType("map2"),
	TextureType("map3"),
	TextureType("map4"),
	TextureType("map5"),
	TextureType("map6"),
	TextureType("map7"),
	TextureType("map8"),
	TextureType("map9"),
	TextureType("map10"),
	TextureType("map11"),
	TextureType("map12"),
	TextureType("map13"),
	TextureType("map14"),
	TextureType("map15")
};
TextureType map_overlay("map_overlay");

class Tile {
public:
	int height = 32;
	int width = 32;

	sf::Sprite sprite;
	void set_sprite(int i) {
		sprite.setTexture(texturetype[i].texture);
	}
	void set_map_sprite(int i) {
		sprite.setTexture(map_texture[i].texture);
	}
	void set_position(int x, int y) {
		sprite.setPosition(x, y);
	}
};

class Text {
public:
	sf::Text text;
	sf::Font font;
	sf::Color color;
	float pos_x, pos_y;
	Text(float _pos_x, float _pos_y) {
		pos_x = _pos_x;
		pos_y = _pos_y;
		set_font();
	}
	void set_font() {
		font.loadFromFile("assets/font.ttf");
		text.setFont(font);
		text.setCharacterSize(16);
		text.setFillColor(sf::Color::White);
		text.setPosition(pos_x, pos_y);
	}
};

Text health(450, 330);
Text expe(450, 350);
Text armor(450, 370);
Text magic_res(450, 390);
Text damage(450, 410);
Text speed(450, 430);
Text mana(450, 450);
Text character_select(150,250);
Text description(10, 500);
Text eq_select(550, 500);
Text game_over(230, 230);
Text end_exp(250, 350);

class Event_manager {
public:
	sf::Event event;
	char get_input() {
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::TextEntered) {
					
				switch (event.text.unicode) {
					case 87: case 119:{//up
						return 'u'; break;
					}
					case 65: case 97:{//left
						return 'l';break;
					}
					case 83: case 115: {//down
						return 'd';break;
					}
					case 68: case 100:{//right
						return 'r';break;
					}
					case 69: case 101: {//equipment
						return 'e';break;
					}
					case 77: case 109: {//magic
						return 'm';break;
					}
					case 13: {//confirm
						return 'c';break;
					}
					case 82: case 114: {//turn
						return 't';break;
					}
				}
			}
			else {
				return 'X';
			}
		}
	}
};

Event_manager event_manager;

class Engine
{
public:
	int size_x = 14;
	int size_y = 14;
	int tile_size = 32;
	int eq_tile_size = 72;
	Tile gameboard[14][14] = {};
	Tile overlayboard[14][14] = {1};
	Tile map[10][10];
	Tile map_ol;
	Tile equipment[5][4];
	Tile equipment_highlight[5][4];
	bool in_equipment = false;
	bool display_action = false;
	Tile eq_image;
	//sf::Image icon = sf::Image{};
	Engine() {
		//icon.loadFromFile("player.png");
		//window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		for (int i = 0; i < size_x; i++) {
			for (int j = 0; j < size_y; j++) {
				gameboard[i][j].set_sprite(1);
				gameboard[i][j].set_position(j * tile_size, i * tile_size);
				overlayboard[i][j].set_sprite(18);
				overlayboard[i][j].set_position(j * tile_size, i * tile_size);
				
			}
		}
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 4; j++)
			{
				equipment[i][j].set_position(i * eq_tile_size + 28, j * eq_tile_size + 28);
				equipment_highlight[i][j].set_position(i * eq_tile_size + 28, j * eq_tile_size + 28);
			}
		}
		map_ol.sprite.setTexture(map_overlay.texture);
		set_ui_colors();
		set_equipment();
		eq_select.text.setCharacterSize(20);
		set_gameover();
	}

	float get_board_size() {
		return size_x * tile_size;
	}

	void set_gameboard() {
		for (int i = 0; i <= rs; i++) {
			for (int j = 0; j <= rs; j++) {
				if (board.walls[roomx + i - 1][roomy + j - 1] != 0) {
					gameboard[i][j].set_sprite(1); //walls
				}
				else if (board.units[roomx + i - 1][roomy + j - 1] > 2) {
					gameboard[i][j].set_sprite(en[board.units[roomx + i - 1][roomy + j - 1]].sprite_no); //units
				}
				else if (board.units[roomx + i - 1][roomy + j - 1] == 2) {
					gameboard[i][j].set_sprite(0); //player
				}
				else if (board.items[roomx + i - 1][roomy + j - 1] != 0) {
					gameboard[i][j].set_sprite(item[board.items[roomx + i - 1][roomy + j - 1]].sprite_no); //items
				}
				else {
					gameboard[i][j].set_sprite(16); //floor
				}
			}
		}
	}
	void set_overlayboard(int i, int j)
	{
		overlayboard[i+1][j+1].set_sprite(17);
	}
	void set_equipment() {
		eq_image.set_sprite(19);
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 4; j++)
			{		
				equipment_highlight[i][j].set_sprite(18);				
				switch (eq.content[i][j]) {
				case 0: equipment[i][j].set_sprite(18);break;
				case 1: equipment[i][j].set_sprite(6);break;
				case 2: equipment[i][j].set_sprite(8);break;
				case 3: equipment[i][j].set_sprite(7);break;
				}			
			}
		}
		equipment_highlight[eq.x][eq.y].set_sprite(20);
		equipment_highlight[eq.choicex][eq.choicey].set_sprite(21);
	}
	void draw_gameboard(sf::RenderWindow& _window) {
		for (int i = 0; i < 14; i++) {
			for (int j = 0; j < 14; j++) {
				_window.draw(gameboard[j][i].sprite);
			}
		}
	}
	void draw_overlayboard(sf::RenderWindow& _window) {
		for (int i = 0; i < 14; i++) {
			for (int j = 0; j < 14; j++) {
				_window.draw(overlayboard[j][i].sprite);
			}
		}
	}
	void draw_eq(sf::RenderWindow& _window) {
		_window.draw(eq_image.sprite);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 5; j++)
			{
				_window.draw(equipment[j][i].sprite);
				_window.draw(equipment_highlight[j][i].sprite);
			}
		}
	}
	void reset_overlayboard()
	{
		for (int i = 0; i <= rs; i++) {
			for (int j = 0; j <= rs; j++) {
				overlayboard[i][j].set_sprite(18);
			}
		}

	}
	void set_map() {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				map[i][j].set_map_sprite(minimap.map[i][j]);
				map[i][j].set_position(j * 32 + get_board_size(), i * 32);
				if (i == hrx && j == hry)map_ol.set_position(j * 32 + get_board_size(), i * 32);
			}
		}
	}
	void draw_map(sf::RenderWindow& _window) {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				_window.draw(map[j][i].sprite);
			}
		}
		_window.draw(map_ol.sprite);
	}
	void set_ui_colors() {
		armor.text.setFillColor(sf::Color::Yellow);
		magic_res.text.setFillColor(sf::Color::Cyan);
		damage.text.setFillColor(sf::Color::Red);
		speed.text.setFillColor(sf::Color::Green);
		mana.text.setFillColor(sf::Color::Blue);
		character_select.text.setFillColor(sf::Color::Blue);
		character_select.text.setLetterSpacing(1);
		character_select.text.setCharacterSize(32);
	}
	void set_ui() {
		health.text.setString("Health: " + to_string(p.chp) + "/" + to_string(p.hp));
		expe.text.setString("Exp: " + to_string(p.cexp));
		armor.text.setString("Armor: " + to_string(p.armor));
		magic_res.text.setString("Magic res.: " + to_string(p.mr));
		damage.text.setString("Damage: " + to_string(p.dmg));
		speed.text.setString("Speed: " + to_string(p.movesleft) + "/" + to_string(p.spd));
		mana.text.setString("Mana: " + to_string(p.cmana) + "/" + to_string(p.mana));
	}
	void draw_ui(sf::RenderWindow& _window) {
		_window.draw(health.text);
		_window.draw(expe.text);
		_window.draw(armor.text);
		_window.draw(magic_res.text);
		_window.draw(damage.text);
		_window.draw(speed.text);
		if(display_action)
			_window.draw(eq_select.text);
		if (hero != 0) {
		_window.draw(mana.text);
		}
	}
	void print_character_selection(sf::RenderWindow& _window, string _character_select){
		window.clear();
		character_select.text.setString(_character_select);
		_window.draw(character_select.text);
		window.display();
	}
	void print_description(sf::RenderWindow& _window) {
		description.text.setString(item[eq.content[eq.x][eq.y]].opis);
		_window.draw(description.text);
	}

	void floorprint() {
		window.clear();
		set_gameboard();
		draw_gameboard(window);
		set_map();
		draw_map(window);
		set_ui();
		draw_ui(window);
		if (in_equipment) {
			set_equipment();
			draw_eq(window);
		}
		print_description(window);
		window.display();
	}
	void overlayprint()
	{
		window.clear();
		draw_gameboard(window);
		draw_overlayboard(window);
		window.display();
	}
	void set_gameover() {
		game_over.text.setString("Game Over");
		game_over.text.setCharacterSize(60);
		game_over.text.setFillColor(sf::Color::Red);
		end_exp.text.setCharacterSize(30);
		end_exp.text.setString("    Final score: " + to_string(p.cexp) + "\npress ENTER to quit");
		
	}
	void draw_gameover(sf::RenderWindow& _window) {
		window.clear();
		set_gameover();
		_window.draw(game_over.text);
		_window.draw(end_exp.text);
		_window.display();
	}
};

Engine engine;

int magicmenu, known_spells = 4, magic = 0, magicfirst;
int magicchoice, how_many_enemies, spellx = 0, spelly = 0, spellhelp, rotation;
bool respawn = 0, stealth = 0;

void kulaognia() { //zakelcia maga
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			board.items[roomx + spellx + i][roomy + spelly + j] = 0;
			if (board.units[spellx + i + roomx][spelly + j + roomy] == 2)
			{
				p.chp -= 20;
			}
			else if (en[board.units[spellx + i + roomx][spelly + j + roomy]].hp - 20 <= 0)
			{
				p.cexp += en[board.units[roomx + spellx + i][roomy + spelly + j]].exp;
				board.units[roomx + spellx + i][roomy + spelly + j] = 0;
			}
			else if (en[board.units[spellx + i + roomx][spelly + j + roomy]].hp - 20 > 0)
			{
				en[board.units[spellx + i + roomx][spelly + j + roomy]].hp -= 20;
			}
		}
	}
}
void pancerzmaga() {
	nextroom.armor += 5;
	nextroom.mr += 5;
	p.armor += 5;
	p.mr += 5;
}
void wallhelp(int a, int b)
{
	board.walls[spellx + a + roomx][spelly + b + roomy] = 4;
}
void kamiennasciana() {
	switch (rotation)
	{
	case 0: case 4:
	{
		wallhelp(0, -1);
		wallhelp(0, 0);
		wallhelp(0, 1);

	}break;
	case 1: case 5:
	{
		wallhelp(-1, 1);
		wallhelp(0, 0);
		wallhelp(1, -1);

	}break;
	case 2: case 6:
	{
		wallhelp(1, 0);
		wallhelp(0, 0);
		wallhelp(-1, 0);

	}break;
	case 3: case 7:
	{
		wallhelp(1, 1);
		wallhelp(0, 0);
		wallhelp(-1, -1);

	}break;
	}
}
void pocisklodu() {
	if (en[board.units[roomx + spellx][roomy + spelly]].hp - 2 <= 0)
	{
		dropitem(roomx + spellx, roomy + spelly, en[board.units[roomx + spellx][roomy + spelly]].item);
		p.cexp += en[board.units[roomx + spellx][roomy + spelly]].exp;
		board.units[roomx + spellx][roomy + spelly] = 0;
	}
	else
	{
		en[board.units[roomx + spellx][roomy + spelly]].hp -= 2;
		en[board.units[roomx + spellx][roomy + spelly]].stunned = TRUE;
	}
}
void teleportacja() {
	if (board.walls[spellx + roomx][spelly + roomy] == 1)
	{
		swap(board.units[spellx + roomx][spelly + roomy], board.units[hcx][hcy]);
		p.chp = 0;
	}
	else
	{
		swap(board.units[spellx + roomx][spelly + roomy], board.units[hcx][hcy]);
		hcx = spellx + roomx;
		hcy = spelly + roomy;
	}
}

void uleczrane() { //zaklecia kalpana
	p.chp = p.chp + 20 > p.hp ? p.hp : p.chp + 20;
}
void rozblyskcienia() {

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (board.units[hcx + i][hcy + j] > 2)
			{
				if (en[board.units[hcx + i][hcy + j]].hp - 5 <= 0)
				{
					dropitem(hcx + i, hcy + j, en[board.units[hcx + i][hcy + j]].item);
					p.cexp += en[board.units[hcx + i][hcy + j]].exp;
					board.units[hcx + i][hcy + j] = 0;
					p.hp++;
				}
				else
				{
					en[board.units[hcx + i][hcy + j]].hp -= 5;
				}
				p.chp = p.chp + 5 > p.hp ? p.hp : p.chp + 5;
			}
		}
	}
}
void wybuchswiatla() {
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (board.units[hcx + i][hcy + j] > 2)
			{
				if (board.walls[hcx + 2 * i][hcy + 2 * j] > 0 || board.units[hcx + 2 * i][hcy + 2 * j] != 0)
				{
					if (en[board.units[hcx + i][hcy + j]].hp - 10 <= 0)
					{
						dropitem(hcx + i, hcy + j, en[board.units[hcx + i][hcy + j]].item);
						p.cexp += en[board.units[hcx + i][hcy + j]].exp;
						board.units[hcx + i][hcy + j] = 0;
					}
					else
					{
						en[board.units[hcx + i][hcy + j]].hp -= 10;
					}
				}
				else if (board.units[hcx + 2 * i][hcy + 2 * j] == 0)
				{
					swap(board.units[hcx + i][hcy + j], board.units[hcx + 2 * i][hcy + 2 * j]);
				}
				en[board.units[hcx + i][hcy + j]].stunned = TRUE;
			}
		}
	}

}
void odrodzenie() {
	respawn = 1;
}
void boskiwyrok() {
	dropitem(roomx + spellx, roomy + spelly, en[board.units[roomx + spellx][roomy + spelly]].item);
	p.cexp += en[board.units[roomx + spellx][roomy + spelly]].exp;
	board.units[roomx + spellx][roomy + spelly] = 0;
}

void cioswplecy() { //zaklecia �otra
	nextattack.dmg += 10;
	p.dmg += 10;
}
void sprint() {
	nextturn.spd += 2;
	p.movesleft += 2;
	p.spd += 2;
}
void ukrycie() {
	stealth = 1;
}
void rzutnozem() {
	if (en[board.units[roomx + spellx][roomy + spelly]].hp - p.dmg <= 0)
	{
		dropitem(roomx + spellx, roomy + spelly, en[board.units[roomx + spellx][roomy + spelly]].item);
		p.cexp += en[board.units[roomx + spellx][roomy + spelly]].exp;
		board.units[roomx + spellx][roomy + spelly] = 0;
	}
	else
	{
		en[board.units[roomx + spellx][roomy + spelly]].hp -= p.dmg;
	}
	nextattack.reset();
}
void improwizacja() {
	switch (r(0, 9))
	{
	case 0: {spellx = r(roomx, roomx + 12); spelly = r(roomx, roomx + 12); kulaognia(); }break;
	case 1: {pancerzmaga(); }break;
	case 2: {spellx = r(roomx, roomx + 12); spelly = r(roomx, roomx + 12); rotation = r(0, 7); kamiennasciana(); }break;
	case 3: {while (board.units[spellx][spelly] < 2) { spellx = r(roomx, roomx + 12); spelly = r(roomx, roomx + 12); }pocisklodu(); }break;
	case 4: {spellx = r(roomx, roomx + 12); spelly = r(roomx, roomx + 12); teleportacja(); }break;
	case 5: {uleczrane(); }break;
	case 6: {rozblyskcienia(); }break;
	case 7: {wybuchswiatla(); }break;
	case 8: {odrodzenie(); }break;
	case 9: {while (board.units[spellx][spelly] < 2) { spellx = r(roomx, roomx + 12); spelly = r(roomx, roomx + 12); }boskiwyrok(); }break;
	}
}

class Zaklecie
{
public:
	string spellname;
	int whencast;
	int manacost;
	string opis;
	void(*castt)();
	void cast()
	{
		castt();
	}
	void display(int a, int b)
	{
		switch (a)
		{
		case 2:
		{
			switch (b)
			{
			case 0:
			{
				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						engine.set_overlayboard(spellx + i, spelly + j);
					}
				}
			}break;
			case 2:
			{
				switch (rotation)
				{
				case 0: case 4:
				{
					engine.set_overlayboard(spellx, spelly - 1);
					engine.set_overlayboard(spellx, spelly);
					engine.set_overlayboard(spellx, spelly + 1);

				}break;
				case 1: case 5:
				{
					engine.set_overlayboard(spellx - 1, spelly + 1);
					engine.set_overlayboard(spellx, spelly);
					engine.set_overlayboard(spellx + 1, spelly - 1);

				}break;
				case 2: case 6:
				{
					engine.set_overlayboard(spellx + 1, spelly);
					engine.set_overlayboard(spellx, spelly);
					engine.set_overlayboard(spellx - 1, spelly);

				}break;
				case 3: case 7:
				{
					engine.set_overlayboard(spellx + 1, spelly + 1);
					engine.set_overlayboard(spellx, spelly);
					engine.set_overlayboard(spellx - 1, spelly - 1);

				}break;
				}
			}break;
			case 4:
			{
				engine.set_overlayboard(spellx, spelly);
			}break;
			}
		}
		engine.overlayprint();
		}
	}
	Zaklecie() {}
	Zaklecie(string s, int w, int m, string o, void(*c)()) {
		spellname = s;
		whencast = w;
		manacost = m;
		opis = o;
		castt = c;
	}
};
Zaklecie pliska;
Zaklecie z[3][5] = { {
Zaklecie("Cios w plecy       ", 3, 2, "Zwieksza obrazenia nastepnego ataku lub rzutu nozem", cioswplecy), //3 - natychmiastowe, 2 - namierzane na cel, 1 namierzane na podloge
Zaklecie("Sprint             " , 3, 4, "Zwieksza predkoc ruchu o 2 do nastepnego pokoju", sprint),
Zaklecie("Ukrycie               ", 3, 0, "Przeciwnicy nie widza cie do nastepnego pokoju", ukrycie),
Zaklecie("Rzut nozem           ", 2, 4, "Zadaje obrazenia postaci wybranemu przeciwnikowi w pokoju", rzutnozem),
Zaklecie("Improwizacja          ", 3, 10, "Rzuca losowe zaklecie z innej klasy", improwizacja),
}, {
Zaklecie("Kula ognia        ", 1, 15, "Zadaje duze obrazenia w polu 3x3. Niszczy przedmioty.", kulaognia),
Zaklecie("Pancerz maga        ", 3, 5, "Zwieksza pancerz i odpornosc na magie do nastepnego pokoju", pancerzmaga),
Zaklecie("Kamienna sciana    ", 1, 10, "Tworzy sciane w wybranym miejscu", kamiennasciana),
Zaklecie("Pocisk lodu        ", 2, 5, "Zadaje obazenia na odlegolosc i zamraza przeciwnika na nastepny ruch", pocisklodu),
Zaklecie("Teleportacja        ", 1, 10, "Teleportuje postac na wybrane pole", teleportacja),
}, {
Zaklecie("Ulecz Rane       ", 3, 5, "Przywraca zdrowie", uleczrane),
Zaklecie("Rozblysk cienia     ", 3, 10, "Zadaje obrazenia wszystkim wrogom dookola postaci i zwieksza maksymalne zdrowie o 1 za kazdego zbitego tym zakleciem wroga", rozblyskcienia),
Zaklecie("Wybuch swiatla        " , 3, 10, "Odrzuca wszystkich wrogow dookola bohatera i oglusza ich na nastepna ture", wybuchswiatla),
Zaklecie("Odrodzenie          ", 3, 20, "Zapobiega nastepnemu smiertelnemu ciosowi przeciwnika", odrodzenie),
Zaklecie("Boski Wyrok         ", 2, 20, "Zabija wybranego wroga", boskiwyrok),
} };

void secretrooms()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (minimap.map[i][j] ==15) //pok�j bez wej��
			{
				for (int k = i * rs; k < i * rs + rs; k++)
				{
					for (int l = j * rs; l < j * rs + rs; l++)
					{
						board.units[k][l] = 0;
					}
				}
				units.loottable(1, 0);
				board.items[i * rs + 5][j * rs + 5] = en[0].item;
				units.loottable(1, 0);
				board.items[i * rs + 4][j * rs + 6] = en[0].item;
				units.loottable(1, 0);
				board.items[i * rs + 5][j * rs + 4] = en[0].item;
			}
		}
	}
}

void mapcontrol(int a, int b) //poruszanie sie postaci, zadawanie obra�e�, przedmioty, zmiana pi�tra
{
	if (board.walls[hcx + a][hcy + b] == 0 && board.units[hcx + a][hcy + b] == 0) { board.units[hcx][hcy] = 0; hcy += b; hcx += a; p.movesleft--; }
	else if (board.units[hcx + a][hcy + b] != 0)
	{
		en[board.units[hcx + a][hcy + b]].hp -= p.dmg;
		nextattack.reset();
		if (stealth == 1) stealth = 0;
		if (en[board.units[hcx + a][hcy + b]].hp <= 0)  //przeciwnik ginie
		{
			dropitem(hcx + a, hcy + b, en[board.units[hcx + a][hcy + b]].item);
			p.cexp += en[board.units[hcx + a][hcy + b]].exp; //zdobywanie doswiadczenia za zabijanie przeciwnikow
			board.units[hcx + a][hcy + b] = 0;
		}
		p.movesleft--;
	}
	else if (board.walls[hcx + a][hcy + b] > 0 && board.walls[hcx + a][hcy + b] < 5)
	{
		board.walls[hcx + a][hcy + b]--; p.movesleft--;
		engine.floorprint();
	}
	else if (board.walls[hcx + a][hcy + b] == 10) //zmiana pi�tra
	{
		behp = behp * 2;
		bedmg = bedmg * 2;
		bespd++; //zwi�kszenie statystyk przeciwnik�w, poziomu trudno�ci
		swap(board.units[hcx][hcy], board.units[123][5]); //reset coord�w postaci
		hcx = 123;
		hcy = 5;
		for (int i = 0; i < 130; i++) //czyszczenie tabel
		{
			for (int j = 0; j < 130; j++)
			{
				if (board.units[i][j] > 2)
					board.units[i][j] = 0;
				board.walls[i][j] = 0;
				board.items[i][j] = 0;
			}
		}
		walls.gen();
		minimap.mmap();
		units.gen();
		secretrooms();
		engine.floorprint();
		p.floor_no++;
	}
	if (board.items[hcx][hcy] > 0) //podnoszenie przedmiot�w
	{
		if (item[board.items[hcx][hcy]].consumable == 1) //consumable
		{
			p.chp = p.chp + item[board.items[hcx][hcy]].hp > p.hp ? p.hp : p.chp + item[board.items[hcx][hcy]].hp;
			board.items[hcx][hcy] = 0;
		}
		else
		{
			for (int i = 0; i < 4; i++) //umieszczanie przedmiot�w w ekwipunku
			{
				for (int j = 1; j < 5; j++)
				{
					if (eq.content[j][i] == 0)
					{
						eq.content[j][i] = board.items[hcx][hcy];
						board.items[hcx][hcy] = 0;
					}
				}
			}
		}
	}
}

class AI
{
private:
	static const int max_no_of_enemies = 5;
public:
	int aiused[max_no_of_enemies] = {}, nr; //AI

	void enemyattack(int i, int j, int k, int c) //funkcja ataku przeciwnika
	{
		if (en[board.units[i][j]].type == 0)
		{
			p.chp -= ((en[c].dmg - p.armor) > 0 ? (en[c].dmg - p.armor) : 0);
		}
		else
		{
			p.chp -= ((en[c].dmg - p.mr) > 0 ? (en[c].dmg - p.mr) : 0);
		}
		k = en[c].spd + 1;
	}

	void AII() //AI przeciwnik�w
	{
		nr = 0;
		for (int i = 0; i < max_no_of_enemies; i++)
		{
			aiused[i] = 0;
		}
		for (int i = roomx; i < roomx + 12; i++)
		{
			for (int j = roomy; j < roomy + 12; j++)
			{
				if (board.units[i][j] > 2) //je�eli jednostka istnieje i nie jest gracze, oraz nie zostala juz poruszona
				{
					int c = board.units[i][j];
					if (en[c].stunned == TRUE) //je�eli jednostka nie ma na sobie efktu stunned
					{
						en[c].stunned = FALSE;
						aiused[nr] = c; nr++;
					}
					else if (board.units[i][j] != aiused[0] && board.units[i][j] != aiused[1] && board.units[i][j] != aiused[2] && board.units[i][j] != aiused[3] && board.units[i][j] != aiused[4])
					{
						int dx = 0, dy = 0;
						aiused[nr] = c; nr++;
						for (int k = 0; k < en[c].spd; k++)
						{
							Sleep(50);
							engine.floorprint();
							if (abs((i + dx) - hcx) >= abs((j + dy) - hcy)) //je�eli jednosta jest dalej w poziomie niz w pionie
							{
								if ((i + dx) - hcx < 0) //je�eli jenostka jest na lewo od gracza
								{
									if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
									{
										swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
										dx++;
									}
									else if (board.units[i + 1 + dx][j + dy] == 2) //atak
									{
										enemyattack(i, j, k, c);
									}
									else if ((j + dy) - hcy < 0)
									{
										if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0)
										{
											swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
											dy++;
										}
										else if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0)
										{
											swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
											dy--;
										}
									}
									else if ((j + dy) - hcy > 0)
									{
										if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0)
										{
											swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
											dy--;
										}
										else if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0)
										{
											swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
											dy++;
										}
									}
									else
									{
										switch (r(0, 1))
										{
										case 0:
										{
											if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0)
											{
												swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
												dy++;
											}
											else if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0)
											{
												swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
												dy--;
											}
										}
										case 1:
										{
											if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0)
											{
												swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
												dy--;
											}
											else if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0)
											{
												swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
												dy++;
											}
										}
										}
									}
								}
								else if ((i + dx) - hcx > 0) //je�eli jednosta jest na prawo od gracza
								{
									if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
									{
										swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
										dx++;
									}
									else if (board.units[i - 1 + dx][j + dy] == 2) //je�eli jednostka s�siaduje z bohaterem, atakuj
									{
										enemyattack(i, j, k, c);
									}
									else if ((j + dy) - hcy < 0) //je�eli droga na prz�d jest zablokowana, sprawd� czy jdnostka jest nad...
									{
										if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0) //czy ruch w d� jest moliwy
										{
											swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
											dy++;
										}
										else if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0) //czy r�ch w g�re jest mozliwy
										{
											swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
											dy--;
										}
									}
									else if ((j + dy) - hcy > 0) //...czy pod graczem
									{
										if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0) //czy ruch w g�re jest mo�liwy
										{
											swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
											dy--;
										}
										else if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0) //czy roch w d� jest mo�liwy
										{
											swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
											dy++;
										}
									}
									else //jezeli jednoska jest na wysokosci gracza, ale nie moze isc naprz�d, losowo wybiera kolejno�� ruch�w
									{
										switch (r(0, 1))
										{
										case 0:
										{
											if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0)
											{
												swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
												dy++;
											}
											else if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0)
											{
												swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
												dy--;
											}
										}
										case 1:
										{
											if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0)
											{
												swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
												dy--;
											}
											else if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0)
											{
												swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
												dy++;
											}
										}
										}
									}
								}
							}
							else if (abs(i - hcx) < abs(j - hcy)) //je�eli jedsnota jest dalej w pionie niz w poziomie
							{
								if ((j + dy) - hcy < 0) //jezeli jednostka jest nad graczem
								{
									if (board.units[i + dx][j + 1 + dy] == 0 && board.walls[i + dx][j + 1 + dy] == 0)
									{
										swap(board.units[i + dx][j + 1 + dy], board.units[i + dx][j + dy]);
										dy++;
									}
									else if (board.units[i + dx][j + 1 + dy] == 2)
									{
										enemyattack(i, j, k, c);
									}
									else if ((i + dx) - hcx < 0)
									{
										if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
										{
											swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx++;
										}
										else if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
										{
											swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx--;
										}
									}
									else if ((i + dx) - hcx > 0)
									{
										if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
										{
											swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx--;
										}
										else if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
										{
											swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx++;
										}
									}
									else
									{
										switch (r(0, 1))
										{
										case 0:
										{
											if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
											{
												swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx++;
											}
											else if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
											{
												swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx--;
											}
										}
										case 1:
										{
											if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
											{
												swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx--;
											}
											else if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
											{
												swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx++;
											}
										}
										}
									}
								}
								else if ((j + dy) - hcy > 0) //jezeli jedsnotka jest pod graczem
								{
									if (board.units[i + dx][j - 1 + dy] == 0 && board.walls[i + dx][j - 1 + dy] == 0)
									{
										swap(board.units[i + dx][j - 1 + dy], board.units[i + dx][j + dy]);
										dy++;
									}
									else if (board.units[i + dx][j - 1 + dy] == 2)
									{
										enemyattack(i, j, k, c);
									}
									else if ((i + dx) - hcx < 0)
									{
										if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
										{
											swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx++;
										}
										else if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
										{
											swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx--;
										}
									}
									else if ((i + dx) - hcx > 0)
									{
										if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
										{
											swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx--;
										}
										else if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
										{
											swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
											dx++;
										}
									}
									else
									{
										switch (r(0, 1))
										{
										case 0:
										{
											if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
											{
												swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx++;
											}
											else if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
											{
												swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx--;
											}
										}
										case 1:
										{
											if (board.units[i - 1 + dx][j + dy] == 0 && board.walls[i - 1 + dx][j + dy] == 0)
											{
												swap(board.units[i - 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx--;
											}
											else if (board.units[i + 1 + dx][j + dy] == 0 && board.walls[i + 1 + dx][j + dy] == 0)
											{
												swap(board.units[i + 1 + dx][j + dy], board.units[i + dx][j + dy]);
												dx++;
											}
										}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
};
AI ai;




int main()
{
	char inp = 'X';
	char eqinp = 'X';
	string champion_select;
	walls.gen();
	minimap.mmap();
	units.gen();
	secretrooms();
	help = 0;
	// Start the game loop
	do
	{
		roomx = hrx * rs;
		roomy = hry * rs; //
		switch (menuhandler)
		{
		case 0: //wyb�r postaci
		{
			switch (inp)
			{
			case 'l': //left
			{
				if (help > 0) help--;
			}break;
			case 'r': //right
			{
				if (help < 3) help++;
			}break;
			case 'c':
			{
				hero = help;
				change = 1;
				menuhandler = 1;
			}break;
			}
			if (inp != 'c')
			{
				champion_select = "";
				if (help == 0) champion_select.append(">");
				else champion_select.append(" ");
				champion_select.append("Fighter  ");
				
				if (help == 1) champion_select.append(">");
				else champion_select.append(" ");
				champion_select.append("Rogue  ");
				if (help == 2) champion_select.append(">");
				else champion_select.append(" ");
				champion_select.append("Mage  ");
				if (help == 3) champion_select.append(">");
				else champion_select.append(" ");
				champion_select.append("Cleric  ");
				engine.print_character_selection(window, champion_select);
			}
			else //przypisanie statystyk postaci
			{
				switch (hero)
				{
				case 0:
				{
					p.assign_class_stats(20, 0, 3, 3, 0, 0);
				}break;
				case 1:
				{
					p.assign_class_stats(5, 10, 1, 1, 10, 3);
				}break;
				case 2:
				{
					p.assign_class_stats(0, 10, 0, 0, 30, 1);
				}break;
				case 3:
				{
					p.assign_class_stats(10, 5, 2, 2, 20, 1);
				}break;
				default: {}
				}
			}
			if (change == 0)
			{
				inp = event_manager.get_input();
			}

		}break;
		case 1: //mapa
		{
			change = 0;
			if (p.movesleft > 0)
			{
				switch (inp)
				{
				case 'l': //left
				{
					mapcontrol(0, -1);
				}break;
				case 'r': //right
				{
					mapcontrol(0, 1);
				}break;
				case 'u': //up
				{
					mapcontrol(-1, 0);
				}break;
				case 'd': //down
				{
					mapcontrol(1, 0);
				}break;
				case 'e': //ekwipunek;
				{
					engine.in_equipment = true;
					engine.floorprint();
					menuhandler = 2;
					inp = 'X';
					help = 0;
					change = 1;
				}break;
				case 'm': //zakl�cia
				{
					if (hero != 0) //jeżeli nie jest wojownikiem
					{
						inp = 'X';
						magic = 1;
						magicfirst = 0;
						while (magic == 1)
						{
							switch (inp)
							{
							case 'u': {if (magicmenu > 0) magicmenu--; }break;
							case 'd': {if (magicmenu < known_spells) magicmenu++; }break;
							case 'c':
							{
								inp = 'X';
								if (p.cmana >= z[hero - 1][magicmenu].manacost)
								{
									p.cmana -= z[hero - 1][magicmenu].manacost;
									if (z[hero - 1][magicmenu].whencast == 3) //instant cast
									{
										z[hero - 1][magicmenu].cast();
										magic = 0;
									}
									else if (z[hero - 1][magicmenu].whencast == 1) //wyb�r miejsca
									{
										magic = 2;
										spellx = hcx - roomx;
										spelly = hcy - roomy;
										while (magic == 2)
										{
											engine.reset_overlayboard();
											switch (inp)
											{
											case 'l': //left
											{
												if (spelly >= 0) spelly--;

											}break;
											case 'r': //right
											{
												if (spelly < 12) spelly++;
											}break;
											case 'u': //up
											{
												if (spellx >= 0) spellx--;
											}break;
											case 'd': //down
											{
												if (spellx < 12) spellx++;
											}break;
											case 'c': //zatwierdzenie
											{
												z[hero - 1][magicmenu].cast();
												magic = 0;
											}break;
											case 't': //obracanie (wyko�ystywanie w jedym zakl�ciu)
											{
												if (rotation < 8)rotation++;
												else rotation = 0;
											}break;
											case 'm': //anulowanie
											{
												p.cmana += z[hero - 1][magicmenu].manacost;
												magic = 0;
											}
											}
											pliska.display(hero, magicmenu);

											if (magic != 0)
											{
												inp = event_manager.get_input();
											}
											else
											{
												engine.floorprint();
											}
										}
									}
									else if (z[hero - 1][magicmenu].whencast == 2) //wyb�r przeciwnika
									{
										how_many_enemies = 0;
										magic = 2;
										magicchoice = 0;
										for (int i = rs - 1; i >= -1; i--) //funkcja zliczaj�ca przeciwnikow w pokoju i wybieraj�ca pierwszego z nich
										{
											for (int j = rs - 1; j >= -1; j--)
											{
												if (board.units[roomx + i][roomy + j] > 2)
												{
													how_many_enemies++;
													spellx = i;
													spelly = j;
												}
											}
										}
										if (how_many_enemies == 0)
										{
											magic = 0;
										}
										engine.overlayboard[spellx + 1][spelly + 1].set_sprite(17);
										engine.overlayprint();
										while (magic == 2)
										{
											engine.reset_overlayboard();
											switch (inp)
											{
											case 'l': {if (magicchoice == 0) { magicchoice = how_many_enemies - 1; }
													else { magicchoice--; }
											}break;
											case 'r': {if (magicchoice >= how_many_enemies - 1) { magicchoice = 0; }
													else { magicchoice++; }
											}break;
											case 'c':
											{
												z[hero - 1][magicmenu].cast();
												magic = 0;
											}break;
											case 'm':
											{
												p.cmana += z[hero - 1][magicmenu].manacost;
												magic = 0;
											}break;
											}
											if (magic != 0)
											{
												spellhelp = 0;
												for (int i = -1; i <= rs; i++) //funkcja pozwalaj�ca wybrac przeciwnika
												{
													for (int j = -1; j <= rs; j++)
													{
														if (board.units[roomx + i][roomy + j] > 2)
														{
															spellhelp++;
															if (spellhelp == magicchoice + 1)
															{
																spellx = i;
																spelly = j;
															}
														}
													}
												}
												engine.overlayboard[spellx + 1][spelly + 1].set_sprite(17);
												engine.overlayprint();
												inp = event_manager.get_input();
											}
											else
											{
												engine.floorprint();
											}
										}
									}
									change = 0;
								}
							}break;
							case 'm':
							{
								magic = 0;
								inp = 'X';
							}break;
							}
							if (magicmenu < magicfirst) magicfirst--;
							else if (magicmenu > magicfirst + 2) magicfirst++;
							if (magic != 0)
							{
								engine.display_action = true;
								string spell_list = "";
								if (magicmenu == magicfirst) { spell_list.append(">"); }
								else { spell_list.append(" "); }
								spell_list.append(z[hero - 1][magicfirst].spellname);
								if (magicmenu == magicfirst + 1) { spell_list.append("\n>"); }
								else { spell_list.append("\n "); }
								spell_list.append(z[hero - 1][magicfirst + 1].spellname);
								if (magicmenu == magicfirst + 2) { spell_list.append("\n>"); }
								else { spell_list.append("\n "); }
								spell_list.append(z[hero - 1][magicfirst + 2].spellname); //wypisywanie list zaklec
								eq_select.text.setString(spell_list);
								engine.floorprint();
								engine.display_action = false;
							}
							if (change == 0)
							{
								inp = event_manager.get_input();
							}
						}
					}
				}break;
				} //koniec zakl��
				if (hrx != chrx || hry != chry) //zmiana pokoju
				{
					chrx = hrx;
					chry = hry;
					nextroom.reset();
					if (stealth == true) stealth = false; //reset ukrycia �otra (ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA )				}
				}
			}
			else
			{
				if (stealth == false)
				{
					ai.AII();
				}
				
				p.movesleft = p.spd;
				inp = 'X';
				nextturn.reset(); //reset statystyk
			}
			if (p.cexp >= p.lvlupexp) //zdobywanie poziomow
			{
				p.lvl++;
				p.cexp -= p.lvlupexp;
				p.lvlupexp *= 2;
				p.hp += 5;
				p.armor += 1;
				p.mr += 1;
				p.dmg += 2;
				p.mana += (p.mana > 0 ? 5 : 0);
			}

			if (change == 0)
			{
				hrx = hcx / rs; //hero room x = hero coordinate x / room size
				hry = hcy / rs;
				board.units[hcx][hcy] = 2;
				engine.floorprint();					//printownie pod�ogi(co ruch)
				if (p.movesleft > 0)
				{
					inp = event_manager.get_input();
				}
			}
			if (respawn == 1 && p.chp <= 0) //zaklecie kapalana (ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA ZAKL�CIA )
			{
				respawn = 0; p.chp = 10;
			}
		}break;
		case 2: //ekwipunek
		{
			change = 0;
			switch (inp)
			{
			case 'l': //left
			{
				if (eq.x > 0) eq.x--;
			}break;
			case 'r': //right
			{
				if (eq.x < 4) eq.x++;
			}break;
			case 'u': //up
			{
				if (eq.y > 0) eq.y--;
			}break;
			case 'd': //down
			{
				if (eq.y < 3) eq.y++;
			}break;
			case 'e': //powrot 
			{
				engine.in_equipment = false;
				engine.floorprint();
				inp = 'X';
				menuhandler = 1;
				change = 1;
				cout << setfill(' ');
			}break;
			case 'c': //akcje zwiazane z przedmiotami
			{
				if (help < 3 && eq.content[eq.x][eq.y] != 0)
				{
					while (inp != 'X')
					{
						if (item[eq.content[eq.x][eq.y]].consumable == 1)
						{
							switch (eqinp)
							{
							case 'u': {if (help > 0) help--; }break;
							case 'd': {if (help < 3) help++; }break;
							case 'c':
							{
								switch (help)
								{
								case 0: //uzycie przedmiotu (DODA� RESET STATYSTYK, NIEKT�RE ITEMY MOG� MIEC DZIA�ANIE TYMCZASOWE)
								{
									p.chp = p.chp + item[eq.content[eq.x][eq.y]].hp > p.hp ? p.hp : p.chp + item[eq.content[eq.x][eq.y]].hp;
									p.armor += item[eq.content[eq.x][eq.y]].armor;
									p.mr += item[eq.content[eq.x][eq.y]].mr;
									p.cmana = p.cmana + item[eq.content[eq.x][eq.y]].mana > p.mana ? p.mana : p.cmana + item[eq.content[eq.x][eq.y]].mana;
									p.spd += item[eq.content[eq.x][eq.y]].spd;
									p.dmg += item[eq.content[eq.x][eq.y]].dmg;
									eq.content[eq.x][eq.y] = 0;
									inp = 'X';
								}break;
								case 1:	//przenoszenie
								{
									eq.choicey = eq.y;
									eq.choicex = eq.x;
									help = 3;
									inp = 'X';
								} break;
								case 2: //wyrzucanie
								{
									dropitem(hcx, hcy, eq.content[eq.x][eq.y]);
									eq.content[eq.x][eq.y] = 0;
									inp = 'X';
								} break;
								case 3: //anulowanie 								
								{
									inp = 'X';
								} break;
								}
							}break;
							}
						}
						else
						{
							switch (eqinp)
							{
							case 'u': {if (help > 0) help--; }break;
							case 'd': {if (help < 2) help++; }break;
							case 'c':
							{
								switch (help)
								{
								case 0:	//przenoszenie
								{
									eq.choicey = eq.y;
									eq.choicex = eq.x;
									help = 3;
									inp = 'X';
								} break;
								case 1: //wyrzucanie
								{
									dropitem(hcx, hcy, eq.content[eq.x][eq.y]);
									eq.content[eq.x][eq.y] = 0;
									inp = 'X';
								} break;
								case 2: //anulowanie 								
								{
									inp = 'X';
								} break;
								}
							}break;
							}
						}
						if (eqinp != 'c' && help != 3) //printowanie wyboru akcji przedmiotu (przenies, wyrzuc, uzyj, anuluj)
						{
							engine.display_action = true;
							string select_text = "";
							short int a = 0;
							if (item[eq.content[eq.x][eq.y]].consumable == 1) 
							{
								if (help == a) { select_text.append(">"); }
								else { select_text.append(" "); }
								select_text.append("Uzyj\n"); a++;
							}
							if (help == a) { select_text.append(">"); }
							else { select_text.append(" "); }
							select_text.append("Przenies\n"); a++;
							if (help == a) { select_text.append(">"); }
							else { select_text.append(" "); }
							select_text.append("Wyrzuc\n"); a++;
							if (help == a) { select_text.append(">"); }
							else { select_text.append(" "); }
							select_text.append("Anuluj"); a++;
							eq_select.text.setString(select_text);
							engine.floorprint();
							engine.display_action = false;
						}
						eqinp = event_manager.get_input();
					}
				}
				else if (help == 3)
				{
				
					if (eq.x == 0)
					{
						if (item[eq.content[eq.choicex][eq.choicey]].eqslot == eq.y + 1) //zak�adanie przedmiot�w
						{
							p.hp = p.hp - item[eq.content[eq.x][eq.y]].hp + item[eq.content[eq.choicex][eq.choicey]].hp;
							p.armor = p.armor - item[eq.content[eq.x][eq.y]].armor + item[eq.content[eq.choicex][eq.choicey]].armor;
							p.mr = p.mr - item[eq.content[eq.x][eq.y]].mr + item[eq.content[eq.choicex][eq.choicey]].mr;
							p.mana = p.mana - item[eq.content[eq.x][eq.y]].mana + item[eq.content[eq.choicex][eq.choicey]].mana;
							p.spd = p.spd - item[eq.content[eq.x][eq.y]].spd + item[eq.content[eq.choicex][eq.choicey]].spd;
							p.dmg = p.dmg - item[eq.content[eq.x][eq.y]].dmg + item[eq.content[eq.choicex][eq.choicey]].dmg;
							swap(eq.content[eq.x][eq.y], eq.content[eq.choicex][eq.choicey]);
						}
						else //pr�ba za�o�enie przedmiotu na z�e miejsce
						{
							eq.x = eq.choicex; eq.y = eq.choicey;
						}
					}
					else
					{
						swap(eq.content[eq.x][eq.y], eq.content[eq.choicex][eq.choicey]);
						help = 0;
					}
					help = 0;
					eq.choicex = 10;
					eq.choicey = 10;
				}
				else
				{
					help = 0;
				}
			}break;
			}
			if (change == 0) //wypisywanie ekwipunku
			{
				engine.floorprint();
			}
			if (change == 0)
			{
				inp = event_manager.get_input();
			}

		}break;
		}
	}while (window.isOpen() && p.chp > 0);
	while(event_manager.get_input() !='c')
	engine.draw_gameover(window);

}

//dodatkowe pi�tra, z�oto
