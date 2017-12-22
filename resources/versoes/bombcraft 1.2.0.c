#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//CONSTANTES
const float FPS = 60;
const int SCREEN_W = 800;
const int SCREEN_H = 600;
const int player_size = 50;
const int enemy_size = 50;
const int tnt_size = 50;

//ESTRUTURAS
typedef struct entity{
	int x[10]; //posiçao no eixo x
	int y[10]; //posiçao no eixo y
	int dx[10]; //movimentaçao no eixo x
	int dy[10]; //movimentaçao no eixo y
	int direction[10]; //direçao que está olhando
	int timer[10]; //temporizador
	int available[10]; //disponibilidade
	int vulnerable[10]; //vulnerabilidade
} entity;

//FUNCOES E PROCEDIMENTOS
//...

int main(){
	
	srand(time(NULL));
	
//VARIAVEIS------------------------------------------------------------------------------------------------------------------------
	FILE *arq, *arq2;
	int i, j;
	int checkplayerkill = 3; //verifica se o player está vivo ou morto
	int checkenemykill = 0; //verifica a quantidade de inimigos que está morto
	int playing = 1;
	int menu = 1;
	int normal = 0;
	int hardcore = 0;
	char my_text[100];
	char my_text2[100];
	float time_alive;
	float high_score;
	entity player;
	entity enemy;
	entity tnt;
	


//VARIAVEIS DO ALLEGRO------------------------------------------------------------------------------------------------------------------------
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_TIMER *timerScore = NULL;
	ALLEGRO_TIMER *timerHardcore = NULL;
	ALLEGRO_BITMAP *bgmenu = NULL;
	ALLEGRO_BITMAP *bg = NULL;
	ALLEGRO_BITMAP *lifeBar = NULL;
	ALLEGRO_BITMAP *lifeBarInv = NULL;
	ALLEGRO_BITMAP *tntBar = NULL;
	ALLEGRO_BITMAP *alplayer = NULL;
	ALLEGRO_BITMAP *alenemy[3];
	ALLEGRO_BITMAP *altnt[3];
	ALLEGRO_FONT *size_32 = NULL;
	ALLEGRO_SAMPLE *mainSong = NULL;
	ALLEGRO_SAMPLE *igniteSound = NULL;
	ALLEGRO_SAMPLE *hurtSound = NULL;
	ALLEGRO_SAMPLE *explosionSound = NULL;
	ALLEGRO_SAMPLE_INSTANCE *main = NULL;
	ALLEGRO_SAMPLE_INSTANCE *ignite = NULL;
	ALLEGRO_SAMPLE_INSTANCE *hurt = NULL;
	ALLEGRO_SAMPLE_INSTANCE *explosion = NULL;
	ALLEGRO_SAMPLE_INSTANCE *step = NULL;
	


//INICIO DE ROTINA------------------------------------------------------------------------------------------------------------------------
	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(10);

	timer = al_create_timer(1/FPS);
	timerScore = al_create_timer(1/FPS);
	timerHardcore = al_create_timer(1/FPS);
	display = al_create_display(SCREEN_W, SCREEN_H);
	size_32 = al_load_font("resources/fonts/MINECRAFT.ttf", 32, 1);
	


//BACKGROUND E HUD------------------------------------------------------------------------------------------------------------------------
	bgmenu = al_load_bitmap("resources/images/menu1.png");
	al_draw_bitmap(bgmenu, 0, 0, 0);
	bg = al_load_bitmap("resources/images/bg_main.png");
	al_draw_bitmap(bg, 0, 0, 0);
	
	lifeBar = al_load_bitmap("resources/images/Life_Full.png");
	al_draw_bitmap(lifeBar, 600, 0, 0);
	lifeBarInv = al_load_bitmap("resources/images/Life_Inv.png");
	al_draw_bitmap(lifeBarInv, 600, 0, 0);
	tntBar = al_load_bitmap("resources/images/tnt_bar3.png");
	al_draw_bitmap(tntBar, 500, 0, 0);

	
	
//IMAGENS DAS ENTIDADES------------------------------------------------------------------------------------------------------------------------
	
	//Players
	alplayer = al_load_bitmap("resources/images/player1_front.png");
	

	//Enemies
	for(i=0;i<2;i++){
		alenemy[i] = al_load_bitmap("resources/images/enemy1_back.png");
	}
	alenemy[2] = al_load_bitmap("resources/images/enemy2_back.png");
	

	//TNT
	for(i=0;i<3;i++){
		altnt[i] = al_load_bitmap("resources/images/tnt3.png");
	}

	

//MUSICAS E SONS------------------------------------------------------------------------------------------------------------------------
	mainSong = al_load_sample("resources/sounds/main.ogg");
	main = al_create_sample_instance(mainSong);
	al_set_sample_instance_playmode(main, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(main, al_get_default_mixer());

	igniteSound = al_load_sample("resources/sounds/ignite.ogg");
	ignite = al_create_sample_instance(igniteSound);
	al_set_sample_instance_playmode(ignite, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(ignite, al_get_default_mixer());

	hurtSound = al_load_sample("resources/sounds/hurt.ogg");
	hurt = al_create_sample_instance(hurtSound);
	al_set_sample_instance_playmode(hurt, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(hurt, al_get_default_mixer());

	explosionSound = al_load_sample("resources/sounds/explode.ogg");
	explosion = al_create_sample_instance(explosionSound);
	al_set_sample_instance_playmode(explosion, ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(explosion, al_get_default_mixer());



//FILA DE EVENTOS------------------------------------------------------------------------------------------------------------------------
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	
	al_flip_display();
	al_start_timer(timer); //inicia a contagem do timer



//INICIO DO JOGO------------------------------------------------------------------------------------------------------------------------
	while(playing == 1){

		ALLEGRO_EVENT ev;



		//MENU
		while(menu > 0){
			al_wait_for_event(event_queue, &ev);
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
				switch(ev.keyboard.keycode){
					
					case ALLEGRO_KEY_UP:
						if(menu == 1){
							al_destroy_bitmap(bgmenu);
							bgmenu = al_load_bitmap("resources/images/menu3.png");
							menu = 3;
						}
						else if(menu == 2){
							al_destroy_bitmap(bgmenu);
							bgmenu = al_load_bitmap("resources/images/menu1.png");
							menu = 1;
						}
						else if(menu == 3){
							al_destroy_bitmap(bgmenu);
							bgmenu = al_load_bitmap("resources/images/menu2.png");
							menu = 2;
						}
						break;
					

					case ALLEGRO_KEY_DOWN:
						if(menu == 1){
							al_destroy_bitmap(bgmenu);
							bgmenu = al_load_bitmap("resources/images/menu2.png");
							menu = 2;
						}
						else if(menu == 2){
							al_destroy_bitmap(bgmenu);
							bgmenu = al_load_bitmap("resources/images/menu3.png");
							menu = 3;
						}
						else if(menu == 3){
							al_destroy_bitmap(bgmenu);
							bgmenu = al_load_bitmap("resources/images/menu1.png");
							menu = 1;
						}
						break;


					case ALLEGRO_KEY_ENTER:
						if(menu == 1){
							menu = 0;
							normal = 1;
							al_set_timer_count(timerScore, 0);
							al_start_timer(timerScore);
						}
						
						if(menu == 2){
							menu = 0;
							hardcore = 1;
							al_set_timer_count(timerHardcore, 0);
							al_start_timer(timerHardcore);
						}

						//if(menu == 3)	
						break;


					case ALLEGRO_KEY_ESCAPE:
						return 0;
				}
			}

			if(ev.type == ALLEGRO_EVENT_TIMER){
				al_draw_bitmap(bgmenu, 0, 0, 0);
				al_flip_display();
				
			}
		}

		al_destroy_bitmap(bg);
		bg = al_load_bitmap("resources/images/bg_main.png");
		lifeBarInv = al_load_bitmap("resources/images/Life_Inv.png");
		checkenemykill = 0;
		checkplayerkill = 3;

		//PROPRIEDADES INICIAS DO PLAYER------------------------------------------------------------------------------------------------------------------------
		player.x[1] = 0;
		player.y[1] = 100;
		player.dx[1] = 50;
		player.dy[1] = 50;
		player.direction[1] = 1;
		player.timer[1] = 0;
		player.vulnerable[1] = 0;
		


		//PROPRIEDADE INICIAS DOS INIMIGOS------------------------------------------------------------------------------------------------------------------------
		for(i=0;i<2;i++){
			enemy.x[i] = 100*(rand()%6); //spawna o inimigo numa posiaçao aleatoria do mapa
			enemy.y[i] = 100*(rand()%4)+200;
		}
		enemy.x[2] = 750;
		enemy.y[2] = 550;
		enemy.direction[0] = 0;
		enemy.direction[1] = 2;
		enemy.direction[2] = 0;
		
		for(i=0;i<3;i++){
			enemy.dx[i] = 50;
			enemy.dy[i] = 50;
			enemy.timer[i] = 0;
		}



		//PROPRIEDADES INICIAS DAS BOMBAS------------------------------------------------------------------------------------------------------------------------
		for(i=0;i<3;i++){
			tnt.x[i] = 2000; //spawna as bombas em posiçoes fora da area de visao do jogador
			tnt.y[i] = 2000;
		}

		for(i=0;i<3;i++){
			tnt.available[i] = 1;
		}



		//MODO NORMAL
		while(normal == 1){
			al_wait_for_event(event_queue, &ev);
			//EVENTO DO TIPO PRESSIONAMENTO DE UMA TECLA
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
				switch(ev.keyboard.keycode){
					
					//SETA PRA CIMA
					case ALLEGRO_KEY_UP: //movimenta o inimigo caso ele ja esteja olhando nessa direçao
						if(player.y[1] > 100 && player.direction[1] == 0){ //evita que o player saia pra fora do mapa
							player.y[1] -= player.dy[1]; //movimenta o player
						}
						else{
							player.direction[1] = 0; //apenas muda a direçao em que o player esta olhando
							al_destroy_bitmap(alplayer); //remove a imagem do player
							alplayer = al_load_bitmap("resources/images/player1_back.png"); //atribui uma nova imagem relacioanado a direçao que o player esta olhando
						}
						break;
					

					//SETA PRA BAIXO
					case ALLEGRO_KEY_DOWN:
						if(player.y[1] < SCREEN_H - player_size && player.direction[1] == 1){
							player.y[1] += player.dy[1];
						}
						else{
							player.direction[1] = 1;
							al_destroy_bitmap(alplayer);
							alplayer = al_load_bitmap("resources/images/player1_front.png");
						}
						break;
					

					//SETA PRA ESQUERDA
					case ALLEGRO_KEY_LEFT:
						if(player.x[1] > 0 && player.direction[1] == 2){
							player.x[1] -= player.dx[1];
						}
						else{
							player.direction[1] = 2;
							al_destroy_bitmap(alplayer);
							alplayer = al_load_bitmap("resources/images/player1_left.png");
						}
						break;
					

					//SETA PRA DIREITA
					case ALLEGRO_KEY_RIGHT:
						if(player.x[1] < SCREEN_W - player_size && player.direction[1] == 3){
							player.x[1] += player.dx[1];
						}
						else{
							player.direction[1] = 3;
							al_destroy_bitmap(alplayer);
							alplayer = al_load_bitmap("resources/images/player1_right.png");
						}
						break;


					//TECLA SPACE
					case ALLEGRO_KEY_SPACE:
						for(i=0;i<3;i++){
							if(tnt.available[i]){ //verifica se existem bombas disponiveis no estoque, se houver:

								al_destroy_bitmap(altnt[i]); //remove a imagem atribuida a bomba
								altnt[i] = al_load_bitmap("resources/images/tnt3.png"); //atribui a imagem da bomba com o relogio em 3 segundos

								tnt.timer[i] = 0; //zera o contador da bomba para o inicio de uma nova contagem
								if(player.direction[1] == 0 && player.y[1] > 100){ //verifica a direçao que o player esta olhando e se o quadrante imediatamente a frente esta dentro da area de visao do jogador
									tnt.x[i] = player.x[1]; //coloca a bomba na posiçao desejada
									tnt.y[i] = player.y[1] - 50;
									tnt.available[i] = 0; //indica que essa bomba nao esta mais disponivel no inventario
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								if(player.direction[1] == 1 && player.y[1] < SCREEN_H - player_size){
									tnt.x[i] = player.x[1];
									tnt.y[i] = player.y[1] + 50;
									tnt.available[i] = 0;
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								if(player.direction[1] == 2 && player.x[1] > 0){
									tnt.x[i] = player.x[1] - 50;
									tnt.y[i] = player.y[1];
									tnt.available[i] = 0;
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								if(player.direction[1] == 3 && player.x[1] < SCREEN_W - player_size){
									tnt.x[i] = player.x[1] + 50;
									tnt.y[i] = player.y[1];
									tnt.available[i] = 0;
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								break;
							}
						}
						break;
					

					//TECLA ESC
					case ALLEGRO_KEY_ESCAPE:
						return 0; //caso o jogador aperte a tecla esc o jogo eh fechado
					
				}
			}
			


			//EVENTO DO TIPO PASSAGEM DE TEMPO
			if(ev.type == ALLEGRO_EVENT_TIMER){

				al_play_sample_instance(main);



				//BARRA DE VIDA------------------------------------------------------------------------------------------------------------------------
				if(player.timer[1] < 180)
					player.timer[1]++;

				if(player.timer[1] == 180){
					al_destroy_bitmap(lifeBarInv);
					lifeBarInv = al_load_bitmap("resources/images/Life_Inv0.png");
					player.vulnerable[1] = 1;
				}

				switch(checkplayerkill){
					case 3:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_Full.png");
					break;

					case 2:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_2.png");
					break;

					case 1:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_1.png");
					break;

					case 0:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_Empty.png");
					break;
				}



				//BARRA DE BOMBAS DISPONIVEIS------------------------------------------------------------------------------------------------------------------------
				if(tnt.available[0] && tnt.available[1] && tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar3.png");
				}
				else if(!tnt.available[0] && !tnt.available[1] && !tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar0.png");
				}
				else if(tnt.available[0] && tnt.available[1]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar2.png");
				}
				else if(tnt.available[0] && tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar2.png");
				}
				else if(tnt.available[1] && tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar2.png");
				}
				else{
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar1.png");
				}
					

				//TNT------------------------------------------------------------------------------------------------------------------------

				//CONTAGEM PARA EXPLOSAO DAS BOMBAS
				for(i=0;i<3;i++){
					if(tnt.timer[i] < 180) tnt.timer[i]++; //se o timer for menor que 180 (3 segundos), acrescenta +1 ao contador
					else{ //senao:
						tnt.timer[i] = 0; //zera o contador da bomba
						tnt.x[i] = 2000; //teleporta a bomba para uma posiçao nao visivel do mapa
						tnt.y[i] = 2000;
						tnt.available[i] = 1; //define que essa bomba esta novamente disponivel no inventario
					}
					if(tnt.timer[i] == 0){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt3.png"); //altera para a imagem do relogio em 3 segundos
					}
					if(tnt.timer[i] == 60){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt2.png"); //altera para a imagem do relogio em 3 segundos
					}
					if(tnt.timer[i] == 120){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt1.png"); //altera para a imagem do relogio em 3 segundos
					}
					if(tnt.timer[i] == 175 && tnt.x[i] != 2000){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt_explosion.png"); //altera para a imagem de uma explosao
						al_stop_sample_instance(explosion);
						al_play_sample_instance(explosion);
					}
				}



				//COLISAO------------------------------------------------------------------------------------------------------------------------
				
				//DANO POR COLISAO
				for(i=0;i<3;i++){
					if(player.x[1] == enemy.x[i] && player.y[1] == enemy.y[i]){
						if(player.vulnerable[1] == 1){
							al_play_sample_instance(hurt);
							checkplayerkill--;
							player.vulnerable[1] = 0;
							player.timer[1] = 0;
							al_destroy_bitmap(lifeBarInv);
							lifeBarInv = al_load_bitmap("resources/images/Life_Inv.png");
						}
					}
				}


				//DANO POR BOMBA
				for(i=0;i<3;i++){
					for(j=0;j<3;j++){
						if(tnt.timer[i] == 179){
							if((fabs(tnt.x[i] - enemy.x[j]) == 50 && fabs(tnt.y[i] - enemy.y[j]) == 0) || fabs(tnt.y[i] - enemy.y[j]) == 50 && fabs(tnt.x[i] - enemy.x[j]) == 0){
								enemy.x[j] = 1200;
								enemy.y[j] = 1200;
								enemy.dx[j] = 0;
								enemy.dy[j] = 0;
								checkenemykill++;
							}
							if((fabs(tnt.x[i] - player.x[1]) == 50 && fabs(tnt.y[i] - player.y[1]) == 0) || fabs(tnt.y[i] - player.y[1]) == 50 && fabs(tnt.x[i] - player.x[1]) == 0){
								if(player.vulnerable[1] == 1){
									al_play_sample_instance(hurt);
									checkplayerkill--;
									player.vulnerable[1] = 0;
									player.timer[1] = 0;
									al_destroy_bitmap(lifeBarInv);
									lifeBarInv = al_load_bitmap("resources/images/Life_Inv.png");
								}
							}
						}
					}
				}


				//NAO PERMITE QUE O PLAYER ANDE SE HOUVER UMA BOMBA NO CAMINHO (A BOMBA OCUPA ESPAÇO)
				for(i=0;i<3;i++){
					if(player.direction[1] == 0 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.y[1] = tnt.y[i]+50;
					if(player.direction[1] == 1 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.y[1] = tnt.y[i]-50;
					if(player.direction[1] == 2 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.x[1] = tnt.x[i]+50;
					if(player.direction[1] == 3 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.x[1] = tnt.x[i]-50;
				}
				

				//RECUO DOS INIMIGOS CASO HAJA UMA BOMBA NO CAMINHO
				for(i=0;i<2;i++){
					for(j=0;j<3;j++){ //sao usadas as variaveis i e j para testar a possibilidade de colisao entre todos os inimigos e todas as bombas
						if(enemy.direction[i] == 0 && enemy.x[i] == tnt.x[j] && enemy.y[i] == tnt.y[j]+50){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_front.png");
							enemy.direction[i] = 1;
						}
						else if(enemy.direction[i] == 1 && enemy.x[i] == tnt.x[j] && enemy.y[i] == tnt.y[j]-50){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_back.png");
							enemy.direction[i] = 0;
						}
						else if(enemy.direction[i] == 2 && enemy.x[i] == tnt.x[j]+50 && enemy.y[i] == tnt.y[j]){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
							enemy.direction[i] = 3;
						}
						else if(enemy.direction[i] == 3 && enemy.x[i] == tnt.x[j]-50 && enemy.y[i] == tnt.y[j]){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
							enemy.direction[i] = 2;
						}
					}
				}
				


				//MOVIMENTAÇAO------------------------------------------------------------------------------------------------------------------------	
				
				//MOVIMENTO DO INIMIGO PADRAO
				if(enemy.timer[0] < 10) enemy.timer[0]++; //contador para cada movimentaçao do inimigo
				else enemy.timer[0] = 0;
				
				for(i=0;i<2;i++){
					if(enemy.timer[0] == 9){
						switch(enemy.direction[i]){ //verifica a direçao que o inimigo esta olhando
									
						case 0: //UP
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_back.png");
						if(enemy.y[i] > 100) //caso o inimigo esteja dentro do limite do mapa, ocorre movimentaçao
							enemy.y[i] -= enemy.dy[i];
						else{ //caso contrario, a direçao do inimigo eh trocada
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_front.png");
							enemy.direction[i] = 1;
						}
						break;

						case 1: //DOWN
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_front.png");
						if(enemy.y[i] < SCREEN_H - enemy_size)
							enemy.y[i] += enemy.dy[i];
						else{
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_back.png");
							enemy.direction[i] = 0;
						}
						break;

						case 2: //LEFT
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_left.png");
						if(enemy.x[i] > 0)
							enemy.x[i] -= enemy.dx[i];
						else{
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
							enemy.direction[i] = 3;
						}
						break;

						case 3: //RIGHT
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
						if(enemy.x[i] < SCREEN_W - enemy_size)
							enemy.x[i] += enemy.dx[i];
						else{
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_left.png");
							enemy.direction[i] = 2;
						}
						break;

						}
					}
				}

				
				//MOVIMENTO DO INIMIGO ESPECIAL
				if(enemy.timer[2] < 30) enemy.timer[2]++; //contador para cada movimentaçao do inimigo
				else enemy.timer[2] = 0;
				
				if(enemy.timer[2] == 29){

					//DIREITA
					if(player.x[1] - enemy.x[2] > 0){ //verifica se o player está a direita do inimigo no eixo x
						if(enemy.direction[2] != 4){ //caso a posiçao seja diferente eh verificado a direçao que o inimigo esta olhando
							enemy.direction[2] = 4; //faz com que o inimigo olhe pra direita
							al_destroy_bitmap(alenemy[2]);
							alenemy[2] = al_load_bitmap("resources/images/enemy2_right.png");
						}
						else
							enemy.x[2] += enemy.dx[2]; //caso a direçao do inimigo ja esteja certa ele eh movimentado
					}

					//ESQUERDA	
					if(player.x[1] - enemy.x[2] < 0){
						if(enemy.direction[2] != 3){
							enemy.direction[2] = 3;
							al_destroy_bitmap(alenemy[2]);
							alenemy[2] = al_load_bitmap("resources/images/enemy2_left.png");
						}
						else
							enemy.x[2] -= enemy.dx[2];
					}
					
					//PRA BAIXO
					else if(player.x[1] == enemy.x[2]){
						if(player.y[1] - enemy.y[2] > 0){
							if(enemy.direction[2] != 1){
								enemy.direction[2] = 1;
								al_destroy_bitmap(alenemy[2]);
								alenemy[2] = al_load_bitmap("resources/images/enemy2_front.png");
							}
							else
								enemy.y[2] += enemy.dy[2];
						}

						//PRA CIMA	
						if(player.y[1] - enemy.y[2] < 0){
							if(enemy.direction[2] != 0){
								enemy.direction[2] = 0;
								al_destroy_bitmap(alenemy[2]);
								alenemy[2] = al_load_bitmap("resources/images/enemy2_back.png");
							}
							else
								enemy.y[2] -= enemy.dy[2];
						}
					}
				}

				
		
				//REDRAW POR ALTERÇAO DO TIMER------------------------------------------------------------------------------------------------------------------------
				al_draw_bitmap(bg, 0, 0, 0);
				al_draw_bitmap(lifeBar, 600, 0, 0);
				al_draw_bitmap(lifeBarInv, 600, 0, 0);
				al_draw_bitmap(tntBar, 500, 0, 0);
				al_draw_bitmap(alplayer, player.x[1], player.y[1], 0);
				for(i=0;i<3;i++){
					al_draw_bitmap(alenemy[i], enemy.x[i], enemy.y[i], 0);
					if(tnt.timer[i] >= 175 && tnt.timer[i] <= 180)
						al_draw_bitmap(altnt[i], tnt.x[i] - 50, tnt.y[i] - 50, 0);
					else
						al_draw_bitmap(altnt[i], tnt.x[i], tnt.y[i], 0);
				}
			
				al_flip_display();
			
			}



			//TELAS FINAIS DO JOGO------------------------------------------------------------------------------------------------------------------------
			if(checkplayerkill == 0){ //se o player morrer
				al_stop_sample_instance(main);
				al_destroy_bitmap(bg);
				bg = al_load_bitmap("resources/images/bg_lose.png"); //exibe a tela de derrota
				al_draw_bitmap(bg, 0, 0, 0);

				al_flip_display(); //atualiza a tela
				al_rest(5); //pausa a tela por 5 segundos

				menu = 1;
				al_destroy_bitmap(bgmenu);
				bgmenu = al_load_bitmap("resources/images/menu1.png");
				normal = 0;
				al_stop_timer(timerScore);

			}



			if(checkenemykill == 3){ //se todo os inimigos forem derrotados

				time_alive = al_get_timer_count(timerScore)/FPS;
				al_stop_sample_instance(main);
				arq = fopen("resources/files/record.txt","r"); //abre o arquivo que armazena o recorde de tempo para leitura
				fscanf(arq, "%f", &high_score); //armazena o conteudo do arquivo na variavel high_score

				if(time_alive < high_score){ //se o tempo do jogador for menor qu o tempo recorde
					high_score = time_alive; //a variavel que armazena o tempo recorde recebe o tempo do jogador
					arq = fopen("resources/files/record.txt", "w"); //o arquivo eh agora aberto para escrita
					fprintf(arq, "%f", time_alive); //escreve no arquivo o novo recorde alcançado
				}
			
				al_destroy_bitmap(bg);
				bg = al_load_bitmap("resources/images/bg_win.png"); //exibe a tela de vitoria
				al_draw_bitmap(bg, 0, 0, 0);
				sprintf(my_text, "Time: %.2f seconds", time_alive);
				al_draw_text(size_32, al_map_rgb(0, 0, 0), 10, 400, 0, my_text); //desenha na tela o texto com o tempo do jogador

				sprintf(my_text2, "Record: %.2f seconds", high_score);
				al_draw_text(size_32, al_map_rgb(0, 0, 0), 10, 500, 0, my_text2); //desenha na tela o texto com o recorde do jogo
				al_stop_timer(timerScore);
				al_flip_display(); //atualiza a tela
				al_rest(5); //pausa a tela por 5 segundos

				fclose(arq); //fecha o arquivo

				menu = 1;
				al_destroy_bitmap(bgmenu);
				bgmenu = al_load_bitmap("resources/images/menu1.png");
				normal = 0;			
			}
		}

		al_destroy_bitmap(bg);
		bg = al_load_bitmap("resources/images/bg_main.png");
		lifeBarInv = al_load_bitmap("resources/images/Life_Inv.png");
		checkenemykill = 0;
		checkplayerkill = 1;

		//PROPRIEDADES INICIAS DO PLAYER------------------------------------------------------------------------------------------------------------------------
		player.x[1] = 0;
		player.y[1] = 100;
		player.dx[1] = 50;
		player.dy[1] = 50;
		player.direction[1] = 1;
		player.timer[1] = 0;
		player.vulnerable[1] = 0;
		


		//PROPRIEDADE INICIAS DOS INIMIGOS------------------------------------------------------------------------------------------------------------------------
		for(i=0;i<2;i++){
			enemy.x[i] = 100*(rand()%6); //spawna o inimigo numa posiaçao aleatoria do mapa
			enemy.y[i] = 100*(rand()%4)+200;
		}
		enemy.x[2] = 750;
		enemy.y[2] = 550;
		enemy.direction[0] = 0;
		enemy.direction[1] = 2;
		enemy.direction[2] = 0;
		
		for(i=0;i<3;i++){
			enemy.dx[i] = 50;
			enemy.dy[i] = 50;
			enemy.timer[i] = 0;
		}



		//PROPRIEDADES INICIAS DAS BOMBAS------------------------------------------------------------------------------------------------------------------------
		for(i=0;i<3;i++){
			tnt.x[i] = 2000; //spawna as bombas em posiçoes fora da area de visao do jogador
			tnt.y[i] = 2000;
		}

		for(i=0;i<3;i++){
			tnt.available[i] = 1;
		}



		//MODO HARDCORE
		while(hardcore == 1){
			
			al_wait_for_event(event_queue, &ev);
			//EVENTO DO TIPO PRESSIONAMENTO DE UMA TECLA
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
				switch(ev.keyboard.keycode){
					
					//SETA PRA CIMA
					case ALLEGRO_KEY_UP: //movimenta o inimigo caso ele ja esteja olhando nessa direçao
						if(player.y[1] > 100 && player.direction[1] == 0){ //evita que o player saia pra fora do mapa
							player.y[1] -= player.dy[1]; //movimenta o player
						}
						else{
							player.direction[1] = 0; //apenas muda a direçao em que o player esta olhando
							al_destroy_bitmap(alplayer); //remove a imagem do player
							alplayer = al_load_bitmap("resources/images/player1_back.png"); //atribui uma nova imagem relacioanado a direçao que o player esta olhando
						}
						break;
					

					//SETA PRA BAIXO
					case ALLEGRO_KEY_DOWN:
						if(player.y[1] < SCREEN_H - player_size && player.direction[1] == 1){
							player.y[1] += player.dy[1];
						}
						else{
							player.direction[1] = 1;
							al_destroy_bitmap(alplayer);
							alplayer = al_load_bitmap("resources/images/player1_front.png");
						}
						break;
					

					//SETA PRA ESQUERDA
					case ALLEGRO_KEY_LEFT:
						if(player.x[1] > 0 && player.direction[1] == 2){
							player.x[1] -= player.dx[1];
						}
						else{
							player.direction[1] = 2;
							al_destroy_bitmap(alplayer);
							alplayer = al_load_bitmap("resources/images/player1_left.png");
						}
						break;
					

					//SETA PRA DIREITA
					case ALLEGRO_KEY_RIGHT:
						if(player.x[1] < SCREEN_W - player_size && player.direction[1] == 3){
							player.x[1] += player.dx[1];
						}
						else{
							player.direction[1] = 3;
							al_destroy_bitmap(alplayer);
							alplayer = al_load_bitmap("resources/images/player1_right.png");
						}
						break;


					//TECLA SPACE
					case ALLEGRO_KEY_SPACE:
						for(i=0;i<3;i++){
							if(tnt.available[i]){ //verifica se existem bombas disponiveis no estoque, se houver:

								al_destroy_bitmap(altnt[i]); //remove a imagem atribuida a bomba
								altnt[i] = al_load_bitmap("resources/images/tnt3.png"); //atribui a imagem da bomba com o relogio em 3 segundos

								tnt.timer[i] = 0; //zera o contador da bomba para o inicio de uma nova contagem
								if(player.direction[1] == 0 && player.y[1] > 100){ //verifica a direçao que o player esta olhando e se o quadrante imediatamente a frente esta dentro da area de visao do jogador
									tnt.x[i] = player.x[1]; //coloca a bomba na posiçao desejada
									tnt.y[i] = player.y[1] - 50;
									tnt.available[i] = 0; //indica que essa bomba nao esta mais disponivel no inventario
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								if(player.direction[1] == 1 && player.y[1] < SCREEN_H - player_size){
									tnt.x[i] = player.x[1];
									tnt.y[i] = player.y[1] + 50;
									tnt.available[i] = 0;
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								if(player.direction[1] == 2 && player.x[1] > 0){
									tnt.x[i] = player.x[1] - 50;
									tnt.y[i] = player.y[1];
									tnt.available[i] = 0;
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								if(player.direction[1] == 3 && player.x[1] < SCREEN_W - player_size){
									tnt.x[i] = player.x[1] + 50;
									tnt.y[i] = player.y[1];
									tnt.available[i] = 0;
									al_stop_sample_instance(ignite);
									al_play_sample_instance(ignite);
								}
								break;
							}
						}
						break;
					

					//TECLA ESC
					case ALLEGRO_KEY_ESCAPE:
						return 0; //caso o jogador aperte a tecla esc o jogo eh fechado
					
				}
			}
			


			//EVENTO DO TIPO PASSAGEM DE TEMPO
			if(ev.type == ALLEGRO_EVENT_TIMER){

				al_play_sample_instance(main);



				//BARRA DE VIDA------------------------------------------------------------------------------------------------------------------------
				if(player.timer[1] < 180)
					player.timer[1]++;

				if(player.timer[1] == 180){
					al_destroy_bitmap(lifeBarInv);
					lifeBarInv = al_load_bitmap("resources/images/Life_Inv0.png");
					player.vulnerable[1] = 1;
				}

				switch(checkplayerkill){
					case 3:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_Full.png");
					break;

					case 2:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_2.png");
					break;

					case 1:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_1.png");
					break;

					case 0:
					al_destroy_bitmap(lifeBar);
					lifeBar = al_load_bitmap("resources/images/Life_Empty.png");
					break;
				}



				//BARRA DE BOMBAS DISPONIVEIS------------------------------------------------------------------------------------------------------------------------
				if(tnt.available[0] && tnt.available[1] && tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar3.png");
				}
				else if(!tnt.available[0] && !tnt.available[1] && !tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar0.png");
				}
				else if(tnt.available[0] && tnt.available[1]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar2.png");
				}
				else if(tnt.available[0] && tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar2.png");
				}
				else if(tnt.available[1] && tnt.available[2]){
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar2.png");
				}
				else{
					al_destroy_bitmap(tntBar);
					tntBar = al_load_bitmap("resources/images/tnt_bar1.png");
				}
					

				//TNT------------------------------------------------------------------------------------------------------------------------

				//CONTAGEM PARA EXPLOSAO DAS BOMBAS
				for(i=0;i<3;i++){
					if(tnt.timer[i] < 180) tnt.timer[i]++; //se o timer for menor que 180 (3 segundos), acrescenta +1 ao contador
					else{ //senao:
						tnt.timer[i] = 0; //zera o contador da bomba
						tnt.x[i] = 2000; //teleporta a bomba para uma posiçao nao visivel do mapa
						tnt.y[i] = 2000;
						tnt.available[i] = 1; //define que essa bomba esta novamente disponivel no inventario
					}
					if(tnt.timer[i] == 0){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt3.png"); //altera para a imagem do relogio em 3 segundos
					}
					if(tnt.timer[i] == 60){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt2.png"); //altera para a imagem do relogio em 3 segundos
					}
					if(tnt.timer[i] == 120){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt1.png"); //altera para a imagem do relogio em 3 segundos
					}
					if(tnt.timer[i] == 175 && tnt.x[i] != 2000){
						al_destroy_bitmap(altnt[i]);
						altnt[i] = al_load_bitmap("resources/images/tnt_explosion.png"); //altera para a imagem de uma explosao
						al_stop_sample_instance(explosion);
						al_play_sample_instance(explosion);
					}
				}



				//COLISAO------------------------------------------------------------------------------------------------------------------------
				
				//DANO POR COLISAO
				for(i=0;i<3;i++){
					if(player.x[1] == enemy.x[i] && player.y[1] == enemy.y[i]){
						if(player.vulnerable[1] == 1){
							al_play_sample_instance(hurt);
							checkplayerkill--;
							player.vulnerable[1] = 0;
							player.timer[1] = 0;
							al_destroy_bitmap(lifeBarInv);
							lifeBarInv = al_load_bitmap("resources/images/Life_Inv.png");
						}
					}
				}


				//DANO POR BOMBA
				for(i=0;i<3;i++){
					for(j=0;j<3;j++){
						if(tnt.timer[i] == 179){
							if((fabs(tnt.x[i] - enemy.x[j]) == 50 && fabs(tnt.y[i] - enemy.y[j]) == 0) || fabs(tnt.y[i] - enemy.y[j]) == 50 && fabs(tnt.x[i] - enemy.x[j]) == 0){
								enemy.x[j] = 1200;
								enemy.y[j] = 1200;
								enemy.dx[j] = 0;
								enemy.dy[j] = 0;
								checkenemykill++;
							}
							if((fabs(tnt.x[i] - player.x[1]) == 50 && fabs(tnt.y[i] - player.y[1]) == 0) || fabs(tnt.y[i] - player.y[1]) == 50 && fabs(tnt.x[i] - player.x[1]) == 0){
								if(player.vulnerable[1] == 1){
									al_play_sample_instance(hurt);
									checkplayerkill--;
									player.vulnerable[1] = 0;
									player.timer[1] = 0;
									al_destroy_bitmap(lifeBarInv);
									lifeBarInv = al_load_bitmap("resources/images/Life_Inv.png");
								}
							}
						}
					}
				}


				//NAO PERMITE QUE O PLAYER ANDE SE HOUVER UMA BOMBA NO CAMINHO (A BOMBA OCUPA ESPAÇO)
				for(i=0;i<3;i++){
					if(player.direction[1] == 0 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.y[1] = tnt.y[i]+50;
					if(player.direction[1] == 1 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.y[1] = tnt.y[i]-50;
					if(player.direction[1] == 2 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.x[1] = tnt.x[i]+50;
					if(player.direction[1] == 3 && player.x[1] == tnt.x[i] && player.y[1] == tnt.y[i])
						player.x[1] = tnt.x[i]-50;
				}
				

				//RECUO DOS INIMIGOS CASO HAJA UMA BOMBA NO CAMINHO
				for(i=0;i<2;i++){
					for(j=0;j<3;j++){ //sao usadas as variaveis i e j para testar a possibilidade de colisao entre todos os inimigos e todas as bombas
						if(enemy.direction[i] == 0 && enemy.x[i] == tnt.x[j] && enemy.y[i] == tnt.y[j]+50){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_front.png");
							enemy.direction[i] = 1;
						}
						else if(enemy.direction[i] == 1 && enemy.x[i] == tnt.x[j] && enemy.y[i] == tnt.y[j]-50){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_back.png");
							enemy.direction[i] = 0;
						}
						else if(enemy.direction[i] == 2 && enemy.x[i] == tnt.x[j]+50 && enemy.y[i] == tnt.y[j]){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
							enemy.direction[i] = 3;
						}
						else if(enemy.direction[i] == 3 && enemy.x[i] == tnt.x[j]-50 && enemy.y[i] == tnt.y[j]){
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
							enemy.direction[i] = 2;
						}
					}
				}
				


				//MOVIMENTAÇAO------------------------------------------------------------------------------------------------------------------------	
				
				//MOVIMENTO DO INIMIGO PADRAO
				if(enemy.timer[0] < 10) enemy.timer[0]++; //contador para cada movimentaçao do inimigo
				else enemy.timer[0] = 0;
				
				for(i=0;i<2;i++){
					if(enemy.timer[0] == 9){
						switch(enemy.direction[i]){ //verifica a direçao que o inimigo esta olhando
									
						case 0: //UP
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_back.png");
						if(enemy.y[i] > 100) //caso o inimigo esteja dentro do limite do mapa, ocorre movimentaçao
							enemy.y[i] -= enemy.dy[i];
						else{ //caso contrario, a direçao do inimigo eh trocada
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_front.png");
							enemy.direction[i] = 1;
						}
						break;

						case 1: //DOWN
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_front.png");
						if(enemy.y[i] < SCREEN_H - enemy_size)
							enemy.y[i] += enemy.dy[i];
						else{
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_back.png");
							enemy.direction[i] = 0;
						}
						break;

						case 2: //LEFT
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_left.png");
						if(enemy.x[i] > 0)
							enemy.x[i] -= enemy.dx[i];
						else{
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
							enemy.direction[i] = 3;
						}
						break;

						case 3: //RIGHT
						al_destroy_bitmap(alenemy[i]);
						alenemy[i] = al_load_bitmap("resources/images/enemy1_right.png");
						if(enemy.x[i] < SCREEN_W - enemy_size)
							enemy.x[i] += enemy.dx[i];
						else{
							al_destroy_bitmap(alenemy[i]);
							alenemy[i] = al_load_bitmap("resources/images/enemy1_left.png");
							enemy.direction[i] = 2;
						}
						break;

						}
					}
				}

				
				//MOVIMENTO DO INIMIGO ESPECIAL
				if(enemy.timer[2] < 30) enemy.timer[2]++; //contador para cada movimentaçao do inimigo
				else enemy.timer[2] = 0;
				
				if(enemy.timer[2] == 29){

					//DIREITA
					if(player.x[1] - enemy.x[2] > 0){ //verifica se o player está a direita do inimigo no eixo x
						if(enemy.direction[2] != 4){ //caso a posiçao seja diferente eh verificado a direçao que o inimigo esta olhando
							enemy.direction[2] = 4; //faz com que o inimigo olhe pra direita
							al_destroy_bitmap(alenemy[2]);
							alenemy[2] = al_load_bitmap("resources/images/enemy2_right.png");
						}
						else
							enemy.x[2] += enemy.dx[2]; //caso a direçao do inimigo ja esteja certa ele eh movimentado
					}

					//ESQUERDA	
					if(player.x[1] - enemy.x[2] < 0){
						if(enemy.direction[2] != 3){
							enemy.direction[2] = 3;
							al_destroy_bitmap(alenemy[2]);
							alenemy[2] = al_load_bitmap("resources/images/enemy2_left.png");
						}
						else
							enemy.x[2] -= enemy.dx[2];
					}
					
					//PRA BAIXO
					else if(player.x[1] == enemy.x[2]){
						if(player.y[1] - enemy.y[2] > 0){
							if(enemy.direction[2] != 1){
								enemy.direction[2] = 1;
								al_destroy_bitmap(alenemy[2]);
								alenemy[2] = al_load_bitmap("resources/images/enemy2_front.png");
							}
							else
								enemy.y[2] += enemy.dy[2];
						}

						//PRA CIMA	
						if(player.y[1] - enemy.y[2] < 0){
							if(enemy.direction[2] != 0){
								enemy.direction[2] = 0;
								al_destroy_bitmap(alenemy[2]);
								alenemy[2] = al_load_bitmap("resources/images/enemy2_back.png");
							}
							else
								enemy.y[2] -= enemy.dy[2];
						}
					}
				}

				
		
				//REDRAW POR ALTERÇAO DO TIMER------------------------------------------------------------------------------------------------------------------------
				al_draw_bitmap(bg, 0, 0, 0);
				al_draw_bitmap(lifeBar, 600, 0, 0);
				al_draw_bitmap(lifeBarInv, 600, 0, 0);
				al_draw_bitmap(tntBar, 500, 0, 0);
				al_draw_bitmap(alplayer, player.x[1], player.y[1], 0);
				for(i=0;i<3;i++){
					al_draw_bitmap(alenemy[i], enemy.x[i], enemy.y[i], 0);
					if(tnt.timer[i] >= 175 && tnt.timer[i] <= 180)
						al_draw_bitmap(altnt[i], tnt.x[i] - 50, tnt.y[i] - 50, 0);
					else
						al_draw_bitmap(altnt[i], tnt.x[i], tnt.y[i], 0);
				}
			
				al_flip_display();
			
			}



			//TELAS FINAIS DO JOGO------------------------------------------------------------------------------------------------------------------------
			if(checkplayerkill == 0){ //se o player morrer
				al_stop_sample_instance(main);
				al_destroy_bitmap(bg);
				bg = al_load_bitmap("resources/images/bg_lose.png"); //exibe a tela de derrota
				al_draw_bitmap(bg, 0, 0, 0);

				al_flip_display(); //atualiza a tela
				al_rest(5); //pausa a tela por 5 segundos

				menu = 1;
				al_destroy_bitmap(bgmenu);
				bgmenu = al_load_bitmap("resources/images/menu1.png");
				hardcore = 0;
				al_stop_timer(timerHardcore);
				
			}



			if(checkenemykill == 3){ //se todo os inimigos forem derrotados

				time_alive = al_get_timer_count(timerHardcore)/FPS;
				al_stop_sample_instance(main);
				arq2 = fopen("resources/files/recordHardcore.txt","r"); //abre o arquivo que armazena o recorde de tempo para leitura
				fscanf(arq2, "%f", &high_score); //armazena o conteudo do arquivo na variavel high_score

				if(time_alive < high_score){ //se o tempo do jogador for menor qu o tempo recorde
					high_score = time_alive; //a variavel que armazena o tempo recorde recebe o tempo do jogador
					arq2 = fopen("resources/files/recordHardcore.txt", "w"); //o arquivo eh agora aberto para escrita
					fprintf(arq2, "%f", time_alive); //escreve no arquivo o novo recorde alcançado
				}
			
				al_destroy_bitmap(bg);
				bg = al_load_bitmap("resources/images/bg_win.png"); //exibe a tela de vitoria
				al_draw_bitmap(bg, 0, 0, 0);
				sprintf(my_text, "Time: %.2f seconds", time_alive);
				al_draw_text(size_32, al_map_rgb(0, 0, 0), 10, 400, 0, my_text); //desenha na tela o texto com o tempo do jogador

				sprintf(my_text2, "Record: %.2f seconds", high_score);
				al_draw_text(size_32, al_map_rgb(0, 0, 0), 10, 500, 0, my_text2); //desenha na tela o texto com o recorde do jogo
				al_stop_timer(timerHardcore);
				al_flip_display(); //atualiza a tela
				al_rest(5); //pausa a tela por 5 segundos

				fclose(arq2); //fecha o arquivo

				menu = 1;
				al_destroy_bitmap(bgmenu);
				bgmenu = al_load_bitmap("resources/images/menu1.png");
				hardcore = 0;
				
			}
	
		}

	}	
	
	return 0;
}