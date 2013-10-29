#ifndef __FILTER_FRAMEWORK_H__
#define __FILTER_FRAMEWORK_H__

#include "filters.h"
#include "options.h"

class FilterFramework
{
public:
    FilterFramework(const Options& opts);
    virtual ~FilterFramework() {};
    int init();
    void run();

private:
	void convertNMatchesToMatches(std::vector<std::vector<cv::DMatch> > &queryMatches, std::vector<std::vector<cv::DMatch> > &trainMatches);
	void convertMatchesToNMatches(std::vector<cv::DMatch> &queryMatches, std::vector<std::vector<cv::DMatch> > &trainMatches);
	void warpPerspectiveRand();
    int match();
    double getRatioGoodMatches(double distance);
    void filter(const FilterType& f);
    void iterate();

    cv::Mat queryImage, trainImage;
    std::vector<cv::KeyPoint> queryKeypoints, trainKeypoints;
    cv::Mat perspectiveTransformMatrix;
    cv::Mat queryDescriptors, trainDescriptors;
	std::vector<std::vector<cv::DMatch> > queryToTrainMatches, trainToQueryMatches;
    cv::Ptr<cv::FeatureDetector> detector;
    cv::Ptr<cv::DescriptorExtractor> descriptorExtractor;
    cv::Ptr<cv::DescriptorMatcher> descriptorMatcher;
    cv::Mat transformIm, filterIm;
    const Options& opts;
    cv::RNG rng;

	std::vector<std::vector<std::vector<cv::DMatch> > > pattern;
};

#endif