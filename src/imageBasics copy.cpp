/*
 * Copyright (C) 2021 Chuanbeibei Shi and Yushu Yu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Eigen/Dense>
#include "upsample.h"
#include "imageBasics.h"


int main(int argc, char **argv) {
  // 读取argv[1]指定的图像
  cv::Mat image;
	
  ifstream infile;
  infile.open("velo_data", ios::in);
  
  if (!infile.is_open())
  {
  	cout << "read file failed" << endl;
  }
  //input as string.
  string buf;
  unsigned long line_no = 0;	
  vector<pointcoordinate> pc_array;	//Five coordinates for each point
  
  while (getline(infile,buf)) //each line
  {
  	cout << buf << endl;
  	int index = buf.find(",");
  	//cout << index << endl;
  	int index2 = buf.find(",", index + 1);
  	int index3 = buf.find(",", index2 + 1);//velo_data 3 colomns
  	//cout<< index << "2." << index2 << "3, " << index3 <<  endl; 
  
  	string s1 = buf.substr(0, index);
  	string s2 = buf.substr(index+1, index2-index-1);
  	string s3 = buf.substr(index2+1, index3-index2-1);
  	string s4 = buf.substr(index3+1, buf.length()-index3-1);
  	//cout << "s1:" << s1 << ", s2:" << s2 << ", s3: " << s3 << ", s4: " << s4 << endl;
  	//float of = stof(s1,0);
  	//cout << "test transfer: " << of << endl; 
  	pointcoordinate thispoint;
  	thispoint.x_3d = stof(s1, 0);
  	thispoint.y_3d = stof(s2, 0);
  	thispoint.z_3d = stof(s3, 0);//3D coordinates
  	thispoint.print();
    pc_array.push_back(thispoint); 

  	line_no++;
  }
  cout << "line number: " << line_no << endl; 
  infile.close(); //close file stream 

  ifstream infile2;
  infile2.open("velo_img_data", ios::in);
  
  if (!infile2.is_open())
  {
  	cout << "read file failed" << endl;
  }
  //input as string. 
  string buf2;
  unsigned long line_no_2 = 0;	
  
  while (getline(infile2,buf2))
  {
  	cout << buf2 << endl;
  	int index = buf2.find(",");
  	//cout << index << endl;
  
  	string s1 = buf2.substr(0, index);
  	string s2 = buf2.substr(index+1, buf2.length()-index-1);//get pixel coodinates
  	//cout << "s1:" << s1 << ", s2:" << s2 << ", s3: " << s3 << ", s4: " << s4 << endl;
  	//float of = stof(s1,0);
  	//cout << "test transfer: " << of << endl; 
  	/*pointcoordinate thispoint;
  	thispoint.u_px = stof(s1, 0);
  	thispoint.v_px = stof(s2, 0);
  	thispoint.print();*/
	pc_array[line_no_2].u_px = stof(s1,0); 
	pc_array[line_no_2].v_px = stof(s2,0); 
	cout << "current line number: " << line_no_2 << endl;
	pc_array[line_no_2].print();
  
  	line_no_2++;
  }
  cout << "line number: " << line_no << endl; 
  infile2.close();   

  double pu = pc_array[0].u_px; 
  double pv = pc_array[1].v_px;

  image = cv::imread(argv[1]); //cv::imread函数读取指定路径下的图像
  Eigen::Vector2d u;
  u << 0,1;
  // upsample(3,3);
  // 判断图像文件是否正确读取
  if (image.data == nullptr) { //数据不存在,可能是文件不存在
    cerr << "文件" << argv[1] << "不存在." << endl;
    return 0;
  }

  // 文件顺利读取, 首先输出一些基本信息
  cout << "图像宽为" << image.cols << ",高为" << image.rows << ",通道数为" << image.channels() << endl;
  cv::imshow("image", image);      // 用cv::imshow显示图像
  cv::waitKey(0);                  // 暂停程序,等待一个按键输入

  // 判断image的类型
  if (image.type() != CV_8UC1 && image.type() != CV_8UC3) {
    // 图像类型不符合要求
    cout << "请输入一张彩色图或灰度图." << endl;
    return 0;
  }

  // 遍历图像, 请注意以下遍历方式亦可使用于随机像素访问
  // 使用 std::chrono 来给算法计时
  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
  for (size_t y = 0; y < image.rows; y++) {
    // 用cv::Mat::ptr获得图像的行指针
    unsigned char *row_ptr = image.ptr<unsigned char>(y);  // row_ptr是第y行的头指针
    for (size_t x = 0; x < image.cols; x++) {
      // 访问位于 x,y 处的像素
      unsigned char *data_ptr = &row_ptr[x * image.channels()]; // data_ptr 指向待访问的像素数据
      // 输出该像素的每个通道,如果是灰度图就只有一个通道
      for (int c = 0; c != image.channels(); c++) {
        unsigned char data = data_ptr[c]; // data为I(x,y)第c个通道的值
      }
    }
  }
  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast < chrono::duration < double >> (t2 - t1);
  cout << "遍历图像用时：" << time_used.count() << " 秒。" << endl;

  // 关于 cv::Mat 的拷贝
  // 直接赋值并不会拷贝数据
  cv::Mat image_another = image;
  // 修改 image_another 会导致 image 发生变化
  image_another(cv::Rect(0, 0, 100, 100)).setTo(0); // 将左上角100*100的块置零
  cv::imshow("image", image);
  cv::waitKey(0);

  // 使用clone函数来拷贝数据
  cv::Mat image_clone = image.clone();
  image_clone(cv::Rect(0, 0, 100, 100)).setTo(255);
  cv::imshow("image", image);
  cv::imshow("image_clone", image_clone);
  cv::waitKey(0);

  // 对于图像还有很多基本的操作,如剪切,旋转,缩放等,限于篇幅就不一一介绍了,请参看OpenCV官方文档查询每个函数的调用方法.
  cv::destroyAllWindows();
  return 0;
}
