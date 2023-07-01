

#include <cstdio>
#include <fstream>
#include <iostream>
#include <graphics.h>
#include "tools.h"


#define W_height  600
#define W_width   900


int CurX, CurY;//植物卡牌的当前位置
int CurPlant;


enum {
	WAN_DOU, XIANG_RI_KUI, KU_GUA
};

IMAGE ImgBg;//表示背景图片
IMAGE ImgBar;//工具栏图片
IMAGE ImgCards[KU_GUA];//卡牌数组

IMAGE* ImgPlant[KU_GUA][20];//植物数组


bool FileExist(const char* name) {
	std::ifstream file(name);
	return file.good();
}



//游戏初始化
void GameInit() {

	memset(ImgPlant, 0, sizeof ImgPlant);
	//加载图片,图片放在内存变量中
	loadimage(&ImgBg, "res/bg.jpg");
	loadimage(&ImgBar, "res/bar5.png");
	//创建图形窗口



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

	initgraph(W_width, W_height, 1);

}

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

	//渲染，拖动过程中的植物卡牌
	if (CurPlant > 0) {
		IMAGE* img = ImgPlant[CurPlant - 1][0];
		putimagePNG(CurX - img->getwidth() / 2, CurY - img->getheight() / 2, img);
	}




	EndBatchDraw();//结束双缓冲
}


void UserClick() {
	ExMessage msg;//消息类型
	static int status = 0;
	if (peekmessage(&msg)) { //存在消息
		if (msg.message == WM_LBUTTONDOWN) //左键按下
		{
			if (msg.x > 335 && msg.x < 335 + 65 * KU_GUA && msg.y < 96)
			{
				int index = (msg.x - 335) / 65;
				status = 1;
				CurPlant = index + 1;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1)//鼠标移动
		{
			CurX = msg.x;
			CurY = msg.y;


		}
		else if (msg.message == WM_LBUTTONUP) //左键释放
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