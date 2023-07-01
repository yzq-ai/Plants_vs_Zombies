

#include <ctime>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <graphics.h>
#include "tools.h"
//------������Ƶ��------
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
//----------------------

#define W_height  600
#define W_width   900


int CurX, CurY;	//ֲ�￨�Ƶĵ�ǰλ��
int CurPlant;	//��ǰֲ��
int SunValue;	//����ֵ

enum {
	WAN_DOU, XIANG_RI_KUI, KU_GUA
};

IMAGE ImgBg;				//��ʾ����ͼƬ
IMAGE ImgBar;				//������ͼƬ
IMAGE ImgCards[KU_GUA];		//��������
IMAGE* ImgPlant[KU_GUA][20];//ֲ������
IMAGE ImgSunShineBall[29];	//������
IMAGE IMGZombie[22];		//��ʬ


struct  Plant //ֲ��ṹ��
{
	int type;		// 0:û��ֲ�� 1:��һ��ֲ��
	int frameIndex; //����֡���
} map[3][9];			//��ͼ���飬����洢ֲ��

struct  SunShineBall //������ṹ��
{
	int x, y;		//��ǰ��λ��
	int frameIndex; //��ǰ��ʾ��ͼƬ֡���
	int goalY;		//Ŀ��λ��
	bool used;      //�Ƿ�����ʹ��
	int timer;		//��ʱ��
}SunPool[10];	//�����

struct  zombie//��ʬ�ṹ��
{
	int x, y;
	int frameIndex;
	bool used;
	int speed;
}Zombies[10];


//�ļ��Ƿ����
bool FileExist(const char* name) {
	std::ifstream file(name);
	return file.good();
}

//��Ϸ��ʼ��
void GameInit() {

	memset(ImgPlant, 0, sizeof ImgPlant);	//��ʼ��ֲ������
	memset(map, 0, sizeof map);				//��ʼ����ͼ����
	memset(SunPool, 0, sizeof SunPool);		//��ʼ�������
	memset(Zombies, 0, sizeof Zombies);		//��ʼ����ʬ����

	//����ͼƬ,ͼƬ�����ڴ������
	loadimage(&ImgBg, "res/bg.jpg");
	loadimage(&ImgBar, "res/bar5.png");



	//��ʼ��ֲ�￨��
	char name[64];
	for (int i = 0; i < KU_GUA; i++) {
		// ����ֲ�￨�Ƶ��ļ���
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
	//��������ͼƬ
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof name, "res/sunshine/%d.png",i+1);
		loadimage(&ImgSunShineBall[i],name);
	}
	//���ؽ�ʬͼƬ
	for (int i = 0; i < 22; i++) {
		sprintf_s(name,sizeof name,"res/zb/%d.png",i+1);
		loadimage(&IMGZombie[i], name);
	}

	//�������������
	srand(time(NULL));

	//������Ϸͼ�δ���
	initgraph(W_width, W_height, 0);

	//��������
	LOGFONT	font;
	gettextstyle(&font);
	font.lfHeight = 30;
	font.lfWeight = 15;
	strcpy(font.lfFaceName, "Segoe UI Black");	//��������Ч��
	font.lfQuality = ANTIALIASED_QUALITY;		//�����
	settextstyle(&font);
	setbkmode(TRANSPARENT);					//����ģʽ������͸��
	setcolor(BLACK);						//������ɫ����ɫ

}

//���ڸ���
void UpdateWindow() {
	BeginBatchDraw();//��ʼ����
	//��ʾͼƬ
	putimage(0, 0, &ImgBg);
	//putimage(250, 0, &ImgBar);
	putimagePNG(250, 0, &ImgBar);


	for (int i = 0; i < KU_GUA; i++) {
		int x = 335 + i * 64;
		int y = 6;
		putimage(x, y, &ImgCards[i]);
	}

	//��Ⱦ��ͼ��ֲ��
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
	//��Ⱦ���϶������е�ֲ�￨��
	if (CurPlant > 0) {
		IMAGE* img = ImgPlant[CurPlant - 1][0];
		putimagePNG(CurX - img->getwidth() / 2, CurY - img->getheight() / 2, img);
	}

	int BallMax = sizeof(SunPool) / sizeof(SunPool[0]);//����������������
	for (int i = 0; i < BallMax; i++) {
		if (SunPool[i].used) {
			IMAGE* img = &ImgSunShineBall[SunPool[i].frameIndex];
			putimagePNG(SunPool[i].x,SunPool[i].y,img);
		}
	}

	//��Ⱦ����ֵ
	char ScoreText[8];
	sprintf_s(ScoreText, sizeof ScoreText, "%d", SunValue);
	outtextxy(285,67,ScoreText);//�������ֵ

	EndBatchDraw();//����˫����
}
//�ռ�����
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
				SunValue += 25;
				mciSendString("play res/sunshine.mp3", 0, 0, 0);
			}

		}
	}
}
//�û����
void UserClick() {
	ExMessage msg;//��Ϣ����
	static int status = 0;
	if (peekmessage(&msg)) { //������Ϣ
		if (msg.message == WM_LBUTTONDOWN) //�������
		{
			if (msg.x > 335 && msg.x < 335 + 65 * KU_GUA && msg.y < 96){//ѡ����
				int index = (msg.x - 335) / 65;
				status = 1;
				CurPlant = index + 1;
			}
			else {	//�ռ�����
				CollectSunShine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1)//����ƶ�
		{
			CurX = msg.x;
			CurY = msg.y;


		}
		else if (msg.message == WM_LBUTTONUP) //����ͷ�
		{

			if (msg.x > 256 && msg.y > 179 && msg.y < 489){
				int row = (msg.y - 180) / 102;
				int col = (msg.x - 256) / 81;
				if(!map[row][col].type) //��ǰλ�ò�����ֲ��
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
//��������
void CreateSunShine() {
	static int count = 0;
	static int fre = 400;
	count++;
	if (count >= fre) {
		fre = 200 + rand() % 200;
		count = 0;

		//���������ѡ��һ�����õ�����
		int BallMax = sizeof(SunPool) / sizeof(SunPool[0]);//����������������

		int i = 0;
		for (i; i < BallMax && SunPool[i].used; i++);
		if (i > BallMax) return;

		SunPool[i].used = true;
		SunPool[i].frameIndex = 0;
		SunPool[i].x = 260 + rand() % 640;//900 - 260
		SunPool[i].y = 60;
		SunPool[i].goalY = 200 + (rand() % 4) * 90;
		SunPool[i].timer = 0;
	}
	
	
}
//��������
void UpdateSunshine(){
	int BallMax = sizeof(SunPool) / sizeof(SunPool[0]);//����������������
	for (int i = 0; i < BallMax; i++) {
		if (SunPool[i].used) {
			SunPool[i].frameIndex = (SunPool->frameIndex + 1) % 29;
			if (SunPool[i].timer == 0) {
				SunPool[i].y += 2;
			}
			if (SunPool[i].y >= SunPool[i].goalY) {
				SunPool[i].timer++;
				if (SunPool[i].timer > 100) {
					SunPool[i].used = false;
				}
			}
		}
	}
}
//������ʬ
void CreatZombie() {
	int i;
	int ZombieMax = sizeof(Zombies) / sizeof(Zombies[0]);

}

// ���ݸ���
void UpdateGame() {
	//ֲ����ֲ��Ķ���
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
	CreateSunShine();//��������
	UpdateSunshine();//��������
	//CreatZombie();	 //������ʬ
}
//����UI
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