#ifndef __OPTIONS_H__
#define __OPTIONS_H__
#include <string>
#include <opencv2/core/core.hpp>

#define RANDOM_MODE 0;
#define DATA_SET_MODE 1;

struct FilterType
{
	std::string filterName;
	double param;
};

struct Options
{
    std::string detectorName;
    std::string descriptorName;
    std::string matcherName;
    int verbose;
    std::string queryImagePath;
    std::string trainImagePath;
	std::vector<FilterType> filterList;
    int iterCount;
    int knn;
	cv::Mat perspectiveTransformMatrix;
	int mode;
    Options();    
    void help();
    int parse(int argc, char* argv[]);
	int LoadConfig(std::string fileName);
	int SaveConfig(std::string fileName);
};

#endif __OPTIONS_H__