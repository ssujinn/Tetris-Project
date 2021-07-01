#include "tetris.h"

static struct sigaction act, oact;
int rankNum = 0;
RecNode *mNode = NULL;
int side_touched = 0;
int downdown = 0;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));
	createRankList();

	recRoot = (RecNode*)malloc(sizeof(RecNode));
	recRoot->level = 0;
	mrRoot = (MoNode*)malloc(sizeof(MoNode));
	mrRoot->lv = -1;
	mrRoot->score = 0;
	for (int i = 0; i < HEIGHT; i++){
		for (int j = 0; j < WIDTH; j++){
			recRoot->recField[i][j] = field[i][j];
		}
	}
	
	mrRoot->f=field;
	recRoot->accumulatedScore = 0;
	constructRecTree(mrRoot);
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECOMMEND: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	//free(recRoot);
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	for (int i = 0; i < HEIGHT; i++){
		for (int j = 0; j < WIDTH; j++){
			recRoot->recField[i][j] = field[i][j];
		}
	}
	
	modified_recommend(mrRoot, 0);
	mNode = NULL;
	recommend(recRoot);
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		if (VISIBLE_BLOCKS >=2){
			move(4+i,WIDTH+13);
			for( j = 0; j < 4; j++ ){
				if( block[nextBlock[1]][0][i][j] == 1 ){
					attron(A_REVERSE);
					printw(" ");
					attroff(A_REVERSE);
				}
				else printw(" ");
			}
		}
		if (VISIBLE_BLOCKS >=3){
			move(10+i, WIDTH+13);
			for (j = 0; j < 4; j++){
				if (block[nextBlock[2]][0][i][j] == 1){
					attron(A_REVERSE);
					printw(" ");
					attroff(A_REVERSE);
				}
				else
					printw(" ");
			}
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j;

	for (i = 0; i < 4; i++){
		for (j = 0; j < 4; j++){
			if (block[currentBlock][blockRotate][i][j] == 1){
				// 예상된 현재 블록이 필드를 벗어나면 return 0
				if (blockY + i >= HEIGHT || blockX + j < 0 || blockX + j >= WIDTH)
					return 0;
				// 예상된 현재 블록이 쌓여 있는 블록과 겹칠 경우 return 0
				if (f[blockY + i][blockX + j] == 1)
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j;
	int preX, preY, preR;	// 블록의 이전 정보를 저장하기 위한 변수

	preX = blockX;
	preY = blockY;
	preR = blockRotate;
	
	// 바뀌기 전 블록의 정보를 얻기 위한 switch문
	switch (command){
		case KEY_UP:	
			if (blockRotate == 0)
				preR = 3;
			else
				preR--;
			break;
		case KEY_DOWN:
			preY++;
			break;
		case KEY_LEFT:
			preX++;
			break;
		case KEY_RIGHT:
			preX--;
			break;
	}

	// 이전에 그려진 블록을 화면에서 지운다
	for (i = 0; i < 4; i++){
		for (j = 0; j < 4; j++){
			if (block[currentBlock][preR][i][j] == 1){
				if (i + preY >= 0 && i + preY <= HEIGHT){
					move(i + preY + 1, j + preX + 1);
					printw(".");
				}
			}
		}
	}
	DrawOutline();

	// 현재 블록을 화면에 그려준다
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);

	// 커서를 필드 밖으로 이동
	move(HEIGHT, WIDTH + 10);
}

void BlockDown(int sig){
	// user code
	timed_out = 0;
	
	// 블록이 한 칸 내려갈 수 있으면
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)){
		blockY++;
	}
	// 블록을 더이상 내릴 수 없을 경우
	else if (blockY == -1)
		gameOver = 1;
	else{
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand()%7;
		blockRotate = 0;
		mNode = NULL;
		blockY = -1;
		blockX = WIDTH / 2 - 2;
		modified_recommend(mrRoot, 0);
		DrawNextBlock(nextBlock);
		for (int i = 0; i < HEIGHT; i++){
			for (int j = 0; j < WIDTH; j++){
				recRoot->recField[i][j] = field[i][j];
			}
		}
		recommend(recRoot);
		PrintScore(score);
		DrawField();
	}
	DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j, k = 1;
	int touched = 0;
  side_touched = 0;

  downdown = 0;
	for (i = blockY; i < blockY + 4; i++){
		for (j = blockX; j < blockX + 4; j++){
			if (block[currentBlock][blockRotate][i-blockY][j-blockX] == 1){
				f[i][j] = 1;
				if (f[i + 1][j] || i + 1 >= HEIGHT)
					touched++;
        if (blockX + j + 1 < WIDTH &f[blockY + i][blockX + j + 1])
          side_touched++;
        if (blockX + j - 1 >= 0 && f[blockY + i][blockX + j - 1])
          side_touched++;

        while(blockY + i + k < HEIGHT){
          if (f[blockY + i + k][blockX + j])
            break;
          if (!f[blockY + i + k][blockX + j])
            downdown++;
          k++;
        }
			}
		}
	}

	return 10 * touched;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i, j, k, flag, cnt = 0;

	for (i = HEIGHT - 1; i >= 0; i--){
		flag = 0;
		for (j = 0; j < WIDTH; j++){
			if (!f[i][j]){
				flag = 1;
				break;
			}
		}
		if (!flag){
			for (j = i; j > 0; j--){
				if (j == 0){
					for (k = 0; k < WIDTH; k++)
						f[j][k] = 0;
				}
				else{
					for (k = 0; k < WIDTH; k++)
						f[j][k] = f[j - 1][k];
		}	
			}
			DrawField();
			i++;
			cnt++;
		}
	}

	return cnt * cnt * 100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int i;
	for (i = y; i < HEIGHT; i++){
		if (CheckToMove(field, nextBlock[0], blockRotate, i, x) == 0)
			break;
	}
	DrawBlock(i - 1, x, nextBlock[0], blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	RecNode *ptr;

	ptr = mNode;
	for (int i = 0; i < VISIBLE_BLOCKS - 1; i++)
		ptr = ptr -> parent;
	DrawRecommend(ptr->recBlockY, ptr->recBlockX, ptr->curBlockID, ptr->recBlockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
}

void createRankList(){
	// user code
	FILE *fp = fopen("rank.txt", "r");
	int i;
	Node *ptr;
	ptr = head;

	if (fp == NULL)
		return;
	else{
		fscanf(fp, "%d", &rankNum);
		for (i = 0; i < rankNum; i++){
			Node* newNode = (Node*)malloc(sizeof(Node));
			fscanf(fp, "%s", newNode->name);
			fscanf(fp, "%d", &(newNode->score));
			newNode->link = NULL;
			if (head){
				ptr->link = newNode;
				ptr = newNode;
			}
			else{
				head = newNode;
				ptr = head;
			}
		}
	}
	fclose(fp);
}

void rank(){
	// user code
	int x = 1, i, cnt = 0, del_rank = 0;
	int y = rankNum;
	char name_sch[NAMELEN];
	Node *ptr, *del;
	ptr = head;
	del = ptr;

	clear();
	move(0,0);
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	switch(wgetch(stdscr)){
		case '1':
			echo();		
			printw("X: ");
			scanw("%d", &x);
			printw("Y: ");
			scanw("%d", &y);
			noecho();

			printw("       name       |   score   \n");
			printw("------------------------------\n");
			if ( x > y || rankNum == 0 || x > rankNum || y > rankNum || x <= 0 || y <= 0 )
				printw("\nsearch failure: no rank in the list\n");
			else{
				for (i = 1; i < x; i++){
					ptr = ptr->link;
				}

				for (i = x; i <= y; i++){	
					printw(" %-17s| %-10d\n", ptr->name, ptr->score);
					ptr = ptr -> link;
				}
			}
			break;
		case '2':
			printw("Input the name: ");
			echo();
			scanw("%s", name_sch);
			noecho();
			printw("       name       |   score   \n");
			printw("------------------------------\n");

			while (ptr){
				if (strcmp(ptr->name, name_sch) == 0){
					printw(" %-17s| %-10d\n", ptr->name, ptr->score);
					cnt++;
				}
				if (ptr->link == NULL)
					break;
				ptr = ptr->link;
			}

			if (cnt == 0)
				printw("\nsearch failure: no name in the list\n");
			break;
		case '3':
			printw("Input the rank: ");
			echo();
			scanw("%d", &del_rank);
			noecho();

			if (del_rank < 1 || del_rank > rankNum)
				printw("\nsearch failure: the rank not in the list");
			else {
				if (del_rank == 1)
					head = ptr -> link;
				else {
					for ( i = 1; i < del_rank - 1; i++)
						ptr = ptr->link;
					del = ptr->link;
					ptr->link = del->link;
				}
				free(del);
				rankNum--;
				printw("\nresult: the rank deleted\n");
				writeRankFile();
			}
			break;
		default:
			break;
			
	}
	getch();
}

void writeRankFile(){
	// user code
	FILE *fp = fopen("rank.txt", "r");
	int currentNum;
	Node *ptr;
	ptr = head;
	
	fscanf(fp, "%d", &currentNum);
	if (currentNum == rankNum)
		return;
	fclose(fp);

	fp = fopen("rank.txt", "w");
	fprintf(fp, "%d\n", rankNum);
	while (ptr){
		fprintf(fp, "%s %d\n", ptr->name, ptr->score);
		ptr = ptr->link;
	}
	fclose(fp);
}

void newRank(int score){
	// user code
	Node* newNode = (Node*)malloc(sizeof(Node));
	Node* ptr;
	ptr = head;
	char newname[NAMELEN];

	clear();
	echo();
	move(0,0);
	printw("your name: ");
	getstr(newname);
	strcpy(newNode->name, newname);
	noecho();

	rankNum++;
	newNode->score = score;
	newNode->link = NULL;

	if(head == NULL)
		head = newNode;
	else if (ptr->score < score){
		newNode->link = ptr;
		ptr = newNode;
		head = ptr;
	}
	else{
		while(ptr->link){
			if (ptr->link->score > score){
				ptr = ptr->link;
			}
			else
				break;
		}
	
		newNode->link = ptr -> link;
		ptr->link = newNode;
	}

	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID, int blockRotate){
	// user code
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code
	for (int i = 0; i < HEIGHT; i++){
		for (int j = 0; j < WIDTH; j++){
			recRoot->recField[i][j] = field[i][j];
		}
	}

	int lv = root->level+1;
	int curID = nextBlock[lv-1];
	int flag, idx = 0, deleteLine = 0, touched = 0;
	int i, j, k, l, m, n;
	RecNode *ptr;

	if (lv < VISIBLE_BLOCKS + 1)
		root->child = (RecNode**)malloc(36*sizeof(RecNode*));
	else if (lv > VISIBLE_BLOCKS){
		if (mNode == NULL)
			mNode = root;
		else if (root->accumulatedScore > mNode->accumulatedScore)
			mNode = root;
		return 0;
	}

	for (i = -3; i < WIDTH; i++){
		for (j = 0; j< 4; j++){
			if (CheckToMove(root->recField, curID, j, 0, i) == 1){
				ptr = root->child[idx]=(RecNode*)malloc(sizeof(RecNode));
				root->child[idx]->level = lv;
				root->child[idx]->parent = root;

				for (k = 0; k < HEIGHT; k++)
					for (l = 0; l < WIDTH; l++)
						root->child[idx]->recField[k][l] = root->recField[k][l];

				for (m = 0; m < HEIGHT; m++)
					if (CheckToMove(root->child[idx]->recField, curID, j, m + 1, i) ==0) break;
				root->child[idx]->curBlockID = curID;
				root->child[idx]->recBlockX = i;
				root->child[idx]->recBlockRotate = j;
				root->child[idx]->recBlockY = m;
	
				touched = 0;
				for (k = 0; k < BLOCK_HEIGHT; k++){
					for (l = 0; l < BLOCK_WIDTH; l++){
						if (block[curID][j][k][l] == 1){
							ptr->recField[m+k][i+l] = 1;
							if (m+k == HEIGHT - 1) touched++;
							else if (ptr->recField[m+k+1][i+l] == 1) touched++;
						}
					}
				}

				deleteLine = 0;
				for (k = 0; k < HEIGHT; k++){
					flag = 0;
					for (l = 0; l < WIDTH; l++){
						if (ptr->recField[k][l] == 1) flag++;
						else break;
					}
					if (flag == WIDTH) deleteLine++;
				}
				ptr->accumulatedScore = root->accumulatedScore + touched*10 + deleteLine*deleteLine*100;
				recommend(root->child[idx]);
				idx++;
			}
		}
	}

	return max;
}

int modified_recommend(MoNode* root, int lv)
{
	int max = 0;
	int i = 0, j, k, r, x, y;
	int tmp;
	MoNode**c = root->c;

	if (lv > VISIBLE_BLOCKS - 1)
		return 0;

	//for all rotations
	for (r = 0; r< NUM_OF_ROTATE; r++)
	{
		if (nextBlock[lv] == 4 && r >0)
			break;

		else if (r>0 && (nextBlock[lv] == 0 || nextBlock[lv] == 5 || nextBlock[lv] == 6))
			break;

		//for all x
		for (x = -3; x< WIDTH; x++)
		{
			y = blockY;

			if (CheckToMove(root->f, nextBlock[lv], r, y, x) == 1)
			{
				for (j = 0; j<HEIGHT; j++)
					for (k = 0; k<WIDTH; k++)
						c[i]->f[j][k] = root->f[j][k];

				while (CheckToMove(c[i]->f, nextBlock[lv], r, y + 1, x))
					y++;

				c[i]->curBlockID = nextBlock[lv];
				c[i]->recBlockRotate = r;
				c[i]->recBlockX = x;
				c[i]->recBlockY = y;

				c[i]->score = root->score + evalstate(lv, c[i]->f, r, y, x);
				if (c[i]->score - root->score <= 10)
					continue;

				if (lv == VISIBLE_BLOCKS - 1)
					max = c[i]->score;

				else
				{
					if (lv >= 2 && c[i]->score < (double)max*0.2) //eliminating least possible scores
						continue;

					tmp = modified_recommend(c[i], lv + 1);

					if (max<tmp)
					{
						max = tmp;

						if (lv == 0)
						{
							mrRoot->curBlockID = nextBlock[lv];
							mrRoot->recBlockRotate = r;
							mrRoot->recBlockX = x;
							mrRoot->recBlockY = y;

						}
					}

					else if (max == tmp && lv == 0 && mrRoot->recBlockY < y)
					{
						max = tmp;

						if (lv == 0)
						{
							mrRoot->curBlockID = nextBlock[lv];
							mrRoot->recBlockRotate = r;
							mrRoot->recBlockX = x;
							mrRoot->recBlockY = y;

						}

					}
				}

				i++;
			}
		}

	}

	recommendX = mrRoot->recBlockX;
	recommendY = mrRoot->recBlockY;
	recommendR = mrRoot->recBlockRotate;

	return max;
}

void BlockDown_rec(int sig) {
	int i, j;
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==1)
	{
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	}

	else
	{
		if(blockY==-1)
			gameOver=1;
		score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);

		score+=DeleteLine(field);
	
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;
		for(i=0;i<VISIBLE_BLOCKS-1;i++)
			nextBlock[i]=nextBlock[i+1];
		nextBlock[VISIBLE_BLOCKS-1]=rand()%7;
		
		modified_recommend(mrRoot, 0);
		DrawNextBlock(nextBlock);

		DrawField();
		PrintScore(score);
		DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	}

	timed_out=0;
}

void recommendedPlay(){
	int i, j;
	clear();
	act.sa_handler = BlockDown_rec;
	sigaction(SIGALRM,&act,&oact);
	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	for(i=0;i<VISIBLE_BLOCKS;i++)
		nextBlock[i]=rand()%7;

	modified_recommend(mrRoot, 0);
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	

	DrawOutline();
	DrawField();
	blockRotate=mrRoot->recBlockRotate;
	blockX=mrRoot->recBlockX;
	blockY=mrRoot->recBlockY;

	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		
		blockRotate=mrRoot->recBlockRotate;
		blockX=mrRoot->recBlockX;
		blockY=mrRoot->recBlockY;

		for(i=0;i<WIDTH;i++)
		{
			if(field[0][i]==1)
				gameOver=1;
		}

		if(gameOver==1)
			break;

		if(GetCommand()==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
		
	}while(!gameOver);

	alarm(0);
	getch();

	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");

	refresh();
	getch();
}


void constructRecTree(MoNode* root)
{
	int i;
	MoNode **child=root->c;

	for(i=0;i<CHILDREN_MAX;++i)
	{
		child[i]=(MoNode*)malloc(sizeof(MoNode));
		child[i]->lv=root->lv+1;
		child[i]->f=(char(*)[WIDTH])malloc(sizeof(char)*HEIGHT*WIDTH);
		if(child[i]->lv<VISIBLE_BLOCKS-1)
			constructRecTree(child[i]);
	}
}

int evalstate(int lv, char f[HEIGHT][WIDTH], int r, int y, int x)
{
	return AddBlockToField(f,nextBlock[lv],r,y,x)+5*DeleteLine(f)+5*y+3*side_touched-3*downdown;
}

