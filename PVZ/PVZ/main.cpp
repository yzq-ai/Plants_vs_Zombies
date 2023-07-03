
#include <cmath>
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
IMAGE ImgZombie[22];		//��ʬ
IMAGE ImgBulletNormal;		//�����ӵ�
IMAGE ImgBulletBlast[4];		//�����ӵ�



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

	float xOff;		//��Ծ��x
	float yOff;		//��Ծ��y

}SunPool[10];	//�����

struct  zombie//��ʬ�ṹ��
{
	int x, y;
	int frameIndex;
	bool used;
	int speed;
	int row;
	int blood;
}Zombies[10];

struct  bullet //�ӵ��ṹ��
{
	int x, y;
	int row;
	int speed;
	bool used;
	bool blast;		//�Ƿ�ը
	int frameindex;	//����֡
}Bullets[30]; 

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
	memset(Bullets, 0, sizeof Bullets);		//��ʼ���ӵ�����
//	memset(ImgBulletBlast,0,sizeof ImgBulletBlast);//

	//����ͼƬ,ͼƬ�����ڴ������
	loadimage(&ImgBg, "res/bg.jpg");							//����ͼƬ
	loadimage(&ImgBar, "res/bar5.png");							//���ƿ�ͼƬ
	loadimage(&ImgBulletNormal,"res/bullets/bullet_normal.png");//��ͨ�ӵ�ͼƬ
	loadimage(&ImgBulletBlast[3], "res/bullets/bullet_blast.png");	//��ը�ӵ�ͼƬ
	//��ʼ����ը�ӵ�
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&ImgBulletBlast[i], "res/bullets/bullet_blast.png",
			ImgBulletBlast[3].getwidth() * k,
			ImgBulletBlast[3].getheight() * k, true);	//��ը�ӵ�ͼƬ
	
	}
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
		loadimage(&ImgZombie[i], name);
	}



	//�������������
	srand(time(NULL));

	//������Ϸͼ�δ���
	initgraph(W_width, W_height, 1);

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
//���ƽ�ʬ
void DrawZombie(){
	int ZbCount = sizeof(Zombies) / sizeof(Zombies[0]);
	for (int i = 0; i < ZbCount; i++) {
		if (Zombies[i].used) {
			IMAGE* img = &ImgZombie[Zombies[i].frameIndex];
			putimagePNG(Zombies[i].x,Zombies[i].y - img->getheight(),img);

		}
	}

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
		if (SunPool[i].used || SunPool[i].xOff) {
			IMAGE* img = &ImgSunShineBall[SunPool[i].frameIndex];
			putimagePNG(SunPool[i].x,SunPool[i].y,img);
		}
	}

	//��Ⱦ����ֵ
	char ScoreText[8];
	sprintf_s(ScoreText, sizeof ScoreText, "%d", SunValue);
	outtextxy(285,67,ScoreText);//�������ֵ


	DrawZombie();//���ƽ�ʬ
	//��Ⱦ�ӵ�
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
				mciSendString("play res/sunshine.mp3", 0, 0, 0);
				//��������ƫ����
				float goalX = 262;
				float goalY = 0;
				float angle = atan((y - goalY) / (x - goalX));
				SunPool[i].xOff = 4 * cos(angle);
				SunPool[i].yOff = 4 * sin(angle);
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
		SunPool[i].xOff = 0;
		SunPool[i].yOff = 0;
	}
	
	
}
//��������
void UpdateSunshine(){
	int BallMax = sizeof(SunPool) / sizeof(SunPool[0]);//����������������
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

			//��������ƫ����
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
//������ʬ
void CreateZombie() {
	static int zmfre = 200;
	static int count = 0;//���ô���
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
//���½�ʬ
void UpdateZombie() {

	static int  count = 0;
	count++;

	if (count > 2) {
		count = 0;
		//���½�ʬ��λ��
		int ZombieMax = sizeof(Zombies) / sizeof(Zombies[0]);
		for (int i = 0; i < ZombieMax; i++) {
			if (Zombies[i].used) {
				Zombies[i].x -= Zombies[i].speed;
				if (Zombies[i].x < 170) {
					std::cout << "��Ϸ�������㱻��ʬ�Ե������ӣ�\n";
					MessageBox(NULL, "��Ϸ������", "��Ϸ������", 0);
					exit(0);//���Ż�
				}
			}
		} 

		for (int i = 0; i < ZombieMax; i++) {
			Zombies[i].frameIndex = (Zombies[i].frameIndex + 1) % 22;
		}

	}
	
}
//�����ӵ�
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
//�����ӵ�
void UpdateBullet() {
	int BulletMax = sizeof(Bullets) / sizeof(Bullets[0]);
	for (int i = 0; i < BulletMax; i++) {
		if (Bullets[i].used) {
			Bullets[i].x += Bullets[i].speed;	
			if (Bullets[i].x > W_width) {
				Bullets[i].used = false;
			}
		}
		//�ӵ���ײʱ
		if (Bullets[i].blast) {
			Bullets[i].frameindex++;
			if (Bullets[i].frameindex > 3) {
				Bullets[i].used = false;
			}
		}
	}
}
//��ײ���
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
	CreateZombie();	 //������ʬ
	UpdateZombie();	 //���½�ʬ
	ShootBullet();	 //�����ӵ�
	UpdateBullet();	 //�����ӵ�
	CollisionCheck();//�㶹�ӵ��뽩ʬ����ײ���
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