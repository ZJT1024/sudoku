// ConsoleApplication3.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include<cstdio>
#include<cmath>
#include<cstring>
#include<string>
#include<algorithm>
#include<iostream> 
#include<time.h>
#include<cstdlib>

typedef long long int ll;

const int Maxn = 1e6 + 10;	//  需求数最大数 + 10
const int Maxm = 10;		//  理论上用9即可，但是为了防止不知名错误而越界，多加一个可以给字符串用
const int INF = 0x3f3f3f3f; //  视为正无穷使用
const int first_num = (2 + 5) % 9 + 1;  //  按题目要求，学号后两位相加模9再加1为第一行第一个数（所以首行不能动，第一个元素不参与首行全排列）


using namespace std;


typedef struct Point
{
	int row;	//  所在行
	int col;	//  所在列
	int num;	//  该空格所在行、列、小九宫格中的由题已知的数字的总和
}Point;


/*  因为放在函数体内的话，堆栈会爆，就只能放在外面，但是不能当做全局变量使用，这样会增加程序的耦合度，在generate部分使用  */
int move_step[80][Maxm];
int permutation[Maxn][Maxm];



int CheckOrder(char order[], int & num, char name[], int & type)
/******************************************************************************
参数：order[]为用户输入的指令		num：记录指令中可能出现的-c的参数（数独终局数量）	name：记录指令中可能出现的-s的参数（题目文件名）
type：记录该指令中的操作类型  1为-c，2为-s

返回值：返回值为int型，表示该指令order知否合法，合法为1，不合法为0
******************************************************************************/
{
	int len = strlen(order);
	int flag = 1;		// 作为函数检验结构的记录结构，使函数实现单出口
	int blank_num = 0;	// 记录指令中空格的数量
	int order_site = INF;	//记录操作指令的位置，便于对后面的参数进行校验


							/*  初始化  */
	num = 0;
	type = 0;
	memset(name, 0, sizeof(char) * 100);
	int name_site = 0;
	/*  结束  */

	if (len < 4) // 合法指令至少4位
	{
		flag = 0;
	}
	else
	{
		if (order[0] != '-')  // 合法指令的首位一定为 -
		{
			flag = 0;
		}
		else if (order[2] == ' ')    //  记录操作符类型
		{
			if (order[1] == 'c')
			{
				type = 1;
				order_site = 1;
			}
			else if (order[1] == 's')
			{
				type = 2;
				order_site = 1;
			}
			else  // 非法操作符
			{
				flag = 0;
			}
			blank_num = 1;
		}
	}


	for (int i = 3; i < len; i++)	// 从空格后第一位开始
	{
		if (i > order_site + 1)  // 如果当前位置在操作符之后
		{
			if (type == 1)
			{
				if (order[i] < '0' || order[i] > '9')	// -c的参数智能为数字
				{
					flag = 0;
					num = 0;	//  以防参数为前数字后字符的用例蒙混过关
					break;
				}
				else
				{
					num *= 10;
					num += order[i] - '0';
				}
			}
			else if (type == 2)		// -s的参数不能有空格（需为合法文件名）
			{
				if (order[i] == ' ')
				{
					flag = 0;
					memset(name, 0, sizeof(char) * strlen(name));
					break;
				}
				else
				{
					name[name_site] = order[i];
					name_site++;
				}
			}
		}
	}


	if (blank_num != 1 || order_site == INF || type == 0)  //  合法指令必须有且仅有一个空格，有一个操作符，且操作符合法
	{
		flag = 0;
	}
	if (type == 1 && num == 0)	// 当操作符为-c时，需求数量不能为0
	{
		flag = 0;
	}
	else if (type == 2 && strcmp(name, "") == 0)	// 当操作符为-s时，文件名不能为空
	{
		flag = 0;
	}


	return flag;

}

void Swap(int & a, int & b)
/**********************
参数 a、b两个数
功能：交换两个数的数值
***********************/
{
	int temp = a;
	a = b;
	b = temp;
}

void Permutate_for_temp(int source[], int start, int end, int target[6][3], int& line)
/*************************************************************************************
参数	source[]：为原始排列，之后生成的排列都是对该原始排列的变换，相当于种子
start：排列的起始位置，用于递归		end：排列的终止位置，用于递归
target[][]：记录排列好的序列，一行为一个排列	line：行号

功能：对2 ~ 9行划分好的平移偏移量小组进行排列，递归完成
*************************************************************************************/
{
	if (start > end)  // 防止错误输入
	{
		start = end;
	}

	if (start == end)
	{
		for (int i = 0; i <= end; i++)
		{
			target[line][i] = source[i];
		}
		line++;
	}
	else
	{
		for (int i = start; i <= end; i++)
		{
			Swap(source[i], source[start]);
			Permutate_for_temp(source, start + 1, end, target, line);
			Swap(source[i], source[start]);
		}
	}
}

void Permutate_for_move_step(int temp1[2][2], int temp2[6][3], int temp3[6][3], int max_num, int move_step[80][Maxm])
/*******************************************************************************************************
参数	temp1、temp2、temp3分别为二三行，四五六行，七八九行的平移偏移量全排列
max_num：用于记录需要的最多2 ~ 9行平移偏移量排列数
move_step[][]：记录2 ~ 9行的平移偏移量排列，每一行为一个排列

功能：对二三行，四五六行，七八九行的平移偏移量的全排列进行组合，生成2 ~ 9行的全排列
********************************************************************************************************/
{
	int cnt = 0;

	for (int i = 0; i < 2; i++)  //  二三行有两个排列，二选一
	{
		for (int j = 0; j < 6; j++)  //  四五六行有六个全排列，六选一
		{
			for (int k = 0; k < 6; k++)  //  七八九行有六个全排列，六选一
			{
				for (int r = 0; r < 8; r++)  //  每个排列有8个元素（首元素不动）
				{
					if (r < 2)
					{
						move_step[cnt][r] = temp1[i][r];
					}
					else if (r < 5)
					{
						move_step[cnt][r] = temp2[j][r - 2];
					}
					else
					{
						move_step[cnt][r] = temp3[k][r - 5];
					}
				}
				cnt++;
				if (cnt >= max_num)
				{
					return;
				}
			}
		}
	}
}

void Permutate_for_permutation(int source[], int start, int end, int target[Maxn][Maxm], int& line, int max_num)
/*****************************************************************************
参数：source[]：初始的排列（后续生成的所有排列通过该排列变换，相当于种子
start：需要排列的序列起点，用于递归
end：需要排列的序列终点，用于递归
target[Maxn][Maxm]：每一行记录一种排列
line：记录当前生成的排列是第几种
max_num：需要最多的首行排列数

作用：该函数能更具max_num和source[]，递归调用自身，完成对初始序列的全排序，将排序结果
放在target数组中，每一行放一种排序，最多有max_num行
******************************************************************************/
{
	if (start > end)  // 防止输入错误，导致越界
	{
		start = end;
	}

	if (start == end)	//  终止条件
	{
		for (int i = 0; i <= end; i++)
		{
			target[line][i] = source[i];
		}
		line++;
	}
	else
	{
		for (int i = start; i <= end; i++)
		{
			if (line >= max_num)	// 当前全排序还没有生成结束，但是应为当前的终局生成需求数不需要那么多，所以强制返回
			{
				return;
			}

			Swap(source[i], source[start]);		//  交换两个元素位置
			Permutate_for_permutation(source, start + 1, end, target, line, max_num);
			Swap(source[i], source[start]);
		}
	}
}

void FillTheMoveStep(const int max_num, int move_step[80][Maxm])	//  根据最大需求数求2 ~ 9行平移偏移量的排列
{
	int source2[] = { 1 , 4 , 7 }, source3[] = { 2 , 5 , 8 };	// 二三行一组，四五六行一组，七八九行一组
	int temp1[2][2], temp2[6][3], temp3[6][3];
	int line2 = 0, line3 = 0;

	temp1[0][0] = 3, temp1[0][1] = 6;
	temp1[1][0] = 6, temp1[1][1] = 3;

	Permutate_for_temp(source2, 0, 2, temp2, line2);
	Permutate_for_temp(source3, 0, 2, temp3, line3);

	Permutate_for_move_step(temp1, temp2, temp3, max_num, move_step);	//  根据各小组的排列信息组合2 ~ 9 行的排列
}

void FillThePermutaion(const int max_num, int permutation[Maxn][Maxm])
{
	int source[] = { first_num , 1 , 2 ,  3 , 4 , 5 , 6 , 7 , 9 };
	int line = 0;
	Permutate_for_permutation(source, 1, 8, permutation, line, max_num);
}

void FillTheBlock(int cnt, int move_step[80][Maxm], int permutation[Maxn][Maxm])
/*****************************************************************************
参数：cnt：指令中-c的参数，即需要的数独生成终局的数量
move_step[80][Maxm]：第2至9行的行平移偏移量，每一行为中排序，每行的第i个元素对应第i + 1行的平移偏移量
permutation[Maxn][Maxm]：首行排序，每一行对应一种排序

作用：该函数将首行全排列和2只9行每行平移偏移量结合起来，生成数独终局，一个首行全排列和一个平移偏移量排列即
可组成一个数独终局
******************************************************************************/
{
	int pl = cnt / 72;	//	需要的排列数量
	int ml = cnt % 72;  //	最后一个排列需要平移偏移量排列数


						//  为了减少最后输出是，写的次数，将一个数独终局以一个字符串的形式保存起来，最后一次性输出
	char temp[200];	// 一个数独终局的符号总数为9 * 9 * 2 = 162
	int temp_site = 0;

	memset(temp, 0, sizeof(char) * 200);

	for (int i = 0; i < 9; i++)		//  输出第一行（因为第一行不平移，所以单独处理）
	{
		temp[temp_site] = permutation[pl][i] + '0';
		temp_site++;

		if (i == 8)
		{
			temp[temp_site] = '\n';
			temp_site++;
		}
		else
		{
			temp[temp_site] = ' ';
			temp_site++;
		}
	}

	for (int i = 1; i < 9; i++)		//  输出 2 ~ 9 行
	{
		for (int j = 0; j < 9; j++)
		{
			int site = (j + move_step[ml][i - 1]) % 9;	//  对整行进行平移（向左）

			temp[temp_site] = permutation[pl][site] + '0';
			temp_site++;

			if (j == 8)
			{
				if (i == 8)
				{
					temp[temp_site] = '\0';
					temp_site++;
				}
				else
				{
					temp[temp_site] = '\n';
					temp_site++;
				}
			}
			else
			{
				temp[temp_site] = ' ';
				temp_site++;
			}
		}
	}

	puts(temp);
}

void Generate(int num)
{

	FillTheMoveStep(min(num, 72), move_step);	// 求解2 ~ 8 行平移偏移量排列
	FillThePermutaion(num / 72 + 1, permutation);  // 求解首行全排列

	for (int cnt = 0; cnt < num; cnt++)
	{
		FillTheBlock(cnt, move_step, permutation);  // 生成数独终局并输出
		if (cnt != num - 1)
		{
			printf("\n");
		}

	}
}

void SetMark(int str[Maxm][Maxm], const int& site, const int& num, const int flag)  // 将str数组的第site行,num列标记为flag，具体意义要看传入的数组是什么
{
	str[site][num] = flag;
}

int GetBlockNum(const int& x, const int& y)  //  根据元素的行号和列号生成元素所在小九宫格好（顺序从0开始，从左到右，从上到下）
{
	return x / 3 * 3 + y / 3;
}

int CheckNum(const int& rm, const int& cm, const int& bm)	//  根据三个打表的参数判断当前试探的数字是否合适，用于DFS中的空位元素试探
{
	return !(rm || cm || bm);
}

bool DFS(Point p[], const int& num, int rm[Maxm][Maxm], int cm[Maxm][Maxm], int bm[Maxm][Maxm], int step, int block[Maxm][Maxm])
/*****************************************************************************
参数：p[]：空位数组，在扫描之后，记录个空位的坐标（行，列）等有关信息
num：空位数量，记录空位总数，作为递归重点的依据
rm[Maxm][Maxm]：行元素记录表，rm[x][y] == 1 表示x行包含元素y
cm[Maxm][Maxm]：列元素记录表，cm[x][y] == 1 表示x列包含元素y
bm[Maxm][Maxm]：块元素记录表，bm[x][y] == 1 表示小九宫盒x包含元素y，小九宫格顺序为从0到8，一行一行编码
step：表示当前在试探的空位在空位数组中的脚标，用于递归
block[Maxm][Maxm]：记录一个数独残局

返回值：bool型，返回1表示递归找到了可行解，否则表示没有找到

作用：通过递归调用自身，按照空位数组p[]中的顺序对每个空位进行试探，当填满所有空位时，递归结束，找到一个可行解
******************************************************************************/
{
	if (step == num)
	{
		return true;
	}
	else if (step < 0 || step > num)
	{
		return false;
	}

	for (int i = 1; i <= 9; i++)	//  对于每个空位，从1到9依次试探
	{
		int r = p[step].row, c = p[step].col;

		if (CheckNum(rm[r][i], cm[c][i], bm[GetBlockNum(r, c)][i]))	// 检查在空位（r,c)上填数字i是否合适
		{
			/* 打表记录 */
			SetMark(rm, r, i, 1);
			SetMark(cm, c, i, 1);
			SetMark(bm, GetBlockNum(r, c), i, 1);

			block[r][c] = i;
			/*  结束  */

			if (DFS(p, num, rm, cm, bm, step + 1, block))	//搜索下一个空位
			{
				return true;	// 递归找到了一个可行解
			}

			/*  递归没有找到可行解，当前位置不能填数字i，恢复之前打表的数据  */
			SetMark(rm, r, i, 0);
			SetMark(cm, c, i, 0);
			SetMark(bm, GetBlockNum(r, c), i, 0);

			block[r][c] = 0;
			/*  结束  */
		}
	}

	return false;
}

void Output(int block[Maxm][Maxm])
{
	char temp[200]; // temp 最大数量为9 * 9 * 2 = 162
	int temp_site = 0;

	memset(temp, 0, sizeof(char) * 200);

	for (int i = 0; i < 9; i++)		//  将一个二维数组编程一个一维字符串，减少写的次数
	{
		for (int j = 0; j < 9; j++)
		{
			temp[temp_site] = block[i][j] + '0';
			temp_site++;

			if (j == 8)
			{
				if (i == 8)
				{
					temp[temp_site] = '\0';
				}
				else
				{
					temp[temp_site] = '\n';
				}
				temp_site++;
			}
			else
			{
				temp[temp_site] = ' ';
				temp_site++;
			}
		}
	}

	puts(temp);
}

bool Cmp(Point& x, Point& y)	// 更具两个参数的num属性（该元素周围的已知数的数量）比较大小
{
	return x.num > y.num;
}


void Init(int rowmark[Maxm][Maxm], int colmark[Maxm][Maxm], int blockmark[Maxm][Maxm],
	int row_num[Maxm], int col_num[Maxm], int block_num[Maxm],
	int& input_row, int& right, int& ans_num)
	/**************************************************************
	参数 见Solve函数模块
	功能：初始化参数
	*************************************************************/
{
	memset(row_num, 0, sizeof(int) * Maxm);
	memset(col_num, 0, sizeof(int) * Maxm);
	memset(block_num, 0, sizeof(int) * Maxm);
	memset(rowmark, 0, sizeof(int) * Maxm * Maxm);
	memset(colmark, 0, sizeof(int) * Maxm * Maxm);
	memset(blockmark, 0, sizeof(int) * Maxm * Maxm);
	input_row = 0;
	right = 1;
	ans_num = 0;
}

void  Solve()
{
	int rowmark[Maxm][Maxm];		//  eg.rowmark[1][3] = 1表示第1行已经有数字3
	int colmark[Maxm][Maxm];		//  eg.colmark[2][4] = 1表示第2列已经有数字4
	int blockmark[Maxm][Maxm];		//  eg.blockmark[3][5] = 1表示第3个小的九宫格中已经有数字5

	int row_num[Maxm], col_num[Maxm], block_num[Maxm];  // 用于记录每行每列每个小九宫格中题目给出的已知数字的数量，用于最后排序优先级的判断

	int block[Maxm][Maxm];		// 用于记录一道数独题的题目
	Point ans_point[Maxm * Maxm];  //用于记录空位数组
	int ans_num;  //  用于记录空位个数

	int right;	//  记录输入的已知数独是否正确


	char temp_input[2 * Maxm];
	int input_row;

	int mark = 0;  //  记录当前求解的是第几个数独残局，用于最后解的时候的格式判断

	Init(rowmark, colmark, blockmark, row_num, col_num, block_num, input_row, right, ans_num);

	while (gets_s(temp_input))
	{
		temp_input[strlen(temp_input)] = '\0';	// 在字符串末加\0防越界
		if (strcmp(temp_input, "") == 0)
		{
			if (input_row != 9)
			{
				if (mark != 0)
				{
					printf("\n");
				}

				/*  如果题目在读的过程中，没有读够9行就遇到空行，证明题目不全  */
				printf("Imcompleted input!\n");
				mark++;
			}

			Init(rowmark, colmark, blockmark, row_num, col_num, block_num, input_row, right, ans_num);
			continue;
		}

		if (right)  // 如果到目前为止，读到的数据都合法的话
		{
			int input_col = 0;  // 作为当前读入的这行题目的脚标，访问当前读入的这一行

			int len = strlen(temp_input);
			if (len != 17)
			{
				/*  合法的题目一行应该有且仅有9个数字，8个空格  */
				right = 0;
				len = 0;  //不执行下面的for循环
			}

			for (int i = 0; i < len; i++)
			{
				if (temp_input[i] <'0' || temp_input[i] > '9')
				{
					if (temp_input[i] != ' ')
					{
						/*  如果读到一个非0到9之间的字符，且这个字符不是空格  */
						right = 0;
						break;
					}

					continue;
				}

				/*  读到的字符在0到9之间  */
				int temp_num = temp_input[i] - '0';

				block[input_row][input_col] = temp_num;

				if (temp_num == 0)  //  如果该字符是0，证明是空位，放入空位数组中
				{
					ans_point[ans_num].row = input_row;
					ans_point[ans_num].col = input_col;
					ans_num++;
				}
				else  // 如果不是0，则打表记录相应信息，使得之后回溯时试探数字是能快速判断试探数字是否有效
				{
					if (rowmark[input_row][temp_num] == 1 ||
						colmark[input_col][temp_num] == 1 ||
						blockmark[GetBlockNum(input_row, input_col)][temp_num] == 1)
					{
						/*  如果当前数字已经在该 行 或 列 或 小九宫格 中出现过，则题目出错，该题没有解  */
						right = 0;
						break;
					}
					else  // 如果该数字满足要求，打表
					{
						SetMark(rowmark, input_row, temp_num, 1);
						SetMark(colmark, input_col, temp_num, 1);
						SetMark(blockmark, GetBlockNum(input_row, input_col), temp_num, 1);

						row_num[input_row] ++;
						col_num[input_col] ++;
						block_num[GetBlockNum(input_row, input_col)] ++;
					}
				}

				input_col++; // 处理完当前输入行的一个字符，之后处理下一个
			}

			input_row++;  //  处理完一行，等待下一行输入

			if (input_row == 9) // 如果已经输入了9行
			{
				if (right)	// 输入的9行都信息正确
				{
					for (int i = 0; i < ans_num; i++)
					{
						int r = ans_point[i].row, c = ans_point[i].col;
						ans_point[i].num = row_num[r] + col_num[c] + block_num[GetBlockNum(r, c)];
					}

					sort(ans_point, ans_point + ans_num, Cmp);	// 根据空位数组中每个元素的num（周围已知数的个数）属性进行从大到小的排序

					if (DFS(ans_point, ans_num, rowmark, colmark, blockmark, 0, block))		// 递归求解 
					{
						if (mark != 0)	// 递归求解成功找到一个解，且当前题目不是第一题，则在输出题目之前，先空一行
						{
							printf("\n");
						}

						Output(block);
						mark++;
					}
				}
				else  // 如果输入的九行中检测到错误
				{
					printf("Some errors?\n");
				}
			}
		}

	}

	/*  已经完成了所有行的读入，但是最后这题没有满9行  */
	if (input_row != 9)
	{
		if (mark != 0)
		{
			printf("\n");
		}

		printf("Imcompleted input!");
		mark++;
	}
}


int main()
{
	char order[100];	//  记录用户输入的指令
	int num, type;		//  num记录终局需求数量，type记录操作类型  1 为生成数独终局  2 为求解数独残局
	char name[100];		//  name记录-s 后的参数，即存储题目的文件名


	if (freopen("sudoku.txt", "w", stdout))
	{
		gets_s(order);		// 接受指令
		while (!CheckOrder(order, num, name, type))   //检查指令是否合法，如果非法，重新取指，如果合法，num中存放终局数量，type中存放问题求解方式
		{
			cout << "Please check the command!" << endl;
			gets_s(order);
		}

		if (type == 1)	//操作符为 -c
		{
			Generate(num);
		}
		else if (type == 2)  // 操作符为 -s
		{
			fclose(stdin);

			if (freopen(name, "r", stdin))
			{
				Solve();
			}
		}

		fclose(stdout);

	}

	return 0;
}