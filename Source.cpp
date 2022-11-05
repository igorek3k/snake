#include <SFML\Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

#define FIELD_CELL_TYPE_NONE 0
#define FIELD_CELL_TYPE_APPLE -1
#define SNAKE_DIRECTION_UP 0
#define SNAKE_DIRECTION_RIGHT 1
#define SNAKE_DIRECTION_DOWN 2
#define SNAKE_DIRECTION_LEFT 3
#define FIELD_CELL_TYPE_WALL -2 
#define MENU_ITEM_START "Start new game"
#define MENU_ITEM_RESUME "Resume"
#define MENU_ITEM_QUIT "Quit"
#define MENU_MAIN 0



const int field_size_x = 30; //Кол-во клетов в ширину 
const int field_size_y = 20; //Кол-во клеток в высоту 
const int cell_size = 32; //Размер клетки
const int bar_height = 35; //Высота меню для счета
const int window_width = field_size_x * cell_size; //Размер окна в ширину
const int window_height = field_size_y * cell_size + bar_height; //Размер окна в высоту


int field[field_size_y][field_size_x]; //Поле
int snake_position_x; //Стартовое положение змейки по ширине
int snake_position_y; //Стартовое положение змейки по высоте
int snake_lenght = 0; //Длина змейки
int snake_direction = SNAKE_DIRECTION_RIGHT; //Сохранение направления движений змейки
int score = 0; //Счет
int current_main_menu_item_index = 0; // Текущая строка в основном меню 
int game_over_timeout = 0;
int current_menu = MENU_MAIN;
int aet_apple = 0;
bool game_started = false; //Продолжение
bool game_over = false; //Конец игры
bool game_paused = true; //Пауза


Texture snake_texture;
Sprite snake;

Texture head_texture;
Sprite head;

Texture none_texture;
Sprite none;

Texture apple_texture;
Sprite apple;

Texture wall_texture;
Sprite wall;

Font font;
Text text_score;
Text text_title;
Text text_game_over;
vector<Text> text_main_menu_items;
vector<string>main_menu_items = { "Start new game", "Quit" };

int get_random_empty_cell() 
{
	int empty_cell_count = 0; //Счетчик пустых клеток
	for (int j = 0; j < field_size_y; j++) {
		for (int i = 0; i < field_size_x; i++) {
			if (field[j][i] == FIELD_CELL_TYPE_NONE) {
				empty_cell_count++;
			}
		}
	}
	int target_empty_cell_index = rand() % empty_cell_count; //Генерация случайного числа
	int empty_cell_index = 0;
	for (int j = 0; j < field_size_y; j++) {
		for (int i = 0; i < field_size_x; i++) {
			if (field[j][i] == FIELD_CELL_TYPE_NONE) {
				if (empty_cell_index == target_empty_cell_index) {
					return j * field_size_x + i;
				}
				empty_cell_index++;
			}
		}
	}
	return -1;
}

void add_apple() //Яблоко
{
	int apple_pos = get_random_empty_cell();
	cout << apple_pos << endl;
	if (apple_pos != -1) {
		field[apple_pos / field_size_x][apple_pos % field_size_x] = FIELD_CELL_TYPE_APPLE;
	}
	
}

void clear_field()//Счетчик поля
{
	for (int j = 0; j < field_size_y; j++) { //Все пустые клетки 
		for (int i = 0; i < field_size_x; i++) {
			field[j][i] = FIELD_CELL_TYPE_NONE;
		}
	}
	for (int i = 0; i < snake_lenght; i++) { //Голова змеи
		field[snake_position_y][snake_position_x - i] = snake_lenght - i;
	}
	for (int i = 0; i < field_size_x; i++) { //Верхняя и нижняя стены
		if (i < 5 || field_size_x - i - 1 < 5) {
			field[0][i] = FIELD_CELL_TYPE_WALL;
			field[field_size_y - 1][i] = FIELD_CELL_TYPE_WALL;
		}
	}
	for (int i = 1; i < field_size_y - 1; i++) { //Боковые стены 
		if (i < 5 || field_size_y - i - 1 < 5) {
			field[i][0] = FIELD_CELL_TYPE_WALL;
			field[i][field_size_x - 1] = FIELD_CELL_TYPE_WALL;
		}
	}
	add_apple(); //Добавление яблока
}

void init_game() //Загрузк текстур
{
	srand(time(NULL));

	clear_field();

	head_texture.loadFromFile("images/head.png");
	head.setTexture(head_texture);

	snake_texture.loadFromFile("images/snake.png");
	snake.setTexture(snake_texture);

	none_texture.loadFromFile("images/none.png");
	none.setTexture(none_texture);

	apple_texture.loadFromFile("images/apple.png");
	apple.setTexture(apple_texture);

	wall_texture.loadFromFile("images/wall.png");
	wall.setTexture(wall_texture);

	font.loadFromFile("font/Pixeloid.ttf");
	
	text_score.setFont(font);
	text_score.setCharacterSize(20);
	text_score.setFillColor(Color::Black);
	
	text_title.setFont(font);
	text_title.setString("Snake");
	text_title.setCharacterSize(30);
	text_title.setFillColor(Color::Black);
	text_title.setPosition(0,0);

	text_game_over.setFont(font); // GAME_OVER
	text_game_over.setString("GAME OVER");
	text_game_over.setCharacterSize(140);
	text_game_over.setFillColor(Color::Red);
	text_game_over.setPosition((window_width - text_game_over.getLocalBounds().width) / 2, (window_height - text_game_over.getLocalBounds().height + bar_height) / 2);
	
    for (int i = 0; i < main_menu_items.size(); i++) { // Основное Меню
		text_main_menu_items.emplace_back(Text());
		text_main_menu_items.back().setString(main_menu_items.at(i));
		text_main_menu_items.back().setFont(font);
		text_main_menu_items.back().setCharacterSize(40);

	}
}

void start_game()
{
	snake_position_x = field_size_x / 2; //Стартовое положение змейки по ширине
	snake_position_y = field_size_y / 2; //Стартовое положение змейки по высоте
	snake_lenght = 5; //Длина змейки
	snake_direction = SNAKE_DIRECTION_RIGHT; //Сохранение направления движений змейки
	score = 0; //Счет
    game_started = true;
	game_over = false;
	game_paused = false;
	clear_field();
}

void finish_game()
{
	game_over = true;
	game_paused = true;
	game_over_timeout = 20;
	current_main_menu_item_index = 0; 
}

void draw_field(RenderWindow &window) //Отрисовка текстур на поле 
{
	for (int j = 0; j < field_size_y; j++) {
		for (int i = 0; i < field_size_x; i++) {
			switch (field[j][i]){
			    case FIELD_CELL_TYPE_NONE:  //При нулевом значении выводится пустая клетка
					none.setPosition(float(i * cell_size), float(j * cell_size + bar_height));
					window.draw(none);
				    break;
				case FIELD_CELL_TYPE_APPLE:  //При нулевом значении выводится яблоко
					apple.setPosition(float(i * cell_size), float(j * cell_size + bar_height));
					window.draw(apple);
					break;
				case FIELD_CELL_TYPE_WALL:  //При нулевом значении выводится стенка
					wall.setPosition(float(i * cell_size), float(j * cell_size + bar_height));
					window.draw(wall);
					break;
				default: //При иных случаях выводится сама змейка
					if (field[j][i] == snake_lenght) { //Голова
						head.setPosition(float(i * cell_size + head.getLocalBounds().width / 2), float(j * cell_size + bar_height+ head.getLocalBounds().height / 2));
						head.setOrigin(head.getLocalBounds().width / 2, head.getLocalBounds().height / 2);
						switch (snake_direction) {
						case SNAKE_DIRECTION_UP:
							head.setRotation(0);
							break;
						case SNAKE_DIRECTION_RIGHT:
							head.setRotation(90);
							break;
						case SNAKE_DIRECTION_LEFT:
							head.setRotation(-90);
							break;
						case SNAKE_DIRECTION_DOWN:
							head.setRotation(180);
							break;
						}
						window.draw(head);
					}
					else { //Остальная часть тела
						snake.setPosition(float(i * cell_size), float(j * cell_size + bar_height));
						window.draw(snake);
					}
             
						
            }
		}
	}
}

void draw_bar(RenderWindow& window) //счетчик сьеденных яблок
{
	text_score.setString("Score: " + to_string(score));
    text_score.setPosition(window_width - text_score.getLocalBounds().width - 5, 5);
	window.draw(text_score);

	window.draw(text_title);
}

void draw_main_menu(RenderWindow& window) //Основное меню
{
	float const menu_padding_horizontal = 40;
	float const menu_padding_vertical = 40;
	float const menu_item_interval = 20;
	float menu_item_max_width = 0;
	float current_menu_item_offset_y = 0;
	for (int i = 0; i < text_main_menu_items.size(); i++) {
		if (main_menu_items.at(i) == MENU_ITEM_START) {
			if (!game_over && game_started) {
				text_main_menu_items.at(i).setString(MENU_ITEM_RESUME);
			}
			else {
				text_main_menu_items.at(i).setString(MENU_ITEM_START);
			}
		}
		text_main_menu_items.at(i).setPosition(0, current_menu_item_offset_y);
		text_main_menu_items.at(i).setFillColor(current_main_menu_item_index == i ? Color(224, 224, 224) : Color(128,128,128));
		current_menu_item_offset_y += text_main_menu_items.at(i).getLocalBounds().height + menu_item_interval;
		menu_item_max_width = max(menu_item_max_width,text_main_menu_items.at(i).getLocalBounds().width);
	}
	float const menu_width = menu_item_max_width+ menu_padding_horizontal * 2; 
	float const menu_height = current_menu_item_offset_y- menu_item_interval + menu_padding_vertical * 2;
	float const menu_position_x = (window_width - menu_width) / 2; 
	float const menu_position_y = (window_height - menu_height) / 2;

	RectangleShape menu_rectangle(Vector2f(menu_width,menu_height));
	menu_rectangle.setPosition(menu_position_x,menu_position_y); 
	menu_rectangle.setFillColor(Color(0, 0, 0, 224));
	window.draw(menu_rectangle);

	for (int i = 0; i < text_main_menu_items.size(); i++) {
		text_main_menu_items.at(i).move(menu_position_x + menu_padding_horizontal, menu_position_y + menu_padding_vertical);
		window.draw(text_main_menu_items.at(i));
	}
}

void grow_snake() // Рост змейки
{
	for (int j = 0; j < field_size_y; j++) {
		for (int i = 0; i < field_size_x; i++) {
			if (field[j][i] > FIELD_CELL_TYPE_NONE) {
				field[j][i]++;
				
			}
		}
	}
}

void make_move() //Перемещение змейки
{
	switch (snake_direction) {
	    case SNAKE_DIRECTION_UP:
			snake_position_y--;
			if (snake_position_y < 0) {
				snake_position_y = field_size_y - 1;
			}
			break;
		case SNAKE_DIRECTION_RIGHT:
			snake_position_x++;
			if (snake_position_x > field_size_x - 1) {
				snake_position_x = 0;
			}
			break;
		case SNAKE_DIRECTION_DOWN:
			snake_position_y++;
			if (snake_position_y > field_size_y - 1) {
				snake_position_y = 0;
			}
			break;
		case SNAKE_DIRECTION_LEFT:
			snake_position_x--;
			if (snake_position_x < 0) {
				snake_position_x = field_size_x - 1;
			}
			break;
	}
	if (field[snake_position_y][snake_position_x] != FIELD_CELL_TYPE_NONE) { //Трекер головы змейки
		switch (field[snake_position_y][snake_position_x]){ 
		    case FIELD_CELL_TYPE_APPLE: //если сьела яблоко
				snake_lenght++;
				score++;
				grow_snake();
				add_apple();
				break;
			case FIELD_CELL_TYPE_WALL:
				finish_game(); 
				break;
		    default: //если столкулась с собой
				if (field[snake_position_y][snake_position_x] > 1) {
					finish_game();
				}
		}
	}

	if (!game_over){
		for (int j = 0; j < field_size_y; j++) { //Удаление прошлого положения хвоста
			for (int i = 0; i < field_size_x; i++) {
				if (field[j][i] > FIELD_CELL_TYPE_NONE) {
					field[j][i]--;
				}
			}
		}
		field[snake_position_y][snake_position_x] = snake_lenght; //Новое положение головы
	}	
}

int main()
{
	init_game();

	RenderWindow window(VideoMode(window_width,window_height), "Snake",Style::Close);

	vector<int> snake_direction_queue;

	while (window.isOpen())
	{
		 Event event;
		while (window.pollEvent(event))
		{
			if (event.type ==  Event::Closed)
				window.close();
			if (event.type ==  Event::KeyPressed) {
				if (game_paused) {
					if (game_over_timeout == 0) {
						if (current_menu == MENU_MAIN) {
							switch (event.key.code) {
								case  Keyboard::Up:
								current_main_menu_item_index--;
								if (current_main_menu_item_index < 0) {
									current_main_menu_item_index = text_main_menu_items.size() - 1;
								}
								break;
							case  Keyboard::Down:
								current_main_menu_item_index++;
								if (current_main_menu_item_index > text_main_menu_items.size() - 1) {
									current_main_menu_item_index = 0;
								}
								break;
							case  Keyboard::Enter:
								if (main_menu_items.at(current_main_menu_item_index) == MENU_ITEM_START) {
									if (!game_over && game_started) {
										game_paused = false;
									}
									else {
										start_game();
									}
								}
								if (main_menu_items.at(current_main_menu_item_index) == MENU_ITEM_QUIT) {
									window.close();
								}
								break;
							case  Keyboard::Escape:
								
								if (!game_over && game_started) {
									game_paused = false;
								}
								break;
							}
						}
						
					}
					else {
						game_over_timeout = 0;
					}
				}
				else {
					int snake_direction_last = snake_direction_queue.empty() ? snake_direction : snake_direction_queue.at(0);
					switch (event.key.code) {
					case  Keyboard::Up:
						if (snake_direction_last != SNAKE_DIRECTION_UP && snake_direction_last != SNAKE_DIRECTION_DOWN) {
							if (snake_direction_queue.size() < 2) {
								snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_UP);
							}
						}
						break;
					case  Keyboard::Right:
						if (snake_direction_last != SNAKE_DIRECTION_RIGHT && snake_direction_last != SNAKE_DIRECTION_LEFT) {
							if (snake_direction_queue.size() < 2) {
								snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_RIGHT);
							}
						}
						break;
					case  Keyboard::Down:
						if (snake_direction_last != SNAKE_DIRECTION_DOWN && snake_direction_last != SNAKE_DIRECTION_UP) {
							if (snake_direction_queue.size() < 2) {
								snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_DOWN);
							}
						}
						break;
					case  Keyboard::Left:
						if (snake_direction_last != SNAKE_DIRECTION_LEFT && snake_direction_last != SNAKE_DIRECTION_RIGHT) {
							if (snake_direction_queue.size() < 2) {
								snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_LEFT);
							}
						}
						break;
					case  Keyboard::Escape:
						game_paused = true;
						break;
					}
				}
			}
		}
		if (!snake_direction_queue.empty()) {
			snake_direction = snake_direction_queue.back();
			snake_direction_queue.pop_back();
		}

		if (!game_paused) {
			make_move();
		}

		window.clear( Color(183, 212, 168));

		draw_field(window);
		draw_bar(window);

		if (game_over) {
			window.draw(text_game_over);
			if (game_over_timeout > 0) {
				game_over_timeout--;
			}
		}

		if (game_paused && game_over_timeout == 0) {
			switch (current_menu) {
			case MENU_MAIN:
				draw_main_menu(window);
				break;
			}
		}
		

		window.display();

		
	    sleep(milliseconds(250));
		
	}
	
}