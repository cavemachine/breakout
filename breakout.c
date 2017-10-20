#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define BLOCKS_X 13
#define BLOCKS_Y 20
#define BLOCKS_W 35
#define BLOCKS_H 15
#define WINDOW_W 500
#define WINDOW_H 400

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Surface* player_s;
SDL_Texture* player_t;
SDL_Rect player_rect;

SDL_Surface* ball_s;
SDL_Texture* ball_t;

SDL_Surface* block_green_s;
SDL_Texture* block_green_t;
SDL_Surface* block_blue_s;
SDL_Texture* block_blue_t;
SDL_Surface* block_red_s;
SDL_Texture* block_red_t;
SDL_Surface* block_yellow_s;
SDL_Texture* block_yellow_t;
SDL_Surface* block_yellow_d1_s;
SDL_Texture* block_yellow_d1_t;
SDL_Surface* block_yellow_d2_s;
SDL_Texture* block_yellow_d2_t;
SDL_Surface* block_unbreak_s;
SDL_Texture* block_unbreak_t;
SDL_Surface* block_unbreak_ani_s;
SDL_Texture* block_unbreak_ani_t;
SDL_Surface* powerup_s;
SDL_Texture* powerup_t;
SDL_Rect block_rect;

SDL_Rect animate_src;
SDL_Rect animate_dest;
int sprite = 0;
int sprite_sub = 0;

bool animate_flag = false;
bool powerup_flag = false;
int powerup_frame_sub;

SDL_Rect intersect_rect;

struct block {
    bool enabled;
    bool unbreakable;
    SDL_Texture* block_sprite;    
    int durability;
    int block_x;
    int block_y;
    
    bool animate;
    SDL_Texture* anim_sprite;
    int anim_frame;
    int anim_frame_max;
    SDL_Rect anim_src;
    SDL_Rect anim_dest;

    bool powerup_released;
    bool powerup_stored;
    char powerup_type;
    SDL_Rect powerup_src;
    SDL_Rect powerup_dest;
    int powerup_frame;
    int powerup_frame_max;
};

struct ball {
    bool enabled;
    SDL_Rect ball_rect;
    SDL_Rect ball_rect_prev;
    bool ball_increase_x;
    bool ball_increase_y;
    int ball_increase_value_x;
    int ball_increase_value_y;
};

struct ball balls[5];
struct block blocks[BLOCKS_Y][BLOCKS_X];

int map1[BLOCKS_Y][BLOCKS_X] = {{0,0,0,0,0,1,0,5,0,0,0,0,0},
				{1,0,0,0,0,0,0,5,0,0,0,0,0},
				{1,2,5,0,0,2,0,2,0,0,0,0,0},
				{1,2,3,0,5,1,0,2,0,0,0,0,0},
				{1,2,3,4,0,0,0,0,0,5,0,0,0},
				{1,2,3,1,2,0,0,0,0,0,0,0,0},
				{1,4,5,1,2,3,0,0,0,0,0,0,0},
				{1,2,3,1,2,3,1,1,0,5,0,0,0},
				{4,2,4,5,2,3,1,2,0,0,0,0,0},
				{1,2,3,1,2,3,1,0,3,0,0,0,0},
				{5,5,5,5,5,5,5,5,5,5,5,5,1},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0},	
				{0,0,0,0,0,0,0,0,0,0,0,0,0}};
void animate_blocks();
void animate_powerup();
void check_ball_player_collision();
void check_ball_blocks_collision();
void check_powerup_player_collision(int i, int j);
void init_window();
void init_sprites();
void init_level(int level);
void random_powerups();
void render_blocks();
void render_all();
void break_block(int i, int j);
void pong_ball();
void main_routine();


//----------------------------------------------------------
void init_window() {
    srand( time(NULL) );
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("breakout",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_W,
                              WINDOW_H,
                              SDL_WINDOW_SHOWN);
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(renderer, 20, 40, 50, 0);
}

void init_sprites() {
    
    player_s = SDL_LoadBMP("img/player.bmp");
    player_t = SDL_CreateTextureFromSurface(renderer, player_s);
    
    ball_s = SDL_LoadBMP("img/ball.bmp");
    ball_t = SDL_CreateTextureFromSurface(renderer, ball_s);

    block_green_s = SDL_LoadBMP("img/block_green.bmp");
    block_green_t = SDL_CreateTextureFromSurface(renderer, block_green_s);

    block_blue_s = SDL_LoadBMP("img/block_blue.bmp");
    block_blue_t = SDL_CreateTextureFromSurface(renderer, block_blue_s);

    block_red_s = SDL_LoadBMP("img/block_red.bmp");
    block_red_t = SDL_CreateTextureFromSurface(renderer, block_red_s);
    
    block_yellow_s = SDL_LoadBMP("img/block_yellow.bmp");
    block_yellow_t = SDL_CreateTextureFromSurface(renderer, block_yellow_s);
    
    block_yellow_d1_s = SDL_LoadBMP("img/block_yellow_d1.bmp");
    block_yellow_d1_t = SDL_CreateTextureFromSurface(renderer, block_yellow_d1_s);
    
    block_yellow_d2_s = SDL_LoadBMP("img/block_yellow_d2.bmp");
    block_yellow_d2_t = SDL_CreateTextureFromSurface(renderer, block_yellow_d2_s);
    
    block_unbreak_s = SDL_LoadBMP("img/block_unbreak.bmp");
    block_unbreak_t = SDL_CreateTextureFromSurface(renderer, block_unbreak_s);
    
    block_unbreak_ani_s = SDL_LoadBMP("img/pack.bmp");
    block_unbreak_ani_t = SDL_CreateTextureFromSurface(renderer, block_unbreak_ani_s);
    
    powerup_s = SDL_LoadBMP("img/powerup.bmp");
    powerup_t = SDL_CreateTextureFromSurface(renderer, powerup_s);
    
    player_rect.h = 10;
    player_rect.w = 50;
    player_rect.x = WINDOW_W / 2;
    player_rect.y = WINDOW_H - 15;

    block_rect.h = BLOCKS_H;
    block_rect.w = BLOCKS_W;
    
    for (int b = 0; b < 5; b++) {
	if (balls[b].enabled) {
	    SDL_RenderCopy(renderer, ball_t, NULL, &balls[b].ball_rect);
	}
    }
    SDL_RenderCopy(renderer, player_t, NULL, &player_rect);
    SDL_RenderPresent(renderer);

}

void init_level(int level) {

    if (level == 1) {
	for (int i = 0; i < BLOCKS_Y; i++) {
	    for (int j = 0; j < BLOCKS_X; j++) {
		if (map1[i][j] == 1) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 0;
		    blocks[i][j].block_sprite = block_red_t;
		    blocks[i][j].block_x = (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 2) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 0;
		    blocks[i][j].block_sprite = block_blue_t;
		    blocks[i][j].block_x = (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 3) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 0;
		    blocks[i][j].block_sprite = block_green_t;
		    blocks[i][j].block_x = (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 4) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 2;
		    blocks[i][j].block_sprite = block_yellow_t;
		    blocks[i][j].block_x = (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 5) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].unbreakable = true;
		    blocks[i][j].block_sprite = block_unbreak_t;
		    blocks[i][j].anim_sprite = block_unbreak_ani_t;
		    blocks[i][j].block_x = (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = (i*(BLOCKS_H+3));	    
		}		
	    }
	}

    }
    random_powerups();
    for (int i = 1; i < 5; i++) {
	balls[i].enabled = false;
    }
    balls[0].enabled = true;
    balls[0].ball_rect.h = 5;
    balls[0].ball_rect.w = 5;
    balls[0].ball_rect.x = 180;
    balls[0].ball_rect.y = 100;
    balls[0].ball_increase_value_x = 5;
    balls[0].ball_increase_value_y = 5;
    balls[0].ball_increase_x = true;
    balls[0].ball_increase_y = false;

    balls[1].enabled = true;
    balls[1].ball_rect.h = 5;
    balls[1].ball_rect.w = 5;
    balls[1].ball_rect.x = 150;
    balls[1].ball_rect.y = 200;
    balls[1].ball_increase_value_x = 5;
    balls[1].ball_increase_value_y = 5;
    balls[1].ball_increase_x = true;
    balls[1].ball_increase_y = false;

    /* blocks[10][12].powerup = 'e'; */
    /* blocks[10][12].powerup_released = false; */
    /* blocks[10][12].powerup_frame = 0; */
    /* blocks[10][12].powerup_frame_max = 6; */
    /* blocks[10][12].powerup_src.h = 7; */
    /* blocks[10][12].powerup_src.w = 15; */
    /* blocks[10][12].powerup_src.x = 0; */
    /* blocks[10][12].powerup_src.y = 25; */
    /* blocks[10][12].powerup_dest.h = 15; */
    /* blocks[10][12].powerup_dest.w = 35; */
    /* blocks[10][12].powerup_dest.x = blocks[10][12].block_x; */
    /* blocks[10][12].powerup_dest.y = blocks[10][12].block_y; */    
}

void random_powerups() {
    int p;
    for (int i = 0; i < BLOCKS_Y; i++) {
	for (int j = 0; j < BLOCKS_X; j++) {
	    if (blocks[i][j].enabled) {
		p = rand() % 10;
		switch(p) {
		case 0:
		    blocks[i][j].powerup_type = 'e';
		    blocks[i][j].powerup_stored = true;
		    blocks[i][j].powerup_released = false;
		    blocks[i][j].powerup_frame = 0;
		    blocks[i][j].powerup_frame_max = 6;
		    blocks[i][j].powerup_src.h = 7;
		    blocks[i][j].powerup_src.w = 15;
		    blocks[i][j].powerup_src.x = 0;
		    blocks[i][j].powerup_src.y = 25;
		    blocks[i][j].powerup_dest.h = 15;
		    blocks[i][j].powerup_dest.w = 35;
		    blocks[i][j].powerup_dest.x = blocks[i][j].block_x;
		    blocks[i][j].powerup_dest.y = blocks[i][j].block_y;
		    break;		
		}
	    }
	    
	}
    }
}

void render_blocks() {
    for (int i = 0; i < BLOCKS_Y; i++) {
	for (int j = 0; j < BLOCKS_X; j++) {
	    if (blocks[i][j].enabled == true) {
		block_rect.x = blocks[i][j].block_x;
		block_rect.y = blocks[i][j].block_y;
		if (blocks[i][j].animate == false) {
		    SDL_RenderCopy(renderer, blocks[i][j].block_sprite, NULL, &block_rect);
		} else {
		    SDL_RenderCopy(renderer, blocks[i][j].anim_sprite, &blocks[i][j].anim_src, &blocks[i][j].anim_dest);
		}
		
	    }
	    if (blocks[i][j].powerup_released) {
		SDL_RenderCopy(renderer, powerup_t, &blocks[i][j].powerup_src, &blocks[i][j].powerup_dest);
	    }
	}
    }
}


void render_all() {
    SDL_RenderClear(renderer);
    render_blocks();
       
    for (int b = 0; b < 5; b++) {
	if (balls[b].enabled) {
	    SDL_RenderCopy(renderer, ball_t, NULL, &balls[b].ball_rect);
	}
    }
    
    SDL_RenderCopy(renderer, player_t, NULL, &player_rect);

    if (animate_flag) {
	sprite_sub++;
	if (sprite_sub == 4) {	    
	    animate_blocks();
	    sprite_sub = 0;
	}
    }

    if (powerup_flag) {
	powerup_frame_sub++;
	if (powerup_frame_sub == 5) {
	    animate_powerup();
	    powerup_frame_sub = 0;
	}
    }
    
    SDL_RenderPresent(renderer);
}

void check_powerup_player_collision(int i, int j) {
    if (SDL_HasIntersection(&blocks[i][j].powerup_dest, &player_rect)) {
	printf("aoo\n");
	blocks[i][j].powerup_released = false;
	blocks[i][j].powerup_stored = false;
	player_rect.w += 20;
	}
}

void check_ball_player_collision() {
    for (int i = 0; i < 5; i++) {
	if (balls[i].enabled) {
	    if (SDL_IntersectRect(&balls[i].ball_rect, &player_rect, &intersect_rect)) {
		//printf("intersect x: %i\n", intersect_rect.x);
		//printf("intersect y: %i\n", intersect_rect.y);
		//printf("player intersect x: %i\n", intersect_rect.x - player_rect.x);
		int it_val = intersect_rect.x - player_rect.x;

		if (it_val >= 0 && it_val <= 7) {
		    balls[i].ball_increase_value_x = 3;
		    balls[i].ball_increase_value_y = 2;
		    if (balls[i].ball_increase_x) { balls[i].ball_increase_x = false; }
		}
		if (it_val > 7 && it_val <= 15) {
		    balls[i].ball_increase_value_x = 2;
		    balls[i].ball_increase_value_y = 2;
		    if (balls[i].ball_increase_x) { balls[i].ball_increase_x = false; }
		}
		if (it_val > 15 && it_val <= 35) {
		    balls[i].ball_increase_value_x = 1;
		    balls[i].ball_increase_value_y = 2;
		}
		if (it_val > 35 && it_val <= 42) {
		    balls[i].ball_increase_value_x = 2;
		    balls[i].ball_increase_value_y = 2;
		    if (!balls[i].ball_increase_x) { balls[i].ball_increase_x = true; }	    
		}
		if (it_val > 42 && it_val <= 50) {
		    balls[i].ball_increase_value_x = 3;
		    balls[i].ball_increase_value_y = 2;
		    if (!balls[i].ball_increase_x) { balls[i].ball_increase_x = true; }	    
		}	
		balls[i].ball_increase_y = false;
	    }
	}
    }
}

void check_ball_blocks_collision() {
    
    /* check if intersection happened; if so then find the closest side (left right up or down) of the 
       intersection coord (inter_rect.x and y) to figure out where the ball touched the block; If the ball
       is already partially inside the block, moves the ball position to be exactly at the
       border of the block; then change ball moving direction based on the current ball moving direction;
       finally call break_block to reduce block durability or destroy it. */
    for (int b = 0; b < 5; b++) {
	if (balls[b].enabled == true) {
	    
	    for (int i = 0; i < BLOCKS_Y; i++) {
		for (int j = 0; j < BLOCKS_X; j++) {
	    
		    if (blocks[i][j].enabled == true) {		
			block_rect.x = blocks[i][j].block_x;
			block_rect.y = blocks[i][j].block_y;
		
			SDL_Rect inter_rect;
			if (SDL_IntersectRect(&block_rect, &balls[b].ball_rect, &inter_rect)) {

			    if (blocks[i][j].block_x != inter_rect.x &&
				blocks[i][j].block_y != inter_rect.y &&
				blocks[i][j].block_x + BLOCKS_W != inter_rect.y &&
				blocks[i][j].block_y + BLOCKS_H != inter_rect.y) {
				balls[b].ball_rect = balls[b].ball_rect_prev;
			    }
			    int m[4];
			    m[0] = blocks[i][j].block_x - inter_rect.x; // left
			    m[1] = blocks[i][j].block_x + BLOCKS_W - inter_rect.x; // right
			    m[2] = blocks[i][j].block_y - inter_rect.y; // up
			    m[3] = blocks[i][j].block_y + BLOCKS_H - inter_rect.y; //down
			
			    int smaller = abs(m[0]);
			    int smaller_index;
			    for (int i = 0; i < 4; i++) {
				if (i != 3) {
				    if (smaller > abs( m[i+1] )) {
					smaller = abs( m[i+1] );
					smaller_index = i+1;
				    }
				}
				if (i == 3) {
				    if (smaller > abs( m[i] )) {
					smaller = abs( m[i] );
					smaller_index = i;
				    }
				}
			    }

			    switch(smaller_index) {
			    case 0:
	
				if (balls[b].ball_increase_x && !blocks[i][j-1].enabled) {
				    if (j-1 != -1) {
					balls[b].ball_increase_x = !balls[b].ball_increase_x;
				    }
				} else {
				    balls[b].ball_increase_y = !balls[b].ball_increase_y; 			    
				}		    
				break;
			    case 1:

				if (!balls[b].ball_increase_x && !blocks[i][j+1].enabled) {
				    balls[b].ball_increase_x = !balls[b].ball_increase_x;
				} else {
				    balls[b].ball_increase_y = !balls[b].ball_increase_y; 
				}
				break;
			    case 2:			

				if (balls[b].ball_increase_y && !blocks[i-1][j].enabled) {
				    balls[b].ball_increase_y = !balls[b].ball_increase_y;
				} else {
				    balls[b].ball_increase_x = !balls[b].ball_increase_x; 			    
				}
				break;
			    case 3:			

				if (!balls[b].ball_increase_y && !blocks[i+1][j].enabled) {
				    balls[b].ball_increase_y = !balls[b].ball_increase_y;
				} else {
				    balls[b].ball_increase_x = !balls[b].ball_increase_x;
				}
				break;
	  
			    }
			    break_block(i,j);
			    return;
			}
		    }
		}
	    }
	}
    }
}

void break_block(int i, int j) {
    if (blocks[i][j].unbreakable == false) {
	if (blocks[i][j].durability == 0) {
	    blocks[i][j].enabled = false;
	    if (blocks[i][j].powerup_stored) {
		blocks[i][j].powerup_released = true;
		powerup_flag = true;
	    }
	    return;
	}
	if (blocks[i][j].durability == 2) {
	    blocks[i][j].durability--;
	    blocks[i][j].block_sprite = block_yellow_d1_t;
	    return;
	}
	if (blocks[i][j].durability == 1) {
	    blocks[i][j].durability--;
	    blocks[i][j].block_sprite = block_yellow_d2_t;
	    return;
	}	
    } else {
	blocks[i][j].anim_dest.h = BLOCKS_H;
	blocks[i][j].anim_dest.w = BLOCKS_W;
	blocks[i][j].anim_dest.x = blocks[i][j].block_x;
	blocks[i][j].anim_dest.y = blocks[i][j].block_y;
	
	blocks[i][j].anim_src.x = 0;
	blocks[i][j].anim_src.y = 24;
	blocks[i][j].anim_src.h = 8;
	blocks[i][j].anim_src.w = 16;
	
        blocks[i][j].anim_frame = 0;
	blocks[i][j].anim_frame_max = 6;	
	blocks[i][j].animate = true;	
	animate_flag = true;
	    	
    }
}

void animate_powerup() {
    bool powerup_on_screen = false;
    for (int i = 0; i < BLOCKS_Y; i++) {
	for (int j = 0; j < BLOCKS_X; j++) {
	    
	    if (blocks[i][j].powerup_released) {
		powerup_on_screen = true;
		if (blocks[i][j].powerup_type == 'e') {
		    blocks[i][j].powerup_src.x = blocks[i][j].powerup_frame * 16;		    
		    blocks[i][j].powerup_dest.y += 5;		    
		    blocks[i][j].powerup_frame++;
		    if (blocks[i][j].powerup_frame == blocks[i][j].powerup_frame_max - 1) {
			blocks[i][j].powerup_frame = 0;
		    }	
		}
		check_powerup_player_collision(i,j);
	    }
	}
    }
    if (!powerup_on_screen) { powerup_flag = false; }
}

void animate_blocks() {
    
    bool global_animation = false;
    for (int i = 0; i < BLOCKS_Y; i++) {
	for (int j = 0; j < BLOCKS_X; j++) {
	    if (blocks[i][j].animate == true) {
		
		global_animation = true;
		blocks[i][j].anim_src.x = blocks[i][j].anim_frame * blocks[i][j].anim_src.w;
		
		blocks[i][j].anim_frame++;
		if (blocks[i][j].anim_frame == blocks[i][j].anim_frame_max) {
		   blocks[i][j].animate = false;
		}
	    }
	}
    }
    if (global_animation == false) { animate_flag = false; }
}

void pong_ball() {
    for (int b = 0; b < 5; b++) {
	if (balls[b].enabled) {
	    
	    if (balls[b].ball_increase_x) {
		balls[b].ball_rect_prev = balls[b].ball_rect;
		balls[b].ball_rect.x += balls[b].ball_increase_value_x;
		if (balls[b].ball_rect.x >= WINDOW_W) { balls[b].ball_increase_x = false; }
	    } else {
		balls[b].ball_rect_prev = balls[b].ball_rect;
		balls[b].ball_rect.x -= balls[b].ball_increase_value_x;
		if (balls[b].ball_rect.x <= 0) { balls[b].ball_increase_x = true; }
	    }
    
	    if (balls[b].ball_increase_y) {
		balls[b].ball_rect_prev = balls[b].ball_rect;	
		balls[b].ball_rect.y += balls[b].ball_increase_value_y;
		if (balls[b].ball_rect.y >= WINDOW_H) { balls[b].ball_increase_y = false; }
	    } else {
		balls[b].ball_rect_prev = balls[b].ball_rect;	
		balls[b].ball_rect.y -= balls[b].ball_increase_value_y;
		if (balls[b].ball_rect.y <= 0) { balls[b].ball_increase_y = true; }
	    }

	}
    }
}

void main_routine() {
    pong_ball();
    check_ball_player_collision();    
    check_ball_blocks_collision();
    render_all();
}

int main() {
    init_window();
    
    init_sprites();

    init_level(1);

   
    SDL_bool done = SDL_FALSE;
    while(!done)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if(event.type == SDL_QUIT) {
                    done = SDL_TRUE;
                }
                if(event.type == SDL_KEYDOWN) {
		    if (event.key.keysym.sym == SDLK_LEFT) {
			player_rect.x -= 15;
		    }
		    if (event.key.keysym.sym == SDLK_RIGHT) {
			player_rect.x += 15;
		    }
		    if (event.key.keysym.sym == SDLK_a) {
		    }		   		    
                }
	    }
	    main_routine();
	    SDL_Delay(10);
	}


}

