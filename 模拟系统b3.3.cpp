#include <iostream>
#include <iomanip>//头文件
#include <chrono>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <thread>
#include <mutex>
#include <conio.h>
#include <fstream>
#include <winsock2.h>  // 必须在 Windows.h 之前
#include <windows.h>    // 确保它在 winsock2.h 之后
#include <ws2tcpip.h> 
#include <wininet.h>
#include "EmQuantAPI.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
const string API_URL = "https://api.deepseek.com/v1/chat/completions";  // DeepSeek API 端点
const string API_KEY = "sk-1f5ba585cadd414993127cc2498e775e"; //DeepSeek API Key
void queryStock() {
	HINTERNET hInternet = InternetOpen(L"StockQueryApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);//股票区
	if (hInternet == NULL) {
		cout << "InternetOpen failed: " << GetLastError() << endl;
		system("pause");
		return;
	}// 上证指数000001，https://mairui.club/的api
	HINTERNET hURL = InternetOpenUrl(hInternet, L"http://api.mairui.club/hsrl/ssjy/000001/F242BF19-1866-4979-9BC3-D0AA77CFD8D6", NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (hURL == NULL) {
		cout << "InternetOpenUrl failed: " << GetLastError() << endl;
		InternetCloseHandle(hInternet);
		system("pause");
		return;
	}
	char buffer[4096] = { 0 };
	DWORD bytesRead = 0;
	string result;
	while (InternetReadFile(hURL, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
		buffer[bytesRead] = '\0';
		result += buffer;
	}
	InternetCloseHandle(hURL);
	InternetCloseHandle(hInternet);
	cout << "股票（上证指数000001）查询结果:" << endl;
	cout << result << endl;
	system("pause");
}
void writeLog(const string& logMessage) {
	time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm localTime;
	localtime_s(&localTime, &now_time);
	std::cout << "当前时间："
		<< std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << std::endl;
}
void readLog() {
	ifstream logFile("system_log.txt");
	if (logFile.is_open()) {
		string line;
		cout << "\n=== 日志内容 ===\n";
		while (getline(logFile, line)) {
			cout << line << endl;
		}
		logFile.close();
	}
	else {
		cout << "日志文件不存在或无法打开." << endl;
	}
	system("pause");
}
void logModule() {
	int logChoice;
	do {
		system("cls");
		cout << "=== 日志记录模块 ===" << endl;
		cout << "1. 写入日志" << endl;
		cout << "2. 查看日志" << endl;
		cout << "3. 返回主菜单" << endl;
		cout << "请输入选择: ";
		cin >> logChoice;
		cin.ignore(); // 清除换行符

		if (logChoice == 1) {
			cout << "请输入日志内容: ";
			string logContent;
			getline(cin, logContent);
			writeLog(logContent);
			cout << "日志已写入!" << endl;
			system("pause");
		}
		else if (logChoice == 2) {
			readLog();
		}
	} while (logChoice != 3);
}
const int MAZE_WIDTH = 30;  // 增加迷宫宽度
const int MAZE_HEIGHT = 30; // 增加迷宫高度
const int OBSTACLE_COUNT = 280; // 增加障碍物数量
struct Maze {
	char maze[MAZE_HEIGHT][MAZE_WIDTH];
};
void initMaze(Maze& maze, int& playerX, int& playerY) {
	// 初始化迷宫
	for (int i = 0; i < MAZE_HEIGHT; i++) {
		for (int j = 0; j < MAZE_WIDTH; j++) {
			if (i == 0 || i == MAZE_HEIGHT - 1 || j == 0 || j == MAZE_WIDTH - 1) {
				maze.maze[i][j] = '#';  // 迷宫边界
			}
			else {
				maze.maze[i][j] = ' ';  // 迷宫内部为空格
			}
		}
	}// 设置玩家的初始位置
	playerX = 1;
	playerY = 1;
	maze.maze[playerY][playerX] = 'O';  // 玩家标记为'O'// 设置终点
	maze.maze[MAZE_HEIGHT - 2][MAZE_WIDTH - 2] = 'E';  // 终点标记为'E'// 随机生成障碍物
	srand(time(0));  // 使用时间作为随机种子
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		int x = rand() % (MAZE_WIDTH - 2) + 1; // 随机生成X坐标
		int y = rand() % (MAZE_HEIGHT - 2) + 1; // 随机生成Y坐标
		if (maze.maze[y][x] == ' ') {  // 如果该位置为空格，设置为障碍物
			maze.maze[y][x] = '#';
		}
	}
}
void displayMaze(const Maze& maze) {
	system("cls");
	for (int i = 0; i < MAZE_HEIGHT; i++) {
		for (int j = 0; j < MAZE_WIDTH; j++) {
			cout << maze.maze[i][j] << " ";
		}
		cout << endl;
	}
}
bool movePlayer(Maze& maze, int& playerX, int& playerY, char direction) {
	int newX = playerX;
	int newY = playerY;// 根据玩家输入的方向来更新位置
	switch (direction) {
	case 'w': newY--; break;  // 上
	case 's': newY++; break;  // 下
	case 'a': newX--; break;  // 左
	case 'd': newX++; break;  // 右
	default: return false;
	}// 判断是否撞墙或者撞到障碍物
	if (maze.maze[newY][newX] == '#' || newY <= 0 || newX <= 0 || newY >= MAZE_HEIGHT - 1 || newX >= MAZE_WIDTH - 1) {
		return false;
	}// 判断是否到达终点
	if (maze.maze[newY][newX] == 'E') {
		return true;
	}// 更新玩家位置
	maze.maze[playerY][playerX] = ' ';  // 清除旧位置
	playerX = newX;
	playerY = newY;
	maze.maze[playerY][playerX] = 'O';  // 更新新位置
	return false;
}
void mazeGame() {
	Maze maze;
	int playerX, playerY;
	initMaze(maze, playerX, playerY);
	bool gameOver = false;
	while (!gameOver) {
		displayMaze(maze);
		cout << "使用WASD控制方向，按Q退出游戏" << endl;
		char move = getchar();  // 获取玩家输入
		if (move == 'q') break;  // 退出游戏
		gameOver = movePlayer(maze, playerX, playerY, move);
	}
	if (gameOver) {
		displayMaze(maze);
		cout << "恭喜！你成功走出迷宫！" << endl;
	}
}
void displayClock() {
	system("cls");
	cout << "实时时钟（按任意键返回）\n";
	while (true) {
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
		if (_kbhit()) {
			int ch = _getch(); // 捕获返回值到临时变量
			break;
		}
		Sleep(1000); // 等待1秒
		system("cls");
	}
}
void encryptData(string& data) {
	char key = 'K';
	for (auto& c : data) {
		c ^= key;
	}
}
double jiafa(double number1, double number2) {//加法函数
	double deshu;
	deshu = number1 + number2;
	return deshu;
}
double jianfa(double number1, double number2) {//减法函数
	double deshu;
	deshu = number1 - number2;
	return deshu;
}
double chengfa(double number1, double number2) {//乘法函数
	double deshu;
	deshu = number1 * number2;
	return deshu;
}
double chufa(double number1, double number2) {//除法函数
	double deshu;
	deshu = number1 / number2;
	return deshu;
}
const string PI_DIGITS = "1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";//圆周率小数点后数位
int main() {//主函数
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(65000);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, 5);
	stringstream ss;
	double a, b, c, d, e, f, g, z, h, j, k, l, m, n, o, appnumber, appbanbenhao, appbanbenhao1, number1, number2, jisuan, deshu, appdate1, end, dmsl, dmsl2, wendu, tizhong, shengao, BMI, BMI1, BMI2, old;//创建double变量;
	BMI2 = 25, BMI1 = 18.5, b = 123456, z = 0, end = 9999, c = 1, d = 2, e = 3, f = 4, g = 5, h = 6, j = 7, k = 8, l = 9, m = 10, n = 11, o = 12;
	appbanbenhao = 3;//版本号（大更新）
	appbanbenhao1 = 3;//版本号（小更新）  
	cout << "欢迎使用模拟系统软件b" << appbanbenhao << "." << appbanbenhao1 << "代！！！" << endl << "用户名：01" << endl << "请输入密码（默认123456）：";
	cin >> a;
	if (a != b) {//判断密码是否正确 
		int attempts = 0;
		while (cin >> a && a != b && attempts < 5) {
			cout << "\033[31m密码错误！剩余尝试次数：\033[0m" << 5 - ++attempts << endl;
		}
		if (attempts == 5) exit(EXIT_FAILURE);
	}
	else {
		cout << "\033[32m欢迎！！！\033[0m" << endl;
	}
	for (int ad = 0; ad <= 1000; ad++) {
		cout << "请选择软件" << endl << "1.计算器" << endl << "2.讲个笑话" << endl << "3.访问官网" << endl << "4.系统信息" << endl << "5.温度换算" << endl << "6.BMI指数测试" << endl << "7.教学软件C++代码功能实例" << endl << "8.鸡兔同笼问题解决工具" << endl << "9.自选股（仅支持上证指数000001）" << endl << "10.时钟" << endl << "11.小游戏-走迷宫" << endl << "12.日志功能" << endl << "13.3月14日特供更新-圆周率记忆挑战" << endl << "输入9999关机" << endl << "输入选择：";//应用列表 
		cin >> appnumber;//输入软件选择 
		if (appnumber == c) {
			cout << "请输入第一个数字:";
			cin >> number1;
			cout << "请选择计算模式：" << endl << "1：加法" << endl << "2：减法" << endl << "3：乘法" << endl << "4：除法" << endl << "请输入：";
			cin >> jisuan;
			if (jisuan == c) {//加法 
				cout << "请输入第二个数字:";
				cin >> number2;
				cout << number1 << "+" << number2 << "=" << jiafa(number1, number2);
			}
			if (jisuan == d) {//减法 
				cout << "请输入第二个数字:";
				cin >> number2;
				cout << number1 << "-" << number2 << "=" << jianfa(number1, number2);
			}
			if (jisuan == e) {//乘法 
				cout << "请输入第二个数字:";
				cin >> number2;
				cout << number1 << "×" << number2 << "=" << chengfa(number1, number2);
			}
			if (jisuan == f) {//除法 
				cout << "请输入第二个数字:";
				cin >> number2;
				cout << number1 << "÷" << number2 << "=" << chufa(number1, number2);
			}
		}
		if (appnumber == d) {
			cout << "老八网购了一双“走路带风”的鞋子，满心欢喜。第一次穿上出门，迎面吹来一阵风，结果...鞋飞了！老八追着鞋跑，路人笑称：“看，这真是‘走路带风’！”";
		}
		if (appnumber == e) {
			cout << "官网是：hsustudios.mysxl.cn";
		}
		if (appnumber == f) {
			cout << "1.软件版本" << endl << "2.著作人" << endl << "3.软件引擎（编写语言）" << endl << "请输入您选择：";
			cin >> appdate1;
			if (appdate1 == c) {
				cout << "软件版本:" << appbanbenhao << "." << appbanbenhao1;
			}
			if (appdate1 == d) {
				cout << "著作人：heikexiaohei";
			}
			if (appdate1 == e) {
				cout << "软件引擎（编写语言）：c++98";
			}
		}
		if (appnumber == g) {
			cout << "请选择模式:" << endl << "1.华氏度换算为摄氏度" << endl << "2.摄氏度换算为华氏度" << endl << "请输入选择：";
			cin >> number1;
			if (number1 == c) {
				cout << "请输入华氏度：";
				cin >> wendu;
				cout << fixed << setprecision(5) << (wendu - 32) * 5 / 9;
			}
			if (number1 == d) {
				cout << "请输入摄氏度：";
				cin >> wendu;
				cout << fixed << setprecision(5) << wendu * 9 / 5 + 32;
			}
		}
		if (appnumber == h) {
			cout << "请输入身高：";
			cin >> shengao;
			cout << "请输入体重：";
			cin >> tizhong;
			BMI = tizhong / shengao * shengao;
			cout << "您的BMI指数为：" << BMI;
			if (BMI < BMI1) {
				cout << endl << "您的身材过轻";
			}
			if (BMI >= BMI2) {
				cout << endl << "您的身材过重";
			}
		}
		if (appnumber == j) {
			cout << "您选择了：C++代码功能实例" << endl << "此软件有：" << endl << "1.一维数组的插入与删除" << endl << "2.直接插入排序" << endl << "请输入选择：" << endl;
			cin >> dmsl;
			if (dmsl == c) {
				int as[100], bs, cs, ds, ns;//as是字符串，ns是字符串的长度，bs是要删除的位置 
				cout << "请输入数字的个数（尽量小于100）：" << endl;
				cin >> ns;
				cout << "请输入" << ns << "个数字：" << endl;
				for (int i = 0; i <= ns - 1; i++) {//输入 
					cin >> as[i];
				}
				cout << "请输入要删除的位置：" << endl;
				cin >> bs;
				for (int i = bs - 1; i < ns - 1; i++) {
					as[i] = as[i + 1];
				}
				cout << "请输入要加入的位置：" << endl;
				cin >> cs;//输入要改变的位置 
				for (int i = ns - 1; i >= cs - 1; i--) {//插入（腾出要插入的空间） 
					as[i + 1] = as[i];
				}
				cout << "输入要插入的数值：" << endl;
				cin >> as[cs - 1];//输入要插入的数值 
				cout << "数组是：" << endl;
				for (int i = 0; i <= ns - 1; i++) {//按顺序输出 
					cout << as[i] << " ";
				}
			}
			if (dmsl == d) {
				cout << "直接插入排序有两个输出方法：" << endl << "1.从大到小" << endl << "2.从小到大" << endl << "请输入选择：" << endl;
				cin >> dmsl2;
				if (dmsl2 == c) {
					cout << "您选择了“从大到小”";
					int all[1000];
					cout << "请输入数组个数（尽量小于1000）：" << endl;
					int nll;
					cin >> nll;
					cout << "请输入数组：" << endl;
					for (int i = 1; i <= nll; i++) {
						cin >> all[i];
					}
					for (int i = 2; i <= nll; i++) {
						int jll = i;
						while (all[jll] > all[jll - 1] && jll > 1) {
							swap(all[jll], all[jll - 1]);
							jll--;
						}
					}
					cout << "数组是：" << endl;
					for (int i = 1; i <= nll; i++) {
						cout << all[i] << " ";
					}
				}
				if (dmsl2 == d) {
					cout << "您选择了“从小到大”";
					int alll[1000];
					cout << "请输入数组个数（尽量小于1000）：" << endl;
					int nlll;
					cin >> nlll;
					cout << "请输入数组：" << endl;
					for (int i = 1; i <= nlll; i++) {
						cin >> alll[i];
					}
					for (int i = 2; i <= nlll; i++) {
						int jlll = i;
						while (alll[jlll] < alll[jlll - 1] && jlll>1) {
							swap(alll[jlll], alll[jlll - 1]);
							jlll--;
						}
					}
					cout << "数组是：" << endl;
					for (int i = 1; i <= nlll; i++) {
						cout << alll[i] << " ";
					}
				}
			}
		}
		if (appnumber == k) {
			int ac, bc, cc;
			cout << "请输入头的数量：" << endl;
			cin >> ac;
			cout << "请输入足的数量：" << endl;
			cin >> bc;
			ac = ac * 2;
			cc = bc - ac;
			ac = cc / 2;
			bc = cc - ac;
			cout << "鸡有" << ac << "只" << ' ' << "兔子有" << bc << "只";
		}
		if (appnumber == l) {
			cout << "\033[31m重要提示：因为我们的技术并不成熟，导致此“自选股”功能并不完善，所提供的数据可能不是最新的，而且因为麦蕊智数（MyData）API提供商的限制，此功能每天所有“模拟系统”用户累计使用次数不能超过50次，所以此应用只是个试验品，我们正在寻找更优质的API，请耐心等待！\033[0m" << endl << "相关代号解释：" << endl << "1.o (Open): 开盘价，当日第一个成交价；2.h (High): 当日最高价；3.hs (换手率): 成交量与流通股本的比率；4.lb (量比): 当前成交量与过去平均成交量的比值；5.l (Low): 当日最低价；" << endl << "6.lt (流通市值): 流通股的总市值；7.pe (市盈率): 股价与每股收益的比率；8.pc (Price Change): 当前价格较前收盘的变动值；9.p (Price): 当前最新价；10.sz (总市值): 公司或指数的总市值；" << endl << "11.cje (成交额): 当日累计成交金额；12.ud (涨跌幅): 当前价格较前收盘的涨跌百分比；13.v (Volume): 成交量（单位：手）；14.yc (昨收): 前一日收盘价；15.zf (振幅): 当日最高与最低价波动幅度；" << endl << "16.sjl (市净率): 股价与每股净资产的比率;17.zdf60 (60日涨跌幅): 过去60天的价格变动百分比；17.zdfnc (年初至今涨跌幅): 年初至今的价格变动百分比；18.t (Time): 数据更新时间。" << endl << "应相关政策要求，使用此应用请进行年龄认证！" << endl << "请输入您的年龄：";;
			cin >> old;
			if (old >= 18) {
				queryStock();
			}
			else {
				cout << "\033[31m道歉，根据国家相关政策，未满18岁人员禁止炒股投资！\033[0m";
			}
		}
		if (appnumber == m) {
			displayClock();
		}
		if (appnumber == n) {
			cout << "应相关政策要求，使用此应用请进行年龄认证！" << endl << "请输入您的年龄：";
			cin >> old;
			if (old >= 18) {
				mazeGame();
			}
			else {
				cout << "\033[31m道歉，根据国家新闻出版署发布的《关于防止未成年人沉迷网络游戏的通知》，未满18岁人员禁止访问游戏资源！\033[0m";
			}
		}
		if (appnumber == 12) { // 日志记录模块
			logModule();
		}
		if (appnumber == 13) {
			cout << "========== 圆周率记忆挑战 ==========\n";
			cout << "规则：根据已知的圆周率数字，输入下一个正确的数字\n";
			cout << "输入非数字字符或错误数字时游戏结束\n";
			cout << "3.";
			int score = 0;
			size_t index = 0;
			string known_digits = "3.";
			while (index < PI_DIGITS.length()) {
				cout << known_digits.substr(2) << endl;  // 显示已知部分（不显示3.）
				cout << "下一个数字是？ >> ";
				char input;
				cin >> input;
				if (!isdigit(input)) {
					cout << "输入的不是数字，游戏结束！\n";
					break;
				}
				if (input == PI_DIGITS[index]) {
					known_digits += input;
					score++;
					index++;
					cout << "\033[32m正确！\033[0m 当前长度：" << known_digits.length() - 2 << endl;// 阶段性鼓励语句
					if (score % 5 == 0) {
						cout << "★ 你已连续答对 " << score << " 位！保持势头！\n";
					}
				}
				else {
					cout << "\033[31m错误！\033[0m 正确数字应该是 " << PI_DIGITS[index] << "\n";
					break;
				}
			}
			cout << "\n========== 游戏结束 ==========\n";
			cout << "最终成绩：" << score << " 位\n";
			cout << "记忆序列：3." << known_digits.substr(2) << endl;// 根据成绩给出不同评价
			if (score >= 50) {
				cout << "🎉 你简直就是人形计算机！";
			}
			else if (score >= 30) {
				cout << "👏 惊人的记忆力！";
			}
			else if (score >= 20) {
				cout << "👍 不错的成绩！";
			}
			else {
				cout << "💪 继续加油，下次会更好！";
			}
			cout << endl;
		}
		if (appnumber == end) {//如果用户输入了9999那么就关闭软件（使用break;跳出循环） 
			break;
		}
		cout << endl << endl;
		ad = 0;
	}
	closesocket(sListen);
	WSACleanup();
	cout << endl << "感谢使用模拟系b" << appbanbenhao << "." << appbanbenhao << "代！！！" << endl << "用户已经关机...";
	return 0;
}
