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
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Eigen/Dense>

#include "upsample.h"
#include "imageBasics.h"

using namespace std;

int main(int argc, char **argv) {
  double Wp_x, Wp_y, Wp_z, Gr_x, Gr_y, Gr_z, Gs, S_x=0, S_y=0, S_z=0, Y_x=0, Y_y=0, Y_z=0;
  double mr_x=0,mr_y=0, mr_z=0;
  //int grid=5; // Size of the mask/kernel
  // 读取argv[1]指定的图像
  cv::Mat image;
  cv::Mat image1;
	
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
  	//cout << buf << endl;
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
  //cout << "line number: " << line_no << endl; 
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
  	//cout << buf2 << endl;
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
	//cout << "current line number: " << line_no_2 << endl;
	//pc_array[line_no_2].print();
  	line_no_2++;
  }
  //cout << "line number: " << line_no << endl; 
  infile2.close();   

  image = cv::imread(argv[1]); //cv::imread函数读取指定路径下的图像
  Eigen::Vector2d uu;
  uu << 0,1;
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
//  for (size_t y = 0; y < image.rows; y++) {
//    // 用cv::Mat::ptr获得图像的行指针
//    unsigned char *row_ptr = image.ptr<unsigned char>(y);  // row_ptr是第y行的头指针
//    for (size_t x = 0; x < image.cols; x++) {
//      // 访问位于 x,y 处的像素
//      unsigned char *data_ptr = &row_ptr[x * image.channels()]; // data_ptr 指向待访问的像素数据
//      // 输出该像素的每个通道,如果是灰度图就只有一个通道
//      for (int c = 0; c != image.channels(); c++) {
//        unsigned char data = data_ptr[c]; // data为I(x,y)第c个通道的值
//      }
//    }
//  }
  cv::Mat image_upsample  = image.clone();//clone original image used for upsampling
//  double pu = pc_array[line_no_2].u_px; 
//  double pv = pc_array[line_no_2].v_px;
//  double dx = pc_array[line_no_2].x_3d;
//  double dy = pc_array[line_no_2].y_3d;
//  double dz = pc_array[line_no_2].z_3d;
//  //double mr=0;
  
  for (int k=0; k<line_no; k=k+1){
    //double pu = pc_array[k].u_px; 
    //double pv = pc_array[k].v_px;
    double dx = pc_array[k].x_3d;
    double dy = pc_array[k].y_3d;
    double dz = pc_array[k].z_3d;
    //double mr=0;
  if  (dx > mr_x) { mr_x = dx; }// mr=max dx
  if  (dy> mr_y) { mr_y = dy; }// mr=max dx
  if  (dz > mr_z) { mr_z = dz; }// mr=max dx
  }
    cout << "mr_x: " << mr_x << endl; 
    cout << "mr_y: " << mr_y << endl; 
    cout << "mr_z: " << mr_z << endl; 
 // unsigned int Dx[5][5], z=0, 
  unsigned int Dx_i;
  unsigned int Dy_i;
  unsigned int Dz_i;
  
 // int i = 0;
  //for(int i=0;i<111;i++){//first 5*5

  unsigned long pu_ori_zone = 0, pv_ori_zone = 0; 
   //pc_array_zonei.swap(vector<pointcoordinate>());
  while(pu_ori_zone + 5 < image.cols) {
	while(pv_ori_zone + 5 < image.rows) {
      vector<pointcoordinate> pc_array_zonei;	//Five coordinates for each point
      for(int i_pc = 0; i_pc < pc_array.size(); i_pc++){
      double pu = pc_array[i_pc].u_px; 
      double pv = pc_array[i_pc].v_px;
      double dx = pc_array[i_pc].x_3d;
      double dy = pc_array[i_pc].y_3d;
      double dz = pc_array[i_pc].z_3d;
      if((pu >= pu_ori_zone)&&(pu < 5 + pu_ori_zone)&&(pv >= pv_ori_zone)&&(pv < 5 + pv_ori_zone)){
          //z++;
        //restore pu and pv in 5*5
        pointcoordinate thispoint_zone;
        /*thispoint_zone.u_px = pu;
        thispoint_zone.v_px = pv;
        thispoint_zone.x_3d = dx;
        thispoint_zone.y_3d = dy;
        thispoint_zone.z_3d = dz;//3D coordinates*/
        thispoint_zone = pc_array[i_pc];
        thispoint_zone.print();
        pc_array_zonei.push_back(thispoint_zone); 
        cout << "test" << endl; 
	    }	
	  }
      
	  //upsampling process: 
	  for(int v = pv_ori_zone; v < 5 + pv_ori_zone; v++){
        for(int u = pu_ori_zone; u < 5 + pu_ori_zone; u++){
		
            S_x=0;Y_x=0;
            S_y=0;Y_y=0;
            S_z=0;Y_z=0;
          for(int j=0;j< pc_array_zonei.size();j++){
            double pu = pc_array_zonei[j].u_px;
            double pv = pc_array_zonei[j].v_px;
            double dx = pc_array_zonei[j].x_3d;
            double dy = pc_array_zonei[j].y_3d;
            double dz = pc_array_zonei[j].z_3d;
            //pu and pv in 5*5
            Gr_x = dx/mr_x;
            Gr_y = dy/mr_y;
            Gr_z = dz/mr_z;
            Gs =  ( (u - pu)*(u - pu) + (v-pv)*(v-pv) );//ij=uv
            Wp_x = 1/sqrt(Gs*fabs(Gr_x));
            Wp_y = 1/sqrt(Gs*fabs(Gr_y));
            Wp_z = 1/sqrt(Gs*fabs(Gr_z));/// dz has negative value.
            S_x = S_x + Wp_x;
            S_y = S_y + Wp_y;
            S_z = S_z + Wp_z;
            Y_x = Y_x + Wp_x*(dx);
            Y_y = Y_y + Wp_y*(dy);
            Y_z = Y_z + Wp_z*(dz);
             
          }
      
          if (S_x==0) {S_x=1;}
          if (S_y==0) {S_y=1;}
          if (S_z==0) {S_z=1;}

          Dx_i = Y_x/S_x;
          Dy_i = Y_y/S_y;
          Dz_i = Y_z/S_z;
        
          unsigned char *row_ptr = image_upsample.ptr<unsigned char>(v);  // row_ptr is the pointer pointing to row u
          unsigned char *data_ptr = &row_ptr[u * image_upsample.channels()]; // data_ptr 指向待访问的像素数据
          data_ptr[0] = Dx_i; // data为I(x,y)第c个通道的值
          data_ptr[1] = Dy_i; // data为I(x,y)第c个通道的值
          data_ptr[2] = Dz_i; // data为I(x,y)第c个通道的值
          // cout << "x_row: " << v << ", x_column: " << u << ", x_value: " << (unsigned int) data_ptr[0] << endl;
          // cout << "y_row: " << v << ", y_column: " << u << ", y_value: " << (unsigned int) data_ptr[1] << endl;
          cout << "z_row: " << v << ", z_column: " << u << ", z_value: " << (unsigned int) data_ptr[2] << endl;
          
          

         // cout << "current data vaule: " << Dx[v][u] << endl;
		}
	  }
	  pv_ori_zone = pv_ori_zone + 5;
	  if(pv_ori_zone + 5 >= image.rows) {
		  pv_ori_zone = 0;
		  break;
	  }
	}
	pu_ori_zone = pu_ori_zone + 5;
  }
  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast < chrono::duration < double >> (t2 - t1);
  cout << "Total time in upsampling: " << time_used.count() << " s." << endl;
  
  cv::imshow("image_upsample", image_upsample);
  cv::waitKey(0);
  cv::Mat channel[3];
  cv::split(image_upsample, channel);//split into three channels

  cv::imshow("x of image_upsample", channel[0]);
  cv::imwrite("upsamplesave_0.png", channel[0]); //save the image 
  cv::imshow("y of image_upsample", channel[1]);
  cv::imwrite("upsamplesave_1.png", channel[1]); //save the image 
  cv::imshow("z of image_upsample", channel[2]);
  cv::imwrite("upsamplesave_2.png", channel[2]); //save the image 

  cv::waitKey(0);
  

  /*// 关于 cv::Mat 的拷贝
  // 直接赋值并不会拷贝数据
  cv::Mat image_another = image;
  // 修改 image_another 会导致 image 发生变化
  //image_another(cv::Rect(0, 0, 100, 100)).setTo(0); // 将左上角100*100的块置零
  cv::imshow("image_another", image_another);
  cv::waitKey(0);

  // 使用clone函数来拷贝数据
  cv::Mat image_clone = image.clone();
  image_clone(cv::Rect(0, 0, 100, 100)).setTo(255);
  cv::imshow("image", image);
  cv::imshow("image_clone", image_clone);
  cv::waitKey(0);*/

  // 对于图像还有很多基本的操作,如剪切,旋转,缩放等,限于篇幅就不一一介绍了,请参看OpenCV官方文档查询每个函数的调用方法.
  cv::destroyAllWindows();
  return 0;
}

