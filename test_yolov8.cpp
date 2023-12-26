/*******************************************************************************
 * test_yolov8.c
 *
 * History:
 *  2023/10/02  - [Alister Hsu] created
 *
 ******************************************************************************/
// #include "yolov8_utils/yolov8_class.h"
// #include "yolov8_utils/object.hpp"
// #include "yolov8_utils/point.hpp"
// #include "yolov8_utils/bounding_box.hpp"
#include "yolov8_utils/vision_tracker.hpp"
using namespace std;
#include "time.h"

std::vector<std::string> getInputFileList(const char* filePath)
{
	// Read lines from the input lists file
	// and store the paths to inputs in strings
	std::ifstream inputList(filePath);
	std::string fileLine;
	std::vector<std::string> lines;
	while (std::getline(inputList, fileLine))
	{
		if (fileLine.empty()) continue;
		lines.push_back(fileLine);
	}
	return lines;
}


void showTrackedHumanResults(std::vector<TrackedObj>& objList)
{	cout<<"In [test_yolov8.cpp] [showTrackedHumanResults]"<<endl;
	cout<<"objList.size()"<<objList.size()<<endl;
	for (int i=0; i<objList.size(); i++)
  {
    TrackedObj& obj = objList[i];

	  cout << "Obj[" << obj.id << "] ";
    cout << "Type: " << obj.type << " ";
    cout << "Conf: " << obj.confidence << " ";
    cout << "Loc: (" << obj.pLoc.x << " m, " << obj.pLoc.y << " m, " << obj.pLoc.z << " m)" << endl;
  }
  cout<<"Out [test_yolov8.cpp] [showTrackedHumanResults]"<<endl;
}

bool fileExists(const std::string& path) {
    FILE *fp;
    if (fp = fopen(path.c_str(), "r")) {
        fclose(fp);
        return true;
    }
    return false;
}

int main(int argc, char **argv)
{

	// bool img1 = fileExists("/ali/yolov8/in/newyork_walk_2230.jpg");
	// bool img2 = fileExists("/ali/yolov8/in/newyork_walk_2240.jpg");
	// bool img3 = fileExists("/ali/yolov8/in/newyork_walk_2250.jpg");
	// bool img4 = fileExists("/ali/yolov8/in/newyork_walk_2260.jpg");
	// bool img5 = fileExists("/ali/yolov8/in/newyork_walk_2270.jpg");
	// bool img6 = fileExists("/ali/yolov8/in/newyork_walk_2280.jpg");

	// cout<<"img1 : "<<img1<<endl;
	// cout<<"img2 : "<<img2<<endl;
	// cout<<"img3 : "<<img3<<endl;
	// cout<<"img4 : "<<img4<<endl;
	// cout<<"img5 : "<<img5<<endl;
	// cout<<"img6 : "<<img6<<endl;
	clock_t start,end;
	double infer_time;


	// ============================================ //
	//                  Entry Point                 //
	// ============================================ //
	int idxFrame = 0;
	// cv::Mat img;
	int sig_flag = 0;
	VisionTracker vTracker("/ali/yolov8/config/config.txt",argc,argv);
	VisionTrackingResults result;
	
	std::cout << "Start WNC Vision Tracking" << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;
	


	while(1)
	{	
		start = clock();
		idxFrame += 1;

		// Run Object Tracking (For AMBA, input img is from AMBA tensor)
		vTracker.run();
		// Get Tracked Results
		vTracker.getResults(result);

		end = clock();

		infer_time = ((double) (end-start)) / CLOCKS_PER_SEC;
		
		cout<<"----------------------infer time :"<<infer_time <<"------------------------------------"<<endl;

		// Show Tracked Results
		if (vTracker.isFinishDetection())
		{
			std::cout << "\nFrame: [" << idxFrame << "]" << std::endl;
			std::cout << "-------------------------------------------------" << std::endl;
			showTrackedHumanResults(result.humanObjList);
		}
	};

	std::cout << "-------------------------------------------------" << std::endl;
	std::cout << "Stop WNC Vision Tracking" << std::endl;
	
}
