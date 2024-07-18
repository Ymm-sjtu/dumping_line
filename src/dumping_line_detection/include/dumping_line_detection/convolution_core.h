#ifndef _CONVOLUTION_METHOD
#define _CONVOLUTION_METHOD

#include <ros/ros.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SVD>

#include "dumping_line_detection/b_spline.h"

#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>

#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/Path.h>

#include <std_msgs/Int32.h>
#include <std_msgs/Int32MultiArray.h>

#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/ChannelFloat32.h>

#include <tf/tf.h>

#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <fstream>  // 包含文件操作的库
#include <chrono>
#include <thread>

namespace convolution_ns{
#define THREE 3
#define TWO 2

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define RIGHT_UP 4
#define RIGHT_DOWN 5
#define LEFT_DOWN 6
#define LEFT_UP 7
#define OUTSIDE false
#define SELF -2

#define FREE 8
#define RIGHT_EDGE 0
#define LEFT_EDGE 1
#define UP_EDGE 2
#define DOWN_EDGE 3
#define RIGHTUP_EDGE 4
#define RIGHTDOWN_EDGE 5
#define LEFTUP_EDGE 6
#define LEFTDOWN_EDGE 7

/*
    POSITION:
                            UP
                            0
            Left Up 7               4 Right Up
            Left 3                  1 Right
            Left Down 6             5 Right Down
                            2
                            Down
    CORE:
           [ 2 3 ]      [ 6 7 8 ]
           [ 0 1 ]      [ 3 4 5 ]
                        [ 0 1 2 ]
*/

struct gridType{
    int Index;//grid index
    int SN;//sequence number

    gridType(const int _index, const int _SN):Index(_index),SN(_SN){};
};

typedef struct Node
{
    int index;
    float distance;
}DistanceNode;

class convolution_core{
    public:
        //Member
        int scale = 0;//阶数
        std::vector<int> conv;
        const int up_norm = 1;
        const int low_norm = 0;
        const float up_norm_float = 0.0;
        const float low_norm_float = 0.0;

        //PROCESS
        void recore(const int _size, const int _position);
        void conv2x2UP();
        void conv2x2LEFT();
        void conv2x2DOWN();
        void conv2x2RIGHT();
        void conv3x3UP();
        void conv3x3LEFT();
        void conv3x3DOWN();
        void conv3x3RIGHT();
        void conv3x3RIGHT_UP();
        void conv3x3RIGHT_DOWN();
        void conv3x3LEFT_DOWN();
        void conv3x3LEFT_UP();
};

class convolution{
    public:
        void main_loop();
        convolution();

    private:
        //Member
        convolution_core core;

        geometry_msgs::Pose oriented_pose;
        geometry_msgs::Pose recommend_pose;

        //Map
        nav_msgs::OccupancyGrid grid;
        nav_msgs::OccupancyGrid grid_after_conv;//Store the data of grid map after convolution
        nav_msgs::OccupancyGrid grid_after_cut;//Store the data of grid map after cut
        nav_msgs::OccupancyGrid grid_after_found;
        //Store a continuous grid map obtained by using the grid points found in the recommended pose direction as seed points
        bool ifSubGrid = false;

        //订阅车体信息
        geometry_msgs::Point vehicle_position;
        int vehicle_posi_index;
        bool ifSubVehiclePose = false;

        int thres = 80;//栅格阈值
        bool ifGetRcmdPose = false;

        visualization_msgs::Marker arrow;
        visualization_msgs::Marker fitted_points;

        //ROS
        ros::NodeHandle nh;
        ros::Publisher pub_grid_after_conv;
        ros::Publisher pub_fitted_points;
        ros::Publisher pub_marker_b_spline;
        ros::Publisher pub_grid_after_cut;
        ros::Publisher pub_grid_after_found;
        
        ros::Subscriber sub_map;
        ros::Subscriber sub_vehicle_pose;
        ros::Subscriber sub_recommend_pose;

        //Main Process Function
        void calcConvolution2x2();
        void calcConvolution3x3();
        
        //Condition Function
        bool conditionA(const std::vector<int> *);

        //Process Calculation Function
        DistanceNode* QuickSort(DistanceNode *R, int low, int high);
        int convolute(const std::vector<int> *arr);
        void reset();
        bool cutGridMap();
        geometry_msgs::Point getNewPosition(const geometry_msgs::Pose& pose, double distance);
        bool computeRotatedLineEquation(const geometry_msgs::Pose& pose, double rotation_angle, double& k, double& b);
        void getNearbyIndex2x2(const int, std::vector<int> *, const int, const int,const int);
        int getNearbyValue(const int data1, const int threshold);
        int getRelativePosi(const int index, const int dimension);
        int getLeaderInCore(const int index, const int dimension);
        void getNearbyValues(const int ,const std::vector<int> *, std::vector<int> *);
        int getIndex(const geometry_msgs::Point _point);

        void getNeighborIndexes(const int _index, std::vector<int> &_vec, std::vector<bool> &_states, const int _width, const int _height);
        void getNeighborIndexes(const int _index, std::vector<int> &_vec, std::vector<int> &_vecSN, const int _width, const int _height, const int _type);
        int getRelativeLocation(const int _index, const int _width, const int _height);

        bool getOrientedAreaInGrid(const float step_dist);
        bool getMaxAreaInGrid();

        //Callback Function
        void callbackSubGrid(const nav_msgs::OccupancyGrid gridData);
        void callbackSubVehiclePose(const geometry_msgs::PoseStamped vehiclePose);
        void callbackSubRecommendPose(const geometry_msgs::PoseStamped recommendPose);
};

class normalFitSequence{
    public:
        std::vector<geometry_msgs::Pose> poses;
        void savePointsToFile(const visualization_msgs::Marker& fitted_points, const std::string& file_name);
        normalFitSequence(std::vector<geometry_msgs::Point> _points, const geometry_msgs::Point _origin, const int _num);
        ~normalFitSequence(){poses.clear();}
    
    private:
        geometry_msgs::Pose circleFit(const std::vector<geometry_msgs::Point> active_points, const geometry_msgs::Point _origin);
        geometry_msgs::Pose linearFit(const std::vector<geometry_msgs::Point> active_points, const geometry_msgs::Point _origin);

        bool isInAngularInterval(const double _angle, const double _low, const double _up);
};

}//end namespace
#endif