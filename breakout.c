#include <stdio.h>
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
SDL_Rect ball_rect;

SDL_Surface* block_green_s;
SDL_Texture* block_green_t;
SDL_Surface* block_blue_s;
SDL_Texture* block_blue_t;
SDL_Surface* block_red_s;
SDL_Texture* block_red_t;
SDL_Surface* block_yellow_s;
SDL_Texture* block_yellow_t;
SDL_Surface* block_unbreak_s;
SDL_Texture* block_unbreak_t;
SDL_Rect block_rect;

SDL_Rect animate_src;
SDL_Rect animate_dest;
int sprite = 0;
int sprite_sub = 0;
bool animate_flag = false;

bool ball_increase_x = true;
bool ball_increase_y = false;
int ball_increase_value_x = 1;
int ball_increase_value_y = 1;
SDL_Rect intersect_rect;

struct block {
    bool enabled;
    bool unbreakable;
    SDL_Texture* block_sprite;    
    //   char color;
    char powerup;
    int durability;
    int block_x;
    int block_y;
    
    bool animate;
    SDL_Texture* anim_sprite;
    int anim_frame;
    int anim_frame_max;
    SDL_Rect anim_src;
    SDL_Rect anim_dest;
};

struct block blocks[BLOCKS_Y][BLOCKS_X];

int map1[BLOCKS_Y][BLOCKS_X] = {{1,0,0,0,0,1,0,0,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,0,0,0,0},
				{1,2,0,0,0,2,0,0,0,0,0,0,0},
				{1,0,0,0,5,1,0,0,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,5,0,0,0},
				{1,1,0,0,0,2,0,0,0,0,0,0,0},
				{1,0,0,0,0,1,0,0,0,0,0,0,0},
				{1,0,1,0,0,0,0,3,0,5,0,0,0},
				{1,0,1,0,0,0,0,1,0,0,0,0,0},
				{1,0,4,0,5,0,0,3,0,0,0,0,0},
				{1,0,4,0,0,0,0,3,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,0,0,0,0},
				{1,0,4,0,0,0,0,1,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,5,0,0,0},
				{1,0,4,0,0,0,0,1,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,0,0,0,0},
				{1,0,1,0,0,0,0,1,0,0,0,0,0},	
				{1,1,1,1,1,1,1,1,1,1,1,1,1}};
void animate_blocks();

//----------------------------------------------------------
void init_window() {    
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("breakout",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_W,
                              WINDOW_H,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);    
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
    
    block_unbreak_s = SDL_LoadBMP("img/pack.bmp");
    block_unbreak_t = SDL_CreateTextureFromSurface(renderer, block_unbreak_s);
    	
    player_rect.h = 10;
    player_rect.w = 50;
    player_rect.x = WINDOW_W / 2;
    player_rect.y = WINDOW_H - 20;

    ball_rect.h = 5;
    ball_rect.w = 5;
    ball_rect.x = 200;
    ball_rect.y = 250;

    block_rect.h = BLOCKS_H;
    block_rect.w = BLOCKS_W;
    
    SDL_RenderCopy(renderer, ball_t, NULL, &ball_rect);   
    SDL_RenderCopy(renderer, player_t, NULL, &player_rect);
    SDL_RenderPresent(renderer);
    
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
		    animate_src.w = 16;
		    animate_src.h = 8;
		    animate_src.y = 24;
		    animate_src.x = blocks[i][j].anim_frame * 16;
		
		    animate_dest.w = 35;
		    animate_dest.h = 15;
		    animate_dest.x = blocks[i][j].block_x;
		    animate_dest.y = blocks[i][j].block_y;
		
		    SDL_RenderCopy(renderer, blocks[i][j].anim_sprite, &animate_src, &animate_dest);
		    
		}
	    }
	}
    }
    
}
void render_all() {
    SDL_RenderClear(renderer);
    render_blocks();
    SDL_RenderCopy(renderer, ball_t, NULL, &ball_rect);   
    SDL_RenderCopy(renderer, player_t, NULL, &player_rect);


    if (animate_flag) {
	sprite_sub++;
	if (sprite_sub == 10) {	    
	    animate_blocks();
	    sprite_sub = 0;
	}
    }
    
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
    for (int i = 0; i < BLOCKS_Y; i++) {
	for (int j = 0; j < BLOCKS_X; j++) {
	    
	    if (blocks[i][j].enabled == true) {		
		block_rect.x = blocks[i][j].block_x;
		block_rect.y = blocks[i][j].block_y;
		SDL_Rect inter_rect;
		if (SDL_IntersectRect(&block_rect, &ball_rect, &inter_rect)) {
		    printf("x collision %i ... %i\n", inter_rect.x, blocks[i][j].block_x);
		    
		    if ((inter_rect.x == blocks[i][j].block_x && ball_increase_x) ||
			(inter_rect.x == blocks[i][j].block_x + BLOCKS_W && !ball_increase_x)) {	
			ball_increase_x = !ball_increase_x;
		    } else {		    
			if ((inter_rect.y == blocks[i][j].block_y && ball_increase_y) ||
			    (inter_rect.y == blocks[i][j].block_y + BLOCKS_H && !ball_increase_y)) {	
			    ball_increase_y = !ball_increase_y;
			}
		    }

		    //-------------------------------------------
		    if (inter_rect.x != blocks[i][j].block_x &&
		    	inter_rect.x != blocks[i][j].block_x + BLOCKS_W &&
		    	inter_rect.y != blocks[i][j].block_y &&
		    	inter_rect.y != blocks[i][j].block_y + BLOCKS_H) {

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
			
			switch (smaller_index) {
			case 0:
			    ball_rect.x = ball_rect.x - m[0] - 1;
			    ball_increase_x = !ball_increase_x;
			    break;
			case 1:
			    ball_rect.x = ball_rect.x + m[1] + 1;
			    ball_increase_x = !ball_increase_x;
			    break;
			case 2:
			    ball_rect.y = ball_rect.y - m[2] - 1;
			    ball_increase_y = !ball_increase_y;
			    break;
			case 3:
			    ball_rect.y = ball_rect.y + m[3] + 1;
			    ball_increase_y = !ball_increase_y;
			    break;    
			}
			
			    printf("all values: %i %i %i %i\n", m[0], m[1], m[2], m[3]);
			    printf("smaller index: %i\n", smaller_index);
		    	/* printf("OPA\n"); */
		    	/* printf("inter x: %i .. block x: %i _ %i\n ", */
		    	/*        inter_rect.x, blocks[i][j].block_x, blocks[i][j].block_x + 35); */
		    	/* printf("inter y: %i .. block y: %i _ %i\n", */
		    	/*        inter_rect.y, blocks[i][j].block_y, blocks[i][j].block_y + 15); */
			
		    }
		    
		    if (blocks[i][j].unbreakable == false) {
			if (blocks[i][j].durability == 0) {
			    blocks[i][j].enabled = false;
			}
			if (blocks[i][j].durability != 0) {
			    blocks[i][j].durability--;			
			}
		    } else {
			// animate unbreakble block 			
		    }
		    
		}
	    }
	}
    }
}


void pong_ball() {

    if (ball_increase_x) {
	ball_rect.x += ball_increase_value_x;
	if (ball_rect.x >= WINDOW_W) { ball_increase_x = false; }
    } else {
	ball_rect.x -= ball_increase_value_x;
	if (ball_rect.x < 0) { ball_increase_x = true; }
    }
    
    if (ball_increase_y) {
	ball_rect.y += ball_increase_value_y;
	if (ball_rect.y >= WINDOW_H) { ball_increase_y = false; }
    } else {
	ball_rect.y -= ball_increase_value_y;
	if (ball_rect.y < 0) { ball_increase_y = true; }
    }
    
    check_ball_player_collision();
    
    check_ball_blocks_collision();

    render_all();
}

void init_level(int level){

    if (level == 1) {
	for (int i = 0; i < BLOCKS_Y; i++) {
	    for (int j = 0; j < BLOCKS_X; j++) {
		if (map1[i][j] == 1) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 0;
		    blocks[i][j].block_sprite = block_red_t;
		    blocks[i][j].block_x = 1 + (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = 1 + (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 2) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 0;
		    blocks[i][j].block_sprite = block_blue_t;
		    blocks[i][j].block_x = 1 + (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = 1 + (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 3) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 0;
		    blocks[i][j].block_sprite = block_green_t;
		    blocks[i][j].block_x = 1 + (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = 1 + (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 4) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].durability = 2;
		    blocks[i][j].block_sprite = block_yellow_t;
		    blocks[i][j].block_x = 1 + (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = 1 + (i*(BLOCKS_H+3));	    
		}
		if (map1[i][j] == 5) {
		    blocks[i][j].enabled = true;
		    blocks[i][j].unbreakable = true;
		    blocks[i][j].block_sprite = block_unbreak_t;
		    blocks[i][j].block_x = 1 + (j*(BLOCKS_W+3));
		    blocks[i][j].block_y = 1 + (i*(BLOCKS_H+3));	    
		}		
	    }
	}
    }
}

void animate_blocks() {
    
    bool global_animation = false;
    for (int i = 0; i < BLOCKS_Y; i++) {
	for (int j = 0; j < BLOCKS_X; j++) {
	    if (blocks[i][j].animate == true) {
		
		global_animation = true;
		animate_src.w = 16;
		animate_src.h = 8;
		animate_src.y = 24;
		animate_src.x = blocks[i][j].anim_frame * 16;
		
		animate_dest.w = 35;
		animate_dest.h = 15;
		animate_dest.x = blocks[i][j].block_x;
		animate_dest.y = blocks[i][j].block_y;
		
		SDL_RenderCopy(renderer, blocks[i][j].anim_sprite, &animate_src, &animate_dest);
		
		blocks[i][j].anim_frame++;
		if (blocks[i][j].anim_frame == blocks[i][j].anim_frame_max) {
		   blocks[i][j].animate = false;
		}
	    }
	}
    }
    if (global_animation == false) { animate_flag = false; }
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
	    pong_ball();
	    SDL_Delay(10);
	}


}

