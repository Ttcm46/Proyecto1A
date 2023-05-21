#include <stdio.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

typedef struct bala {
	int x = -1;
	int y = -1;
	int velocidad = 2;
	bool direccion = false;		//1 iz, 0 der
	bool estado = true;
	bool playerShot = false;
	int hurtboxX = 12;
	int hurtboxY = 12;
	bala* suiguiente = NULL;
	bala* anterior = NULL;
}*PtrBala;

typedef struct nave {
	int codigo = -1;
	int x = 1000;
	int y = 400;
	int velocidad = 10;
	bool estado = true;
	int hitBoxX = 50;
	int hitBoxY = 25;
	nave* siguiente = NULL;
	int disparos = 0;
	int enemigosDestruidos = 0;
	int missed = 0;
}*PtrNave;

typedef struct Enemigo {
	int codigoimagen = -1;
	int x = -1;
	int y = -1;
	bool estado = false;
	int velocidad = 0;
	int hitboxX = 50;
	int hitboxY = 50;
	Enemigo* siguiente = NULL;
	unsigned int lifeTime = 9500000;
	ALLEGRO_BITMAP* img = NULL;
	ALLEGRO_BITMAP* img_d = NULL;
}*PtrEnemigo;

const float FPS = 20.0;	//fps 
PtrBala balas;
PtrNave vidas;
PtrEnemigo enemigos;
PtrNave actual;
bool left, right, up, down;
int offsetx = 1000; int offsety = 0;
int absx = 1000;
int absy = 400;
 
void drawLifes(ALLEGRO_BITMAP* ship) {
	int i= (vidas->estado) ? (3) : ((vidas->siguiente->estado) ? (2) : ((vidas->siguiente->siguiente->estado) ? (1) : (0)));
	int x = 0;
	for (int j = 0; j < i; j++)
	{
		al_draw_bitmap(ship, x, 725, 0);
		x += 100;
	}
}
void drawBullets(ALLEGRO_BITMAP* bullet) {
	PtrBala balAct = balas;
	while (balAct)
	{	
		if (balAct->x > offsetx-600 && balAct->x < (offsetx + 600)) {
			al_draw_bitmap(bullet, ((balAct->x) - (offsetx-600)) - 12, balAct->y-12, 0);
		}
		balAct->x += (balAct->direccion) ? (-1*(balAct->velocidad)) : (balAct->velocidad);
		balAct = balAct->suiguiente;
	}
}
void disparar(int x,int y,bool iz,bool user) {
	PtrBala tmp = new(bala);
	tmp->x = x;
	tmp->y = y;
	tmp->direccion = iz;
	tmp->suiguiente = balas;
	tmp->playerShot = user;
	if (tmp->suiguiente)tmp->suiguiente->anterior = tmp;
	tmp->anterior = NULL;;
	balas = tmp;
	
}
void eleminarBalaInact() {
	PtrBala tmp = balas;
	PtrBala tmp_B;
	while (tmp)
	{
		if (!tmp->estado) {
			if (tmp == balas) balas = tmp->suiguiente;
			if (tmp->suiguiente) tmp->suiguiente->anterior = tmp->anterior;
			if (tmp->anterior)tmp->anterior->suiguiente = tmp->suiguiente;
			tmp_B = tmp->suiguiente;
			free(tmp);
			tmp = tmp_B;
		}
		else {
			tmp = tmp->suiguiente;
		}
	}
}
void eliminarBalas() {
	PtrBala tmp = balas;
	while (tmp) {
		if (tmp->x > (2000-tmp->hurtboxX) || tmp->x < (0+tmp->hurtboxX)) 
			tmp->estado = false;
		tmp = tmp->suiguiente;
	}
	eleminarBalaInact();
}

void spawnEnems() {
	//TODO: hacerla :/
}
void bulletHits(ALLEGRO_DISPLAY* display) {		//TODO: finish
	PtrBala tmp = balas;
	PtrEnemigo tmpEnem= enemigos;
	while (tmp){
		//espresion para comprobar impactos;
		//std::cout << "condX: " << (actual->x + actual->hitBoxX > tmp->x - tmp->hurtboxX && actual->x - actual->hitBoxX < tmp->x + tmp->hurtboxX) << " condY: " << (actual->y + actual->hitBoxY > tmp->y - tmp->hurtboxY && actual->y - actual->hitBoxY < tmp->y + tmp->hurtboxY) << std::endl;
		while (tmpEnem) {
			if (tmp->playerShot && tmp->estado && (tmpEnem->x + tmpEnem->hitboxX > tmp->x - tmp->hurtboxX && tmpEnem->x - tmpEnem->hitboxX < tmp->x + tmp->hurtboxX)&& (tmpEnem->y + tmpEnem->hitboxY > tmp->y - tmp->hurtboxY && tmpEnem->y - tmpEnem->hitboxY < tmp->y + tmp->hurtboxY)) {
				actual->enemigosDestruidos += 1;
				tmpEnem->estado = false;

				//TODO: comprobar funcionamiento
			}
			else if ((!tmp->playerShot) && tmp->estado && (actual->x + actual->hitBoxX > tmp->x - tmp->hurtboxX && actual->x - actual->hitBoxX < tmp->x + tmp->hurtboxX) && (actual->y + actual->hitBoxY > tmp->y - tmp->hurtboxY && actual->y - actual->hitBoxY < tmp->y + tmp->hurtboxY)) {
				actual->estado = false;
				actual = (vidas->estado) ? (vidas) : ((vidas->siguiente->estado) ? (vidas->siguiente) : ((vidas->siguiente->siguiente->estado) ? (vidas->siguiente->siguiente) : (NULL)));
				tmp = balas;
				while (tmp) {
					tmp->estado = false;
					tmp = tmp->suiguiente;
				}
				al_show_native_message_box(display, "Error", " ", "Ha perdido una vida", "Acepto", NULL);
				offsetx = 1000;
				return;
			}
			tmpEnem = tmpEnem->siguiente;
		}
		tmp = tmp->suiguiente;
	}
}
void gameLogic(ALLEGRO_BITMAP* bullet, ALLEGRO_BITMAP* ship,PtrNave actual, ALLEGRO_DISPLAY* display) {
	bulletHits(display);
	eliminarBalas();
	drawBullets(bullet);
	drawLifes(ship);
}

void mainGame(ALLEGRO_DISPLAY* display,ALLEGRO_BITMAP* background,ALLEGRO_EVENT_QUEUE* cola_eventos) {
	actual = (vidas->estado) ? (vidas) : ((vidas->siguiente->estado) ? (vidas->siguiente) : ((vidas->siguiente->siguiente->estado) ? (vidas->siguiente->siguiente) : (NULL)));
	if (!actual) {
		al_show_native_message_box(display, "Error", "NO tiene mas vidas", "No tiene mas vidas", "Acepto", NULL);
		return;
	}
	background = al_load_bitmap("DefenderMap.png");
	ALLEGRO_BITMAP* ship,* ship_R, * ship_L;
	ship_R= al_load_bitmap("shipRight.png");
	ship_L = al_load_bitmap("shipLeft.png");
	ALLEGRO_BITMAP* bullet= al_load_bitmap("Bala.png");
	ship = ship_R;
	if (!background||!ship||!ship_L||!bullet)
	{
		al_show_native_message_box(display, "Error", "Su maquina no tiene una de las fuentes necesarias para cargar", "Una imagen no se pudo cargar", "Acepto", NULL);
		return;
	}
	al_draw_bitmap(background, offsetx, 0, 0);
	al_flip_display();
	while (true)
	{	
		if (!actual)return;
		ALLEGRO_EVENT events;
		al_wait_for_event(cola_eventos, &events);
		if (events.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (events.keyboard.keycode)
			{
			case ALLEGRO_KEY_W:
				up = true;
				break;
			case ALLEGRO_KEY_S:
				down = true;
				break;
			case ALLEGRO_KEY_A:
				left = true;
				break;
			case ALLEGRO_KEY_D:
				right = true;
				break;
			case ALLEGRO_KEY_UP:
				up = true;
				break;
			case ALLEGRO_KEY_DOWN:
				down = true;
				break;
			case ALLEGRO_KEY_LEFT:
				left = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				right = true;
				break;
			case ALLEGRO_KEY_ESCAPE:
				return;
				break;
			default:
				break;
			}
		}
		if (events.type == ALLEGRO_EVENT_KEY_UP) {
			switch (events.keyboard.keycode)
			{
			case ALLEGRO_KEY_W:
				up = false;
				break;
			case ALLEGRO_KEY_S:
				down = false;
				break;
			case ALLEGRO_KEY_A:
				left = false;
				break;
			case ALLEGRO_KEY_D:
				right = false;
				break;
			case ALLEGRO_KEY_UP:
				up = false;
				break;
			case ALLEGRO_KEY_DOWN:
				down = false;
				break;
			case ALLEGRO_KEY_LEFT:
				left = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				right = false;
				break;
			case ALLEGRO_KEY_ESCAPE:
				return;
				break;
			default:
				break;
			}
		}
		if (up) {actual->y -= actual->velocidad; }
		if (down) { actual->y += actual->velocidad; }
		if (left) { actual->x -= actual->velocidad; offsetx -= actual->velocidad; ship = ship_L; }
		if (right) { actual->x += actual->velocidad; offsetx += actual->velocidad; ship = ship_R; }
		al_draw_bitmap(background, -1*(offsetx-600), 0, 0);
		al_draw_bitmap(ship, 550, actual->y-50, 0);
		if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN||(events.type== ALLEGRO_EVENT_KEY_DOWN&& events.keyboard.keycode==75)) {
			disparar((actual->x+((ship==ship_L)?(-1*actual->hitBoxX):(actual->hitBoxX))), actual->y, (ship == ship_L) ? (true) : (false), true);
			actual->disparos += 1;
			std::cout << "Nave x: " << actual->x << " y: " << actual->y << " Disparos: " << actual->disparos << std::endl;
			//std::cout << "Nave x: " << actual->x << " y: " << actual->y << " Disparos: " << actual->disparos << std::endl;
		}
		gameLogic(bullet, ship_L,actual,display);
		al_flip_display();

		//TODO: cerrar todas la imagenes para no tener tanta basura por ahi
	}


	al_destroy_bitmap(ship);
	al_destroy_bitmap(ship_L);
	al_destroy_bitmap(ship_R);

}

void menu(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP* background, ALLEGRO_EVENT_QUEUE* cola_eventos, ALLEGRO_TIMER* timer) {
	background = al_load_bitmap("Fondo.png");
	ALLEGRO_BITMAP* back_A, *back_B, *back_C,*back_D;
	back_A = background;
	back_B = al_load_bitmap("Fondo_Opc_1.png");
	back_C = al_load_bitmap("Fondo_Opc_2.png");
	back_D = al_load_bitmap("Fondo_Opc_3.png");
	int mousex = 0;
	int mousey = 0;
	if (!background&&!back_A&&!back_B&&!back_C&&!back_D)
	{
		al_show_native_message_box(display, "Error", "Imagen no cargada", "Uno de los recursos importantes no se encuentra", "Acepto", NULL);
		return;
	}
	al_draw_bitmap(background, 0, 0, 0);
	al_flip_display();
	while (true) {
		ALLEGRO_EVENT eventos;
		al_wait_for_event(cola_eventos, &eventos);
		if (eventos.type == ALLEGRO_EVENT_MOUSE_AXES)
		{
			mousex = eventos.mouse.x;
			mousey = eventos.mouse.y;
		}
		if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (mousex > 500 && mousey > 386 && mousex < 718 && mousey < 434) {
				mainGame(display, background, cola_eventos);
			}
			else if (mousex > 418 && mousey > 499 && mousex < 741 && mousey < 547) {
				//TODO: opcion de cargar partida aqui;
				al_show_native_message_box(display, "Error", "Opcion aun no implementada", "Esta opcion aun no se implementa", "Acepto", NULL);
			}
			else if (mousex > 516 && mousey > 607 && mousex < 713 && mousey < 655) {
				al_destroy_bitmap(back_A);
				al_destroy_bitmap(back_B);
				al_destroy_bitmap(back_C);
				al_destroy_bitmap(back_D);
				return;
			}
			
		}
		background = (mousex > 500 && mousey > 386 && mousex < 718 && mousey <434) ? (back_B) :((mousex > 481 && mousey > 499 && mousex < 741 && mousey <547) ? (back_C) : ((mousex > 516 && mousey > 607 && mousex < 713 && mousey < 655) ? (back_D): (back_A)));
		al_draw_bitmap(background, 0, 0, 0);
		al_flip_display();
	}

}
void inicializar() {
	balas = NULL;
	vidas = NULL;
	enemigos = NULL;
	left = false;
	right = false;
	up = false;
	down = false;

	PtrNave tmp;
	for (int i = 0; i < 3; i++)
	{	
		tmp = new(nave);
		tmp->codigo = i+1;
		tmp->siguiente = vidas;
		vidas = tmp;
	}
	PtrEnemigo tmpE;
	
	for (int i = 0; i < 10; i++)
	{
		tmpE = new(Enemigo);
		tmpE->siguiente = enemigos;
		enemigos = tmpE;

	}
}

int main()
{	

	inicializar();


	ALLEGRO_DISPLAY* display;
	ALLEGRO_BITMAP* background;
	ALLEGRO_EVENT_QUEUE* cola_eventos;
	background = NULL;
	if (!al_init()) {
		al_show_native_message_box(NULL, "Ventana Emergente", "Error", "No se puede inicializar Allegro", NULL, NULL);
		return -1;
	}
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();


	ALLEGRO_MONITOR_INFO monitor;
	al_get_monitor_info(0, &monitor);
	const int RX = monitor.x2 - monitor.x1;
	const int RY = monitor.y2 - monitor.y1;


	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	display = al_create_display(1200, 800);
	if (!display) {
		al_show_native_message_box(display, "test", "Si vez esto, algo muy malo ocurrio", "Display window", "Acepto", NULL);
	}
	/*
	background = al_load_bitmap("Fondo.png");
	if (!background)
	{
		al_show_native_message_box(display, "Error", "Imagen no cargada", "La imagen no se pudo cargar", "Acepto", NULL);
		return -1;
	}
	*/
	ALLEGRO_FONT* Fuente = al_load_font("SuperMario256.ttf", 40, NULL);
	if (!Fuente) {
		al_show_native_message_box(display, "Error", "Fuente no cargada", "La fuente no se pudo cargar", "Acepto", NULL);
		return -1;
	}
	
	cola_eventos = al_create_event_queue();
	al_register_event_source(cola_eventos, al_get_timer_event_source(timer));
	al_register_event_source(cola_eventos, al_get_display_event_source(display));
	al_register_event_source(cola_eventos, al_get_keyboard_event_source());
	al_register_event_source(cola_eventos, al_get_mouse_event_source());

	al_start_timer(timer);


	//al_draw_bitmap(background, -480, 0, 0);
	// al_draw_text(Fuente, al_map_rgb(44, 117, 255), x, y, ALLEGRO_ALIGN_CENTRE, "Practica Allegro");
	menu(display, background, cola_eventos,timer);
	//mainGame(display,background,cola_eventos);

	al_destroy_display(display);
	al_destroy_event_queue(cola_eventos);
	al_destroy_font(Fuente);
	al_destroy_timer(timer);
	al_destroy_bitmap(background);
	return 0;
}