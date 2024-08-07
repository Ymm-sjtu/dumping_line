// MapPublisher.h

#ifndef MAPPUBLISHER_H
#define MAPPUBLISHER_H

#include <ros/ros.h>
#include <nav_msgs/OccupancyGrid.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <opencv2/opencv.hpp>

class MapPublisher {
public:
    MapPublisher(); // 构造函数，接收JSON文件路径
    void publishMap(); // 发布地图的公共接口
    void publishMapAfterOpenClose(); // 发布开运算、闭运算后的地图的公共接口

private:
    float origin_x, origin_y; // 地图原点
    ros::NodeHandle nh;
    ros::Publisher map_pub;
    ros::Publisher map_pub_after_open_close;
    ros::Publisher map_pub_after_open;
    nav_msgs::OccupancyGrid occupancy_grid;
    nav_msgs::OccupancyGrid occupancy_grid_after_open_close;
    nav_msgs::OccupancyGrid occupancy_grid_after_open;
    cv::Mat cv_gridmap;

    std::string getMapPath(const std::string& filename);
    void processJson(const Json::Value& data); // 处理JSON的私有函数
    void processJsonWithIndex(const Json::Value& data); // 处理有索引的JSON数据
    void processJsonWithoutIndex(const Json::Value& data); // 处理无索引的JSON数据
    void occupancyGridToMat(const nav_msgs::OccupancyGrid& grid); // 将OccupancyGrid转换为cv::Mat
    void MatToOccupancyGrid(const cv::Mat &cv_gridmap,nav_msgs::OccupancyGrid &occupancy_grid); // 将cv::Mat转换为OccupancyGrid
    bool Erode(cv::Mat &cv_gridmap); // 腐蚀
    bool Dilate(cv::Mat &cv_gridmap); // 膨胀
};

#endif
