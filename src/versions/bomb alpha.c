#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>

const float FPS = 60;
const int SCREEN_W = 800;
const int SCREEN_H = 800;
const int player_size = 100;
const int enemy_size = 100;
const int tnt_size = 40;

typedef struct jogador{
	int x;
	int y;
	int dx;
	int dy;
	int direction;
} jogador;

typedef struct inimigo{
	int x[3];
	int y[3];
	int dx[3];
	int dy[3];
	int timer;
	int direction;
} inimigo;

typedef struct bomba{
	int x[3];
	int y[3];
	int timer[3];
	int available[3];
} bomba;

int main(){
	srand(time(NULL));
	
	int i, j;
	jogador player;
	inimigo enemy;
	bomba tnt;
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *alplayer = NULL;
	ALLEGRO_BITMAP *alenemy[3];
	ALLEGRO_BITMAP *altnt[3];
	
	//propriedades iniciais do player
	player.x = 0;
	player.y = 0;
	player.dx = 100;
	player.dy = 100;
	
	//propriedades iniciais dos enemies
	for(i=0;i<2;i++){
		enemy.x[i] = 100*(rand()%6);
		enemy.y[i] = 100*(rand()%6);
	}
	
	enemy.x[2] = 700;
	enemy.y[2] = 700;

	for(i=0;i<3;i++){
		enemy.dx[i] = 100;
		enemy.dy[i] = 100;
	}

	enemy.timer = 0;
	enemy.direction = 0;

	//propriedades iniciais das bombas
	for(i=0;i<3;i++){
		tnt.x[i] = 1000;
		tnt.y[i] = 1000;
	}

	for(i=0;i<3;i++){
		tnt.available[i] = 1;
	}

	int count_tnt = 0;

	
	//inicio de rotina
	al_init();
	timer = al_create_timer(1/FPS); //temporizador
	display = al_create_display(SCREEN_W, SCREEN_H); //define o tamando da tela
	
	//atribui uma imagem ao plano de fundo
	al_init_image_addon();
	ALLEGRO_BITMAP *bg = al_load_bitmap("bg_2.png");
	al_draw_bitmap(bg, 0, 0, 0);
	al_flip_display();
	
	//atribui uma imagem ao player
	alplayer = al_load_bitmap("player1_front.png");
	
	//atribui uma imagem aos inimigos
	for(i=0;i<2;i++){
		alenemy[i] = al_load_bitmap("enemy1_back.png");
	}
	alenemy[2] = al_load_bitmap("enemy2_back.png");
	
	//atribui uma imagem a bomba
	for(i=0;i<3;i++){
		altnt[i] = al_load_bitmap("tnt.png");
	}
	
	//inicia o teclado
	al_install_keyboard();
	
	//fila de eventos
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	
	al_flip_display();
	al_start_timer(timer);
	
	do{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
			switch(ev.keyboard.keycode){

				//TECLA SPACE
				case ALLEGRO_KEY_SPACE:
				if(tnt.available[0]){
					tnt.timer[0] = 0;
					tnt.x[0] = player.x;
					tnt.y[0] = player.y;
					tnt.available[0] = 0;
				}
				else if(tnt.available[1]){
					tnt.timer[1] = 0;
					tnt.x[1] = player.x;
					tnt.y[1] = player.y;
					tnt.available[1] = 0;
				}
				else if(tnt.available[2]){
					tnt.timer[2] = 0;
					tnt.x[2] = player.x;
					tnt.y[2] = player.y;
					tnt.available[2] = 0;
				}
				break;
				
				//TECLA ESC
				case ALLEGRO_KEY_ESCAPE:
				return 0;
				
			}
		}	

		if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
			switch(ev.keyboard.keycode){
				
				//SETA PRA CIMA
				case ALLEGRO_KEY_UP:
				al_destroy_bitmap(alplayer);
				alplayer = al_load_bitmap("player1_back.png");
				if(player.y > 0)
				player.y -= player.dy;
				break;
				
				//SETA PRA BAIXO
				case ALLEGRO_KEY_DOWN:
				al_destroy_bitmap(alplayer);
				alplayer = al_load_bitmap("player1_front.png");
				if(player.y < SCREEN_H - player_size)
				player.y += player.dy;
				break;
				
				//SETA PRA ESQUERDA
				case ALLEGRO_KEY_LEFT:
				al_destroy_bitmap(alplayer);
				alplayer = al_load_bitmap("player1_left.png");
				if(player.x > 0)
				player.x -= player.dx;
				break;
				
				//SETA PRA DIREITA
				case ALLEGRO_KEY_RIGHT:
				al_destroy_bitmap(alplayer);
				alplayer = al_load_bitmap("player1_right.png");
				if(player.x < SCREEN_W - player_size)
				player.x += player.dx;
				break;
				
			}
		}
		
		if(ev.type == ALLEGRO_EVENT_TIMER){
			
			//conta tres segundos para que as bombas explodam
			for(i=0;i<3;i++){
				if(tnt.timer[i] < 180) tnt.timer[i]++;
				else{ 
					tnt.x[i] = 2000;
					tnt.y[i] = 2000;
					tnt.available[i] = 1;
				}	
			}
			
			
			//redesenha as imagens dos elementos a cada alteração do timer
			al_draw_bitmap(bg, 0, 0, 0);
			al_draw_bitmap(alplayer, player.x, player.y, 0);
			for(i=0;i<3;i++){
				al_draw_bitmap(altnt[i], tnt.x[i], tnt.y[i], 0);
			}
				
			
			//Movimento do inimigo 2
			if(enemy.timer < 30) enemy.timer++;
			else enemy.timer = 0;
			
			if(enemy.timer == 29){
				switch(enemy.direction){
					
					case 0: //UP
					al_destroy_bitmap(alenemy[1]);
					alenemy[1] = al_load_bitmap("enemy1_back.png");
					if(enemy.y[1] > 0)
						enemy.y[1] -= enemy.dy[1];
					else{
						al_destroy_bitmap(alenemy[1]);
						alenemy[1] = al_load_bitmap("enemy1_front.png");
						enemy.direction = 1;
					}
					break;

					case 1: //DOWN
					al_destroy_bitmap(alenemy[1]);
					alenemy[1] = al_load_bitmap("enemy1_front.png");
					if(enemy.y[1] < SCREEN_H - enemy_size)
						enemy.y[1] += enemy.dy[1];
					else{
						al_destroy_bitmap(alenemy[1]);
						alenemy[1] = al_load_bitmap("enemy1_back.png");
						enemy.direction = 0;
					}
					break;

					/*case 2: //LEFT
					al_destroy_bitmap(alenemy[2]);
					alenemy[2] = al_load_bitmap("enemy1_left.png");
					if(enemy.x[2] > 0)
						enemy.x[2] -= enemy.dx[2];
					else
						enemy.direction = rand()%3;
					break;

					case 3: //RIGHT
					al_destroy_bitmap(alenemy[2]);
					alenemy[2] = al_load_bitmap("enemy1_right.png");
					if(enemy.x[2] < SCREEN_W - enemy_size)
						enemy.x[2] += enemy.dx[2];
					else
						enemy.direction = rand()%3;
					break;*/
				

				}
			}

			//Movimento do inimigo 3
			if(enemy.timer == 29){

					if(player.x - enemy.x[2] > 0){ //VAI PRA DIREITA
						al_destroy_bitmap(alenemy[2]);
						alenemy[2] = al_load_bitmap("enemy2_right.png");
						enemy.x[2] += enemy.dx[2];
					}
					if(player.x - enemy.x[2] < 0){ //VAI PRA ESQUERDA
						al_destroy_bitmap(alenemy[2]);
						alenemy[2] = al_load_bitmap("enemy2_left.png");
						enemy.x[2] -= enemy.dx[2];
					}
				
					if(player.x == enemy.x[2]){
						if(player.y - enemy.y[2] > 0){ //VAI PRA BAIXO
							al_destroy_bitmap(alenemy[2]);
							alenemy[2] = al_load_bitmap("enemy2_front.png");
							enemy.y[2] += enemy.dy[2];
						}
						if(player.y - enemy.y[2] < 0){ //VAI PRA CIMA
							al_destroy_bitmap(alenemy[2]);
							alenemy[2] = al_load_bitmap("enemy2_back.png");
							enemy.y[2] -= enemy.dy[2];
						}
					}

			}
			
			for(i=0;i<3;i++){ 
				al_draw_bitmap(alenemy[i], enemy.x[i], enemy.y[i], 0);
			}
			
		}
		
			
		//dano aos inimigos e ao player
		for(i=0;i<3;i++){
			for(j=0;j<3;j++){
				if(tnt.timer[i] == 179){
					if(fabs((tnt.x[i] + tnt_size)/2 - (enemy.x[j] + enemy_size)/2) < 50 && fabs((tnt.y[i] + tnt_size)/2 - (enemy.y[j] + enemy_size)/2) < 50){
						enemy.x[j] = 1200;
						enemy.y[j] = 1200;
						enemy.dx[j] = 0;
						enemy.dy[j] = 0;
					}
					if(fabs((tnt.x[i] + tnt_size)/2 - (player.x + player_size)/2) < 50 && fabs((tnt.y[i] + tnt_size)/2 - (player.y + player_size)/2) < 50){
						player.x = 1200;
						player.y = 1200;
					}
				}
			}
		}
		
		al_flip_display();
		
	}while(1==1);
	
	al_destroy_bitmap(alplayer);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
	return 0;
}