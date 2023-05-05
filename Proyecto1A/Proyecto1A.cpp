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
	int velocidad = 5;
	bool direccion = false;
	bool estado = true;
	bala* suiguiente = NULL;
	bala* anterior = NULL;
}*PtrBala;

typedef struct nave {
	int codigo;
	int x = 1200/2;
	int y = 800/2;
	int velocidad = 10;
	bool estado = true;
	int hurtBoxX = 25;
	int hurtBoxY = 15;
	nave* siguiente = NULL;
}*PtrNave;

typedef struct Enemigo {
	int codigoimagen = -1;
	int x = -1;
	int y = -1;
	bool estado = true;
	int velocidad = 5;
	Enemigo* siguiente = NULL;
}*PtrEnemigo;

const float FPS = 30.0;	//fps 
PtrBala balas;
PtrNave vidas;
PtrEnemigo enemigos;
bool left, right, up, down;

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
		
		al_draw_bitmap(bullet, balAct->x, balAct->y, 0);
		balAct->x += (balAct->direccion) ? (-1*(balAct->velocidad)) : (balAct->velocidad);
		balAct = balAct->suiguiente;
	}
}
void disparar(int x,int y,bool iz) {
	PtrBala tmp = new(bala);
	tmp->x = x;
	tmp->y = y;
	tmp->direccion = iz;
	tmp->suiguiente = balas;
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

	//TODO: acabaar esat funcion que elimina
}
void eliminarBalas() {
	PtrBala tmp = balas;
	while (tmp) {
		if (tmp->x > 2000 || tmp->x < 0) tmp->estado = false;
		tmp = tmp->suiguiente;
	}
	eleminarBalaInact();
}

void mainGame(ALLEGRO_DISPLAY* display,ALLEGRO_BITMAP* background,ALLEGRO_EVENT_QUEUE* cola_eventos) {
	PtrNave actual = (vidas->estado) ? (vidas) : ((vidas->siguiente->estado) ? (vidas->siguiente) : ((vidas->siguiente->siguiente->estado) ? (vidas->siguiente->siguiente) : (NULL)));
	if (!actual) {
		al_show_native_message_box(display, "Error", "NO tiene mas vidas", "No tiene mas vidas", "Acepto", NULL);
		return;
	}
	int absx = 1200 / 2;
	int absy = 800 / 2;
	int offsetx = -480; int offsety = 0;
	background = al_load_bitmap("DefenderMap.png");
	ALLEGRO_BITMAP* ship,* ship_R, * ship_L;
	ship_R= al_load_bitmap("shipRight.png");
	ship_L = al_load_bitmap("shipLeft.png");
	ALLEGRO_BITMAP* bullet= al_load_bitmap("Bala.png");
	ship = ship_R;
	if (!background||!ship||!ship_L||!bullet)
	{
		al_show_native_message_box(display, "Error", "Su maquina no tiene una de las fuentes necesarias para cargar", "La imagen no se pudo cargar", "Acepto", NULL);
		return;
	}
	al_draw_bitmap(background, offsetx, 0, 0);
	al_flip_display();
	while (true)
	{	

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
		if (left) { actual->x += actual->velocidad; offsetx += actual->velocidad; ship = ship_L; }
		if (right) { actual->x -= actual->velocidad; offsetx -= actual->velocidad; ship = ship_R; }
		al_draw_bitmap(background, offsetx, 0, 0);
		al_draw_bitmap(ship, 600, actual->y, 0);
		if (events.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			disparar(actual->x, actual->y, (ship == ship_L) ? (true) : (false));
		}
		eliminarBalas();
		drawBullets(bullet);
		drawLifes(ship_L);
		al_flip_display();

		//TODO: cerrar todas la imagenes para no tener basura por ahi
	}
}

void menu(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP* background, ALLEGRO_EVENT_QUEUE* cola_eventos, ALLEGRO_TIMER* timer) {
	background = al_load_bitmap("Fondo.png");
	ALLEGRO_BITMAP* back_A, *back_B, *back_C;
	back_A = background;
	back_B = al_load_bitmap("FondoA.png");
	back_C = al_load_bitmap("FondoB.png");
	int mousex = 0;
	int mousey = 0;
	if (!background)
	{
		al_show_native_message_box(display, "Error", "Imagen no cargada", "La imagen no se pudo cargar", "Acepto", NULL);
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
			if (mousex > 480 && mousey > 344 && mousex < 700 && mousey < 394) {
				mainGame(display, background, cola_eventos);
			}
			else if (mousex > 495 && mousey > 449 && mousex < 691 && mousey < 498) {
				al_destroy_bitmap(back_A);
				al_destroy_bitmap(back_B);
				al_destroy_bitmap(back_C);
				return;
			}
			
		}
		background = (mousex > 480 && mousey > 344 && mousex < 700 && mousey <394) ? (back_B) :((mousex > 495 && mousey > 449 && mousex < 691 && mousey <498) ? (back_C) : (back_A));
		//mainGame(cola_eventos, display, background);
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
		tmp->siguiente = vidas;
		vidas = tmp;
	}
	PtrEnemigo tmpE;
	for (int i = 0; i < 20; i++)
	{
		tmpE = new(Enemigo);
		tmpE->siguiente = enemigos;
		enemigos = tmpE;
	}
	PtrBala tmpB;
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