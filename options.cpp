#include "options.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Options::Options()
{
    detectorName = "SURF";
    descriptorName = "SURF";
    matcherName = "BruteForce";
    verbose = 1;
    queryImagePath = "lena.jpg";
	trainImagePath = "lena.jpg";
    iterCount = -1;
    knn = 2;
	mode = 0;
	FilterType f;
	f.filterName = "RatioCheck";
	f.param = 0.8;
	filterList.push_back(f);
}
    
void Options::help()
{
    cout << "-det - set detector type \n"
	    <<"-desc - set descriptor type \n"
	    <<"-matcher - set matches type \n"
	    <<"-verbose - visible/invisible interface \n"
	    <<"-image - set query image \n"
	    <<"-filter - set filter type \n"
	    <<"-knn - set size for knn match \n"
	    <<"-iter - set iteration count for invisible mode \n"
		<<"-conf - load configure file \n"
	    <<"-help - help" << endl;
}

int Options::parse(int argc, char* argv[])
{
	if (argc == 1)
	{
		help();
		return -1;
	}
	int i = 1;
	while (i < argc)
	{
		if(strcmp(argv[i], "-det") == 0)
		{
			detectorName = argv[++i];
		}
		else if(strcmp(argv[i], "-desc") == 0)
		{
			descriptorName = argv[++i];
		}
		else if(strcmp(argv[i], "-matcher") == 0)
		{
            matcherName = argv[++i];
		}
		else if(strcmp(argv[i], "-verbose") == 0)
		{
			verbose = atoi(argv[++i]);
		}
		else if(strcmp(argv[i], "-image") == 0)
		{
			queryImagePath = argv[++i];
		}
		else if(strcmp(argv[i], "-filter") == 0)
		{
			filterList[0].filterName = argv[++i];
		}
		else if(strcmp(argv[i], "-knn") == 0)
		{
            knn = atoi(argv[++i]);
		}
		else if(strcmp(argv[i],"-iter") == 0)
		{
            iterCount = atoi(argv[++i]);
		}
		else if(strcmp(argv[i], "-dst") == 0)
		{
			filterList[0].param = atof(argv[++i]);
		}
		else if(strcmp(argv[i],"-rat") == 0)
		{
			filterList[0].param = atof(argv[++i]);
		}
		else if(strcmp(argv[i],"-conf") == 0)
		{
			LoadConfig(argv[++i]);
			return 0;
		}
		else if(strcmp(argv[i],"-help") == 0)
		{
			help();
			return -1;
		}
        else 
        {
            cout << "Unknown argument: " << argv[i] << endl;
            return 1;
        }
		i++;
	}
	
    if ( verbose == 1 )
    {
        iterCount = -1;
    }
	return 0;
}

int Options::LoadConfig(string fileName)
{
	FileStorage fConfig;
	fConfig.open(fileName, 0);
	fConfig["detector"] >> detectorName;
	fConfig["descriptor"] >> descriptorName;
	fConfig["matcher"] >> matcherName;
	fConfig["visible"] >> verbose;
	fConfig["mode"] >> mode;
	fConfig["knn"] >> knn;
	fConfig["iterCount"] >> iterCount;
	if(!mode)
		fConfig["queryImage"] >> queryImagePath;
	else
	{
		fConfig["queryImage"] >> queryImagePath;
		fConfig["trainImage"] >> trainImagePath;
		FileStorage mConfig;
		mConfig.open(fConfig["homography"], 0);
		mConfig[fConfig["homography_name"]] >> perspectiveTransformMatrix;
	}
	FileNode fn = fConfig["filter"];
	filterList.clear();
	for(int i = 0; i < fn.size(); i++)
	{
		FilterType f;
		f.filterName = string(fn[i]["filterName"]);
		f.param = double(fn[i]["param"]);
		filterList.push_back(f);
	}
	return 0;
}

int Options::SaveConfig(string fileName)
{
	FileStorage fConfig;
	fConfig.open(fileName, 1);
	fConfig << "detector" << detectorName;
	fConfig << "descriptor" << descriptorName;
	fConfig << "matcher" << matcherName;
	fConfig << "visible" << verbose;
	fConfig << "mode" << mode;
	fConfig << "queryImage" << queryImagePath;
	fConfig << "trainImage" << trainImagePath;
	fConfig << "filterName" << filterList[0].filterName;
	fConfig << "knn" << knn;
	fConfig << "iterCount" << iterCount;
	fConfig << "param" << filterList[0].param;

	return 0;
}