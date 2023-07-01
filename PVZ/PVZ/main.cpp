

#include <cstdio>
#include <fstream>
#include <iostream>
#include <graphics.h>
#include "tools.h"


#define W_height  600
#define W_width   900


int CurX, CurY;//ֲ�￨�Ƶĵ�ǰλ��
int CurPlant;


enum {
	WAN_DOU, XIANG_RI_KUI, KU_GUA
};

IMAGE ImgBg;//��ʾ����ͼƬ
IMAGE ImgBar;//������ͼƬ
IMAGE ImgCards[KU_GUA];//��������

IMAGE* ImgPlant[KU_GUA][20];//ֲ������


bool FileExist(const char* name) {
	std::ifstream file(name);
	return file.good();
}



//��Ϸ��ʼ��
void GameInit() {

	memset(ImgPlant, 0, sizeof ImgPlant);
	//����ͼƬ,ͼƬ�����ڴ������
	loadimage(&ImgBg, "res/bg.jpg");
	loadimage(&ImgBar, "res/bar5.png");
	//����ͼ�δ���



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

	initgraph(W_width, W_height, 1);

}

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

	//��Ⱦ���϶������е�ֲ�￨��
	if (CurPlant > 0) {
		IMAGE* img = ImgPlant[CurPlant - 1][0];
		putimagePNG(CurX - img->getwidth() / 2, CurY - img->getheight() / 2, img);
	}




	EndBatchDraw();//����˫����
}


void UserClick() {
	ExMessage msg;//��Ϣ����
	static int status = 0;
	if (peekmessage(&msg)) { //������Ϣ
		if (msg.message == WM_LBUTTONDOWN) //�������
		{
			if (msg.x > 335 && msg.x < 335 + 65 * KU_GUA && msg.y < 96)
			{
				int index = (msg.x - 335) / 65;
				status = 1;
				CurPlant = index + 1;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1)//����ƶ�
		{
			CurX = msg.x;
			CurY = msg.y;


		}
		else if (msg.message == WM_LBUTTONUP) //����ͷ�
		{
			status = 0;
		}
		else {

		}
	}
}


int main() {

	GameInit();

	while (true)
	{
		UserClick();
		UpdateWindow();

	}

	system("pause");
	return 0;
}