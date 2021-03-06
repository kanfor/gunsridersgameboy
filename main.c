/* 
 * GUNS & RIDERS - Game Boy game
 * 
 * Author: J.M. Climent
 * email: kanfor@yahoo.es
 * 
 * This game was created usind gbdk lib.
 * http://gbdk.sourceforge.net/
 *
 * Created on 5 de March of 2016
 * 
 * ---GNU GENERAL PUBLIC LICENSE---
 */

#include <stdio.h>
#include <gb/gb.h>
#include <string.h>
#include <rand.h>
#include <stdlib.h>
#include "drawing.c"
#include "gfx.c"
#include "music.c"

// <editor-fold defaultstate="collapsed" desc="ENUMS, FUNCTIONS AND GLOBAL VARIABLES">
enum limits_game
{
    K_WIDTH = 160,
    K_HEIGHT = 144,
    K_LIMIT_UP = 40,
    K_LIMIT_DOWN = 112
};
//This enum is to count the total tiles of sprites I must to load.
enum sprites_tiles_8x8 {
    SP_COWBOY1,
    SP_COWBOY2,
    SP_COWBOY3,
    SP_COWBOY4,
    SP_ENEMY1,
    SP_ENEMY2,
    SP_ENEMY3,
    SP_ENEMY4,
    SP_LETTER_P,
    SP_LETTER_O,
    SP_LETTER_I,
    SP_LETTER_N,
    SP_LETTER_T,
    SP_LETTER_S,
    SP_LETTER_0,
    SP_LETTER_1,
    SP_LETTER_2,
    SP_LETTER_3,
    SP_LETTER_4,
    SP_LETTER_5,
    SP_LETTER_6,
    SP_LETTER_7,
    SP_LETTER_8,
    SP_LETTER_9,
    SP_BULLET,
    SP_ENEMY_DEAD1,
    SP_ENEMY_DEAD2,
    SP_PLAYER_DEAD1,
    SP_PLAYER_DEAD2,
    SP_LETTER_G1,
    SP_LETTER_A2,
    SP_LETTER_M3,
    SP_LETTER_E4,
    SP_LETTER_O5,
    SP_LETTER_V6,
    SP_LETTER_E7,
    SP_LETTER_R8,
    TOTAL_SPRITES_TILES_8X8
};

enum enum_state {
    CREDITS,
    MAIN_TITLE,
    GAME_PLAY,
    GAME_PAUSE,
    GAME_DEAD,
    GAME_OVER
} state;

//Enum of sprites only for player and enemies.
enum enum_sprites {
    SPRITE_COWBOY,
    SPRITE_ENEMY1,
    SPRITE_ENEMY2,
    SPRITE_ENEMY3,
    SPRITE_ENEMY4,
    TOTAL_SPRITES
};

//Constants dont work. We can use enums instead.

enum enum_total_elements {
    TOTAL_BULLETS = 5,
    TOTAL_ENEMYS = 3 //4
};

enum enum_first_tile_sprite {
    TILE_COWBOY = 0,
    TILE_ENEMY = 4,
    TILE_LETTER = 8,
    TILE_BULLET = 24,
    TILE_ENEMY_DEAD = 25,
    TILE_PLAYER_DEAD = 27,
    TILE_GAME_OVER = 29
};

enum enum_sprite_letter {
    SPRITE_LETTER = 10,
    SPRITE_BULLET = 26,
    SPRITE_BULLET_ENEMY = 32
};

typedef struct {
    UINT8 x_;
    UINT8 y_;
    BOOLEAN isAlive_;
    BOOLEAN isReadyToMove_;
    BOOLEAN isRightMove_;
} Bullet;
Bullet bulletPlayer[TOTAL_BULLETS];
Bullet bulletEnemy[TOTAL_ENEMYS];

typedef struct {
    UINT8 x_;
    UINT8 y_;
    BOOLEAN isAlive_;
    BOOLEAN isOnScreen_;
    UINT8 numSprite_;
    UINT8 numTile_;
    UINT8 currentFrame_;
    UINT16 timeToChangeFrame_;
    UINT8 timeToCheckCollision_;
    UINT8 timeDeadReset_;
    UINT8 movingTo_;
    UINT8 timeToShoot_;
    UINT8 clockToShoot_;
} Sprite;
Sprite sprite[TOTAL_SPRITES];

void draw_sprite_conwboy(UINT8 numSprite_, UINT8 x_, UINT8 y_);

void draw_sprite_enemy(UINT8 numSprite_, UINT8 x_, UINT8 y_);

void draw_sprite_enemy_dead(UINT8 numSprite_, UINT8 x_, UINT8 y_);

void draw_sprite_player_dead(UINT8 numSprite_, UINT8 x_, UINT8 y_);

void move_sprite_to(UINT8 numSprite_, UINT8 x_, UINT8 y_);

void update_sprite();

UINT8 get_random_y();

UINT8 get_random_direction();

UINT8 get_random_time_to_shoot();

UINT8 print_number(UINT8 number_);

void load_sprites_data();

void load_tiles_game();

void load_tiles_menu();

void paint_points();

void paint_game_over(UINT8 x_, UINT8 y_);

void add_points();

void update_hud();

void update_enemy_less();

void sound_init();

void sound_clean_noise();

void sound_shoot();

void sound_dead_enemy();

void sound_dead_player();

//Global variables. Dont use ints. Use UINT8 instead.
UINT8 clock;
UINT8 clock2;
UINT8 frame;
UINT8 pointsX;
UINT8 pointsY;
UINT8 pointsDigit1;
UINT8 pointsDigit2;
UINT8 pointsDigit3;
UINT8 countBulletPlayer;
UINT8 countBulletEnemy;
UINT8 HUDcountBullets;
UINT8 HUDcountEnemysLess;
// </editor-fold>

int main() {
    
    // <editor-fold defaultstate="collapsed" desc="INIT VARIABLES">
    //Variables.
    UINT8 i;
    UINT8 j;
    UINT8 keys = 0;
    BOOLEAN isInited = FALSE;
    UINT8 tempx = 0;
    UINT8 tempy = 0;
    UINT8 speedEnemy = 0;
    UINT8 scroll = 0;
    UINT8 clockSpeedEnemy = 0;
    UINT8 clockScroll = 0;
    UINT8 timeToMoveEnemy = 2;
    UINT8 timeToScroll = 4;
    UBYTE points[3];
    UBYTE moveUpDown, moveLeftRight;
    BOOLEAN isPossibleToShoot = TRUE;
    UINT8 timePauseShoot = 0;
    UINT8 totalTimePauseShoot = 15;
    UINT8 totalTimeCheclCollision = 2;
    UINT8 timeDeadEnemyReset = 20;
    BOOLEAN keyRelease = TRUE;
    UINT8 timeDeadToGameOver = 100;
    UINT8 clocktimeDeadToGameOver = 0;
    BOOLEAN stopGame = FALSE;
    
    UINT8 timeToNewEnemy = 250; //150;
    UINT8 clockToNewEnemy = 0;
    UINT8 countEnemey = 0;
    UINT8 clockToChangeDirection = 60;
    UINT8 counterToChangeDirection = 0;
    BOOLEAN isTimeToChangeDirection = FALSE;
    UINT8 countTimeReadyShootHUD = 0;
    UINT8 timeReadyShootHUD = 60;

    SPRITES_8x8;

    HIDE_BKG;
    HIDE_SPRITES;
    
    load_sprites_data();

    state = CREDITS; // </editor-fold>

    while (1) {
        
        Kwait_vbl_done();
        
        switch(state)
        {
            case CREDITS:
                if (!isInited) {
                    printf("\n \n \n \n    GUNS&RIDERS\n\n   MADE WITH GBDK\n\n CODE: J.M.CLIMENT\n MUSIC: MACHINET\n\n\n NOT PRODUCED BY OR\n UNDER LICENSE FROM\n NINTENTO");
                    isInited = TRUE;
                }
                tempx++;
                if (tempx > 200)
                {
                    tempx = 0;
                    state = MAIN_TITLE;
                    isInited = FALSE;
                    HIDE_BKG;
                }
                break;
            // <editor-fold defaultstate="collapsed" desc="MAIN TITLE">
            case MAIN_TITLE:
                if (!isInited) {
                    sound_init();
                    Kmove_bkg(0, 0);
                    load_tiles_menu();
                    isInited = TRUE;
                    timerCounter = 0; //Reset music.
                    currentBeat = 0;
                    moreSpeed = 0;
                    SHOW_BKG;
                    HIDE_SPRITES;
                    HIDE_WIN;
                }
                updateMusicMenu(); //Play the music.
                clock2++;
                if (clock2 > 25) {
                    Kset_bkg_tiles(0,14,20,1,gunsriders_press_on);
                }
                if (clock2 > 50) {
                    Kset_bkg_tiles(0,14,20,1,gunsriders_press_off);
                    clock2 = 0;
                }
                keys = Kjoypad();
                if (keys & (J_START)) {
                    HIDE_BKG;
                    HIDE_SPRITES;
                    HIDE_WIN;
                    load_tiles_game();
                    state = GAME_PLAY;
                    isInited = FALSE;
                }

                break; // </editor-fold>

            // <editor-fold defaultstate="collapsed" desc="GAME_PLAY">
            case GAME_PLAY:
                if (!isInited)
                {
                    isInited = TRUE;
                    
                    timerCounter = 0; //Reset music.
                    currentBeat = 0;
                    
                    HUDcountBullets = 5;
                    HUDcountEnemysLess = 5;
                    
                    //TOCHACO!!!!
                    //Variables.
                    pointsDigit1 = 0;
                    pointsDigit2 = 0;
                    pointsDigit3 = 0;
                    
                    keys = 0;
                    tempx = 0;
                    tempy = 0;
                    speedEnemy = 0;
                    scroll = 0;
                    clockSpeedEnemy = 0;
                    clockScroll = 0;
                    timeToMoveEnemy = 2;
                    timeToScroll = 4;
                    isPossibleToShoot = TRUE;
                    timePauseShoot = 0;
                    totalTimePauseShoot = 15;
                    totalTimeCheclCollision = 2;
                    timeDeadEnemyReset = 20;
                    keyRelease = TRUE;
                    timeDeadToGameOver = 100;
                    clocktimeDeadToGameOver = 0;
                    stopGame = FALSE;

                    timeToNewEnemy = 150;
                    clockToNewEnemy = 0;
                    countEnemey = 0;
                    clockToChangeDirection = 60;
                    counterToChangeDirection = 0;
                    isTimeToChangeDirection = FALSE;

                    clock = 0;
                    clock2 = 0;
                    frame = 0;
                    points[0] = 0;
                    points[1] = 0;
                    points[2] = 0;
                    pointsX = 48;
                    pointsY = 20;
                    pointsDigit1 = 0;
                    pointsDigit2 = 0;
                    pointsDigit3 = 0;
                    countBulletPlayer = 1;
                    countBulletEnemy = 0;

                    for (i = 0; i < TOTAL_BULLETS; i++)
                    {
                        bulletPlayer[i].isAlive_ = FALSE;
                        bulletPlayer[i].isRightMove_ = FALSE;
                        bulletPlayer[i].isReadyToMove_ = FALSE;
                        bulletPlayer[i].x_ = 20;
                        bulletPlayer[i].y_ = -100 + (10 * i);
                    }
                    for (i = 0; i < TOTAL_ENEMYS; i++)
                    {
                        bulletEnemy[i].isAlive_ = FALSE;
                        bulletEnemy[i].isRightMove_ = TRUE;
                        bulletEnemy[i].isReadyToMove_ = FALSE;
                        bulletEnemy[i].x_ = K_WIDTH + 10;
                        bulletEnemy[i].y_ = K_HEIGHT + (10 * i);
                    }

                    //Cowboy
                    sprite[SPRITE_COWBOY].x_ = 40;
                    sprite[SPRITE_COWBOY].y_ = 80;
                    sprite[SPRITE_COWBOY].isAlive_ = TRUE;
                    sprite[SPRITE_COWBOY].isOnScreen_ = FALSE;
                    sprite[SPRITE_COWBOY].numSprite_ = 0;
                    sprite[SPRITE_COWBOY].numTile_ = TILE_COWBOY;
                    sprite[SPRITE_COWBOY].currentFrame_ = 0;
                    sprite[SPRITE_COWBOY].timeToChangeFrame_ = 7;
                    sprite[SPRITE_COWBOY].movingTo_ = 1;
                    sprite[SPRITE_COWBOY].timeToShoot_ = 0;
                    sprite[SPRITE_COWBOY].clockToShoot_ = 0;
                    draw_sprite_conwboy(sprite[SPRITE_COWBOY].numSprite_, sprite[SPRITE_COWBOY].x_, sprite[SPRITE_COWBOY].y_);

                    //Enemy
                    for (i = 0; i < TOTAL_ENEMYS; i++) {
                        sprite[SPRITE_ENEMY1 + i].x_ = K_WIDTH + (i * 10) + 10;
                        sprite[SPRITE_ENEMY1 + i].y_ = get_random_y();
                        sprite[SPRITE_ENEMY1 + i].isAlive_ = TRUE;
                        sprite[SPRITE_ENEMY1 + i].isOnScreen_ = FALSE;
                        sprite[SPRITE_ENEMY1 + i].numSprite_ = 2 + (2 * i);
                        sprite[SPRITE_ENEMY1 + i].numTile_ = TILE_ENEMY;
                        sprite[SPRITE_ENEMY1 + i].currentFrame_ = 0;
                        sprite[SPRITE_ENEMY1 + i].timeToChangeFrame_ = 7;
                        sprite[SPRITE_ENEMY1 + i].timeToCheckCollision_ = 0;
                        sprite[SPRITE_ENEMY1 + i].timeDeadReset_ = 0;
                        sprite[SPRITE_ENEMY1 + i].movingTo_ = 1;
                        sprite[SPRITE_ENEMY1 + i].timeToShoot_ = 100;
                        sprite[SPRITE_ENEMY1 + i].clockToShoot_ = 0;
                        draw_sprite_enemy(sprite[SPRITE_ENEMY1 + i].numSprite_, sprite[SPRITE_ENEMY1 + i].x_, sprite[SPRITE_ENEMY1 + i].y_);
                    }
                    //Initial positions of bullets.
                    Kmove_sprite(SPRITE_BULLET, K_WIDTH + 10, 10);
                    Kmove_sprite(SPRITE_BULLET + 1, K_WIDTH + 10, 10);
                    Kmove_sprite(SPRITE_BULLET + 2, K_WIDTH + 10, 10);
                    Kmove_sprite(SPRITE_BULLET + 3, K_WIDTH + 10, 10);
                    Kmove_sprite(SPRITE_BULLET + 4, K_WIDTH + 10, 10);
                    Kmove_sprite(SPRITE_BULLET + 5, K_WIDTH + 10, 10);
                    Kmove_sprite(SPRITE_BULLET + 6, K_WIDTH + 10, 10);
                    Kmove_sprite(SPRITE_BULLET + 7, K_WIDTH + 10, 10);
                    
                    load_sprites_data();

                    SHOW_SPRITES;
                    SHOW_BKG;
                    SHOW_WIN;
                    
                    Kmove_win(7, K_HEIGHT - 16); //HUD
                }

                //////////
                //UPDATE//
                //////////
                update_sprite();

                // <editor-fold defaultstate="collapsed" desc="BULLETS">
                //////////////////
                //UPDATE BULLETS//
                //////////////////
                if (!stopGame)
                {
                    //HUD bullets.
                    if (HUDcountBullets < 5)
                    {
                        countTimeReadyShootHUD++;
                        if (countTimeReadyShootHUD > timeReadyShootHUD)
                        {
                            countTimeReadyShootHUD = 0;
                            HUDcountBullets++;
                            update_hud();
                        }
                    }
                
                    if (!isPossibleToShoot)
                    {
                        timePauseShoot++;
                        if (timePauseShoot > totalTimePauseShoot)
                        {
                            timePauseShoot = 0;
                            isPossibleToShoot = TRUE;
                        }
                    }
                    for (i = 1; i < TOTAL_BULLETS; i++)
                    {
                        if (bulletPlayer[i].isAlive_)
                        {
                            if (!bulletPlayer[i].isReadyToMove_)
                            {
                                sound_shoot();
                                bulletPlayer[i].x_ = sprite[SPRITE_COWBOY].x_ + 4;
                                bulletPlayer[i].y_ = sprite[SPRITE_COWBOY].y_ + 7;
                                bulletPlayer[i].isReadyToMove_ = TRUE;
                            }
                            bulletPlayer[i].x_ += 2;
                            Kmove_sprite(SPRITE_BULLET + i, bulletPlayer[i].x_, bulletPlayer[i].y_);
                            if (bulletPlayer[i].x_ > K_WIDTH + 5 && bulletPlayer[i].isReadyToMove_)
                            {
                                bulletPlayer[i].x_ = -10;
                                bulletPlayer[i].isAlive_ = FALSE;
                                bulletPlayer[i].isReadyToMove_ = FALSE;
                            }
                        }
                    }
                    //Bullets enemy.
                    for (i = 0; i < TOTAL_ENEMYS; i++)
                    {
                        if (bulletEnemy[i].isAlive_)
                        {
                            bulletEnemy[i].x_-= 2;
                            Kmove_sprite(SPRITE_BULLET_ENEMY + i, bulletEnemy[i].x_, bulletEnemy[i].y_);
                            //if (bulletEnemy[i].isAlive_)
                            //{
                                if (!bulletEnemy[i].isReadyToMove_)
                                {
                                    bulletEnemy[i].x_ = sprite[SPRITE_ENEMY1 + i].x_;
                                    bulletEnemy[i].y_ = sprite[SPRITE_ENEMY1 + i].y_ + 7;
                                    bulletEnemy[i].isReadyToMove_ = TRUE;
                                }
                                //Collision with player.
                                if (bulletEnemy[i].x_ < sprite[SPRITE_COWBOY].x_)
                                {
                                    if (bulletEnemy[i].x_ > sprite[SPRITE_COWBOY].x_ - 8)
                                    {
                                        if (bulletEnemy[i].y_ > sprite[SPRITE_COWBOY].y_)
                                        {
                                            if (bulletEnemy[i].y_ < sprite[SPRITE_COWBOY].y_ + 16)
                                            {
                                                sprite[SPRITE_COWBOY].isAlive_ = FALSE;
                                                bulletEnemy[i].x_ = K_WIDTH + 10;
                                                bulletEnemy[i].y_ = K_HEIGHT + (10 * i);
                                                bulletEnemy[i].isReadyToMove_ = FALSE;
                                                bulletEnemy[i].isAlive_ = FALSE;
                                                Kmove_sprite(SPRITE_BULLET_ENEMY + i, bulletEnemy[i].x_, bulletEnemy[i].y_);
                                                sound_dead_player();
                                            }
                                        }
                                    }
                                }
                            //}
                            if (bulletEnemy[i].x_ < 1 && bulletEnemy[i].isReadyToMove_ || bulletEnemy[i].x_ > K_WIDTH)
                            {
                                bulletEnemy[i].x_ = K_WIDTH + 10;
                                bulletEnemy[i].y_ = K_HEIGHT + (10 * i);
                                bulletEnemy[i].isReadyToMove_ = FALSE;
                                bulletEnemy[i].isAlive_ = FALSE;
                                Kmove_sprite(SPRITE_BULLET_ENEMY + i, bulletEnemy[i].x_, bulletEnemy[i].y_);
                            }
                        }
                    }
                }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="SCROLL">
                //////////
                //SCROLL//
                //////////
                if (!stopGame)
                {
                    clockSpeedEnemy++;
                    if (clockSpeedEnemy > timeToMoveEnemy) {
                        clockSpeedEnemy = 0;
                        speedEnemy = 1;
                    } else {
                        speedEnemy = 0;
                    }
                    clockScroll++;
                    if (clockScroll > timeToScroll) {
                        clockScroll = 0;
                        scroll = 1;
                    } else {
                        scroll = 0;
                    }
                    Kscroll_bkg(scroll, 0);
                }
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="SPRITES">
                //////////////////
                //UPDATE SPRITES//
                //////////////////
                
                //////////
                //Player//
                //////////
                if (sprite[SPRITE_COWBOY].isAlive_)
                {
                    updateMusicGameplay(); //Play the music.
                    draw_sprite_conwboy(sprite[SPRITE_COWBOY].numSprite_, sprite[SPRITE_COWBOY].x_, sprite[SPRITE_COWBOY].y_);
                }
                else
                {
                    //Preparing screen to clean of sprites.
                    if (clocktimeDeadToGameOver > timeDeadToGameOver)
                    {
                        if (!stopGame)
                        {
                            stopGame = TRUE;
                            for (i = 0; i < TOTAL_ENEMYS; i++)
                            { 
                                draw_sprite_enemy(sprite[SPRITE_ENEMY1 + i].numSprite_, K_WIDTH + 10, K_HEIGHT);
                            }
                            draw_sprite_conwboy(sprite[SPRITE_COWBOY].numSprite_, K_WIDTH + 10, K_HEIGHT);
                            for (i = 0; i < TOTAL_ENEMYS; i++)
                            { 
                                Kmove_sprite(SPRITE_BULLET_ENEMY + i, K_WIDTH + 10, K_HEIGHT);
                            }
                            timerCounter = 0; //Reset music.
                            currentBeat = 0;
                            sound_clean_noise();
                        }
                    }
                    else
                    {
                        sprite[SPRITE_COWBOY ].x_ -= scroll;
                        draw_sprite_player_dead(sprite[SPRITE_COWBOY].numSprite_, sprite[SPRITE_COWBOY ].x_, sprite[SPRITE_COWBOY ].y_);
                        clocktimeDeadToGameOver++;
                    }
                }

                /////////
                //ENEMY//
                /////////
                if (!stopGame)
                {
                    //This check when appear a new enemy.
                    clockToNewEnemy++;
                    if (clockToNewEnemy > timeToNewEnemy)
                    {
                        if (timeToNewEnemy > 80)
                        {
                            timeToNewEnemy -= 5;
                        }
                        else
                        {
                            timeToMoveEnemy = 1;
                        }
                        clockToNewEnemy = 0;
                        countEnemey++;
                        if (countEnemey > TOTAL_ENEMYS)
                        {
                            countEnemey = 0;
                        }
                        sprite[countEnemey].isOnScreen_ = TRUE;
                    }

                    counterToChangeDirection++;
                    if (counterToChangeDirection > clockToChangeDirection)
                    {
                        counterToChangeDirection = 0;
                        isTimeToChangeDirection = TRUE;
                    }
                    for (i = 0; i < TOTAL_ENEMYS; i++)
                    {   
                        if (sprite[SPRITE_ENEMY1 + i].isOnScreen_)
                        {
                            if (sprite[SPRITE_ENEMY1 + i].isAlive_)
                            {
                                //Shoot enemy bullet.
                                //if (i%2 == 0)
                                {
                                    if (!bulletEnemy[i].isAlive_)
                                    {
                                        sprite[SPRITE_ENEMY1 + i].clockToShoot_++;
                                    }
                                    if (sprite[SPRITE_ENEMY1 + i].clockToShoot_ > sprite[SPRITE_ENEMY1 + i].timeToShoot_)
                                    {
                                        bulletEnemy[i].isAlive_ = TRUE;
                                        sprite[SPRITE_ENEMY1 + i].clockToShoot_ = 0;
                                        sound_shoot();
                                    }
                                }
                                //Change direction.
                                if (isTimeToChangeDirection)
                                {
                                    sprite[SPRITE_ENEMY1 + i].movingTo_ = get_random_direction();
                                }
                                if (speedEnemy == 1)
                                {
                                    switch (sprite[SPRITE_ENEMY1 + i].movingTo_)
                                    {
                                        case 0:
                                            sprite[SPRITE_ENEMY1 + i].x_ -= speedEnemy;
                                            break;
                                        case 1:
                                            sprite[SPRITE_ENEMY1 + i].x_ -= speedEnemy;
                                            if (sprite[SPRITE_ENEMY1 + i].y_ > K_LIMIT_UP)
                                            {
                                                sprite[SPRITE_ENEMY1 + i].y_ -= speedEnemy;
                                            }
                                            break;
                                        case 2:
                                            sprite[SPRITE_ENEMY1 + i].x_ -= speedEnemy;
                                            if (sprite[SPRITE_ENEMY1 + i].y_ < K_LIMIT_DOWN)
                                            {
                                                sprite[SPRITE_ENEMY1 + i].y_ += speedEnemy;
                                            }
                                            break;
                                    }
                                    tempx = sprite[SPRITE_ENEMY1 + i].x_;
                                    tempy = sprite[SPRITE_ENEMY1 + i].y_;
                                    draw_sprite_enemy(sprite[SPRITE_ENEMY1 + i].numSprite_, tempx, tempy); 
                                    /////////////////////////
                                    //COLLISION WITH BULLET//
                                    /////////////////////////
                                    sprite[SPRITE_ENEMY1 + i].timeToCheckCollision_++;
                                    if (sprite[SPRITE_ENEMY1 + i].timeToCheckCollision_ > totalTimeCheclCollision)
                                    {
                                        sprite[SPRITE_ENEMY1 + i].timeToCheckCollision_ = 0;
                                        for (j = 1; j < TOTAL_BULLETS; j++)
                                        {
                                            if (bulletPlayer[j].isAlive_)
                                            {
                                                if (bulletPlayer[j].x_ > tempx - 16)
                                                {
                                                    if (bulletPlayer[j].x_ < tempx + 8)
                                                    {
                                                        if (bulletPlayer[j].y_ > tempy - 1)
                                                        {
                                                            if (bulletPlayer[j].y_ < tempy + 16)
                                                            {
                                                                sprite[SPRITE_ENEMY1 + i].isAlive_ = FALSE;
                                                                bulletPlayer[j].isAlive_ = FALSE;
                                                                bulletPlayer[j].isReadyToMove_ = FALSE;
                                                                Kmove_sprite(SPRITE_BULLET + j, -10, -10);
                                                                sound_dead_enemy();
                                                                add_points();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            //The enemy is laying dead on the floor.
                            {
                                sprite[SPRITE_ENEMY1 + i].x_ -= scroll;
                                tempx = sprite[SPRITE_ENEMY1 + i].x_;
                                tempy = sprite[SPRITE_ENEMY1 + i].y_;
                                draw_sprite_enemy_dead(sprite[SPRITE_ENEMY1 + i].numSprite_, tempx, tempy);

                                sprite[SPRITE_ENEMY1 + i].timeDeadReset_++;
                                if (sprite[SPRITE_ENEMY1 + i].timeDeadReset_ > timeDeadEnemyReset)
                                {
                                    sprite[SPRITE_ENEMY1 + i].timeDeadReset_ = 0;
                                    //Reset enemy.
                                    sprite[SPRITE_ENEMY1 + i].x_ = 170;
                                    sprite[SPRITE_ENEMY1 + i].y_ = get_random_y();
                                    sprite[SPRITE_ENEMY1 + i].isAlive_ = TRUE;
                                    sprite[SPRITE_ENEMY1 + i].isOnScreen_ = FALSE;
                                }
                            }
                        }
                        else
                        {
                            sprite[SPRITE_ENEMY1 + i].x_ = 170;
                            tempx = sprite[SPRITE_ENEMY1 + i].x_;
                            tempy = sprite[SPRITE_ENEMY1 + i].y_;
                            draw_sprite_enemy(sprite[SPRITE_ENEMY1 + i].numSprite_, tempx, tempy);
                        }
                        //Enemy is out of screen.
                        if (sprite[SPRITE_ENEMY1 + i].x_ < 1)
                        {
                            sprite[SPRITE_ENEMY1 + i].timeDeadReset_ = 0;
                            //Reset enemy.
                            sprite[SPRITE_ENEMY1 + i].x_ = 170;
                            sprite[SPRITE_ENEMY1 + i].y_ = get_random_y();
                            sprite[SPRITE_ENEMY1 + i].isAlive_ = TRUE;
                            sprite[SPRITE_ENEMY1 + i].isOnScreen_ = FALSE;
                            sprite[SPRITE_ENEMY1 + i].timeToShoot_ = 100;
                            //Hud.
                            HUDcountEnemysLess--;
                            update_enemy_less();
                            sound_dead_player();
                            if (HUDcountEnemysLess == 0)
                            {
                                sprite[SPRITE_COWBOY].isAlive_ = FALSE;
                                //clocktimeDeadToGameOver = timeDeadToGameOver + 1;
                            }
                        }
                    }
                    isTimeToChangeDirection = FALSE;
                }
                else
                {
                    paint_game_over(50,90);
                    updateMusicGameover();
                }
                // </editor-fold>

                //////////
                //POINTS//
                //////////
                paint_points();

                // <editor-fold defaultstate="collapsed" desc="CONTROLS">
                ////////////
                //CONTROLS//
                ////////////
                keys = Kjoypad();
                moveUpDown = 0;
                moveLeftRight = 0;
                if (sprite[SPRITE_COWBOY].isAlive_)
                {
                    if (keys & (J_RIGHT)) {
                    moveLeftRight = 1;
                    }
                    if (keys & (J_LEFT)) {
                        moveLeftRight = -1;
                    }
                    if (keys & (J_UP)) {
                        moveUpDown = -1;
                    }
                    if (keys & (J_DOWN)) {
                        moveUpDown = 1;
                    }
                    if (keys & (J_A) && keyRelease) {
                        keyRelease = FALSE;
                        if (isPossibleToShoot && HUDcountBullets != 0)
                        {
                            isPossibleToShoot = FALSE;
                            bulletPlayer[countBulletPlayer].isAlive_ = TRUE;
                            bulletPlayer[countBulletPlayer].isReadyToMove_ = FALSE;
                            countBulletPlayer++;
                            if (countBulletPlayer >= TOTAL_BULLETS)
                            {
                                countBulletPlayer = 1;
                            }
                            //HUD
                            if (HUDcountBullets > 0)
                            {
                                HUDcountBullets--;
                                countTimeReadyShootHUD = 0;
                            }
                            update_hud();
                        }
                    }
                    if (!( keys & J_A ))
                    {
                        keyRelease = TRUE;
                    }
                    if (keys) {
                        move_sprite_to(sprite[SPRITE_COWBOY].numSprite_, moveLeftRight, moveUpDown);
                    }
                }
                else
                {
                    if (keys & (J_A) || keys & (J_B) || keys & (J_START))
                    {
                        if (stopGame)
                        {
                            state = MAIN_TITLE;
                            isInited = FALSE;
                            HIDE_BKG;
                            HIDE_SPRITES;
                        }
                    }
                }
                break;
        }// </editor-fold>
    }// </editor-fold>
}                   

void move_sprite_to(UINT8 numSprite_, UINT8 x_, UINT8 y_)
{
    if (sprite[SPRITE_COWBOY].y_ > K_LIMIT_DOWN)
    {
        sprite[SPRITE_COWBOY].y_ = K_LIMIT_DOWN;
        y_ = 0;
    }
    if (sprite[SPRITE_COWBOY].y_ < K_LIMIT_UP)
    {
        sprite[SPRITE_COWBOY].y_ = K_LIMIT_UP;
        y_ = 0;
    }
    if (sprite[SPRITE_COWBOY].x_ < 8)
    {
        sprite[SPRITE_COWBOY].x_ = 8;
        x_ = 0;
    }
    if (sprite[SPRITE_COWBOY].x_ > K_WIDTH - 16)
    {
        sprite[SPRITE_COWBOY].x_ = K_WIDTH - 17;
        x_ = 0;
    }
    
    Kscroll_sprite(numSprite_, x_, y_);
    Kscroll_sprite(numSprite_ + 1, x_, y_);
    
    sprite[numSprite_].x_ += x_;
    sprite[numSprite_].y_ += y_;
}

void draw_sprite_conwboy(UINT8 numSprite_, UINT8 x_, UINT8 y_)
{
    int i = 0;
    if (frame == 0)
    {
        Kset_sprite_tile(numSprite_, TILE_COWBOY);
        Kset_sprite_tile(numSprite_ + 1, TILE_COWBOY + 1);
    }
    else
    {
        Kset_sprite_tile(numSprite_, TILE_COWBOY + 2);
        Kset_sprite_tile(numSprite_ + 1, TILE_COWBOY + 3);
    }
    Kmove_sprite(numSprite_, x_, y_);
    Kmove_sprite(numSprite_ + 1, x_, y_ + 8);
}

void draw_sprite_enemy(UINT8 numSprite_, UINT8 x_, UINT8 y_)
{
    int i = 0;
    if (frame == 0)
    {
        Kset_sprite_tile(numSprite_ + (2 * i), TILE_ENEMY);
        Kset_sprite_tile(numSprite_+ 1 + (2 * i) , TILE_ENEMY + 1);
    }
    else
    {
        Kset_sprite_tile(numSprite_ + (2 * i), TILE_ENEMY + 2);
        Kset_sprite_tile(numSprite_ + 1 + (2 * i) , TILE_ENEMY + 3);
    }
    Kmove_sprite(numSprite_, x_, y_);
    Kmove_sprite(numSprite_ + 1, x_, y_ + 8);
}

void draw_sprite_enemy_dead(UINT8 numSprite_, UINT8 x_, UINT8 y_)
{
    Kset_sprite_tile(numSprite_, TILE_ENEMY_DEAD);
    Kset_sprite_tile(numSprite_+ 1, TILE_ENEMY_DEAD + 1);
    
    Kmove_sprite(numSprite_, x_, y_ + 8);
    Kmove_sprite(numSprite_ + 1, x_ + 8, y_ + 8);
}

void draw_sprite_player_dead(UINT8 numSprite_, UINT8 x_, UINT8 y_)
{
    Kset_sprite_tile(numSprite_, TILE_PLAYER_DEAD);
    Kset_sprite_tile(numSprite_+ 1, TILE_PLAYER_DEAD + 1);
    
    Kmove_sprite(numSprite_, x_, y_ + 8);
    Kmove_sprite(numSprite_ + 1, x_ + 8, y_ + 8);
}

void update_sprite()
{
    clock++;
    if (clock > 7)
    {
        clock = 0;
        frame++;
        
        if (frame > 1)
        {
            frame = 0;
        }
    }
}

UINT8 get_random_y()
{
    UINT8 random = Krand();
    
    while (random > K_LIMIT_DOWN - K_LIMIT_UP)
    {
        random = Krand();
    }
    
    return random + K_LIMIT_UP;
}

UINT8 get_random_direction()
{
    UINT8 direction = Krand();
    
    while (direction > 2)
    {
        direction = Krand();
    }
    
    return direction;
}

UINT8 get_random_time_to_shoot()
{
    UINT8 time = Krand();
    
    while (time > 100)
    {
        time = Krand();
    }
    return time;
}

UINT8 print_number(UINT8 number_)
{
    switch(number_)
    {
        case 0:
            return SPRITE_LETTER + 4;
            break;
        case 1:
            return SPRITE_LETTER + 5;
            break;
        case 2:
            return SPRITE_LETTER + 6;
            break;
        case 3:
            return SPRITE_LETTER + 7;
            break;
        case 4:
            return SPRITE_LETTER + 8;
            break;
        case 5:
            return SPRITE_LETTER + 9;
            break;
        case 6:
            return SPRITE_LETTER + 10;
            break;
        case 7:
            return SPRITE_LETTER + 11;
            break;
        case 8:
            return SPRITE_LETTER + 12;
            break;
        case 9:
            return SPRITE_LETTER + 13;
            break;
    }
}

void load_sprites_data()
{
    int i = 0;
    Kset_sprite_data(0, TOTAL_SPRITES_TILES_8X8, sprites);
    
    //Sprites.
    Kset_sprite_tile(sprite[SPRITE_COWBOY].numSprite_, TILE_COWBOY);
    Kset_sprite_tile(sprite[SPRITE_COWBOY].numSprite_ + 1, TILE_COWBOY + 1);
    
    for (i = 0; i < TOTAL_ENEMYS; i++)
    {
        Kset_sprite_tile(sprite[SPRITE_ENEMY1 + i].numSprite_, TILE_COWBOY);
        Kset_sprite_tile(sprite[SPRITE_ENEMY1 + i].numSprite_ + 1, TILE_COWBOY + 1);
    }
    
    //Fonts.
    for (i = 0; i < 16; i++)
    {
        Kset_sprite_tile(SPRITE_LETTER + i, TILE_LETTER + i);
    }
    Kset_sprite_tile(SPRITE_LETTER + 6, print_number(pointsDigit1));
    Kset_sprite_tile(SPRITE_LETTER + 7, print_number(pointsDigit2));
    Kset_sprite_tile(SPRITE_LETTER + 8, print_number(pointsDigit3));
    
    //Bullets of player.
    for (i = 0; i < TOTAL_BULLETS; i++)
    {
        Kset_sprite_tile(SPRITE_BULLET + i, TILE_BULLET);
    }
    
    //Bullets of enemy.
    for (i = 0; i < TOTAL_ENEMYS; i++)
    {
        Kset_sprite_tile(SPRITE_BULLET_ENEMY + i, TILE_BULLET);
    }
}

void paint_points()
{
    //Text.
    Kmove_sprite(SPRITE_LETTER, pointsX, pointsY);
    Kmove_sprite(SPRITE_LETTER + 1, pointsX + 8, pointsY);
    Kmove_sprite(SPRITE_LETTER + 2, pointsX + 16, pointsY);
    Kmove_sprite(SPRITE_LETTER + 3, pointsX + 24 , pointsY);
    Kmove_sprite(SPRITE_LETTER + 4, pointsX + 32, pointsY);
    Kmove_sprite(SPRITE_LETTER + 5, pointsX + 40, pointsY);
    //Numbers.
    Kmove_sprite(SPRITE_LETTER + 6, pointsX + 56, pointsY);
    Kmove_sprite(SPRITE_LETTER + 7, pointsX + 64, pointsY);
    Kmove_sprite(SPRITE_LETTER + 8, pointsX + 72, pointsY);
}

void paint_game_over(UINT8 x_, UINT8 y_)
{
    Kset_sprite_tile(SPRITE_BULLET, TILE_GAME_OVER);
    Kset_sprite_tile(SPRITE_BULLET + 1, TILE_GAME_OVER + 1);
    Kset_sprite_tile(SPRITE_BULLET + 2, TILE_GAME_OVER + 2);
    Kset_sprite_tile(SPRITE_BULLET + 3, TILE_GAME_OVER + 3);
    Kset_sprite_tile(SPRITE_BULLET + 4, TILE_GAME_OVER + 4);
    Kset_sprite_tile(SPRITE_BULLET + 5, TILE_GAME_OVER + 5);
    Kset_sprite_tile(SPRITE_BULLET + 6, TILE_GAME_OVER + 6);
    Kset_sprite_tile(SPRITE_BULLET + 7, TILE_GAME_OVER + 7);
    
    Kmove_sprite(SPRITE_BULLET, x_, y_);
    Kmove_sprite(SPRITE_BULLET + 1, x_ + 8, y_);
    Kmove_sprite(SPRITE_BULLET + 2, x_ + 16, y_);
    Kmove_sprite(SPRITE_BULLET + 3, x_ + 24, y_);

    Kmove_sprite(SPRITE_BULLET + 4, x_ + 40, y_);
    Kmove_sprite(SPRITE_BULLET + 5, x_ + 48, y_);
    Kmove_sprite(SPRITE_BULLET + 6, x_ + 56, y_);
    Kmove_sprite(SPRITE_BULLET + 7, x_ + 64, y_); 
}

void update_hud()
{
    switch(HUDcountBullets)
    {
        case 5:
            Kset_win_tiles(5, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(5, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(4, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(4, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(3, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(3, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(2, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(2, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(1, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(1, 1, 1, 1, hud_bullet_down);
            break;
        case 4:
            Kset_win_tiles(5, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(5, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(4, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(4, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(3, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(3, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(2, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(2, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(1, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(1, 1, 1, 1, hud_bullet_down);
            break;
        case 3:
            Kset_win_tiles(5, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(5, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(4, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(4, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(3, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(3, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(2, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(2, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(1, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(1, 1, 1, 1, hud_bullet_down);
            break;
        case 2:
            Kset_win_tiles(5, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(5, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(4, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(4, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(3, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(3, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(2, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(2, 1, 1, 1, hud_bullet_down);
            Kset_win_tiles(1, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(1, 1, 1, 1, hud_bullet_down);
            break;
        case 1:
            Kset_win_tiles(5, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(5, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(4, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(4, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(3, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(3, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(2, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(2, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(1, 0, 1, 1, hud_bullet_up);
            Kset_win_tiles(1, 1, 1, 1, hud_bullet_down);
            break;
        case 0:
            Kset_win_tiles(5, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(5, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(4, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(4, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(3, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(3, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(2, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(2, 1, 1, 1, hud_clean_down);
            Kset_win_tiles(1, 0, 1, 1, hud_clean_up);
            Kset_win_tiles(1, 1, 1, 1, hud_clean_down);
            break;
    }
}

void update_enemy_less()
{
    switch(HUDcountEnemysLess)
    {
        case 4:
            Kset_win_tiles(18, 0, 1, 1, hud_enemy_less_4_up);
            Kset_win_tiles(18, 1, 1, 1, hud_enemy_less_4_down);
            break;
        case 3:
            Kset_win_tiles(18, 0, 1, 1, hud_enemy_less_3_up);
            Kset_win_tiles(18, 1, 1, 1, hud_enemy_less_3_down);
            break;
        case 2:
            Kset_win_tiles(18, 0, 1, 1, hud_enemy_less_2_up);
            Kset_win_tiles(18, 1, 1, 1, hud_enemy_less_2_down);
            break;
        case 1:
            Kset_win_tiles(18, 0, 1, 1, hud_enemy_less_1_up);
            Kset_win_tiles(18, 1, 1, 1, hud_enemy_less_1_down);
            break;
        case 0:
            Kset_win_tiles(18, 0, 1, 1, hud_enemy_less_0_up);
            Kset_win_tiles(18, 1, 1, 1, hud_enemy_less_0_down);
            break;
    }
}

void add_points()
{
    pointsDigit3++;
    if (pointsDigit3 > 9)
    {
        pointsDigit3 = 0;
        pointsDigit2++;
    }
    if (pointsDigit2 > 9)
    {
        pointsDigit2 = 0;
        pointsDigit1++;
    }
    
    Kset_sprite_tile(SPRITE_LETTER + 6, print_number(pointsDigit1));
    Kset_sprite_tile(SPRITE_LETTER + 7, print_number(pointsDigit2));
    Kset_sprite_tile(SPRITE_LETTER + 8, print_number(pointsDigit3));
}

void load_tiles_game()
{
    int i = 0;
    Kset_bkg_data(0, 50 + 38, bkgdata);
    
    //Clean garbage memery.
    for (i = 0; i < 17; i++)
    {
        Kset_bkg_tiles(0,i,50,1,clear);
    }

    Kset_bkg_tiles(0, 0, 32, 18 - 2, background_map);
    
    //HUD
    Kset_win_tiles(0, 0, 20, 2, hud_map);
}

void load_tiles_menu()
{
    int i = 0;
    Kset_bkg_data(0, 154, menudata);
    
    //Clean garbage memery.
    for (i = 0; i < 17; i++)
    {
        Kset_bkg_tiles(0,i,32,1,menuClear);
    }
    
    Kset_bkg_tiles(0,0,20,18,gunsriders_map);
}

void sound_init()
{
    NR52_REG = 0xFFU;
    NR51_REG = 0x00U;
    NR50_REG = 0x77U;
}

void sound_dead_enemy()
{
    NR41_REG = 0x0FU;
    NR42_REG = 0xF3U;
    NR43_REG = 100;
    NR44_REG = 0xC0U;
    
    muteChannel4 = 1;
}

void sound_dead_player()
{
    NR10_REG = 0x1E;
    NR11_REG = 0x10;
    NR12_REG = 0xF3;
    NR13_REG = 0x00;
    NR14_REG = 0x87;
    
    muteChannel1 = 1;
}

void sound_clean_noise()
{
    NR41_REG = 1;//0x0FU; //Duration
    NR42_REG = 1;         //Volumen
    NR43_REG = 1;         //Tone
    NR44_REG = 0xC0U;
    NR51_REG |= 0x88;
}

void sound_shoot()
{
    NR41_REG = 0x0FU;
    NR42_REG = 0xF3U;
    NR43_REG = 128;
    NR44_REG = 0xC0U;
    
    muteChannel4 = 1;
}