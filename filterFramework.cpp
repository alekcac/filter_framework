#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <math.h>
#include <iostream>
#include "filterFramework.h"
#include "Options.h"
#include <iomanip>

using namespace cv;
using namespace std;

FilterFramework::FilterFramework(const Options& opts) : opts(opts)
{
	if(opts.mode)
	{
		perspectiveTransformMatrix = opts.perspectiveTransformMatrix;
	}
    rng = theRNG();
}

int FilterFramework::init()
{
    queryImage = imread(opts.queryImagePath);
	if(opts.mode) 
		trainImage = imread(opts.trainImagePath);
    if (queryImage.empty())
    {
        cout << "Can't read query image!" << endl;
        return 1;
    }

    try
    {
        descriptorExtractor = DescriptorExtractor::create(opts.descriptorName);
    }
    catch(...)
    {
        cout << "Can't initialize descriptor!" << endl;
        return 1;
    }
    
    try
    {
        detector = FeatureDetector::create(opts.detectorName);
    }
    catch(...)
    {
        cout << "Can't initialize detector!" << endl;
        return 1;
    }

    try
    {
        descriptorMatcher = DescriptorMatcher::create(opts.matcherName);
    }
    catch(...)
    {
        cout << "Can't initialize matcher!" << endl;
        return 1;
    }

    return 0;
}

void FilterFramework::warpPerspectiveRand()
{
	perspectiveTransformMatrix.create(3, 3, CV_32FC1);
	perspectiveTransformMatrix.at<float>(0,0) = rng.uniform( 0.8f, 1.2f);
	perspectiveTransformMatrix.at<float>(0,1) = rng.uniform(-0.1f, 0.1f);
	perspectiveTransformMatrix.at<float>(0,2) = rng.uniform(-0.1f, 0.1f) * queryImage.cols;
	perspectiveTransformMatrix.at<float>(1,0) = rng.uniform(-0.1f, 0.1f);
	perspectiveTransformMatrix.at<float>(1,1) = rng.uniform( 0.8f, 1.2f);
	perspectiveTransformMatrix.at<float>(1,2) = rng.uniform(-0.1f, 0.1f) * queryImage.rows;
	perspectiveTransformMatrix.at<float>(2,0) = rng.uniform( -1e-4f, 1e-4f);
	perspectiveTransformMatrix.at<float>(2,1) = rng.uniform( -1e-4f, 1e-4f);
	perspectiveTransformMatrix.at<float>(2,2) = rng.uniform( 0.8f, 1.2f);
    warpPerspective(queryImage, trainImage, perspectiveTransformMatrix, queryImage.size());
}

int FilterFramework::match()
{
	if(!opts.mode)warpPerspectiveRand();
	detector->detect(queryImage, queryKeypoints);
	descriptorExtractor->compute(queryImage, queryKeypoints, queryDescriptors);

	detector->detect(trainImage, trainKeypoints);
	descriptorExtractor->compute(trainImage, trainKeypoints, trainDescriptors);

	queryToTrainMatches.clear();
    descriptorMatcher->knnMatch(queryDescriptors, trainDescriptors, queryToTrainMatches, opts.knn);
    queryToTrainMatches.resize(queryDescriptors.rows);

    return 0;
}

void FilterFramework::run()
{
    if (opts.iterCount == -1)
	{
        iterate();
		if( (opts.mode == 1) && (opts.verbose == 0) )
		{
			return;
		}
		while(true)
		{
			int c = waitKey(0);
			if (c == 27)
			{
			  return;
			}
			else if (c == 32)
			{  
                iterate();
			}
		}
	}
	else
	{
        for (int i = 0; i < opts.iterCount; i++)
		{
            iterate();
		}
	}
}


double FilterFramework::getRatioGoodMatches(double distance)
{
    double ratioGoodMatches;
    int countGoodMatches = 0;
    vector<Point2f> queryPoints;
    KeyPoint::convert(queryKeypoints, queryPoints);
    Mat transformedPointsMat;
    perspectiveTransform(Mat(queryPoints), transformedPointsMat, perspectiveTransformMatrix);
    vector<Point2f> transformedPoints(transformedPointsMat);
    vector<Point2f> trainPoints;
    KeyPoint::convert(trainKeypoints, trainPoints);


    for (int i = 0; i < queryToTrainMatches.size(); i++)
    {
         if (norm(trainPoints[queryToTrainMatches[i][0].trainIdx] - transformedPoints[queryToTrainMatches[i][0].queryIdx]) <= distance)
         {
             countGoodMatches++;
         }
    }
    ratioGoodMatches = countGoodMatches / (double)queryToTrainMatches.size();

    return ratioGoodMatches;
}

void FilterFramework::filter(const FilterType& f)
{
	if (!f.filterName.compare("KnotCheck"))
	{
		KnotCheck::filter(queryToTrainMatches);
	}
	else if (!f.filterName.compare("RatioCheck"))
	{
		RatioCheck::filter(queryToTrainMatches, f.param);
	}
	else if (!f.filterName.compare("DistanceCheck"))
	{
		DistanceCheck::filter(queryToTrainMatches, f.param);
	}
	else if (!f.filterName.compare("CrossCheck"))
	{
		trainToQueryMatches.clear();
		descriptorMatcher->knnMatch(trainDescriptors,queryDescriptors,trainToQueryMatches,opts.knn);
		NCrossCheck::filter(queryToTrainMatches,trainToQueryMatches);
	}
	else if (!f.filterName.compare("GeometryCheck"))
	{
		GeometryCheck::filter(queryKeypoints, trainKeypoints, queryToTrainMatches);
	}
	else if (!f.filterName.compare("GeometryCheckAngle"))
	{
		GeometryCheckAngle::filter(queryKeypoints, trainKeypoints, queryToTrainMatches, f.param);
	}
    else
    {
        throw "BadFilterName";
    }

}

void FilterFramework::iterate()
{
	if (match() != 0)
        return;

	double gOldMatches = getRatioGoodMatches(1.0);
 	cout << gOldMatches << " ";
	gOldMatches *= (double)queryToTrainMatches.size();

	std::vector<std::vector<cv::DMatch> > matches;
	convertNMatchesToMatches(queryToTrainMatches, matches);

	drawMatches(queryImage, queryKeypoints, trainImage, trainKeypoints, matches, transformIm, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255));

	for(int i = 0; i < opts.filterList.size(); i++)
	{
		filter(opts.filterList[i]);
	}
	matches.clear();
	convertNMatchesToMatches(queryToTrainMatches, matches);

	drawMatches(queryImage, queryKeypoints, trainImage, trainKeypoints, matches, filterIm, CV_RGB(0, 255, 0), CV_RGB(0, 0, 255));

	double gNewMatches = getRatioGoodMatches(1.0);
	cout << gNewMatches << " ";
	cout << (gNewMatches * (double)queryToTrainMatches.size()) / gOldMatches << endl;
	if (opts.verbose)
	{
		imshow("transfromation", transformIm);
		imshow("filtering", filterIm);
	}
}

void FilterFramework::convertNMatchesToMatches(vector<vector<DMatch> > &queryMatches, vector<vector<DMatch> > &trainMatches)
{
	for(int i = 0; i < queryMatches.size(); i++)
	{
		vector<DMatch> tmp;
		tmp.push_back(queryMatches[i][0]);
		trainMatches.push_back(tmp);
	}
}

void FilterFramework::convertMatchesToNMatches(vector<DMatch> &queryMatches, vector<vector<DMatch> > &trainMatches)
{
	for(int i = 0; i < trainMatches.size(); i++)
	{
		vector<DMatch> tmp;
		for(int j = 0; j < opts.knn; j++)
		{
			tmp.push_back(queryMatches[i]);
		}
		trainMatches.push_back(tmp);
	}
}