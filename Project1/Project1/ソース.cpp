//残りやりたいこと
//クリアの表記
//BGM,SEのついか
//撃墜数の表記


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define SCREEN_WIDTH    (32)
#define SCREEN_HEIGHT   (24)

#define FPS             (10)
#define INTERVAL        (1000 / FPS) 

#define INVADER_COLUMN  (11)
#define INVADER_ROW     (5)

enum 
{
	TILE_NONE,
	TILE_INVADER,
	TILE_PLAYER,
	TILE_PLAYER_BULLET,
	TILE_INVADER_BULLET,
	TILE_MAX
};

enum
{
	DIRECTION_RIGHT,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_MAX
};

typedef struct {
	int x, y;
}VEC2;

typedef struct
{
	int x, y;
	bool isDead;
}INVADER;

typedef struct
{
	int x, y;
}PLAYER;

typedef struct
{
	int x, y;
	bool isFired; //Playerの弾が発射されてるかのフラグ
}PLAYERBULLET;

typedef struct
{
	int x, y;
	bool isFired; //INVADERの弾が発射されてるかのフラグ
}INVADERRBULLET;

VEC2 directions[] = 
{
	{ 1,0},//DIRECTION_RIGHT,
	{ 0,1},//DIRECTION_DOWN,
    {-1,0}//DIRECTION_LEFT,
};

int screen[SCREEN_HEIGHT][SCREEN_WIDTH];
INVADER invaders[INVADER_ROW][INVADER_COLUMN];
PLAYER player;
PLAYERBULLET playerBullet;
INVADERRBULLET invaderBullet[INVADER_COLUMN];

int invaderDirection;
int invaderCount;

const char* tileAA[TILE_MAX] =
{
	"  ",  //TILE_NONE
	"★",  //TILE_INVADER
	"▲",  //TILE_PLAYER
	"↑",  //TILE_PLAYER_BULLET
	"↓",  //TILE_INVADER_BULLET
};

void DrawScreen() //画面に描画する
{
	memset(screen, 0, sizeof screen);
	for (int y = 0; y < INVADER_ROW; y++)
		for (int x = 0; x < INVADER_COLUMN; x++)
			if (!invaders[y][x].isDead)
			screen[invaders[y][x].y][invaders[y][x].x] = TILE_INVADER;

	screen[player.y][player.x] = TILE_PLAYER;

	if(playerBullet.isFired)
		screen[playerBullet.y][playerBullet.x] = TILE_PLAYER_BULLET;

	for (int x = 0; x < INVADER_COLUMN; x++)
		if (invaderBullet[x].isFired)
			screen[invaderBullet[x].y][invaderBullet[x].x] = TILE_INVADER_BULLET;

	system("cls");
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++)
			printf("%s", tileAA[screen[y][x]]);
		printf("\n");
	}
}

void Init()
{
	for (int y = 0; y < INVADER_ROW; y++)
		for (int x = 0; x < INVADER_COLUMN; x++)
		{
			invaders[y][x].x = x * 2;
			invaders[y][x].y = y * 2;
			invaders[y][x].isDead = false;
		}
	invaderDirection = DIRECTION_RIGHT;
	invaderCount = 0;

	player.x = SCREEN_WIDTH / 2;
	player.y = SCREEN_HEIGHT - 2;

	playerBullet. isFired = false;

	for (int x = 0; x < INVADER_COLUMN; x++)
		invaderBullet[x].isFired = false;

	DrawScreen();
}
bool playerBulletIntersectInvaders() //playerとinvadersの当たり判定
{
	for (int y = 0; y < INVADER_ROW; y++)
		for (int x = 0; x < INVADER_COLUMN; x++)
			if ((!invaders[y][x].isDead)
				&& (invaders[y][x].x == playerBullet.x)
				&& (invaders[y][x].y == playerBullet.y))
			{
				invaders[y][x].isDead = true;
				playerBullet.isFired = false; //この行を消すと貫通弾になる
				return true;
			}
	return false;
}

bool invaderBulleterPlayer()
{
	for(int x = 0; x < INVADER_COLUMN; x++)
		if (invaderBullet[x].isFired
			&& (invaderBullet[x].x == player.x)
			&& (invaderBullet[x].y == player.y)) 
		{
			return true;
		}
	return false;
}
int main()
{
	Init();
	
	clock_t lastClock = clock();
	while (1)
	{
		clock_t nowClock = clock();
		if (nowClock >= lastClock + INTERVAL) 
		{
			lastClock = nowClock;

			if (playerBullet.isFired)  //弾の動き
			{
				playerBullet.y--;
				if (playerBullet.y < 0)
					playerBullet.isFired = false;

				playerBulletIntersectInvaders();
			}

			invaderCount++;
			if (invaderCount > 10)
			{
				invaderCount = 0;

				int nextDirection = invaderDirection;
				for (int y = 0; y < INVADER_ROW; y++)
					for (int x = 0; x < INVADER_COLUMN; x++)
					{
						if (invaders[y][x].isDead)
							continue;

						invaders[y][x].x += directions[invaderDirection].x;
						invaders[y][x].y += directions[invaderDirection].y;

						switch (invaderDirection)
						{
						case DIRECTION_RIGHT:
							if (invaders[y][x].x >= SCREEN_WIDTH - 1)
								nextDirection = DIRECTION_DOWN;
							break;
						case DIRECTION_DOWN:
							if (invaders[y][x].x >= SCREEN_WIDTH - 1)
								nextDirection = DIRECTION_LEFT;
							if (invaders[y][x].x <= 0)
								nextDirection = DIRECTION_RIGHT;
							break;
						case DIRECTION_LEFT:
							if (invaders[y][x].x <= 0)
								nextDirection = DIRECTION_DOWN;
							break;
						}
					}
				invaderDirection = nextDirection;

				for (int x = 0; x < INVADER_COLUMN; x++)
				{
					if (!invaderBullet[x].isFired)
					{
						if (rand() % INVADER_COLUMN)
							continue;

						int invaderRow = -1;
						for (int y = 0; y < INVADER_ROW; y++)
						{
							if (!invaders[y][x].isDead)
								invaderRow = y;
						}

						if (invaderRow > 0)
						{
							invaderBullet[x].isFired = true;
							invaderBullet[x].x = invaders[invaderRow][x].x;
							invaderBullet[x].y = invaders[invaderRow][x].y + 1;
						}
					}
				}
			}

			for (int x = 0; x < INVADER_COLUMN; x++)
				if (invaderBullet[x].isFired)
				{
					invaderBullet[x].y++;
					if (invaderBullet[x].y >= SCREEN_HEIGHT)
						invaderBullet[x].isFired = false;
				}
			if (invaderBulleterPlayer()) //invaderの弾がplayerに当たったら最初から
			{
				Init();
				lastClock = clock();
				continue;
			}
			
			DrawScreen();
		}

		if (_kbhit())  //Playerの移動 //弾の動き
		{
			switch (_getch())
			{
			case 'a': player.x--; break;
			case 'd': player.x++; break;
			default:
				if (playerBullet.isFired)
					break;

				playerBullet.x = player.x;
				playerBullet.y = player.y - 1;
				playerBullet.isFired = true;
				break;
			}
			if (player.x < 0) //画面外に行かない処理
				player.x = 0;
			if (player.x >= SCREEN_WIDTH)
				player.x = SCREEN_WIDTH - 1;

			if (invaderBulleterPlayer()) //invaderの弾がplayerに当たったら最初から
			{
				Init();
				continue;
			}

			DrawScreen();
		}
	}
}
