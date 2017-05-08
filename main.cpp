#include "CMT.h"
#include "gui.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <string>
#include <stdio.h>
#include <vector>  

#ifdef __GNUC__
#include <getopt.h>
#else
#include "getopt/getopt.h"
#endif

using cmt::CMT;
using cv::imread;
using cv::namedWindow;
using cv::Scalar;
using cv::VideoCapture;
using cv::waitKey;
using std::cerr;
using std::istream;
using std::ifstream;
using std::stringstream;
using std::ofstream;
using std::cout;
using std::min_element;
using std::max_element;
using std::endl;
using ::atof;

using namespace cv;
using namespace std;

static string WIN_NAME = "CMT";
static string WIN_NAME2 = "CMT2";

static string OUT_FILE_COL_HEADERS =
    "Frame,Timestamp (ms),Active points,"\
    "Bounding box centre X (px),Bounding box centre Y (px),"\
    "Bounding box width (px),Bounding box height (px),"\
    "Bounding box rotation (degrees),"\
    "Bounding box vertex 1 X (px),Bounding box vertex 1 Y (px),"\
    "Bounding box vertex 2 X (px),Bounding box vertex 2 Y (px),"\
    "Bounding box vertex 3 X (px),Bounding box vertex 3 Y (px),"\
    "Bounding box vertex 4 X (px),Bounding box vertex 4 Y (px)";

 //initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int FRAME_FPS = 10;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20 ;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowName = "Thresholded Image";


//////////////////////////
    //CMT cmt, cmt2;

    int penghitung = 0;
    float awal = 0.00;
    float awal2 = 0.00;

    float per1 = 1.00;
    float per2 = 1.00;
/////////////////////////



void morphOps(Mat &thresh) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(5, 5));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);

}

/**
int color(int argc, char* argv[])
{

	bool trackObjects = true;
	bool useMorphOps = true;

	Mat cameraFeed;

	Mat HSV;

	Mat threshold;

	int x = 0, y = 0,a =0;
    bool ax;

	VideoCapture capture;

	capture.open(0);

	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	while (1) {
	
		capture.read(cameraFeed);
	
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
	
		inRange(HSV, Scalar(123, 77, 0), Scalar(217, 256, 256), threshold);
	
		if (useMorphOps)
			morphOps(threshold);
	
		if (trackObjects){
			ax = trackFilteredObject(x, y, threshold, cameraFeed);
            if (ax){
                a++;
            }
            
        }

		if (a%25 == 0){
            imshow(windowName2, threshold);

        }
	
	
		waitKey(30);
	}

	return 0;
}**/


vector<float> getNextLineAndSplitIntoFloats(istream& str)
{
    vector<float>   		result;
    string                	line;
    getline(str,line);

    stringstream          lineStream(line);
    string                cell;
    while(getline(lineStream,cell,','))
    {
        result.push_back(atof(cell.c_str()));
    }
    return result;
}

int display(Mat im, CMT & cmt)
{
    //Visualize the output
    //It is ok to draw on im itself, as CMT only uses the grayscale image
    for(size_t i = 0; i < cmt.points_active.size(); i++)
    {
        circle(im, cmt.points_active[i], 2, Scalar(255,0,0));
    }

    Point2f vertices[4];
    cmt.bb_rot.points(vertices);
    for (int i = 0; i < 4; i++)
    {
        line(im, vertices[i], vertices[(i+1)%4], Scalar(255,0,0));
    }

    imshow(WIN_NAME, im); waitKey(30);


    return waitKey(5);
}

int display2(Mat im, CMT & cmt)
{
    //Visualize the output
    //It is ok to draw on im itself, as CMT only uses the grayscale image
    for(size_t i = 0; i < cmt.points_active.size(); i++)
    {
        circle(im, cmt.points_active[i], 2, Scalar(255,0,0));
    }

    Point2f vertices[4];
    cmt.bb_rot.points(vertices);
    for (int i = 0; i < 4; i++)
    {
        line(im, vertices[i], vertices[(i+1)%4], Scalar(255,0,0));
    }

    imshow(WIN_NAME2, im); waitKey(30);


    return waitKey(5);
}

string write_rotated_rect(RotatedRect rect)
{
    Point2f verts[4];
    rect.points(verts);
    stringstream coords;

    coords << rect.center.x << "," << rect.center.y << ",";
    coords << rect.size.width << "," << rect.size.height << ",";
    coords << rect.angle << ",";

    for (int i = 0; i < 4; i++)
    {
        coords << verts[i].x << "," << verts[i].y;
        if (i != 3) coords << ",";
    }

    return coords.str();
}

void objecttrack(Mat im_gray, Mat im_gray2, Mat im, Mat im2, CMT cmt, CMT cmt2){

        //Let CMT process the frame
        cmt.processFrame(im_gray);
        cmt2.processFrame(im_gray2);


        if (penghitung == 0){
                 
                awal = (float)cmt.points_active.size(); waitKey(30);
                
                awal2 = (float)cmt2.points_active.size(); waitKey(30);
                penghitung++; 
        }

            
        per1 = 100*(cmt.points_active.size()/awal) + 50.00 ;
            
        per2 = 100*(cmt2.points_active.size()/awal2);

        

        if (per1 >= per2){ 
            char key =   display(im, cmt); waitKey(30);
        }
        else {
            char key2 =   display2(im2, cmt2); waitKey(30);
        }

}

int facetracker1 (int argc, char **argv)
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Create a CMT object
    CMT cmt, cmt2;

    //Initialization bounding box
    Rect rect,rect2;

    //Parse args
    int challenge_flag = 0;
    int loop_flag = 0;
    int verbose_flag = 0;
    int bbox_flag = 0;
    int skip_frames = 0;
    int skip_msecs = 0;
    int output_flag = 0;
    string input_path;
    string output_path;
    const int FRAME_WIDTH = 640;
	const int FRAME_HEIGHT = 480;

    const int detector_cmd = 1000;
    const int descriptor_cmd = 1001;
    const int bbox_cmd = 1002;
    const int no_scale_cmd = 1003;
    const int with_rotation_cmd = 1004;
    const int skip_cmd = 1005;
    const int skip_msecs_cmd = 1006;
    const int output_file_cmd = 1007;
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct option longopts[] =
    {
        //No-argument options
        {"challenge", no_argument, &challenge_flag, 1},
        {"loop", no_argument, &loop_flag, 1},
        {"verbose", no_argument, &verbose_flag, 1},
        {"no-scale", no_argument, 0, no_scale_cmd},
        {"with-rotation", no_argument, 0, with_rotation_cmd},
        //Argument options
        {"bbox", required_argument, 0, bbox_cmd},
        {"detector", required_argument, 0, detector_cmd},
        {"descriptor", required_argument, 0, descriptor_cmd},
        {"output-file", required_argument, 0, output_file_cmd},
        {"skip", required_argument, 0, skip_cmd},
        {"skip-msecs", required_argument, 0, skip_msecs_cmd},
        {0, 0, 0, 0}
    };

    int index = 0;
    int c;
    while((c = getopt_long(argc, argv, "v", longopts, &index)) != -1)
    {
        switch (c)
        {
            case 'v':
                verbose_flag = true;
                break;
            case bbox_cmd:
                {
                    //TODO: The following also accepts strings of the form %f,%f,%f,%fxyz...
                    string bbox_format = "%f,%f,%f,%f";
                    float x,y,w,h;
                    int ret = sscanf(optarg, bbox_format.c_str(), &x, &y, &w, &h);
                    if (ret != 4)
                    {
                        cerr << "bounding box must be given in format " << bbox_format << endl;
                        return 1;
                    }

                    bbox_flag = 1;
                    rect = Rect(x,y,w,h);
                }
                break;
            case detector_cmd:
                cmt.str_detector = optarg;
                break;
            case descriptor_cmd:
                cmt.str_descriptor = optarg;
                break;
            case output_file_cmd:
                output_path = optarg;
                output_flag = 1;
                break;
            case skip_cmd:
                {
                    int ret = sscanf(optarg, "%d", &skip_frames);
                    if (ret != 1)
                    {
                      skip_frames = 0;
                    }
                }
                break;
            case skip_msecs_cmd:
                {
                    int ret = sscanf(optarg, "%d", &skip_msecs);
                    if (ret != 1)
                    {
                      skip_msecs = 0;
                    }
                }
                break;
            case no_scale_cmd:
                cmt.consensus.estimate_scale = false;
                break;
            case with_rotation_cmd:
                cmt.consensus.estimate_rotation = true;
                break;
            case '?':
                return 1;
        }

    }

    // Can only skip frames or milliseconds, not both.
    if (skip_frames > 0 && skip_msecs > 0)
    {
      cerr << "You can only skip frames, or milliseconds, not both." << endl;
      return 1;
    }

    //One argument remains
    if (optind == argc - 1)
    {
        input_path = argv[optind];
    }

    else if (optind < argc - 1)
    {
        cerr << "Only one argument is allowed." << endl;
        return 1;
    }

    //Set up logging
    FILELog::ReportingLevel() = verbose_flag ? logDEBUG : logINFO;
    Output2FILE::Stream() = stdout; //Log to stdout

    //Challenge mode
    if (challenge_flag)
    {
        //Read list of images
        ifstream im_file("images.txt");
        vector<string> files;
        string line;
        while(getline(im_file, line ))
        {
            files.push_back(line);
        }

        //Read region
        ifstream region_file("region.txt");
        vector<float> coords = getNextLineAndSplitIntoFloats(region_file);

        if (coords.size() == 4) {
            rect = Rect(coords[0], coords[1], coords[2], coords[3]);
        }

        else if (coords.size() == 8)
        {
            //Split into x and y coordinates
            vector<float> xcoords;
            vector<float> ycoords;

            for (size_t i = 0; i < coords.size(); i++)
            {
                if (i % 2 == 0) xcoords.push_back(coords[i]);
                else ycoords.push_back(coords[i]);
            }

            float xmin = *min_element(xcoords.begin(), xcoords.end());
            float xmax = *max_element(xcoords.begin(), xcoords.end());
            float ymin = *min_element(ycoords.begin(), ycoords.end());
            float ymax = *max_element(ycoords.begin(), ycoords.end());

            rect = Rect(xmin, ymin, xmax-xmin, ymax-ymin);
            cout << "Found bounding box" << xmin << " " << ymin << " " <<  xmax-xmin << " " << ymax-ymin << endl;
        }

        else {
            cerr << "Invalid Bounding box format" << endl;
            return 0;
        }

        //Read first image
        Mat im0 = imread(files[0]);
        Mat im0_gray;
        cvtColor(im0, im0_gray, CV_BGR2GRAY);

        //Initialize cmt
        cmt.initialize(im0_gray, rect);

        //Write init region to output file
        ofstream output_file("output.txt");
        output_file << rect.x << ',' << rect.y << ',' << rect.width << ',' << rect.height << std::endl;

        //Process images, write output to file
        for (size_t i = 1; i < files.size(); i++)
        {
            FILE_LOG(logINFO) << "Processing frame " << i << "/" << files.size();
            Mat im = imread(files[i]);
            Mat im_gray;
            cvtColor(im, im_gray, CV_BGR2GRAY);
            cmt.processFrame(im_gray);
            if (verbose_flag)
            {
                display(im, cmt);
                
            }
            rect = cmt.bb_rot.boundingRect();
            output_file << rect.x << ',' << rect.y << ',' << rect.width << ',' << rect.height << std::endl;
        }

        output_file.close();

        return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Normal mode

    //Create window
    namedWindow(WIN_NAME);
    namedWindow(WIN_NAME2);

    Mat cameraFeed;
    Mat HSV;
    Mat threshold;
    
    int x = 0, y = 0,a =0;

    VideoCapture cap,cap2,cap0;

    bool show_preview = true;

    //If no input was specified
    if (input_path.length() == 0)
    {
        cap0.open(0);
        cap.open(1); //Open default camera device
        cap2.open(2);

        
        
        cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CV_CAP_PROP_FPS, 10);



        cap2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        cap2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
        cap2.set(CV_CAP_PROP_FPS, 10);


        cap0.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        cap0.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
        cap0.set(CV_CAP_PROP_FPS, 10);

    }

    //Else open the video specified by input_path
    else
    {
        cap.open(input_path);

        if (skip_frames > 0)
        {
          cap.set(CV_CAP_PROP_POS_FRAMES, skip_frames);
        }

        if (skip_msecs > 0)
        {
          cap.set(CV_CAP_PROP_POS_MSEC, skip_msecs);

          // Now which frame are we on?
          skip_frames = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
        }

        show_preview = false;
    }

    //If it doesn't work, stop
    if(!cap.isOpened())
    {
        cerr << "Unable to open video capture." << endl;
        return -1;
    }

    //Show preview until key is pressed
    while (show_preview)
    {
        Mat preview, preview2;
        cap >> preview;
        cap2 >> preview2;

        screenLog(preview, "Tekan tombol apapun untuk memulai");
        imshow(WIN_NAME, preview);
        screenLog(preview2, "Tekan tombol apapun untuk memulai");
        imshow(WIN_NAME2, preview2);


        char k = waitKey(10);
        if (k != -1) {
            show_preview = false;
        }
    }

    //Get initial image
    Mat im0,im02;
    cap >> im0;
    cap2 >> im02;

    //If no bounding was specified, get it from user
    if (!bbox_flag)
    {
        rect = getRect(im0, WIN_NAME);
        rect2 = getRect(im02, WIN_NAME2);
    }

    FILE_LOG(logINFO) << "Using " << rect.x << "," << rect.y << "," << rect.width << "," << rect.height
        << " as initial bounding box.";

    //Convert im0 to grayscale
    Mat im0_gray, im0_gray2;
    if (im0.channels() > 1 || im02.channels() > 1 ) {
        cvtColor(im0, im0_gray, CV_BGR2GRAY);
        cvtColor(im02, im0_gray2, CV_BGR2GRAY);
    } else {
        im0_gray = im0; 
        im0_gray2 = im02;
    }

    //Initialize CMT
    /** inisialisasi keypoint, 
    detector (fast feature), descriptor (BRISK) 
    dan matcher (BruteForce-Hamming)
    dan konsensus (inisialisasi point mana yg kepake dan nggak)
    **/
    //destroyWindow(WIN_NAME);
    //destroyWindow(WIN_NAME2);

    cmt.initialize(im0_gray, rect);
    cmt2.initialize(im0_gray2, rect2);


    int frame = skip_frames;

    //Open output file.
    ofstream output_file;
    ofstream outfile1;


    if (output_flag)
    {
        int msecs = (int) cap.get(CV_CAP_PROP_POS_MSEC);

        output_file.open(output_path.c_str());
        output_file << OUT_FILE_COL_HEADERS << endl;
        output_file << frame << "," << msecs << ",";
        output_file << cmt.points_active.size() << ",";
        output_file << write_rotated_rect(cmt.bb_rot) << endl;
    }


    //Main loop
    while (true)
    {
        frame++;


        Mat im, im2;
        Mat threshold(480,640, CV_8UC1);

        //If loop flag is set, reuse initial image (for debugging purposes)
        if (loop_flag) im0.copyTo(im);
        else cap >> im; cap2 >> im2;//Else use next image in stream


        
        cap0.read(cameraFeed);
    
        cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
        inRange(HSV, Scalar(123, 77, 0), Scalar(217, 256, 256), threshold);

    
        morphOps(threshold);

        Mat temp;
        threshold.copyTo(temp);

        vector< vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    
        double refArea = 0;
        bool objectFound = false;
    
        if (hierarchy.size() > 0) {
            int numObjects = hierarchy.size();
            if (numObjects<MAX_NUM_OBJECTS) {
                for (int index = 0; index >= 0; index = hierarchy[index][0]) {

                    Moments moment = moments((cv::Mat)contours[index]);
                    double area = moment.m00;

                    if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
                    x = moment.m10 / area; //central mass
                    y = moment.m01 / area;
                    objectFound = true;
                    }
                    else objectFound = false;

                }
            
                if(objectFound == true){
                outfile1.open("/home/ipung/Downloads/sekolah_asli/file/forarduino2.txt", ios::out | ios::app);
                outfile1 << "1"<< endl;
                outfile1.close();


                }
            else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
            }  

        }    
    

        if (im.empty() || im2.empty()) break; //Exit at end of video stream

        Mat im_gray, im_gray2;

        if (im.channels() > 1 || im2.channels() > 1) {
            cvtColor(im, im_gray, CV_BGR2GRAY);
            cvtColor(im2, im_gray2, CV_BGR2GRAY);
        } else {
            im_gray = im;
            im_gray2 = im2;
        }
        //Let CMT process the frame
        cmt.processFrame(im_gray);
        cmt2.processFrame(im_gray2);


        if (penghitung == 0){
                 
                awal = (float)cmt.points_active.size();
                
                awal2 = (float)cmt2.points_active.size();
                penghitung++; 
        }

            
        per1 = 100*(cmt.points_active.size()/awal) + 50.00 ;
            
        per2 = 100*(cmt2.points_active.size()/awal2);

        

        if (per1 >= per2){
            char key =   display(im, cmt);
        }
        else {
            char key2 =   display2(im2, cmt2);
        }

        waitKey(1000);
        

    }


 //Close output file.
    if (output_flag) output_file.close();

    return 0;

}

   


int main(){
    int facetracker1(int argc, char **argv) ;
    char ** a;
    int b;

    facetracker1(b,a);

	return 0;
}