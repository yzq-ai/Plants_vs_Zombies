## 植物大战僵尸

### 当前环境

* 主机

  ```
  Windows 10 家庭版
  版本号	20H2  
  安装日期	2020-09-22  
  操作系统内部版本	19042.1415
  体验	Windows Feature Experience Pack 120.2212.3920.0
  ```

* IDE 

  ```
  Microsoft Visual Studio Community 2019 (64 位) 版本 17.0.4
  ```

### 第三方库

* EasyX下载地址: [EasyX](https://easyx.cn/) 


### 项目运行

* 可执行文件  在 PVZ文件夹的 Debug文件夹中的 **PVZ.exe** 鼠标双击即可执行程序
* 解决方案    PVZ文件夹的**PVZ.sln** 

### 有关运行问题
* 错误处理

  * `图片加载不出来的问题`
  ```
    一般该问题是来自图片路径的问题，我们不喜欢将资源文件放在同一目录下，main.cpp所在的文件夹无法读取上一级目录中的文件，此时使用类似
    loadimage(&image, "C:/path/to/image.jpg");的路径可以解决该问题，但这种代码不友好。
    存在更好的解决方案 点击项目  属性 -->  VC++目录  --> 包含目录 --> 编辑 --> 写入 $(SolutionDir)/FilePath   
    
    注意
    $(SolutionDir) 是指你项目所在的根目录
    此处的FilePath是应该是图片所在的资源文件夹目录

  ```

  * `C2665 “loadimage”:2个重载中没有一个可以转换所有参数类型`

    ```c++
        1、第一种解决方法 
        点击项目的      属性  --> 高级 --> 字符集    将字符集修改为 “多字节字符集”
        1. 第二种解决方法
        使用 loadimage(&ImgBg,_T("Image Path"));  //使用你自己的图片路径
    ```
  
  * `C4996	'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.`
 
   ```c++
       项目属性  --> C/C++ --> 常规 --> SDL检查 --> 将  是(/sdl)  改为 否(/sdl-)
    ```
    
  * `C4996	'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.	`
    ```c++
       项目属性  --> C/C++ --> 常规 --> SDL检查 --> 将  是(/sdl)  改为 否(/sdl-)
    ```