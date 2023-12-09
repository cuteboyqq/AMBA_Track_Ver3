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
{
	for (int i=0; i<objList.size(); i++)
  {
    TrackedObj& obj = objList[i];

	  cout << "Obj[" << obj.id << "] ";
    cout << "Type: " << obj.type << " ";
    cout << "Conf: " << obj.confidence << " ";
    cout << "Loc: (" << obj.pLoc.x << " m, " << obj.pLoc.y << " m, " << obj.pLoc.z << " m)" << endl;
  }
}


int main(int argc, char **argv)
{
	// ============================================ //
	//                  Entry Point                 //
	// ============================================ //
	int idxFrame = 0;
	// cv::Mat img;
	int sig_flag = 0;
	VisionTracker vTracker("./config/config.txt",argc,argv);
	VisionTrackingResults result;
	
	std::cout << "Start WNC Vision Tracking" << std::endl;
	std::cout << "-------------------------------------------------" << std::endl;
	
	while(1)
	{	
		idxFrame += 1;

		// Run Object Tracking (For AMBA, input img is from AMBA tensor)
		vTracker.run();

		// Get Tracked Results
		vTracker.getResults(result);

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
