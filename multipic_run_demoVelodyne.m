%function run_demoVelodyne (base_dir,calib_dir)
% KITTI RAW DATA DEVELOPMENT KIT
% 
% Demonstrates projection of the velodyne points into the image plane
%
% Input arguments:
% base_dir .... absolute path to sequence base directory (ends with _sync)
% calib_dir ... absolute path to directory that contains calibration files

% clear and close everything
close all; dbstop error; clc;
disp('======= KITTI DevKit Demo =======');
Output_path = 'D:/kitti/2011_09_28/2011_09_28_drive_0021_sync/image_02/collectdata_test/';
% options (modify this to select your sequence)
%if nargin<1
base_dir  = 'D:/kitti/2011_09_28/2011_09_28_drive_0021_sync';
%end
%if nargin<2
calib_dir = 'D:/kitti/2011_09_28';
%end
cam       = 3; % 0-based index
frame     = 0; % 0-based index

% load calibration
calib = loadCalibrationCamToCam(fullfile(calib_dir,'calib_cam_to_cam.txt'));
Tr_velo_to_cam = loadCalibrationRigid(fullfile(calib_dir,'calib_velo_to_cam.txt'));

% compute projection matrix velodyne->image plane
R_cam_to_rect = eye(4);
R_cam_to_rect(1:3,1:3) = calib.R_rect{1};% % 参考相机0到相机xx图像平面的旋转矩阵
P_velo_to_img = calib.P_rect{cam+1}*R_cam_to_rect*Tr_velo_to_cam;%相机内参，旋转矩阵，点云到相机的外参矩阵

% load and display image
piclist = dir(strcat(base_dir,'/image_02/data/*.png'));
velolist = dir(strcat(base_dir,'/velodyne_points/data/*.bin'));
len = length(piclist);
for i = 1:len
    name=piclist(i).name;  %namelist(i).name; %这里获得的只是该路径下的文件名
    path_pic = sprintf('%s/image_%02d/data/%s',base_dir,cam,name);
    img=imread(path_pic); 
    fig = figure('Position',[20 100 size(img,2) size(img,1)]); axes('Position',[0 0 1 1]);
    filename = strcat(Output_path, name);
    imwrite(img,filename);
    imshow(img); hold on;


    % load velodyne points
    name_velo=velolist(i).name;
    path_velo = sprintf('%s/velodyne_points/data/%s',base_dir, name_velo);
    fid = fopen(path_velo,'rb');
    %fid = fopen(sprintf('\velodyne_points\data\%001d.bin',base_dir, frame),'rb');
    velo = fread(fid,[4 inf],'single')';%spatial 3D position
    velo = velo(1:5:end,:); % remove every 5th point for display speed
    fclose(fid);

    % remove all points behind image plane (approximation
    idx = velo(:,1)<5;
    velo(idx,:) = [];

    % project to image plane (exclude luminance)
    velo_img = project(velo(:,1:3),P_velo_to_img);%pixel poistion 点云在图像上的坐标

    % plot points
    cols = jet;
    for i=1:size(velo_img,1)
      col_idx = round(64*5/velo(i,1));
      plot(velo_img(i,1),velo_img(i,2),'o','LineWidth',4,'MarkerSize',1,'Color',cols(col_idx,:));
      %imwrite(img,filename);
    end
    datafilename1 = strcat(name, 'velo_img_data');
    datafilename2 = strcat(name, 'velo_data');
    dlmwrite(datafilename1, velo_img)
    dlmwrite(datafilename2, velo)
end
