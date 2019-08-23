#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;
Mat src, src_gray, IX, IX2, IY2, IY, IXIY, R;
Mat IX2_blur, IY2_blur, IXIY_blur;
int thresh = 200;
int max_thresh = 255;
const char* source_window = "Source image";
const char* corners_window = "Corners detected";

void cornerHarris_demo( int, void* );
void non_max_suppresion(int r, Mat R);
void sharpen(Mat x, float alpha, float beta, float gamma);

int main( int argc, char** argv )
{
    CommandLineParser parser( argc, argv, "{@input | ../data/building.jpg | input image}" );
    src = imread( parser.get<String>( "@input" ) );
    src = src(cv::Rect(2000, 2000, 2000, 2000));
    
    //cv::GaussianBlur(frame, image, cv::Size(0, 0), 3);
    //cv::addWeighted(frame, 1.5, image, -0.5, 0, image);

    // cv::resize(src, src, cv::Size(), 0.2, 0.2, 1);
    if ( src.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        cout << "Usage: " << argv[0] << " <Input image>" << endl;
        return -1;
    }

    // bilateral
    // Mat temp;
    // imwrite("gray.tif", src_gray);
    // bilateralFilter(src_gray, temp, 9, 150, 150);
    // imwrite("bilateral.tif", temp);
    // src_gray = temp;

    //sharpen(src_gray, 1., -0.5,1);
    Mat temp;
    //imwrite("gray.tif", src_gray);
    bilateralFilter(src, temp, 9, 155, 155);
    imwrite("bilateral.tif", temp);
    src = temp;
    imshow("source", src);
    // addWeighted(src_gray, .7, temp, .3, 0, src_gray);
    // Canny(src_gray, src_gray, 250, 300, 3, true);
    
    cvtColor(src, src_gray, COLOR_BGR2GRAY );
    imwrite("test_.tif", src_gray);
    Sobel(src_gray, IX, CV_64F, 1, 0, 3);
    Sobel(src_gray, IY, CV_64F, 0, 1, 3);
    
    
    // imshow(source_window, IX);
    // waitKey(0);

    // imshow(source_window, IY);
    // waitKey(0);
    std::cout << "IX size: " << IX.size() << std::endl;
    std::cout << "IY size: " << IY.size() << std::endl;
    IXIY = IX.mul(IY);
    IX2 = IX.mul(IX);
    IY2 = IY.mul(IY);
    // imshow(source_window, IX2);
    // waitKey(0);

    // imshow(source_window, IY2);
    // waitKey(0);
    
    // imshow(source_window, IXIY);
    // waitKey(0);

    std::cout << IX.size() << IY.size() << IXIY.size() << std::endl;
    GaussianBlur(IX2, IX2_blur, Size(3, 3), 1, 0);
    GaussianBlur(IY2, IY2_blur, Size(3, 3), 0, 1);
    GaussianBlur(IXIY, IXIY_blur, Size(3, 3), 0, 0);

    // sharpen(src_gray, 1, -1, 0.5);
    // Canny(src_gray, src_gray, 250, 300, 3, true);
    
    // R = (IX2IY2 - IXIY * IXIY) - 0.05 * (IX2 + IY2) * (IX2 + IY2);
    R = (IX2_blur.mul(IY2_blur) - IXIY_blur.mul(IXIY_blur)) - (0.05 * (IX2_blur + IY2_blur) * (IX2_blur + IY2_blur));
    // R = (IX2_blur.mul(IY2_blur) - IXIY_blur.mul(IXIY_blur)).mul(1 / (IX2_blur + IY2_blur) * (IX2_blur + IY2_blur));
    // adding threshold
    double minVal; 
    double maxVal; 
    Point minLoc; 
    Point maxLoc;

    minMaxLoc( R, &minVal, &maxVal, &minLoc, &maxLoc );
    float t = 0.1 * R.at<float>(maxLoc.x, maxLoc.y);
    std::cout << "threshold is: " << t <<std::endl;
    for(int i = 0; i < R.rows; i++){
      for(int j = 0; j < R.cols; j++){
        //std::cout << "i, j: " << i << j << " intensity: " << R.at<float>(i,j) << std::endl;
        if (R.at<float>(i,j) < t){
          R.at<float>(i,j) = 0;
        }
        else{
          circle(src, Point(i, j), 1, Scalar(255, 0, 0), 1, 8, 0);
        }
      }
    }

    imshow(source_window, R);
    waitKey(0);
    //non_max_suppresion(9, R);
    imwrite("R.tif", R);
    imwrite("subsrc.tif", src);
    // ----------------------------------//
    // cvtColor( src, src_gray, COLOR_BGR2GRAY );
    // namedWindow( source_window );
    // createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo );
    // imshow( source_window, src );
    // cornerHarris_demo( 0, 0 );
    imshow(source_window, src_gray);
    waitKey(0);

    return 0;
}

void non_max_suppresion(int r, Mat R){
  Mat newR;
  copyMakeBorder(R, newR, r, r, r, r, BORDER_CONSTANT, 0);
  newR = newR.clone();
  std::cout << "new R size: " << newR.size() << std::endl;
  for(int i = 0; i < R.rows; i++){
    for(int j = 0; j < R.cols; j++){
      // max/min minMaxLoc( m, &minVal, &maxVal, &minLoc, &maxLoc )
      double a,b;
      Point c,d;
      minMaxLoc(newR(Rect(i, j, 2*r+1, 2*r+1)), &a, &b, &c, &d);
      std::cout << R.at<float>(i, j) << std::endl;
      if (R.at<float>(i, j) != newR.at<float>(c.x, c.y) && c.x >= 0 && c.y >=0){
        R.at<float>(i, j) = 0;
        //circle(src_gray, Point(i,j), 3,  Scalar(0, 0, 255), 2, 8, 0 );
      }
    }
  }
}

void sharpen(Mat x, float alpha, float beta, float gamma){
    // sharpen
  Mat temp;
  GaussianBlur(x, temp, Size(0, 0), 3);
  addWeighted(x, alpha, temp, beta, gamma, x);
  // sharpen
}