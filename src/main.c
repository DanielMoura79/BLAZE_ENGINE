//////////////////////////////////////////////////////////////////////////////////////
// BLAZE ENGINE by GameDevBoss, Znemezes and Paulo Linhares - 2022                  //
// www.youtube.com/c/GameDevBoss                                                    //
// BLAZE ENGINE É GRATUITA E SEMPRE SERÁ - BLAZE ENGINE IS FREE AND ALWAYS WILL BE  //
// O CONHECIMENTO DEVE SER COMPARTILHADO - KNOWLEDGE MUST BE SHARED                 //
//////////////////////////////////////////////////////////////////////////////////////

#include <genesis.h>
#include "sprite.h"
#include "gfx.h"
#include "sound.h"

//--- DEFINICOES ---//
#define RELEASE 0
#define ATTACK_MARGIN 8
#define MAX_ENEMYS 11
#define SHAKE_ON_HIT 0
//SOUND DEFs
#define INGAME_SFX 64
#define P1_SFX     65
#define P2_SFX     66
#define ENEMYS_SFX 67

//--- FUNCOES ---//
void FUNCAO_INPUT_SYSTEM(void);
void FUNCAO_INIT_PLAYERS(void);
void CREATE_ENEMY(const u8 ID, const u16 State, const s16 pX, const s16 pY);
void FUNCAO_FSM(void);
void FUNCAO_FISICA(void);
void FUNCAO_CAMERA(void);
void FUNCAO_GERENCIAR_OBJS(void);
void FUNCAO_MOVER_OBJS(void);
void FUNCAO_SPRITE_POSITION(void);
void PLAYER_STATE(const u8 Player, u16 State);
void ENEMY_STATE(const u8 Enemy, u16 State);
void FUNCAO_CTRL_SLOTS_BGS(void);
void FUNCAO_ANIMACAO(void);
void FUNCAO_PLAY_SND(const u8 CharID, const u16 State, const u8 Chanel);
void FUNCAO_FSM_HITBOXES(const u8 Player);
void FUNCAO_FSM_HITBOXES_ENEMYS(const u8 Enemy);
bool FUNCAO_COLISAO(s16 R1x1, const s16 R1y1,
					s16 R1x2, const s16 R1y2,
					s16 R2x1, const s16 R2y1,
					s16 R2x2, const s16 R2y2);
void CLEAR_VDP(void);

//--- VARIAVEIS ---//
bool show_debug = 1; //Exibe o debug (caixas de colisao e outros)
u32 frames = 0; //controle de tempo
u8 room = 8; //sala inicial
s16 MAX_ENERGY = 55; //energia maxima
s16 invisibleWallY; //barreira invisivel no eixo Y
u8 gPing2 = 0; //Variavel que alterna entre 0 e 1 constantemente
u8 gPing4 = 0; //Variavel que alterna entre 0 ; 1 ; 2 ; 3 constantemente
u8 gPing10 = 0; //Variavel que alterna entre 0 ; 1 ; 2 ... 7 ; 8 ; 9 constantemente
u8 SPR_METHOD = 1; //Metodo de carregamento dos sprites; ALTERADO EM "IN GAME FRAME 5"
u8 qtdePlayers = 1; //O jogo inicia com 1 player
u8 qtdeInimigos = 0; //O jogo começa com zero inimigos
u8 qtdeInimigosMortos = 0; //variavel que controla o scrolling
u8 activeDistance = 120; //distancia em que os inimigos percebem você, distancia de ativacao
bool enableChainSequence = 0; //controle que permite golpes em sequencia
s16 camPosX, camPosY; //posicao da camera
u8 faseAtual = 1; //fase atual
u16 gScrollValue = 0; //controle de incremento para carregar novos segmentos de cenario, 20 em 20 tiles (160px)
u16 segmentInFocus = 0; //se torna 1 logo no inicio do jogo
u16 segmentLimiter = 0; //limita o movimento
s16 LimiterX; //limitador de carregamentos dos slots de cenario
u8 margin = 24; //margem que impede o avanco do player no canto da tela
s32 lvlWSize; //largura do cenario
s32 lvlHSize; //altura do cenario
s32 VirtualPositionX; //posicao relativa x
s32 VirtualPositionY; //posicao relativa y 
char gStr[64]; //Variavel para armazenar textos, usada no debug
u8 enemyCounter; //controle de qtde de inimigos
u8 gravidadePadrao = 2; //Utilizado nos controles de Fisica
u8 impulsoPadrao = -15; //Utilizado nos controles de Fisica
bool paused;

//Sprites
Sprite* point;
Sprite* go;
Sprite* Rect1BB1_Q1; Sprite* Rect1BB1_Q2; Sprite* Rect1BB1_Q3; Sprite* Rect1BB1_Q4; 
Sprite* Rect1HB1_Q1; Sprite* Rect1HB1_Q2; Sprite* Rect1HB1_Q3; Sprite* Rect1HB1_Q4;
Sprite* Rect2BB1_Q1; Sprite* Rect2BB1_Q2; Sprite* Rect2BB1_Q3; Sprite* Rect2BB1_Q4;
Sprite* Rect2HB1_Q1; Sprite* Rect2HB1_Q2; Sprite* Rect2HB1_Q3; Sprite* Rect2HB1_Q4;
Sprite* spr_face_slot1; 
Sprite* spr_face_slot2; 
Sprite* spr_face_slot3; 
Sprite* spr_face_slot4; 
Sprite* spr_energy_slot1; 
Sprite* spr_energy_slot2; 
Sprite* spr_energy_slot3; 
Sprite* spr_energy_slot4; 
Sprite* spr_sel_P1;
Sprite* spr_sel_P2;
Sprite* spr_sel_P1_icon;
Sprite* spr_sel_P2_icon;
Sprite* GE01; //Graphic Element
Sprite* GE02;
Sprite* GE03;
Sprite* GE04;
Sprite* GE05;
Sprite* GE06;
Sprite* GE07;
Sprite* GE08;
Sprite* GE09;
Sprite* GE10;

//Inputs, Joystick
u16 JOY1;
u16 JOY2;
bool JOY1_UP, JOY1_DOWN, JOY1_LEFT, JOY1_RIGHT, JOY1_A, JOY1_B, JOY1_C, JOY1_X, JOY1_Y, JOY1_Z, JOY1_START, JOY1_MODE;
bool key_JOY1_UP_pressed, key_JOY1_UP_hold, key_JOY1_UP_released;
bool key_JOY1_DOWN_pressed, key_JOY1_DOWN_hold, key_JOY1_DOWN_released;
bool key_JOY1_LEFT_pressed, key_JOY1_LEFT_hold, key_JOY1_LEFT_released;
bool key_JOY1_RIGHT_pressed, key_JOY1_RIGHT_hold, key_JOY1_RIGHT_released;
bool key_JOY1_A_pressed, key_JOY1_A_hold, key_JOY1_A_released;
bool key_JOY1_B_pressed, key_JOY1_B_hold, key_JOY1_B_released;
bool key_JOY1_C_pressed, key_JOY1_C_hold, key_JOY1_C_released;
bool key_JOY1_X_pressed, key_JOY1_X_hold, key_JOY1_X_released;
bool key_JOY1_Y_pressed, key_JOY1_Y_hold, key_JOY1_Y_released;
bool key_JOY1_Z_pressed, key_JOY1_Z_hold, key_JOY1_Z_released;
bool key_JOY1_START_pressed, key_JOY1_START_hold, key_JOY1_START_released;
bool key_JOY1_MODE_pressed, key_JOY1_MODE_hold, key_JOY1_MODE_released;

bool JOY2_UP, JOY2_DOWN, JOY2_LEFT, JOY2_RIGHT, JOY2_A, JOY2_B, JOY2_C, JOY2_X, JOY2_Y, JOY2_Z, JOY2_START, JOY2_MODE;
bool key_JOY2_UP_pressed, key_JOY2_UP_hold, key_JOY2_UP_released;
bool key_JOY2_DOWN_pressed, key_JOY2_DOWN_hold, key_JOY2_DOWN_released;
bool key_JOY2_LEFT_pressed, key_JOY2_LEFT_hold, key_JOY2_LEFT_released;
bool key_JOY2_RIGHT_pressed, key_JOY2_RIGHT_hold, key_JOY2_RIGHT_released;
bool key_JOY2_A_pressed, key_JOY2_A_hold, key_JOY2_A_released;
bool key_JOY2_B_pressed, key_JOY2_B_hold, key_JOY2_B_released;
bool key_JOY2_C_pressed, key_JOY2_C_hold, key_JOY2_C_released;
bool key_JOY2_X_pressed, key_JOY2_X_hold, key_JOY2_X_released;
bool key_JOY2_Y_pressed, key_JOY2_Y_hold, key_JOY2_Y_released;
bool key_JOY2_Z_pressed, key_JOY2_Z_hold, key_JOY2_Z_released;
bool key_JOY2_START_pressed, key_JOY2_START_hold, key_JOY2_START_released;
bool key_JOY2_MODE_pressed, key_JOY2_MODE_hold, key_JOY2_MODE_released;

struct PlayerDEF {
	u8  id;
	Sprite* sprite;
	Sprite* spark;
	Sprite* partB;
	u16 w;
	u16 h;
	s32 x;
	s32 y;
	s32 axisX;
	s32 axisY;
	s32 sparkX;
	s32 sparkY;
	s32 partBx;
	s32 partBy;
	u16 ground;
	u8  receiveDamageType;
	bool enableDashAirAtack;
	u8  dashDir;
	bool activeHit;
	u16 frameTimeAtual;
	u16 frameTimeTotal;
	u16 animFrame;
	u16 animFrameTotal;
	u16 state;
	s8  direction;
	u8  velocidade;
	u8  cicloInteracoesGravidade;     //Controles de Gravidade
	u8  cicloInteracoesGravidadeCont; //Controles de Gravidade
	s8  impulsoX;                     //Controles de Gravidade
	s8  gravidadeX;                   //Controles de Gravidade
	s8  impulsoY;                     //Controles de Gravidade
	s8  gravidadeY;                   //Controles de Gravidade
	s16 energy;
	u8  CountDownEnemyInfo;
	u8  CountDownSpark;
	u8  CountDownChain;
	u16 dataAnim[70]; //Total de frames disponiveis para cada estado (animacao)
	u16 dataHBox[4];  //Posiconamento das Hit Boxes (caixas vermelhas de ataque)
	u16 dataBBox[4];  //Posiconamento das Body Boxes (caixas azuis, area vulneravel a ataques)
	u16 dataMBox[4];  //Posiconamento das Mass Boxes (caixas de massa, corpo fisico do personagem, usado para empurrar)
	//JOYSTICK
	u8 key_JOY_UP_status; u8 key_JOY_DOWN_status; u8 key_JOY_LEFT_status; u8 key_JOY_RIGHT_status;
	u8 key_JOY_A_status; u8 key_JOY_B_status; u8 key_JOY_C_status;
	u8 key_JOY_X_status; u8 key_JOY_Y_status; u8 key_JOY_Z_status;
	u8 key_JOY_START_status; u8 key_JOY_MODE_status;
	u8 key_JOY_countdown[10]; //timer regressivo, ativado apos pressionar um botao direcional (usado para correr, esquivar, etc)
}; struct PlayerDEF P[3];

struct EnemyDEF {
	bool active;
	u8  id;
	Sprite* sprite;
	Sprite* spark;
	Sprite* partB;
	u16 w;
	u16 h;
	s32 x;
	s32 y;
	s32 axisX;
	s32 axisY;
	s32 sparkX;
	s32 sparkY;
	s32 partBx;
	s32 partBy;
	u16 frameTimeAtual;
	u16 frameTimeTotal;
	u16 animFrame;
	u16 animFrameTotal;
	u16 state;
	u16 ground;
	u8  receiveDamageType;
	bool enableDashAirAtack;
	u8  dashDir;
	u8  CountDownSpark;
	u8  CountDownChain;
	s8  direction;
	u8  velocidade;
	u8  cicloInteracoesGravidade;     //Controles de Gravidade
	u8  cicloInteracoesGravidadeCont; //Controles de Gravidade
	s8  impulsoX;                     //Controles de Gravidade
	s8  gravidadeX;                   //Controles de Gravidade
	s8  impulsoY;                     //Controles de Gravidade
	s8  gravidadeY;                   //Controles de Gravidade
	s16 energy;
	u16 dataAnim[10]; //Total de frames disponiveis para cada estado (animacao)
	u16 dataHBox[4];  //Posiconamento das Hit Boxes (caixas vermelhas de ataque)
	u16 dataBBox[4];  //Posiconamento das Body Boxes (caixas azuis, area vulneravel a ataques)
	u16 dataMBox[4];  //Posiconamento das Mass Boxes (caixas de massa, corpo fisico do personagem, usado para empurrar)
	//JOYSTICK
	u8 key_JOY_UP_status; u8 key_JOY_DOWN_status; u8 key_JOY_LEFT_status; u8 key_JOY_RIGHT_status;
	u8 key_JOY_A_status; u8 key_JOY_B_status; u8 key_JOY_C_status;
	u8 key_JOY_X_status; u8 key_JOY_Y_status; u8 key_JOY_Z_status;
	u8 key_JOY_START_status; u8 key_JOY_MODE_status;
	u8 key_JOY_countdown[10]; //timer regressivo, ativado apos pressionar um botao direcional (usado para correr, esquivar, etc)
}; struct EnemyDEF E[MAX_ENEMYS];

#define UNUSED_PARAMETER(X) ((void)X)

int main(u16 hard) //--- MAIN ---//
{
	UNUSED_PARAMETER(hard);

    //inicializacao da VDP (Video Display Processor)
	SYS_disableInts();
	 VDP_init();
	 VDP_setScreenWidth320();
	 VDP_setScreenHeight224();
	 //VDP_setPlaneSize(64, 64, FALSE); //desabilitado no momento, esse comando permite BGs de 512x512px
	 VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	SYS_enableInts();
	
	//inicializacao de variaveis
	frames = 0;
	room = 8;
	camPosX = 0;
	camPosY = 0;
	paused = FALSE;
	
	//inicializacao de sprites
	SPR_init();
	VDP_setBackgroundColor(0); //Range 0-63 //4 Paletas de 16 cores = 64 cores
	
    while(TRUE) //MAIN LOOP
    {
		FUNCAO_INPUT_SYSTEM();
		
		if(room==8) //CREDITS SCREEN
		{
			if(frames==0)
			{
				VDP_setPaletteColors(0, (u16 *)palette_black, 64); 
				//BG_B
				VDP_loadTileSet(bgb_credits.tileset, 1, DMA); //Load the tileset
				VDP_setTileMapEx(BG_B,bgb_credits.tilemap, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 1), 0, 0, 0, 0, 40, 28, DMA);
				//BG_A
				VDP_loadTileSet(bga_credits.tileset, 501, DMA); //Load the tileset
				VDP_setTileMapEx(BG_A,bga_credits.tilemap, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 501), 0, 0, 0, 0, 40, 28, DMA);
			}
			if(frames==1)
			{
				VDP_setPalette(PAL0, bgb_credits.palette->data);
				VDP_setPalette(PAL1, bga_credits.palette->data);
			}
			if(
			(
			P[1].key_JOY_A_status==1 || 
			P[1].key_JOY_B_status==1 || 
			P[1].key_JOY_C_status==1 || 
			P[1].key_JOY_X_status==1 || 
			P[1].key_JOY_Y_status==1 || 
			P[1].key_JOY_Z_status==1 || 
			P[1].key_JOY_START_status==1 || 
			P[1].key_JOY_MODE_status==1 
			)
			&& frames>30){ CLEAR_VDP(); room=9; frames=0; }
		}
		
		if(room==9) //MAIN SCREEN
		{
			if(frames==0)
			{
				//SEGMENTO1
				VDP_loadTileSet(bgb_logo.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,bgb_logo.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),0,0,0,0,40,32,DMA_QUEUE);
				VDP_setPalette(PAL0, bgb_logo.palette->data);
				
				//sound, sfx
				XGM_setPCM(INGAME_SFX, snd_main_screen, sizeof(snd_main_screen)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
			}
			if(
			(
			P[1].key_JOY_A_status==1 || 
			P[1].key_JOY_B_status==1 || 
			P[1].key_JOY_C_status==1 || 
			P[1].key_JOY_X_status==1 || 
			P[1].key_JOY_Y_status==1 || 
			P[1].key_JOY_Z_status==1 || 
			P[1].key_JOY_START_status==1 || 
			P[1].key_JOY_MODE_status==1 
			)
			&& frames>30)
			{ 
				CLEAR_VDP(); room=10; frames=0; 
			}
		}
		
		if(room==10) //PLAYER SELECT
		{
			if(frames==0)
			{
				//BGB
				VDP_loadTileSet(bgb_select.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,bgb_select.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),0,0,0,0,40,32,DMA_QUEUE);
				VDP_setPalette(PAL0, bgb_select.palette->data);
				
				qtdePlayers = 1;
				
				E[1].sprite = SPR_addSprite(&spr_icon_p1,   0, 32, TILE_ATTR(PAL1,FALSE,FALSE,FALSE) );
				VDP_setPalette(PAL1, spr_icon_p1.palette->data);
				if(E[1].sprite){ SPR_setDepth(E[1].sprite, 0); }
				
				P[1].id=1;
				P[1].sprite = SPR_addSprite(&spr_sel_char01, 2, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) );
				VDP_setPalette(PAL2, spr_sel_char01.palette->data);
			}
			
			//sair
			if(
			(
			P[1].key_JOY_A_status==1 || 
			P[1].key_JOY_B_status==1 || 
			P[1].key_JOY_C_status==1 || 
			P[1].key_JOY_X_status==1 || 
			P[1].key_JOY_Y_status==1 || 
			P[1].key_JOY_Z_status==1 || 
			P[1].key_JOY_START_status==1 || 
			P[1].key_JOY_MODE_status==1 
			)
			&& frames>30)
			{ 
				if (P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; }
				if (P[2].sprite){ SPR_releaseSprite(P[2].sprite); P[2].sprite = NULL; }
				if (E[1].sprite){ SPR_releaseSprite(E[1].sprite); E[1].sprite = NULL; }
				if (E[2].sprite){ SPR_releaseSprite(E[2].sprite); E[2].sprite = NULL; }
				
				//sound, sfx
				XGM_setPCM(INGAME_SFX, snd_confirm, sizeof(snd_confirm)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
				
				CLEAR_VDP(); room=11; frames=0; 
			}
			
			//add P2
			if(JOY2_START && qtdePlayers==1)
			{
				qtdePlayers=2;
				
				//sound, sfx
				XGM_setPCM(INGAME_SFX, snd_main_screen, sizeof(snd_main_screen)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
				
				E[2].sprite = SPR_addSprite(&spr_icon_p2, 296, 32, TILE_ATTR(PAL1,FALSE,FALSE,FALSE) );
				if(E[2].sprite){ SPR_setDepth(E[2].sprite, 0); }
				
				P[2].id=4;
				P[2].sprite = SPR_addSprite(&spr_sel_char04, 242, 32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) );
				VDP_setPalette(PAL3, spr_sel_char04.palette->data);
			}
			
			//Seleciona o personagem:
			//primeiro, atualiza o ID de acordo com a selecao atual;
			//segundo, exibe a imagem correspondente, em sua respectiva posicao na tela
			
			for(u8 i=1; i<=2; i++)
			{
				bool atualiza_tela=0;
				if(i<=qtdePlayers)
				{
					//Etapa 1
					if(P[i].id==1 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=5; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=5; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=4; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=2; atualiza_tela=1; }
					}
					if(P[i].id==2 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=6; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=6; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=1; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=3; atualiza_tela=1; }
					}
					if(P[i].id==3 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=8; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=8; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=2; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=4; atualiza_tela=1; }
					}
					if(P[i].id==4 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=9; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=9; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=3; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=1; atualiza_tela=1; }
					}
					if(P[i].id==5 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=1; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=1; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=9; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=6; atualiza_tela=1; }
					}
					if(P[i].id==6 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=2; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=2; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=5; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=7; atualiza_tela=1; }
					}
					if(P[i].id==7 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=2; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=3; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=6; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=8; atualiza_tela=1; }
					}
					if(P[i].id==8 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=3; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=3; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=7; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=9; atualiza_tela=1; }
					}
					if(P[i].id==9 && atualiza_tela==0)
					{
						if(P[i].key_JOY_UP_status   ==1){ P[i].id=4; atualiza_tela=1; }
						if(P[i].key_JOY_DOWN_status ==1){ P[i].id=4; atualiza_tela=1; }
						if(P[i].key_JOY_LEFT_status ==1){ P[i].id=8; atualiza_tela=1; }
						if(P[i].key_JOY_RIGHT_status==1){ P[i].id=5; atualiza_tela=1; }
					}
					//Etapa 2
					if(atualiza_tela==1)
					{
						if (P[i].sprite){ SPR_releaseSprite(P[i].sprite); P[i].sprite = NULL; }
						if(i==1)
						{
							if(P[i].id==1)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char1_namevoice, sizeof(snd_char1_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char01,   2,  32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char01.palette->data); SPR_setPosition(E[1].sprite,   0,  32); 
							}
							if(P[i].id==2)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char2_namevoice, sizeof(snd_char2_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char02,  82,  32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char02.palette->data); SPR_setPosition(E[1].sprite,  80,  32); 
							}
							if(P[i].id==3)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char3_namevoice, sizeof(snd_char3_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char03, 162,  32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char03.palette->data); SPR_setPosition(E[1].sprite, 160,  32); 
							}
							if(P[i].id==4)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char4_namevoice, sizeof(snd_char4_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char04, 242,  32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char04.palette->data); SPR_setPosition(E[1].sprite, 240,  32); 
							}
							if(P[i].id==5)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char05,   2, 160, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char05.palette->data); SPR_setPosition(E[1].sprite,   0, 184); 
							}
							if(P[i].id==6)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char06,  66, 160, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char06.palette->data); SPR_setPosition(E[1].sprite,  64, 184); 
							}
							if(P[i].id==7)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char07, 130, 160, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char07.palette->data); SPR_setPosition(E[1].sprite, 128, 184); 
							}
							if(P[i].id==8)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char08, 194, 160, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char08.palette->data); SPR_setPosition(E[1].sprite, 192, 184); 
							}
							if(P[i].id==9)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3);
								P[i].sprite = SPR_addSprite(&spr_sel_char09, 258, 160, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL2, spr_sel_char09.palette->data); SPR_setPosition(E[1].sprite, 256, 184); 
							}
						}
						if(i==2)
						{
							if(P[i].id==1)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char1_namevoice, sizeof(snd_char1_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char01,   2,  32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char01.palette->data); SPR_setPosition(E[2].sprite,   0+56,  32); 
							}
							if(P[i].id==2)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char2_namevoice, sizeof(snd_char2_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char02,  82,  32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char02.palette->data); SPR_setPosition(E[2].sprite,  80+56,  32); 
							}
							if(P[i].id==3)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char3_namevoice, sizeof(snd_char3_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char03, 162,  32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char03.palette->data); SPR_setPosition(E[2].sprite, 160+56,  32); 
							}
							if(P[i].id==4)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_char4_namevoice, sizeof(snd_char4_namevoice)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char04, 242,  32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char04.palette->data); SPR_setPosition(E[2].sprite, 240+56,  32); 
							}
							if(P[i].id==5)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char05,   2, 160, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char05.palette->data); SPR_setPosition(E[2].sprite,   0+40, 184); 
							}
							if(P[i].id==6)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char06,  66, 160, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char06.palette->data); SPR_setPosition(E[2].sprite,  64+40, 184); 
							}
							if(P[i].id==7)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char07, 130, 160, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char07.palette->data); SPR_setPosition(E[2].sprite, 128+40, 184); 
							}
							if(P[i].id==8)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char08, 194, 160, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char08.palette->data); SPR_setPosition(E[2].sprite, 192+40, 184); 
							}
							if(P[i].id==9)
							{ 
								//sound, sfx
								XGM_setPCM(INGAME_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH4);
								P[i].sprite = SPR_addSprite(&spr_sel_char09, 258, 160, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); VDP_setPalette(PAL3, spr_sel_char09.palette->data); SPR_setPosition(E[2].sprite, 256+40, 184); 
							}
						}
					}
				}
			}//---end "Seleciona o personagem"
			
		}
		
		if(room==11) //IN GAME
		{
			if(frames==0)
			{
				//tamanho das fases
				if(faseAtual==1){ lvlWSize = 2304; lvlHSize = 256; invisibleWallY = 150; }
				if(faseAtual==2){ lvlWSize = 1280; lvlHSize = 224; }
				if(faseAtual==3){  } 
				
				qtdeInimigosMortos = 0;
				
				//SEGMENTO1
				if(faseAtual==1)
				{
					VDP_loadTileSet(map0101.tileset,0x0001,DMA); //0x0000 = 0 dec
					VDP_setTileMapEx(BG_B,map0101.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
					VDP_setPalette(PAL0, map0101.palette->data);
					gScrollValue+=20;
				}
				if(faseAtual==2)
				{
					VDP_loadTileSet(map0201.tileset,0x0001,DMA); //0x0000 = 0 dec
					VDP_setTileMapEx(BG_B,map0201.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
					VDP_setPalette(PAL0, map0201.palette->data);
					gScrollValue+=20;
				}
				
				//OVERLAYER
				VDP_loadTileSet(bga_overlayer.tileset,0x03E7,DMA); //0x03E7 = 999 dec
				VDP_setTileMapEx(BG_A,bga_overlayer.tilemap,TILE_ATTR_FULL(PAL3,1,FALSE,FALSE,0x03E7),0,0,0,0,40,6,DMA_QUEUE);
				VDP_setPalette(PAL3, bga_overlayer.palette->data);
				
			}
			if(frames==1)
			{
				//SEGMENTO2
				if(faseAtual==1)
				{
					VDP_loadTileSet(map0102.tileset,0x0154,DMA); //0x0154 = 340 dec
					VDP_setTileMapEx(BG_B,map0102.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
					VDP_setPalette(PAL0, map0102.palette->data);
					gScrollValue+=20;
				}
				if(faseAtual==2)
				{
					VDP_loadTileSet(map0202.tileset,0x0154,DMA); //0x0154 = 340 dec
					VDP_setTileMapEx(BG_B,map0202.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
					VDP_setPalette(PAL0, map0202.palette->data);
					gScrollValue+=20;
				}
				
			}
			if(frames==5) //LOAD P1;P2
			{
				if(SPR_METHOD==1){ point = SPR_addSprite(&spr_point, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
				if(SPR_METHOD==2){ point = SPR_addSpriteExSafe(&spr_point, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				
				spr_energy_slot1 = SPR_addSprite(&spr_energy_bar,  23,  8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) );
				if(spr_energy_slot1)
				{
					SPR_setVRAMTileIndex(spr_energy_slot1, 1443); //define uma posicao especifica para o GFX na VRAM
					SPR_setDepth(spr_energy_slot1, 1);
					SPR_setAnimAndFrame(spr_energy_slot1, 0, 56); 
				} 
				if(qtdePlayers>=2)
				{
					spr_energy_slot2 = SPR_addSprite(&spr_energy_bar, 255,  8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) );
					if(spr_energy_slot2)
					{
						SPR_setVRAMTileIndex(spr_energy_slot2, 1466); //define uma posicao especifica para o GFX na VRAM
						SPR_setDepth(spr_energy_slot2, 1);
						SPR_setAnimAndFrame(spr_energy_slot2, 0, 56); 
					} 
				}
				spr_energy_slot3 = SPR_addSprite(&spr_energy_bar,  23, 32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) );
				if(spr_energy_slot3)
				{
					SPR_setVRAMTileIndex(spr_energy_slot3, 1477); //define uma posicao especifica para o GFX na VRAM
					SPR_setDepth(spr_energy_slot3, 1);
					SPR_setVisibility(spr_energy_slot3, HIDDEN);
				} 
				spr_energy_slot4 = SPR_addSprite(&spr_energy_bar, 255, 32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) );
				if(spr_energy_slot4)
				{
					SPR_setVRAMTileIndex(spr_energy_slot4, 1488); //define uma posicao especifica para o GFX na VRAM
					SPR_setDepth(spr_energy_slot4, 1);
					SPR_setVisibility(spr_energy_slot4, HIDDEN);
				} 
				
				//LOAD CHARACTERS
				
				if(P[1].id==1)
				{ 
					P[1].sprite = SPR_addSprite(&spr_mum_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_mum_000, 8, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==2)
				{ 
					P[1].sprite = SPR_addSprite(&spr_cap_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_cap_000, 8, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==3)
				{ 
					P[1].sprite = SPR_addSprite(&spr_nin_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_nin_000, 8, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==4)
				{ 
					P[1].sprite = SPR_addSprite(&spr_bab_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_bab_000, 8, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==5)
				{ 
					P[1].sprite = SPR_addSprite(&spr_e01_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_e01_000, 8, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==6)
				{ 
					P[1].sprite = SPR_addSprite(&spr_e02_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_e02_000, 8, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==7)
				{ 
					P[1].sprite = SPR_addSprite(&spr_e03_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_e03_000, 8, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==8)
				{ 
					P[1].sprite = SPR_addSprite(&spr_e04_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_e04_000, 8, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				}
				if(P[1].id==9)
				{ 
					P[1].sprite = SPR_addSprite(&spr_e05_100, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					spr_face_slot1 = SPR_addSprite(&spr_e05_000, 8, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				}
				
				if(spr_face_slot1)
				{
					SPR_setVRAMTileIndex(spr_face_slot1, 1451); //define uma posicao especifica para o GFX na VRAM
					SPR_setDepth(spr_face_slot1, 0);
				} 
				
				if(qtdePlayers==2)
				{
					if(P[2].id==1)
					{ 
						P[2].sprite = SPR_addSprite(&spr_mum_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_mum_000, 240, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==2)
					{ 
						P[2].sprite = SPR_addSprite(&spr_cap_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_cap_000, 240, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==3)
					{ 
						P[2].sprite = SPR_addSprite(&spr_nin_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_nin_000, 240, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==4)
					{ 
						P[2].sprite = SPR_addSprite(&spr_bab_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_bab_000, 240, 8, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==5)
					{ 
						P[2].sprite = SPR_addSprite(&spr_e01_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_e01_000, 240, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==6)
					{ 
						P[2].sprite = SPR_addSprite(&spr_e02_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_e02_000, 240, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==7)
					{ 
						P[2].sprite = SPR_addSprite(&spr_e03_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_e03_000, 240, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==8)
					{ 
						P[2].sprite = SPR_addSprite(&spr_e04_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_e04_000, 240, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					}
					if(P[2].id==9)
					{ 
						P[2].sprite = SPR_addSprite(&spr_e05_100, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
						spr_face_slot2 = SPR_addSprite(&spr_e05_000, 240, 8, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
					}
					
					
					if(spr_face_slot2)
					{
						SPR_setVRAMTileIndex(spr_face_slot2, 1473); //define uma posicao especifica para o GFX na VRAM
						SPR_setDepth(spr_face_slot2, 0);
					} 
				}
				
				spr_face_slot3 = SPR_addSprite(&spr_bab_000,   8, 32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				if(spr_face_slot3)
				{
					SPR_setVRAMTileIndex(spr_face_slot3, 1484); //define uma posicao especifica para o GFX na VRAM
					SPR_setDepth(spr_face_slot3, 0);
					SPR_setVisibility(spr_face_slot3, HIDDEN);
				} 
				spr_face_slot4 = SPR_addSprite(&spr_bab_000, 240, 32, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				if(spr_face_slot4)
				{
					SPR_setVRAMTileIndex(spr_face_slot4, 1495); //define uma posicao especifica para o GFX na VRAM
					SPR_setDepth(spr_face_slot4, 0);
					SPR_setVisibility(spr_face_slot4, HIDDEN);
				} 
				
				//Paleta padrao dos viloes (PAL2)
				VDP_setPalette(PAL2, spr_e01_100.palette->data);
				//Paleta padrao dos herois (PAL3)
				VDP_setPalette(PAL3, spr_mum_100.palette->data);
				
				FUNCAO_INIT_PLAYERS();
				
				SPR_METHOD = 2;
			}
			
			FUNCAO_FISICA();
			FUNCAO_CAMERA();
			FUNCAO_FSM();
			FUNCAO_ANIMACAO();
			FUNCAO_SPRITE_POSITION();
			FUNCAO_MOVER_OBJS();
			FUNCAO_CTRL_SLOTS_BGS();
			
			//atualiza barras de energia
			if(spr_energy_slot1)
			{
				SPR_setAnimAndFrame(spr_energy_slot1, 0, P[1].energy);
			}

			if(qtdePlayers>=2 && spr_energy_slot2)
			{
				SPR_setAnimAndFrame(spr_energy_slot2, 0, P[2].energy);
			}
		}
		
		/////////////
		// Colisao //
		/////////////
		
		//Attack Margin Concept
		//. . . . . . . . . . . . . . . . <- margin in Y axis
		//------------------- Enemy.y
		//Player.y ------------------
		
		for(u8 i=1; i<=qtdePlayers; i++)
		{
			for(u8 j=1; j<=MAX_ENEMYS-1; j++)
			{
				if( ((E[j].y >= P[i].y-ATTACK_MARGIN) && (E[j].y <= P[i].y+ATTACK_MARGIN)) && E[j].sprite )
				{
					if( FUNCAO_COLISAO(
					 P[i].x+P[i].dataHBox[0], P[i].y+P[i].dataHBox[1], P[i].x+P[i].dataHBox[2], P[i].y+P[i].dataHBox[3], 
					 E[j].x+E[j].dataBBox[0], E[j].y+E[j].dataBBox[1], E[j].x+E[j].dataBBox[2], E[j].y+E[j].dataBBox[3]
					)==1 )
					{
						//tratamento de colisao de ataque
						
						//P[i].dataHBox[0]=0; P[i].dataHBox[1]=0; P[i].dataHBox[2]=0; P[i].dataHBox[3]=0; //desativa a hitbox do Player
						E[j].dataBBox[0]=0; E[j].dataBBox[1]=0; E[j].dataBBox[2]=0; E[j].dataBBox[3]=0; //desativa bbox do Enemy
						
						s16 Dano = 0;
						s16 DanoFraco = -10;
						s16 DanoMedio = -15;
						s16 DanoForte = -20;
						
						if(E[j].energy==0)
						{
							E[j].direction=P[i].direction*-1; 
							ENEMY_STATE(j,550);
							FUNCAO_PLAY_SND(0,999,0);
							qtdeInimigosMortos++;
						}else{
							
							if(P[i].state==101){ ENEMY_STATE(j,501); Dano = DanoFraco; }
							if(P[i].state==102 || P[i].state==202){ ENEMY_STATE(j,502); Dano = DanoFraco; }
							if(P[i].state==103 || P[i].state==203){ ENEMY_STATE(j,501); Dano = DanoFraco; }
							if(P[i].state==104 || P[i].state==204){ E[j].direction=P[i].direction*-1; ENEMY_STATE(j,550); Dano = DanoMedio; }
							if(P[i].state==105 || P[i].state==205){ E[j].direction=P[i].direction*-1; ENEMY_STATE(j,550); Dano = DanoMedio; }
							if(P[i].state==199)
							{ 
								E[j].direction=P[i].direction*-1; 
								if(P[i].id==2)
								{
									E[j].direction=P[i].direction*-1;
									ENEMY_STATE(j,505); //state '199' do personagem Captain Commando 'id 2' faz o inimigo eletrificar
								}else{ 
									ENEMY_STATE(j,550); 
								}
								Dano = DanoMedio; 
							}
							if(P[i].state==301){ E[j].direction=P[i].direction*-1; ENEMY_STATE(j,550); Dano = DanoFraco; }
							if(P[i].state==321){ E[j].direction=P[i].direction*-1; ENEMY_STATE(j,550); Dano = DanoFraco; }
							if(P[i].state==322)
							{ 
								E[j].direction=P[i].direction*-1; 
								if(P[i].id==2)
								{
									E[j].direction=P[i].direction*-1;
									ENEMY_STATE(j,506); //state '322' do personagem Captain Commando 'id 2' faz o inimigo queimar
								}else{ 
									ENEMY_STATE(j,550); 
								}
								Dano = DanoForte; 
							}
							if(P[i].state==328){ ENEMY_STATE(j,501); Dano = DanoFraco; }
							if(P[i].state==430){ E[j].direction=P[i].direction*-1; ENEMY_STATE(j,550); Dano = DanoFraco; }
							if(P[i].state==431){ E[j].direction=P[i].direction*-1; ENEMY_STATE(j,550); Dano = DanoForte; }
							
						}
						
						if(Dano!=0)
						{
							//E[j].energy = E[j].energy+Dano;
							//if(E[j].energy<0){E[j].energy=0;}
							//if(E[j].energy>MAX_ENERGY){E[j].energy=MAX_ENERGY;}
							
							if(Dano<0)
							{
								if(E[j].energy + Dano > 0)
								{
									E[j].energy += Dano;
								}else{
									E[j].energy = 0;
									E[j].direction=P[i].direction*-1; 
									if(P[i].id!=2 && P[i].state!=322){ ENEMY_STATE(j,550); } //state '322' do personagem Captain Commando faz o inimigo queimar
									FUNCAO_PLAY_SND(0,999,0);
									qtdeInimigosMortos++;
								}
							}else{
								if(E[j].energy + Dano < MAX_ENERGY)
								{
									E[j].energy += Dano;
								}else{
									E[j].energy = MAX_ENERGY;
								}
							}
						}
						
						if(i==1)
						{
							SPR_releaseSprite(spr_face_slot3);
							if(E[j].energy==0)
							{
								if(E[j].id== 5){ spr_face_slot3 = SPR_addSprite(&spr_face_dead, 7, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
							}else{
								if(E[j].id== 5){ spr_face_slot3 = SPR_addSprite(&spr_e01_000,   8, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 6){ spr_face_slot3 = SPR_addSprite(&spr_e02_000,   8, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 7){ spr_face_slot3 = SPR_addSprite(&spr_e03_000,   8, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 8){ spr_face_slot3 = SPR_addSprite(&spr_e04_000,   8, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 9){ spr_face_slot3 = SPR_addSprite(&spr_e05_000,   8, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
							}
							SPR_setVisibility(spr_face_slot3, VISIBLE); 
							SPR_setVisibility(spr_energy_slot3, VISIBLE);
							P[1].CountDownEnemyInfo=200;

							SPR_setAnimAndFrame(spr_energy_slot3, 0,  E[j].energy);
						}
						if(i==2)
						{
							SPR_releaseSprite(spr_face_slot4);
							if(E[j].energy==0)
							{
								if(E[j].id== 5){ spr_face_slot4 = SPR_addSprite(&spr_face_dead, 239, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
							}else{
								if(E[j].id== 5){ spr_face_slot4 = SPR_addSprite(&spr_e01_000, 240, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 6){ spr_face_slot4 = SPR_addSprite(&spr_e02_000, 240, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 7){ spr_face_slot4 = SPR_addSprite(&spr_e03_000, 240, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 8){ spr_face_slot4 = SPR_addSprite(&spr_e04_000, 240, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
								if(E[j].id== 9){ spr_face_slot4 = SPR_addSprite(&spr_e05_000, 240, 32, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
							}
							SPR_setVisibility(spr_face_slot4, VISIBLE); 
							SPR_setVisibility(spr_energy_slot4, VISIBLE);
							P[2].CountDownEnemyInfo=200;

							SPR_setAnimAndFrame(spr_energy_slot4, 0, E[j].energy);
						}
						
						P[i].CountDownChain = 30; //temporizador que desativa o 'activeHit', desabilitando os combos
						P[i].activeHit = 1;
						
						//---spark ctrl init---//
						
						// 'receiveDamageType' table
						// 0 = normal
						// 1 = eletric
						// 2 = fire
						// 3 = melt
						// 4 = cut
						
						E[j].CountDownSpark = 12;
						if (E[j].spark){ SPR_releaseSprite(E[j].spark); E[j].spark = NULL; } //solta a spark se estiver ativada
						
						//ATENCAO: No caso do Captain Commando, os personagens possuem spark especificos!
						// Os sparks devem ser configurados de acordo com cada projeto
						
						if(P[i].id==1) //MUMMY
						{ 
							E[j].sparkX = E[j].x-16-camPosX;
							E[j].sparkY = P[i].y-96-camPosY;
							E[j].spark = SPR_addSpriteExSafe(&spr_hit_mum, E[j].sparkX, E[j].sparkY, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
							E[j].receiveDamageType = 3; //melt
							FUNCAO_PLAY_SND(0,4,0);
						}
						if(P[i].id==2 || P[i].id==4) //CAPTAIN; BABY
						{ 
							E[j].sparkX = E[j].x-16-camPosX;
							E[j].sparkY = P[i].y-64-camPosY;
							E[j].spark = SPR_addSpriteExSafe(&spr_spark_hit_padrao, E[j].sparkX, E[j].sparkY, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
							E[j].receiveDamageType = 0; //normal
							FUNCAO_PLAY_SND(0,0,0);
						}
						if(P[i].id==3) //NINJA
						{ 
							E[j].sparkX = E[j].x-8-camPosX;
							E[j].sparkY = P[i].y-64-camPosY;
							E[j].spark = SPR_addSpriteExSafe(&spr_hit_blood, E[j].sparkX, E[j].sparkY, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
							E[j].receiveDamageType = 4; //cut
							FUNCAO_PLAY_SND(0,4,0);
						}
						if(E[j].spark)
						{ 
							if(P[i].direction==-1){ SPR_setHFlip(E[j].spark, TRUE); } //mirror horizontal se direcao for esquerda
							SPR_setDepth(E[j].spark, 0); 
						}
						
						//---spark ctrl end---//
						
					}else{
						if( FUNCAO_COLISAO(
						 P[i].x+P[i].dataBBox[0], P[i].y+P[i].dataBBox[1], P[i].x+P[i].dataBBox[2], P[i].y+P[i].dataBBox[3], 
						 E[j].x+E[j].dataBBox[0], E[j].y+E[j].dataBBox[1], E[j].x+E[j].dataBBox[2], E[j].y+E[j].dataBBox[3]
						)==1 )
						{
							//tratamento de agarrao
							//P[i].dataHBox[0]=0; P[i].dataHBox[1]=0; P[i].dataHBox[2]=0; P[i].dataHBox[3]=0; //desativa a hitbox do Player
							//P[i].dataBBox[0]=0; P[i].dataBBox[1]=0; P[i].dataBBox[2]=0; P[i].dataBBox[3]=0; //desativa a bodybox do Player
							//E[j].dataHBox[0]=0; E[j].dataHBox[1]=0; E[j].dataHBox[2]=0; E[j].dataHBox[3]=0; //desativa a hitbox do Enemy
							//E[j].dataBBox[0]=0; E[j].dataBBox[1]=0; E[j].dataBBox[2]=0; E[j].dataBBox[3]=0; //desativa a bodybox do Enemy
							//...
						}
					}
				}
			}
		}
		
		
		///////////
		// DEBUG //
		///////////
		
		if(show_debug==1)
		{
			//VDP_setPaletteColor(15,RGB24_TO_VDPCOLOR(0xff0000));
			//VDP_drawText("[CAPTAIN COMMANDO]", 0, 0);
			/*1*/ //sprintf(gStr, "P1-> S:%d Pos:%d,%d", P[1].state, P[1].x, P[1].y ); 
			/*2*/ //sprintf(gStr, "Joy_Countdown-> ^ %d, v %d, < %d, > %d  ", P[1].key_JOY_countdown[8], P[1].key_JOY_countdown[2], P[1].key_JOY_countdown[4], P[1].key_JOY_countdown[6] ); 
			
			// sprintf(gStr, "State:%d", P[1].state ); 
			// sprintf(gStr, "E01Energy: %d", E[1].energy ); 		
			
			VDP_drawText(gStr, 1, 2);
			
			if(P[1].dataBBox[0]==P[1].dataBBox[2])
		{
			SPR_setPosition(Rect1BB1_Q1,-8,-8); SPR_setPosition(Rect1BB1_Q2,-8,-8); SPR_setPosition(Rect1BB1_Q3,-8,-8); SPR_setPosition(Rect1BB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect1BB1_Q1, P[1].x+P[1].dataBBox[0]  -camPosX, P[1].y+P[1].dataBBox[1]  -camPosY);
			SPR_setPosition(Rect1BB1_Q2, P[1].x+P[1].dataBBox[2]-8-camPosX, P[1].y+P[1].dataBBox[1]  -camPosY);
			SPR_setPosition(Rect1BB1_Q3, P[1].x+P[1].dataBBox[0]  -camPosX, P[1].y+P[1].dataBBox[3]-8-camPosY);
			SPR_setPosition(Rect1BB1_Q4, P[1].x+P[1].dataBBox[2]-8-camPosX, P[1].y+P[1].dataBBox[3]-8-camPosY);
		}
		
		if(P[1].dataHBox[0]==P[1].dataHBox[2])
		{
			SPR_setPosition(Rect1HB1_Q1,-8,-8); SPR_setPosition(Rect1HB1_Q2,-8,-8); SPR_setPosition(Rect1HB1_Q3,-8,-8); SPR_setPosition(Rect1HB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect1HB1_Q1, P[1].x+P[1].dataHBox[0]  -camPosX, P[1].y+P[1].dataHBox[1]  -camPosY);
			SPR_setPosition(Rect1HB1_Q2, P[1].x+P[1].dataHBox[2]-8-camPosX, P[1].y+P[1].dataHBox[1]  -camPosY);
			SPR_setPosition(Rect1HB1_Q3, P[1].x+P[1].dataHBox[0]  -camPosX, P[1].y+P[1].dataHBox[3]-8-camPosY);
			SPR_setPosition(Rect1HB1_Q4, P[1].x+P[1].dataHBox[2]-8-camPosX, P[1].y+P[1].dataHBox[3]-8-camPosY);
		}
		
		if(E[1].dataBBox[0]==E[1].dataBBox[2])
		{
			SPR_setPosition(Rect2BB1_Q1,-8,-8); SPR_setPosition(Rect2BB1_Q2,-8,-8); SPR_setPosition(Rect2BB1_Q3,-8,-8); SPR_setPosition(Rect2BB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect2BB1_Q1, E[1].x+E[1].dataBBox[0]  -camPosX, E[1].y+E[1].dataBBox[1]  -camPosY);
			SPR_setPosition(Rect2BB1_Q2, E[1].x+E[1].dataBBox[2]-8-camPosX, E[1].y+E[1].dataBBox[1]  -camPosY);
			SPR_setPosition(Rect2BB1_Q3, E[1].x+E[1].dataBBox[0]  -camPosX, E[1].y+E[1].dataBBox[3]-8-camPosY);
			SPR_setPosition(Rect2BB1_Q4, E[1].x+E[1].dataBBox[2]-8-camPosX, E[1].y+E[1].dataBBox[3]-8-camPosY);
		}
		
		if(E[1].dataHBox[0]==E[1].dataHBox[2])
		{
			SPR_setPosition(Rect2HB1_Q1,-8,-8); SPR_setPosition(Rect2HB1_Q2,-8,-8); SPR_setPosition(Rect2HB1_Q3,-8,-8); SPR_setPosition(Rect2HB1_Q4,-8,-8);
		}else{
			SPR_setPosition(Rect2HB1_Q1, E[1].x+E[1].dataHBox[0]  -camPosX, E[1].y+E[1].dataHBox[1]  -camPosY);
			SPR_setPosition(Rect2HB1_Q2, E[1].x+E[1].dataHBox[2]-8-camPosX, E[1].y+E[1].dataHBox[1]  -camPosY);
			SPR_setPosition(Rect2HB1_Q3, E[1].x+E[1].dataHBox[0]  -camPosX, E[1].y+E[1].dataHBox[3]-8-camPosY);
			SPR_setPosition(Rect2HB1_Q4, E[1].x+E[1].dataHBox[2]-8-camPosX, E[1].y+E[1].dataHBox[3]-8-camPosY);
		}
		
		}
		
		//Incr Frames, show FPS
		frames++;
		if(gPing2  == 1){ gPing2 = -1; } gPing2++;  //var 'gPing2'  (50%) variacao: 0 ; 1
		if(gPing4  == 3){ gPing4 = -1; } gPing4++;  //var 'gPing4'  (25%) variacao: 0 ; 1 ; 2 ; 3
		if(gPing10 == 9){ gPing10= -1; } gPing10++; //var 'gPing10' (10%) variacao: 0 ; 1 ; 2 ; 3 ; 4 ; 5 ; 6 ; 7 ; 8 ; 9
		if(show_debug==1){ VDP_showFPS(TRUE); }
		
        // draw screen
		SPR_update();

        // wait for screen refresh and do all SGDK VBlank tasks
        SYS_doVBlankProcess();
    }

    return 0;
}

//--- FUNCOES ---//
void FUNCAO_INPUT_SYSTEM(void)
{
	//joystick P1
	JOY1 = JOY_readJoypad(JOY_1);
	if(JOY1 & BUTTON_UP    ) { JOY1_UP    = TRUE; } else { JOY1_UP    = FALSE; }
	if(JOY1 & BUTTON_DOWN  ) { JOY1_DOWN  = TRUE; } else { JOY1_DOWN  = FALSE; }
	if(JOY1 & BUTTON_LEFT  ) { JOY1_LEFT  = TRUE; } else { JOY1_LEFT  = FALSE; }
	if(JOY1 & BUTTON_RIGHT ) { JOY1_RIGHT = TRUE; } else { JOY1_RIGHT = FALSE; }
	if(JOY1 & BUTTON_A     ) { JOY1_A     = TRUE; } else { JOY1_A     = FALSE; }
	if(JOY1 & BUTTON_B     ) { JOY1_B     = TRUE; } else { JOY1_B     = FALSE; }
	if(JOY1 & BUTTON_C     ) { JOY1_C     = TRUE; } else { JOY1_C     = FALSE; }
	if(JOY1 & BUTTON_X     ) { JOY1_X     = TRUE; } else { JOY1_X     = FALSE; }
	if(JOY1 & BUTTON_Y     ) { JOY1_Y     = TRUE; } else { JOY1_Y     = FALSE; }
	if(JOY1 & BUTTON_Z     ) { JOY1_Z     = TRUE; } else { JOY1_Z     = FALSE; }
	if(JOY1 & BUTTON_START ) { JOY1_START = TRUE; } else { JOY1_START = FALSE; }
	if(JOY1 & BUTTON_MODE  ) { JOY1_MODE  = TRUE; } else { JOY1_MODE  = FALSE; }
	
	//joystick P2
	JOY2 = JOY_readJoypad(JOY_2);
	if(JOY2 & BUTTON_UP    ) { JOY2_UP    = TRUE; } else { JOY2_UP    = FALSE; }
	if(JOY2 & BUTTON_DOWN  ) { JOY2_DOWN  = TRUE; } else { JOY2_DOWN  = FALSE; }
	if(JOY2 & BUTTON_LEFT  ) { JOY2_LEFT  = TRUE; } else { JOY2_LEFT  = FALSE; }
	if(JOY2 & BUTTON_RIGHT ) { JOY2_RIGHT = TRUE; } else { JOY2_RIGHT = FALSE; }
	if(JOY2 & BUTTON_A     ) { JOY2_A     = TRUE; } else { JOY2_A     = FALSE; }
	if(JOY2 & BUTTON_B     ) { JOY2_B     = TRUE; } else { JOY2_B     = FALSE; }
	if(JOY2 & BUTTON_C     ) { JOY2_C     = TRUE; } else { JOY2_C     = FALSE; }
	if(JOY2 & BUTTON_X     ) { JOY2_X     = TRUE; } else { JOY2_X     = FALSE; }
	if(JOY2 & BUTTON_Y     ) { JOY2_Y     = TRUE; } else { JOY2_Y     = FALSE; }
	if(JOY2 & BUTTON_Z     ) { JOY2_Z     = TRUE; } else { JOY2_Z     = FALSE; }
	if(JOY2 & BUTTON_START ) { JOY2_START = TRUE; } else { JOY2_START = FALSE; }
	if(JOY2 & BUTTON_MODE  ) { JOY2_MODE  = TRUE; } else { JOY2_MODE  = FALSE; }
	
	//---P1
	if(qtdePlayers>=1)
	{
	
		if (JOY1_UP) {
		if (key_JOY1_UP_pressed==1 && key_JOY1_UP_hold==0) { key_JOY1_UP_hold=1; key_JOY1_UP_pressed=0; }
		if (key_JOY1_UP_pressed==0 && key_JOY1_UP_hold==0) { key_JOY1_UP_pressed=1; }}
		else if (!JOY1_UP) {
		if (key_JOY1_UP_released==1) { key_JOY1_UP_released=0; key_JOY1_UP_pressed=0; key_JOY1_UP_hold=0; }
		if (key_JOY1_UP_pressed>0 || key_JOY1_UP_hold>0) {key_JOY1_UP_released=1;}}
		if (key_JOY1_UP_pressed==0 && key_JOY1_UP_hold==0 && key_JOY1_UP_released==0){ P[1].key_JOY_UP_status=0; }
		if (key_JOY1_UP_pressed  ==1 ) { P[1].key_JOY_UP_status=1; }
		if (key_JOY1_UP_hold     ==1 ) { P[1].key_JOY_UP_status=2; }
		if (key_JOY1_UP_released ==1 ) { P[1].key_JOY_UP_status=3; }
		
		if (JOY1_DOWN) {
		if (key_JOY1_DOWN_pressed==1 && key_JOY1_DOWN_hold==0) { key_JOY1_DOWN_hold=1; key_JOY1_DOWN_pressed=0; }
		if (key_JOY1_DOWN_pressed==0 && key_JOY1_DOWN_hold==0) { key_JOY1_DOWN_pressed=1; }}
		else if (!JOY1_DOWN) {
		if (key_JOY1_DOWN_released==1) { key_JOY1_DOWN_released=0; key_JOY1_DOWN_pressed=0; key_JOY1_DOWN_hold=0; }
		if (key_JOY1_DOWN_pressed>0 || key_JOY1_DOWN_hold>0) {key_JOY1_DOWN_released=1;}}
		if (key_JOY1_DOWN_pressed==0 && key_JOY1_DOWN_hold==0 && key_JOY1_DOWN_released==0){ P[1].key_JOY_DOWN_status=0; }
		if (key_JOY1_DOWN_pressed  ==1 ) { P[1].key_JOY_DOWN_status=1; }
		if (key_JOY1_DOWN_hold     ==1 ) { P[1].key_JOY_DOWN_status=2; }
		if (key_JOY1_DOWN_released ==1 ) { P[1].key_JOY_DOWN_status=3; }
		
		if (JOY1_LEFT) {
		if (key_JOY1_LEFT_pressed==1 && key_JOY1_LEFT_hold==0) { key_JOY1_LEFT_hold=1; key_JOY1_LEFT_pressed=0; }
		if (key_JOY1_LEFT_pressed==0 && key_JOY1_LEFT_hold==0) { key_JOY1_LEFT_pressed=1; }}
		else if (!JOY1_LEFT) {
		if (key_JOY1_LEFT_released==1) { key_JOY1_LEFT_released=0; key_JOY1_LEFT_pressed=0; key_JOY1_LEFT_hold=0; }
		if (key_JOY1_LEFT_pressed>0 || key_JOY1_LEFT_hold>0) {key_JOY1_LEFT_released=1;}}
		if (key_JOY1_LEFT_pressed==0 && key_JOY1_LEFT_hold==0 && key_JOY1_LEFT_released==0){ P[1].key_JOY_LEFT_status=0; }
		if (key_JOY1_LEFT_pressed  ==1 ) { P[1].key_JOY_LEFT_status=1; }
		if (key_JOY1_LEFT_hold     ==1 ) { P[1].key_JOY_LEFT_status=2; }
		if (key_JOY1_LEFT_released ==1 ) { P[1].key_JOY_LEFT_status=3; }
		
		if (JOY1_RIGHT) {
		if (key_JOY1_RIGHT_pressed==1 && key_JOY1_RIGHT_hold==0) { key_JOY1_RIGHT_hold=1; key_JOY1_RIGHT_pressed=0; }
		if (key_JOY1_RIGHT_pressed==0 && key_JOY1_RIGHT_hold==0) { key_JOY1_RIGHT_pressed=1; }}
		else if (!JOY1_RIGHT) {
		if (key_JOY1_RIGHT_released==1) { key_JOY1_RIGHT_released=0; key_JOY1_RIGHT_pressed=0; key_JOY1_RIGHT_hold=0; }
		if (key_JOY1_RIGHT_pressed>0 || key_JOY1_RIGHT_hold>0) {key_JOY1_RIGHT_released=1;}}
		if (key_JOY1_RIGHT_pressed==0 && key_JOY1_RIGHT_hold==0 && key_JOY1_RIGHT_released==0){ P[1].key_JOY_RIGHT_status=0; }
		if (key_JOY1_RIGHT_pressed  ==1 ) { P[1].key_JOY_RIGHT_status=1; }
		if (key_JOY1_RIGHT_hold     ==1 ) { P[1].key_JOY_RIGHT_status=2; }
		if (key_JOY1_RIGHT_released ==1 ) { P[1].key_JOY_RIGHT_status=3; }
		
		if (JOY1_A) {
		if (key_JOY1_A_pressed==1 && key_JOY1_A_hold==0) { key_JOY1_A_hold=1; key_JOY1_A_pressed=0; }
		if (key_JOY1_A_pressed==0 && key_JOY1_A_hold==0) { key_JOY1_A_pressed=1; }}
		else if (!JOY1_A) {
		if (key_JOY1_A_released==1) { key_JOY1_A_released=0; key_JOY1_A_pressed=0; key_JOY1_A_hold=0; }
		if (key_JOY1_A_pressed>0 || key_JOY1_A_hold>0) {key_JOY1_A_released=1;}}
		if (key_JOY1_A_pressed==0 && key_JOY1_A_hold==0 && key_JOY1_A_released==0){ P[1].key_JOY_A_status=0; }
		if (key_JOY1_A_pressed  ==1 ) { P[1].key_JOY_A_status=1; }
		if (key_JOY1_A_hold     ==1 ) { P[1].key_JOY_A_status=2; }
		if (key_JOY1_A_released ==1 ) { P[1].key_JOY_A_status=3; }
		
		if (JOY1_B) {
		if (key_JOY1_B_pressed==1 && key_JOY1_B_hold==0) { key_JOY1_B_hold=1; key_JOY1_B_pressed=0; }
		if (key_JOY1_B_pressed==0 && key_JOY1_B_hold==0) { key_JOY1_B_pressed=1; }}
		else if (!JOY1_B) {
		if (key_JOY1_B_released==1) { key_JOY1_B_released=0; key_JOY1_B_pressed=0; key_JOY1_B_hold=0; }
		if (key_JOY1_B_pressed>0 || key_JOY1_B_hold>0) {key_JOY1_B_released=1;}}
		if (key_JOY1_B_pressed==0 && key_JOY1_B_hold==0 && key_JOY1_B_released==0){ P[1].key_JOY_B_status=0; }
		if (key_JOY1_B_pressed  ==1 ) { P[1].key_JOY_B_status=1; }
		if (key_JOY1_B_hold     ==1 ) { P[1].key_JOY_B_status=2; }
		if (key_JOY1_B_released ==1 ) { P[1].key_JOY_B_status=3; }
		
		if (JOY1_C) {
		if (key_JOY1_C_pressed==1 && key_JOY1_C_hold==0) { key_JOY1_C_hold=1; key_JOY1_C_pressed=0; }
		if (key_JOY1_C_pressed==0 && key_JOY1_C_hold==0) { key_JOY1_C_pressed=1; }}
		else if (!JOY1_C) {
		if (key_JOY1_C_released==1) { key_JOY1_C_released=0; key_JOY1_C_pressed=0; key_JOY1_C_hold=0; }
		if (key_JOY1_C_pressed>0 || key_JOY1_C_hold>0) {key_JOY1_C_released=1;}}
		if (key_JOY1_C_pressed==0 && key_JOY1_C_hold==0 && key_JOY1_C_released==0){ P[1].key_JOY_C_status=0; }
		if (key_JOY1_C_pressed  ==1 ) { P[1].key_JOY_C_status=1; }
		if (key_JOY1_C_hold     ==1 ) { P[1].key_JOY_C_status=2; }
		if (key_JOY1_C_released ==1 ) { P[1].key_JOY_C_status=3; }
		
		if (JOY1_X) {
		if (key_JOY1_X_pressed==1 && key_JOY1_X_hold==0) { key_JOY1_X_hold=1; key_JOY1_X_pressed=0; }
		if (key_JOY1_X_pressed==0 && key_JOY1_X_hold==0) { key_JOY1_X_pressed=1; }}
		else if (!JOY1_X) {
		if (key_JOY1_X_released==1) { key_JOY1_X_released=0; key_JOY1_X_pressed=0; key_JOY1_X_hold=0; }
		if (key_JOY1_X_pressed>0 || key_JOY1_X_hold>0) {key_JOY1_X_released=1;}}
		if (key_JOY1_X_pressed==0 && key_JOY1_X_hold==0 && key_JOY1_X_released==0){ P[1].key_JOY_X_status=0; }
		if (key_JOY1_X_pressed  ==1 ) { P[1].key_JOY_X_status=1; }
		if (key_JOY1_X_hold     ==1 ) { P[1].key_JOY_X_status=2; }
		if (key_JOY1_X_released ==1 ) { P[1].key_JOY_X_status=3; }
		
		if (JOY1_Y) {
		if (key_JOY1_Y_pressed==1 && key_JOY1_Y_hold==0) { key_JOY1_Y_hold=1; key_JOY1_Y_pressed=0; }
		if (key_JOY1_Y_pressed==0 && key_JOY1_Y_hold==0) { key_JOY1_Y_pressed=1; }}
		else if (!JOY1_Y) {
		if (key_JOY1_Y_released==1) { key_JOY1_Y_released=0; key_JOY1_Y_pressed=0; key_JOY1_Y_hold=0; }
		if (key_JOY1_Y_pressed>0 || key_JOY1_Y_hold>0) {key_JOY1_Y_released=1;}}
		if (key_JOY1_Y_pressed==0 && key_JOY1_Y_hold==0 && key_JOY1_Y_released==0){ P[1].key_JOY_Y_status=0; }
		if (key_JOY1_Y_pressed  ==1 ) { P[1].key_JOY_Y_status=1; }
		if (key_JOY1_Y_hold     ==1 ) { P[1].key_JOY_Y_status=2; }
		if (key_JOY1_Y_released ==1 ) { P[1].key_JOY_Y_status=3; }
		
		if (JOY1_Z) {
		if (key_JOY1_Z_pressed==1 && key_JOY1_Z_hold==0) { key_JOY1_Z_hold=1; key_JOY1_Z_pressed=0; }
		if (key_JOY1_Z_pressed==0 && key_JOY1_Z_hold==0) { key_JOY1_Z_pressed=1; }}
		else if (!JOY1_Z) {
		if (key_JOY1_Z_released==1) { key_JOY1_Z_released=0; key_JOY1_Z_pressed=0; key_JOY1_Z_hold=0; }
		if (key_JOY1_Z_pressed>0 || key_JOY1_Z_hold>0) {key_JOY1_Z_released=1;}}
		if (key_JOY1_Z_pressed==0 && key_JOY1_Z_hold==0 && key_JOY1_Z_released==0){ P[1].key_JOY_Z_status=0; }
		if (key_JOY1_Z_pressed  ==1 ) { P[1].key_JOY_Z_status=1; }
		if (key_JOY1_Z_hold     ==1 ) { P[1].key_JOY_Z_status=2; }
		if (key_JOY1_Z_released ==1 ) { P[1].key_JOY_Z_status=3; }
		
		if (JOY1_START) {
		if (key_JOY1_START_pressed==1 && key_JOY1_START_hold==0) { key_JOY1_START_hold=1; key_JOY1_START_pressed=0; }
		if (key_JOY1_START_pressed==0 && key_JOY1_START_hold==0) { key_JOY1_START_pressed=1; }}
		else if (!JOY1_START) {
		if (key_JOY1_START_released==1) { key_JOY1_START_released=0; key_JOY1_START_pressed=0; key_JOY1_START_hold=0; }
		if (key_JOY1_START_pressed>0 || key_JOY1_START_hold>0) {key_JOY1_START_released=1;}}
		if (key_JOY1_START_pressed==0 && key_JOY1_START_hold==0 && key_JOY1_START_released==0){ P[1].key_JOY_START_status=0; }
		if (key_JOY1_START_pressed  ==1 ) { P[1].key_JOY_START_status=1; }
		if (key_JOY1_START_hold     ==1 ) { P[1].key_JOY_START_status=2; }
		if (key_JOY1_START_released ==1 ) { P[1].key_JOY_START_status=3; }
		
		if (JOY1_MODE) {
		if (key_JOY1_MODE_pressed==1 && key_JOY1_MODE_hold==0) { key_JOY1_MODE_hold=1; key_JOY1_MODE_pressed=0; }
		if (key_JOY1_MODE_pressed==0 && key_JOY1_MODE_hold==0) { key_JOY1_MODE_pressed=1; }}
		else if (!JOY1_MODE) {
		if (key_JOY1_MODE_released==1) { key_JOY1_MODE_released=0; key_JOY1_MODE_pressed=0; key_JOY1_MODE_hold=0; }
		if (key_JOY1_MODE_pressed>0 || key_JOY1_MODE_hold>0) {key_JOY1_MODE_released=1;}}
		if (key_JOY1_MODE_pressed==0 && key_JOY1_MODE_hold==0 && key_JOY1_MODE_released==0){ P[1].key_JOY_MODE_status=0; }
		if (key_JOY1_MODE_pressed  ==1 ) { P[1].key_JOY_MODE_status=1; }
		if (key_JOY1_MODE_hold     ==1 ) { P[1].key_JOY_MODE_status=2; }
		if (key_JOY1_MODE_released ==1 ) { P[1].key_JOY_MODE_status=3; }
		
	}
	
	//---P2
	if(qtdePlayers>=2)
	{
	
		if (JOY2_UP) {
		if (key_JOY2_UP_pressed==1 && key_JOY2_UP_hold==0) { key_JOY2_UP_hold=1; key_JOY2_UP_pressed=0; }
		if (key_JOY2_UP_pressed==0 && key_JOY2_UP_hold==0) { key_JOY2_UP_pressed=1; }}
		else if (!JOY2_UP) {
		if (key_JOY2_UP_released==1) { key_JOY2_UP_released=0; key_JOY2_UP_pressed=0; key_JOY2_UP_hold=0; }
		if (key_JOY2_UP_pressed>0 || key_JOY2_UP_hold>0) {key_JOY2_UP_released=1;}}
		if (key_JOY2_UP_pressed==0 && key_JOY2_UP_hold==0 && key_JOY2_UP_released==0){ P[2].key_JOY_UP_status=0; }
		if (key_JOY2_UP_pressed  ==1 ) { P[2].key_JOY_UP_status=1; }
		if (key_JOY2_UP_hold     ==1 ) { P[2].key_JOY_UP_status=2; }
		if (key_JOY2_UP_released ==1 ) { P[2].key_JOY_UP_status=3; }
		
		if (JOY2_DOWN) {
		if (key_JOY2_DOWN_pressed==1 && key_JOY2_DOWN_hold==0) { key_JOY2_DOWN_hold=1; key_JOY2_DOWN_pressed=0; }
		if (key_JOY2_DOWN_pressed==0 && key_JOY2_DOWN_hold==0) { key_JOY2_DOWN_pressed=1; }}
		else if (!JOY2_DOWN) {
		if (key_JOY2_DOWN_released==1) { key_JOY2_DOWN_released=0; key_JOY2_DOWN_pressed=0; key_JOY2_DOWN_hold=0; }
		if (key_JOY2_DOWN_pressed>0 || key_JOY2_DOWN_hold>0) {key_JOY2_DOWN_released=1;}}
		if (key_JOY2_DOWN_pressed==0 && key_JOY2_DOWN_hold==0 && key_JOY2_DOWN_released==0){ P[2].key_JOY_DOWN_status=0; }
		if (key_JOY2_DOWN_pressed  ==1 ) { P[2].key_JOY_DOWN_status=1; }
		if (key_JOY2_DOWN_hold     ==1 ) { P[2].key_JOY_DOWN_status=2; }
		if (key_JOY2_DOWN_released ==1 ) { P[2].key_JOY_DOWN_status=3; }
		
		if (JOY2_LEFT) {
		if (key_JOY2_LEFT_pressed==1 && key_JOY2_LEFT_hold==0) { key_JOY2_LEFT_hold=1; key_JOY2_LEFT_pressed=0; }
		if (key_JOY2_LEFT_pressed==0 && key_JOY2_LEFT_hold==0) { key_JOY2_LEFT_pressed=1; }}
		else if (!JOY2_LEFT) {
		if (key_JOY2_LEFT_released==1) { key_JOY2_LEFT_released=0; key_JOY2_LEFT_pressed=0; key_JOY2_LEFT_hold=0; }
		if (key_JOY2_LEFT_pressed>0 || key_JOY2_LEFT_hold>0) {key_JOY2_LEFT_released=1;}}
		if (key_JOY2_LEFT_pressed==0 && key_JOY2_LEFT_hold==0 && key_JOY2_LEFT_released==0){ P[2].key_JOY_LEFT_status=0; }
		if (key_JOY2_LEFT_pressed  ==1 ) { P[2].key_JOY_LEFT_status=1; }
		if (key_JOY2_LEFT_hold     ==1 ) { P[2].key_JOY_LEFT_status=2; }
		if (key_JOY2_LEFT_released ==1 ) { P[2].key_JOY_LEFT_status=3; }
		
		if (JOY2_RIGHT) {
		if (key_JOY2_RIGHT_pressed==1 && key_JOY2_RIGHT_hold==0) { key_JOY2_RIGHT_hold=1; key_JOY2_RIGHT_pressed=0; }
		if (key_JOY2_RIGHT_pressed==0 && key_JOY2_RIGHT_hold==0) { key_JOY2_RIGHT_pressed=1; }}
		else if (!JOY2_RIGHT) {
		if (key_JOY2_RIGHT_released==1) { key_JOY2_RIGHT_released=0; key_JOY2_RIGHT_pressed=0; key_JOY2_RIGHT_hold=0; }
		if (key_JOY2_RIGHT_pressed>0 || key_JOY2_RIGHT_hold>0) {key_JOY2_RIGHT_released=1;}}
		if (key_JOY2_RIGHT_pressed==0 && key_JOY2_RIGHT_hold==0 && key_JOY2_RIGHT_released==0){ P[2].key_JOY_RIGHT_status=0; }
		if (key_JOY2_RIGHT_pressed  ==1 ) { P[2].key_JOY_RIGHT_status=1; }
		if (key_JOY2_RIGHT_hold     ==1 ) { P[2].key_JOY_RIGHT_status=2; }
		if (key_JOY2_RIGHT_released ==1 ) { P[2].key_JOY_RIGHT_status=3; }
		
		if (JOY2_A) {
		if (key_JOY2_A_pressed==1 && key_JOY2_A_hold==0) { key_JOY2_A_hold=1; key_JOY2_A_pressed=0; }
		if (key_JOY2_A_pressed==0 && key_JOY2_A_hold==0) { key_JOY2_A_pressed=1; }}
		else if (!JOY2_A) {
		if (key_JOY2_A_released==1) { key_JOY2_A_released=0; key_JOY2_A_pressed=0; key_JOY2_A_hold=0; }
		if (key_JOY2_A_pressed>0 || key_JOY2_A_hold>0) {key_JOY2_A_released=1;}}
		if (key_JOY2_A_pressed==0 && key_JOY2_A_hold==0 && key_JOY2_A_released==0){ P[2].key_JOY_A_status=0; }
		if (key_JOY2_A_pressed  ==1 ) { P[2].key_JOY_A_status=1; }
		if (key_JOY2_A_hold     ==1 ) { P[2].key_JOY_A_status=2; }
		if (key_JOY2_A_released ==1 ) { P[2].key_JOY_A_status=3; }
		
		if (JOY2_B) {
		if (key_JOY2_B_pressed==1 && key_JOY2_B_hold==0) { key_JOY2_B_hold=1; key_JOY2_B_pressed=0; }
		if (key_JOY2_B_pressed==0 && key_JOY2_B_hold==0) { key_JOY2_B_pressed=1; }}
		else if (!JOY2_B) {
		if (key_JOY2_B_released==1) { key_JOY2_B_released=0; key_JOY2_B_pressed=0; key_JOY2_B_hold=0; }
		if (key_JOY2_B_pressed>0 || key_JOY2_B_hold>0) {key_JOY2_B_released=1;}}
		if (key_JOY2_B_pressed==0 && key_JOY2_B_hold==0 && key_JOY2_B_released==0){ P[2].key_JOY_B_status=0; }
		if (key_JOY2_B_pressed  ==1 ) { P[2].key_JOY_B_status=1; }
		if (key_JOY2_B_hold     ==1 ) { P[2].key_JOY_B_status=2; }
		if (key_JOY2_B_released ==1 ) { P[2].key_JOY_B_status=3; }
		
		if (JOY2_C) {
		if (key_JOY2_C_pressed==1 && key_JOY2_C_hold==0) { key_JOY2_C_hold=1; key_JOY2_C_pressed=0; }
		if (key_JOY2_C_pressed==0 && key_JOY2_C_hold==0) { key_JOY2_C_pressed=1; }}
		else if (!JOY2_C) {
		if (key_JOY2_C_released==1) { key_JOY2_C_released=0; key_JOY2_C_pressed=0; key_JOY2_C_hold=0; }
		if (key_JOY2_C_pressed>0 || key_JOY2_C_hold>0) {key_JOY2_C_released=1;}}
		if (key_JOY2_C_pressed==0 && key_JOY2_C_hold==0 && key_JOY2_C_released==0){ P[2].key_JOY_C_status=0; }
		if (key_JOY2_C_pressed  ==1 ) { P[2].key_JOY_C_status=1; }
		if (key_JOY2_C_hold     ==1 ) { P[2].key_JOY_C_status=2; }
		if (key_JOY2_C_released ==1 ) { P[2].key_JOY_C_status=3; }
		
		if (JOY2_X) {
		if (key_JOY2_X_pressed==1 && key_JOY2_X_hold==0) { key_JOY2_X_hold=1; key_JOY2_X_pressed=0; }
		if (key_JOY2_X_pressed==0 && key_JOY2_X_hold==0) { key_JOY2_X_pressed=1; }}
		else if (!JOY2_X) {
		if (key_JOY2_X_released==1) { key_JOY2_X_released=0; key_JOY2_X_pressed=0; key_JOY2_X_hold=0; }
		if (key_JOY2_X_pressed>0 || key_JOY2_X_hold>0) {key_JOY2_X_released=1;}}
		if (key_JOY2_X_pressed==0 && key_JOY2_X_hold==0 && key_JOY2_X_released==0){ P[2].key_JOY_X_status=0; }
		if (key_JOY2_X_pressed  ==1 ) { P[2].key_JOY_X_status=1; }
		if (key_JOY2_X_hold     ==1 ) { P[2].key_JOY_X_status=2; }
		if (key_JOY2_X_released ==1 ) { P[2].key_JOY_X_status=3; }
		
		if (JOY2_Y) {
		if (key_JOY2_Y_pressed==1 && key_JOY2_Y_hold==0) { key_JOY2_Y_hold=1; key_JOY2_Y_pressed=0; }
		if (key_JOY2_Y_pressed==0 && key_JOY2_Y_hold==0) { key_JOY2_Y_pressed=1; }}
		else if (!JOY2_Y) {
		if (key_JOY2_Y_released==1) { key_JOY2_Y_released=0; key_JOY2_Y_pressed=0; key_JOY2_Y_hold=0; }
		if (key_JOY2_Y_pressed>0 || key_JOY2_Y_hold>0) {key_JOY2_Y_released=1;}}
		if (key_JOY2_Y_pressed==0 && key_JOY2_Y_hold==0 && key_JOY2_Y_released==0){ P[2].key_JOY_Y_status=0; }
		if (key_JOY2_Y_pressed  ==1 ) { P[2].key_JOY_Y_status=1; }
		if (key_JOY2_Y_hold     ==1 ) { P[2].key_JOY_Y_status=2; }
		if (key_JOY2_Y_released ==1 ) { P[2].key_JOY_Y_status=3; }
		
		if (JOY2_Z) {
		if (key_JOY2_Z_pressed==1 && key_JOY2_Z_hold==0) { key_JOY2_Z_hold=1; key_JOY2_Z_pressed=0; }
		if (key_JOY2_Z_pressed==0 && key_JOY2_Z_hold==0) { key_JOY2_Z_pressed=1; }}
		else if (!JOY2_Z) {
		if (key_JOY2_Z_released==1) { key_JOY2_Z_released=0; key_JOY2_Z_pressed=0; key_JOY2_Z_hold=0; }
		if (key_JOY2_Z_pressed>0 || key_JOY2_Z_hold>0) {key_JOY2_Z_released=1;}}
		if (key_JOY2_Z_pressed==0 && key_JOY2_Z_hold==0 && key_JOY2_Z_released==0){ P[2].key_JOY_Z_status=0; }
		if (key_JOY2_Z_pressed  ==1 ) { P[2].key_JOY_Z_status=1; }
		if (key_JOY2_Z_hold     ==1 ) { P[2].key_JOY_Z_status=2; }
		if (key_JOY2_Z_released ==1 ) { P[2].key_JOY_Z_status=3; }
		
		if (JOY2_START) {
		if (key_JOY2_START_pressed==1 && key_JOY2_START_hold==0) { key_JOY2_START_hold=1; key_JOY2_START_pressed=0; }
		if (key_JOY2_START_pressed==0 && key_JOY2_START_hold==0) { key_JOY2_START_pressed=1; }}
		else if (!JOY2_START) {
		if (key_JOY2_START_released==1) { key_JOY2_START_released=0; key_JOY2_START_pressed=0; key_JOY2_START_hold=0; }
		if (key_JOY2_START_pressed>0 || key_JOY2_START_hold>0) {key_JOY2_START_released=1;}}
		if (key_JOY2_START_pressed==0 && key_JOY2_START_hold==0 && key_JOY2_START_released==0){ P[2].key_JOY_START_status=0; }
		if (key_JOY2_START_pressed  ==1 ) { P[2].key_JOY_START_status=1; }
		if (key_JOY2_START_hold     ==1 ) { P[2].key_JOY_START_status=2; }
		if (key_JOY2_START_released ==1 ) { P[2].key_JOY_START_status=3; }
		
		if (JOY2_MODE) {
		if (key_JOY2_MODE_pressed==1 && key_JOY2_MODE_hold==0) { key_JOY2_MODE_hold=1; key_JOY2_MODE_pressed=0; }
		if (key_JOY2_MODE_pressed==0 && key_JOY2_MODE_hold==0) { key_JOY2_MODE_pressed=1; }}
		else if (!JOY2_MODE) {
		if (key_JOY2_MODE_released==1) { key_JOY2_MODE_released=0; key_JOY2_MODE_pressed=0; key_JOY2_MODE_hold=0; }
		if (key_JOY2_MODE_pressed>0 || key_JOY2_MODE_hold>0) {key_JOY2_MODE_released=1;}}
		if (key_JOY2_MODE_pressed==0 && key_JOY2_MODE_hold==0 && key_JOY2_MODE_released==0){ P[2].key_JOY_MODE_status=0; }
		if (key_JOY2_MODE_pressed  ==1 ) { P[2].key_JOY_MODE_status=1; }
		if (key_JOY2_MODE_hold     ==1 ) { P[2].key_JOY_MODE_status=2; }
		if (key_JOY2_MODE_released ==1 ) { P[2].key_JOY_MODE_status=3; }
		
	}
	
}

void PLAYER_STATE(const u8 Player, u16 State)
{
	//padroes
	if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; }
	P[Player].animFrame      = 1;
	P[Player].frameTimeAtual = 1;
	P[Player].dataAnim[1]    = 1;
	P[Player].animFrameTotal = 1;
	P[Player].state = State;
	
	FUNCAO_PLAY_SND(P[Player].id, State, Player); //SOUND; SFX!
	
	if(P[Player].id==1)//MUMMY
	{
		if( State==100){
			P[Player].w = 80;
			P[Player].h = 88;
			P[Player].axisX = 21;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 240;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_100, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_100, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			P[Player].w = 112;
			P[Player].h = 88;
			P[Player].axisX = 23;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 10;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_101, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_101, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102){
			P[Player].w = 112;
			P[Player].h = 96;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 16;//2
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_102, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_102, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103){
			P[Player].w = 128;
			P[Player].h = 80;
			P[Player].axisX = 41;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 16;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_103, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_103, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104){
			P[Player].w = 112;
			P[Player].h = 104;
			P[Player].axisX = 29;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_104, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_104, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==105){
			P[Player].w = 112;
			P[Player].h = 104;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 8;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_105, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_105, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 128;
			P[Player].h = 88;
			P[Player].axisX = 66;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 1;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 1;
			P[Player].dataAnim[12] = 1;
			P[Player].dataAnim[13] = 1;
			P[Player].dataAnim[14] = 1;
			P[Player].dataAnim[15] = 1;
			P[Player].dataAnim[16] = 1;
			P[Player].dataAnim[17] = 1;
			P[Player].dataAnim[18] = 1;
			P[Player].dataAnim[19] = 1;
			P[Player].dataAnim[20] = 1;
			P[Player].dataAnim[21] = 1;
			P[Player].dataAnim[22] = 1;
			P[Player].dataAnim[23] = 1;
			P[Player].dataAnim[24] = 1;
			P[Player].dataAnim[25] = 1;
			P[Player].dataAnim[26] = 1;
			P[Player].dataAnim[27] = 1;
			P[Player].dataAnim[28] = 1;
			P[Player].dataAnim[29] = 1;
			P[Player].dataAnim[30] = 1;
			P[Player].dataAnim[31] = 1;
			P[Player].dataAnim[32] = 1;
			P[Player].dataAnim[33] = 1;
			P[Player].dataAnim[34] = 1;
			P[Player].dataAnim[35] = 1;
			P[Player].dataAnim[36] = 1;
			P[Player].dataAnim[37] = 1;
			P[Player].dataAnim[38] = 1;
			P[Player].dataAnim[39] = 1;
			P[Player].dataAnim[40] = 1;
			P[Player].dataAnim[41] = 1;
			P[Player].dataAnim[42] = 1;
			P[Player].dataAnim[43] = 1;
			P[Player].dataAnim[44] = 1;
			P[Player].dataAnim[45] = 1;
			P[Player].dataAnim[46] = 1;
			P[Player].dataAnim[47] = 1;
			P[Player].dataAnim[48] = 1;
			P[Player].dataAnim[49] = 1;
			P[Player].dataAnim[50] = 1;
			P[Player].dataAnim[51] = 1;
			P[Player].dataAnim[52] = 1;
			P[Player].dataAnim[53] = 1;
			P[Player].dataAnim[54] = 1;
			P[Player].dataAnim[55] = 1;
			P[Player].dataAnim[56] = 1;
			P[Player].dataAnim[57] = 1;
			P[Player].dataAnim[58] = 1;
			P[Player].dataAnim[59] = 1;
			P[Player].dataAnim[60] = 1;
			P[Player].animFrameTotal = 60;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_199, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_199, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==202){
			P[Player].w = 112;
			P[Player].h = 96;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 12;
			P[Player].dataAnim[6]  = 16;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_202, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_202, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==203){
			P[Player].w = 112;
			P[Player].h = 96;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 30;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_203, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_203, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==204){
			P[Player].w = 112;
			P[Player].h = 104;
			P[Player].axisX = 29;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 12;
			P[Player].animFrameTotal = 9;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_204, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_204, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==205){
			P[Player].w = 112;
			P[Player].h = 104;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 12;
			P[Player].dataAnim[6]  = 6;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_205, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_205, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 104;
			P[Player].axisX = 16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 15;
			P[Player].dataAnim[2]  = 99;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_300, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_300, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==301){
			P[Player].w = 112;
			P[Player].h = 80;
			P[Player].axisX = 56;
			P[Player].axisY = P[Player].h-5+16;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 25;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_301, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_301, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 88;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 15;
			P[Player].dataAnim[2]  = 99;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_320, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_320, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==321){
			P[Player].w = 96;
			P[Player].h = 112;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;//2
			P[Player].dataAnim[3]  = 1;//2
			P[Player].dataAnim[4]  = 1;//2
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_321, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_321, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==322){
			P[Player].w = 152;
			P[Player].h = 88;
			P[Player].axisX = 56;
			P[Player].axisY = P[Player].h-5+8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 99;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_322, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_322, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==420){
			P[Player].w = 80;
			P[Player].h = 88;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].animFrameTotal = 12;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_420, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_420, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==430){
			P[Player].w = 88;
			P[Player].h = 88;
			P[Player].axisX = 34;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_430, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_430, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==431){
			P[Player].w = 136;
			P[Player].h = 88;
			P[Player].axisX = 39;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 30;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_431, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_431, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 80;
			P[Player].h = 88;
			P[Player].axisX = 42;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_501, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_501, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 88;
			P[Player].h = 80;
			P[Player].axisX = 20;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_502, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_502, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 20;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_503, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_503, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 64;
			P[Player].h = 80;
			P[Player].axisX = 28;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_505, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_505, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 21;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 64;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_508, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_508, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_mum_510, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_mum_513, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_510, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_mum_513, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 32;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_mum_511, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_mum_514, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_511, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_mum_514, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 104;
			P[Player].h = 96;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 250;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_550, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_550, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 128;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_551, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_551, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 96;
			P[Player].h = 80;
			P[Player].axisX = P[Player].w/2+8;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_552, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_552, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 112;
			P[Player].h = 32;
			P[Player].axisX = P[Player].w/2-24;
			P[Player].axisY = P[Player].h-5-8;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_570, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_570, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 56;
			P[Player].h = 64;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_mum_606, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_606, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==2)//CAPTAIN COMMANDO
	{
		if( State==100){
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 21;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 240;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_100, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_100, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			P[Player].w = 96;
			P[Player].h = 80;
			P[Player].axisX = 22;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 15;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_101, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_101, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_102, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_102, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103){
			P[Player].w = 88;
			P[Player].h = 96;
			P[Player].axisX = 28;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_103, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_103, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104){
			P[Player].w = 88;
			P[Player].h = 88;
			P[Player].axisX = 31;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 12;//4
			P[Player].dataAnim[10] = 2;
			P[Player].dataAnim[11] = 2;
			P[Player].dataAnim[12] = 2;
			P[Player].animFrameTotal = 12;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_104, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_104, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==105){
			P[Player].w = 120;
			P[Player].h = 80;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_105, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_105, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 192;
			P[Player].h = 72;
			P[Player].axisX = 96;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 1;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 1;
			P[Player].dataAnim[12] = 1;
			P[Player].dataAnim[13] = 1;
			P[Player].dataAnim[14] = 1;
			P[Player].dataAnim[15] = 1;
			P[Player].dataAnim[16] = 1;
			P[Player].dataAnim[17] = 1;
			P[Player].dataAnim[18] = 1;
			P[Player].dataAnim[19] = 1;
			P[Player].dataAnim[20] = 1;
			P[Player].dataAnim[21] = 1;
			P[Player].dataAnim[22] = 1;
			P[Player].dataAnim[23] = 1;
			P[Player].dataAnim[24] = 1;
			P[Player].dataAnim[25] = 1;
			P[Player].dataAnim[26] = 1;
			P[Player].dataAnim[27] = 1;
			P[Player].dataAnim[28] = 1;
			P[Player].dataAnim[29] = 1;
			P[Player].dataAnim[30] = 1;
			P[Player].dataAnim[31] = 1;
			P[Player].dataAnim[32] = 1;
			P[Player].dataAnim[33] = 1;
			P[Player].dataAnim[34] = 1;
			P[Player].dataAnim[35] = 1;
			P[Player].dataAnim[36] = 1;
			P[Player].dataAnim[37] = 1;
			P[Player].dataAnim[38] = 1;
			P[Player].dataAnim[39] = 1;
			P[Player].dataAnim[40] = 1;
			P[Player].dataAnim[41] = 1;
			P[Player].dataAnim[42] = 1;
			P[Player].dataAnim[43] = 1;
			P[Player].dataAnim[44] = 1;
			P[Player].dataAnim[45] = 1;
			P[Player].dataAnim[46] = 1;
			P[Player].dataAnim[47] = 1;
			P[Player].dataAnim[48] = 1;
			P[Player].dataAnim[49] = 1;
			P[Player].dataAnim[50] = 1;
			P[Player].animFrameTotal = 50;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_199, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_199, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==202){
			P[Player].w = 104;
			P[Player].h = 80;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 12;
			P[Player].dataAnim[7]  = 16;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_202, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_202, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==203){
			P[Player].w = 80;
			P[Player].h = 96;
			P[Player].axisX = 31;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 12;
			P[Player].dataAnim[6]  = 16;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_203, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_203, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==204){
			P[Player].w = 80;
			P[Player].h = 96;
			P[Player].axisX = 30;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 12;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 1;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 12;//4
			P[Player].dataAnim[12] = 1;
			P[Player].dataAnim[13] = 1;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_204, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_204, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==205){
			P[Player].w = 120;
			P[Player].h = 80;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 12;
			P[Player].dataAnim[7]  = 1;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_205, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_205, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_300, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_300, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==301){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5+8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 25;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_301, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_301, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 64;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 15;
			P[Player].dataAnim[2]  = 99;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_320, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_320, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==321){
			P[Player].w = 96;
			P[Player].h = 72;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;//2
			P[Player].dataAnim[3]  = 25;//2
			P[Player].dataAnim[4]  = 2;//2
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_321, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_321, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==322){
			P[Player].w = 176;
			P[Player].h = 88;
			P[Player].axisX = 56;
			P[Player].axisY = P[Player].h-5+8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 1;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 1;
			P[Player].dataAnim[12] = 1;
			P[Player].dataAnim[13] = 1;
			P[Player].dataAnim[14] = 1;
			P[Player].dataAnim[15] = 1;
			P[Player].dataAnim[16] = 1;
			P[Player].dataAnim[17] = 1;
			P[Player].dataAnim[18] = 1;
			P[Player].dataAnim[19] = 1;
			P[Player].dataAnim[20] = 99;
			P[Player].animFrameTotal = 20;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_322, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_322, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==420){
			P[Player].w = 64;
			P[Player].h = 80;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].animFrameTotal = 12;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_420, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_420, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==430){
			P[Player].w = 80;
			P[Player].h = 80;
			P[Player].axisX = 38;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_430, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_430, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==431){
			P[Player].w = 120;
			P[Player].h = 72;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 32;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_431, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_431, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 38;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_501, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_501, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 64;
			P[Player].h = 72;
			P[Player].axisX = 28;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_502, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_502, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 20;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_503, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_503, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 64;
			P[Player].h = 80;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_505, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_505, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 19;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 64;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_508, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_508, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_cap_510, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_cap_513, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_510, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_cap_513, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 32;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_cap_511, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_cap_514, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_511, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_cap_514, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 96;
			P[Player].h = 80;
			P[Player].axisX = P[Player].w/2-16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 250;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_550, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_550, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 80;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-8;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_551, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_551, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 80;
			P[Player].h = 72;
			P[Player].axisX = P[Player].w/2+16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_552, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_552, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 96;
			P[Player].h = 32;
			P[Player].axisX = P[Player].w/2-16;
			P[Player].axisY = P[Player].h-5-8;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_570, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_570, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 48;
			P[Player].h = 64;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_cap_606, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_cap_606, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==3)//NINJA
	{
		if( State==100){
			P[Player].w = 40;
			P[Player].h = 80;
			P[Player].axisX = 18;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 240;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 8;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_100, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_100, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			P[Player].w = 104;
			P[Player].h = 72;
			P[Player].axisX = 18;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_101, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_101, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102){
			P[Player].w = 112;
			P[Player].h = 80;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_102, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_102, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103){
			P[Player].w = 112;
			P[Player].h = 64;
			P[Player].axisX = 56;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 12;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 16;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_103, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_103, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104){
			P[Player].w = 96;
			P[Player].h = 88;
			P[Player].axisX = 23;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 12;
			P[Player].dataAnim[7]  = 1;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_104, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_104, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 176;
			P[Player].h = 136;
			P[Player].axisX = 56;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 1;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 1;
			P[Player].dataAnim[12] = 1;
			P[Player].dataAnim[13] = 1;
			P[Player].dataAnim[14] = 1;
			P[Player].dataAnim[15] = 1;
			P[Player].dataAnim[16] = 1;
			P[Player].dataAnim[17] = 1;
			P[Player].dataAnim[18] = 1;
			P[Player].dataAnim[19] = 1;
			P[Player].dataAnim[20] = 1;
			P[Player].dataAnim[21] = 1;
			P[Player].dataAnim[22] = 1;
			P[Player].dataAnim[23] = 1;
			P[Player].dataAnim[24] = 1;
			P[Player].dataAnim[25] = 1;
			P[Player].dataAnim[26] = 1;
			P[Player].dataAnim[27] = 1;
			P[Player].dataAnim[28] = 1;
			P[Player].dataAnim[29] = 1;
			P[Player].dataAnim[30] = 1;
			P[Player].dataAnim[31] = 1;
			P[Player].dataAnim[32] = 1;
			P[Player].dataAnim[33] = 1;
			P[Player].dataAnim[34] = 1;
			P[Player].dataAnim[35] = 1;
			P[Player].dataAnim[36] = 1;
			P[Player].dataAnim[37] = 1;
			P[Player].dataAnim[38] = 1;
			P[Player].dataAnim[39] = 1;
			P[Player].dataAnim[40] = 1;
			P[Player].dataAnim[41] = 1;
			P[Player].dataAnim[42] = 1;
			P[Player].dataAnim[43] = 1;
			P[Player].dataAnim[44] = 1;
			P[Player].dataAnim[45] = 1;
			P[Player].animFrameTotal = 45;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_199, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_199, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==202){
			P[Player].w = 112;
			P[Player].h = 80;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 16;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_202, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_202, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==203){
			P[Player].w = 96;
			P[Player].h = 72;
			P[Player].axisX = 47;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 12;
			P[Player].dataAnim[7]  = 16;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_203, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_203, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==204){
			P[Player].w = 120;
			P[Player].h = 88;
			P[Player].axisX = 45;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 12;
			P[Player].dataAnim[9]  = 2;
			P[Player].animFrameTotal = 9;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_204, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_204, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==205 || State==105){
			P[Player].w = 112;
			P[Player].h = 80;
			P[Player].axisX = 37;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 1;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_205, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_205, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 48;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 99;
			P[Player].animFrameTotal = 9;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_300, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_300, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==301){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 20;
			P[Player].axisY = P[Player].h-5+8;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 25;
			P[Player].dataAnim[3]  = 99;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_301, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_301, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 80;
			P[Player].h = 88;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_320, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_320, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==321){
			P[Player].w = 80;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;//2
			P[Player].dataAnim[3]  = 25;//2
			P[Player].dataAnim[4]  = 2;//2
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_321, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_321, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==322){
			P[Player].w = 96;
			P[Player].h = 88;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 20;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_322, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_322, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==420){
			P[Player].w = 48;
			P[Player].h = 80;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].animFrameTotal = 11;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_420, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_420, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==430){
			P[Player].w = 72;
			P[Player].h = 64;
			P[Player].axisX = 35;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_430, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_430, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==431){
			P[Player].w = 128;
			P[Player].h = 64;
			P[Player].axisX = 43;
			P[Player].axisY = P[Player].h-6;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 30;
			P[Player].dataAnim[4]  = 6;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_431, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_431, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 56;
			P[Player].h = 72;
			P[Player].axisX = 35;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_501, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_501, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 42;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_502, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_502, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 18;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_503, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_503, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 64;
			P[Player].h = 72;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_505, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_505, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 19;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 64;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_508, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_508, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_nin_510, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_nin_513, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_510, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_nin_513, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 32;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_nin_511, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_nin_514, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_511, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_nin_514, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = P[Player].w/2-16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 250;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_550, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_550, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 88;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2-8;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_551, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_551, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 112;
			P[Player].h = 72;
			P[Player].axisX = P[Player].w/2+16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_552, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_552, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 88;
			P[Player].h = 24;
			P[Player].axisX = P[Player].w/2-16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_570, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_570, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}		
		if( State==606){
			P[Player].w = 40;
			P[Player].h = 72;
			P[Player].axisX = 18;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_nin_606, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_nin_606, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==4)//BABY
	{
		if( State==100){
			P[Player].w = 80;
			P[Player].h = 80;
			P[Player].axisX = 36;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 230;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_100, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_100, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			P[Player].w = 112;
			P[Player].h = 72;
			P[Player].axisX = 18;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 15;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_101, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_101, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 18;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 16;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_102, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_102, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 16;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_103, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_103, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104){
			P[Player].w = 144;
			P[Player].h = 72;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 4;//2
			P[Player].dataAnim[3]  = 4;//2
			P[Player].dataAnim[4]  = 6;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_104, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_104, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==105){
			P[Player].w = 144;
			P[Player].h = 80;
			P[Player].axisX = 44;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 6;
			P[Player].dataAnim[7]  = 2;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_105, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_105, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 160;
			P[Player].h = 104;
			P[Player].axisX = 48;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 1;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 1;
			P[Player].dataAnim[12] = 1;
			P[Player].dataAnim[13] = 1;
			P[Player].dataAnim[14] = 1;
			P[Player].dataAnim[15] = 1;
			P[Player].dataAnim[16] = 1;
			P[Player].dataAnim[17] = 1;
			P[Player].dataAnim[18] = 1;
			P[Player].dataAnim[19] = 1;
			P[Player].dataAnim[20] = 1;
			P[Player].dataAnim[21] = 1;
			P[Player].dataAnim[22] = 1;
			P[Player].dataAnim[23] = 1;
			P[Player].dataAnim[24] = 1;
			P[Player].dataAnim[25] = 1;
			P[Player].dataAnim[26] = 1;
			P[Player].dataAnim[27] = 1;
			P[Player].dataAnim[28] = 1;
			P[Player].dataAnim[29] = 1;
			P[Player].dataAnim[30] = 1;
			P[Player].dataAnim[31] = 1;
			P[Player].dataAnim[32] = 1;
			P[Player].dataAnim[33] = 1;
			P[Player].dataAnim[34] = 1;
			P[Player].dataAnim[35] = 1;
			P[Player].dataAnim[36] = 1;
			P[Player].dataAnim[37] = 1;
			P[Player].dataAnim[38] = 1;
			P[Player].dataAnim[39] = 1;
			P[Player].dataAnim[40] = 1;
			P[Player].dataAnim[41] = 1;
			P[Player].dataAnim[42] = 1;
			P[Player].dataAnim[43] = 1;
			P[Player].dataAnim[44] = 1;
			P[Player].dataAnim[45] = 1;
			P[Player].dataAnim[46] = 1;
			P[Player].dataAnim[47] = 1;
			P[Player].animFrameTotal = 47;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_199, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_199, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==202){
			P[Player].w = 120;
			P[Player].h = 80;
			P[Player].axisX = 44;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 12;
			P[Player].dataAnim[8]  = 16;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_202, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_202, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==203){
			P[Player].w = 104;
			P[Player].h = 88;
			P[Player].axisX = 49;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 18;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 8;
			P[Player].dataAnim[10] = 8;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_203, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_203, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==204){
			P[Player].w = 144;
			P[Player].h = 80;
			P[Player].axisX = 44;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 6;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].dataAnim[10] = 12;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_204, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_204, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==205){
			P[Player].w = 120;
			P[Player].h = 80;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 12;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_205, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_205, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 88;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_300, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_300, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==301){
			P[Player].w = 88;
			P[Player].h = 96;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 25;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 99;
			P[Player].animFrameTotal = 7;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_301, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_301, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 88;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_320, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_320, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); }
		}
		if( State==321){
			P[Player].w = 136;
			P[Player].h = 96;
			P[Player].axisX = 56;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 25;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_321, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_321, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==322){
			P[Player].w = 112;
			P[Player].h = 96;
			P[Player].axisX = 56;
			P[Player].axisY = P[Player].h-5+8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_322, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_322, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[Player].ground+1); }
		}
		if( State==420){
			P[Player].w = 80;
			P[Player].h = 80;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].animFrameTotal = 12;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_420, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_420, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==430){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 48;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_430, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_430, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==431){
			P[Player].w = 120;
			P[Player].h = 80;
			P[Player].axisX = 48;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 2;
			P[Player].animFrameTotal = 11;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_431, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_431, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 96;
			P[Player].h = 80;
			P[Player].axisX = 60;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_501, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_501, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 48;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_502, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_502, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 80;
			P[Player].h = 80;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_503, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_503, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 104;
			P[Player].h = 80;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_505, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_505, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 19;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 64;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_508, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_508, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 184;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2+16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_bab_509, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				//P[Player].partB = SPR_addSprite(&spr_bab_513, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_509, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				//P[Player].partB = SPR_addSpriteExSafe(&spr_bab_513, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				//SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			//if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 184;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2+16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 8;
			P[Player].dataAnim[8]  = 8;
			P[Player].dataAnim[9]  = 8;
			P[Player].dataAnim[10] = 8;
			P[Player].dataAnim[11] = 8;
			P[Player].dataAnim[12] = 8;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_bab_509, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
				//P[Player].partB = SPR_addSprite(&spr_bab_514, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_509, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				//P[Player].partB = SPR_addSpriteExSafe(&spr_bab_514, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				//SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 112;
			P[Player].h = 80;
			P[Player].axisX = P[Player].w/2-16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 250;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_550, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_550, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 96;
			P[Player].h = 72;
			P[Player].axisX = P[Player].w/2-8;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_551, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_551, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 96;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_552, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_552, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 112;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-8;
			P[Player].axisY = P[Player].h-5-8;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_570, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_570, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 72;
			P[Player].h = 88;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-6;
			P[Player].dataAnim[1]  = 8;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_bab_606, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_bab_606, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==5)//ENEMY01
	{
		if( State==100){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 8;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 8;
			P[Player].dataAnim[6]  = 8;
			P[Player].dataAnim[7]  = 8;
			P[Player].dataAnim[8]  = 8;
			P[Player].dataAnim[9]  = 8;
			P[Player].dataAnim[10] = 8;
			P[Player].dataAnim[11] = 8;
			P[Player].dataAnim[12] = 8;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 16;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102 || State==202){
			P[Player].w = 104;
			P[Player].h = 72;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_102, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_102, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103 || State==203){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 18;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_103, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_103, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			P[Player].w = 88;
			P[Player].h = 88;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 10;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 64;
			P[Player].h = 48;
			P[Player].axisX = 19;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 56;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==301 || State==321 || State==322){
			P[Player].w = 104;
			P[Player].h = 56;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].ground+1); }
		}
		if( State==420 || State==430){
			P[Player].w = 64;
			P[Player].h = 80;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 22;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 80;
			P[Player].h = 72;
			P[Player].axisX = 28;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 36;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 64;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e01_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e01_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e01_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e01_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e01_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e01_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 96;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 88;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 80;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 112;
			P[Player].h = 32;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 8;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e01_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e01_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==6)//ENEMY02
	{
		if( State==100){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 8;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 8;
			P[Player].dataAnim[6]  = 8;
			P[Player].dataAnim[7]  = 8;
			P[Player].dataAnim[8]  = 8;
			P[Player].dataAnim[9]  = 8;
			P[Player].dataAnim[10] = 8;
			P[Player].dataAnim[11] = 8;
			P[Player].dataAnim[12] = 8;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 16;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102 || State==202){
			P[Player].w = 104;
			P[Player].h = 72;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_102, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_102, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103 || State==203){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 18;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_103, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_103, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			P[Player].w = 88;
			P[Player].h = 88;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 10;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 64;
			P[Player].h = 48;
			P[Player].axisX = 19;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 8;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 64;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==301 || State==321 || State==322){
			P[Player].w = 104;
			P[Player].h = 64;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].ground+1); }
		}
		if( State==420 || State==430){
			P[Player].w = 64;
			P[Player].h = 80;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 22;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 80;
			P[Player].h = 72;
			P[Player].axisX = 28;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 36;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 64;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e02_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e02_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e02_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e02_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e02_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e02_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 96;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 88;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 80;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 112;
			P[Player].h = 32;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 48;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 8;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e02_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e02_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==7)//ENEMY03
	{
		if( State==100){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].dataAnim[2]  = 200;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 8;
			P[Player].dataAnim[3]  = 16;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102 || State==202){
			P[Player].w = 104;
			P[Player].h = 72;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 16;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_102, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_102, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103 || State==203){
			P[Player].w = 88;
			P[Player].h = 72;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 18;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_103, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_103, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			P[Player].w = 88;
			P[Player].h = 88;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 10;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 80;
			P[Player].h = 72;
			P[Player].axisX = 25;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 12;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 72;
			P[Player].h = 64;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==301 || State==321 || State==322){
			P[Player].w = 88;
			P[Player].h = 56;
			P[Player].axisX = 8;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].ground+1); }
		}
		if( State==420 || State==430){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 33;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 22;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 80;
			P[Player].h = 72;
			P[Player].axisX = 28;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 36;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 64;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e03_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e03_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e03_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e03_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e03_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e03_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 96;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 88;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 80;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 112;
			P[Player].h = 32;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 48;
			P[Player].h = 72;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 8;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e03_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e03_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==8)//ENEMY04
	{
		//ATENCAO: Este personagem finaliza a sequencia em 2 hits
		// por isso eu salto do state 102;202 direto para o 104
		if( State==102 || State==202 ){ State=104; } 
		
		if( State==100){
			P[Player].w = 88;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-4;
			P[Player].dataAnim[1]  = 160;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			P[Player].dataAnim[7]  = 6;
			P[Player].dataAnim[8]  = 6;
			P[Player].dataAnim[9]  = 6;
			P[Player].animFrameTotal = 9;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101 || State==102 || State==202 || State==103 || State==203 ){
			P[Player].w = 136;
			P[Player].h = 104;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 12;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 8;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			P[Player].w = 136;
			P[Player].h = 88;
			P[Player].axisX = 38;
			P[Player].axisY = P[Player].h-4;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 12;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 120;
			P[Player].h = 88;
			P[Player].axisX = 38;
			P[Player].axisY = P[Player].h-4;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 20;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 56;
			P[Player].h = 80;
			P[Player].axisX = 16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==301 || State==321 || State==322){
			P[Player].w = 72;
			P[Player].h = 72;
			P[Player].axisX = 16;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].ground+1); }
		}
		if( State==420 || State==430){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 24;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			P[Player].dataAnim[7]  = 6;
			P[Player].dataAnim[8]  = 6;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 44;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 80;
			P[Player].h = 64;
			P[Player].axisX = 50;
			P[Player].axisY = P[Player].h-4;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 44;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 64;
			P[Player].h = 88;
			P[Player].axisX = 28;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 72;
			P[Player].h = 96;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 72;
			P[Player].h = 80;
			P[Player].axisX = 36;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 64;
			P[Player].animFrameTotal = 5;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e04_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e04_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e04_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e04_511, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e04_514, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_511, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e04_514, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 88;
			P[Player].h = 32;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 96;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 104;
			P[Player].h = 16;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 72;
			P[Player].h = 48;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 8;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e04_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e04_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(P[Player].id==9)//ENEMY05
	{
		if( State==100){
			P[Player].w = 72;
			P[Player].h = 104;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101 || State==102 || State==202 || State==103 || State==203 || State==104 || State==204 || State==105 || State==205 || State==431){
			P[Player].w = 144;
			P[Player].h = 112;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 12;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 16;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			P[Player].w = 104;
			P[Player].h = 80;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].animFrameTotal = 9;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 80;
			P[Player].h = 96;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 99;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==301 || State==321 || State==322){
			P[Player].w = 104;
			P[Player].h = 96;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].ground+1); }
		}
		if( State==320){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 104;
			P[Player].h = 96;
			P[Player].axisX = 26;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].dataAnim[10] = 2;
			P[Player].dataAnim[11] = 99;
			P[Player].animFrameTotal = 11;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_320, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_320, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==420){
			P[Player].w = 72;
			P[Player].h = 112;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-6;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			P[Player].animFrameTotal = 6;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==430){
			P[Player].w = 96;
			P[Player].h = 128;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].animFrameTotal = 4;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_430, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_430, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			P[Player].w = 80;
			P[Player].h = 112;
			P[Player].axisX = 49;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 14;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			P[Player].w = 88;
			P[Player].h = 96;
			P[Player].axisX = 48;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].animFrameTotal = 8;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			P[Player].w = 80;
			P[Player].h = 104;
			P[Player].axisX = 32;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 16;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 104;
			P[Player].h = 128;
			P[Player].axisX = 52;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].animFrameTotal = 10;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==506){
			P[Player].w = 88;
			P[Player].h = 112;
			P[Player].axisX = 54;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 4;
			P[Player].dataAnim[11] = 4;
			P[Player].dataAnim[12] = 4;
			P[Player].dataAnim[13] = 8;
			P[Player].animFrameTotal = 13;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			P[Player].w = 96;
			P[Player].h = 48;
			P[Player].axisX = P[Player].w/2-1;
			P[Player].axisY = P[Player].h-7-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 3;
			P[Player].dataAnim[16] = 3;
			P[Player].dataAnim[17] = 3;
			P[Player].dataAnim[18] = 3;
			P[Player].dataAnim[19] = 3;
			P[Player].dataAnim[20] = 3;
			P[Player].dataAnim[21] = 3;
			P[Player].dataAnim[22] = 3;
			P[Player].dataAnim[23] = 3;
			P[Player].dataAnim[24] = 6;
			P[Player].animFrameTotal = 24;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			P[Player].w = 144;
			P[Player].h = 40;
			P[Player].axisX = 72;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 64;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -10;//impulsoPadrao; //-15
			if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 64;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e05_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e05_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e05_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==510){
			if (P[Player].partB){ SPR_releaseSprite(P[Player].partB); P[Player].partB = NULL; }
			
			//if(P[Player].direction==-1){ P[Player].partBx = -50; }else{ P[Player].partBx = 25; }
			P[Player].w = 64;
			P[Player].h = 32;
			P[Player].axisX = 27;
			P[Player].axisY = P[Player].h-5;
			
			P[Player].dataAnim[1]  = 120;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				P[Player].sprite = SPR_addSprite(&spr_e05_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				P[Player].partB = SPR_addSprite(&spr_e05_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				P[Player].partB = SPR_addSpriteExSafe(&spr_e05_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(P[Player].direction==-1)
			{
				SPR_setHFlip(P[Player].sprite, 1);
				SPR_setHFlip(P[Player].partB, 1);
			}
		}
		if( State==550){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].w = 128;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==551){
			P[Player].ground = P[Player].y;
			P[Player].y--;
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = -8;
			P[Player].w = 128;
			P[Player].h = 64;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, 257-P[Player].y+1); }
		}
		if( State==552){
			P[Player].w = 128;
			P[Player].h = 104;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].animFrameTotal = 2;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			P[Player].w = 144;
			P[Player].h = 40;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 90;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			P[Player].w = 80;
			P[Player].h = 96;
			P[Player].axisX = 40;
			P[Player].axisY = P[Player].h-5;
			P[Player].dataAnim[1]  = 8;
			P[Player].animFrameTotal = 1;
			if(SPR_METHOD==1){ P[Player].sprite = SPR_addSprite(&spr_e05_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ P[Player].sprite = SPR_addSpriteExSafe(&spr_e05_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	//morte, dead state
	if( State==999){
		P[Player].w = 0;
		P[Player].h = 0;
		P[Player].axisX = 0;
		P[Player].axisY = 0;
		P[Player].dataAnim[1]  = 250;
		P[Player].animFrameTotal = 1;
		//OBS: este state nao carrega novo sprite
	}	
	
	if(P[Player].sprite)
	{
		if(P[Player].direction==-1){ SPR_setHFlip(P[Player].sprite, TRUE); }
		SPR_setAnimAndFrame(P[Player].sprite, 0, P[Player].animFrame-1);
		P[Player].frameTimeTotal  = P[Player].dataAnim[1];
		if (P[Player].sprite){ FUNCAO_SPRITE_POSITION(); } //Define a prioridade de desenho, e a posicao do Sprite (se sprite completamente carregado)
		FUNCAO_FSM_HITBOXES(Player); //Atualiza as Hurt / Hitboxes
	}
	
	/*
	//>>>>>>>>>>if (P[Player].sprite){ SPR_releaseSprite(P[Player].sprite); P[Player].sprite = NULL; }
	
	//>>>>>>>>>>P[Player].animFrame      = 1;
	//>>>>>>>>>>P[Player].frameTimeAtual = 1;
	//>>>>>>>>>>P[Player].dataAnim[1]    = 1;
	//>>>>>>>>>>P[Player].animFrameTotal = 1;
	//>>>>>>>>>>P[Player].state = State;
	
	FUNCAO_PLAY_SND(Player, State); //SOUND; SFX!
	
	//virando de lado (mudanca de estado)
	if(Player==1)
	{
		if(P[1].direcao== 1 && P[2].x<P[1].x && State==100){ State=607; }
		if(P[1].direcao==-1 && P[1].x<P[2].x && State==100){ State=607; }
		if(P[1].direcao== 1 && P[2].x<P[1].x && State==200){ State=608; }
		if(P[1].direcao==-1 && P[1].x<P[2].x && State==200){ State=608; }
	}else{
		if(P[2].direcao== 1 && P[1].x<P[2].x && State==100){ State=607; }
		if(P[2].direcao==-1 && P[2].x<P[1].x && State==100){ State=607; }
		if(P[2].direcao== 1 && P[1].x<P[2].x && State==200){ State=608; }
		if(P[2].direcao==-1 && P[2].x<P[1].x && State==200){ State=608; }
	}
	
	//metodo2 SPR (desativado)
	//SPR_setDefinition (P[Player].sprite, &spr_raiden_100);
	
	//--- DEBUG_CHARACTER_TEST ---// #ID:0
	if(P[Player].id==0)
	{
		if(State==100){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8;
			P[Player].h = 8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_point, 512, 256, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
	}
	
	//--- CHAR1 ---// #ID:1
	if(P[Player].id==1)
	{
		if(State==100){
			P[Player].y = gAlturaPiso;
			P[Player].w = 12*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 120;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mum_100, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==101){
			P[Player].w = 13*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+16;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 14;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_geese_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
	}
	
	if(P[Player].direcao==1){SPR_setHFlip(P[Player].sprite, FALSE);}else{SPR_setHFlip(P[Player].sprite, TRUE);}
	SPR_setAnimAndFrame(P[Player].sprite, 0, P[Player].animFrame-1);
	P[Player].frameTimeTotal  = P[Player].dataAnim[1];
	if (P[Player].sprite){ FUNCAO_DEPTH(Player); } //Define a prioridade de desenho (se sprite completamente carregado)
	FUNCAO_SPRITE_POSITION(); //Define a posicao do Sprite
	FUNCAO_FSM_HITBOXES(Player); //Atualiza as Hurt / Hitboxes
	*/
}

void ENEMY_STATE(const u8 Enemy, u16 State)
{
	//padroes
	if (E[Enemy].sprite){ SPR_releaseSprite(E[Enemy].sprite); E[Enemy].sprite = NULL; }
	E[Enemy].animFrame      = 1;
	E[Enemy].frameTimeAtual = 1;
	E[Enemy].dataAnim[1]    = 1;
	E[Enemy].animFrameTotal = 1;
	E[Enemy].state = State;

	
	
	FUNCAO_PLAY_SND(E[Enemy].id, State, Enemy); //SOUND; SFX!
	
	if(E[Enemy].id==5)//ENEMY01
	{
		if( State==100){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 8;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 8;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 8;
			E[Enemy].dataAnim[6]  = 8;
			E[Enemy].dataAnim[7]  = 8;
			E[Enemy].dataAnim[8]  = 8;
			E[Enemy].dataAnim[9]  = 8;
			E[Enemy].dataAnim[10] = 8;
			E[Enemy].dataAnim[11] = 8;
			E[Enemy].dataAnim[12] = 8;
			E[Enemy].dataAnim[13] = 8;
			E[Enemy].animFrameTotal = 13;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			E[Enemy].w = 88;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 6;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 16;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102 || State==202){
			E[Enemy].w = 104;
			E[Enemy].h = 72;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 12;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 16;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_102, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_102, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103 || State==203){
			E[Enemy].w = 88;
			E[Enemy].h = 72;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 18;
			E[Enemy].dataAnim[3]  = 2;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_103, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_103, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			E[Enemy].w = 88;
			E[Enemy].h = 88;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 1;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 12;
			E[Enemy].dataAnim[5]  = 10;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			E[Enemy].w = 64;
			E[Enemy].h = 48;
			E[Enemy].axisX = 19;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 8;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 16;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 72;
			E[Enemy].h = 56;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==301 || State==321 || State==322){
			E[Enemy].w = 104;
			E[Enemy].h = 56;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].ground+1); }
		}
		if( State==420 || State==430){
			E[Enemy].w = 64;
			E[Enemy].h = 80;
			E[Enemy].axisX = 33;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].animFrameTotal = 8;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 22;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 80;
			E[Enemy].h = 72;
			E[Enemy].axisX = 28;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].animFrameTotal = 10;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==506){
			E[Enemy].w = 72;
			E[Enemy].h = 96;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].dataAnim[11] = 4;
			E[Enemy].dataAnim[12] = 4;
			E[Enemy].dataAnim[13] = 8;
			E[Enemy].animFrameTotal = 13;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			E[Enemy].w = 96;
			E[Enemy].h = 48;
			E[Enemy].axisX = E[Enemy].w/2-1;
			E[Enemy].axisY = E[Enemy].h-7-8;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 3;
			E[Enemy].dataAnim[5]  = 3;
			E[Enemy].dataAnim[6]  = 3;
			E[Enemy].dataAnim[7]  = 3;
			E[Enemy].dataAnim[8]  = 3;
			E[Enemy].dataAnim[9]  = 3;
			E[Enemy].dataAnim[10] = 3;
			E[Enemy].dataAnim[11] = 3;
			E[Enemy].dataAnim[12] = 3;
			E[Enemy].dataAnim[13] = 3;
			E[Enemy].dataAnim[14] = 3;
			E[Enemy].dataAnim[15] = 3;
			E[Enemy].dataAnim[16] = 3;
			E[Enemy].dataAnim[17] = 3;
			E[Enemy].dataAnim[18] = 3;
			E[Enemy].dataAnim[19] = 3;
			E[Enemy].dataAnim[20] = 3;
			E[Enemy].dataAnim[21] = 3;
			E[Enemy].dataAnim[22] = 3;
			E[Enemy].dataAnim[23] = 3;
			E[Enemy].dataAnim[24] = 6;
			E[Enemy].animFrameTotal = 24;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 36;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 6;
			E[Enemy].dataAnim[4]  = 6;
			E[Enemy].dataAnim[5]  = 6;
			E[Enemy].dataAnim[6]  = 64;
			E[Enemy].animFrameTotal = 6;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -10;//impulsoPadrao; //-15
			if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e01_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e01_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e01_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==510){
			if (E[Enemy].partB){ SPR_releaseSprite(E[Enemy].partB); E[Enemy].partB = NULL; }
			
			//if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 32;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			
			E[Enemy].dataAnim[1]  = 120;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e01_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e01_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e01_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==550){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 96;
			E[Enemy].h = 40;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==551){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -8;
			E[Enemy].w = 88;
			E[Enemy].h = 40;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==552){
			E[Enemy].w = 80;
			E[Enemy].h = 64;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].animFrameTotal = 2;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			E[Enemy].w = 112;
			E[Enemy].h = 32;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 90;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 8;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==700){
			E[Enemy].w = 64;
			E[Enemy].h = 48;
			E[Enemy].axisX = 19;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 8;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 16;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==701){
			E[Enemy].w = 72;
			E[Enemy].h = 48;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 20;
			E[Enemy].dataAnim[2]  = 12;
			E[Enemy].dataAnim[3]  = 12;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e01_198, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e01_198, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(E[Enemy].id==6)//ENEMY02
	{
		if( State==100){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 8;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 8;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 8;
			E[Enemy].dataAnim[6]  = 8;
			E[Enemy].dataAnim[7]  = 8;
			E[Enemy].dataAnim[8]  = 8;
			E[Enemy].dataAnim[9]  = 8;
			E[Enemy].dataAnim[10] = 8;
			E[Enemy].dataAnim[11] = 8;
			E[Enemy].dataAnim[12] = 8;
			E[Enemy].dataAnim[13] = 8;
			E[Enemy].animFrameTotal = 13;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			E[Enemy].w = 88;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 6;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 16;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102 || State==202){
			E[Enemy].w = 104;
			E[Enemy].h = 72;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 12;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 16;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_102, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_102, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103 || State==203){
			E[Enemy].w = 88;
			E[Enemy].h = 72;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 18;
			E[Enemy].dataAnim[3]  = 2;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_103, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_103, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			E[Enemy].w = 88;
			E[Enemy].h = 88;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 1;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 12;
			E[Enemy].dataAnim[5]  = 10;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			E[Enemy].w = 64;
			E[Enemy].h = 48;
			E[Enemy].axisX = 19;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 8;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 16;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 72;
			E[Enemy].h = 64;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==301 || State==321 || State==322){
			E[Enemy].w = 104;
			E[Enemy].h = 64;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].ground+1); }
		}
		if( State==420 || State==430){
			E[Enemy].w = 64;
			E[Enemy].h = 80;
			E[Enemy].axisX = 33;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].animFrameTotal = 8;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 22;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 80;
			E[Enemy].h = 72;
			E[Enemy].axisX = 28;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].animFrameTotal = 10;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==506){
			E[Enemy].w = 72;
			E[Enemy].h = 96;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].dataAnim[11] = 4;
			E[Enemy].dataAnim[12] = 4;
			E[Enemy].dataAnim[13] = 8;
			E[Enemy].animFrameTotal = 13;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			E[Enemy].w = 96;
			E[Enemy].h = 48;
			E[Enemy].axisX = E[Enemy].w/2-1;
			E[Enemy].axisY = E[Enemy].h-7-8;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 3;
			E[Enemy].dataAnim[5]  = 3;
			E[Enemy].dataAnim[6]  = 3;
			E[Enemy].dataAnim[7]  = 3;
			E[Enemy].dataAnim[8]  = 3;
			E[Enemy].dataAnim[9]  = 3;
			E[Enemy].dataAnim[10] = 3;
			E[Enemy].dataAnim[11] = 3;
			E[Enemy].dataAnim[12] = 3;
			E[Enemy].dataAnim[13] = 3;
			E[Enemy].dataAnim[14] = 3;
			E[Enemy].dataAnim[15] = 3;
			E[Enemy].dataAnim[16] = 3;
			E[Enemy].dataAnim[17] = 3;
			E[Enemy].dataAnim[18] = 3;
			E[Enemy].dataAnim[19] = 3;
			E[Enemy].dataAnim[20] = 3;
			E[Enemy].dataAnim[21] = 3;
			E[Enemy].dataAnim[22] = 3;
			E[Enemy].dataAnim[23] = 3;
			E[Enemy].dataAnim[24] = 6;
			E[Enemy].animFrameTotal = 24;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 36;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 6;
			E[Enemy].dataAnim[4]  = 6;
			E[Enemy].dataAnim[5]  = 6;
			E[Enemy].dataAnim[6]  = 64;
			E[Enemy].animFrameTotal = 6;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -10;//impulsoPadrao; //-15
			if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e02_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e02_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e02_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==510){
			if (E[Enemy].partB){ SPR_releaseSprite(E[Enemy].partB); E[Enemy].partB = NULL; }
			
			//if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 32;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			
			E[Enemy].dataAnim[1]  = 120;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e02_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e02_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e02_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==550){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 96;
			E[Enemy].h = 40;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==551){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -8;
			E[Enemy].w = 88;
			E[Enemy].h = 40;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==552){
			E[Enemy].w = 80;
			E[Enemy].h = 64;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].animFrameTotal = 2;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			E[Enemy].w = 112;
			E[Enemy].h = 32;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 90;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			E[Enemy].w = 48;
			E[Enemy].h = 80;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 8;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==700){
			E[Enemy].w = 64;
			E[Enemy].h = 48;
			E[Enemy].axisX = 19;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 8;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 16;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==701){
			E[Enemy].w = 72;
			E[Enemy].h = 48;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 20;
			E[Enemy].dataAnim[2]  = 12;
			E[Enemy].dataAnim[3]  = 12;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e02_198, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e02_198, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(E[Enemy].id==7)//ENEMY03
	{
		if( State==100){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].dataAnim[2]  = 200;
			E[Enemy].animFrameTotal = 2;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101){
			E[Enemy].w = 88;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 6;
			E[Enemy].dataAnim[2]  = 8;
			E[Enemy].dataAnim[3]  = 16;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==102 || State==202){
			E[Enemy].w = 104;
			E[Enemy].h = 72;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 12;
			E[Enemy].dataAnim[4]  = 8;
			E[Enemy].dataAnim[5]  = 16;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_102, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_102, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==103 || State==203){
			E[Enemy].w = 88;
			E[Enemy].h = 72;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 18;
			E[Enemy].dataAnim[3]  = 2;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_103, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_103, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			E[Enemy].w = 88;
			E[Enemy].h = 88;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 1;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 12;
			E[Enemy].dataAnim[5]  = 10;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			E[Enemy].w = 80;
			E[Enemy].h = 72;
			E[Enemy].axisX = 25;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 2;
			E[Enemy].dataAnim[2]  = 2;
			E[Enemy].dataAnim[3]  = 2;
			E[Enemy].dataAnim[4]  = 2;
			E[Enemy].dataAnim[5]  = 12;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 72;
			E[Enemy].h = 64;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==301 || State==321 || State==322){
			E[Enemy].w = 88;
			E[Enemy].h = 56;
			E[Enemy].axisX = 8;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].ground+1); }
		}
		if( State==420 || State==430){
			E[Enemy].w = 72;
			E[Enemy].h = 80;
			E[Enemy].axisX = 33;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].animFrameTotal = 8;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 22;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 80;
			E[Enemy].h = 72;
			E[Enemy].axisX = 28;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].animFrameTotal = 10;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==506){
			E[Enemy].w = 72;
			E[Enemy].h = 96;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].dataAnim[11] = 4;
			E[Enemy].dataAnim[12] = 4;
			E[Enemy].dataAnim[13] = 8;
			E[Enemy].animFrameTotal = 13;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			E[Enemy].w = 96;
			E[Enemy].h = 48;
			E[Enemy].axisX = E[Enemy].w/2-1;
			E[Enemy].axisY = E[Enemy].h-7-8;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 3;
			E[Enemy].dataAnim[5]  = 3;
			E[Enemy].dataAnim[6]  = 3;
			E[Enemy].dataAnim[7]  = 3;
			E[Enemy].dataAnim[8]  = 3;
			E[Enemy].dataAnim[9]  = 3;
			E[Enemy].dataAnim[10] = 3;
			E[Enemy].dataAnim[11] = 3;
			E[Enemy].dataAnim[12] = 3;
			E[Enemy].dataAnim[13] = 3;
			E[Enemy].dataAnim[14] = 3;
			E[Enemy].dataAnim[15] = 3;
			E[Enemy].dataAnim[16] = 3;
			E[Enemy].dataAnim[17] = 3;
			E[Enemy].dataAnim[18] = 3;
			E[Enemy].dataAnim[19] = 3;
			E[Enemy].dataAnim[20] = 3;
			E[Enemy].dataAnim[21] = 3;
			E[Enemy].dataAnim[22] = 3;
			E[Enemy].dataAnim[23] = 3;
			E[Enemy].dataAnim[24] = 6;
			E[Enemy].animFrameTotal = 24;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 36;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 6;
			E[Enemy].dataAnim[4]  = 6;
			E[Enemy].dataAnim[5]  = 6;
			E[Enemy].dataAnim[6]  = 64;
			E[Enemy].animFrameTotal = 6;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -10;//impulsoPadrao; //-15
			if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e03_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e03_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e03_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==510){
			if (E[Enemy].partB){ SPR_releaseSprite(E[Enemy].partB); E[Enemy].partB = NULL; }
			
			//if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 32;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			
			E[Enemy].dataAnim[1]  = 120;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e03_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e03_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e03_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==550){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 96;
			E[Enemy].h = 40;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==551){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -8;
			E[Enemy].w = 88;
			E[Enemy].h = 40;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==552){
			E[Enemy].w = 80;
			E[Enemy].h = 64;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].animFrameTotal = 2;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			E[Enemy].w = 112;
			E[Enemy].h = 32;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 90;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			E[Enemy].w = 48;
			E[Enemy].h = 72;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 8;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e03_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e03_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(E[Enemy].id==8)//ENEMY04
	{
		//ATENCAO: Este personagem finaliza a sequencia em 2 hits
		// por isso eu salto do state 102;202 direto para o 104
		if( State==102 || State==202 ){ State=104; } 
		
		if( State==100){
			E[Enemy].w = 88;
			E[Enemy].h = 80;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-4;
			E[Enemy].dataAnim[1]  = 160;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 6;
			E[Enemy].dataAnim[4]  = 6;
			E[Enemy].dataAnim[5]  = 6;
			E[Enemy].dataAnim[6]  = 6;
			E[Enemy].dataAnim[7]  = 6;
			E[Enemy].dataAnim[8]  = 6;
			E[Enemy].dataAnim[9]  = 6;
			E[Enemy].animFrameTotal = 9;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101 || State==102 || State==202 || State==103 || State==203 ){
			E[Enemy].w = 136;
			E[Enemy].h = 104;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 2;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 3;
			E[Enemy].dataAnim[5]  = 12;
			E[Enemy].dataAnim[6]  = 3;
			E[Enemy].dataAnim[7]  = 3;
			E[Enemy].dataAnim[8]  = 8;
			E[Enemy].animFrameTotal = 8;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==104 || State==204 || State==105 || State==205 || State==431 ){
			E[Enemy].w = 136;
			E[Enemy].h = 88;
			E[Enemy].axisX = 38;
			E[Enemy].axisY = E[Enemy].h-4;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 3;
			E[Enemy].dataAnim[5]  = 3;
			E[Enemy].dataAnim[6]  = 12;
			E[Enemy].animFrameTotal = 6;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_104, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_104, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			E[Enemy].w = 120;
			E[Enemy].h = 88;
			E[Enemy].axisX = 38;
			E[Enemy].axisY = E[Enemy].h-4;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 20;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300 || State==320){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 56;
			E[Enemy].h = 80;
			E[Enemy].axisX = 16;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==301 || State==321 || State==322){
			E[Enemy].w = 72;
			E[Enemy].h = 72;
			E[Enemy].axisX = 16;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].ground+1); }
		}
		if( State==420 || State==430){
			E[Enemy].w = 72;
			E[Enemy].h = 80;
			E[Enemy].axisX = 24;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 6;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 6;
			E[Enemy].dataAnim[4]  = 6;
			E[Enemy].dataAnim[5]  = 6;
			E[Enemy].dataAnim[6]  = 6;
			E[Enemy].dataAnim[7]  = 6;
			E[Enemy].dataAnim[8]  = 6;
			E[Enemy].animFrameTotal = 8;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			E[Enemy].w = 72;
			E[Enemy].h = 80;
			E[Enemy].axisX = 44;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 9;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			E[Enemy].w = 80;
			E[Enemy].h = 64;
			E[Enemy].axisX = 50;
			E[Enemy].axisY = E[Enemy].h-4;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 9;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			E[Enemy].w = 72;
			E[Enemy].h = 80;
			E[Enemy].axisX = 44;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 9;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 64;
			E[Enemy].h = 88;
			E[Enemy].axisX = 28;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].animFrameTotal = 10;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==506){
			E[Enemy].w = 72;
			E[Enemy].h = 96;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].dataAnim[11] = 4;
			E[Enemy].dataAnim[12] = 4;
			E[Enemy].dataAnim[13] = 8;
			E[Enemy].animFrameTotal = 13;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			E[Enemy].w = 96;
			E[Enemy].h = 48;
			E[Enemy].axisX = E[Enemy].w/2-1;
			E[Enemy].axisY = E[Enemy].h-7-8;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 3;
			E[Enemy].dataAnim[5]  = 3;
			E[Enemy].dataAnim[6]  = 3;
			E[Enemy].dataAnim[7]  = 3;
			E[Enemy].dataAnim[8]  = 3;
			E[Enemy].dataAnim[9]  = 3;
			E[Enemy].dataAnim[10] = 3;
			E[Enemy].dataAnim[11] = 3;
			E[Enemy].dataAnim[12] = 3;
			E[Enemy].dataAnim[13] = 3;
			E[Enemy].dataAnim[14] = 3;
			E[Enemy].dataAnim[15] = 3;
			E[Enemy].dataAnim[16] = 3;
			E[Enemy].dataAnim[17] = 3;
			E[Enemy].dataAnim[18] = 3;
			E[Enemy].dataAnim[19] = 3;
			E[Enemy].dataAnim[20] = 3;
			E[Enemy].dataAnim[21] = 3;
			E[Enemy].dataAnim[22] = 3;
			E[Enemy].dataAnim[23] = 3;
			E[Enemy].dataAnim[24] = 6;
			E[Enemy].animFrameTotal = 24;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			E[Enemy].w = 72;
			E[Enemy].h = 80;
			E[Enemy].axisX = 36;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 12;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 6;
			E[Enemy].dataAnim[4]  = 6;
			E[Enemy].dataAnim[5]  = 64;
			E[Enemy].animFrameTotal = 5;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_508, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_508, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -10;//impulsoPadrao; //-15
			if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e04_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e04_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e04_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==510){
			if (E[Enemy].partB){ SPR_releaseSprite(E[Enemy].partB); E[Enemy].partB = NULL; }
			
			//if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 32;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			
			E[Enemy].dataAnim[1]  = 120;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e04_511, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e04_514, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_511, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e04_514, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==550){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 96;
			E[Enemy].h = 48;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==551){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -8;
			E[Enemy].w = 88;
			E[Enemy].h = 32;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==552){
			E[Enemy].w = 96;
			E[Enemy].h = 64;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			E[Enemy].w = 104;
			E[Enemy].h = 16;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 90;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			E[Enemy].w = 72;
			E[Enemy].h = 48;
			E[Enemy].axisX = 32;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 8;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e04_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e04_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	if(E[Enemy].id==9)//ENEMY05
	{
		if( State==100){
			E[Enemy].w = 72;
			E[Enemy].h = 104;
			E[Enemy].axisX = 40;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_100, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_100, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==101 || State==102 || State==202 || State==103 || State==203 || State==104 || State==204 || State==105 || State==205 || State==431){
			E[Enemy].w = 144;
			E[Enemy].h = 112;
			E[Enemy].axisX = 52;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 12;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 16;
			E[Enemy].animFrameTotal = 6;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_101, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_101, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==199){
			E[Enemy].w = 104;
			E[Enemy].h = 80;
			E[Enemy].axisX = 52;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].animFrameTotal = 9;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_199, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_199, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==300){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 80;
			E[Enemy].h = 96;
			E[Enemy].axisX = 40;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 10;
			E[Enemy].dataAnim[2]  = 10;
			E[Enemy].dataAnim[3]  = 99;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_300, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_300, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==301 || State==321 || State==322){
			E[Enemy].w = 104;
			E[Enemy].h = 96;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_301, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_301, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].ground+1); }
		}
		if( State==320){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 104;
			E[Enemy].h = 96;
			E[Enemy].axisX = 26;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 2;
			E[Enemy].dataAnim[3]  = 2;
			E[Enemy].dataAnim[4]  = 2;
			E[Enemy].dataAnim[5]  = 2;
			E[Enemy].dataAnim[6]  = 2;
			E[Enemy].dataAnim[7]  = 2;
			E[Enemy].dataAnim[8]  = 2;
			E[Enemy].dataAnim[9]  = 2;
			E[Enemy].dataAnim[10] = 2;
			E[Enemy].dataAnim[11] = 99;
			E[Enemy].animFrameTotal = 11;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_320, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_320, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==420){
			E[Enemy].w = 72;
			E[Enemy].h = 112;
			E[Enemy].axisX = 40;
			E[Enemy].axisY = E[Enemy].h-6;
			E[Enemy].dataAnim[1]  = 6;
			E[Enemy].dataAnim[2]  = 6;
			E[Enemy].dataAnim[3]  = 6;
			E[Enemy].dataAnim[4]  = 6;
			E[Enemy].dataAnim[5]  = 6;
			E[Enemy].dataAnim[6]  = 6;
			E[Enemy].animFrameTotal = 6;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_420, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_420, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==430){
			E[Enemy].w = 96;
			E[Enemy].h = 128;
			E[Enemy].axisX = 52;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].animFrameTotal = 4;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_430, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_430, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==501){
			E[Enemy].w = 80;
			E[Enemy].h = 112;
			E[Enemy].axisX = 49;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 14;
			E[Enemy].dataAnim[3]  = 2;
			E[Enemy].animFrameTotal = 3;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_501, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_501, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==502){
			E[Enemy].w = 88;
			E[Enemy].h = 96;
			E[Enemy].axisX = 48;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 2;
			E[Enemy].dataAnim[2]  = 2;
			E[Enemy].dataAnim[3]  = 2;
			E[Enemy].dataAnim[4]  = 2;
			E[Enemy].dataAnim[5]  = 2;
			E[Enemy].dataAnim[6]  = 2;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].animFrameTotal = 8;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_502, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_502, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==503){
			E[Enemy].w = 80;
			E[Enemy].h = 104;
			E[Enemy].axisX = 32;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 16;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_503, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_503, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==505){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 104;
			E[Enemy].h = 128;
			E[Enemy].axisX = 52;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].animFrameTotal = 10;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_505, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_505, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==506){
			E[Enemy].w = 88;
			E[Enemy].h = 112;
			E[Enemy].axisX = 54;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].dataAnim[3]  = 4;
			E[Enemy].dataAnim[4]  = 4;
			E[Enemy].dataAnim[5]  = 4;
			E[Enemy].dataAnim[6]  = 4;
			E[Enemy].dataAnim[7]  = 4;
			E[Enemy].dataAnim[8]  = 4;
			E[Enemy].dataAnim[9]  = 4;
			E[Enemy].dataAnim[10] = 4;
			E[Enemy].dataAnim[11] = 4;
			E[Enemy].dataAnim[12] = 4;
			E[Enemy].dataAnim[13] = 8;
			E[Enemy].animFrameTotal = 13;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_506, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_506, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==507){
			E[Enemy].w = 96;
			E[Enemy].h = 48;
			E[Enemy].axisX = E[Enemy].w/2-1;
			E[Enemy].axisY = E[Enemy].h-7-8;
			E[Enemy].dataAnim[1]  = 3;
			E[Enemy].dataAnim[2]  = 3;
			E[Enemy].dataAnim[3]  = 3;
			E[Enemy].dataAnim[4]  = 3;
			E[Enemy].dataAnim[5]  = 3;
			E[Enemy].dataAnim[6]  = 3;
			E[Enemy].dataAnim[7]  = 3;
			E[Enemy].dataAnim[8]  = 3;
			E[Enemy].dataAnim[9]  = 3;
			E[Enemy].dataAnim[10] = 3;
			E[Enemy].dataAnim[11] = 3;
			E[Enemy].dataAnim[12] = 3;
			E[Enemy].dataAnim[13] = 3;
			E[Enemy].dataAnim[14] = 3;
			E[Enemy].dataAnim[15] = 3;
			E[Enemy].dataAnim[16] = 3;
			E[Enemy].dataAnim[17] = 3;
			E[Enemy].dataAnim[18] = 3;
			E[Enemy].dataAnim[19] = 3;
			E[Enemy].dataAnim[20] = 3;
			E[Enemy].dataAnim[21] = 3;
			E[Enemy].dataAnim[22] = 3;
			E[Enemy].dataAnim[23] = 3;
			E[Enemy].dataAnim[24] = 6;
			E[Enemy].animFrameTotal = 24;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_507, 512, 256, TILE_ATTR(PAL3,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_507, 512, 256, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==508){
			E[Enemy].w = 144;
			E[Enemy].h = 40;
			E[Enemy].axisX = 72;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 64;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==509){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -10;//impulsoPadrao; //-15
			if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 64;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e05_509, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e05_512, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_509, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e05_512, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==510){
			if (E[Enemy].partB){ SPR_releaseSprite(E[Enemy].partB); E[Enemy].partB = NULL; }
			
			//if(E[Enemy].direction==-1){ E[Enemy].partBx = -50; }else{ E[Enemy].partBx = 25; }
			E[Enemy].w = 64;
			E[Enemy].h = 32;
			E[Enemy].axisX = 27;
			E[Enemy].axisY = E[Enemy].h-5;
			
			E[Enemy].dataAnim[1]  = 120;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1)
			{ 
				E[Enemy].sprite = SPR_addSprite(&spr_e05_510, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
				E[Enemy].partB = SPR_addSprite(&spr_e05_513, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); 
			}
			if(SPR_METHOD==2){ 
				E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_510, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
				E[Enemy].partB = SPR_addSpriteExSafe(&spr_e05_513, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
			}
			if(E[Enemy].direction==-1)
			{
				SPR_setHFlip(E[Enemy].sprite, 1);
				SPR_setHFlip(E[Enemy].partB, 1);
			}
		}
		if( State==550){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = impulsoPadrao;
			E[Enemy].w = 128;
			E[Enemy].h = 64;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_550, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_550, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==551){
			E[Enemy].ground = E[Enemy].y;
			E[Enemy].y--;
			E[Enemy].gravidadeY = gravidadePadrao; 
			E[Enemy].impulsoY = -8;
			E[Enemy].w = 128;
			E[Enemy].h = 64;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 250;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_551, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_551, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(E[Enemy].sprite){ SPR_setDepth(E[Enemy].sprite, 257-E[Enemy].y+1); }
		}
		if( State==552){
			E[Enemy].w = 128;
			E[Enemy].h = 104;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 4;
			E[Enemy].dataAnim[2]  = 4;
			E[Enemy].animFrameTotal = 2;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_552, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_552, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==570){
			E[Enemy].w = 144;
			E[Enemy].h = 40;
			E[Enemy].axisX = E[Enemy].w/2;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 90;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_570, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_570, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		if( State==606){
			E[Enemy].w = 80;
			E[Enemy].h = 96;
			E[Enemy].axisX = 40;
			E[Enemy].axisY = E[Enemy].h-5;
			E[Enemy].dataAnim[1]  = 8;
			E[Enemy].animFrameTotal = 1;
			if(SPR_METHOD==1){ E[Enemy].sprite = SPR_addSprite(&spr_e05_606, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
			if(SPR_METHOD==2){ E[Enemy].sprite = SPR_addSpriteExSafe(&spr_e05_606, 512, 256, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
	}
	
	//morte, dead state
	if( State==999){
		E[Enemy].w = 0;
		E[Enemy].h = 0;
		E[Enemy].axisX = 0;
		E[Enemy].axisY = 0;
		E[Enemy].dataAnim[1]  = 250;
		E[Enemy].animFrameTotal = 1;
		//OBS: este state nao carrega novo sprite
	}	
	
	if(E[Enemy].sprite)
	{
		if(E[Enemy].direction==-1){ SPR_setHFlip(E[Enemy].sprite, TRUE); }
		SPR_setAnimAndFrame(E[Enemy].sprite, 0, E[Enemy].animFrame-1);
		E[Enemy].frameTimeTotal  = E[Enemy].dataAnim[1];
		if (E[Enemy].sprite){ FUNCAO_SPRITE_POSITION(); } //Define a prioridade de desenho, e a posicao do Sprite (se sprite completamente carregado)
		FUNCAO_FSM_HITBOXES_ENEMYS(Enemy); //Atualiza as Hurt / Hitboxes
	}
	
	
}

void FUNCAO_INIT_PLAYERS(void)
{
	//configuracoes iniciais dos players
	
	//P1
	if(P[1].id==0){ P[1].id = 1; }
	P[1].energy = MAX_ENERGY;
	P[1].direction = 1;
	P[1].ground = 0;
	P[1].x = 64;
	P[1].y = 190;
	P[1].CountDownSpark = 0;
	P[1].CountDownEnemyInfo=0;
	P[1].velocidade = 1;
	P[1].key_JOY_countdown[8]=0;
	P[1].key_JOY_countdown[2]=0;
	P[1].key_JOY_countdown[4]=0;
	P[1].key_JOY_countdown[6]=0;
	P[1].enableDashAirAtack = 0;
	P[1].dataBBox[0]=-16;
	P[1].dataBBox[1]=-72;
	P[1].dataBBox[2]=+16;
	P[1].dataBBox[3]=  0;
	P[1].dataHBox[0]=0;
	P[1].dataHBox[1]=0;
	P[1].dataHBox[2]=0;
	P[1].dataHBox[3]=0;
	PLAYER_STATE(1, 100);
	
	if(qtdePlayers==2)
	{
		//P2
		if(P[2].id==0){ P[2].id = 1; }
		P[2].energy = MAX_ENERGY;
		P[2].direction = 1;
		P[2].ground = 0;
		P[2].x = 64-16;
		P[2].y = 190+16;
		P[2].CountDownSpark = 0;
		P[2].CountDownEnemyInfo=0;
		P[2].velocidade = 1;
		P[2].key_JOY_countdown[8]=0;
		P[2].key_JOY_countdown[2]=0;
		P[2].key_JOY_countdown[4]=0;
		P[2].key_JOY_countdown[6]=0;
		P[2].enableDashAirAtack = 0;
		P[2].dataBBox[0]=-16;
		P[2].dataBBox[1]=-72;
		P[2].dataBBox[2]=+16;
		P[2].dataBBox[3]=  0;
		P[2].dataHBox[0]=0;
		P[2].dataHBox[1]=0;
		P[2].dataHBox[2]=0;
		P[2].dataHBox[3]=0;
		PLAYER_STATE(2, 100);
	}
	
	Rect1BB1_Q1 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1BB1_Q1, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect1BB1_Q2 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1BB1_Q2, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect1BB1_Q3 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1BB1_Q1, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect1BB1_Q4 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1BB1_Q2, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect1HB1_Q1 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1HB1_Q1, 1442); //define uma posicao especifica para o GFX na VRAM
	Rect1HB1_Q2 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1HB1_Q2, 1442); //define uma posicao especifica para o GFX na VRAM
	Rect1HB1_Q3 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1HB1_Q1, 1442); //define uma posicao especifica para o GFX na VRAM
	Rect1HB1_Q4 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect1HB1_Q2, 1442); //define uma posicao especifica para o GFX na VRAM
	
	Rect2BB1_Q1 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2BB1_Q1, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect2BB1_Q2 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2BB1_Q2, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect2BB1_Q3 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2BB1_Q1, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect2BB1_Q4 = SPR_addSprite(&spr_rect_bb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2BB1_Q2, 1441); //define uma posicao especifica para o GFX na VRAM
	Rect2HB1_Q1 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2HB1_Q1, 1442); //define uma posicao especifica para o GFX na VRAM
	Rect2HB1_Q2 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2HB1_Q2, 1442); //define uma posicao especifica para o GFX na VRAM
	Rect2HB1_Q3 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2HB1_Q1, 1442); //define uma posicao especifica para o GFX na VRAM
	Rect2HB1_Q4 = SPR_addSprite(&spr_rect_hb, -8, -8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	SPR_setVRAMTileIndex(Rect2HB1_Q2, 1442); //define uma posicao especifica para o GFX na VRAM
	
	//depth
	if(Rect1BB1_Q1){ SPR_setDepth(Rect1BB1_Q1, 0); }
	if(Rect1BB1_Q2){ SPR_setDepth(Rect1BB1_Q2, 0); }
	if(Rect1BB1_Q3){ SPR_setDepth(Rect1BB1_Q3, 0); }
	if(Rect1BB1_Q4){ SPR_setDepth(Rect1BB1_Q4, 0); }
	if(Rect1HB1_Q1){ SPR_setDepth(Rect1HB1_Q1, 0); }
	if(Rect1HB1_Q2){ SPR_setDepth(Rect1HB1_Q2, 0); }
	if(Rect1HB1_Q3){ SPR_setDepth(Rect1HB1_Q3, 0); }
	if(Rect1HB1_Q4){ SPR_setDepth(Rect1HB1_Q4, 0); }
	if(Rect2BB1_Q1){ SPR_setDepth(Rect2BB1_Q1, 0); }
	if(Rect2BB1_Q2){ SPR_setDepth(Rect2BB1_Q2, 0); }
	if(Rect2BB1_Q3){ SPR_setDepth(Rect2BB1_Q3, 0); }
	if(Rect2BB1_Q4){ SPR_setDepth(Rect2BB1_Q4, 0); }
	if(Rect2HB1_Q1){ SPR_setDepth(Rect2HB1_Q1, 0); }
	if(Rect2HB1_Q2){ SPR_setDepth(Rect2HB1_Q2, 0); }
	if(Rect2HB1_Q3){ SPR_setDepth(Rect2HB1_Q3, 0); }
	if(Rect2HB1_Q4){ SPR_setDepth(Rect2HB1_Q4, 0); }
	
	if(Rect1BB1_Q2){ SPR_setHFlip(Rect1BB1_Q2, TRUE); }
	if(Rect1BB1_Q3){ SPR_setVFlip(Rect1BB1_Q3, TRUE); }
	if(Rect1BB1_Q4){ SPR_setHFlip(Rect1BB1_Q4, TRUE); SPR_setVFlip(Rect1BB1_Q4, TRUE); }
	if(Rect1HB1_Q2){ SPR_setHFlip(Rect1HB1_Q2, TRUE); }
	if(Rect1HB1_Q3){ SPR_setVFlip(Rect1HB1_Q3, TRUE); }
	if(Rect1HB1_Q4){ SPR_setHFlip(Rect1HB1_Q4, TRUE); SPR_setVFlip(Rect1HB1_Q4, TRUE); }
	
	if(Rect2BB1_Q2){ SPR_setHFlip(Rect2BB1_Q2, TRUE); }
	if(Rect2BB1_Q3){ SPR_setVFlip(Rect2BB1_Q3, TRUE); }
	if(Rect2BB1_Q4){ SPR_setHFlip(Rect2BB1_Q4, TRUE); SPR_setVFlip(Rect2BB1_Q4, TRUE); }
	if(Rect2HB1_Q2){ SPR_setHFlip(Rect2HB1_Q2, TRUE); }
	if(Rect2HB1_Q3){ SPR_setVFlip(Rect2HB1_Q3, TRUE); }
	if(Rect2HB1_Q4){ SPR_setHFlip(Rect2HB1_Q4, TRUE); SPR_setVFlip(Rect2HB1_Q4, TRUE); }
	
}

void CREATE_ENEMY(const u8 ID, const u16 State, const s16 pX, const s16 pY)
{
	qtdeInimigos++;
	//ENEMY
	E[qtdeInimigos].id = ID;
	E[qtdeInimigos].state = State;
	E[qtdeInimigos].active = 0;
	E[qtdeInimigos].energy = MAX_ENERGY;
	E[qtdeInimigos].direction = -1;
	E[qtdeInimigos].x = pX;
	E[qtdeInimigos].y = pY;
	E[qtdeInimigos].velocidade = 1;
	E[qtdeInimigos].dataBBox[0]=-24;
	E[qtdeInimigos].dataBBox[1]=-72;
	E[qtdeInimigos].dataBBox[2]=+24;
	E[qtdeInimigos].dataBBox[3]=  0;
	if(E[qtdeInimigos].id == 5){ E[qtdeInimigos].sprite = SPR_addSprite(&spr_point, E[qtdeInimigos].x-E[qtdeInimigos].axisX, E[qtdeInimigos].y-E[qtdeInimigos].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
	if(E[qtdeInimigos].id == 6){ E[qtdeInimigos].sprite = SPR_addSprite(&spr_point, E[qtdeInimigos].x-E[qtdeInimigos].axisX, E[qtdeInimigos].y-E[qtdeInimigos].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
	if(E[qtdeInimigos].id == 7){ E[qtdeInimigos].sprite = SPR_addSprite(&spr_point, E[qtdeInimigos].x-E[qtdeInimigos].axisX, E[qtdeInimigos].y-E[qtdeInimigos].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
	if(E[qtdeInimigos].id == 8){ E[qtdeInimigos].sprite = SPR_addSprite(&spr_point, E[qtdeInimigos].x-E[qtdeInimigos].axisX, E[qtdeInimigos].y-E[qtdeInimigos].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
	if(E[qtdeInimigos].id == 9){ E[qtdeInimigos].sprite = SPR_addSprite(&spr_point, E[qtdeInimigos].x-E[qtdeInimigos].axisX, E[qtdeInimigos].y-E[qtdeInimigos].axisY, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) ); }
	if(E[qtdeInimigos].sprite!=NULL){ SPR_setHFlip(E[qtdeInimigos].sprite,  TRUE); }
	if(E[qtdeInimigos].id==5){ VDP_setPalette(PAL2, spr_e01_100.palette->data); }
	if(E[qtdeInimigos].id==6){ VDP_setPalette(PAL2, spr_e02_100.palette->data); }
	if(E[qtdeInimigos].id==7){ VDP_setPalette(PAL2, spr_e03_100.palette->data); }
	if(E[qtdeInimigos].id==8){ VDP_setPalette(PAL2, spr_e04_100.palette->data); }
	if(E[qtdeInimigos].id==9){ VDP_setPalette(PAL2, spr_e05_100.palette->data); }
	ENEMY_STATE(qtdeInimigos, State);
}

void FUNCAO_FISICA(void)
{
	LimiterX = (segmentLimiter-1)*160;
	
	//MOVIMENTACAO DOS PLAYERS
	
	for(u8 i=1; i<=qtdePlayers; i++)
	{
		//SPECIAIS de "MUMMY" e "DOLG" ; CORRENDO
		if( 
			(P[i].state==199 && P[i].id==1) || 
			(P[i].state==199 && P[i].id==9) || 
			P[i].state==430)
		{
			if(i==1){
				if(JOY1_UP   ) { if(P[i].y-P[i].velocidade*2 > 0){ P[i].y-=P[i].velocidade*2; }else{ P[i].y = 0; } }
				if(JOY1_DOWN ) { if(P[i].y+P[i].velocidade*2 < lvlHSize){ P[i].y+=P[i].velocidade*2; }else{ P[i].y = lvlHSize; } }
				if(JOY1_LEFT ) { if(P[i].x-P[i].velocidade*2 > camPosX+margin){ P[i].x-=P[i].velocidade*2; }else{ P[i].x = camPosX+margin; } }
				if(JOY1_RIGHT) 
				{ 
					if(P[i].x+P[i].velocidade*2 < lvlWSize-margin){ P[i].x+=P[i].velocidade*2; }else{ P[i].x = lvlWSize-margin; } 
					if(P[i].x+P[i].velocidade < camPosX+320-margin){  }else{ P[i].x = camPosX+320-margin; }
				}
			}
			if(i==2){
				if(JOY2_UP   ) { if(P[i].y-P[i].velocidade*2 > 0){ P[i].y-=P[i].velocidade*2; }else{ P[i].y = 0; } }
				if(JOY2_DOWN ) { if(P[i].y+P[i].velocidade*2 < lvlHSize){ P[i].y+=P[i].velocidade*2; }else{ P[i].y = lvlHSize; } }
				if(JOY2_LEFT ) { if(P[i].x-P[i].velocidade*2 > camPosX+margin){ P[i].x-=P[i].velocidade*2; }else{ P[i].x = camPosX+margin; } }
				if(JOY2_RIGHT) 
				{ 
					if(P[i].x+P[i].velocidade*2 < lvlWSize-margin){ P[i].x+=P[i].velocidade*2; }else{ P[i].x = lvlWSize-margin; } 
					if(P[i].x+P[i].velocidade < camPosX+320-margin){  }else{ P[i].x = camPosX+320-margin; }
				}
			}
		}
		
		//PULOS E GOLPES AEREOS; 550; 551
		if(
		P[i].state==300 || 
		P[i].state==301 || 
		P[i].state==320 || 
		P[i].state==321 || 
		P[i].state==322 || 
		P[i].state==329 || 
		P[i].state==331 || 
		P[i].state==550 || 
		P[i].state==551 
		){
			if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade)
			{
				P[i].cicloInteracoesGravidadeCont = 0;
				P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
				P[i].impulsoY--;
			}
			P[i].y += P[i].impulsoY;
			
			//deslocamento na vertical, durante o pulo neutro
			if(P[i].key_JOY_UP_status  >=1){ P[i].y--; P[i].ground--; }
			if(P[i].key_JOY_DOWN_status>=1){ P[i].y++; P[i].ground++; if(P[i].ground>255){ P[i].y--; P[i].ground--; } }
			
			//deslocamento na horizontal
			if(P[i].state==320 || P[i].state==321){ P[i].x += (P[i].velocidade*2)*P[i].direction; } //pulando para frente '320';'321'
			if(P[i].state==550){ P[i].x -= (P[i].velocidade*2)*P[i].direction; } //caindo '550'
			if(P[i].state==551){ P[i].x -= (P[i].velocidade)*P[i].direction; } //caindo '551'
			if(P[i].state==322)
			{ 
				if
				(
				(P[i].key_JOY_RIGHT_status>=1 && P[i].direction== 1) || 
				(P[i].key_JOY_LEFT_status >=1 && P[i].direction==-1)
				)
				{
					P[i].x += (P[i].velocidade*3)*P[i].direction; 	
				}
			} //pulando para frente '322'
		}
		
		//'505' choque; 509 pedacos caindo
		if(
		P[i].state==505 || 
		P[i].state==509
		){
			if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade)
			{
				P[i].cicloInteracoesGravidadeCont = 0;
				P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
				P[i].impulsoY--;
			}
			P[i].y += P[i].impulsoY;
			
			//deslocamento na vertical, durante o pulo neutro
			if(P[i].key_JOY_UP_status  >=1){ P[i].y--; P[i].ground--; }
			if(P[i].key_JOY_DOWN_status>=1){ P[i].y++; P[i].ground++; if(P[i].ground>255){ P[i].y--; P[i].ground--; } }
			
			//deslocamento na horizontal
			if(P[i].state==505 || P[i].state==509){ P[i].x += (P[i].velocidade*2)*(P[i].direction*-1); P[i].partBx+=(P[i].velocidade*2)*P[i].direction; } 
		}
		
		//(!!!)PROLONGANDO A QUEDA, AUMENTANDO SUA DURACAO E REDUZINDO O EFEITO DA GRAVIDADE (OPCIONAL)
		if(P[i].impulsoY>0)
		{
			if(
			P[i].state==300 || 
			P[i].state==301 || 
			P[i].state==320 || 
			P[i].state==321 || 
			P[i].state==322 || 
			P[i].state==329 || 
			P[i].state==331
			){
				P[i].impulsoY=P[i].impulsoY-(P[i].impulsoY/7);
			}
		}
		
		//PULOS E GOLPES AEREOS; QUEDAS > PISO
		if( 
		(
		P[i].state==300 || 
		P[i].state==301 || 
		P[i].state==320 || 
		P[i].state==321 || 
		P[i].state==322 || 
		P[i].state==329 || 
		P[i].state==331 || 
		P[i].state==505 || 
		P[i].state==550 
		) && P[i].y>P[i].ground)
		{
			P[i].y = P[i].ground;
			P[i].ground = 0;
			P[i].enableDashAirAtack = 0;
			P[i].gravidadeY = gravidadePadrao;
			if(P[i].state!=550)
			{
				PLAYER_STATE(i,606); //levantando
			}else{
				PLAYER_STATE(i,551); //quicando no chao
			}
		}
		
		//'551' > PISO
		if( P[i].state==551 && P[i].y>P[i].ground)
		{
			P[i].y = P[i].ground;
			P[i].ground = 0;
			P[i].enableDashAirAtack = 0;
			P[i].gravidadeY = gravidadePadrao;
			PLAYER_STATE(i,570); //caido
		}
		
		//'509' PARTES > PISO
		if( 
		(
		P[i].state==509
		) && P[i].y>P[i].ground)
		{
			P[i].y = P[i].ground;
			P[i].ground = 0;
			P[i].enableDashAirAtack = 0;
			P[i].gravidadeY = gravidadePadrao;
			PLAYER_STATE(i,510); 
		}
		
		//ANDANDO 
		if(P[i].state==420)
		{
			if(i==1){
				if(JOY1_UP   ) { if(P[i].y-P[i].velocidade > 0){ P[i].y-=P[i].velocidade; }else{ P[i].y = 0; } }
				if(JOY1_DOWN ) { if(P[i].y+P[i].velocidade < lvlHSize){ P[i].y+=P[i].velocidade; }else{ P[i].y = lvlHSize; } }
				if(JOY1_LEFT ) { if(P[i].x-P[i].velocidade > camPosX+margin){ P[i].x-=P[i].velocidade; }else{ P[i].x = camPosX+margin; } }
				if(JOY1_RIGHT) 
				{ 
					if(P[i].x+P[i].velocidade < lvlWSize-margin){ P[i].x+=P[i].velocidade; }else{ P[i].x = lvlWSize-margin; } 
					if(P[i].x+P[i].velocidade < camPosX+320-margin){  }else{ P[i].x = camPosX+320-margin; }
				}
			}
			if(i==2){
				if(JOY2_UP   ) { if(P[i].y-P[i].velocidade > 0){ P[i].y-=P[i].velocidade; }else{ P[i].y = 0; } }
				if(JOY2_DOWN ) { if(P[i].y+P[i].velocidade < lvlHSize){ P[i].y+=P[i].velocidade; }else{ P[i].y = lvlHSize; } }
				if(JOY2_LEFT ) { if(P[i].x-P[i].velocidade > camPosX+margin){ P[i].x-=P[i].velocidade; }else{ P[i].x = camPosX+margin; } }
				if(JOY2_RIGHT) 
				{ 
					if(P[i].x+P[i].velocidade < lvlWSize-margin){ P[i].x+=P[i].velocidade; }else{ P[i].x = lvlWSize-margin; } 
					if(P[i].x+P[i].velocidade < camPosX+320-margin){  }else{ P[i].x = camPosX+320-margin; }
				}
			}
		}
		
		//GOLPE DASH
		if(P[i].state==431)
		{
			//animFrame
			//frameTimeAtual
			u8 vel = 0;
			
			//direcoes, sao baseadas no teclado numerico
			// 7 8 9
			// 4 o 6
			// 1 2 3
			
			//define a direcao do movimento no inicio da animacao
			if(P[i].animFrame==1) 
			{
				if(i==1){
					if( JOY1_UP   && !JOY1_LEFT && !JOY1_RIGHT){ P[i].dashDir=8; }
					if( JOY1_UP   &&  JOY1_RIGHT              ){ P[i].dashDir=9; }
					if(!JOY1_UP   && !JOY1_DOWN &&  JOY1_RIGHT){ P[i].dashDir=6; }
					if( JOY1_DOWN &&  JOY1_RIGHT              ){ P[i].dashDir=3; }
					if(!JOY1_LEFT &&  JOY1_DOWN && !JOY1_RIGHT){ P[i].dashDir=2; }
					if( JOY1_LEFT &&  JOY1_DOWN               ){ P[i].dashDir=1; }
					if( JOY1_LEFT && !JOY1_DOWN && !JOY1_UP   ){ P[i].dashDir=4; }
					if( JOY1_LEFT &&  JOY1_UP                 ){ P[i].dashDir=7; }
					if( !JOY1_LEFT && !JOY1_RIGHT && !JOY1_UP && !JOY1_DOWN){ P[i].dashDir=0; }
				}
				if(i==2){
					if( JOY2_UP   && !JOY2_LEFT && !JOY2_RIGHT){ P[i].dashDir=8; }
					if( JOY2_UP   &&  JOY2_RIGHT              ){ P[i].dashDir=9; }
					if(!JOY2_UP   && !JOY2_DOWN &&  JOY2_RIGHT){ P[i].dashDir=6; }
					if( JOY2_DOWN &&  JOY2_RIGHT              ){ P[i].dashDir=3; }
					if(!JOY2_LEFT &&  JOY2_DOWN && !JOY2_RIGHT){ P[i].dashDir=2; }
					if( JOY2_LEFT &&  JOY2_DOWN               ){ P[i].dashDir=1; }
					if( JOY2_LEFT && !JOY2_DOWN && !JOY2_UP   ){ P[i].dashDir=4; }
					if( JOY2_LEFT &&  JOY2_UP                 ){ P[i].dashDir=7; }
					if( !JOY2_LEFT && !JOY2_RIGHT && !JOY2_UP && !JOY2_DOWN){ P[i].dashDir=0; }
				}
			}
			
			//Todos os outros chars menos o Ninja
			if(P[i].animFrame<=3){ vel=2; }
			if(P[i].animFrame==4){ 
				if(P[i].frameTimeAtual>= 1 && P[i].frameTimeAtual<10){ vel=2; }
				if(P[i].frameTimeAtual>=10 && P[i].frameTimeAtual<20){ vel=1; }
				if(P[i].frameTimeAtual>=20 && P[i].frameTimeAtual<30){ vel=0; }
			}
			
			//Ninja
			if(P[i].id==3)
			{
				if(P[i].animFrame==3){ 
					if(P[i].frameTimeAtual>= 1 && P[i].frameTimeAtual<10){ vel=2; }
					if(P[i].frameTimeAtual>=10 && P[i].frameTimeAtual<20){ vel=1; }
					if(P[i].frameTimeAtual>=20 && P[i].frameTimeAtual<30){ vel=0; }
				}
				if(P[i].animFrame==4){ vel=0; }
			}
			
			//movimenta na direcao definida
			if(P[i].dashDir==8){ P[i].y-=vel; }
			if(P[i].dashDir==9){ P[i].y-=vel; P[i].x+=vel; }
			if(P[i].dashDir==6){ P[i].x+=vel; }
			if(P[i].dashDir==3){ P[i].x+=vel; P[i].y+=vel; }
			if(P[i].dashDir==2){ P[i].y+=vel; }
			if(P[i].dashDir==1){ P[i].x-=vel; P[i].y+=vel; }
			if(P[i].dashDir==4){ P[i].x-=vel; }
			if(P[i].dashDir==7){ P[i].x-=vel; P[i].y-=vel; }
			
			//corrige posicao, limites da tela
			if(P[i].x-P[i].velocidade < camPosX +margin){ P[i].x = camPosX+margin; }
			if(P[i].x+P[i].velocidade > lvlWSize-margin){ P[i].x = lvlWSize-margin; }
			if(P[i].x+P[i].velocidade < camPosX+320-margin){  }else{ P[i].x = camPosX+320-margin; }
			if(P[i].y <   1){ P[i].y =   1; }
			if(P[i].y > lvlHSize){ P[i].y = lvlHSize; }
		}
		
	}
	
	//INIMIGOS
	
	for(u8 j=1; j<=MAX_ENEMYS-1; j++)
	{
		//SPECIAIS de "MUMMY" e "DOLG" ; CORRENDO
		if( 
			(E[j].state==199 && E[j].id==1) || 
			(E[j].state==199 && E[j].id==9) || 
			E[j].state==430)
		{
			if(j==1){
				if(JOY1_UP   ) { if(E[j].y-E[j].velocidade*2 > 0){ E[j].y-=E[j].velocidade*2; }else{ E[j].y = 0; } }
				if(JOY1_DOWN ) { if(E[j].y+E[j].velocidade*2 < lvlHSize){ E[j].y+=E[j].velocidade*2; }else{ E[j].y = lvlHSize; } }
				if(JOY1_LEFT ) { if(E[j].x-E[j].velocidade*2 > camPosX+margin){ E[j].x-=E[j].velocidade*2; }else{ E[j].x = camPosX+margin; } }
				if(JOY1_RIGHT) { if(E[j].x+E[j].velocidade*2 < lvlWSize-margin){ E[j].x+=E[j].velocidade*2; }else{ E[j].x = lvlWSize-margin; } }
			}
			if(j==2){
				if(JOY2_UP   ) { if(E[j].y-E[j].velocidade*2 > 0){ E[j].y-=E[j].velocidade*2; }else{ E[j].y = 0; } }
				if(JOY2_DOWN ) { if(E[j].y+E[j].velocidade*2 < lvlHSize){ E[j].y+=E[j].velocidade*2; }else{ E[j].y = lvlHSize; } }
				if(JOY2_LEFT ) { if(E[j].x-E[j].velocidade*2 > camPosX+margin){ E[j].x-=E[j].velocidade*2; }else{ E[j].x = camPosX+margin; } }
				if(JOY2_RIGHT) { if(E[j].x+E[j].velocidade*2 < lvlWSize-margin){ E[j].x+=E[j].velocidade*2; }else{ E[j].x = lvlWSize-margin; } }
			}
		}
		
		//PULOS E GOLPES AEREOS; 550; 551
		if(
		E[j].state==300 || 
		E[j].state==301 || 
		E[j].state==320 || 
		E[j].state==321 || 
		E[j].state==322 || 
		E[j].state==329 || 
		E[j].state==331 || 
		E[j].state==550 || 
		E[j].state==551 
		){
			if(E[j].cicloInteracoesGravidadeCont>=E[j].cicloInteracoesGravidade)
			{
				E[j].cicloInteracoesGravidadeCont = 0;
				E[j].impulsoY = E[j].impulsoY+E[j].gravidadeY;
				E[j].impulsoY--;
			}
			E[j].y += E[j].impulsoY;
			
			//deslocamento na vertical, durante o pulo neutro
			if(E[j].key_JOY_UP_status  >=1){ E[j].y--; E[j].ground--; }
			if(E[j].key_JOY_DOWN_status>=1){ E[j].y++; E[j].ground++; if(E[j].ground>255){ E[j].y--; E[j].ground--; } }
			
			//deslocamento na horizontal
			if(E[j].state==320 || E[j].state==321){ E[j].x += (E[j].velocidade*2)*E[j].direction; } //pulando para frente '320';'321'
			if(E[j].state==550){ E[j].x -= (E[j].velocidade*2)*E[j].direction; } //caindo '550'
			if(E[j].state==551){ E[j].x -= (E[j].velocidade)*E[j].direction; } //caindo '551'
			if(E[j].state==322)
			{ 
				if
				(
				(E[j].key_JOY_RIGHT_status>=1 && E[j].direction== 1) || 
				(E[j].key_JOY_LEFT_status >=1 && E[j].direction==-1)
				)
				{
					E[j].x += (E[j].velocidade*3)*E[j].direction; 	
				}
			} //pulando para frente '322'
		}
		
		//'505' choque; 509 pedacos caindo
		if(
		E[j].state==505 || 
		E[j].state==509
		){
			if(E[j].cicloInteracoesGravidadeCont>=E[j].cicloInteracoesGravidade)
			{
				E[j].cicloInteracoesGravidadeCont = 0;
				E[j].impulsoY = E[j].impulsoY+E[j].gravidadeY;
				E[j].impulsoY--;
			}
			E[j].y += E[j].impulsoY;
			
			//deslocamento na vertical, durante o pulo neutro
			if(E[j].key_JOY_UP_status  >=1){ E[j].y--; E[j].ground--; }
			if(E[j].key_JOY_DOWN_status>=1){ E[j].y++; E[j].ground++; if(E[j].ground>255){ E[j].y--; E[j].ground--; } }
			
			//deslocamento na horizontal
			if(E[j].state==505 || E[j].state==509){ E[j].x += (E[j].velocidade*2)*(E[j].direction*-1); E[j].partBx+=(E[j].velocidade*2)*E[j].direction; } 
		}
		
		//(!!!)PROLONGANDO A QUEDA, AUMENTANDO SUA DURACAO E REDUZINDO O EFEITO DA GRAVIDADE (OPCIONAL)
		if(E[j].impulsoY>0)
		{
			if(
			E[j].state==300 || 
			E[j].state==301 || 
			E[j].state==320 || 
			E[j].state==321 || 
			E[j].state==322 || 
			E[j].state==329 || 
			E[j].state==331
			){
				E[j].impulsoY=E[j].impulsoY-(E[j].impulsoY/7);
			}
		}
		
		//PULOS E GOLPES AEREOS; QUEDAS > PISO
		if( 
		(
		E[j].state==300 || 
		E[j].state==301 || 
		E[j].state==320 || 
		E[j].state==321 || 
		E[j].state==322 || 
		E[j].state==329 || 
		E[j].state==331 || 
		E[j].state==505 || 
		E[j].state==550 
		) && E[j].y>E[j].ground)
		{
			E[j].y = E[j].ground;
			E[j].ground = 0;
			E[j].enableDashAirAtack = 0;
			E[j].gravidadeY = gravidadePadrao;
			if(E[j].state!=550)
			{
				ENEMY_STATE(j,606); //levantando
			}else{
				ENEMY_STATE(j,551); //quicando no chao
			}
		}
		
		//'551' > PISO
		if( E[j].state==551 && E[j].y>E[j].ground)
		{
			E[j].y = E[j].ground;
			E[j].ground = 0;
			E[j].enableDashAirAtack = 0;
			E[j].gravidadeY = gravidadePadrao;
			ENEMY_STATE(j,570); //caido
		}
		
		//'509' PARTES > PISO
		if( 
		(
		E[j].state==509
		) && E[j].y>E[j].ground)
		{
			E[j].y = E[j].ground;
			E[j].ground = 0;
			E[j].enableDashAirAtack = 0;
			E[j].gravidadeY = gravidadePadrao;
			ENEMY_STATE(j,510); 
		}
		
		//ANDANDO 
		if(E[j].state==420)
		{
			if(E[j].key_JOY_UP_status   ==2) { if(E[j].y-E[j].velocidade > 0){ E[j].y-=E[j].velocidade; }else{ E[j].y = 0; } }
			if(E[j].key_JOY_DOWN_status ==2) { if(E[j].y+E[j].velocidade < lvlHSize){ E[j].y+=E[j].velocidade; }else{ E[j].y = lvlHSize; } }
			if(E[j].key_JOY_LEFT_status ==2) { if(E[j].x-E[j].velocidade > camPosX+margin){ E[j].x-=E[j].velocidade; }else{ E[j].x = camPosX+margin; } }
			if(E[j].key_JOY_RIGHT_status==2) { if(E[j].x+E[j].velocidade < lvlWSize-margin){ E[j].x+=E[j].velocidade; }else{ E[j].x = lvlWSize-margin; } }
		}
		
		//GOLPE DASH
		if(E[j].state==431)
		{
			//animFrame
			//frameTimeAtual
			u8 vel = 0;
			
			//direcoes, sao baseadas no teclado numerico
			// 7 8 9
			// 4 o 6
			// 1 2 3
			
			//define a direcao do movimento no inicio da animacao
			if(E[j].animFrame==1) 
			{
				//tem que arrumar aqui. está usando parametros dos players rsrs :)
				if(j==1){
					if( JOY1_UP   && !JOY1_LEFT && !JOY1_RIGHT){ E[j].dashDir=8; }
					if( JOY1_UP   &&  JOY1_RIGHT              ){ E[j].dashDir=9; }
					if(!JOY1_UP   && !JOY1_DOWN &&  JOY1_RIGHT){ E[j].dashDir=6; }
					if( JOY1_DOWN &&  JOY1_RIGHT              ){ E[j].dashDir=3; }
					if(!JOY1_LEFT &&  JOY1_DOWN && !JOY1_RIGHT){ E[j].dashDir=2; }
					if( JOY1_LEFT &&  JOY1_DOWN               ){ E[j].dashDir=1; }
					if( JOY1_LEFT && !JOY1_DOWN && !JOY1_UP   ){ E[j].dashDir=4; }
					if( JOY1_LEFT &&  JOY1_UP                 ){ E[j].dashDir=7; }
					if( !JOY1_LEFT && !JOY1_RIGHT && !JOY1_UP && !JOY1_DOWN){ E[j].dashDir=0; }
				}
				if(j==2){
					if( JOY2_UP   && !JOY2_LEFT && !JOY2_RIGHT){ E[j].dashDir=8; }
					if( JOY2_UP   &&  JOY2_RIGHT              ){ E[j].dashDir=9; }
					if(!JOY2_UP   && !JOY2_DOWN &&  JOY2_RIGHT){ E[j].dashDir=6; }
					if( JOY2_DOWN &&  JOY2_RIGHT              ){ E[j].dashDir=3; }
					if(!JOY2_LEFT &&  JOY2_DOWN && !JOY2_RIGHT){ E[j].dashDir=2; }
					if( JOY2_LEFT &&  JOY2_DOWN               ){ E[j].dashDir=1; }
					if( JOY2_LEFT && !JOY2_DOWN && !JOY2_UP   ){ E[j].dashDir=4; }
					if( JOY2_LEFT &&  JOY2_UP                 ){ E[j].dashDir=7; }
					if( !JOY2_LEFT && !JOY2_RIGHT && !JOY2_UP && !JOY2_DOWN){ E[j].dashDir=0; }
				}
			}
			
			//Todos os outros chars menos o Ninja
			if(E[j].animFrame<=3){ vel=2; }
			if(E[j].animFrame==4){ 
				if(E[j].frameTimeAtual>= 1 && E[j].frameTimeAtual<10){ vel=2; }
				if(E[j].frameTimeAtual>=10 && E[j].frameTimeAtual<20){ vel=1; }
				if(E[j].frameTimeAtual>=20 && E[j].frameTimeAtual<30){ vel=0; }
			}
			
			//Ninja
			if(E[j].id==3)
			{
				if(E[j].animFrame==3){ 
					if(E[j].frameTimeAtual>= 1 && E[j].frameTimeAtual<10){ vel=2; }
					if(E[j].frameTimeAtual>=10 && E[j].frameTimeAtual<20){ vel=1; }
					if(E[j].frameTimeAtual>=20 && E[j].frameTimeAtual<30){ vel=0; }
				}
				if(E[j].animFrame==4){ vel=0; }
			}
			
			//movimenta na direcao definida
			if(E[j].dashDir==8){ E[j].y-=vel; }
			if(E[j].dashDir==9){ E[j].y-=vel; E[j].x+=vel; }
			if(E[j].dashDir==6){ E[j].x+=vel; }
			if(E[j].dashDir==3){ E[j].x+=vel; E[j].y+=vel; }
			if(E[j].dashDir==2){ E[j].y+=vel; }
			if(E[j].dashDir==1){ E[j].x-=vel; E[j].y+=vel; }
			if(E[j].dashDir==4){ E[j].x-=vel; }
			if(E[j].dashDir==7){ E[j].x-=vel; E[j].y-=vel; }
			
			/*
			//corrige posicao, limites da tela
			if(E[j].x-E[j].velocidade < camPosX +margin){ E[j].x = camPosX+margin; }
			if(E[j].x+E[j].velocidade > lvlWSize-margin){ E[j].x = lvlWSize-margin; }
			if(E[j].y <   1){ E[j].y =   1; }
			if(E[j].y > lvlHSize){ E[j].y = lvlHSize; }
			*/
		}
		
	}
	
}

void FUNCAO_CAMERA(void)
{
	//AVANCO DE TELA! CONTROLE DE SCROOLING!
	//restringe o movimento na horizontal
	//de acordo com a quantidade de inimigos abatidos
	//a tela so se move se vc vencer os inimigos que estao em cena
	//é importante gerenciar essa quantidade em conjunto com a 
	//funcao 'FUNCAO_GERENCIAR_OBJS' que ira cria-los.
	//Entao vc cria os objetos inimigos lá, e restringe a rolagem aqui.
	if(faseAtual==1)
	{
		if(qtdeInimigosMortos==0 && lvlWSize!=160* 5){lvlWSize=160* 5;}
		if(qtdeInimigosMortos>=3 && lvlWSize!=160* 7){lvlWSize=160* 7;}
		if(qtdeInimigosMortos>=5 && lvlWSize!=2304){lvlWSize=2304;} //2304 é o tamanho da primeira fase do Captain Commando
	}
	
	//movimenta o BG
	VDP_setHorizontalScroll(BG_B, camPosX*-1);
	VDP_setVerticalScroll(BG_B, camPosY);
	
	//posicionar camera
		//camPosX = P[1].x-160; //desativado, para fazer o movimento de camera ser mais suave, durante reativacao da rolagem, este é automatico
	
	//X
	if(qtdePlayers==1)
	{
		if(
			(camPosX<P[1].x-160 && P[1].x-160-camPosX>=2) || 
			(camPosX>P[1].x-160 && camPosX-P[1].x-160>=2)
		)
		{
			if(camPosX<P[1].x-160){camPosX++;}else{camPosX--;}
		}
	}
	if(qtdePlayers==2)
	{
		s16 pontoMedio = 0;
		if(P[1].x<P[2].x){ pontoMedio = P[1].x+((P[2].x-P[1].x)/2); }
		if(P[1].x>P[2].x){ pontoMedio = P[2].x+((P[1].x-P[2].x)/2); }
		
		if(
			(camPosX<pontoMedio-160 && pontoMedio-160-camPosX>=2) || 
			(camPosX>pontoMedio-160 && camPosX-pontoMedio-160>=2)
		)
		{
			if(camPosX<pontoMedio-160){camPosX++;}else{camPosX--;}
		}
	}
	
	//Y
	if(qtdePlayers==1)
	{
		if(P[1].ground==0){camPosY = P[1].y-190;}else{camPosY = P[1].ground-190;}
	}
	if(qtdePlayers==2)
	{
		s16 pontoMedio = 0;
		if(P[1].y<P[2].y){ pontoMedio = P[1].y+((P[2].y-P[1].y)/2); }
		if(P[1].y>=P[2].y){ pontoMedio = P[2].y+((P[1].y-P[2].y)/2); }
		
		if(P[1].ground==0 && P[2].ground==0)
		{
			camPosY = pontoMedio-190;
		}else
		{
			if(P[1].ground==0)
			{  
				if(P[1].ground<P[2].y){ pontoMedio = P[1].ground+((P[2].y-P[1].ground)/2); }
				if(P[1].ground>P[2].y){ pontoMedio = P[2].y+((P[1].ground-P[2].y)/2); }
			}
			if(P[2].ground==0)
			{  
				if(P[1].y<P[2].ground){ pontoMedio = P[1].y+((P[2].ground-P[1].y)/2); }
				if(P[1].y>P[2].ground){ pontoMedio = P[2].ground+((P[1].y-P[2].ground)/2); }
			}
			if(P[1].ground!=0 && P[2].ground!=0)
			{
				if(P[1].ground<P[2].ground){ pontoMedio = P[1].ground+((P[2].ground-P[1].ground)/2); }
				if(P[1].ground>P[2].ground){ pontoMedio = P[2].ground+((P[1].ground-P[2].ground)/2); }
			}
			camPosY = P[1].ground-190;
		}
	}
	
	if( camPosY< 0 ){ camPosY= 0; } 
	if( camPosY>lvlHSize-224 ){ camPosY=lvlHSize-224; } 
	if( camPosX< 0 ){ camPosX= 0; } 
	if( camPosX>lvlWSize-320 ){ camPosX=lvlWSize-320; } 
	
	//Parede invisivel, limitador esquerdo de jogos Beat em Up
	if( P[1].x<camPosX+margin ){ P[1].x=camPosX+margin; }
	if( camPosX<LimiterX ){ camPosX=LimiterX; } 
}

void FUNCAO_FSM(void)
{
	//modificador dos estados dos players
	
	//MUDAR DIRECAO PLAYERS
	if( (P[1].state>=501 && P[1].state<=570) || P[1].state==550 || P[1].state==431 ){
		//dont change
	}else{
		if(JOY1_LEFT ){ SPR_setHFlip(P[1].sprite,  TRUE); P[1].direction=-1; }
		if(JOY1_RIGHT){ SPR_setHFlip(P[1].sprite, FALSE); P[1].direction= 1; }
	}
	if( (P[2].state>=501 && P[2].state<=570) || P[2].state==550 || P[2].state==431 ){
		//dont change
	}else{
		if(JOY2_LEFT ){ SPR_setHFlip(P[2].sprite,  TRUE); P[2].direction=-1; }
		if(JOY2_RIGHT){ SPR_setHFlip(P[2].sprite, FALSE); P[2].direction= 1; }
	}
	
	for(u8 i=1; i<=qtdePlayers; i++)
	{
		//ANDAR ou CORRER
		if(P[i].state==100) 
		{
			if( 
			P[i].key_JOY_UP_status>=1 || 
			P[i].key_JOY_DOWN_status>=1 || 
			P[i].key_JOY_LEFT_status>=1 || 
			P[i].key_JOY_RIGHT_status>=1 
			){ 
				
				if(P[i].key_JOY_UP_status   ==1){ if(P[i].key_JOY_countdown[8]>0){ PLAYER_STATE(i,430); }else{ PLAYER_STATE(i,420); } }
				if(P[i].key_JOY_DOWN_status ==1){ if(P[i].key_JOY_countdown[2]>0){ PLAYER_STATE(i,430); }else{ PLAYER_STATE(i,420); } }
				if(P[i].key_JOY_LEFT_status ==1){ if(P[i].key_JOY_countdown[4]>0){ PLAYER_STATE(i,430); }else{ PLAYER_STATE(i,420); } }
				if(P[i].key_JOY_RIGHT_status==1){ if(P[i].key_JOY_countdown[6]>0){ PLAYER_STATE(i,430); }else{ PLAYER_STATE(i,420); } }
				//ativacao de 'key_JOY_countdown'...
				if(P[i].key_JOY_UP_status   ==1){ P[i].key_JOY_countdown[8]=12; }
				if(P[i].key_JOY_LEFT_status ==1){ P[i].key_JOY_countdown[4]=12; }
				if(P[i].key_JOY_RIGHT_status==1){ P[i].key_JOY_countdown[6]=12; }
				if(P[i].key_JOY_DOWN_status ==1){ P[i].key_JOY_countdown[2]=12; }
			}
		}
		
		//temporizador de combos
		if(P[i].CountDownChain>0){ P[i].CountDownChain--; }
		if(P[i].CountDownChain==1){ P[i].activeHit = 0; } //intervalo entre golpes demorou muito, reinicia o chain combo
		
		//Para facilitar a criação de combos, coloque aqui as condicoes que ativam a proxima sequencia de golpes do combo
		//  Exemplo: Jogando com "Mummy" vc precisa apertar o botao a partir 
		//  do frame 3 (terceiro frame) da animacao 101 (soco inicial) para ativar a prox sequencia de golpes
		
		//--- ENABLE CHAIN SEQUENCE
		
		enableChainSequence = 0;
		
		//MUMMY CHAIN; id=1
		if( (P[i].id==1) && 
		(
			(P[i].state==101 && P[i].animFrame>=3) || 
			(P[i].state==102 && P[i].animFrame>=3) || 
			(P[i].state==103 && P[i].animFrame>=3) || 
			(P[i].state==202 && P[i].animFrame>=6) ||
			(P[i].state==203 && P[i].animFrame>=7) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//CAPTAIN COMMANDO CHAIN; id=2
		if( (P[i].id==2) && 
		(
			(P[i].state==101 && P[i].animFrame>=2) || 
			(P[i].state==102 && P[i].animFrame>=4) || 
			(P[i].state==103 && P[i].animFrame>=5) || 
			(P[i].state==202 && P[i].animFrame>=7) ||
			(P[i].state==203 && P[i].animFrame>=6) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//NINJA CHAIN; id=3
		if( (P[i].id==3) && 
		(
			(P[i].state==101 && P[i].animFrame>=4) || 
			(P[i].state==102 && P[i].animFrame>=5) || 
			(P[i].state==103 && P[i].animFrame>=8) || 
			(P[i].state==202 && P[i].animFrame>=6) ||
			(P[i].state==203 && P[i].animFrame>=7) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//BABY CHAIN; id=4
		if( (P[i].id==4) && 
		(
			(P[i].state==101 && P[i].animFrame>=2) || 
			(P[i].state==102 && P[i].animFrame>=4) || 
			(P[i].state==103 && P[i].animFrame>=4) || 
			(P[i].state==202 && P[i].animFrame>=8) ||
			(P[i].state==203 && P[i].animFrame>=8) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//ENEMY01 CHAIN; id=5
		if( (P[i].id==5) && 
		(
			(P[i].state==101 && P[i].animFrame>=2) || 
			(P[i].state==102 && P[i].animFrame>=5) || 
			(P[i].state==103 && P[i].animFrame>=2) || 
			(P[i].state==202 && P[i].animFrame>=5) || 
			(P[i].state==203 && P[i].animFrame>=2) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//ENEMY02 CHAIN; id=6
		if( (P[i].id==6) && 
		(
			(P[i].state==101 && P[i].animFrame>=2) || 
			(P[i].state==102 && P[i].animFrame>=5) || 
			(P[i].state==103 && P[i].animFrame>=2) || 
			(P[i].state==202 && P[i].animFrame>=5) || 
			(P[i].state==203 && P[i].animFrame>=2) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//ENEMY03 CHAIN; id=7
		if( (P[i].id==7) && 
		(
			(P[i].state==101 && P[i].animFrame>=2) || 
			(P[i].state==102 && P[i].animFrame>=5) || 
			(P[i].state==103 && P[i].animFrame>=2) || 
			(P[i].state==202 && P[i].animFrame>=5) || 
			(P[i].state==203 && P[i].animFrame>=2) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//ENEMY04 CHAIN; id=8
		if( (P[i].id==8) && 
		(
			(P[i].state==101 && P[i].animFrame>=6) || 
			(P[i].state==102 && P[i].animFrame>=6) || 
			(P[i].state==103 && P[i].animFrame>=6) || 
			(P[i].state==202 && P[i].animFrame>=6) || 
			(P[i].state==203 && P[i].animFrame>=6) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//ENEMY05 CHAIN; id=9
		if( (P[i].id==9) && 
		(
			(P[i].state==101 && P[i].animFrame>=5) || 
			(P[i].state==102 && P[i].animFrame>=5) || 
			(P[i].state==103 && P[i].animFrame>=5) || 
			(P[i].state==202 && P[i].animFrame>=5) || 
			(P[i].state==203 && P[i].animFrame>=5) 
		) )
		{
			enableChainSequence = 1;
		}
		
		//--- end ENABLE CHAIN SEQUENCE
		
		//SOCO4 CHAIN ; SOCO5 CHAIN+DIR[UP/DOWN] //( OBS: SOCO3 '103;203' acertou )
		if( enableChainSequence==1 && (P[i].state==103 || P[i].state==203) && P[i].activeHit==1 && P[i].key_JOY_B_status==1 ) 
		{
			if( P[i].key_JOY_UP_status>=1 || P[i].key_JOY_DOWN_status>=1 ){ 
				if
				( 
				  (P[i].key_JOY_LEFT_status>=1 && P[i].direction==-1) 
				  ||
				  (P[i].key_JOY_RIGHT_status>=1 && P[i].direction==1) 
				) 
				{ 
					//SOCO4<5> CHAIN+DIR
					PLAYER_STATE(i, 205); 
				}else{ 
					//SOCO4<5> CHAIN
					PLAYER_STATE(i, 105); 
				}
				if(P[i].key_JOY_LEFT_status==0 && P[i].key_JOY_RIGHT_status==0){ PLAYER_STATE(i, 105); } //SOCO4<5> CHAIN
			}else{ 
				if
				( 
				  (P[i].key_JOY_LEFT_status>=1 && P[i].direction==-1) 
				  ||
				  (P[i].key_JOY_RIGHT_status>=1 && P[i].direction==1) 
				) 
				{ 
					//SOCO4 CHAIN+DIR
					PLAYER_STATE(i, 204); 
				}else{ 
					//SOCO4 CHAIN
					PLAYER_STATE(i, 104); 
				}
				if(P[i].key_JOY_LEFT_status==0 && P[i].key_JOY_RIGHT_status==0){ PLAYER_STATE(i, 104); } //SOCO4 CHAIN
			}
		}
		
		//SOCO3 CHAIN ; SOCO3 CHAIN+DIR //( OBS: SOCO2 '102;202' acertou )
		if( enableChainSequence==1 && (P[i].state==102 || P[i].state==202) && P[i].activeHit==1 && P[i].key_JOY_B_status==1 ) 
		{
			if
			( 
			  (P[i].key_JOY_LEFT_status>=1 && P[i].direction==-1) 
			  ||
			  (P[i].key_JOY_RIGHT_status>=1 && P[i].direction==1) 
			) 
			{ 
				//SOCO3 CHAIN+DIR
				PLAYER_STATE(i, 203); 
			}else{ 
				//SOCO3 CHAIN
				PLAYER_STATE(i, 103); 
			}
		}
		
		//SOCO2 CHAIN ; SOCO2 CHAIN+DIR //( OBS: SOCO1 '101' acertou )
		if( enableChainSequence==1 && (P[i].state==101) && P[i].activeHit==1 && P[i].key_JOY_B_status==1 ) 
		{
			if
			( 
			  (P[i].key_JOY_LEFT_status>=1 && P[i].direction==-1) 
			  ||
			  (P[i].key_JOY_RIGHT_status>=1 && P[i].direction==1) 
			) 
			{ 
				//SOCO2 CHAIN+DIR
				PLAYER_STATE(i, 202); 
			}else{ 
				//SOCO2 CHAIN
				PLAYER_STATE(i, 102); 
			}
		}
		
		//SOCO1 RAPIDO (nao teve hit)
		if( P[i].state==101 && P[i].activeHit==0) 
		{
			//SOCO1 RAPIDO
			if( P[i].key_JOY_B_status==1 && P[i].animFrame>=3 && P[i].activeHit==0 ){ PLAYER_STATE(i, 101); } 
		}
		
		//SOCO1 ou JUMP
		if( (P[i].state==100) || (P[i].state==420) ) 
		{
			//SOCO1
			if( P[i].key_JOY_B_status==1 ){ PLAYER_STATE(i, 101); }
			//JUMP
			if( P[i].key_JOY_C_status==1 )
			{ 
				if(P[i].state==100){PLAYER_STATE(i, 300); }
				if(P[i].state==420)
				{
					if(P[i].key_JOY_LEFT_status>=1 || P[i].key_JOY_RIGHT_status>=1)
					{
						PLAYER_STATE(i, 320);
					}else
					{
						PLAYER_STATE(i, 300);
					}
				}
			}
		}
		
		//GOLPE DASH ou JUMP
		if( P[i].state==430 ) 
		{
			//SOCO1
			if( P[i].key_JOY_B_status==1 ){ PLAYER_STATE(i, 431); }
			//JUMP
			if( P[i].key_JOY_C_status==1 )
			{ 
				P[i].enableDashAirAtack = 1;
				if(P[i].key_JOY_LEFT_status>=1 || P[i].key_JOY_RIGHT_status>=1)
				{
					PLAYER_STATE(i, 320);
				}else
				{
					PLAYER_STATE(i, 300);
				}
				if(i==1){ FUNCAO_PLAY_SND(i,500,1); } //interrompe sons de passos
				if(i==2){ FUNCAO_PLAY_SND(i,500,2); } //interrompe sons de passos
			}
		}
		
		//SPECIAL
		if( (P[i].state==100 || P[i].state==420 || P[i].state==430) ||  (P[i].state>=101 && P[i].state<=198) || (P[i].state>=201 && P[i].state<=298) ) 
		{
			//SPECIAL
			if( P[i].key_JOY_A_status==1){ PLAYER_STATE(i, 199); }
		}
		
		//'301' SOCO AEREO NEUTRO ; '322' GOLPE AEREO DASH
		if( P[i].state==300 ) 
		{
			if( P[i].key_JOY_B_status==1 ){ 
				if(P[i].enableDashAirAtack==0)
				{
					//'301' SOCO AEREO NEUTRO
					PLAYER_STATE(i, 301); 
				}else{
					//'322' GOLPE AEREO DASH
					PLAYER_STATE(i, 322); 
				}
			}
		}
		
		//'321' SOCO AEREO HORIZONTAL ; '322' GOLPE AEREO DASH
		if( P[i].state==320 ) 
		{
			if( P[i].key_JOY_B_status==1 )
			{ 
				if(P[i].enableDashAirAtack==0)
				{
					//'321' SOCO AEREO HORIZONTAL
					PLAYER_STATE(i, 321); 
				}else{
					//'322' GOLPE AEREO DASH
					PLAYER_STATE(i, 322); 
				}
			}
		}
		
		//PARAR DE ANDAR ; PARAR DE CORRER
		if(P[i].state==420 || P[i].state==430) 
		{
			//PARAR '100'
			if(P[i].key_JOY_UP_status==0 && P[i].key_JOY_DOWN_status==0 && P[i].key_JOY_LEFT_status==0 && P[i].key_JOY_RIGHT_status==0)
			{ 
				PLAYER_STATE(i, 100); 
				if(i==1){ FUNCAO_PLAY_SND(i,500,1); } //interrompe sons de passos
				if(i==2){ FUNCAO_PLAY_SND(i,500,2); } //interrompe sons de passos
			}
			//metodo 2, desativado
			//if(i==1 && !JOY1_UP && !JOY1_DOWN && !JOY1_LEFT && !JOY1_RIGHT){ PLAYER_STATE(i, 100); }
			//if(i==2 && !JOY2_UP && !JOY2_DOWN && !JOY2_LEFT && !JOY2_RIGHT){ PLAYER_STATE(i, 100); }
		}
		
		//restringe movimento no eixo Y
		if(P[i].ground==0){ if(P[i].y<invisibleWallY){ P[i].y=invisibleWallY; } }
		
		
		//key_JOY_countdown é um timer regressivo, ativado quando se aperta algum botao direcional, usado para Corrida, entre outros...
		if(P[i].key_JOY_countdown[8]>0){ P[i].key_JOY_countdown[8]--; }
		if(P[i].key_JOY_countdown[2]>0){ P[i].key_JOY_countdown[2]--; }
		if(P[i].key_JOY_countdown[4]>0){ P[i].key_JOY_countdown[4]--; }
		if(P[i].key_JOY_countdown[6]>0){ P[i].key_JOY_countdown[6]--; }
	}
	
	//modificador dos estados dos inimigos
	for(u8 j=1; j<=MAX_ENEMYS-1; j++)
	{
		//ativador dos inimigos //'activeDistance' eh a distancia necessaria para a ativacao
		if(E[j].active==0 && ((P[1].x+activeDistance>E[j].x) || (P[2].x+activeDistance>E[j].x)) ){ E[j].active = 1; }
		if(E[j].energy==0){ E[j].active = 0; }
		
		if(E[j].active==1)
		{
			//IA DOS INIMIGOS AQUI
			//necessita melhor implementacao... solucao temporaria
			
			//sair do estado 700
			if(E[j].state==700){ E[j].state=100; }
			
			//eh necessario colocar algum controle aqui. um temporizador talvez.
			//no momento, ele nao fica parado, e ja comeca a andar imediatamente
			if(E[j].state==100){ E[j].state=420; }
			
			if( gPing10==1 && E[j].state==420 )
			{
				//muda a direcao dos inimigos
				if( (E[j].x<P[1].x) && E[j].direction==-1 ){ E[j].direction= 1; }else if( (E[j].x>P[1].x) && E[j].direction== 1 ){ E[j].direction=-1; }
				
				//movimenta os inimigos
				if(E[j].x<P[1].x){E[j].x++;}else{E[j].x--;}
				if(E[j].y<P[1].y){E[j].y++;}else{E[j].y--;}
			}
			
		}
		
		//inimigo partido ao meio por golpes cortantes (Capain Commando)
		if(E[j].receiveDamageType==4 && E[j].energy==0 && E[j].state==550)
		{
			ENEMY_STATE(j, 509);
		}
	}
	
	
}

void FUNCAO_GERENCIAR_OBJS(void)
{
	//Objetos usados no cenario
	
	if(faseAtual==1)
	{
		//CRIAR OBJS
		if(segmentLimiter==1)
		{
			/*
			GE03 = SPR_addSprite(&spr_sonic, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) );
			GE04 = SPR_addSprite(&spr_sonic_floor, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) );
			GE05 = SPR_addSprite(&spr_genesis_does, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) );
			*/
			GE06 = SPR_addSprite(&spr_smoke, 512, 256, TILE_ATTR(PAL0,FALSE,FALSE,FALSE) );
			if(GE06){ SPR_setDepth(GE06, 0); }
			
			go = SPR_addSprite(&spr_go, 320-(9*8), 64, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) );
			if(go){ SPR_setDepth(go, 0); }	
			
			CREATE_ENEMY(5, 100, 450, 180);
			CREATE_ENEMY(6, 700, 620, 170);
			CREATE_ENEMY(5, 700, 700, 150);
		}
		if(segmentLimiter==2)
		{
			/*
			GE01 = SPR_addSprite(&spr_tv1, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) );
			GE02 = SPR_addSprite(&spr_letreiro, 512, 256, TILE_ATTR(PAL2,FALSE,FALSE,FALSE) );
			*/
		}
		if(segmentLimiter==6)
		{
			CREATE_ENEMY(5, 701, 904, 225);
			CREATE_ENEMY(5, 701, 1096, 217);
		}		
		
		//EXCLUIR OBJS
		if(segmentLimiter==2)
		{
			SPR_releaseSprite(GE06);
		}
		if(segmentLimiter==3)
		{
			/*
			SPR_releaseSprite(GE03);
			SPR_releaseSprite(GE04);
			SPR_releaseSprite(GE05);
			*/
		}
		if(segmentLimiter==5)
		{
			/*
			SPR_releaseSprite(GE01);
			SPR_releaseSprite(GE02);
			*/
		}
		
	}
	
	if(faseAtual==1)
	{
		//CRIAR OBJS
		
		//EXCLUIR OBJS
	}
	
}

void FUNCAO_MOVER_OBJS(void)
{
	if(GE01!=NULL){ SPR_setPosition(GE01,   5-camPosX,  70-camPosY); }
	if(GE02!=NULL){ SPR_setPosition(GE02, 490-camPosX,  16-camPosY); }
	if(GE03!=NULL){ SPR_setPosition(GE03, 204-camPosX,   8-camPosY); }
	if(GE04!=NULL){ SPR_setPosition(GE04, 196-camPosX,  22-camPosY); }
	if(GE05!=NULL){ SPR_setPosition(GE05, 248-camPosX,   6-camPosY); }
	if(GE06!=NULL){ SPR_setPosition(GE06,  42-camPosX,  53-camPosY); }
}

void FUNCAO_SPRITE_POSITION(void)
{
	//Virtual Position
	//X
	if(P[1].x>= 0 && P[1].x<=160){ VirtualPositionX=0; }
	else if(P[1].x>160 && P[1].x<lvlWSize-160){ VirtualPositionX=P[1].x-160; }
	else if(P[1].x>=lvlWSize-160){ VirtualPositionX=lvlWSize-320; }
	//Y
	if(P[1].y>= 0 && P[1].y<=112){ VirtualPositionY=0; }
	else if(P[1].y>112 && P[1].y<lvlHSize-112){ VirtualPositionY=P[1].y-112; }
	else if(P[1].y>=lvlHSize-112){ VirtualPositionY=lvlHSize-224; }
	
	//Posicao dos Sprites
	if(P[1].sprite!=NULL)
	{
		u8 shakeX = 0;
		if(SHAKE_ON_HIT==1 && (P[1].state>=501 && P[1].state<=501) ){ shakeX=gPing4; } //Adiciona tremor ao sprite
		if(P[1].direction== 1){ SPR_setPosition(P[1].sprite, (P[1].x-P[1].axisX)-camPosX+shakeX, (P[1].y-P[1].axisY)-camPosY); }
		if(P[1].direction==-1){ SPR_setPosition(P[1].sprite, ((P[1].x-P[1].w)+P[1].axisX)-camPosX+shakeX, (P[1].y-P[1].axisY)-camPosY); }
		if(P[1].ground==0){ if(P[1].sprite){ SPR_setDepth(P[1].sprite, 257-P[1].y+1); } }
	}
	if(P[1].partB!=NULL)
	{
		if(P[1].direction== 1){ SPR_setPosition(P[1].partB, (P[1].x-P[1].axisX)-camPosX+P[1].partBx, (P[1].y-P[1].axisY-16)-camPosY); }
		if(P[1].direction==-1){ SPR_setPosition(P[1].partB, ((P[1].x-P[1].w)+P[1].axisX)-camPosX+P[1].partBx, (P[1].y-P[1].axisY-16)-camPosY); }
		if(P[1].ground==0){ if(P[1].partB){ SPR_setDepth(P[1].partB, 257-P[1].ground+1); } }
	}
	if(P[1].spark!=NULL)
	{
		//SPR_setPosition(P[1].spark, P[1].x-camPosX-16, P[1].y-camPosY-16);
		if(P[1].spark){ SPR_setDepth(P[1].spark, 0); }
	}
	
	if(P[2].sprite!=NULL)
	{
		u8 shakeX = 0;
		if(SHAKE_ON_HIT==1 && (P[2].state>=501 && P[2].state<=501) ){ shakeX=gPing4; } //Adiciona tremor ao sprite
		if(P[2].direction== 1){ SPR_setPosition(P[2].sprite, (P[2].x-P[2].axisX)-camPosX+shakeX, (P[2].y-P[2].axisY)-camPosY); }
		if(P[2].direction==-1){ SPR_setPosition(P[2].sprite, ((P[2].x-P[2].w)+P[2].axisX)-camPosX+shakeX, (P[2].y-P[2].axisY)-camPosY); }
		if(P[2].ground==0){ if(P[2].sprite){ SPR_setDepth(P[2].sprite, 257-P[2].y+1); } }
	}
	if(P[2].partB!=NULL)
	{
		if(P[2].direction== 1){ SPR_setPosition(P[2].partB, (P[2].x-P[2].axisX)-camPosX+P[2].partBx, (P[2].y-P[2].axisY-16)-camPosY); }
		if(P[2].direction==-1){ SPR_setPosition(P[2].partB, ((P[2].x-P[2].w)+P[2].axisX)-camPosX+P[2].partBx, (P[2].y-P[2].axisY-16)-camPosY); }
		if(P[2].ground==0){ if(P[2].partB){ SPR_setDepth(P[2].partB, 257-P[2].ground+1); } }
	}
	if(P[2].spark!=NULL)
	{
		//SPR_setPosition(P[2].spark, P[2].x-camPosX-16, P[2].y-camPosY-16);
		if(P[2].spark){ SPR_setDepth(P[2].spark, 0); }
	}
	
	for(u8 j=1; j<=MAX_ENEMYS-1; j++)
	{
		if(E[j].sprite!=NULL)
		{
			u8 shakeX = 0;
			if(SHAKE_ON_HIT==1 && (E[j].state>=501 && E[j].state<=501) ){ shakeX=gPing4; } //Adiciona tremor ao sprite
			if(E[j].direction== 1){ SPR_setPosition(E[j].sprite, (E[j].x-E[j].axisX)-camPosX+shakeX, (E[j].y-E[j].axisY)-camPosY); }
			if(E[j].direction==-1){ SPR_setPosition(E[j].sprite, ((E[j].x-E[j].w)+E[j].axisX)-camPosX+shakeX, (E[j].y-E[j].axisY)-camPosY); }
			if(E[j].ground==0){ if(E[j].sprite){ SPR_setDepth(E[j].sprite, 257-E[j].y+1); } }
		}
		if(E[j].partB!=NULL)
		{
			if(E[j].direction== 1){ SPR_setPosition(E[j].partB, (E[j].x-E[j].axisX)-camPosX+E[j].partBx, (E[j].y-E[j].axisY-16)-camPosY); }
			if(E[j].direction==-1){ SPR_setPosition(E[j].partB, ((E[j].x-E[j].w)+E[j].axisX)-camPosX+E[j].partBx, (E[j].y-E[j].axisY-16)-camPosY); }
			if(E[j].ground==0){ if(E[j].partB){ SPR_setDepth(E[j].partB, 257-E[j].ground+1); } }
		}
		if(E[j].spark!=NULL)
		{
			//SPR_setPosition(E[j].spark, (E[j].sparkX), (E[j].sparkY) );
			//if(E[j].spark){ SPR_setDepth(E[j].spark, 0); }
		}
	}
	
	if(point!=NULL && show_debug==1)
	{
		SPR_setPosition(point, P[1].x-camPosX-4, P[1].y-camPosY-4);
		if(point){ SPR_setDepth(point, 257-P[1].y); }
	}	
	
	//Enemys
	if(E[1].sprite!=NULL)
	{
		if(E[1].direction== 1){ SPR_setPosition(E[1].sprite, (E[1].x-E[1].axisX)-camPosX, (E[1].y-E[1].axisY)-camPosY); }
		if(E[1].direction==-1){ SPR_setPosition(E[1].sprite, ((E[1].x-E[1].w)+E[1].axisX)-camPosX, (E[1].y-E[1].axisY)-camPosY); }
		if(E[1].sprite){ SPR_setDepth(E[1].sprite, 257-E[1].y); }
	}
}

void FUNCAO_CTRL_SLOTS_BGS(void)
{
	s8 updateSlot = 0; //valor 1 (atualiza bg a direita) ; valor -1 (atualiza bg a esquerda)
			
	if(frames>=5)
	{
		u16 centerCamPos = camPosX;
		
		if( (centerCamPos==0 || centerCamPos>0) && centerCamPos< 160 && segmentInFocus!=1)
		{ 
			if(segmentInFocus== 0){updateSlot= 1;}
			//if(segmentInFocus== 2){updateSlot=-1;} //nao existe o segmento 0, nao atualizar
			segmentInFocus= 1; 
		}
		else if(centerCamPos>= 160 && centerCamPos< 320 && segmentInFocus!=2)
		{ 
			if(segmentInFocus== 1){updateSlot= 1;}
			if(segmentInFocus== 3){updateSlot=-1;}
			segmentInFocus= 2;
		}
		else if(centerCamPos>= 320 && centerCamPos< 480 && segmentInFocus!=3)
		{ 
			if(segmentInFocus== 2){updateSlot= 1;}
			if(segmentInFocus== 4){updateSlot=-1;}
			segmentInFocus= 3; 
		}
		else if(centerCamPos>= 480 && centerCamPos< 640 && segmentInFocus!=4)
		{ 
			if(segmentInFocus== 3){updateSlot= 1;}
			if(segmentInFocus== 5){updateSlot=-1;}
			segmentInFocus= 4; 
		}
		else if(centerCamPos>= 640 && centerCamPos< 800 && segmentInFocus!=5)
		{ 
			if(segmentInFocus== 4){updateSlot= 1;}
			if(segmentInFocus== 6){updateSlot=-1;}
			segmentInFocus= 5; 
		}
		else if(centerCamPos>= 800 && centerCamPos< 960 && segmentInFocus!=6)
		{ 
			if(segmentInFocus== 5){updateSlot= 1;}
			if(segmentInFocus== 7){updateSlot=-1;}
			segmentInFocus= 6; 
		}
		else if(centerCamPos>= 960 && centerCamPos<1120 && segmentInFocus!=7)
		{ 
			if(segmentInFocus== 6){updateSlot= 1;}
			if(segmentInFocus== 8){updateSlot=-1;}
			segmentInFocus= 7; 
		}
		else if(centerCamPos>=1120 && centerCamPos<1280 && segmentInFocus!=8)
		{ 
			if(segmentInFocus== 7){updateSlot= 1;}
			if(segmentInFocus== 9){updateSlot=-1;}
			segmentInFocus= 8; 
		}
		else if(centerCamPos>=1280 && centerCamPos<1440 && segmentInFocus!=9)
		{ 
			if(segmentInFocus== 8){updateSlot= 1;}
			if(segmentInFocus==10){updateSlot=-1;}
			segmentInFocus= 9; 
		}
		else if(centerCamPos>=1440 && centerCamPos<1600 && segmentInFocus!=10)
		{ 
			if(segmentInFocus== 9){updateSlot= 1;}
			if(segmentInFocus==11){updateSlot=-1;}
			segmentInFocus=10; 
		}
		else if(centerCamPos>=1600 && centerCamPos<1760 && segmentInFocus!=11)
		{ 
			if(segmentInFocus==10){updateSlot= 1;}
			if(segmentInFocus==12){updateSlot=-1;}
			segmentInFocus=11; 
		}
		else if(centerCamPos>=1760 && centerCamPos<1920 && segmentInFocus!=12)
		{ 
			if(segmentInFocus==11){updateSlot= 1;}
			if(segmentInFocus==13){updateSlot=-1;}
			segmentInFocus=12; 
		}
		else if(centerCamPos>=1920 && centerCamPos<2080 && segmentInFocus!=13)
		{ 
			if(segmentInFocus==12){updateSlot= 1;}
			if(segmentInFocus==14){updateSlot=-1;}
			segmentInFocus=13; 
		}
		else if(centerCamPos>=2080 && centerCamPos<2240 && segmentInFocus!=14)
		{ 
			if(segmentInFocus==13){updateSlot= 1;}
			//if(segmentInFocus==15){updateSlot=-1;} //este cenario nao tem mais que 14 segmentos, nao atualizar a esquerda
			segmentInFocus=14; 
		}
		
		//---a seguir o codigo que deve ser usado para cenarios ainda maiores (exemplo: Turtles S.R.)
		/*
		else if(centerCamPos>=2080 && centerCamPos<2240 && segmentInFocus!=14)
		{ 
			if(segmentInFocus==13){updateSlot= 1;}
			if(segmentInFocus==15){updateSlot=-1;}
			segmentInFocus=14; 
		}
		else if(centerCamPos>=2240 && centerCamPos<2400 && segmentInFocus!=15)
		{ 
			if(segmentInFocus==14){updateSlot= 1;}
			if(segmentInFocus==16){updateSlot=-1;}
			segmentInFocus=15; 
		}
		else if(centerCamPos>=2400 && centerCamPos<2560 && segmentInFocus!=16)
		{ 
			if(segmentInFocus==15){updateSlot= 1;}
			if(segmentInFocus==17){updateSlot=-1;}
			segmentInFocus=16; 
		}
		else if(centerCamPos>=2560 && centerCamPos<2720 && segmentInFocus!=17)
		{ 
			if(segmentInFocus==16){updateSlot= 1;}
			if(segmentInFocus==18){updateSlot=-1;}
			segmentInFocus=17; 
		}
		else if(centerCamPos>=2720 && centerCamPos<2880 && segmentInFocus!=18)
		{ 
			if(segmentInFocus==17){updateSlot= 1;}
			if(segmentInFocus==19){updateSlot=-1;}
			segmentInFocus=18; 
		}
		else if(centerCamPos>=2880 && centerCamPos<3040 && segmentInFocus!=19)
		{ 
			if(segmentInFocus==18){updateSlot= 1;}
			if(segmentInFocus==20){updateSlot=-1;}
			segmentInFocus=19; 
		}
		else if(centerCamPos>=3040 && centerCamPos<3200 && segmentInFocus!=20)
		{ 
			if(segmentInFocus==19){updateSlot= 1;}
			if(segmentInFocus==21){updateSlot=-1;}
			segmentInFocus=20; 
		}
		else if(centerCamPos>=3200 && centerCamPos<3360 && segmentInFocus!=21)
		{ 
			if(segmentInFocus==20){updateSlot= 1;}
			if(segmentInFocus==22){updateSlot=-1;}
			segmentInFocus=21; 
		}
		else if(centerCamPos>=3360 && centerCamPos<3520 && segmentInFocus!=22)
		{ 
			if(segmentInFocus==21){updateSlot= 1;}
			if(segmentInFocus==23){updateSlot=-1;}
			segmentInFocus=22; 
		}
		else if(centerCamPos>=3520 && centerCamPos<3680 && segmentInFocus!=23)
		{ 
			if(segmentInFocus==22){updateSlot= 1;}
			if(segmentInFocus==24){updateSlot=-1;}
			segmentInFocus=23; 
		}
		else if(centerCamPos>=3680 && centerCamPos<3840 && segmentInFocus!=24)
		{ 
			if(segmentInFocus==23){updateSlot= 1;}
			if(segmentInFocus==25){updateSlot=-1;}
			segmentInFocus=24; 
		}
		else if(centerCamPos>=3840 && centerCamPos<4000 && segmentInFocus!=25)
		{ 
			if(segmentInFocus==24){updateSlot= 1;}
			if(segmentInFocus==26){updateSlot=-1;}
			segmentInFocus=25; 
		}
		else if(centerCamPos>=4000 && centerCamPos<4160 && segmentInFocus!=26)
		{ 
			if(segmentInFocus==25){updateSlot= 1;}
			if(segmentInFocus==27){updateSlot=-1;}
			segmentInFocus=26; 
		}
		else if(centerCamPos>=4160 && centerCamPos<4320 && segmentInFocus!=27)
		{ 
			if(segmentInFocus==26){updateSlot= 1;}
			if(segmentInFocus==28){updateSlot=-1;}
			segmentInFocus=27; 
		}
		else if(centerCamPos>=4320 && centerCamPos<4480 && segmentInFocus!=28)
		{ 
			if(segmentInFocus==27){updateSlot= 1;}
			if(segmentInFocus==29){updateSlot=-1;}
			segmentInFocus=28; 
		}
		else if(centerCamPos>=4480 && centerCamPos<4640 && segmentInFocus!=29)
		{ 
			if(segmentInFocus==28){updateSlot= 1;}
			if(segmentInFocus==30){updateSlot=-1;}
			segmentInFocus=29; 
		}
		else if(centerCamPos>=4640 && centerCamPos<4800 && segmentInFocus!=30)
		{ 
			if(segmentInFocus==29){updateSlot= 1;}
			if(segmentInFocus==31){updateSlot=-1;}
			segmentInFocus=30; 
		}
		else if(centerCamPos>=4800 && centerCamPos<4960 && segmentInFocus!=31)
		{ 
			if(segmentInFocus==30){updateSlot= 1;}
			if(segmentInFocus==32){updateSlot=-1;}
			segmentInFocus=31; 
		}
		else if(centerCamPos>=4960 && centerCamPos<5120 && segmentInFocus!=32)
		{ 
			if(segmentInFocus==31){updateSlot= 1;}
			if(segmentInFocus==33){updateSlot=-1;}
			segmentInFocus=32; 
		}
		else if(centerCamPos>=5120 && centerCamPos<5280 && segmentInFocus!=33)
		{ 
			if(segmentInFocus==32){updateSlot= 1;}
			if(segmentInFocus==34){updateSlot=-1;}
			segmentInFocus=33; 
		}
		else if(centerCamPos>=5280 && centerCamPos<5440 && segmentInFocus!=34)
		{ 
			if(segmentInFocus==33){updateSlot= 1;}
			if(segmentInFocus==35){updateSlot=-1;}
			segmentInFocus=34; 
		}
		else if(centerCamPos>=5440 && centerCamPos<5600 && segmentInFocus!=35)
		{ 
			if(segmentInFocus==34){updateSlot= 1;}
			if(segmentInFocus==36){updateSlot=-1;}
			segmentInFocus=35; 
		}
		else if(centerCamPos>=5600 && centerCamPos<5760 && segmentInFocus!=36)
		{ 
			if(segmentInFocus==35){updateSlot= 1;}
			if(segmentInFocus==37){updateSlot=-1;}
			segmentInFocus=36; 
		}
		else if(centerCamPos>=5760 && centerCamPos<5920 && segmentInFocus!=37)
		{ 
			if(segmentInFocus==36){updateSlot= 1;}
			if(segmentInFocus==38){updateSlot=-1;}
			segmentInFocus=37; 
		}
		else if(centerCamPos>=5920 && centerCamPos<6080 && segmentInFocus!=38)
		{ 
			if(segmentInFocus==37){updateSlot= 1;}
			if(segmentInFocus==39){updateSlot=-1;}
			segmentInFocus=38; 
		}
		else if(centerCamPos>=6080 && centerCamPos<6240 && segmentInFocus!=39)
		{ 
			if(segmentInFocus==38){updateSlot= 1;}
			if(segmentInFocus==40){updateSlot=-1;}
			segmentInFocus=39; 
		}
		else if(centerCamPos>=6240 && centerCamPos<6400 && segmentInFocus!=40)
		{ 
			if(segmentInFocus==39){updateSlot= 1;}
			if(segmentInFocus==41){updateSlot=-1;}
			segmentInFocus=40; 
		}
		else if(centerCamPos>=6400 && centerCamPos<6560 && segmentInFocus!=41)
		{ 
			if(segmentInFocus==40){updateSlot= 1;}
			if(segmentInFocus==42){updateSlot=-1;}
			segmentInFocus=41; 
		}
		else if(centerCamPos>=6560 && centerCamPos<6720 && segmentInFocus!=42)
		{ 
			if(segmentInFocus==41){updateSlot= 1;}
			if(segmentInFocus==43){updateSlot=-1;}
			segmentInFocus=42; 
		}
		else if(centerCamPos>=6720 && centerCamPos<6880 && segmentInFocus!=43)
		{ 
			if(segmentInFocus==42){updateSlot= 1;}
			if(segmentInFocus==44){updateSlot=-1;}
			segmentInFocus=43; 
		}
		else if(centerCamPos>=6880 && centerCamPos<7040 && segmentInFocus!=44)
		{ 
			if(segmentInFocus==43){updateSlot= 1;}
			if(segmentInFocus==45){updateSlot=-1;}
			segmentInFocus=44; 
		}
		*/
		//---
		
		if(segmentLimiter<segmentInFocus){segmentLimiter=segmentInFocus;}else{updateSlot=0;}
	}
	
	//Atualiza o Slot de Cenario
	if(updateSlot!=0)
	{
		if(updateSlot==1 && faseAtual==1)
		{
			if(frames==0){gScrollValue+=20;} //incremento inicial no frame 0
			if(segmentInFocus==1)
			{
				//SEGMENTO3
				VDP_loadTileSet(map0103.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0103.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0103.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==2)
			{
				//SEGMENTO4
				VDP_loadTileSet(map0104.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0104.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0104.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==3)
			{
				//SEGMENTO5
				VDP_loadTileSet(map0105.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0105.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0105.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==4)
			{
				//SEGMENTO6
				VDP_loadTileSet(map0106.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0106.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0106.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==5)
			{
				//SEGMENTO7
				VDP_loadTileSet(map0107.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0107.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0107.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==6)
			{
				//SEGMENTO8
				VDP_loadTileSet(map0108.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0108.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0108.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==7)
			{
				//SEGMENTO9
				VDP_loadTileSet(map0109.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0109.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0109.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==8)
			{
				//SEGMENTO10
				VDP_loadTileSet(map0110.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0110.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0110.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==9)
			{
				//SEGMENTO11
				VDP_loadTileSet(map0111.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0111.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0111.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==10)
			{
				//SEGMENTO12
				VDP_loadTileSet(map0112.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0112.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0112.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==11)
			{
				//SEGMENTO13
				VDP_loadTileSet(map0113.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0113.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0113.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==12)
			{
				//SEGMENTO14
				VDP_loadTileSet(map0114.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0114.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0114.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==13)
			{
				//SEGMENTO15
				VDP_loadTileSet(map0115.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0115.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0115.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==14)
			{
				//...Captain Commando Fase 1 termina aqui
			}
			
			//--- a seguir o codigo que deve ser usado para cenarios ainda maiores (exemplo: Turtles S.R.)
			/*
			if(segmentInFocus==14)
			{
				//SEGMENTO16
				VDP_loadTileSet(map0116.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0116.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0116.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==15)
			{
				//SEGMENTO17
				VDP_loadTileSet(map0117.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0117.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0117.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==16)
			{
				//SEGMENTO18
				VDP_loadTileSet(map0118.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0118.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0118.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==17)
			{
				//SEGMENTO19
				VDP_loadTileSet(map0119.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0119.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0119.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==18)
			{
				//SEGMENTO20
				VDP_loadTileSet(map0120.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0120.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0120.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==19)
			{
				//SEGMENTO21
				VDP_loadTileSet(map0121.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0121.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0121.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==20)
			{
				//SEGMENTO22
				VDP_loadTileSet(map0122.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0122.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0122.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==21)
			{
				//SEGMENTO23
				VDP_loadTileSet(map0123.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0123.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0123.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==22)
			{
				//SEGMENTO24
				VDP_loadTileSet(map0124.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0124.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0124.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==23)
			{
				//SEGMENTO25
				VDP_loadTileSet(map0125.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0125.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0125.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==24)
			{
				//SEGMENTO26
				VDP_loadTileSet(map0126.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0126.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0126.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==25)
			{
				//SEGMENTO27
				VDP_loadTileSet(map0127.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0127.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0127.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==26)
			{
				//SEGMENTO28
				VDP_loadTileSet(map0128.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0128.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0128.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==27)
			{
				//SEGMENTO29
				VDP_loadTileSet(map0129.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0129.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0129.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==28)
			{
				//SEGMENTO30
				VDP_loadTileSet(map0130.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0130.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0130.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==29)
			{
				//SEGMENTO31
				VDP_loadTileSet(map0131.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0131.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0131.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==30)
			{
				//SEGMENTO32
				VDP_loadTileSet(map0132.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0132.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0132.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==31)
			{
				//SEGMENTO33
				VDP_loadTileSet(map0133.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0133.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0133.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==32)
			{
				//SEGMENTO34
				VDP_loadTileSet(map0134.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0134.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0134.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==33)
			{
				//SEGMENTO35
				VDP_loadTileSet(map0135.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0135.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0135.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==34)
			{
				//SEGMENTO36
				VDP_loadTileSet(map0136.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0136.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0136.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==35)
			{
				//SEGMENTO37
				VDP_loadTileSet(map0137.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0137.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0137.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==36)
			{
				//SEGMENTO38
				VDP_loadTileSet(map0138.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0138.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0138.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==37)
			{
				//SEGMENTO39
				VDP_loadTileSet(map0139.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0139.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0139.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==38)
			{
				//SEGMENTO040
				VDP_loadTileSet(map0140.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0140.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0140.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==39)
			{
				//SEGMENTO41
				VDP_loadTileSet(map0141.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0141.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0141.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==40)
			{
				//SEGMENTO42
				VDP_loadTileSet(map0142.tileset,0x02A8,DMA); 
				VDP_setTileMapEx(BG_B,map0142.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0142.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==41)
			{
				//SEGMENTO043
				VDP_loadTileSet(map0143.tileset,0x0001,DMA); 
				VDP_setTileMapEx(BG_B,map0143.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0143.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==42)
			{
				//SEGMENTO44
				VDP_loadTileSet(map0144.tileset,0x0154,DMA); 
				VDP_setTileMapEx(BG_B,map0144.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0144.palette->data);
				gScrollValue+=20;
			}
			*/
			//---
			
			//ATUALIZA OBJS DE CENARIO
			FUNCAO_GERENCIAR_OBJS();
		}
		
		if(updateSlot==1 && faseAtual==2)
		{
			if(frames==0){gScrollValue+=20;}
			if(segmentInFocus==1)
			{
				//SEGMENTO3
				VDP_loadTileSet(map0203.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0203.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0203.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==2)
			{
				//SEGMENTO4
				VDP_loadTileSet(map0204.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0204.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0204.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==3)
			{
				//SEGMENTO5
				VDP_loadTileSet(map0205.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0205.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0205.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==4)
			{
				//SEGMENTO6
				VDP_loadTileSet(map0206.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0206.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0206.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==5)
			{
				//SEGMENTO7
				VDP_loadTileSet(map0207.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0207.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0207.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==6)
			{
				//SEGMENTO8
				VDP_loadTileSet(map0208.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0208.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0208.palette->data);
				gScrollValue+=20;
			}
			if(segmentInFocus==7)
			{
				//...
			}
			
			//ATUALIZA OBJS DE CENARIO
			FUNCAO_GERENCIAR_OBJS();
		}
		
		
		//ATUALIZACAO A ESQUERDA (AINDA) NAO FUNCIONAL, E PORTANTO, DESABILITADA
		/*if(updateSlot==-1)
		{
			if(segmentInFocus==2)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO1
				VDP_loadTileSet(map0101.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0101.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0101.palette->data);
			}
			if(segmentInFocus==3)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO2
				VDP_loadTileSet(map0102.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0102.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0102.palette->data);
			}
			if(segmentInFocus==4)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO3
				VDP_loadTileSet(map0103.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0103.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0103.palette->data);
			}
			if(segmentInFocus==5)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO4
				VDP_loadTileSet(map0104.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0104.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0104.palette->data);
			}
			if(segmentInFocus==6)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO5
				VDP_loadTileSet(map0105.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0105.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0105.palette->data);
			}
			if(segmentInFocus==7)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO6
				VDP_loadTileSet(map0106.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0106.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0106.palette->data);
			}
			if(segmentInFocus==8)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO7
				VDP_loadTileSet(map0107.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0107.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0107.palette->data);
			}
			if(segmentInFocus==9)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO8
				VDP_loadTileSet(map0108.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0108.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0108.palette->data);
			}
			if(segmentInFocus==10)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO9
				VDP_loadTileSet(map0109.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0109.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0109.palette->data);
			}
			if(segmentInFocus==11)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO10
				VDP_loadTileSet(map0110.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0110.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0110.palette->data);
			}
			if(segmentInFocus==12)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO11
				VDP_loadTileSet(map0111.tileset,0x0154,DMA); //0x0154 = 340 dec
				VDP_setTileMapEx(BG_B,map0111.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x0154),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0111.palette->data);
			}
			if(segmentInFocus==13)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO12
				VDP_loadTileSet(map0112.tileset,0x02A8,DMA); //0x02A8 = 680 dec
				VDP_setTileMapEx(BG_B,map0112.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,0x02A8),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL1, map0112.palette->data);
			}
			if(segmentInFocus==14)
			{
				gScrollValue=(segmentInFocus*20)-20;
				//SEGMENTO13
				VDP_loadTileSet(map0113.tileset,0x0001,DMA); //0x0000 = 0 dec
				VDP_setTileMapEx(BG_B,map0113.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0x0001),gScrollValue,0,0,0,20,32,DMA_QUEUE);
				VDP_setPalette(PAL0, map0113.palette->data);
			}
		}
		*/
	}

}

void FUNCAO_ANIMACAO(void) //controles de animacao do Player e Enemys
{
	//desliga a visibilidade das infos dos inimigos atingidos
	if(P[1].CountDownEnemyInfo>0){P[1].CountDownEnemyInfo--;}
	if(P[2].CountDownEnemyInfo>0){P[2].CountDownEnemyInfo--;}
	if(P[1].CountDownEnemyInfo==1)
	{
		SPR_setVisibility(spr_face_slot3, HIDDEN); 
		SPR_setVisibility(spr_energy_slot3, HIDDEN);
	}	
	if(P[2].CountDownEnemyInfo==1)
	{ 
		SPR_setVisibility(spr_face_slot4, HIDDEN); 
		SPR_setVisibility(spr_energy_slot4, HIDDEN);
	}
	
	//PLAYERS
	for(u8 i=1; i<=qtdePlayers; i++)
	{
		P[i].frameTimeAtual++; 
		if(P[i].frameTimeAtual>P[i].frameTimeTotal) //hora de trocar o frame!
		{
			P[i].animFrame++;
			if(P[i].animFrame>P[i].animFrameTotal) //hora de trocar ou recarregar a animacao!
			{
				if(P[i].state==100){ PLAYER_STATE(i,100); }
				// estes aqui previnem bug, caso o tempo de animacao dos Aereos seja definido errado e o player nao tenha tempo de chegar ao solo...
				if(P[i].state==300 || P[i].state==320 || P[i].state==301 || P[i].state==321 || P[i].state==322 || P[i].state==328){ PLAYER_STATE(i,100); }
				if( (P[i].state>=101 && P[i].state<=299) || P[i].state==431 || P[i].state==606)				
				{ 
					if( P[i].key_JOY_UP_status>=1 || P[i].key_JOY_DOWN_status>=1 || P[i].key_JOY_LEFT_status>=1 || P[i].key_JOY_RIGHT_status>=1 )
					{
						PLAYER_STATE(i,420); 
					}else{
						PLAYER_STATE(i,100); 
					}
				}
				if(P[i].state==199){ PLAYER_STATE(i,100); }
				if(P[i].state==420){ PLAYER_STATE(i,420); }
				if(P[i].state==430){ PLAYER_STATE(i,430); }
				//hits
				if(P[i].state==501){ PLAYER_STATE(i,100); }
				if(P[i].state==502){ PLAYER_STATE(i,100); }
				if(P[i].state==503){ PLAYER_STATE(i,100); }
				if(P[i].state==505){ PLAYER_STATE(i,100); }
				if(P[i].state==506)
				{ 
					if(P[i].energy>0)
					{
						PLAYER_STATE(i,552); 
					}else{
						PLAYER_STATE(i,507); 
					}
				}else{
					if(P[i].state==507){ PLAYER_STATE(i,100); }
				}				
				if(P[i].state==508){ PLAYER_STATE(i,100); }
				if(P[i].state==510)
				{ 
					if (P[i].partB){ SPR_releaseSprite(P[i].partB); P[i].partB = NULL; }
					PLAYER_STATE(i,999); 
				}
				if(P[i].state==552){ PLAYER_STATE(i,100); }
				if(P[i].state==570){ 
					//PS: Se energy<=0, entao perde uma vida
					PLAYER_STATE(i,552); 
				}
			}
			P[i].frameTimeAtual = 1;
			P[i].frameTimeTotal = P[i].dataAnim[P[i].animFrame];
			if(P[i].sprite){ SPR_setAnimAndFrame(P[i].sprite, 0, P[i].animFrame-1); }
			FUNCAO_FSM_HITBOXES(i); //Atualiza as Hurt / Hitboxes
		}
	}
	
	//ENEMYS
	for(u8 i=1; i<=MAX_ENEMYS-1; i++)
	{
		if(E[i].sprite) //verifica se o sprite esta carregado
		{
			E[i].frameTimeAtual++; 
			if(E[i].frameTimeAtual>E[i].frameTimeTotal) //hora de trocar o frame!
			{
				E[i].animFrame++;
				if(E[i].animFrame>E[i].animFrameTotal) //hora de trocar ou recarregar a animacao!
				{
					if(E[i].state==100){ ENEMY_STATE(i,100); }
					// estes aqui previnem bug, caso o tempo de animacao dos Aereos seja definido errado e o player nao tenha tempo de chegar ao solo...
					if(E[i].state==300 || E[i].state==320 || E[i].state==301 || E[i].state==321 || E[i].state==322 || E[i].state==328){ ENEMY_STATE(i,100); }
					if( (E[i].state>=101 && E[i].state<=299) || E[i].state==431 || E[i].state==606)
					{ 
						if( E[i].key_JOY_UP_status>=1 || E[i].key_JOY_DOWN_status>=1 || E[i].key_JOY_LEFT_status>=1 || E[i].key_JOY_RIGHT_status>=1 )
						{
							ENEMY_STATE(i,420); 
						}else{
							ENEMY_STATE(i,100); 
						}
					}
					if(E[i].state==199){ ENEMY_STATE(i,100); }
					if(E[i].state==420){ ENEMY_STATE(i,420); }
					if(E[i].state==430){ ENEMY_STATE(i,430); }
					//hits
					if(E[i].state==501){ ENEMY_STATE(i,100); }
					if(E[i].state==502){ ENEMY_STATE(i,100); }
					if(E[i].state==503){ ENEMY_STATE(i,100); }
					if(E[i].state==505){ ENEMY_STATE(i,100); }
					if(E[i].state==506)
					{ 
						if(E[i].energy>0)
						{
							ENEMY_STATE(i,552); 
						}else{
							ENEMY_STATE(i,507); 
						}
					}else{
						if(E[i].state==507){ ENEMY_STATE(i,999); } //Perde uma vida
					}				
					if(E[i].state==508){ ENEMY_STATE(i,999); }
					if(E[i].state==510)
					{ 
						if (E[i].partB){ SPR_releaseSprite(E[i].partB); E[i].partB = NULL; }
						ENEMY_STATE(i,999); 
					}
					if(E[i].state==552)
					{ 
						if( E[i].energy==0 && E[i].receiveDamageType == 3 )
						{ 
							ENEMY_STATE(i,508); //melt
						}else{
							ENEMY_STATE(i,100); 
						}
					}
					if(E[i].state==570){ 
						
						if(E[i].energy==0 && (E[i].receiveDamageType==0 || E[i].receiveDamageType==1))
						{
							//PS: Se energy==0, entao perde uma vida
							ENEMY_STATE(i,999);
						}else{
							ENEMY_STATE(i,552); 
						}
					}
					if(E[i].state==700){ ENEMY_STATE(i,700); }
					if(E[i].state==701){ ENEMY_STATE(i,100); }
				}
				E[i].frameTimeAtual = 1;
				E[i].frameTimeTotal = E[i].dataAnim[E[i].animFrame];
				if(E[i].sprite){ SPR_setAnimAndFrame(E[i].sprite, 0, E[i].animFrame-1); }
				FUNCAO_FSM_HITBOXES_ENEMYS(i); //Atualiza as Hurt / Hitboxes
			}
		}
	}
	
	//SPARKS e efeitos
	
	//players
	for(u8 i=1; i<=qtdePlayers; i++)
	{
		if(P[i].CountDownSpark>0){ P[i].CountDownSpark--; }
		if(P[i].CountDownSpark==1)
		{
			if (P[i].spark){ SPR_releaseSprite(P[i].spark); P[i].spark = NULL; } //solta a spark se estiver ativada
		}
	}	
	
	//enemys
	for(u8 j=1; j<=MAX_ENEMYS-1; j++)
	{
		if(E[j].CountDownSpark>0){ E[j].CountDownSpark--; }
		if(E[j].CountDownSpark==1)
		{
			if (E[j].spark){ SPR_releaseSprite(E[j].spark); E[j].spark = NULL; } //solta a spark se estiver ativada
		}
	}
	
}

void FUNCAO_PLAY_SND(u8 CharID, u16 State, u8 Chanel)
{
	//'GAME' utiliza o 'INGAME_SFX' e 'SOUND_PCM_CH2'
	//P1 utiliza o 'P1_SFX' e 'SOUND_PCM_CH3'
	//P2 utiliza o 'P2_SFX' e 'SOUND_PCM_CH4'
	
	//Chanel = 0 => Game System Chanel
	//Chanel = 1 => Player1 Chanel
	//Chanel = 2 => Player2 Chanel
	
	if(CharID==0) //GAME SYSTEM SFX
	{
		if( State==0 )
		{
			if(Chanel==0){ XGM_setPCM(INGAME_SFX, snd_hit_normal , sizeof(snd_hit_normal)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH2); }
		}
		if( State==4 )
		{
			if(Chanel==0){ XGM_setPCM(INGAME_SFX, snd_hit_cut , sizeof(snd_hit_cut)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH2); }
		}
		if( State==999 )
		{
			if(Chanel==0){ XGM_setPCM(INGAME_SFX, snd_enemy_dead , sizeof(snd_enemy_dead)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH2); }
		}
	}
	
	if(CharID==1) //MUMMY
	{
		if( State==101 || State==102 || State==103 || State==104 || State==105 || 
			State==202 || State==203 || State==204 || State==205 || State==321 || State==322 || State==328 || 
			State==431
			)
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char1_101 , sizeof(snd_char1_101 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char1_101 , sizeof(snd_char1_101 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==199 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char1_199 , sizeof(snd_char1_199 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char1_199 , sizeof(snd_char1_199 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==301 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char1_301 , sizeof(snd_char1_301 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char1_301 , sizeof(snd_char1_301 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
	}
	
	if(CharID==2) //CAPTAIN
	{
		if( State==101 || State==102 || State==103 || State==104 || State==105 || 
			State==202 || State==203 || State==204 || State==205 || State==321 || State==322 || State==328 || 
			State==431
			)
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char2_101 , sizeof(snd_char2_101 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char2_101 , sizeof(snd_char2_101 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==199 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char2_199 , sizeof(snd_char2_199 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char2_199 , sizeof(snd_char2_199 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==301 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char2_301 , sizeof(snd_char2_301 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char2_301 , sizeof(snd_char2_301 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
	}
	
	if(CharID==3) //NINJA
	{
		if( State==101 || State==102 || State==103 || State==104 || State==105 || 
			State==202 || State==204 || State==205 || State==322 || State==328 || 
			State==431
			)
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char3_101 , sizeof(snd_char3_101 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char3_101 , sizeof(snd_char3_101 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==199 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char3_199 , sizeof(snd_char3_199 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char3_199 , sizeof(snd_char3_199 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==203 || State==301 || State==321 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char3_301 , sizeof(snd_char3_301 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char3_301 , sizeof(snd_char3_301 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
	}
	
	if(CharID==4) //BABY
	{
		if( State==101 || State==102 || State==103 || State==104 || State==105 || 
			State==202 || State==203 || State==204 || State==205 || State==328 || 
			State==431
			)
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char4_101 , sizeof(snd_char4_101 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char4_101 , sizeof(snd_char4_101 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==199 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char4_199 , sizeof(snd_char4_199 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char4_199 , sizeof(snd_char4_199 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
		if( State==301 || State==321 || State==322 )
		{
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char4_301 , sizeof(snd_char4_301 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char4_301 , sizeof(snd_char4_301 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
	}
	
	//padroes
	if(State==430)
	{
		if(CharID!=4) 
		{
			//se nao for o BABY... tem sons de passos normais
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_430 , sizeof(snd_430 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_430 , sizeof(snd_430 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}else{ 
			//passos de corrida mecanicos
			if(Chanel==1){ XGM_setPCM(P1_SFX, snd_char4_430 , sizeof(snd_char4_430 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(Chanel==2){ XGM_setPCM(P2_SFX, snd_char4_430 , sizeof(snd_char4_430 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
	}
	if(State==500)
	{
		if(Chanel==1){ XGM_setPCM(P1_SFX, snd_null , sizeof(snd_null )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
		if(Chanel==2){ XGM_setPCM(P2_SFX, snd_null , sizeof(snd_null )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
	}
	if(State==606)
	{
		if(Chanel==1){ XGM_setPCM(P1_SFX, snd_606 , sizeof(snd_606 )); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
		if(Chanel==2){ XGM_setPCM(P2_SFX, snd_606 , sizeof(snd_606 )); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
	}
	
}

void FUNCAO_FSM_HITBOXES(const u8 Player)
{
	bool reset_BBox=0;
	bool reset_HBox=0;
	
	switch ( P[Player].id )
	{
		
		case 0: //char_default
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			
				case 101:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 102:
				//...
				break;
				
				case 103:
				reset_HBox=1; 
				reset_BBox=1;
				break;
				
				case 104:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 105:
				//...
				break;
				
				case 106:
				//...
				break;
				
				case 113:
				//...
				break;
				
				case 151:
				//...
				break;
				
				case 152:
				//...
				break;
				
				case 154:
				//...
				break;
				
				case 155:
				//...
				break;
				
				case 200:
				//...
				break;
				
				case 201:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 202:
				//...
				break;
				
				case 204:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 205:
				//...
				break;
				
				case 301:
				case 311:
				case 321:
				//...
				break;
				
				case 302:
				case 312:
				case 322:
				//...
				break;
				
				case 304:
				case 314:
				case 324:
				//...
				break;
				
				case 305:
				case 315:
				case 325:
				//...
				break;
				
				case 306:
				case 316:
				case 326:
				//...
				break;
				
				case 601:
				case 602:
				case 603:
				case 604:
				case 605:
				case 606:
				reset_HBox=1;
				break;
			}
		break;
		
		case 1: //MUMMY
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				break;
			
				case 101:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 102:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 103:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 104:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 105:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 199:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-60; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 203:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==7){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==8){ reset_HBox=1; }
				break;
				
				case 204:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==9){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==10){ reset_HBox=1; }
				break;
				
				case 205:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 301:
				case 328:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 321:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 322:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 431:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
			}
		break;
		
		case 2: //CAPTAIN COMMANDO
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 101:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 102:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 103:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 104:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				if(P[Player].animFrame==9){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==10){ reset_HBox=1; }
				break;
				
				case 105:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 199:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==15){ P[Player].dataHBox[0]=- 60; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==20){ P[Player].dataHBox[0]=- 80; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==25){ P[Player].dataHBox[0]=-100; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==6){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==7){ reset_HBox=1; }
				break;
				
				case 203:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 204:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				if(P[Player].animFrame==11){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==12){ reset_HBox=1; }
				break;
				
				case 205:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==6){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==7){ reset_HBox=1; }
				break;
				
				case 301:
				case 328:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 321:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 70; P[Player].dataHBox[3]=-30; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 322:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame== 7){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=120; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==19){ reset_HBox=1; }
				break;
				
				case 431:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
			}
		break;
		
		case 3: //NINJA
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 101:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
			
				case 102:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 103:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 70; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 104:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==6){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 40; P[Player].dataHBox[2]= 70; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==7){ reset_HBox=1; }
				break;
				
				case 199:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==10){ P[Player].dataHBox[0]=-60; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 203:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==6){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==7){ reset_HBox=1; }
				break;
				
				case 204:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==8){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==9){ reset_HBox=1; }
				break;
				
				case 105:
				case 205:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 40; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 301:
				case 328:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 321:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 40; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-10; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 322:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 431:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
			}
		break;
		
		case 4: //BABY
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 101:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 40; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 102:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 103:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-30; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 104:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-30; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 105:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-50; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				if(P[Player].animFrame==6){ P[Player].dataHBox[0]=-50; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==7){ reset_HBox=1; }
				break;
				
				case 199:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==10){ P[Player].dataHBox[0]=-60; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==8){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==9){ reset_HBox=1; }
				break;
				
				case 203:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==7){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==8){ reset_HBox=1; }
				break;
				
				case 204:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				if(P[Player].animFrame==7){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==8){ reset_HBox=1; }
				break;
				
				case 205:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==7){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==8){ reset_HBox=1; }
				break;
				
				case 301:
				case 328:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 321:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 322:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==6){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==7){ reset_HBox=1; }
				break;
				
				case 431:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-16; P[Player].dataBBox[1]=- 72; P[Player].dataBBox[2]= 16; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==11){ reset_HBox=1; }
				break;
			}
		break;
		
		case 5: //ENEMY01
		case 6: //ENEMY02
		case 7: //ENEMY03
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			
				case 101:
				case 201:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]= 65; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 102:
				case 202:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==3){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-30; }
				if(P[Player].animFrame==4){ reset_HBox=1; }
				break;
				
				case 103:
				case 203:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==2){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 40; P[Player].dataHBox[2]= 70; P[Player].dataHBox[3]=-20; }
				if(P[Player].animFrame==3){ reset_HBox=1; }
				break;
				
				case 104:
				case 204:
				case 105:
				case 205:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 75; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 300:
				case 320:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 301:
				case 321:
				case 322:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==1){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=- 1; }
				break;
				
				case 420:
				case 430:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 431:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 65; P[Player].dataHBox[3]=-50; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 501:
				case 502:
				case 503:
				case 505:
				case 506:
				case 507:
				case 508:
				case 509:
				case 510:
				case 511:
				case 512:
				case 513:
				case 514:
				case 550:
				case 551:
				case 552:
				case 570:
				if(P[Player].animFrame==1){ reset_HBox=1; reset_HBox=1; }
				break;
				
				case 606:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			}
		break;
		
		case 8: //ENEMY04
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			
				case 101:
				case 201:
				case 102:
				case 202:
				case 103:
				case 203:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 50; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]= 50; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 104:
				case 204:
				case 105:
				case 205:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 300:
				case 320:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 301:
				case 321:
				case 322:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==1){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=- 1; }
				break;
				
				case 420:
				case 430:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 431:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==5){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-40; }
				if(P[Player].animFrame==6){ reset_HBox=1; }
				break;
				
				case 501:
				case 502:
				case 503:
				case 505:
				case 506:
				case 507:
				case 508:
				case 509:
				case 510:
				case 511:
				case 512:
				case 513:
				case 514:
				case 550:
				case 551:
				case 552:
				case 570:
				if(P[Player].animFrame==1){ reset_HBox=1; reset_HBox=1; }
				break;
				
				case 606:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			}
		break;
		
		case 9: //ENEMY05
			switch ( P[Player].state )
			{
				case 100:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 101:
				case 201:
				case 102:
				case 202:
				case 103:
				case 203:
				case 104:
				case 204:
				case 105:
				case 205:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-30; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 199:
				if(P[Player].animFrame==1){ P[Player].dataHBox[0]=-50; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-50; }
				break;
				
				case 300:
				case 320:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 301:
				case 321:
				case 322:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==1){ P[Player].dataHBox[0]=  0; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=- 1; }
				break;
				
				case 420:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
				
				case 430:
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==1){ P[Player].dataHBox[0]=-50; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 50; P[Player].dataHBox[3]=-50; }
				break;
				
				case 431:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 25; P[Player].dataBBox[3]=  0; }
				if(P[Player].animFrame==4){ P[Player].dataHBox[0]=-10; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-30; }
				if(P[Player].animFrame==5){ reset_HBox=1; }
				break;
				
				case 501:
				case 502:
				case 503:
				case 505:
				case 506:
				case 507:
				case 508:
				case 509:
				case 510:
				case 511:
				case 512:
				case 513:
				case 514:
				case 550:
				case 551:
				case 552:
				case 570:
				if(P[Player].animFrame==1){ reset_HBox=1; reset_HBox=1; }
				break;
				
				case 606:
				if(P[Player].animFrame==1){ reset_HBox=1; }
				if(P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
				break;
			}
		break;
		
	}
	
	if(P[Player].direction==-1)//P[Player].dataHBox[0]<P[Player].dataHBox[2]) //Normaliza a box
	{
		s16 a;
		a = P[Player].dataHBox[0];
		P[Player].dataHBox[0] = P[Player].dataHBox[2];
		P[Player].dataHBox[2] = a;
		P[Player].dataHBox[0] = P[Player].dataHBox[0] * P[Player].direction;
		P[Player].dataHBox[2] = P[Player].dataHBox[2] * P[Player].direction;
	}
	
	//padroes
	if(P[Player].state==100 && P[Player].animFrame==1){ reset_HBox=1; } //parado
	if(P[Player].state==199 && P[Player].animFrame==1){ reset_BBox=1; } //especial (essa linha sempre deixa invencivel ao ativar o especial)
	if(P[Player].state==420 && P[Player].animFrame==1){ reset_HBox=1; } //andando
	
	if( (P[Player].state>=610 && P[Player].state<=618) /*&& P[Player].animFrame==1*/ ){ reset_BBox=1; } //win poses, time over; rage explosion
	if( P[Player].state>=501 && P[Player].state>=570 ){ reset_BBox=1; } 
	
	if(P[Player].state==300 || P[Player].state==310 || P[Player].state==320){ reset_HBox=1; }
	
	if(Player==1 && (P[2].state==110 || P[2].state==210)){ reset_HBox=1; }
	if(Player==2 && (P[1].state==110 || P[1].state==210)){ reset_HBox=1; }
	
	//resets
	if(reset_HBox==1){ P[Player].dataHBox[0]=0; P[Player].dataHBox[1]=0; P[Player].dataHBox[2]=0; P[Player].dataHBox[3]=0; } //Hit Boxes
	if(reset_BBox==1){ P[Player].dataBBox[0]=0; P[Player].dataBBox[1]=0; P[Player].dataBBox[2]=0; P[Player].dataBBox[3]=0; } //Body Boxes
	
}

void FUNCAO_FSM_HITBOXES_ENEMYS(const u8 Enemy)
{
	bool reset_BBox=0;
	bool reset_HBox=0;
	
	switch ( E[Enemy].id )
	{

		case 0: //char_default
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 90; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
			
				case 101:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 102:
				//...
				break;
				
				case 103:
				reset_HBox=1; 
				reset_BBox=1;
				break;
				
				case 104:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 105:
				//...
				break;
				
				case 106:
				//...
				break;
				
				case 113:
				//...
				break;
				
				case 151:
				//...
				break;
				
				case 152:
				//...
				break;
				
				case 154:
				//...
				break;
				
				case 155:
				//...
				break;
				
				case 200:
				//...
				break;
				
				case 201:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 202:
				//...
				break;
				
				case 204:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				//...
				break;
				
				case 205:
				//...
				break;
				
				case 301:
				case 311:
				case 321:
				//...
				break;
				
				case 302:
				case 312:
				case 322:
				//...
				break;
				
				case 304:
				case 314:
				case 324:
				//...
				break;
				
				case 305:
				case 315:
				case 325:
				//...
				break;
				
				case 306:
				case 316:
				case 326:
				//...
				break;
				
				case 601:
				case 602:
				case 603:
				case 604:
				case 605:
				case 606:
				reset_HBox=1;
				break;
			}
		break;
		
		case 1: //MUMMY
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				break;
			
				case 101:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==3){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==4){ reset_HBox=1; }
				break;
				
				case 102:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				break;
				
				case 103:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				break;
				
				case 104:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 105:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				break;
				
				case 199:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]=-60; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 203:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==7){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==8){ reset_HBox=1; }
				break;
				
				case 204:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==9){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==10){ reset_HBox=1; }
				break;
				
				case 205:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 431:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]=  0; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]=100; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
			}
		break;
		
		case 2: //CAPTAIN COMMANDO
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 101:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 80; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				break;
				
				case 102:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 103:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 104:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==3){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==4){ reset_HBox=1; }
				if(E[Enemy].animFrame==9){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==10){ reset_HBox=1; }
				break;
				
				case 105:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 199:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==15){ E[Enemy].dataHBox[0]=-60; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==6){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==7){ reset_HBox=1; }
				break;
				
				case 203:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 204:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				if(E[Enemy].animFrame==11){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==12){ reset_HBox=1; }
				break;
				
				case 205:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==6){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==7){ reset_HBox=1; }
				break;
				
				case 431:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]=  0; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 80; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
			}
		break;
		
		case 3: //NINJA
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 101:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
			
				case 102:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==3){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==4){ reset_HBox=1; }
				break;
				
				case 103:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 70; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 104:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==6){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 40; E[Enemy].dataHBox[2]= 70; E[Enemy].dataHBox[3]=-20; }
				if(E[Enemy].animFrame==7){ reset_HBox=1; }
				break;
				
				case 199:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==10){ E[Enemy].dataHBox[0]=-60; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 203:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==6){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==7){ reset_HBox=1; }
				break;
				
				case 204:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==8){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==9){ reset_HBox=1; }
				break;
				
				case 105:
				case 205:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 40; E[Enemy].dataHBox[2]= 80; E[Enemy].dataHBox[3]=-20; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 431:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==3){ E[Enemy].dataHBox[0]=  0; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]=100; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==4){ reset_HBox=1; }
				break;
			}
		break;
		
		case 4: //BABY
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 101:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 40; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-20; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				break;
				
				case 102:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 60; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 103:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-30; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 104:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 50; E[Enemy].dataHBox[2]=110; E[Enemy].dataHBox[3]=-30; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 105:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]=-50; E[Enemy].dataHBox[1]=- 60; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				if(E[Enemy].animFrame==6){ E[Enemy].dataHBox[0]=-50; E[Enemy].dataHBox[1]=- 60; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==7){ reset_HBox=1; }
				break;
				
				case 199:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==10){ E[Enemy].dataHBox[0]=-60; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-40; }
				break;
				
				case 202:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==8){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==9){ reset_HBox=1; }
				break;
				
				case 203:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==7){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==8){ reset_HBox=1; }
				break;
				
				case 204:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				if(E[Enemy].animFrame==7){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==8){ reset_HBox=1; }
				break;
				
				case 205:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==7){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==8){ reset_HBox=1; }
				break;
				
				case 431:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-16; E[Enemy].dataBBox[1]=- 72; E[Enemy].dataBBox[2]= 16; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==3){ E[Enemy].dataHBox[0]=  0; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]=100; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==11){ reset_HBox=1; }
				break;
			}
		break;
		
		case 5: //ENEMY01
		case 6: //ENEMY02
		case 7: //ENEMY03
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
			
				case 101:
				case 201:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 60; E[Enemy].dataHBox[2]= 65; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				break;
				
				case 102:
				case 202:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==3){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 50; E[Enemy].dataHBox[2]= 80; E[Enemy].dataHBox[3]=-30; }
				if(E[Enemy].animFrame==4){ reset_HBox=1; }
				break;
				
				case 103:
				case 203:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 40; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=-20; }
				if(E[Enemy].animFrame==3){ reset_HBox=1; }
				break;
				
				case 104:
				case 204:
				case 105:
				case 205:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 80; E[Enemy].dataHBox[2]= 65; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 300:
				case 320:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 301:
				case 321:
				case 322:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataHBox[0]=  0; E[Enemy].dataHBox[1]=- 30; E[Enemy].dataHBox[2]= 80; E[Enemy].dataHBox[3]=- 1; }
				break;
				
				case 420:
				case 430:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 431:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 80; E[Enemy].dataHBox[2]= 65; E[Enemy].dataHBox[3]=-50; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 501:
				case 502:
				case 503:
				case 505:
				case 506:
				case 507:
				case 508:
				case 509:
				case 510:
				case 511:
				case 512:
				case 513:
				case 514:
				case 550:
				case 551:
				case 552:
				case 570:
				if(E[Enemy].animFrame==1){ reset_HBox=1; reset_HBox=1; }
				break;
				
				case 606:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
			}
		break;
		
		case 8: //ENEMY04
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
			
				case 101:
				case 201:
				case 102:
				case 202:
				case 103:
				case 203:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 50; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 50; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]=110; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 104:
				case 204:
				case 105:
				case 205:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]=110; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 300:
				case 320:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 301:
				case 321:
				case 322:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataHBox[0]=  0; E[Enemy].dataHBox[1]=- 30; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=- 1; }
				break;
				
				case 420:
				case 430:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 431:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==2){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==5){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 70; E[Enemy].dataHBox[2]=110; E[Enemy].dataHBox[3]=-40; }
				if(E[Enemy].animFrame==6){ reset_HBox=1; }
				break;
				
				case 501:
				case 502:
				case 503:
				case 505:
				case 506:
				case 507:
				case 508:
				case 509:
				case 510:
				case 511:
				case 512:
				case 513:
				case 514:
				case 550:
				case 551:
				case 552:
				case 570:
				if(E[Enemy].animFrame==1){ reset_HBox=1; reset_HBox=1; }
				break;
				
				case 606:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
			}
		break;
		
		case 9: //ENEMY05
			switch ( E[Enemy].state )
			{
				case 100:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 101:
				case 201:
				case 102:
				case 202:
				case 103:
				case 203:
				case 104:
				case 204:
				case 105:
				case 205:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 80; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-30; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 199:
				if(E[Enemy].animFrame==1){ E[Enemy].dataHBox[0]=-50; E[Enemy].dataHBox[1]=- 80; E[Enemy].dataHBox[2]= 50; E[Enemy].dataHBox[3]=-50; }
				break;
				
				case 300:
				case 320:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 301:
				case 321:
				case 322:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataHBox[0]=  0; E[Enemy].dataHBox[1]=- 30; E[Enemy].dataHBox[2]= 60; E[Enemy].dataHBox[3]=- 1; }
				break;
				
				case 420:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
				
				case 430:
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataHBox[0]=-50; E[Enemy].dataHBox[1]=- 80; E[Enemy].dataHBox[2]= 50; E[Enemy].dataHBox[3]=-50; }
				break;
				
				case 431:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 25; E[Enemy].dataBBox[3]=  0; }
				if(E[Enemy].animFrame==4){ E[Enemy].dataHBox[0]= 10; E[Enemy].dataHBox[1]=- 80; E[Enemy].dataHBox[2]= 90; E[Enemy].dataHBox[3]=-30; }
				if(E[Enemy].animFrame==5){ reset_HBox=1; }
				break;
				
				case 501:
				case 502:
				case 503:
				case 505:
				case 506:
				case 507:
				case 508:
				case 509:
				case 510:
				case 511:
				case 512:
				case 513:
				case 514:
				case 550:
				case 551:
				case 552:
				case 570:
				if(E[Enemy].animFrame==1){ reset_HBox=1; reset_HBox=1; }
				break;
				
				case 606:
				if(E[Enemy].animFrame==1){ reset_HBox=1; }
				if(E[Enemy].animFrame==1){ E[Enemy].dataBBox[0]=-20; E[Enemy].dataBBox[1]=- 70; E[Enemy].dataBBox[2]= 15; E[Enemy].dataBBox[3]=  0; }
				break;
			}
		break;
		
	}
	
	if(E[Enemy].direction==-1)//E[Enemy].dataHBox[0]<E[Enemy].dataHBox[2]) //Normaliza a box
	{
		s16 a;
		a = E[Enemy].dataHBox[0];
		E[Enemy].dataHBox[0] = E[Enemy].dataHBox[2];
		E[Enemy].dataHBox[2] = a;
		E[Enemy].dataHBox[0] = E[Enemy].dataHBox[0] * E[Enemy].direction;
		E[Enemy].dataHBox[2] = E[Enemy].dataHBox[2] * E[Enemy].direction;
	}
	
	//padroes
	if(E[Enemy].state==100 && E[Enemy].animFrame==1){ reset_HBox=1; } //parado
	if(E[Enemy].state==199 && E[Enemy].animFrame==1){ reset_BBox=1; } //especial (essa linha sempre deixa invencivel ao ativar o especial)
	if(E[Enemy].state==420 && E[Enemy].animFrame==1){ reset_HBox=1; } //andando
	
	if( (E[Enemy].state>=610 && E[Enemy].state<=618) /*&& E[Enemy].animFrame==1*/ ){ reset_BBox=1; } //win poses, time over; rage explosion
	if( E[Enemy].state>=501 && E[Enemy].state>=570 ){ reset_BBox=1; } 
	
	if(E[Enemy].state==300 || E[Enemy].state==310 || E[Enemy].state==320){ reset_HBox=1; }
	
	//if(Player==1 && (P[2].state==110 || P[2].state==210)){ reset_HBox=1; }
	//if(Player==2 && (P[1].state==110 || P[1].state==210)){ reset_HBox=1; }
	
	//resets
	if(reset_HBox==1){ E[Enemy].dataHBox[0]=0; E[Enemy].dataHBox[1]=0; E[Enemy].dataHBox[2]=0; E[Enemy].dataHBox[3]=0; } //Hit Boxes
	if(reset_BBox==1){ E[Enemy].dataBBox[0]=0; E[Enemy].dataBBox[1]=0; E[Enemy].dataBBox[2]=0; E[Enemy].dataBBox[3]=0; } //Body Boxes
	
}

bool FUNCAO_COLISAO(s16 R1x1, const s16 R1y1,
					s16 R1x2, const s16 R1y2,
					s16 R2x1, const s16 R2y1,
					s16 R2x2, const s16 R2y2)
{
	
	s16 temp=0;
	if(R1x1>R1x2){ temp=R1x1; R1x1=R1x2; R1x2=temp; }
	if(R2x1>R2x2){ temp=R2x1; R2x1=R2x2; R2x2=temp; }
	
	if ( (R1x1+(R1x2-R1x1)>R2x1) && (R1x1<R2x1+(R2x2-R2x1)) && 
		 (R1y1+(R1y2-R1y1)>R2y1) && (R1y1<R2y1+(R2y2-R2y1)) )
	{ 
		
		if(R1x1==R1x2 && R1y1==R1y2){
			return 0;
			}else if(R2x1==R2x2 && R2y1==R2y2){
				return 0;
			}else{
				/*houve colisao*/
				return 1;
			}
	}else{ 
		/*nao houve colisao*/
		return 0;
	}
}

void CLEAR_VDP(void)
{
	SYS_disableInts();
	SPR_reset();
	VDP_clearPlane(BG_A, TRUE);
	VDP_clearPlane(BG_B, TRUE);	
	VDP_setTextPlane(BG_A);  
	VDP_setHorizontalScroll(BG_B, 0); 
	VDP_setVerticalScroll(BG_B, 0); 
	VDP_setHorizontalScroll(BG_A, 0); 
	VDP_setVerticalScroll(BG_A, 0);
	VDP_setPaletteColors(0, (u16 *)palette_black, 64); 
	SYS_enableInts();
}


//EOF; End of File