#include <opencv2/opencv.hpp>
#include "filters.h"

using namespace std;
using namespace cv;

void KnotCheck::filter(vector<vector<DMatch> >& matches)
{
	vector<vector<DMatch> > result_matches;
	map<int, int> train_arr;
	for(size_t i = 0; i < matches.size(); i++)
	{
		train_arr[matches[i][0].trainIdx]++;
	}
	for(size_t i = 0; i < matches.size(); i++)
	{
		if(train_arr[matches[i][0].trainIdx]==1)
		{
			result_matches.push_back(matches[i]);
		}
	}
	matches = result_matches;
}

void DistanceCheck::filter(vector<vector<DMatch> >& matches, double dst)
{
	vector<vector<DMatch> > result_matches;
	for(size_t i = 0; i < matches.size(); i++)
	{
		if(matches[i][0].distance>dst)
		{
			result_matches.push_back(matches[i]);
		}
	}
	matches = result_matches;
}

void RatioCheck::filter(std::vector<vector<DMatch> >& nmatches, double RatioT=0.8)
{
	vector<vector<DMatch> > knmatches;
	for(size_t i = 0; i < nmatches.size(); i++)
	{
		if((nmatches[i].size() == 1)
			||(abs(nmatches[i][0].distance/nmatches[i][1].distance) < RatioT))
		{
			knmatches.push_back(nmatches[i]);
		}
	}
	nmatches = knmatches;
}

void NCrossCheck::filter(vector<vector<DMatch> > &queryToTrainNmatches, vector<vector<DMatch> > &trainToQueryNmatches)
{
	vector<vector<DMatch> > result_matches;
	for(size_t i= 0 ; i < queryToTrainNmatches.size(); i++)
	{
		for(size_t j = 0; j < trainToQueryNmatches[queryToTrainNmatches[i][0].trainIdx].size(); j++)
		{
			if(queryToTrainNmatches[i][0].queryIdx == trainToQueryNmatches[queryToTrainNmatches[i][0].trainIdx][j].trainIdx)
			{	
				result_matches.push_back(queryToTrainNmatches[i]);
				break;
			}
		}
	}
	queryToTrainNmatches=result_matches;
}

void GeometryCheck::filter(vector<KeyPoint> queryKeypoints, vector<KeyPoint> trainKeypoints, vector<vector<DMatch> >& matches)
{
	vector<vector<DMatch> > result_matches;
	for(size_t i = 0; i < matches.size(); i++)
	{
		for(size_t j = 0; j < i; j++)
		{
			if((KeyPoint::overlap(queryKeypoints[matches[i][0].queryIdx],queryKeypoints[matches[j][0].queryIdx])>0.)&&(KeyPoint::overlap(trainKeypoints[matches[i][0].trainIdx],trainKeypoints[matches[j][0].trainIdx])>0.))
			{
					result_matches.push_back(matches[i]);
					break;
			}
		}
	}
	matches=result_matches;
}

void GeometryCheckAngle::filter(vector<KeyPoint> queryKeypoints,vector<KeyPoint> trainKeypoints,vector<vector<DMatch> >& matches,double distance )
{
	vector<vector<DMatch> > result_matches;
	for(size_t i = 0; i < matches.size(); i++)
	{
		for(size_t j = 0; j < i; j++)
		{
			if((abs(queryKeypoints[matches[i][0].queryIdx].angle - queryKeypoints[matches[j][0].queryIdx].angle ) - abs(trainKeypoints[matches[i][0].trainIdx].angle - trainKeypoints[matches[j][0].trainIdx].angle ) < distance))
			{
				result_matches.push_back(matches[i]);
				break;
			}
		}
	}

	matches=result_matches;
}