
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
  if (argc < 2) {
    cout << "Usage: ./find_roi <image_path>" << endl;
    return -1;
  }

  Mat img = imread(argv[1]);
  if (img.empty()) {
    cout << "Could not read the image: " << argv[1] << endl;
    return -1;
  }

  Mat gray, blurred, edged;
  cvtColor(img, gray, COLOR_BGR2GRAY);
  GaussianBlur(gray, blurred, Size(5, 5), 0);
  // Use Thresholding instead of Canny
  Mat binary;
  threshold(gray, binary, 100, 255,
            THRESH_BINARY_INV); // Invert to search for black features

  vector<vector<Point>> contours;
  findContours(binary, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

  // Sort contours by area descending
  sort(contours.begin(), contours.end(),
       [](const vector<Point> &a, const vector<Point> &b) {
         return contourArea(a) > contourArea(b);
       });

  cout << "Image size: " << img.cols << "x" << img.rows << endl;

  for (const auto &c : contours) {
    Rect rect = boundingRect(c);
    float aspectRatio = (float)rect.width / (float)rect.height;
    float area = rect.width * rect.height;

    // Relaxed filters to find smaller digit boxes
    // Digits are small, maybe area > 100
    if (area > 100 && aspectRatio > 0.5 && aspectRatio < 10.0) {
      cout << "Contour: x=" << rect.x << ", y=" << rect.y
           << ", w=" << rect.width << ", h=" << rect.height
           << ", AR=" << aspectRatio << ", Area=" << area << endl;
    }
  }

  return 0;
}
