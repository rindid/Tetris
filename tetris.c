#include "tetris.h"
static struct sigaction act, oact;

int main(){
	int exit=0;
	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	start_color();/////////
	init_pair(1,COLOR_BLACK,RED);
	init_pair(2,COLOR_BLACK,YELLOW);
	init_pair(3,COLOR_BLACK,ORANGE);
	init_pair(4,COLOR_BLACK,GREEN);
	init_pair(5,COLOR_BLACK,BLUE);
	init_pair(6,COLOR_BLACK,VIOLET);
	init_pair(7,COLOR_BLACK,PINK);
	init_pair(8,COLOR_WHITE,GRAY);
	init_pair(9,COLOR_BLACK,GRAY2);

	srand(1);

	while(!exit){
		clear();
		switch(menu()){
			case MENU_PLAY: play(); break;
			case MENU_EXIT: exit=1; break;
			case MENU_RANK: rank(); break;
			case MENU_RECPLAY: recommendedPlay(); break;
		//	case '5': infinite_recommendedPlay(); break;
			default: break;
		}
	}

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

	recRoot=(RecNode*)malloc(sizeof(RecNode));
	recRoot->lv=0;
	recRoot->score=score;
	recRoot->f=(char (*)[WIDTH])malloc(sizeof(char)*WIDTH*HEIGHT);
	memcpy(recRoot->f, field, sizeof(char)*WIDTH*HEIGHT);
	modified_recommend(recRoot);
	free(recRoot->f);
	free(recRoot);

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	fast_down=0;
	PauseFlag=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
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
			command = FALL;
			break;
		case 'q':
		case 'Q':
			command = QUIT;
			break;
		case 'p':
		case 'P':
			command = PAUSE; 
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
		case PAUSE:
			ret = PAUSE;
			break;
		case KEY_UP:
			if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
				blockRotate=(blockRotate+1)%4;
			break;
		case KEY_DOWN:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
				blockY++;
			else if(fast_down==1)
				Stack_Block(field,&blockRotate,&blockY,&blockX);
			else
				fast_down=1;
			break;
		case KEY_RIGHT:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
				blockX++;
			break;
		case KEY_LEFT:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
				blockX--;
			break;
		case FALL:
			if(PauseFlag==1)
				break;
			while(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==1)
				blockY++;//DrawChange(field,command,nextBlock[0],blockRotate,++blockY,blockX);
			Stack_Block(field,&blockRotate,&blockY,&blockX);
			break;
		default:
			break;
	}
	if(drawFlag && PauseFlag==0) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]!=0){
				attron(COLOR_PAIR(field[j][i]));
				printw(" ");
				attroff(COLOR_PAIR(field[j][i]));
			}
			else printw(".");
		}
	}
	move(22,WIDTH+10);
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
	move(22,WIDTH+10);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(COLOR_PAIR(nextBlock[1]+1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[1]+1));
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(COLOR_PAIR(nextBlock[2]+1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[2]+1));
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				if(tile=='/')
				{
					attron(COLOR_PAIR(8));
					printw("%c",tile);
					attroff(COLOR_PAIR(8));
				}
				else if(tile=='R')
				{
					attron(COLOR_PAIR(9));
					printw("%c",tile);
					attroff(COLOR_PAIR(9));
				}
				else
				{
					attron(COLOR_PAIR(blockID+1));
					printw("%c",tile);
					attroff(COLOR_PAIR(blockID+1));
				}
			}
		}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch('+');
	//addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch('-');
	//addch(ACS_HLINE);
	addch('+');
	//addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch('|');
		//addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch('|');
		//addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch('+');
	//addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch('-');
	//addch(ACS_HLINE);
	addch('+');
	//addch(ACS_LRCORNER);
}

void play(){
	int command;
	int command_flag;
	play_flag=0;
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
		command_flag=ProcessCommand(command);
		if(PauseFlag!=1 && command_flag==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
		else if(command_flag==PAUSE)
		{
			if(PauseFlag==1)
			{	
				PauseFlag=0;
				DrawOutline();
				DrawField();
				BlockDown(SIGINT+1);
				//이런식으로 하니 pause가 풀리면 바로 한칸 내려간다. 일종의 패널티로 봐야 하나...
			}
			else
			{
				alarm(0);
				PauseFlag=1;
				DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
				move(HEIGHT/2,WIDTH/2-4);
				printw("   Pause  ");
				refresh();
				move(22,WIDTH+10);
			}
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	//
	clear();
	printw("Put Your Name: ");
	createRankList();
	newRank(score);
	writeRankFile();
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1 && i+blockY>=0){
				//move(23,WIDTH+10);////////////////////
				//printw("%d", blockX);///////////////////////
				if(i+blockY+1<0 || i+blockY+1>HEIGHT)
					return 0;
				if(j+blockX+1<1 || j+blockX+1>WIDTH)
					return 0;
				if(f[i+blockY][j+blockX]!=0)
					return 0;
			}
		}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);
}

void BlockDown(int sig){//0이면 not recommend, 1이면 recommend
	int flag;
	int X=recommendX, Y=recommendY, R=recommendR;
	if(play_flag==0)
	{
		flag=CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX);
		timed_out=0;
		if(flag==1)
			ProcessCommand(KEY_DOWN);
		else{
			Stack_Block(field,&blockRotate,&blockY,&blockX);
		}
	}
	else
	{
	//	timed_out=0;
		Stack_Block(field,&R,&Y,&X);
		 timed_out=0;
	}
	move(22,WIDTH+10);
}
void Stack_Block(char f[HEIGHT][WIDTH],int *blockRotate, int *blockY, int *blockX)
{
	int h;
	int sd;
	if(play_flag == 0) 
		alarm(0);
	if(*blockY==-1)
		gameOver=1;
	score=score+10*AddBlockToField(f, nextBlock[0], *blockRotate,*blockY,*blockX);
	sd=DeleteLine(f);
	score=score+sd*sd*100;
	PrintScore(score);

	//다음 블록으로 바꾸기, x,y좌표 초기화....
	nextBlock[0]=nextBlock[1];
	nextBlock[1]=nextBlock[2];
	nextBlock[2]=rand()%7;
//		nextBlock[2]=4;////////////////////////////////////////////////////////
	recRoot=(RecNode*)malloc(sizeof(RecNode));
	recRoot->lv=0;
	recRoot->score=1000000;
	recRoot->f=(char (*)[WIDTH])malloc(sizeof(char)*WIDTH*HEIGHT);
	memcpy(recRoot->f, f, sizeof(char)*WIDTH*HEIGHT);
	sd=modified_recommend(recRoot);
	////////printw("%d",sd-score);///
	free(recRoot->f);
	free(recRoot);

	//move(21,WIDTH+10);/////////////
	//printw("%d %d", recommendY, recommendX);

	*blockRotate=0;
	*blockY=-1;
	*blockX=WIDTH/2-2;
	fast_down=0;
	DrawNextBlock(nextBlock);
	DrawField();
	DrawBlockWithFeatures(*blockY,*blockX,nextBlock[0],*blockRotate);
	if(play_flag==0)
		alarm(1);
}	
int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j,s=0;
	stackHeight=1;
	for(i=0;i<4;i++){//점수매기기
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1 && i+blockY>=0)
			{
				if(i+blockY+1<HEIGHT)
					s+=(f[i+blockY+1][j+blockX]!=0);
				else
					s++;
			}
		}
	}	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1 && i+blockY>=0)
			{
				f[i+blockY][j+blockX]=currentBlock+1;
				if(i+blockY>stackHeight)
					stackHeight=i+blockY;
			}
		}
	}
	return s;//바닥에 닿은 갯수
	//score=score+s*10;
	//PrintScore(score);
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j, rj, flag=1, flag2=1, flag3=1;
	rj=HEIGHT-1;
	deleteHeight=1;
	lastHeight=1;
	lastlastHeight=1;
	for(j=HEIGHT-1;j>=0;j--){
		flag=1;
		flag2=1;
		flag3=1;
		for(i=0;i<WIDTH;i++){
			if(f[j][i]==0)//flag==0이면 빈칸 존재, flag==1이면 삭제될 줄
				flag=0;	
			 if(f[j][i]==1)//flag2==0이면 블럭 존재, flag2==1이면 빈줄
				flag2=0;
			 if(f[rj][i]==1)
				 flag3=0;
			f[rj][i]=f[j][i];
		}
		if(flag==0)
			rj--;
		else{
			if(deleteHeight<j)
				deleteHeight=j;
		}
		if(flag2==1)
		{
			if(lastHeight<j)//깨지기 전의 블록
				lastHeight=j;
		}
		if(flag3==1)
		{
			if(lastlastHeight<j)//깨지고 난 후의 블록
				lastlastHeight=j;
		}
	
	}
	lastHeight++;//마지막으로 쌓인 줄
	lastlastHeight++;
	return rj-j;//지워진 라인 수
	//score=score+(rj-j)*(rj-j)*100;
	//PrintScore(score);
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	while(CheckToMove(field,blockID,blockRotate,y+1,x)==1)
		y++;
	DrawBlock(y,x,blockID,blockRotate,'/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	DrawRecommend(recommendY,recommendX,nextBlock[0],recommendR);
	DrawShadow( y, x, blockID, blockRotate);
	DrawBlock( y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	int i;
	FILE * fp = fopen("rank.txt","r");
	Ranking *nowRank;
	startRank=NULL;
	Rank_n=0;
	if(fp==NULL || fp==0)
		return;
	fscanf(fp, "%d", &Rank_n);
	nowRank=(Ranking *)malloc(sizeof(Ranking));
	nowRank->frontRank=NULL;
	startRank=nowRank;
	for(i=0;i<Rank_n;i++)
	{
		fscanf(fp,"%s %d", (nowRank->name), &(nowRank->scoring));
		if(i==Rank_n-1)
		{
			nowRank->nextRank=NULL;
			break;
		}
		nowRank->nextRank=(Ranking *)malloc(sizeof(Ranking));
		nowRank->nextRank->frontRank=nowRank;
		nowRank=nowRank->nextRank;
	}
	fclose(fp);
}

void rank(){
	char s;
	char x[100]={0},y[100]={0};
	int ix,iy;
	int i;
	int flag;
	Ranking* nowRank;
	clear();
	createRankList();
	printw("1. list rank form X to Y\n");
	printw("2. list rank by a specific name\n");
	printw("3. delete a specific rank\n");
	///////////////////////////////////////////
	s=wgetch(stdscr);
	switch(s)
	{
		case '1':
			{
				echo();
				printw("X: ");
				wgetstr(stdscr, x);
				printw("Y: ");
				wgetstr(stdscr, y);
				if(x[0]==' ' || x[0]==0)
					x[0]='1';
				if(y[0]==' ' || y[0]==0)
					sprintf(y,"%d",Rank_n);
				nowRank=startRank;
				ix=atoi(x);
				iy=atoi(y);
				if(ix>iy)
				{
					printw("search failure: no rank in the list\n");
					noecho();
					break;
				}
				printw("%12s         | %10s\n----------------------------------------\n","name","score");
				for(i=1;i<=Rank_n;i++)
				{
					if(i>=ix && i<=iy)
						printw(" %-20s| %-10d\n", nowRank->name, nowRank->scoring);
					nowRank=nowRank->nextRank;
				}
				noecho();
			}
			break;
		case '2'://이름에 해당하는 정보찾기
			{
				flag=1;
				printw("input the name which you want to search : ");
				echo();
				wgetstr(stdscr,x);
				printw("%12s         | %10s\n----------------------------------------\n","name","score");
				for(nowRank=startRank; nowRank!=NULL; nowRank=nowRank->nextRank)
				{
					if(strcmp(nowRank->name, x)==0)
					{
						printw(" %-20s| %-10d\n", nowRank->name, nowRank->scoring);
						flag=0;
					}
				}
				if(flag!=0)
					printw("search failure: no name in the list\n");
				noecho();
			}
			break;
		case '3'://랭킹정보삭제
			{
				i=1;
				printw("input the rank which you want to delete : ");
				echo();
				wgetstr(stdscr,x);
				ix=atoi(x);
				flag=1;
				for(nowRank=startRank; nowRank!=NULL; nowRank=nowRank->nextRank)
				{
					if(i==ix)
					{
						//삭제
						if(nowRank==startRank)
							startRank=nowRank->nextRank;
						if(nowRank->frontRank!=NULL)
							nowRank->frontRank->nextRank=nowRank->nextRank;
						if(nowRank->nextRank!=NULL)
							nowRank->nextRank->frontRank=nowRank->frontRank;
						free(nowRank);
						Rank_n--;
						flag=0;
						printw("result : the rank deleted\n");
						break;
					}
					i++;
				}
				if(flag==1)
					printw("search failure: no rank in the list\n");
				noecho();
			}
			break;

		default : break;	

	}
	writeRankFile();
	getch();
}

void writeRankFile(){
	int i;
	FILE * fp = fopen("rank.txt","w");
	Ranking *tmpRank;
	fprintf(fp, "%d\n", Rank_n);
	for(i=0; i<Rank_n; i++)
	{
		fprintf(fp, "%s %d", startRank->name, startRank->scoring);
		if(i!=Rank_n-1)
			fprintf(fp, "\n");
		tmpRank=startRank;
		startRank=startRank->nextRank;
		free(tmpRank);
	}
	fclose(fp);
}

void newRank(int score){
	Ranking *nowRank;
	Ranking *myRank;
	char tmp[100];//
	myRank=(Ranking *)malloc(sizeof(Ranking));
	//이름 적는 거 하기 
	echo();
	while(1)
	{
		wgetstr(stdscr,tmp);
		if(strlen(tmp)<16)
			break;
		printw("Please Put less than 15 chars\nPut Your Name: ");
	}
	noecho();
	strcpy(myRank->name, tmp);///
	myRank->scoring=score;
	for(nowRank=startRank; nowRank!=NULL; nowRank=nowRank->nextRank)
	{
		if(nowRank->scoring < score)
		{
			myRank->frontRank=nowRank->frontRank;//<-my
			break;
		}
		myRank->frontRank=nowRank;//<-my
	}
	if(Rank_n==0)
	{
		myRank->frontRank=NULL;
		startRank=myRank;
	}
	myRank->nextRank=nowRank;//my->
	if(myRank->frontRank!=NULL)
		myRank->frontRank->nextRank=myRank;//front->
	if(myRank->nextRank!=NULL)
		myRank->nextRank->frontRank=myRank;//<-next
	if(startRank==nowRank)
		startRank=myRank;
	Rank_n++;
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	DrawBlock(y,x,blockID,blockRotate,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int nowB=nextBlock[root->lv];
	int i, j, tree_n=0;
	int d;
	int blockY, blockX;
	RecNode* current;

	for(i=0; i<blockRN[nowB]; i++)//돌리기, i=Rotate
	{
		blockX=0;
		blockY=-1;
		while(CheckToMove(root->f,nowB,i,blockY,blockX-1)==1)
			blockX--;
		for(j=0; j< WIDTH+1-blockRec[nowB][i]; j++)//각 칸마다 
		{
			//move(22,WIDTH+10);/////////////
			//printw("%d", tree_n);////////////////////
			
			root->c[tree_n]=(RecNode*)malloc(sizeof(RecNode));
			current=root->c[tree_n];
			current->f=(char (*)[WIDTH])malloc(sizeof(char)*WIDTH*HEIGHT);//그대로 f복사
			current->lv=(root->lv)+1;
			blockY=-1;
			memcpy(current->f, root->f,sizeof(char)*WIDTH*HEIGHT);
			while(CheckToMove(current->f,nowB,i,blockY+1,blockX+j)==1)
				blockY++;
			current->score=(root->score)+10*AddBlockToField(current->f,nowB, i, blockY, blockX+j);
			d=DeleteLine(current->f);
			current->score=(current->score)+(d*d)*100*(4-(current->lv));//100;
			if(blockY<1)
				current->score=0;
			else if(blockY<HEIGHT)
			{	
				current->score/=(HEIGHT-blockY);
				if(current->lv!=3)//current, 보여주는 다음 블록까지 본다
					current->score=recommend(current);
			}
		//	if(current->lv==1){
		//	move(22,WIDTH+10);////
		//	printw("j=%d c=%d m=%d d=%d", j, current->score-root->score, max-root->score, d);////////////
		//	getch();}
			
			if(max < (current->score) || max==0)
			{
				max=(current->score);
				if(current->lv==1)
				{
					recommendR=i;
					recommendY=blockY;
					recommendX=blockX+j;
				}
			}
			free(current->f);
			free(current);
	//		tree_n++;
		}
	}
	return max;
}

int modified_recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int nowB=nextBlock[root->lv];
	int i, j, tree_n=0;
	int a,d;
	int blockY, blockX;
	RecNode* current;

	for(i=0; i<blockRN[nowB]; i++)//돌리기, i=Rotate
	{
		blockX=0;
		blockY=-1;
		while(CheckToMove(root->f,nowB,i,blockY,blockX-1)==1)
			blockX--;
		for(j=0; j< WIDTH+1-blockRec[nowB][i]; j++)//각 칸마다 
		{
			root->c[tree_n]=(RecNode*)malloc(sizeof(RecNode));
			current=root->c[tree_n];
			current->f=(char (*)[WIDTH])malloc(sizeof(char)*WIDTH*HEIGHT);//그대로 f복사
			current->lv=(root->lv)+1;
			blockY=-1;
			memcpy(current->f, root->f,sizeof(char)*WIDTH*HEIGHT);
			while(CheckToMove(current->f,nowB,i,blockY+1,blockX+j)==1)
				blockY++;
			a=AddBlockToField(current->f,nowB, i, blockY, blockX+j);
			d=DeleteLine(current->f);
			current->score=(root->score)+10*a;
			current->score*=stackHeight;
			current->score+=d*250*deleteHeight;//*(deleteHeight-lastHeight+1);//*(4-(current->lv));
			//current->score*=stackHeight;
			current->score*=(4-(current->lv));//one
			//current->score/=(deleteHeight-lastHeight+1);
			if(blockY<1)
				current->score=1;
			else if(blockY<HEIGHT)
			{
				current->score/=(HEIGHT-blockY);
				//current->score/=(HEIGHT-lastHeight+1);
				if(current->lv!=3)//current, 보여주는 다음 블록까지 본다
					current->score=modified_recommend(current);
			}

			if(max < (current->score) || max==0)
			{
				max=(current->score);
				if(current->lv==1)
				{
					recommendR=i;
					recommendY=blockY;
					recommendX=blockX+j;
				}
			}
			free(current->f);
			free(current);
		}
	}
	return max;
}

void recommendedPlay(){
	int command;
	int command_flag=0;
	play_flag=1;
	clear();
	act.sa_handler = BlockDown;//1=recommend
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command = GetCommand();
		if(command == QUIT)
			command_flag=QUIT;
		/*else if(command == PAUSE)
			command_flag=PAUSE;*/
		else
			command_flag=NOTHING;
		if(PauseFlag!=1 && command_flag==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}
		else if(command_flag==PAUSE)
		{
			if(PauseFlag==1)
			{
				PauseFlag=0;
				DrawOutline();
				DrawField();
				BlockDown(SIGINT+1);
			}
			else
			{
				alarm(0);
				PauseFlag=1;
				DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
				move(HEIGHT/2,WIDTH/2-4);
				printw("   Pause  ");
				refresh();
				move(22,WIDTH+10);
			}
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

void infinite_recommendedPlay(){
	int command=0;
	int command_flag=0;
	play_flag=1;
	clear();
	InitTetris();
	do{//끌 수 없던가, settilmelrr뭐 이딴거였나 이걸쓰던가...
		//usleep(1000*100);
		BlockDown(SIGINT);
		refresh();
	}while(!gameOver);

	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();		
}
