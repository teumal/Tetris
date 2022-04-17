// 테트리스에 사용할 함수들의 모음입니다.

# define _CRT_SECURE_NO_WARNINGS

// 색깔 모음.
# define red 68  
# define pink 204
# define yellow 238
# define blue 17
# define cyan 51
# define green 34
# define gray 119
# define magenta 85
# define DEFAULT 15
# define white 255
# define mutant 240

// 기본 블록의 수
# define BASE 7


// 블럭 1칸의 색깔(color)과 값(value)
typedef struct BLOCK
{
	int color;  // (color: 블록의 색깔)--(red: 68)--(yellow: 238)--(blue: 17)--(green: 34)--(gray: 119)--(cyan: 204)--(magenta: 85)
	int value; // (value: 블록의 정보)--(0: 비어있음)--(1: 쌓여있는 블록)--(2: 떨어지는 블록)--(3: 고스트 블록)

}BLOCK;

// 새로 생성한 블록의 빈줄 수(blankLine, blankLine_t)와 점수(score)그리고 위치(y, x)
typedef struct INFO
{
	int blankLine; // (blankLine: 우측 빈줄 수)    ( 회전하기 전 블록의, 윗쪽 빈줄 수)
	int blankLine_t; // (bllankLine_t: 위쪽 빈줄 수)  ( 회전하기 전 블록의, 우측 빈줄 수)
	int score;  // ( score: 블록의 유형에 따른 점수)

	
	int y; // (y: 움직이는 블록의 실제 y 좌표)   (default: 0)
	int x; // (x: 움직이는 블록의 실제 x 좌표)   (default: 3)

}INFO;

void tetris_board(BLOCK(*)[10],BLOCK(*)[4], int);  
INFO create_block(BLOCK(*)[4],int);  
int vrtl_Mov(BLOCK(*)[10], BLOCK(*)[4], int, int);  
int srch(BLOCK(*)[10], BLOCK(*)[4],int); 
int rotate(BLOCK(*)[10], BLOCK(*)[4]);  
void load_block(BLOCK(*)[10], BLOCK(*)[4], int);
void load_ghost(BLOCK(*)[10], BLOCK(*)[4], int);
int line_delete(BLOCK(*board)[10]); 
int stageUp(int, int); 
void status(int);
void guide(int);
void How_to(); 

# include"letter.h"
# include<stdlib.h>
# include<time.h>
# include<conio.h>
# include<string.h>
# include<limits.h>
# include<windows.h>


INFO info = { 0 }; // (info: 현재 움직이고 있는 블록의 정보)  ( 구조체 맴버: blankLine, blankLine_t, score, y, x )
BLOCK Next[4][4] = { 0 }; // (Next: 다음에 사용할 블럭의 표본)
int randomNext = -1; // (randomNext: 다음에 사용할 블럭의 번호)   (default: -1)
int nature = 0; // (nature: 블록이 회전했을때, 자연스럽게 해주는 용도)

int CUR_SCORE = 0; // (CUR_SCORE: 현재 점수)  (default: 0)
int stage = 1; // (stage: 현재 레벨)--(1: 기본 블록 7종)--(2~7: 신규블록 2종)--(8:블록 최종 고정 시간 감소)--(9:블록 최종 고정 시간 감소)
int combo = 0; // (combo: 연속으로 줄을 없앤 횟수)  (default: 0)
int score_on = 0; // (score_on: 현재 얻은 점수를 보여줄지 여부)--(0: 안 보여줌)--(1: 보여줌)
int nG = 0; // (nG: 블록이 받는 n중력)  (default: 0) (최대 10)


// 테트리스 판을 화면에 그려줍니다.
// 움직이는 블록과 고스트 블록을 테트리스 판에 넣습니다.
// 그릴때, 블록에 색깔이 있다면, 색깔도 넣어줍니다.
void tetris_board( BLOCK(*board)[10], BLOCK(*block)[4], int block_on )
{
	int color = 15; // (color: 해당 블록의 색깔 값을 보관합니다.)    (default: 15)

	// 움직이는 블록과 고스트 블록을 테트리스 판에 적재.
	if (block_on)
	{
		load_ghost(board, block, 1);
		load_block(board, block, 1);
	}


	printf("\n\n\n");

	// 20 x 10 테트리스 판을 출력.
	for (int i = 0; i < 20; ++i)
	{
		// 테트리스 판 왼쪽에 기타정보들을 나타냅니다.
		status(i);

		printf("<!");

		for (int j = 0; j < 10; ++j)
		{
			// 블록의 색깔 정하기
			// 블록 칸이 비어있다면, 기본 색으로 바꾸어줍니다.
			color = (board[i][j].value != 0) ? (board[i][j].color) : DEFAULT;

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color );

			// 해당 칸에 블록이 있다면 ■을, 비어있다면 . 을 출력.
			printf("%s ", board[i][j].value ? "■" : "  ");
		}
		
		// 블록을 출력하는 것이 아니면, 글자 색은 기본 값으로 바꿉니다.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT );
		printf("!>");

		// 테트리스 판 오른쪽에 간단한 조작정보들을 나타냅니다.
		guide(i);

		printf("\n");
	}

	// 테트리스 판의 아래를 꾸며줍니다.
	printf("\t\t\t\t\t<!== == == == == == == == == == !>\n");
	printf("\t\t\t\t\t  UU UU UU UU UU UU UU UU UU UU \n");


	// 움직이는 블록과 고스트 블록을 테트리스 판에 삭제.
	if (block_on)
	{
		load_ghost(board, block, 2);
		load_block(board, block, 3);
	}
	
}



// 현재 조작할 블록을 제작합니다.
// 다음에 사용할 블록도 미리 만들어 둡니다.
// 새로 만든 블록의 정보를 반환합니다.
INFO create_block( BLOCK(*block)[4], int stage )
{
	srand((unsigned)time(NULL));

	int blankLine = 0; // (blankLine: 우측 빈줄 수)
	int blankLine_t = 0; // (blankLine_t: 위쪽 빈줄 수)
	int score = 0; // ( score: 블록의 유형에 따른 점수)

	int random = randomNext; // (random: 랜덤으로 정한 블록의 번호)--(0~6: 기본블록)--(7~18: 기타 변종 블록)
	
	// 신규 블록 추가 제한.
	if (stage > 6)
		stage = 6;

	// 이 함수를 처음 실행하는 것이라면, random에 직접
	// 블록의 번호를 랜덤으로 넣어줍니다.
	if (random == -1)
		random = rand() % (BASE + 2*stage);


	// 블록의 번호에 따라, block에 값을 넣어줍니다.
	// 블록의 번호는 0~18 까지 존재.
	switch (random)
	{
	// I 미노
	case 0:

		for (int i = 0; i < 4; ++i)
		{
			block[i][0].value = 2;
			block[i][0].color = cyan;
		}
		blankLine = 3;
		blankLine_t = 0;
		score = 300;
		break;


	// O미노
	case 1:

		for (int i = 0; i < 2; ++i)
		{
			block[2][i].value = 2; block[2][i].color = yellow;
			block[3][i].value = 2; block[3][i].color = yellow;
		}
		blankLine = 2;
		blankLine_t = 2;
		score = 200;
		break;

	// Z 미노
	case 2:

		for (int i = 0; i < 2; ++i)
		{
			block[2 - i][i].value = 2; block[2 - i][i].color = red;
			block[3 - i][i].value = 2; block[3 - i][i].color = red;
		}
		blankLine = 2;
		blankLine_t = 1;
		score = 500;
		break;

	// S 미노
	case 3:

		for (int i = 0; i < 2; ++i)
		{
			block[2 - i][1 - i].value = 2; block[2 - i][1 - i].color = green;
			block[3 - i][1 - i].value = 2; block[3 - i][1 - i].color = green;
		}
		blankLine = 2;
		blankLine_t = 1;
		score = 500;
		break;


	// J 미노
	case 4:

		block[2][0].value = 2; block[2][0].color = blue;

		for (int i = 0; i < 3; ++i)
		{
			block[3][i].value = 2; block[3][i].color = blue;
		}
		blankLine = 1;
		blankLine_t = 2;
		score = 700;
		break;


	// L 미노
	case 5:

		block[2][2].value = 2; block[2][2].color = magenta;

		for (int i = 0; i < 3; ++i)
		{
			block[3][i].value = 2; block[3][i].color = magenta;
		}
		blankLine = 1;
		blankLine_t = 2;
		score = 700;
		break;


	// T 미노
	case 6:

		block[3][1].value = 2; block[3][1].color = pink;

		for (int i = 0; i < 3; ++i)
		{
			block[2][i].value = 2; block[2][i].color = pink;
		}
		blankLine = 1;
		blankLine_t = 2;
		score = 1200;
		break;

	// v 미노
	case 7:

		block[2][0].value = 2; block[2][0].color = blue;
		block[2][1].value = 2; block[2][1].color = blue;
		block[3][1].value = 2; block[3][1].color = blue;

		blankLine = 2;
		blankLine_t = 2;
		score = 700;
		break;


	// v 미노2
	case 8:

		block[2][0].value = 2; block[2][0].color = red;
		block[2][1].value = 2; block[2][1].color = red;
		block[3][0].value = 2; block[3][0].color = red;

		blankLine = 2;
		blankLine_t = 2;
		score = 700;
		break;

	// 변칙 T미노
	case 9:

		for(int i=0; i<3; ++i)
		{
			block[1][i].value = 2;
			block[1][i].color = mutant;
	    }

		block[2][1].value = 2; block[2][1].color = mutant;
		block[3][1].value = 2; block[3][1].color = mutant;

		blankLine = 1;
		blankLine_t = 1;
		score = 1500;
		break;
		
	// Q미노
	case 10:

		for (int i = 0; i < 3; ++i)
		{
			block[1][i].value = 2; block[1][i].color = green;
			block[2][i].value = 2; block[2][i].color = green;
		}

		block[3][1].value = 2; block[3][1].color = green;

		blankLine = 1;
		blankLine_t = 1;
		score = 1700;
		break;

	// B미노
	case 11:

		for (int i = 0; i < 2; ++i)
		{
			block[1][i].value = 2; block[1][i].color = magenta;
			block[2][i].value = 2; block[2][i].color = magenta;
		}

		block[3][0].value = 2; block[3][0].color = magenta;

		blankLine = 2;
		blankLine_t = 1;
		score = 1800;
		break;

	// D미노
	case 12:

		for (int i = 0; i < 2; ++i)
		{
			block[1][i].value = 2; block[1][i].color = pink;
			block[2][i].value = 2; block[2][i].color = pink;
		}

		block[3][1].value = 2; block[3][1].color = pink;

		blankLine = 2;
		blankLine_t = 1;
		score = 1800;
		break;

	// 변칙 Z미노
	case 13:

		for (int i = 0; i < 3; ++i)
		{
			block[2][i].value = 2;
			block[2][i].color = cyan;
		}

		block[1][0].value = 2; block[1][0].color = cyan;
		block[3][2].value = 2; block[3][2].color = cyan;

		blankLine = 1;
		blankLine_t = 1;
		score = 2000;
		break;

	// 변칙 S미노
	case 14:

		for (int i = 0; i < 3; ++i)
		{
			block[2][i].value = 2;
			block[2][i].color = yellow;
		}

		block[1][2].value = 2; block[1][2].color = yellow;
		block[3][0].value = 2; block[3][0].color = yellow;

		blankLine = 1;
		blankLine_t = 1;
		score = 2000;
		break;

	// + 미노
	case 15:

		for (int i = 0; i < 3; ++i)
		{
			block[2][i].value = 2;
			block[2][i].color = mutant;
		}

		block[1][1].value = 2; block[1][1].color = mutant;
		block[3][1].value = 2; block[3][1].color = mutant;

		blankLine = 1;
		blankLine_t = 1;
		score = 3000;
		break;

	// 항구 미노
	case 16:

		for (int i = 0; i < 3; ++i)
		{
			block[1][i].value = 2;
			block[1][i].color = mutant;
		}

		block[2][0].value = 2; block[2][0].color = mutant;
		block[2][2].value = 2; block[2][2].color = mutant;
		block[3][0].value = 2; block[3][0].color = mutant;
		block[3][1].value = 2; block[3][1].color = mutant;

		blankLine = 1;
		blankLine_t = 1;
		score = 3000;
		break;

	// H 미노
	case 17:

		for (int i = 0; i < 3; ++i)
		{
			block[i + 1][0].value = 2; block[i + 1][0].color = mutant;
			block[i + 1][2].value = 2; block[i + 1][2].color = mutant;
		}

		block[2][1].value = 2; block[2][1].color = mutant;

		blankLine = 1;
		blankLine_t = 1;
		score = 5000;
		break;

	// U 미노
	case 18:

		for (int i = 0; i < 3; ++i)
		{
			block[i + 1][0].value = 2; block[i + 1][0].color = mutant;
			block[i + 1][2].value = 2; block[i + 1][2].color = mutant;
		}

		block[3][1].value = 2; block[3][1].color = mutant;

		blankLine = 1;
		blankLine_t = 1;
		score = 5000;
		break;

	// 디폴트( . )  미노
	default:

		block[3][0].value = 2; block[3][0].color = DEFAULT;

		blankLine = 3;
		blankLine_t = 3;
		score = 99999;
		break;

	}


	// 다음에 사용할 블록의 번호를 랜덤으로 정합니다.
	if(block != Next)
	   randomNext = rand() % (BASE + 2 * stage);


	// 현재 만들고 있는 블럭이 Next라면, 탈출합니다.
	if (block == Next)
		return (INFO) { 0, 0 };

	// 다음에 사용할 블록을 미리 제작합니다.
	create_block( Next, stage );



	return (INFO) {blankLine, blankLine_t, score, 0, 3};
}



// 현재 조작하는 블록을 원하는 방향대로 (y,x)칸 움직였을때,
// 유효한지 시뮬레이션을 해봅니다.  
// ←: (0,-1), →: (0, 1), ↓: (1, 0), 제자리: (0,0)
// 움직였을때 오류가 있으면 (-1 or -2), 없으면 (0)을 반환합니다.
int vrtl_Mov(BLOCK(*board)[10], BLOCK(*block)[4], int ymov, int xmov)
{
	int show = (info.y+ymov > 3) ? (4) : (info.y+1); // (show: 현재 블록이 테트리스판에서 모습을 드러낸 정도)--(1~4: 유효 범위)
	int yto = (show >= 4-info.blankLine_t) ? (4-info.blankLine_t) : (show); // (to: 3행부터 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)
	int xto = 4-info.blankLine; // (xto: 0열 부터, 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)

	int vy = info.y + ymov; // (vy: block의 가상 y 좌표)
	int vx = info.x + xmov; // (vx: block의 가상 x 좌표)




	// (ymov, xmov)만큼 움직였을때, 오류가 있는지 검사합니다.
	for (int i = 0; i < yto; ++i)
	{
		for (int j = 0; j < xto; ++j)
		{
			// 블록이 유효범위(x: 0~9)를 벗어났을 경우, 오류(-1)를 반환.
			if (0 > vx+j || vx+j > 9)
				return -1;

			// 블록이 유효범위(y: 0~19)를 벗어났을 경우, 오류(-1)를 반환.
			else if (0 > vy-i || vy-i > 19)
				return -1;

			// 블록이 움직인 자리에, (1) 값이 있을 경우, 오류(-2)를 반환.
			else
			  if (board[vy - i][vx + j].value == 1 && block[3 - i][j].value == 2)
					return -2;

		}
	}

	// 이동해도 문제 없음.
	return 0;
}



// 모드 변수의 값에 따라, 블록에서 특정 정보를 얻어냅니다.
// mode 1: 회전 가능 여부. (0: 회전 가능)--(1:회전 불가)
// mode 2: 처음으로 겹치는 열 검색 (열의 값 반환)--(-1: 겹치는 열 없음)
int srch(BLOCK(*board)[10], BLOCK(*block)[4], int mode)
{
	int show = (info.y > 3) ? (4) : (info.y + 1); // (show: 현재 블록이 테트리스판에서 모습을 드러낸 정도)--(1~4: 유효 범위)
	int yto = (show >= 4 - info.blankLine_t) ? (4 - info.blankLine_t) : (show); // (to: 3행부터 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)
	int xto = 4 - info.blankLine; // (xto: 0열 부터, 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)

	int y = info.y; // ( y: 실제 블록의 y 좌표의 줄임말 )
	int x = info.x; // ( x: 실제 블록의 x 좌표의 줄임말 )


	// (mode 1) 회전 가능 여부
	if (mode == 1)
	{

		// 블록의 범위에 1데이터가 있다면, 회전이 불가능하다고 간주합니다.
		for (int i = 0; i < yto; ++i)
		{
			for (int j = 0; j < xto; ++j)
			{
				// 유효 범위(x: 0~9)를 벗어났을 경우, 무시하고 다음 열로 감.
				if (0 > x + j && x + j > 9)
					continue;

				// 1데이터
				if (board[y - i][x + j].value == 1)
					return 1;
			}

		}

		// 회전을 할 수 있다면, (0)를 반환.
		return 0;
	}


   // (mode 2) 처음으로 겹치는 열 검색
	else
	{
		// 0열부터 시작해서, 회전한 블록이 처음으로
		// 1과 겹치게 되는 열의 값을 반환합니다.
		for (int i = 0; i < xto; ++i)
		{
			for (int j = 0; j < yto; ++j)
			{
				// 유효 범위(x: 0~9)를 벗어났을 경우, 무시하고 다음 열로 감.
				if (0 > x + i && x + i > 9)
					continue;

				// 겹치는 부분 발생 검사. 현재 열의 값을 반환.
				if (board[y-j][x+i].value == 1 && block[3 - j][i].value == 2)
					return i;
			}
		}

		// 만약, 겹치는 열이 없다면, -1을 반환합니다.
		return -1;
	}

	
	
}



// 현재 조작하고 있는 블록을 우측으로 90도 회전시킵니다.
// 회전한 블록에 오류가 있으면 (-1)을, 없으면 (0)을 반환합니다.
int rotate(BLOCK(*board)[10], BLOCK(*src)[4])
{
	BLOCK dest[4][4] = { 0 }; // (dest: 회전한 뒤의 블록)

	int blankLine_tmp = info.blankLine_t; // (blankLine_tmp: 회전한 블록의 우측 빈줄 수)
	int rotateError = 0; // (rotateError: 오류 여부)--(0: 오류 없음)--(-1, -2, -3: 오류 있음)
	int rotate_flag = 0; // (rotate_flag: 회전 가능 여부)--(0: 회전 가능)--(1:회전 불가)

	int mx = 0; // ( mx: 가상적/최종적으로 움직여야할 x좌표 )   (default: 0)
	int col; // (col: 겹치는 열의 번호)--(-1: 겹치는 열 없음.)

	
	// 회전하기 전에, 블록이 회전할 수 있는 상황에 있는지 살펴봅니다.
	// 회전할 수 없는 상황이면, rotate_flag에 1을 넣어줍니다.
	if ( srch(board, src, 1) )
		rotate_flag = 1;
		

	// 회전 시작.
	for (int x = (3-info.blankLine); x > -1;  --x   ) // (x: 행 또는 열로 취급됩니다.)--(src: 열)--(dest: 행)
	{
		for (int y = 3; y > -1; --y) // (y: 행 또는 열로 취급됩니다.)--(src: 행)--(dest: 열)
		{
			// 칸의 데이터(value)와 색깔(color)을 대입. src → dest 방향.
			dest[ x+info.blankLine ][3 - y] = src[y][x];
		}
	}

	// 블록이 회전했으므로, (우측, 윗쪽)빈줄 수를 교환합니다.
	info.blankLine_t = info.blankLine;
	info.blankLine = blankLine_tmp;


	// 회전시킨 블록이 유효한지 확인합니다.
	rotateError = vrtl_Mov(board, dest, 0, 0);
	
	

	// (회전 불가능 판정)과 (회전시킨 블록에 오류)가 있을 경우,
	// 회전을 강제 중지시킵니다.
	if (rotateError && rotate_flag)
	{
		// 회전에 실패했으므로, (우측, 윗쪽)빈줄 수를 교환합니다.
		blankLine_tmp = info.blankLine_t;
		info.blankLine_t = info.blankLine;
		info.blankLine = blankLine_tmp;

		return -3;
	}
		

	// 오류가 있지만, 예외일 경우를 고려해봅니다.
	if (rotateError)
	{
		// 첫번째 예외를 위한 장치.
		col = srch(board, dest, 2);


		// 1) 회전시킨 블록이 우측에 있던 블록을 뚫고 지나간 경우.
		if ( rotateError && (col != -1 ) )
		{

			mx = ( info.blankLine_t - col + 1 );


			rotateError = vrtl_Mov(board, dest, 0, -mx);

		}

		// 2) 회전시킨 블록이 우측 벽을 뚫고 지나간 경우.
	    if ( rotateError && info.x > (6 + info.blankLine)  )
		{

			mx = ( info.x - (6 + info.blankLine) );


			rotateError = vrtl_Mov(board, dest, 0, -mx);

		}

		// 위의 과정을 통해서, 오류를 해결했을 경우.
		// 블록의 실제 x좌표에 mx를 더해줍니다.
		if (!rotateError)
			info.x -= mx;

		// 오류를 해결하지 못했다면, 회전을 중지합니다.
		else
		{
			// 회전에 실패했으므로, (우측, 윗쪽)빈줄 수를 교환합니다.
			blankLine_tmp = info.blankLine_t;
			info.blankLine_t = info.blankLine;
			info.blankLine = blankLine_tmp;

			return -1;
		}


	}



	// 블록을 회전한 모양으로 바꾸어줍니다.
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
			src[i][j] = dest[i][j];
	}

	

	// 회전을 했을때, 예외여서 움직였었다면,
	// 다시 전의 위치로 복구합니다.
	if (nature)
	{
		info.x += nature;
		nature = 0;

	}

	// 회전을 했을 경우, 다음 회전의 어색함을 없애주기
	// 위해서, nature에 움직인 정도를 보관합니다.
	nature = mx;


	return rotateError;
}



// 모드 변수에 따른, 블록의 데이터를 테트리스 판에 적재합니다.
// mode 1: 움직이는 블록을 반영구적으로 적재.
// mode 2: 움직이는 블록을 영구적으로 적재.
// mode 3: 움직이는 블록 삭제.
void load_block(BLOCK(*board)[10], BLOCK(*block)[4], int mode)
{
	int show = (info.y > 3) ? (4) : (info.y + 1); // (show: 현재 블록이 테트리스판에서 모습을 드러낸 정도)--(1~4: 유효 범위)
	int yto = (show >= 4 - info.blankLine_t) ? (4 - info.blankLine_t) : (show); // (yto: 3행부터 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)
	int xto = 4 - info.blankLine; // (xto: 0열 부터, 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)

	int y = info.y; // ( y: 실제 블록의 y 좌표의 줄임말 )
	int x = info.x; // ( x: 실제 블록의 x 좌표의 줄임말 )



	// 적재 시작.
	for (int i = 0; i < yto; ++i)
	{
		for (int j = 0; j < xto; ++j)
		{
			// (mode 1) 움직이는 블록을 반영구적으로 적재.
			if (mode == 1)
			{
				// 블록의 칸이 비어있을 경우, 무시합니다.
				if (block[3 - i][j].value == 0)
					continue;

				// 블록의 데이터(색깔과 값)를 테트리스 판에 넣어줍니다.
				board[y - i][x + j] = block[3 - i][j];
		    }

			// (mode 2) 움직이는 블록을 영구적으로 적재.
			else if (mode == 2)
			{
				// 블록의 칸은 비어있을 경우, 무시합니다.
				if ( block[3 - i][j].value == 0 )
					continue;

				// 블록의 데이터(색깔과 값)를 테트리스 판에 넣어줍니다.
				board[y - i][x + j].value = 1;
				board[y - i][x + j].color = block[3 - i][j].color;

			}

			// (mode 3) 움직이는 블록 삭제.
			else
			{
				// 블록의 데이터가 2일 경우, 삭제.
				if (board[y - i][x + j].value == 2)
				{
					board[y - i][x + j].value = 0;
					board[y - i][x + j].color = 0;
				}
			}
		}
	}
}



// 고스트 블록을 테트리스 판에 적재/삭제합니다.
// mode 1: 고스트 블록을 적재.
// mode 2: 고스트 블록을 삭제.
void load_ghost(BLOCK(*board)[10], BLOCK(*ghost)[4], int mode)
{
	int yto = 4; // (yto: 3행부터 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)  ( 이 변수는 아래의 문장을 거쳐야 완전히 완성됩니다.)
	int xto = 4 - info.blankLine; // (xto: 0열 부터, 몇 행까지 할지 여부)  (반복을 최소로 하도록 도와줍니다.)

	int y = info.y; // ( y: 실제 블록의 y 좌표의 줄임말 )
	int x = info.x; // ( x: 실제 블록의 x 좌표의 줄임말 )
	int ghost_y = 0; // ( ghost_y: 고스트 블록의 y좌표)  (default: 1)

	// 고스트 블록의 최대 y좌표를 구합니다.
	while ( !vrtl_Mov(board, ghost, ghost_y+1, 0) )
		++ghost_y;

	ghost_y += y;

	// yto 변수를 완성시킵니다.
	if ( ghost_y < 3 )
		yto = y;



	// 고스트 블록 적재 시작.
	for (int i = 0; i < yto; ++i)
	{
		for (int j = 0; j < xto; ++j)
		{
			// (mode 1)  고스트 블록을 적재.
			if (mode == 1)
			{
				// 블록의 칸이 비어있거나, 테트리스 판의 칸이 차 있을 경우, 무시합니다.
				if (board[ghost_y - i][x + j].value == 1 || ghost[3 - i][j].value == 0)
					continue;

				// 블록의 데이터(색깔과 값)를 테트리스 판에 넣어줍니다.
				// 고스트 블록의 색깔은 gray입니다.
				board[ghost_y - i][x + j].value = 3;
				board[ghost_y - i][x + j].color = gray;
			}

			// (mode 2)  고스트 블록을 삭제.
			else
			{
				// 블록의 데이터가 3일 경우, 삭제.
				if (board[ghost_y-i][x + j].value == 3)
				{
					board[ghost_y-i][x + j].value = 0;
					board[ghost_y-i][x + j].color = 0;
				}
			}
		
		}
	}
	
}



// 각 줄을 검사하여, 꽉 채워진 줄이 있다면 지우고 점수를 얻습니다.
// 얻은 점수의 총 합계를 반환합니다.
int line_delete( BLOCK(*board)[10] )
{
	int total = 0; // (total: 얻은 점수들의 합계)  (default: 0)
	int filled = 0; // (filled: 1데이터를 발견한 횟수)   (10이 되면, 1줄이 가득차 있다는 의미)  (default:0)
	int unfilled = 0; // (unfilled: 0데이터를 발견한 횟수)   (10이 되면, 1줄이 다 비어있다는 의미)  (default:0)

re:
	// 현재 상태를 출력.
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { 0, 0 });

	tetris_board(board, NULL, 0);

	Sleep(300);

	// 각 줄을 검사.
	for (int y = 19; y > -1; --y)
	{
		for (int x = 0; x < 10; ++x)
		{
			// 현재 줄이 꽉차 있지 않을 경우, 다음 행으로 넘어감.
			if (board[y][x].value == 0)
				break;

			// 1데이터를 발견했으므로, filled의 값을 1증가.
			++filled;


			// 1줄이 가득참을 발견했다면, 그 줄을 제거함.
			// 그리고, 위에 있던 블록들을 한칸씩 앞당깁니다.
			if (filled == 10)
			{
				// 1줄을 없앴으니, 콤보 수를 증가시킴.
				combo += 1;

				// 얻은 점수를 보여주기 위해, 점수 보여주기 플래그를 ON합니다.
				score_on = 1;

				// 없어질 줄을 하얗게 만들어서 보여줍니다.
				for (int i = 0; i < 10; ++i)
					board[y][i].color = white;

				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { 0, 0 });

				tetris_board(board, NULL, 0);

				Sleep(300);



				// 이제 그 줄을 삭제하고 삭제한 모습을 보여줍니다.
				for (int i = 0; i < 10; ++i)
				{
					board[y][i].color = 0;
					board[y][i].value = 0;
				}
					
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { 0, 0 });

				tetris_board(board, NULL, 0);

				Sleep(400);


				// 위의 블록들을 한칸씩 내립니다.
				for (int i = y; i > -1; --i)
				{

					for (int j = 0; j < 10; ++j)
					{
						// 윗 칸이 비어있을 경우, unfilled의 값을 1증가.
						if (board[i-1][j].value == 0)
							++unfilled;


						board[i][j] = board[i- 1][j];


						// 만약, 윗줄이 완전히 비어있다면, 다음 검사를 하러 갑니다.
						if (unfilled == 10)
						{
							filled = 0;
							unfilled = 0;

							goto re;
						}
					}

					unfilled = 0;
				}

				filled = 0;
				unfilled = 0;


				goto re;

			}

		}

		
		filled = 0;
	}

	// 점수 총합 계산.
	total = combo * info.score;

	// 다음 계산을 위해서, combo를 0으로 만들어줍니다.
	combo = 0;

	// 이제 득점을 화면에 보여줄 필요가 없으므로, 0으로 만들어줍니다.
	score_on = 0;

	// 지금까지 얻은 점수를 반환.
	return total;
}



// 현재 점수를 감안하여, 스테이지 레벨을 조정합니다.
// 변동된 스테이지 레벨을 반환합니다.
int stageUp(int stage, int score )
{
	int changes = stage; // (changes: 변동 사항 여부)  (아래의 문장을 거친 후에, 이 changes와 stage의 값이 다를 경우, 화면에 stage up!을 입체적으로 출력)


	// 스테이지 검사 시작.
	// 1) 1 → 2
	if (stage == 1 && score >= 4000)
		stage = 2;

	// 2) 2 → 3
	else if (stage == 2 && score >= 9000)
		stage = 3;

	// 3) 3 → 4
	else if (stage == 3 && score >= 16000)
		stage = 4;

	// 4) 4 → 5
	else if (stage == 4 && score >= 30000)
		stage = 5;

	// 5) 5 → 6
	else if (stage == 5 && score >= 50000)
		stage = 6;

	// 6) 6 → 7
	else if (stage == 6 && score >= 60000)
		stage = 7;

	// 7) 7 → 8
	else if (stage == 7 && score >= 85000)
	{
		stage = 8;

		nG = 5;
	}

	// 8) 8 → 9
	else if (stage == 8 && score >= 120000)
	{
		stage = 9;

		nG = 10;
	}


	// 스테이지 레벨에 변동 사항이 생겼을 경우,
	// 화면에 stage up! 문자를 큼지막하게 출력합니다.
	if (stage != changes)
	{
		// 현재 상태를 출력.
		system("cls");

		strtd("\n     stage up!!");


		Sleep(1000);
	}

	return stage;
}



// 테트리스 판을 출력함과 동시에 몇 가지 정보들도 같이 출력합니다.
// (현재 점수)--(스테이지 레벨)--(다음 블록)--( 간단한 조작 키)
void status( int i )
{

	// 스테이지 레벨 정보.
	if (i == 1)
		printf("             Your level: %d              ", stage);


	// 현재 점수 정보.
	else if (i == 3)
	{
		// CUR_SCORE를 문자열로 만들어줍니다.
		char num[11]; // (num: 문자열로 바꾼 현재 점수.)  (최대 10 자리)

		sprintf(num, "%d", CUR_SCORE);

		int len = strlen(num); // (len: CUR_SCORE의 자릿수)


		// 문자열의 자릿수를 생각해서 출력을 합니다.
		// 단, 점수는 핑크색으로 표시합니다.
		printf("          SCORE ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

		// 문자열이 10자리 이하일 경우.
		if (len <= 10 && CUR_SCORE < INT_MAX)
		{
			printf("     %s", num);

			for (int i = 0; i < 19 - len; ++i)
				printf(" ");
		}

		// 문자열이 19자리 이상일 경우.
		else
			printf("     MAX                ");


		// 다시 원래 글자 색으로 되돌립니다.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
	}

	// 점수를 득점할때, 점수를 얼마나 얻었는지 보여줍니다.
	else if (score_on && i == 4)
	{
		// info.score를 문자열로 만들어 줍니다.
		char get[5]; // (get: 얻은 득점의 자릿수)

		sprintf(get, "%d", info.score);

		int len = strlen(get); // (info.score의 자릿수)


		// 얻은 점수도 핑크색으로 표시합니다.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

		printf("                   + %s x %d", get, combo);

		// 다시 원래 색으로 되돌림.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);

		// 뒷 처리를 합니다. 현재 블록의 점수에 따라서, 달라짐.
		for (int i = 0; i < 14 - len; ++i)
			printf(" ");

		// 2번째 뒷 처리. 콤보 수가 10자리일 경우.
		if (combo < 10)
			printf(" ");
	}


	// 다음에 나올 블록 정보.
	else if (i == 5)
		printf("                NEXT                    ");

	else if (i == 6)
		printf("              ________                  ");

	else if ( 7 <= i && i <11)
	{
		printf("             |");

		for (int j = 0; j < 4; ++j)
		{
			// 블록의 색깔 정하기
			// 블록 칸이 비어있다면, 기본 색으로 바꾸어줍니다.
			int color = (Next[i-7][j].value != 0) ? (Next[i-7][j].color) : DEFAULT; // (color: 해당 블록의 색깔 값을 보관합니다.)    (default: 15)

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);

			// 해당 칸에 블록이 있다면 ■을, 비어있다면 . 을 출력.
			printf("%s", Next[i-7][j].value ? "■" : "  ");
		}

		// 블록을 출력하는 것이 아니면, 글자 색은 기본 값으로 바꿉니다.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);

		printf("|                 ");
	}

	else if (i == 11)
		printf("              --------                  ");


	// 보여줄 정보가 없을 때에는 공백( 40칸 )을 출력.
	else
		printf("                                        ");
}



// 테트리스 판을 출력함과 동시에 게임의 조작법도 같이 출력합니다.
// (블록 회전)--(블록 이동)--(소프트 드롭)--(하드 드롭)
void guide(int i)
{
	if (i == 2)
		printf("       [  ↑  :   블록 회전     ]");

	else if (i == 3)
		printf("       [←  →: 블록 좌우 이동  ]");

	else if(i==4)
		printf("       [  ↓  :   소프트 드롭   ]");

	else if(i==6)
		printf("       [SPACE :     하드드롭    ]");
}



// 게임 방법을 알려줍니다.
void How_to()
{
	int page = 1; // (page: 현재 페이지)  (default: 1)
	char choice = 0; // (choice: 방향키 입력)

TOP:

	// 페이지 1.
	if (page == 1)
	{
		system("cls");

		strtd("        how to?");

		printf("\n <기본 조작>\n\n -> [ ← → : 블록을 좌우로 움직이기, ↑ : 블록을 오른쪽으로 90도 회전,  ↓: 소프트 드롭,  SPACE: 하드 드롭. ]\n");
		printf("\n\n <점수>\n\n -> 블록을 쌓으면서, 쌓은 블록이 1줄을 가득 채우면 그 줄은 사라지며, 점수를 얻습니다. 얻는 점수는 블록마다 다르며");
		printf("\n\n   한번에 여러 줄을 없앨 경우, 콤보 수가 올라갑니다. 즉, 얻는 점수는 ( 블록의 점수 x 콤보 수 )가 되게 됩니다.");
		printf("\n\n\n <인피니티 룰>\n\n -> 블록이 땅에 떨어진 상태라도 버튼을 연타해서 좌우로 흔들거나 회전을 하는 동안에는 최종 고정이 되지 않습니다.");
		printf("\n\n\n\t\t\t\t\t\tpre ←  → next");
		printf("\n\n\n\t\t\t\t\t\t\t\t\t\t\t\tmade by hyunsoo jeong");

		// 가고 싶은 방향 입력.
		choice = _getch();

	}

	// 페이지 2
	else if (page == 2)
	{
		system("cls");

		strtd("        how to?");

		printf("\n <스테이지>\n\n -> 스테이지는 총 9까지 있으며, 특정 점수 이상이 되면, 자동으로 증가합니다. 1스테이지에는 기본블록 7가지만 등장합니다.");
		printf("\n\n   이후 2~7스테이지에 돌입하게 되면, 신규 블록 2종이 추가됩니다. 8~9스테이지는 블록의 낙하 속도가 조금 증가하게 됩니다.\n");
		printf("\n\n <고스트>\n\n -> 고스트는 현 시점에서 일직선으로 쭉 떨어뜨렸을때, 블록이 고정될 지점을 미리 보여줍니다.");
		printf("\n\n\n <게임 결과>\n\n -> 게임이 끝났을 때, 얻은 점수에 따라서 메세지를 받게 됩니다. 메세지는 총 8개입니다. 모두 찾아보세요!.");
		printf("\n\n\n\t\t\t\t\t\tpre ←  → next");
		printf("\n\n\n\t\t\t\t\t\t\t\t\t\t\t\tmade by hyunsoo jeong");
		

		choice = _getch();
	}

	// 방향키를 입력했을 경우에만 실행.
	if (choice == -32)
	{
		choice = _getch();

		// ← 입력 시.
		if (choice == 75)
			page -= 1;

		// → 입력 시.
		else if (choice == 77)
			page += 1;

	}


	// 없는 페이지를 골랐을 경우, 설명 종료.
	if ( page <= 0 || page > 2 )
		return;

	Sleep(50);

	goto TOP;
}