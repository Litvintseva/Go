#include <stdio.h>
#include "SDL.h"

#ifdef _WIN32
#undef main
#endif

struct no;
typedef struct no node;
typedef struct
{
	unsigned int number;
	unsigned int maxcount;
	unsigned int count;
	unsigned int damecount;
	unsigned int color;
	node **stone;
	node **dame;
} group;
struct no
{
	unsigned int number;
	int str;
	int col;
	unsigned int color;
	group* gr;
};
group* CreateGroup(unsigned int nu, unsigned int max, unsigned int col)
{
	group* gr = malloc(sizeof(group));
	gr->number = nu;
	gr->maxcount = max;
	gr->count = 0;
	gr->damecount = 0;
	gr->color = col;
	gr->stone = malloc(sizeof(int) * max);
	gr->dame = malloc(sizeof(int) * max * 2);
	return gr;
}
node pole[19][19];
group* arrGgoup[360];
void CleanGroup(group* gr)
{
	free(gr->stone);
	free(gr->dame);
	free(gr);
}
group* ExtendGroup(group* gr)
{
	printf("Extend ");
	group* newgr = CreateGroup(gr->number, gr->maxcount * 2, gr->color);
	unsigned int i;
	for (i = 0; i < gr->count; i++)
	{
		newgr->stone[i] = gr->stone[i];
		newgr->stone[i]->gr = newgr;
	}
	for (i = 0; i < gr->damecount; i++)
	{
		newgr->dame[i] = gr->dame[i];
	}
	newgr->count = gr->count;
	newgr->damecount = gr->damecount;
	CleanGroup(gr);
	return newgr;
}
void TakeDame(group* gr, node *nod)
{
	printf("TakeDame");
	unsigned int i;
	for (i = 0; i < gr->damecount; i++)
		if (gr->dame[i]->number == nod->number)
		{
			gr->dame[i] = gr->dame[gr->damecount - 1];
			gr->damecount--;
		}
	printf("%d", gr->damecount);
}
void AddStore(group* gr, node *nod)
{
	printf("AddStone");
	unsigned int i, add = 1;
	if ((gr->count == gr->maxcount) || (gr->damecount == gr->maxcount*2)) gr = ExtendGroup(gr);
	for (i = 0; i < gr->count; i++)
		if (gr->stone[i]->number == nod->number) add = 0;
	if (add)
	{
		gr->stone[gr->count] = nod;
		gr->count++;
		nod->gr = gr;
		TakeDame(gr, nod);
	}
}
void AddDame(group* gr, node *nod)
{
	unsigned int i, add=1;
	if ((gr->count == gr->maxcount) || (gr->damecount == gr->maxcount*2)) gr = ExtendGroup(gr);
	for (i = 0; i < gr->damecount; i++)
		if (gr->dame[i]->number == nod->number) add = 0;
	if (add)
	{
		printf("AddDame");
		gr->dame[gr->damecount] = nod;
		gr->damecount++;
		printf("%d", gr->damecount);
	}
}
void DrawImage(SDL_Surface * screen, SDL_Surface * img, int x, int y, int w, int h, int sx, int sy)
{
	SDL_Rect desc; // координаты, куда нужно наложить часть.
	desc.x = x;
	desc.y = y;
	SDL_Rect src; // накладываемый прямоугольник.
	src.x = sx;
	src.y = sy;
	src.w = w;
	src.h = h;
	SDL_BlitSurface(img, &src, screen, &desc);
}
void CaptureGroup(SDL_Surface * screen, SDL_Surface * img, group* gr)
{
	printf("Capute ");
	unsigned int i;
	for (i = 0; i < gr->count; i++)
	{
		
		if ((gr->stone[i]->col - 1 >= 0) && (pole[gr->stone[i]->col - 1][gr->stone[i]->str].color != gr->stone[i]->color) && (pole[gr->stone[i]->col - 1][gr->stone[i]->str].color != 0))
			AddDame(pole[gr->stone[i]->col - 1][gr->stone[i]->str].gr, gr->stone[i]);
		if ((gr->stone[i]->col + 1 < 19) && (pole[gr->stone[i]->col + 1][gr->stone[i]->str].color != gr->stone[i]->color) && (pole[gr->stone[i]->col + 1][gr->stone[i]->str].color != 0))
			AddDame(pole[gr->stone[i]->col + 1][gr->stone[i]->str].gr, gr->stone[i]);
		if ((gr->stone[i]->str - 1 >= 0) && (pole[gr->stone[i]->col][gr->stone[i]->str - 1].color != gr->stone[i]->color) && (pole[gr->stone[i]->col][gr->stone[i]->str - 1].color != 0))
			AddDame(pole[gr->stone[i]->col][gr->stone[i]->str - 1].gr, gr->stone[i]);
		if ((gr->stone[i]->str + 1 < 19) && (pole[gr->stone[i]->col][gr->stone[i]->str + 1].color != gr->stone[i]->color) && (pole[gr->stone[i]->col][gr->stone[i]->str + 1].color != 0))
			AddDame(pole[gr->stone[i]->col][gr->stone[i]->str + 1].gr, gr->stone[i]);
		gr->stone[i]->color = 0;
		gr->stone[i]->gr = NULL;
		DrawImage(screen, img, 220 + gr->stone[i]->col * 20 - 7, 120 + gr->stone[i]->str * 20 - 7, 15, 15, 0, 0);
	}
	CleanGroup(gr);
}
void MergeGroup(group* gr1, group* gr2, node *nod)
{
	unsigned int i;
	printf("Merge");
	while ((gr1->count + gr2->count > gr1->maxcount) || (gr1->damecount + gr2->damecount > gr1->maxcount*2))	gr1 = ExtendGroup(gr1);
	for (i = 0; i < gr2->count; i++)
	{
		gr1->stone[gr1->count + i] = gr2->stone[i];
		gr1->stone[gr1->count + i]->gr = gr1;
	}
	gr1->count = gr1->count + gr2->count;
	for (i = 0; i < gr2->damecount; i++) AddDame(gr1, gr2->dame[i]);
	TakeDame(gr1, nod);
	CleanGroup(gr2);
}
unsigned int SameStone(node* nod, int str, int col, unsigned int color, unsigned int neighbor)
{
	if ((str >= 0) && (str < 19) && (col >= 0) && (col < 19) && (pole[col][str].color == color))
	{
		if ((neighbor) && (nod->gr != pole[col][str].gr)) MergeGroup(nod->gr, pole[col][str].gr, nod);
		else AddStore(pole[col][str].gr, nod);
		neighbor++;
	}
	return neighbor;
}
void OtherStone(node* nod, int str, int col, unsigned int color, SDL_Surface * screen, SDL_Surface * img)
{
	if ((str >= 0) && (str < 19) && (col >= 0) && (col < 19) && (pole[col][str].color != color) && (pole[col][str].color != 0))
	{
		TakeDame(pole[col][str].gr, nod);
		if (pole[col][str].gr->damecount == 0) CaptureGroup(screen, img, pole[col][str].gr);
	}
}
void EmptyNode(group* gr, int str, int col )
{
	if ((str >= 0) && (str < 19) && (col >= 0) && (col < 19) && (pole[col][str].color == 0))
		AddDame(gr, &pole[col][str]);
}
void PrintArr()
{
	unsigned int i, j;
	for (i = 0; i < 19; i++)
	{
		for (j = 0; j < 19; j++)
		{
			printf("%d %d number %d color %d \t", i, j, pole[i][j].number, pole[i][j].color);
		}
		printf("\n");
	}
}
SDL_Surface * LoadImage(char* name)
{
	SDL_Surface *image = SDL_LoadBMP(name);
	if (image == NULL){
		perror("SDL_LoadBMP Error: %s", SDL_GetError());
		abort();
	}
	return image;
}
void Move(int x, int y, unsigned int *move, unsigned int *groups, unsigned int *countpas, 	SDL_Surface * screen, 
	SDL_Surface * blackbmp, SDL_Surface * whitebmp, SDL_Surface * moveblack, SDL_Surface * movewhite, SDL_Surface * nodebmp)
{
	SDL_Surface * substitution;
	int col, str;
	x -= 220;
	y -= 120;
	if (x % 20 < 10) x /= 20;
	else x = x / 20 + 1;
	if (y % 20 < 10) y /= 20;
	else y = y / 20 + 1;
	str = y;
	col = x;

	if (pole[col][str].color == 0)
	{
		unsigned int neighbor = 0, color = 0;
		printf("move %d ", *move);
		if (*move % 2 == 1)
		{
			substitution = blackbmp;
			color = 1;
		}
		else
		{
			substitution = whitebmp;
			color = 2;
		}
		DrawImage(screen, substitution, 220 + x * 20 - 7, 120 + y * 20 - 7, 15, 15, 0, 0);
		pole[col][str].color = color;
		neighbor = SameStone(&pole[col][str], str - 1, col, color, neighbor);
		neighbor = SameStone(&pole[col][str], str + 1, col, color, neighbor);
		neighbor = SameStone(&pole[col][str], str, col - 1, color, neighbor);
		neighbor = SameStone(&pole[col][str], str, col + 1, color, neighbor);
		if (!neighbor)
		{
			arrGgoup[*groups] = CreateGroup(groups, 30, color);
			AddStore(arrGgoup[*groups], &pole[col][str]);
			groups++;
		}
		OtherStone(&pole[col][str], str - 1, col, color, screen, nodebmp);
		OtherStone(&pole[col][str], str + 1, col, color, screen, nodebmp);
		OtherStone(&pole[col][str], str, col - 1, color, screen, nodebmp);
		OtherStone(&pole[col][str], str, col + 1, color, screen, nodebmp);
		EmptyNode(pole[col][str].gr, str - 1, col);
		EmptyNode(pole[col][str].gr, str + 1, col);
		EmptyNode(pole[col][str].gr, str, col - 1);
		EmptyNode(pole[col][str].gr, str, col + 1);
		printf("%d %d number %d color %d gr %d \n", col, str, pole[col][str].number, pole[col][str].color, pole[col][str].gr);
		*countpas = 0;
		(*move)++;
		if (color == 1)DrawImage(screen, movewhite, 200, 20, 400, 60, 0, 0);
		else DrawImage(screen, moveblack, 200, 20, 400, 60, 0, 0);

	}
}
void IsThisTheEnd(int col, int str, int* b, int* w, int dec, int column)
{
	while (pole[col][str].color == 0 && col>=0 && col<19 && str>=0 && str<19)
	{
		if (dec)
		{
			if (column) col--;
			else str--;
		}
		else
		{
			if (column) col++;
			else str++;
		}
		if (col >= 0 && col < 19 && str >= 0 && str < 19)
		{
			if (pole[col][str].color == 1) (*b)++;
			if (pole[col][str].color == 2) (*w)++;
		}
	}
}
void ThisIsTheEnd(int i, int j, int dec, int column)
{
	int col = i, str = j;
	while (pole[i][j].color == 0 && col >= 0 && col<19 && str >= 0 && str<19)
	{
		if (dec)
		{
			if (column) col--;
			else str--;
		}
		else
		{
			if (column) col++;
			else str++;
		}
		if (col >= 0 && col < 19 && str >= 0 && str < 19) 
			pole[i][j].color = pole[col][str].color;
	}
}
void Pas(SDL_Surface * screen,	SDL_Surface * blackbmp, SDL_Surface * whitebmp, SDL_Surface * winblack, SDL_Surface * winwhite, SDL_Surface * noend)
{
	{
		int b = 0, w = 0, i, j;
		for (i = 0; i < 19; i++)
		{
			for (j = 0; j < 19; j++)
			{
				b = 0;
				w = 0;
				IsThisTheEnd(i, j, &b, &w, 1, 1);
				IsThisTheEnd(i, j, &b, &w, 1, 0);
				IsThisTheEnd(i, j, &b, &w, 0, 1);
				IsThisTheEnd(i, j, &b, &w, 0, 0);
				if (b && w) break;
			}
			if (b && w) break;
		}
		if (b && w) DrawImage(screen, noend, 200, 20, 400, 60, 0, 0);
		else
		{
			int result1 = 0, result2 = 0;
			for (i = 0; i < 19; i++)
				for (j = 0; j < 19; j++)
				{
					ThisIsTheEnd(i, j, 0, 1);
					ThisIsTheEnd(i, j, 0, 0);
					ThisIsTheEnd(i, j, 1, 1);
					ThisIsTheEnd(i, j, 1, 0);
					if (pole[i][j].color == 1)
					{
						DrawImage(screen, blackbmp, 220 + i * 20 - 7, 120 + j * 20 - 7, 15, 15, 0, 0);
						SDL_Flip(screen);
						result1++;
					}
					if (pole[i][j].color == 2)
					{
						DrawImage(screen, whitebmp, 220 + i * 20 - 7, 120 + j * 20 - 7, 15, 15, 0, 0);
						SDL_Flip(screen);
						result2++;
					}
				}
			printf("black - %d, white - %d", result1, result2);
			if (result1 > result2) DrawImage(screen, winblack, 200, 20, 400, 60, 0, 0);
			else DrawImage(screen, winwhite, 200, 20, 400, 60, 0, 0);
		}
	}
}

int main() {
	SDL_Event event;
	unsigned int done = 0, move = 1, i, j, groups = 1, countpas = 0;

	for (i = 0; i < 19; i++)
		for (j = 0; j < 19; j++)
		{
			pole[i][j].number = j * 19 + i;
			pole[i][j].str = j;
			pole[i][j].col = i;
			pole[i][j].color = 0;
			pole[i][j].gr = NULL;
		}

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		perror("Unable to init SDL: %s", SDL_GetError());
		return 1;
	}

	SDL_Surface * screen = SDL_SetVideoMode(800, 600, 16, SDL_HWSURFACE | SDL_DOUBLEBUF); /// 800x600 — таким будет размер нашего окна. А цвет будет 16-битным. Видео с аппаратным ускорением из-за параметра SDL_HWSURFACE
	// Если не поддерживается аппаратное ускорение, то может SDL_SWSURFACE(программное ускорение)

	if(!screen)
	{
		perror("Can't set videomode: %s", SDL_GetError());
		return 1;
	}

	SDL_Surface *polebmp = LoadImage("pole.bmp");
	SDL_Surface *pravila = LoadImage("pravila.bmp");
	SDL_Surface *noend = LoadImage("noend.bmp");
	SDL_Surface *movewhite = LoadImage("movewhite.bmp");
	SDL_Surface *moveblack = LoadImage("moveblack.bmp");
	SDL_Surface *winwhite = LoadImage("winwhite.bmp");
	SDL_Surface *winblack = LoadImage("winblack.bmp");
	SDL_Surface *blackbmp = LoadImage("black.bmp");
	SDL_Surface *whitebmp = LoadImage("white.bmp");
	SDL_Surface *nodebmp = LoadImage("yzel.bmp");
	SDL_Surface *pas = LoadImage("pas.bmp");

	DrawImage(screen, polebmp, 200, 100, 400, 400, 0, 0);
	DrawImage(screen, pas, 620, 460, 80, 40, 0, 0);
	DrawImage(screen, pravila, 0, 100, 200, 400, 0, 0);
	DrawImage(screen, moveblack, 200, 20, 400, 60, 0, 0);

	SDL_Flip(screen);

	while (!done)
	{
		SDL_WaitEvent(&event);
		switch(event.type)
		{
           case SDL_QUIT: // Событие выхода
			done = 1;
			break;
		   case SDL_MOUSEBUTTONDOWN:
		   {
			   if (event.button.button = SDL_BUTTON_LEFT)
			   {
				   int x = event.button.x, y = event.button.y;  // Координаты клика
				   if ((x >= 220) && (x <= 580) && (y >= 120) && (y <= 480))
				   {
					   Move(x, y, &move, &groups, &countpas, screen, blackbmp, whitebmp, moveblack, movewhite, nodebmp);
				   }
				   if ((x >= 620) && (x <= 700) && (y >= 460) && (y <= 500))
				   {
					   if (move % 2 == 1)
					   {
						   DrawImage(screen, movewhite, 200, 20, 400, 60, 0, 0);
					   }
					   else
					   {
						   DrawImage(screen, moveblack, 200, 20, 400, 60, 0, 0);
					   }
					   countpas++;
					   move++;
				   }
				   if (countpas == 2)
				   {
					   Pas(screen, blackbmp, whitebmp, winblack, winwhite, noend);
				   }
				   SDL_Flip(screen);

			   }
			   break;
		   }
		}

	}
	
	return 0;
}