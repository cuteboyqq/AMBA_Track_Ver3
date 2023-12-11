/*
  (C) 2023-2024 Wistron NeWeb Corporation (WNC) - All Rights Reserved

  This software and its associated documentation are the confidential and
  proprietary information of Wistron NeWeb Corporation (WNC) ("Company") and
  may not be copied, modified, distributed, or otherwise disclosed to third
  parties without the express written consent of the Company.

  Unauthorized reproduction, distribution, or disclosure of this software and
  its associated documentation or the information contained herein is a
  violation of applicable laws and may result in severe legal penalties.
*/

#include "config_reader.hpp"


/////////////////////////
// public member functions
////////////////////////
TrackerConfigReader::TrackerConfigReader()
{
  cout<<"[TrackerConfigReader constructer] Start new Config_S"<<endl;
  m_config = new Config_S();
  cout<<"[TrackerConfigReader constructer] End new Config_S"<<endl;

};

TrackerConfigReader::~TrackerConfigReader() {};


Config_S* TrackerConfigReader::getConfig()
{
  cout<<"[getConfig] Start return m_config"<<endl;
  return m_config;
}

// yolov8_det.lua file content
// _nn_arm_nms_config_ = {
// 	conf_threshold = 0.20,    -- Confidence threshold
// 	top_k = 200,             -- Top k on each class
// 	nms_threshold = 0.45,    -- NMS threshold
// 	class_num = 4,          -- class num
// 	enable_seg = 0,          -- enable segmentation
// 	log_level = 2,           -- 0 none, 1 error, 2 notice, 3 debug, 4 verbose
// 	disable_fsync = 0,       -- disable fsync for live mode when it is 1
// 	output_0 = "output0",    -- Output name for the detected objects
// }
// return _nn_arm_nms_config_



bool TrackerConfigReader::read(std::string configPath)
{
  // Create object of the class ConfigReader
  ConfigReader *configReader = ConfigReader::getInstance();

#if defined (SPDLOG)
  auto m_logger = spdlog::get("TrackerConfigReader");
#endif

  if (utils::checkFileExists(configPath) && configReader->parseFile(configPath))
  {
    // Print divider on the console to understand the output properly
#if defined (SPDLOG)
    m_logger->info("=================================================");
#endif
    // Define variables to store the value

    // Platform Runtime
    string runtime = "";      // For QCS6490

    // Alister add 2023-12-10
    // AMBA Lua File Settings
    string conf_threshold ="";
    int top_k = 200;
    string nms_threshold ="";
    int class_num = 4;
    int enable_seg = 0;
    int log_level = 2;
    int disable_fsync = 0;
    std::string output = "output_0";

    // AMBA Init Param Setting
    int mode = 1;
    int draw_mode = 0;
    int input_color_type = 0;
    int yuv_flag = 0;
    int use_pyramid = 0;
    int enable_fsync_flag = 1;
    int queue_size = 1;
    int thread_num = 1;
    int acinf_gpu_id = -1;
    int overlay_buffer_offset = -1;

    // AMBA Command Param Setting
    string multi_images = "";
    string lua_file_path = "";
    string label_path = "";
    string output_dir = "";
    string model = "yolov8";
    // char model_path = "";

    // Model Information
    string modelPath = "";
    int modelWidth = 0;
    int modelHeight = 0;

    // Camera Information
    string cameraHeight = "";
    string cameraFocalLength = "";
    int frameWidth = 0;
    int frameHeight = 0;

    // Processing Time
    string procFrameRate = "";
    int procFrameStep = 4;

    // Object Detection
    string humanConfidence = "";
    string carConfidence = "";
    string bikeConfidence = "";
    string motorbikeConfidence = "";
    string detectionRange = "";
    int maxDetection = 5;
    int removeOverlap = 1;

    // Object Tracking
    string matchingLevel = "";
    int maxTracking = 3;

    // Debug Information
    int debugConfig = 0;
    int debugTracking = 0;
    int debugYolov8 = 0;
    int debugObjectDetection = 0;
    int debugObjectTracking = 0;
    int debugHumanTracker = 0;
    int debugBikeTracker = 0;
    int debugVehicleTracker = 0;
    int debugMotorbikeTracker = 0;
    int debugSaveLogs = 0;
    int debugSaveImages = 0;
    int debugSaveRawImages = 0;
    string debugLogsDirPath = "";
    string debugImagesDirPath = "";
    string debugRawImagesDirPath = "";

    // Display Results
    int displayResults = 0;
    int displayObjectDetection = 0;
    int displayObjectTracking = 0;
    int displayWarningZone = 0;
    int displayInformation = 0;
    int displayMaxFrameIndex = 0;

    // Show Processing Time
    int showProcTimeTracking = 0;
    int showProcTimeYolov8 = 0;
    int showProcTimeObjectTracking = 0;

    // Update the variable by the value present in the configuration file.
    configReader->getValue("Runtime", runtime);

    // yolov8_det.lua file content
// _nn_arm_nms_config_ = {
// 	conf_threshold = 0.20,    -- Confidence threshold
// 	top_k = 200,             -- Top k on each class
// 	nms_threshold = 0.45,    -- NMS threshold
// 	class_num = 4,          -- class num
// 	enable_seg = 0,          -- enable segmentation
// 	log_level = 2,           -- 0 none, 1 error, 2 notice, 3 debug, 4 verbose
// 	disable_fsync = 0,       -- disable fsync for live mode when it is 1
// 	output_0 = "output0",    -- Output name for the detected objects
// }
// return _nn_arm_nms_config_


    //AMBA lua file settings Alister add 2023-12-10
    configReader->getValue("ConfThreshold",conf_threshold);
    configReader->getValue("TopK", top_k);
    configReader->getValue("NMSThreshold", nms_threshold);
    configReader->getValue("ClassNum", class_num);
    configReader->getValue("EnableSeg", enable_seg);
    configReader->getValue("LogLevel", log_level);
    configReader->getValue("DisableFsync", disable_fsync);
    configReader->getValue("Output", output);

    //AMBA init param settings Alister add 2023-12-11
    configReader->getValue("Mode",mode);
    configReader->getValue("DrawMode",draw_mode);
    configReader->getValue("InputColorType",input_color_type);
    configReader->getValue("UsePyramid",use_pyramid);
    configReader->getValue("EnableFsyncFlag",enable_fsync_flag);
    configReader->getValue("QueueSize",queue_size);
    configReader->getValue("ThreadNum",thread_num);
    configReader->getValue("AcinfGpuId",acinf_gpu_id);
    configReader->getValue("OverlayBufferOffset",overlay_buffer_offset);

    //AMBA command param setting Alister add 2023-12-11
    configReader->getValue("MultiImages",multi_images);
    configReader->getValue("LuafilePath",lua_file_path);
    configReader->getValue("LabelPath",label_path);
    configReader->getValue("Model",model);
    configReader->getValue("ClassNum",class_num);
    configReader->getValue("OutputDirPath",output_dir);

    // Model Information
    configReader->getValue("ModelPath", modelPath);
    configReader->getValue("ModelWidth", modelWidth);
    configReader->getValue("ModelHeight", modelHeight);

    // Camera Information
    configReader->getValue("CameraHeight", cameraHeight);
    configReader->getValue("CameraFocalLength", cameraFocalLength);
    configReader->getValue("FrameWidth", frameWidth);
    configReader->getValue("FrameHeight", frameHeight);

    // Processing Time
    configReader->getValue("ProcessingFrameRate", procFrameRate);
    configReader->getValue("ProcessingFrameStep", procFrameStep);

    // Object Detection
    configReader->getValue("MaxDetection", maxDetection);
    configReader->getValue("HumanConfidence", humanConfidence);
    configReader->getValue("CarConfidence", carConfidence);
    configReader->getValue("BikeConfidence", bikeConfidence);
    configReader->getValue("MotorbikeConfidence", motorbikeConfidence);
    configReader->getValue("DetectionRange", detectionRange);
    configReader->getValue("RemoveOverlap", removeOverlap);

    // Object Tracking
    configReader->getValue("MaxTracking", maxTracking);
    configReader->getValue("MatchingLevel", matchingLevel);

    // Debug Information
    configReader->getValue("DebugConfig", debugConfig);
    configReader->getValue("DebugTracking", debugTracking);
    configReader->getValue("DebugYolov8", debugYolov8);
    configReader->getValue("DebugObjectDetection", debugObjectDetection);
    configReader->getValue("DebugObjectTracking", debugObjectTracking);
    configReader->getValue("DebugHumanTracker", debugHumanTracker);
    configReader->getValue("DebugBikeTracker", debugBikeTracker);
    configReader->getValue("DebugVehicleTracker", debugVehicleTracker);
    configReader->getValue("DebugMotorbikeTracker", debugMotorbikeTracker);
    configReader->getValue("DebugSaveLogs", debugSaveLogs);
    configReader->getValue("DebugSaveImages", debugSaveImages);
    configReader->getValue("DebugSaveRawImages", debugSaveRawImages);
    configReader->getValue("DebugLogsDirPath", debugLogsDirPath);
    configReader->getValue("DebugImagesDirPath", debugImagesDirPath);
    configReader->getValue("DebugRawImagesDirPath", debugRawImagesDirPath);

    // Display Results
    configReader->getValue("DisplayResults", displayResults);
    configReader->getValue("DisplayObjectDetection", displayObjectDetection);
    configReader->getValue("DisplayObjectTracking", displayObjectTracking);
    configReader->getValue("DisplayWarningZone", displayWarningZone);
    configReader->getValue("DisplayInformation", displayInformation);
    configReader->getValue("DisplayMaxFrameIndex", displayMaxFrameIndex);

    // Show Processing Time
    configReader->getValue("ShowProcTimeTracking", showProcTimeTracking);
    configReader->getValue("ShowProcTimeYOLOv8", showProcTimeYolov8);
    configReader->getValue("ShowProcTimeObjectTracking", showProcTimeObjectTracking);


    // Pass configuration

    // Platform Runtime
    m_config->runtime = runtime;

    // Model Information
    m_config->modelPath = modelPath;
    m_config->modelWidth = modelWidth;
    m_config->modelHeight = modelHeight;
    
    // AMBA lua file info (Alister add 2023-12-11)
    m_config->AMBALuaConfig.class_num = class_num;
    m_config->AMBALuaConfig.conf_threshold = conf_threshold;
    m_config->AMBALuaConfig.disable_fsync = disable_fsync;
    m_config->AMBALuaConfig.enable_seg = enable_seg;
    m_config->AMBALuaConfig.log_level = log_level;
    m_config->AMBALuaConfig.nms_threshold = nms_threshold;
    m_config->AMBALuaConfig.output_0 = output;
    m_config->AMBALuaConfig.top_k = top_k;

    // AMBA init parameter setting (Alister add 2023-12-11)
    m_config->AMBAInitParamConfig.acinf_gpu_id = acinf_gpu_id;
    m_config->AMBAInitParamConfig.draw_mode = draw_mode;
    m_config->AMBAInitParamConfig.enable_fsync_flag = enable_fsync_flag;
    m_config->AMBAInitParamConfig.input_color_type = input_color_type;
    m_config->AMBAInitParamConfig.mode = mode;
    m_config->AMBAInitParamConfig.overlay_buffer_offset = overlay_buffer_offset;
    m_config->AMBAInitParamConfig.queue_size = queue_size;
    m_config->AMBAInitParamConfig.thread_num = thread_num;
    m_config->AMBAInitParamConfig.use_pyramid = use_pyramid;
    m_config->AMBAInitParamConfig.yuv_flag = yuv_flag;

    //AMBA command parameter setting (Alister add 2023-12-11)
    m_config->AMBACommandParamConfig.label_path = label_path;
    m_config->AMBACommandParamConfig.lua_file_path = lua_file_path;
    m_config->AMBACommandParamConfig.multi_images = multi_images;
    m_config->AMBACommandParamConfig.model = model;
    m_config->AMBACommandParamConfig.output_dir = output_dir;
    m_config->AMBACommandParamConfig.class_num = class_num;

    // Camera Information
    m_config->stCameraConfig.height = std::stof(cameraHeight);
    m_config->stCameraConfig.focalLength = std::stof(cameraFocalLength);
    m_config->frameWidth = frameWidth;
    m_config->frameHeight = frameHeight;

    // Processing Time
    m_config->procFrameRate = std::stof(procFrameRate);
    m_config->procFrameStep = procFrameStep;

    // Object Detection
    m_config->stOdConfig.maxDetection = maxDetection;
    m_config->stOdConfig.detectionRange = detectionRange;
    m_config->stOdConfig.removeOverlap = removeOverlap;
    m_config->stOdConfig.humanConfidence = std::stof(humanConfidence);
    m_config->stOdConfig.bikeConfidence = std::stof(bikeConfidence);
    m_config->stOdConfig.carConfidence = std::stof(carConfidence);
    m_config->stOdConfig.motorbikeConfidence = std::stof(motorbikeConfidence);

    // Object Tracking
    m_config->stTrackerConifg.maxTracking = maxTracking;
    m_config->stTrackerConifg.matchingLevel = matchingLevel;

    // Debug Information
    m_config->stDebugConfig.config = debugConfig;
    m_config->stDebugConfig.tracking = debugTracking;
    m_config->stDebugConfig.yolov8 = debugYolov8;
    m_config->stDebugConfig.objectDetection = debugObjectDetection;
    m_config->stDebugConfig.objectTracking = debugObjectTracking;
    m_config->stDebugConfig.humanTracker = debugHumanTracker;
    m_config->stDebugConfig.bikeTracker = debugBikeTracker;
    m_config->stDebugConfig.vehicleTracker = debugVehicleTracker;
    m_config->stDebugConfig.motorbikeTracker = debugMotorbikeTracker;
    m_config->stDebugConfig.saveLogs = debugSaveLogs;
    m_config->stDebugConfig.saveImages = debugSaveImages;
    m_config->stDebugConfig.saveRawImages = debugSaveRawImages;
    m_config->stDebugConfig.logsDirPath = debugLogsDirPath;
    m_config->stDebugConfig.imgsDirPath = debugImagesDirPath;
    m_config->stDebugConfig.rawImgsDirPath = debugRawImagesDirPath;

    // Display Results
    m_config->stDisplayConfig.results = displayResults;
    m_config->stDisplayConfig.objectDetection = displayObjectDetection;
    m_config->stDisplayConfig.objectTracking = displayObjectTracking;
    m_config->stDisplayConfig.warningZone = displayWarningZone;
    m_config->stDisplayConfig.information = displayInformation;
    m_config->stDisplayConfig.maxFrameIndex = displayMaxFrameIndex;

    // Show Processing Time
    m_config->stShowProcTimeConfig.tracking = showProcTimeTracking;
    m_config->stShowProcTimeConfig.yolov8 = showProcTimeYolov8;
    m_config->stShowProcTimeConfig.objectTracking = showProcTimeObjectTracking;
  }
  else
  {
#if defined (SPDLOG)
    m_logger->error("=================================================");
    m_logger->error("Read Config Failed! ===> Use default configs");
    m_logger->error("=================================================");
#endif
    return false;
  }

  //
  configReader = NULL;

  return true;
}