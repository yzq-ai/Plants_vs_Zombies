
#include <cmath>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <graphics.h>
#include "tools.h"
//------引入音频库------
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
//----------------------

#define W_height  600
#define W_width   900


int CurX, CurY;	//植物卡牌的当前位置
int CurPlant;	//当前植物
int SunValue;	//阳光值

enum {
	WAN_DOU, XIANG_RI_KUI, KU_GUA
};

IMAGE ImgBg;				//表示背景图片
IMAGE ImgBar;				//工具栏图片
IMAGE ImgCards[KU_GUA];		//卡牌数组
IMAGE* ImgPlant[KU_GUA][20];//植物数组
IMAGE ImgSunShineBall[29];	//阳光球
IMAGE ImgZombie[22];		//僵尸
IMAGE ImgBulletNormal;		//正常子弹
IMAGE ImgBulletBlast[4];		//爆照子弹



struct  Plant //植物结构体
{
	int type;		// 0:没有植物 1:第一种植物
	int frameIndex; //序列帧序号
} map[3][9];			//地图数组，方便存储植物

struct  SunShineBall //阳光球结构体
{
	int x, y;		//当前的位置
	int frameIndex; //当前显示的图片帧序号
	int goalY;		//目标位置
	bool used;      //是否正在使用
	int timer;		//计时器

	float xOff;		//飞跃的x
	float yOff;		//飞跃的y

}SunPool[10];	//阳光池

struct  zombie//僵尸结构体
{
	int x, y;
	int frameIndex;
	bool used;
	int speed;
	int row;
	int blood;
}Zombies[10];

struct  bullet //子弹结构体
{
	int x, y;
	int row;
	int speed;
	bool used;
	bool blast;		//是否爆炸
	int frameindex;	//序列帧
}Bullets[30]; 

//文件是否存在
bool FileExist(const char* name) {
	std::ifstream file(name);
	return file.good();
}

//游戏初始化
void GameInit() {

	memset(ImgPlant, 0, sizeof ImgPlant);	//初始化植物数组
	memset(map, 0, sizeof map);				//初始化地图数组
	memset(SunPool, 0, sizeof SunPool);		//初始化阳光池
	memset(Zombies, 0, sizeof Zombies);		//初始化僵尸数组
	memset(Bullets, 0, sizeof Bullets);		//初始化子弹数组
//	memset(ImgBulletBlast,0,sizeof ImgBulletBlast);//

	//加载图片,图片放在内存变量中
	loadimage(&ImgBg, "res/bg.jpg");							//背景图片
	loadimage(&ImgBar, "res/bar5.png");							//卡牌框图片
	loadimage(&ImgBulletNormal,"res/bullets/bullet_normal.png");//普通子弹图片
	loadimage(&ImgBulletBlast[3], "res/bullets/bullet_blast.png");	//爆炸子弹图片
	//初始化爆炸子弹
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&ImgBulletBlast[i], "res/bullets/bullet_blast.png",
			ImgBulletBlast[3].getwidth() * k,
			ImgBulletBlast[3].getheight() * k, true);	//爆炸子弹图片
	
	}
	//初始化植物卡牌
	char name[64];
	for (int i = 0; i < KU_GUA; i++) {
		// 生成植物卡牌的文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&ImgCards[i], name);

		for (int j = 0; j < 20; j++)
		{
			sprintf_s(name, sizeof(name), "res/plant/%d/%d.png", i, j + 1);
			if (FileExist(name))
			{
				ImgPlant[i][j] = new IMAGE;
				loadimage(ImgPlant[i][j], name);
			}
			else break;
		}
	}

	SunValue = 50;
	//加载阳光图片
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof name, "res/sunshine/%d.png",i+1);
		loadimage(&ImgSunShineBall[i],name);
	}
	//加载僵尸图片
	for (int i = 0; i < 22; i++) {
		sprintf_s(name,sizeof name,"res/zb/%d.png",i+1);
		loadimage(&ImgZombie[i], name);
	}



	//配置随机数种子
	srand(time(NULL));

	//创建游戏图形窗口
	initgraph(W_width, W_height, 1);

	//设置字体
	LOGFONT	font;
	gettextstyle(&font);
	font.lfHeight = 30;
	font.lfWeight = 15;
	strcpy(font.lfFaceName, "Segoe UI Black");	//设置字体效果
	font.lfQuality = ANTIALIASED_QUALITY;		//抗锯齿
	settextstyle(&font);
	setbkmode(TRANSPARENT);					//字体模式：背景透明
	setcolor(BLACK);						//字体颜色：黑色

}
//绘制僵尸
void DrawZombie(){
	int ZbCount = sizeof(Zombies) / sizeof(Zombies[0]);
	for (int i = 0; i < ZbCount; i++) {
		if (Zombies[i].used) {
			IMAGE* img = &ImgZombie[Zombies[i].frameIndex];
			putimagePNG(Zombies[i].x,Zombies[i].y - img->getheight(),img);

		}
	}

}

//窗口更新
void UpdateWindow() {
	BeginBatchDraw();//开始缓冲
	//显示图片
	putimage(0, 0, &ImgBg);
	//putimage(250, 0, &ImgBar);
	putimagePNG(250, 0, &ImgBar);


	for (int i = 0; i < KU_GUA; i++) {
		int x = 335 + i * 64;
		int y = 6;
		putimage(x, y, &ImgCards[i]);
	}

	//渲染地图上植物
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				int x = 256 + j * 81;
				int y = 180 + i * 102 + 10;
				int PlantType = map[i][j].type - 1;
				int Index = map[i][j].frameIndex;
				putimagePNG(x,y,ImgPlant[PlantType][Index]);
			}
		}
	}
	//渲染，拖动过程中的植物卡牌
	if (CurPlant > 0) {
		IMAGE* img = ImgPlant[CurPlant - 1][0];
		putimagePNG(CurX - img->getwidth() / 2, CurY - img->getheight() / 2, img);
	}

	int BallMax = sizeof(SunPool) / sizeof(SunPool[0]);//可用阳光的最大数量
	for (int i = 0; i < BallMax; i++) {
		if (SunPool[i].used || SunPool[i].xOff) {
			IMAGE* img = &ImgSunShineBall[SunPool[i].frameIndex];
			putimagePNG(SunPool[i].x,SunPool[i].y,img);
		}
	}

	//渲染阳光值
	char ScoreText[8];
	sprintf_s(ScoreText, sizeof ScoreText, "%d", SunValue);
	outtextxy(285,67,ScoreText);//输出阳光值


	DrawZombie();//绘制僵尸
	//渲染子弹
	int BulletMax = sizeof(Bullets) / sizeof(Bullets[0]);
	for (int i = 0; i < BulletMax; i++) {
		if (Bullets[i].used) {
			if (Bullets[i].blast) {
				IMAGE* img = &ImgBulletBlast[Bullets[i].frameindex];
				putimagePNG(Bullets[i].x, Bullets[i].y, img);
			}
			else {
				putimagePNG(Bullets[i].x, Bullets[i].y, &ImgBulletNormal);
			}
		}
	}
	EndBatchDraw();//结束双缓冲
}
//收集阳光
void CollectSunShine(const ExMessage* msg) {
	int count = sizeof(SunPool) / sizeof(SunPool[0]);
	int w = ImgSunShineBall[0].getwidth();
	int h = ImgSunShineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (SunPool[i].used) {
			int x = SunPool[i].x;
			int y = SunPool[i].y;
			if (msg->x > x && msg->x < x + w && 
				msg->y> y && msg->y < y + h) {
				SunPool[i].used = false;
				mciSendString("play res/sunshine.mp3", 0, 0, 0);
				//设置阳光偏移量
				float goalX = 262;
				float goalY = 0;
				float angle = atan((y - goalY) / (x - goalX));
				SunPool[i].xOff = 4 * cos(angle);
				SunPool[i].yOff = 4 * sin(angle);
			}
		}
	}
}
//用户点击
void UserClick() {
	ExMessage msg;//消息类型
	static int status = 0;
	if (peekmessage(&msg)) { //存在消息
		if (msg.message == WM_LBUTTONDOWN) //左键按下
		{
			if (msg.x > 335 && msg.x < 335 + 65 * KU_GUA && msg.y < 96){//选择卡牌
				int index = (msg.x - 335) / 65;
				status = 1;
				CurPlant = index + 1;
			}
			else {	//收集阳光
				CollectSunShine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1)//鼠标移动
		{
			CurX = msg.x;
			CurY = msg.y;


		}
		else if (msg.message == WM_LBUTTONUP) //左键释放
		{

			if (msg.x > 256 && msg.y > 179 && msg.y < 489){
				int row = (msg.y - 180) / 102;
				int col = (msg.x - 256) / 81;
				if(!map[row][col].type) //当前位置不存在植物
				map[row][col].type = CurPlant;
				map[row][col].frameIndex = 0;
			}
			
			CurPlant = 0;
			status = 0;
		}
		else {

		}
	}
}
//创建阳光
void CreateSunShine() {
	static int count = 0;
	static int fre = 400;
	count++;
	if (count >= fre) {
		fre = 200 + rand() % 200;
		count = 0;

		//从阳光池中选择一个可用的阳光
		int BallMax = sizeof(SunPool) / sizeof(SunPool[0]);//可用阳光的最大数量

		int i = 0;
		for (i; i < BallMax && SunPool[i].used; i++);
		if (i > BallMax) return;

		SunPool[i].used = true;
		SunPool[i].frameIndex = 0;
		SunPool[i].x = 260 + rand() % 640;//900 - 260
		SunPool[i].y = 60;
		SunPool[i].goalY = 200 + (rand() % 4) * 90;
		SunPool[i].timer = 0;
		SunPool[i].xOff = 0;
		SunPool[i].yOff = 0;
	}
	
	
}
//更新阳光
void UpdateSunshine(){
	int BallMax = sizeof(SunPool) / sizeof(SunPool[0]);//可用阳光的最大数量
	for (int i = 0; i < BallMax; i++) {
		if (SunPool[i].used) {
			SunPool[i].frameIndex = (SunPool->frameIndex + 1) % 29;
			if (SunPool[i].timer == 0) {
				SunPool[i].y += 4;
			}
			if (SunPool[i].y >= SunPool[i].goalY) {
				SunPool[i].timer++;
				if (SunPool[i].timer > 100) {
					SunPool[i].used = false;
				}
			}
		}
		else if (SunPool[i].xOff) {

			//设置阳光偏移量
			float goalX = 262;
			float goalY = 0;
			float angle = atan((SunPool[i].y - goalY) / (SunPool[i].x - goalX));
			SunPool[i].xOff = 4 * cos(angle);
			SunPool[i].yOff = 4 * sin(angle);

			SunPool[i].x -= SunPool[i].xOff;
			SunPool[i].y -= SunPool[i].yOff;
			if (SunPool[i].x < 262 || SunPool[i].y < 0) {
				SunPool[i].xOff = 0;
				SunPool[i].yOff = 0;
				SunValue += 25;
			}
		}
	}
}
//创建僵尸
void CreateZombie() {
	static int zmfre = 200;
	static int count = 0;//调用次数
	count++;
	if (count >= zmfre) {
		count = 0;
		zmfre = rand() % 200 + 300;
		int i;
		int ZombieMax = sizeof(Zombies) / sizeof(Zombies[0]);
		for (i = 0; i < ZombieMax && Zombies[i].used; i++);
		if (i < ZombieMax) {
			Zombies[i].used = true;
			Zombies[i].x = W_width;
			Zombies[i].row = rand() % 3;
			Zombies[i].y = 172 + ( Zombies[i].row + 1) * 100;
			Zombies[i].speed = 1;
			Zombies[i].blood = 100;
		}
	}
	

}
//更新僵尸
void UpdateZombie() {

	static int  count = 0;
	count++;

	if (count > 2) {
		count = 0;
		//更新僵尸的位置
		int ZombieMax = sizeof(Zombies) / sizeof(Zombies[0]);
		for (int i = 0; i < ZombieMax; i++) {
			if (Zombies[i].used) {
				Zombies[i].x -= Zombies[i].speed;
				if (Zombies[i].x < 170) {
					std::cout << "游戏结束！你被僵尸吃掉了脑子！\n";
					MessageBox(NULL, "游戏结束！", "游戏结束！", 0);
					exit(0);//待优化
				}
			}
		} 

		for (int i = 0; i < ZombieMax; i++) {
			Zombies[i].frameIndex = (Zombies[i].frameIndex + 1) % 22;
		}

	}
	
}
//发射子弹
void ShootBullet() {
	int rows[3] = { 0 };
	int ZombieMax = sizeof(Zombies) / sizeof(Zombies[0]);
	int BulletMax = sizeof(Bullets) / sizeof(Bullets[0]);
	int DangerX = W_width - ImgZombie->getwidth();
	for (int i = 0; i < ZombieMax; i++) {
		if (Zombies[i].used && Zombies[i].x < DangerX) {
			rows[Zombies[i].row] = 1;
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == WAN_DOU + 1 && rows[i]) {
				static int count = 0;
				count++;
				if (count > 20) {
					count = 0;
					int k;
					for (k = 0; k < BulletMax && Bullets[k].used; k++);
					if (k < BulletMax) {
						Bullets[k].used = true;
						Bullets[k].row = i;
						Bullets[k].speed = 6;
						Bullets[k].blast = false;
						Bullets[k].frameindex = 0;

						int PlantX = 256 + j * 81;
						int PlantY = 179 + i * 102 + 14;

						Bullets[k].x = PlantX + ImgPlant[map[i][j].type - 1][0]->getwidth()-10;
						Bullets[k].y = PlantY + 5;
					}
				}
			}
		}
	}

}
//更新子弹
void UpdateBullet() {
	int BulletMax = sizeof(Bullets) / sizeof(Bullets[0]);
	for (int i = 0; i < BulletMax; i++) {
		if (Bullets[i].used) {
			Bullets[i].x += Bullets[i].speed;	
			if (Bullets[i].x > W_width) {
				Bullets[i].used = false;
			}
		}
		//子弹碰撞时
		if (Bullets[i].blast) {
			Bullets[i].frameindex++;
			if (Bullets[i].frameindex > 3) {
				Bullets[i].used = false;
			}
		}
	}
}
//碰撞检测
void CollisionCheck(){
	int BulletMax = sizeof(Bullets) / sizeof(Bullets[0]);
	int ZombieMax = sizeof(Zombies) / sizeof(Zombies[0]);

	for (int i = 0; i < BulletMax; i++) {
		if (Bullets[i].used == false || Bullets[i].blast) continue;

		for (int j = 0; j < ZombieMax; j++) {
			if (Zombies[j].used == false) continue;
			int x1 = Zombies[j].x + 80;	
			int x2 = Zombies[j].x + 110;	
			int x = Bullets[i].x;
			if (Bullets[i].row == Zombies[j].row && x > x1 && x< x2){
				Zombies[j].blood -= 20;
				Bullets[i].blast = true;
				Bullets[i].speed = 0;
			}
		}
	}
}

// 数据更新
void UpdateGame() {
	//植物种植后的动画
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int PlantType = map[i][j].type - 1;
				int Index = map[i][j].frameIndex;
				if (ImgPlant[PlantType][Index] == NULL)
				{
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
	CreateSunShine();//创建阳光
	UpdateSunshine();//更新阳光
	CreateZombie();	 //创建僵尸
	UpdateZombie();	 //更新僵尸
	ShootBullet();	 //发射子弹
	UpdateBullet();	 //更新子弹
	CollisionCheck();//豌豆子弹与僵尸的碰撞检测
}
//启动UI
void StartUI(){
	IMAGE ImgUI;
	IMAGE ImgMenu1;
	IMAGE ImgMenu2;
	loadimage(&ImgUI,	"res/menu.png");
	loadimage(&ImgMenu1,"res/menu1.png");
	loadimage(&ImgMenu2,"res/menu2.png");
	int flag = false;
	while (true)
	{
		BeginBatchDraw();
		putimage(0,0,&ImgUI);
		putimagePNG(475,75,flag ? &ImgMenu2 : &ImgMenu1);
		
		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN && 
				msg.x> 475 && msg.x < 775 && 
				msg.y> 75 && msg.y < 215){
				flag = true;
			}
			else if (msg.message == WM_LBUTTONUP && flag) {
				return;
			}
		}
		EndBatchDraw();
	}
}

int main() {

	GameInit();
	StartUI();

	int timer = 0;
	bool flag = true;
	while (true)
	{
		UserClick();

		timer += getDelay();
		if (timer > 20) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			UpdateWindow();
			UpdateGame();
			flag = false;
		}


		
		Sleep(10);
	}

	system("pause");
	return 0;
}