#ifndef __FILTER_H__
#define __FILTER_H__

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

class KnotCheck
{
	public:
		KnotCheck(){};
		static void filter(std::vector<std::vector<cv::DMatch> > &matches);
};

class NCrossCheck
{
	public:
		NCrossCheck(){};
		static void filter(std::vector<std::vector<cv::DMatch> >& queryToTrainNmatches, 
            std::vector<std::vector<cv::DMatch> >& trainToQueryNmatches);
};

class DistanceCheck
{
	public:
		DistanceCheck(){};
        static void filter(std::vector<std::vector<cv::DMatch> >& matches, double distance);
};

class RatioCheck
{
	public:
		RatioCheck(){};
		static void filter(std::vector<std::vector<cv::DMatch> >& matches, double ratioThreshold);
};

class GeometryCheck
{
	public:
		GeometryCheck(){};
        static void filter(std::vector<cv::KeyPoint> queryKeypoints,
            std::vector<cv::KeyPoint> trainKeypoints, std::vector<std::vector<cv::DMatch> >& matches);
};

class GeometryCheckAngle
{
	public:
		GeometryCheckAngle(){};
        static void filter(std::vector<cv::KeyPoint> queryKeypoints,
            std::vector<cv::KeyPoint> trainKeypoints, std::vector<std::vector<cv::DMatch> >& matches, double distance);
};

#endif __FILTER_H__