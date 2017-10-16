#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define BLOCK_NUM 20
SDL_Window* window;
SDL_Renderer* renderer;

SDL_Surface* player_s;
SDL_Texture* player_t;
SDL_Rect player_rect;

SDL_Surface* ball_s;
SDL_Texture* ball_t;
SDL_Rect ball_rect;

SDL_Surface* block_s;
SDL_Texture* block_t;
SDL_Rect block_rect;

bool ball_increase_x = true;
bool ball_increase_y = false;
int ball_increase_value_x = 2;
int ball_increase_value_y = 2;
SDL_Rect intersect_rect;

struct block {
    bool enabled;
    char color;
    char powerup;
    int block_x;
    int block_y;
};

struct block blocks[BLOCK_NUM];
//----------------------------------

void init_window() {    
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("breakout",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              500,
                              500,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);    
}

void init_sprites() {
    
    player_s = SDL_LoadBMP("img/player.bmp");
    player_t = SDL_CreateTextureFromSurface(renderer, player_s);
    
    ball_s = SDL_LoadBMP("img/ball.bmp");
    ball_t = SDL_CreateTextureFromSurface(renderer, ball_s);

    block_s = SDL_LoadBMP("img/block.bmp");
    block_t = SDL_CreateTextureFromSurface(renderer, block_s);

    player_rect.h = 10;
    player_rect.w = 50;
    player_rect.x = 250;
    player_rect.y = 480;

    ball_rect.h = 5;
    ball_rect.w = 5;
    ball_rect.x = 200;
    ball_rect.y = 250;

    block_rect.h = 10;
    block_rect.w = 25;
    
    SDL_RenderCopy(renderer, ball_t, NULL, &ball_rect);   
    SDL_RenderCopy(renderer, player_t, NULL, &player_rect);
    SDL_RenderPresent(renderer);
    
}

void render_blocks() {
    for (int i = 0; i < BLOCK_NUM; i++) {
	if (blocks[i].enabled == true) {
	    block_rect.x = blocks[i].block_x;
	    block_rect.y = blocks[i].block_y;
	    SDL_RenderCopy(renderer, block_t, NULL, &block_rect);	    
	}
    }
}
void render_all() {
    SDL_RenderClear(renderer);
    render_blocks();
    SDL_RenderCopy(renderer, ball_t, NULL, &ball_rect);   
    SDL_RenderCopy(renderer, player_t, NULL, &player_rect);
    SDL_RenderPresent(renderer);
}

void check_ball_player_collision() {
    if (SDL_IntersectRect(&ball_rect, &player_rect, &intersect_rect)) {
	//printf("intersect x: %i\n", intersect_rect.x);
	//printf("intersect y: %i\n", intersect_rect.y);
	//printf("player intersect x: %i\n", intersect_rect.x - player_rect.x);
	int it_val = intersect_rect.x - player_rect.x;

	if (it_val >= 0 && it_val <= 7) {
	    ball_increase_value_x = 3;
	    ball_increase_value_y = 2;
	    if (ball_increase_x) { ball_increase_x = false; }
	}
	if (it_val > 7 && it_val <= 15) {
	    ball_increase_value_x = 2;
	    ball_increase_value_y = 2;
	    if (ball_increase_x) { ball_increase_x = false; }
	}
	if (it_val > 15 && it_val <= 35) {
	    ball_increase_value_x = 1;
	    ball_increase_value_y = 2;
	}
	if (it_val > 35 && it_val <= 42) {
	    ball_increase_value_x = 2;
	    ball_increase_value_y = 2;
	    if (!ball_increase_x) { ball_increase_x = true; }	    
	}
	if (it_val > 42 && it_val <= 50) {
	    ball_increase_value_x = 3;
	    ball_increase_value_y = 2;
	    if (!ball_increase_x) { ball_increase_x = true; }	    
	}	
	ball_increase_y = false;
    }
}

void check_ball_blocks_collision() {
    for (int i = 0; i < BLOCK_NUM; i++) {
	if (blocks[i].enabled) {
	    block_rect.x = blocks[i].block_x;
	    block_rect.y = blocks[i].block_y;
	    if (SDL_HasIntersection(&block_rect, &ball_rect)) {
		printf("collision block %i\n", i);
		blocks[i].enabled = false;
		ball_increase_y = !ball_increase_y;
	    }
	}
    }
}

void pong_ball() {

    if (ball_increase_x) {
	ball_rect.x += ball_increase_value_x;
	if (ball_rect.x > 500) { ball_increase_x = false; }
    } else {
	ball_rect.x -= ball_increase_value_x;
	if (ball_rect.x < 0) { ball_increase_x = true; }
    }
    
    if (ball_increase_y) {
	ball_rect.y += ball_increase_value_y;
	if (ball_rect.y > 500) { ball_increase_y = false; }
    } else {
	ball_rect.y -= ball_increase_value_y;
	if (ball_rect.y < 0) { ball_increase_y = true; }
    }
    
    check_ball_player_collision();
    
    check_ball_blocks_collision();

    render_all();
}

void init_level(int level){

    int init_x = 20;
    int init_y = 10;
    
    if (level == 1) {
	for (int i = 0; i < BLOCK_NUM; i++) {
	    blocks[i].enabled = true;
	    if (init_x < 475) {
		blocks[i].block_x = init_x;
		blocks[i].block_y = init_y;
		init_x += 27;
	    } else {
		init_x = 20;
		init_y += 12;
		blocks[i].block_x = init_x;
		blocks[i].block_y = init_y;
		
	    }
	}
    }
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
                if(event.type == SDL_KEYUP) {
		    if (event.key.keysym.sym == SDLK_LEFT) {
			player_rect.x -= 15;
		    }
		    if (event.key.keysym.sym == SDLK_RIGHT) {
			player_rect.x += 15;
		    }		    
                }
	    }
	    pong_ball();
	    SDL_Delay(10);
	}


}
