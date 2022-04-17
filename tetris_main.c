// 테트리스 만들기
# include"tetris.h"

int main()
{
	// 콘솔 창 제목 설정.
	system("title tetris");
  
	// 콘솔 창 크기 조정.
	system("mode con cols=122 lines=34");

	BLOCK board[20][10] = { 0 }; // (board: 테트리스 판)    value--(0: 비어있음)--(1: 쌓여있는 블록)--(2: 떨어지는 블록)--(3: 고스트 블록)
	BLOCK block[4][4] = { 0 }; // (block: 떨어지는 블록)    value--(0: 비어있음)--(1: 쌓여있는 블록)--(2: 떨어지는 블록)--(3: 고스트 블록)


	char choice;// (choice: 키보드 입력 값)
	int menu = 0; // (menu: 게임의 메뉴 번호)--(0: 게임 시작)--(1: 게임 방법)--(2: 게임 종료)
	int Loof = 0; // (Loof: 강제 강하 타이밍)  (20이 되면, 1칸을 내려가기로 합니다.)
	int floated = 10; // (floated: 블록이 땅에 닿았을 때, 최종 고정이 되지 않는 시간)--(0: 고정 확정)--(1~10: 떠 있음)    (default:10)
	int hdrop_y = 0; // (hdrop_y: 하드 드롭을 실행할때, 현재 블록이 내려갈 수 있는 최대 y좌표)
	
menu:;

	// 메인 메뉴.
	while (1)
	{
		strtd("        tetris");

		printf("\n\n\n\t\t\t\t\t\t%s  게임시작", menu == 0 ? "→" : " ");
		printf("\n\n\n\t\t\t\t\t\t%s  게임방법", menu == 1 ? "→" : " ");
		printf("\n\n\n\t\t\t\t\t\t%s  게임종료", menu == 2 ? "→" : " ");

		// 사용자로부터 입력을 받습니다.
		choice = _getch();

		// 사용자가 방향키를 눌렀을 경우,
		if (choice == -32)
		{
			choice = _getch();

			// 윗키를 눌렀을 경우,
			if (choice == 72 && menu != 0)
				menu -= 1;

			// 아랫키를 눌렀을 경우,
			else if (choice == 80 && menu != 2)
				menu += 1;

		}

		// A키를 눌러서, 해당 메뉴를 선택했을 경우.
		else if (choice == 'a')
			break;

		// 화면 새로고침.
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { 0, 0 });

	}

	// (게임 방법) 선택 시
	if (menu == 1)
	{
		How_to();

		// 다 읽었으면, 다시 메뉴로 돌아갑니다.
		system("cls");

		goto menu;
	}

	// (게임 종료) 선택 시
	else if (menu == 2)
		return 0;




	// (게임 시작) 선택 시
	while (1)
	{
		// 화면을 지우고, 테트리스 판을 출력합니다.
		system("cls");

		// 떨어지는 블록을 정해줍니다.
		info = create_block(block, stage - 1);

		tetris_board(board, block, 0);


		// 맨 위까지 블록이 쌓였다면, 게임 오버로 간주합니다.
		for (int i = 0; i < 10; ++i)
		{
			if (board[0][i].value == 1)
				goto GameOver;
		}

		// 0.3초 쉽니다.
		Sleep(333);

		// 떨어지는 블록 조작.
		while (1)
		{
			// 화면을 지우고, 테트리스 판을 출력합니다.
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { 0, 0 });


			tetris_board(board,block, 1);


			// 키보드 입력 감지
			if ( _kbhit() )
			{
				choice = _getch();


				// 방향키를 눌렀는지 확인.
				if ( choice == -32 )
				{
					// 무슨 키를 눌렀는지 탐색.
					switch ( _getch() )
					{

					// ↑ 키( 블록 회전 )
					case 72:
						
						if ( !rotate(board, block) )
						{
							// 인피니티 상태라면, floated를 4증가.
							if (floated < 10)
								floated += 4;

						}break;

						    
					// ← 키
					case 75:
						
						if ( !vrtl_Mov(board, block, 0, -1) )
						{
							info.x -= 1;

							// 인피니티 상태라면, floated를 1증가.
							if (floated < 10)
								floated += 4;

							nature = 0;
						}break;

					// → 키
					case 77:
						
						if ( !vrtl_Mov(board, block, 0, 1) )
						{
							info.x += 1;

							// 인피니티 상태라면, floated를 1증가.
							if (floated < 10)
								floated += 4;

							nature = 0;
						} break;

					// ↓ 키
					case 80:
					
					FALL:

						if (!vrtl_Mov(board, block, 1, 0))
						{
							info.y += 1;

							// 인피니티 상태가 아닐때만 가능하므로,
							// 이때는 floated를 15로 초기화시킵니다.
							floated = 10;
						}

					}
				}


				// 스페이스바를 눌렀는지 확인.
				else if(choice == 32)
				{
					while ( !vrtl_Mov(board, block, hdrop_y + 1, 0) )
						++hdrop_y;


					info.y += hdrop_y;

					// 다음 하드드롭 계산을 위해, hdrop_y 초기화.
					hdrop_y = 0;
				}

				
			}



			// Loof가 20이 되면, 강제 강하를 시도합니다.
			if (Loof == 20-nG)
			{
				Loof = -1;

				goto FALL;
			}

			// 블록이 쌓여야할 상황에 놓였을때, 움직이는 블록을 영구적으로 적재해봅니다.
			if ( vrtl_Mov(board, block, 1, 0) )
			{
				// 블록이 떠있는 상태가 아닐 경우, 영구적 적재 실행.
				if ( floated==0  )
				{

					load_block(board, block,2);

					// 적재에 성공했으므로, 블록 조작 부분을 탈출합니다.
					break;
				}
				
				// floated를 감소시킵니다.
				--floated;
			}


			// 강제 강하 카운트 증가
			++Loof;

			// 0.05초 쉽니다.
			Sleep(50);
			
		}


		// 한 줄을 꽉 채운 곳이 있는지 검사.
		CUR_SCORE += line_delete(board);

		// 점수가 오버플로됬다면, 최대치로 바꾸어줍니다.
		if (CUR_SCORE < 0)
			CUR_SCORE = INT_MAX;

		// 스테이지 증가.( 최대 레벨 9 )
		if(stage < 9)
		  stage = stageUp(stage, CUR_SCORE );

		// 블록 값 초기화.
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				block[i][j].value = 0;
				block[i][j].color = 0;
			}
		}

		// 넥스트 블록 값 초기화.
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				Next[i][j].value = 0;
				Next[i][j].color = 0;
			}
		}

		// Loof 초기화.
		Loof = 0;

		// floated 초기화.
		floated = 0;

	}

GameOver:;

	// 점수에 따라 메세지를 출력합니다.
	// 1) it's  real?!..
	if (CUR_SCORE == 0)
		strtd("   it's  real?!..");

	// 2) that's ok.
	else if (CUR_SCORE < 5000)
		strtd("      that's ok.");

	// 3) good job!
	else if (CUR_SCORE < 15000)
		strtd("       good job!");

	// 4) wow!!!!
	else if (CUR_SCORE < 30000)
		strtd("        wow!!!!");

	// 5) wonderful..
	else if (CUR_SCORE < 100000)
		strtd("       wonderful..");

	// 6) fantastic!!
	else if (CUR_SCORE < 1000000)
		strtd("      fantastic!!");

	// 7) oh! my god!!!
	else if (CUR_SCORE < INT_MAX)
		strtd("     oh! my god!!!");

	// 점수가 최대치일 경우.
	else
		strtd("       you're \n  tetris master!");


	// 게임이 끝난 후, 바로 콘솔창이 꺼지는 것을 막는 용도.
	printf("\n\t\t\t\t\tA 키를 누르면, 종료합니다.");

	while (_getch() != 'a');

	return 0;
}


