#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>  
#include <time.h>
#define map_size_x 15
#define map_size_y 25

//    SNAKE NA PODSTAWIE LISTY DWUKIERUNKOWEJ

/*TODO:
[krytyczne]:
---------
[opcjonalne]:
	-poziomy
	-jakies podpowiedzi co do klawiszy
*/

HANDLE wHnd;

//poziomy trudnosci
short int easy = 18;
short int medium = 14;
short int hard = 10;

struct point{
	int x,y;
};

struct node{
	struct node *prev;
	struct node *next;
	int x,y, _x, _y;
};

struct snake{
	struct node *head;
    struct node *tail;
    long int score;
	short int count,last_x,last_y;
	bool field[map_size_x][map_size_y]; //miejsca w ktorych aktualnie znajduje sie waz
	bool status; //status gry(false=game over)
};

void board_init(char (*ptr)[map_size_y]);
void board_print(char (*ptr)[map_size_y], COORD begin);
void wait(short int);

void snake_init(struct snake *s);
void snake_set(char (*ptr)[map_size_y], struct snake *s);

void movement(char (*ptr)[map_size_y], struct snake *s, int move_x, int move_y);
void append(struct snake *s, struct point *p, short int difficulty);

void random_point(struct snake *s, struct point *p, char (*ptr)[map_size_y]);
bool compare_point(struct snake *s, struct point *p);
void set_point(struct point *p, char (*ptr)[map_size_y]);

void menu(void);
char menu_option(void);
char menu_difficulty(void);

int main() {
	COORD begin;
	begin.X=0;
	begin.Y=0;
	
	srand(time(NULL));
	short int difficulty;
	difficulty = easy;
	int ch;
	char choice_d,choice,last;
	char tab[map_size_x][map_size_y];
	char (*t_ptr)[map_size_y]= tab;
	
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleTitle(TEXT("Snake v1.0"));
	SMALL_RECT windowSize = {0, 0, 25, 17};
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
    
    // zmienna COORD trzymajaca rozmiar bufora:
    COORD bufferSize = {26, 18};
    // zmiana rozmiarow bufora:
    SetConsoleScreenBufferSize(wHnd, bufferSize);
    // wspolrzedne pozycji menu po przegranej
	COORD game_over;
	game_over.X = 7;
	game_over.Y = 3;
	menu();
	system("cls");
	while(choice!='3')			//menu i inicjacja startowych wartosci
	{
		menu();
		choice = menu_option();
		switch(choice)
		{
			case '1':
				break;
			case '2':
				system("cls");
				menu_difficulty();
				choice_d = menu_option();
				switch(choice_d)
				{
					case '1':
						difficulty = easy;
						break;
					case '2':
						difficulty = medium;
						break;
					case '3':
						difficulty = hard;
						break;
				}
				break;
			case '3':
				return 0;
				break;
		}
		
		struct snake *snake_ptr = (struct snake*)malloc(sizeof(struct snake));
		struct point *point_ptr = (struct point*)malloc(sizeof(struct point));
		board_init(t_ptr);
		snake_init(snake_ptr);
		snake_set(t_ptr,snake_ptr);
		random_point(snake_ptr,point_ptr, t_ptr);
		set_point(point_ptr, t_ptr);
		board_print(t_ptr, begin);	
		ch = 0;
		
		while(!snake_ptr->status) //glowna petla
		{
			if( kbhit() )
			{
				last = ch;
				ch = getch();
				if(ch == 112){
					do{
						ch=last;
						if(kbhit())
							ch = getch();
					}while(ch!=112);
				}
				
				if((ch != 80 && ch!=72&&ch!=75&&ch!=77) || abs(ch-last)==8 || abs(ch-last)==2) //((zly klawisz) || zmiana kierunku o 180* || zmiana kierunku o 180*)
					ch = last;
			}
	
			switch(ch)
			{
				case 80: movement(t_ptr, snake_ptr, 1, 0);	break;  //down
				case 72: movement(t_ptr, snake_ptr, -1,0);	break; //up
				case 75: movement(t_ptr, snake_ptr, 0,-1);	break; //left
				case 77: movement(t_ptr, snake_ptr, 0,1);	break; //right
				default: ;
			}
			board_print(t_ptr,begin);
			set_point(point_ptr, t_ptr); //fix rzadkiego buga gdy punkt znikal z mapy
			if( compare_point(snake_ptr, point_ptr) )
			{
				Beep(300,30);
				append(snake_ptr,point_ptr,difficulty);	
				random_point(snake_ptr,point_ptr, t_ptr);
				set_point(point_ptr, t_ptr);
			}
			
			wait(difficulty);
			
			if( kbhit() )
			{
				last = ch;
				ch = getch();
				if(ch == 112){
					do{
						ch=last;
						if(kbhit())
							ch = getch();
					}while(ch!=112);
				}
				if((ch != 80 && ch!=72&&ch!=75&&ch!=77) || abs(ch-last)==8 || abs(ch-last)==2) //((zly klawisz) || zmiana kierunku o 180* || zmiana kierunku o 180*)
					ch = last;
			}
		printf("Wynik: %d\t\n", snake_ptr->score);
		} //koniec petli gry
		
		SetConsoleCursorPosition(wHnd,game_over);
		printf("Przegrales!\n");
		free(snake_ptr);
		free(point_ptr);
	}//koniec menu
}

void board_init(char (*ptr)[map_size_y]) //wypelnienie tablicy(mapy)
{
	int i,j;
	
	for(i=0;i<map_size_x;i++)
	{
		for(j=0;j<map_size_y;j++)
		{
			if(i==0 || i==(map_size_x-1))  //gorna i dolna krawedz mapy
				ptr[i][j] = 205;
			else if(j==0 || j==(map_size_y-1))   //boczne krawedzi mapy
				ptr[i][j] = 186;
			else
				ptr[i][j] = 255; //srodek mapy(pusty znak)
		
		//corners
			if( i==0 && j==0 )
				ptr[i][j] = 201;
			if( i==0 && j==(map_size_y-1) )
				ptr[i][j] = 187;
			if( i==(map_size_x-1) && j==0 )
				ptr[i][j] = 200;
			if( i==(map_size_x-1) && j==(map_size_y-1) )
				ptr[i][j] = 188;
		}
	}
}

void snake_init(struct snake *s) //chyba mozna to bylo krocej napisac ale boje sie bo dziala
{
	int i,j;
	struct node *_head = (struct node*)malloc(sizeof(struct node));
	struct node *_tail = (struct node*)malloc(sizeof(struct node));
	_head->x = 5;
	_head->y = 5;
	_head->_x = 5;
	_head->_y = 5;
	_head->next = NULL;
	_head->prev = NULL;
	s->head = _head;
	s->tail = NULL;
	s->last_x = 5;
	s->last_y = 5;
	s->count = 1;
	s->score = 1;
	s->status = false;
	
	for(i=0;i<map_size_x;i++)
		for(j=0;j<map_size_y;j++)
			s->field[i][j] = false;
	s->field[5][5] = true;
}

void board_print(char (*ptr)[map_size_y], COORD begin)
{
	int i,j;
	SetConsoleCursorPosition(wHnd,begin);
	for(i=0;i<map_size_x;i++){
		for(j=0;j<map_size_y;j++)
			printf("%c", ptr[i][j]);	
		puts("");
	}
}

void wait(short int difficulty)
{
    clock_t koniec = clock() + difficulty * CLOCKS_PER_SEC/100;
    while( clock() < koniec ) continue;  
}

void snake_set(char (*ptr)[map_size_y], struct snake *s) //ustawia glowke na poczatkowe koordynaty
{
	ptr[s->head->_x][s->head->_y] = 178;
}

void movement(char (*ptr)[map_size_y], struct snake *s, int move_x, int move_y) //efekt kilku dni debugowania i pisania wszystkiego od nowa
{
	if( s->head->_x >= map_size_x-1 ||s->head->_x <= 0 || s->head->_y >= (map_size_y-1) || s->head->_y <= 0) //warunki konczace gre
	{
			s->status=true;
			Beep(100,400);
	}
	else
	{
		struct node *element = (struct node*)malloc(sizeof(struct node));
		element = s->head;
		element->_x += move_x;
		element->_y += move_y;
		do
		{	
			ptr[element->_x][element->_y] = 178;
			//gdy waz ma jeden element
			if(s->tail==NULL)
			{
				ptr[element->x][element->y] = 0;
				s->last_x = element->x;
				s->last_y = element->y;
				element->x = element->_x;
				element->y = element->_y;
			}
			//
			if(element->prev)
			{
				element = element->prev;
				if(element->prev==NULL)
				{
					s->last_x = element->_x;
					s->last_y = element->_y;
					s->tail->_x = element->next->x;
					s->tail->_y = element->next->y;
					s->tail->x = s->last_x;
					s->tail->y = s->last_y;
					element->next->x = element->next->_x;
					element->next->y = element->next->_y;
					ptr[s->last_x][s->last_y] = 0;
				}
				else{
		
					element->_x = element->next->x;
					element->_y = element->next->y;
					element->next->x = element->next->_x;
					element->next->y = element->next->_y;
				}
			}
			
			if(s->tail)
				if( (s->head->_x==element->_x && s->head->_y==element->_y) ) //kolizja z wezem
				{
					Beep(100,400);
					s->status=true;
				}
			s->field[element->_x][element->_y] = true;
			s->field[s->last_x][s->last_y] = false;
			
		}while(element->prev);
	}
}

void random_point(struct snake *s, struct point *p, char (*ptr)[map_size_y]) //randomuje punkt i sprawdza czy pojawil sie na wezu
{
	p->x = ( rand() % (map_size_x-2) ) + 1;
	p->y = ( rand() % (map_size_y-2) ) + 1;
	if(s->field[p->x][p->y] )
		random_point(s,p,ptr);
}

void set_point(struct point *p, char (*ptr)[map_size_y])
{
	ptr[p->x][p->y] = 158;
}

bool compare_point(struct snake *s, struct point *p)
{
	if(s->head->_x == p->x && s->head->_y == p->y)
		return true;
	return false;
}

void append(struct snake *s, struct point *p, short int difficulty) //dodaje element na koniec weza
{
	struct node *element = (struct node*)malloc(sizeof(struct node));
	element->prev = NULL;
	if(s->tail==NULL){ //gdy tail nie istnieje(sama glowa)
		element->_x = s->head->_x;
		element->_y = s->head->_y;
		element->next = s->head;
		s->head->prev = element;
		s->tail = element;
	}
	else
	{
		element->_x = s->tail->_x;
		element->_y = s->tail->_y;
		element->next = s->tail;
		s->tail->prev = element;
		s->tail = element;
	}
	
	s->count++;
	
	if(difficulty==easy)
		s->score += (s->count*0.4) * (difficulty*0.1);
	else if(difficulty==medium)
		s->score += (s->count*0.4) * (difficulty*0.2);
	else
		s->score += (s->count*0.4) * (difficulty*0.4);
}

void menu(void)
{
	SetConsoleTextAttribute(wHnd, BACKGROUND_GREEN | BACKGROUND_RED  | FOREGROUND_RED);
	printf("\n\n\n");
	printf("-------------------------\n");
	printf("[1] Nowa gra\t\t\n");
	printf("[2] Poziom trudnosci\t\n");
	printf("[3] Wyjscie\t\t\n");
	printf("-------------------------\n");
}

char menu_option(void)
{
	char choice;
	do{
		choice = getch();
		switch( choice )
		{
			case '1':
			case '2':
			case '3': return(choice);
			default:
				choice='0';
		}
	}while( choice == '0');
	return choice;
}

char menu_difficulty(void)
{
	printf("\n");
	printf("-------------------------\n");
	printf("[1] Latwy\t\t\n");
	printf("[2] Sredni\t\n");
	printf("[3] Trudny\t\t\n");
	printf("-------------------------\n");
}
