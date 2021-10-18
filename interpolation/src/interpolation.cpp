/*
 * Copyright (C) 2021 Chuanbeibei Shi, Yushu Yu, Mauro Bellone 
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

#include <opencv2/opencv.hpp>
#include "interpolation.h"

using namespace std;
void interpolation(unsigned int img_count);

int main(int argc, char **argv) {
  
  interpolation(atoi(argv[1]));
  
  return 0;
}

void interpolation(unsigned int img_count){
  cv::Mat image;
  int flag=3;
  //for(int v=0;v<122;v++){	
  for(int v = 0; v < img_count; v++){
    char filename_ima[60];
    // sprintf(filename_ima, "./dataset/pc/%d/%03d.jpg", flag, v);
    sprintf(filename_ima, "./dataset/train/3channel_train_pc/Frame_%d_channelxyz.png", v);
  // image = cv::imread(filename_ima, cv::IMREAD_GRAYSCALE);
  image = cv::imread(filename_ima);
 
  cout << "width:" << image.cols << ",height: " << image.rows << ",channels:" << image.channels() << endl;
  cv::imshow("image", image);      
  // cv::waitKey(0);                  
 
  if (image.type() != CV_8UC1 && image.type() != CV_8UC3) {
    
    cout << "Please enter a color image or grayscale image." << endl;
  }

//Traverse the image, please note that the following traversal methods can also be used for random pixel access
  // Use std::chrono to time the algorithm
  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

  cv::Mat image_interpolation  = image.clone();//clone original image used for upsampling

  unsigned long pu_ori_zone = 0, pv_ori_zone = 0; 
  unsigned blackholeflag = 0; 
  unsigned grid = 2;
  unsigned grid_margin = 50;
  while(pu_ori_zone + grid + grid_margin < image.cols) {
	while(pv_ori_zone + grid + grid_margin < image.rows) {

      blackholeflag = 0; // blackholeflag = 0 indicates it is a blackhole
	  //if all the pixel values are zeros:/
	  for(int v = pv_ori_zone; v < (grid + pv_ori_zone); v++){
        for(int u = pu_ori_zone; u < (grid + pu_ori_zone); u++){
       
          unsigned char *row_ptr = image_interpolation.ptr<unsigned char>(v);  // row_ptr is the pointer pointing to row u
          unsigned char *data_ptr = &row_ptr[u * image_interpolation.channels()]; // data_ptr points to the pixel data to be accessed
          if (data_ptr[0] != 0){
			  blackholeflag = 1;
		      break;
		  }
		}
	  }
      if(blackholeflag == 0){
		  cv::Mat srcROT(image_interpolation, cv::Rect(pu_ori_zone, pv_ori_zone, grid + grid_margin, grid + grid_margin));
		 // cv::Mat srcROT(image_interpolation, cv::Rect(0,0, 50,50));
		  cv::blur(srcROT, srcROT, cv::Size(5,5), cv::Point(-1,-1));
      }
	  blackholeflag = 0;
	  pv_ori_zone = pv_ori_zone + 1;
	  if(pv_ori_zone + grid + grid_margin >= image.rows) {
		  pv_ori_zone = 0;
		  break;
	  }
	}
	pu_ori_zone = pu_ori_zone + 1;
  }

  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast < chrono::duration < double >> (t2 - t1);
  cout << "Total time in interpolation: " << time_used.count() << " s." << endl;

 char pic1[70];
 sprintf(pic1, "./dataset/train/3channel_train_pc_inter/Frame_%d_channelxyz.png", v);
 cv::imshow("interpolation", image_interpolation);
 cv::imwrite(pic1, image_interpolation); //save the image 
  
}
  // cv::waitKey(0);
  cv::destroyAllWindows();
}

