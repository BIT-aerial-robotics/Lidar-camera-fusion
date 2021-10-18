//背景：某文件夹series下有若干张图片，对每一张图片进行处理，将处理后的图片保存至新的文件夹save下
#include<opencv2/opencv.hpp>
#include<iostream>
#include<time.h>
#include<fstream>

using namespace std;
using namespace cv;

int main() {
    ifstream file("testpic.txt");
    int img_index = 0;
    while (!file.eof()) {
        char txt_cont[200];
        file.getline(txt_cont, 200);
        char img_file[200], save_file[200];
        sprintf(img_file, "../bulid/matlab/pic/%s", txt_cont);
        sprintf(save_file, "../bulid/matlab/savepic/%2d.png", img_index);
        Mat src = imread(img_file);
        img_index++;
        imwrite(save_file, src);
    }
}
