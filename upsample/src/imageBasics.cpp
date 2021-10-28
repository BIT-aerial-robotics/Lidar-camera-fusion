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
#include <algorithm>
#include <vector>
#include <fstream>
#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Eigen/Dense>

#include <opencv2/opencv.hpp>
#include "upsample.h"
#include "imageBasics.h"

using namespace std;

int main(int argc, char **argv) {
  double Wp_x, Wp_y, Wp_z, Gr_x, Gr_y, Gr_z, Gs, S_x=0, S_y=0, S_z=0, Y_x=0, Y_y=0, Y_z=0;
  double mr_x=0,mr_y=0, mr_z=0;
  
  cv::Mat image;
  cv::Mat image_upsample_grey;
	
  for(int v=0;v<2;v++){
    ifstream infile;
  char filename[50];
  sprintf(filename, "./matlab 828/%010d.pngvelo_data", v);
  infile.open(filename, ios::in);
  cout << "filename1:" << filename << endl;

  if (!infile.is_open())
  {
  	cout << "read file1 failed" << endl;
  }

  //input as string.
  string buf;
  unsigned long line_no = 0;	
  vector<pointcoordinate> pc_array;	

  double c_ori_min[3];
  
  while (getline(infile,buf)) 
  {
  	int index = buf.find(",");
  	int index2 = buf.find(",", index + 1);
  	int index3 = buf.find(",", index2 + 1);
  	//cout<< index << "2." << index2 << "3, " << index3 <<  endl; 
  
  	string s1 = buf.substr(0, index);
  	string s2 = buf.substr(index+1, index2-index-1);
  	string s3 = buf.substr(index2+1, index3-index2-1);
  	string s4 = buf.substr(index3+1, buf.length()-index3-1);
  	//cout << "s1:" << s1 << ", s2:" << s2 << ", s3: " << s3 << ", s4: " << s4 << endl;

  	pointcoordinate thispoint;
  	thispoint.x_3d = stof(s1, 0);
  	thispoint.y_3d = stof(s2, 0);
  	thispoint.z_3d = stof(s3, 0);
  	thispoint.print();
    pc_array.push_back(thispoint); 

  if  (thispoint.x_3d < c_ori_min[0]) { c_ori_min[0] = thispoint.x_3d ; }
  if  (thispoint.y_3d < c_ori_min[1]) { c_ori_min[1] = thispoint.y_3d ; }
  if  (thispoint.z_3d < c_ori_min[2]) { c_ori_min[2] = thispoint.z_3d ; }

  if  (thispoint.x_3d> mr_x) { mr_x = thispoint.x_3d; }
  if  (thispoint.y_3d> mr_y) { mr_y = thispoint.y_3d; }
  if  (thispoint.z_3d> mr_z) { mr_z = thispoint.z_3d; }

  	line_no++;
  }
  //cout << "line number: " << line_no << endl; 
  infile.close(); 
  //close file stream 

  cout << "c_ori_min, x:  " << c_ori_min[0] << endl; 
  cout << "c_ori_min, y:  " << c_ori_min[1] << endl; 
  cout << "c_ori_min, z:  " << c_ori_min[2] << endl; 


  ifstream infile2;
  char filename2[50];
  sprintf(filename2, "./matlab 828/%010d.pngvelo_img_data", v);
  infile2.open(filename2, ios::in);
  cout << "filename2:" << filename2 << endl;
  
  if (!infile2.is_open())
  {
  	cout << "read file2 failed" << endl;
  }

  string buf2;
  unsigned long line_no_2 = 0;	
  
  while (getline(infile2,buf2))
  {
  	int index = buf2.find(",");
  	string s1 = buf2.substr(0, index);
  	string s2 = buf2.substr(index+1, buf2.length()-index-1);
  	//cout << "s1:" << s1 << ", s2:" << s2 << ", s3: " << s3 << ", s4: " << s4 << endl;
    pc_array[line_no_2].u_px = stof(s1,0); 
    pc_array[line_no_2].v_px = stof(s2,0); 
    //pc_array[line_no_2].print();
  	line_no_2++;
  }
  infile2.close();   

  sort(pc_array.begin(),pc_array.end(),compare_pc_v);

  int minrow = 0;
  minrow = static_cast<int> (pc_array[0].v_px);  //the minimum v coordinate of the points
  cout << "minrow:" << minrow << endl;
  cout << "pc_array[0].v_px:" << pc_array[0].v_px << endl;

  char filename_ima[50];
  sprintf(filename_ima, "./%03d.png", v);
  image = cv::imread(filename_ima);

  //image = cv::imread(argv[1]); 
  Eigen::Vector2d uu;
  uu << 0,1;
 
  if (image.data == nullptr) { 
    cerr << "File" << argv[1] << "does not exist. " << endl;
    return 0;
  }
  argv++;
  
  cout << "width:" << image.cols << ",height: " << image.rows << ",channels:" << image.channels() << endl;
  cv::imshow("image", image);      
  cv::waitKey(0);                  

 
  if (image.type() != CV_8UC1 && image.type() != CV_8UC3) {
    
    cout << "Please enter a color image or grayscale image." << endl;
    return 0;
  }

//Traverse the image, please note that the following traversal methods can also be used for random pixel access
  // Use std::chrono to time the algorithm
  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

  cv::Mat image_upsample  = image.clone();//clone original image used for upsampling
  double* ima3d = (double*)malloc(sizeof(double)*(image.rows*image.cols*3));

  double maxima3d[3] = {0,0,0};

  double Dx_i;
  double Dy_i;
  double Dz_i;

  int kin = 0;
  int grid = 5;
  int sd = pc_array.size();

  for (int v=0; v<image.rows - minrow; v=v+1)
  {
   for (int u=0; u<image.cols; u=u+1)
   {
	   S_x=0; Y_x=0;
	   S_y=0; Y_y=0;
	   S_z=0; Y_z=0;

       for (int k=kin; k<sd; k=k+1)
       {
              if( pc_array[k].v_px <= v + minrow -grid) { kin=k; }
              if( pc_array[k].v_px >= v + minrow +grid) { break; }

          if ( pc_array[k].u_px > u-grid && pc_array[k].u_px < u+grid && pc_array[k].v_px > v+ minrow -grid && pc_array[k].v_px < v+ minrow +grid )
          {
              double pu = pc_array[k].u_px;
              double pv = pc_array[k].v_px;
              double dx = pc_array[k].x_3d;
              double dy = pc_array[k].y_3d;
              double dz = pc_array[k].z_3d;
              Gr_x = dx/mr_x;
              Gr_y = dy/mr_y;
              Gr_z = dz/mr_z;
              Gs =  ( (u - pu)*(u - pu) + (v-pv)*(v-pv) );
              Wp_x = 1/sqrt(Gs*Gr_x);
              Wp_y = 1/sqrt(Gs*Gr_y);
              Wp_z = 1/sqrt(Gs*Gr_z);
              S_x = S_x + Wp_x;
              S_y = S_y + Wp_y;
              S_z = S_z + Wp_z;
              Y_x = Y_x + Wp_x*dx;
              Y_y = Y_y + Wp_y*dy;
              Y_z = Y_z + Wp_z*dz;

//              Gr = x[k+2*sd]/mr;
//              //Gs =  sqrt( (u - x[k])*(u - x[k]) + (v+dim[0]-x[k+sd])*(v+dim[0]-x[k+sd]) );
//              Gs =  ( (u - x[k])*(u - x[k]) + (v+dim[0]-x[k+sd])*(v+dim[0]-x[k+sd]) );
//              WGain = 1/sqrt(Gs*Gr);
//              //mexPrintf("Filter Gain = %f\n",WGain);
//              S = S + WGain;
//              Y = Y + WGain*(x[k+2*sd]);
          }
       }
//      if (S==0) {S=1;}
//      y[u*(int)dim[1]  + v] = Y/S;

      if (S_x==0) {S_x=1;}
      if (S_y==0) {S_y=1;}
      if (S_z==0) {S_z=1;}

      Dx_i = Y_x/S_x;
      Dy_i = Y_y/S_y;
      Dz_i = Y_z/S_z;

      if(maxima3d[0] < Dx_i ) (maxima3d[0] = Dx_i) ;
      if(maxima3d[1] < Dy_i ) (maxima3d[1] = Dy_i) ;
      if(maxima3d[2] < Dz_i ) (maxima3d[2] = Dz_i) ;

      ima3d[(v+minrow)*image.cols*3 + u*3] = Dx_i;
	  ima3d[(v+minrow)*image.cols*3 + u*3 +1] = Dy_i;
	  ima3d[(v+minrow)*image.cols*3 + u*3 +2] = Dz_i;

   }
  }


  unsigned long pu_ori_zone = 0, pv_ori_zone = 0; 
   //pc_array_zonei.swap(vector<pointcoordinate>());
  /*while(pu_ori_zone + 5 < image.cols) {
	while(pv_ori_zone + 5 < image.rows) {
      vector<pointcoordinate> pc_array_zonei;	
      for(int i_pc = 0; i_pc < pc_array.size(); i_pc++){
      double pu = pc_array[i_pc].u_px; 
      double pv = pc_array[i_pc].v_px;
      double dx = pc_array[i_pc].x_3d;
      double dy = pc_array[i_pc].y_3d;
      double dz = pc_array[i_pc].z_3d;

      if((pu >= pu_ori_zone)&&(pu < 5 + pu_ori_zone)&&(pv >= pv_ori_zone)&&(pv < 5 + pv_ori_zone)){
        pointcoordinate thispoint_zone;

        thispoint_zone = pc_array[i_pc];
//        thispoint_zone.x_3d = (pc_array[i_pc].x_3d)*10;
//        thispoint_zone.y_3d = (pc_array[i_pc].y_3d - c_ori_min[1])*5;
//        thispoint_zone.z_3d = (pc_array[i_pc].z_3d - c_ori_min[2])*50;

        thispoint_zone.print();
        pc_array_zonei.push_back(thispoint_zone); 
	    }	
	  }

  //find the max-value in 5*5.
//    for (int k=0; k<pc_array_zonei.size(); k=k+1){
//    double dx = pc_array_zonei[k].x_3d;
//    double dy = pc_array_zonei[k].y_3d;
//    double dz = pc_array_zonei[k].z_3d;
//
//  if  (fabs(dx) > mr_x) { mr_x = fabs(dx); }
//  if  (fabs(dy)> mr_y) { mr_y = fabs(dy); }
//  if  (fabs(dz) > mr_z) { mr_z = fabs(dz); }
//  }
      
	  //upsampling process: 
	  for(int v = pv_ori_zone; v < (5 + pv_ori_zone); v++){
        for(int u = pu_ori_zone; u < (5 + pu_ori_zone); u++){
		
            S_x=0;Y_x=0;
            S_y=0;Y_y=0;
            S_z=0;Y_z=0;
          for(int j=0;j< pc_array_zonei.size();j++){
            double pu = pc_array_zonei[j].u_px;
            double pv = pc_array_zonei[j].v_px;
            double dx = pc_array_zonei[j].x_3d;
            double dy = pc_array_zonei[j].y_3d;
            double dz = pc_array_zonei[j].z_3d;
            Gr_x = dx/mr_x;
            Gr_y = dy/mr_y;
            Gr_z = dz/mr_z;
            Gs =  ( (u - pu)*(u - pu) + (v-pv)*(v-pv) );
            Wp_x = 1/sqrt(Gs*Gr_x);
            Wp_y = 1/sqrt(Gs*Gr_y);
            Wp_z = 1/sqrt(Gs*Gr_z);
            S_x = S_x + Wp_x;
            S_y = S_y + Wp_y;
            S_z = S_z + Wp_z;
            Y_x = Y_x + Wp_x*dx;
            Y_y = Y_y + Wp_y*dy;
            Y_z = Y_z + Wp_z*dz;
            
          }
      
          if (S_x==0) {S_x=1;}
          if (S_y==0) {S_y=1;}
          if (S_z==0) {S_z=1;}

          Dx_i = Y_x/S_x;
          Dy_i = Y_y/S_y;
          Dz_i = Y_z/S_z;
        
//          unsigned char *row_ptr = image_upsample.ptr<unsigned char>(v);  // row_ptr is the pointer pointing to row v
//          unsigned char *data_ptr = &row_ptr[u * image_upsample.channels()]; // data_ptr points to the pixel data to be accessed
//          data_ptr[0] = Dx_i;
//          data_ptr[1] = Dy_i;
//          data_ptr[2] = Dz_i;
          
          if(maxima3d[0] < Dx_i ) (maxima3d[0] = Dx_i) ;
          if(maxima3d[1] < Dy_i ) (maxima3d[1] = Dy_i) ;
          if(maxima3d[2] < Dz_i ) (maxima3d[2] = Dz_i) ;

//	      ima3d[u][v][0] = Dx_i;
//          ima3d[u][v][1] = Dy_i;
//          ima3d[u][v][2] = Dz_i;
          ima3d[v*image.cols*3 + u*3] = Dx_i;
		  ima3d[v*image.cols*3 + u*3 +1] = Dy_i;
		  ima3d[v*image.cols*3 + u*3 +2] = Dz_i;

		  cout << "test line i" <<  endl;

		}
	  }
	  pv_ori_zone = pv_ori_zone + 1;
	  if(pv_ori_zone + 5 >= image.rows) {
		  pv_ori_zone = 0;
		  break;
	  }
	}
	pu_ori_zone = pu_ori_zone + 1;
  }
  */

  cout << "test line" <<  endl;

  for(int vali = 0; vali < image.rows; vali++)
	  for(int uali = 0; uali < image.cols; uali++){
          unsigned char *row_ptr = image_upsample.ptr<unsigned char>(vali);  // row_ptr is the pointer pointing to row vali
          unsigned char *data_ptr = &row_ptr[uali * image_upsample.channels()]; // data_ptr points to the pixel data to be accessed
          data_ptr[0] = static_cast<unsigned char>(255*ima3d[vali*image.cols*3 + uali*3]/maxima3d[0]);
          data_ptr[1] = static_cast<unsigned char>(255*ima3d[vali*image.cols*3 + uali*3 + 1]/maxima3d[1]);
          data_ptr[2] = static_cast<unsigned char>(255*ima3d[vali*image.cols*3 + uali*3 + 2]/maxima3d[2]);
	  }

  free(ima3d);

//	double minv, maxv;
//	cv::Point pt_min, pt_max;
//	cv::minMaxLoc(image_upsample, &minv, &maxv);
//	cout << "minv = " << minv << endl;
//	cout << "idx_min = " << pt_min << endl;
//	cout << "maxv = " << maxv << endl;
//	cout << "idx_max = " << pt_max << endl;

  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast < chrono::duration < double >> (t2 - t1);
  cout << "Total time in upsampling: " << time_used.count() << " s." << endl;

  char pic0[50];
  sprintf(pic0, "./savepic/%02dimage_upsample.png",v);
  cv::cvtColor(image_upsample, image_upsample_grey, cv::COLOR_BGR2GRAY);
  cv::imshow("image_upsample", image_upsample_grey);
  cv::imwrite(pic0, image_upsample_grey); //save the image 
  cv::waitKey(0);
  cv::Mat channel[3];
  cv::split(image_upsample, channel);//split into three channels

 char pic1[50];
 char pic2[50];
 char pic3[50];
 sprintf(pic1, "./matlab 828/%02dupsamplesave_0.png",v);
 sprintf(pic2, "./matlab 828/%02dupsamplesave_1.png",v);
 sprintf(pic3, "./matlab 828/%02dupsamplesave_2.png",v);
 cv::imshow("x of image_upsample", channel[0]);
 cv::imwrite(pic1, channel[0]); //save the image 
 cv::imshow("y of image_upsample", channel[1]);
 cv::imwrite(pic2, channel[1]); //save the image 
 cv::imshow("z of image_upsample", channel[2]);
 cv::imwrite(pic3, channel[2]); //save the image 
  
  
}
  cv::waitKey(0);
cv::destroyAllWindows();
  return 0;
}
