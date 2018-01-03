#include "RobotVision2017.h"
#include <math.h>
#include <thread>

#define PI 3.14159265

using namespace cv;

long int cycles = 0;

std::thread nt_thread;

void contours_callback(int, void*);

extern nt_table_data table_data;
extern string light_color;

bool compare_rect(const Rect &a, const Rect & b)
{
    return a.x < b.x;
}

Mat image, original_image;
int thresh = 100;
string ImageDir;

int processImage(Mat source_image, string imageFileDirectory)
{
    extern bool SHOW_ORIG_IMAGE;
    extern bool SHOW_PROCESSED_IMAGE;
    extern bool SHOW_CONTOUR_IMAGE;
    ImageDir = imageFileDirectory;
    image = source_image;
    original_image = source_image.clone();

    if (scale != 1)
    {
        cv::Size cvResizeDsize(0, 0);
        double cvResizeFx = scale; // default Double
        double cvResizeFy = scale; // default Double
        int cvResizeInterpolation = cv::INTER_LINEAR;
        resize(image,
                image,
                Size(),
                cvResizeFx,
                cvResizeFy,
                CV_INTER_AREA);
    }

    image = image + Scalar(BRIGHT_ADJUST, BRIGHT_ADJUST, BRIGHT_ADJUST);

    if (light_color == "white")
    {
        cvtColor(image, image, CV_BGR2GRAY);
    }
    blur(image, image, Size(3, 3));

    createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, contours_callback);
    contours_callback(0, 0);

    return (0);
}

/** @function thresh_callback */
void contours_callback(int, void*)
{

    extern bool SHOW_ORIG_IMAGE;
    extern bool SHOW_PROCESSED_IMAGE;
    extern bool SHOW_CONTOUR_IMAGE;

    extern Mat global_img;
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Scalar color;

    if (light_color == "blue")
        color = Scalar(0, 0, 255);
    else
        color = Scalar(255, 255, 255);

    int MIN_RECT_HEIGHT = 25;
    int MAX_RECT_HEIGHT = 999;

    table_data.seq_no = cycles;

    if (light_color == "blue")
        inRange(image, cv::Scalar(0, 0, 200), cv::Scalar(255, 255, 255), image);

    /// Detect edges using canny
    Canny(image, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));



    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<RotatedRect> rotRect(contours.size());
    vector<RotatedRect> potentialRotRect(contours.size());
    vector<Point2f>center(contours.size());
    vector<float>radius(contours.size());

    vector<Rect> potentialRect(contours.size());
    int numPotentialRect = 0;

    vector<Rect> acceptedRect(contours.size());
    int numAcceptedRect = 0;

    for (int i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
        rotRect[i] = minAreaRect(Mat(contours_poly[i]));
    }
    //sort(boundRect.begin(), boundRect.end(), compare_rect);

    //fprintf(stderr, "---------------------------------------\n");

    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++)
    {
        if (boundRect[i].height > MIN_RECT_HEIGHT && boundRect[i].height < MAX_RECT_HEIGHT && rotRect[i].angle < 5)
        {
            double aspect_ratio = (double) boundRect[i].width / (double) boundRect[i].height;

            if (aspect_ratio >= ASPECT_RATIO_MIN && aspect_ratio <= ASPECT_RATIO_MAX)
            {
                potentialRotRect[numPotentialRect] = rotRect[i];
                potentialRect[numPotentialRect++] = boundRect[i];

                if (SHOW_CONTOUR_IMAGE)
                {
                    Scalar color = Scalar(192, 192, 192);
                    drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
                    color = Scalar(255, 0, 0);
                    rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
                }
                if (SHOW_ORIG_IMAGE)
                {
                    Scalar color = Scalar(192, 192, 192);
                    drawContours(original_image, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
                    color = Scalar(255, 0, 0);
                    rectangle(original_image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
                }
            }
        }
    }

    int last_x = 0;
    int last_y = 0;
    for (int i = 0; i < numPotentialRect; i++)
    {
        double MAX_X_OFF = 1.7 * potentialRect[i].height;
        double MIN_X_OFF = .8 * potentialRect[i].height;
        double MAX_Y_OFF = .15 * potentialRect[i].height;

        for (int x = 0; x < numPotentialRect; x++)
        {

            int delta_x = abs(potentialRect[i].x - potentialRect[x].x);
            int delta_y = abs(potentialRect[i].y - potentialRect[x].y);
            if (x != i && delta_x > 0 & delta_y < MAX_Y_OFF)
            {
                double height_diff_pct = (double) abs(potentialRect[i].height - potentialRect[x].height) / (double) potentialRect[i].height;
                double width_diff_pct = (double) abs(potentialRect[i].width - potentialRect[x].width) / (double) potentialRect[i].width;

                if (last_x != potentialRect[x].x && last_y != potentialRect[x].y)
                {
                    if (delta_x >= MIN_X_OFF && delta_x <= MAX_X_OFF && height_diff_pct < 0.1 && width_diff_pct < 0.1)
                    {
                        last_x = potentialRect[x].x;
                        last_y = potentialRect[x].y;

                        acceptedRect[numAcceptedRect++] = potentialRect[i];
                        break;
                    }
                }
            }
        }
    }

    //fprintf(stderr, "numAcceptedRect = %d\n", numAcceptedRect);

    double avg_distance = 0;
    for (int i = 0; i < numAcceptedRect; i++)
    {
        color = Scalar(0, 255, 0);
        if (SHOW_CONTOUR_IMAGE)
            rectangle(drawing, acceptedRect[i].tl(), acceptedRect[i].br(), color, 2, 8, 0);
        rectangle(original_image, acceptedRect[i].tl(), acceptedRect[i].br(), color, 2, 8, 0);

        cv::circle(drawing, cv::Point(acceptedRect[i].x, acceptedRect[i].y), 3, cv::Scalar(0, 0, 255));
    }



    double img_h_center = (double) (image.cols / 2);
    double img_v_center = (double) (image.rows / 2);
    double off_center = 0.0;
    double angle_calc = 0.0;
    if (numAcceptedRect == 2)
    {
        int FOCAL_LENGTH = 836 - (2 * acceptedRect[0].height);
        double dist1 = ((double) (5 * FOCAL_LENGTH) / (double) acceptedRect[0].height);
        double dist2 = ((double) (5 * FOCAL_LENGTH) / (double) acceptedRect[1].height);

        avg_distance += (double) (dist1 + dist2) / 2.0;

        //angle_calc = asin((dist1 - dist2) / 8.25) * 180.0 / PI;

        double pt1 = (double) acceptedRect[0].x + (double) acceptedRect[0].width;
        double pt2 = (double) acceptedRect[1].x;

        //fprintf(stderr, "%.0f %.0f %.3f\n", pt1, pt2, img_h_center);
        off_center = ((pt1 - img_h_center) / 2) + ((pt2 - img_h_center) / 2);

        double dist_off_center = acceptedRect[0].height / 5 * off_center;
        angle_calc = asin((off_center / acceptedRect[0].height * 5) / dist1) * 180.0 / PI;

        table_data.angle = angle_calc;
        table_data.distance = avg_distance;
        table_data.steer = off_center;
        table_data.data_seq_no = cycles;
        table_data.good_images++;
        //fprintf(stderr, "Distance = %.3f\n", avg_distance);
        //fprintf(stderr, "Off Center = %.3f\n", off_center);
    }
    fprintf(stderr, "Good Images = %Ld\n", table_data.good_images);

    if (1 == 1 || SHOW_ORIG_IMAGE)
    {
        Scalar grid_color = Scalar(153, 255, 255);

        // vertical center line
        line(original_image, cv::Point(img_h_center, 0), cv::Point(img_h_center, original_image.rows), grid_color, 1, 8, 0);
        int pt1 = img_h_center - 20;
        int pt2 = img_h_center + 20;
        int space = (int) (original_image.rows / 8);
        for (int intersect_point = space; intersect_point < original_image.rows; intersect_point += space)
        {
            line(original_image, cv::Point(pt1, intersect_point), cv::Point(pt2, intersect_point), grid_color, 1, 8, 0);
        }

        // horizontal center line
        line(original_image, cv::Point(0, img_v_center), cv::Point(original_image.cols, img_v_center), grid_color, 1, 8, 0);
        pt1 = img_v_center - 20;
        pt2 = img_v_center + 20;
        space = (int) (original_image.cols / 8);
        for (int intersect_point = space; intersect_point < original_image.cols; intersect_point += space)
        {
            line(original_image, cv::Point(intersect_point, pt1), cv::Point(intersect_point, pt2), grid_color, 1, 8, 0);
        }

        // ADD GRAY BACKGROUND FOR DATA DISPLAY
        int box_height = 50;
        color = Scalar(75, 75, 75);
        rectangle(original_image, Point(0, original_image.rows), Point(original_image.cols, original_image.rows - box_height), color, -1, 8);

        color = Scalar(190, 255, 255);
        int x1 = 15;
        int x2 = 160;
        int x3 = 330;
        int x4 = 500;
        int bottom_row = original_image.rows - 10;
        int top_row = bottom_row - 22;
        int fontFace = 10;
        bool bottomLeftOrigin = false;
        char str_buffer[150];

        int camera = 0;
        double speed_fps;
        sprintf(str_buffer, "Speed: %.1ffps", speed_fps);
        putText(original_image, str_buffer, Point(x1, top_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);

        sprintf(str_buffer, "Heading: %.1f", table_data.heading);
        putText(original_image, str_buffer, Point(x1, bottom_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);

        sprintf(str_buffer, "Accel: %.1ffps/s", speed_fps);
        putText(original_image, str_buffer, Point(x2, top_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);

        sprintf(str_buffer, "Camera: %d", camera);
        putText(original_image, str_buffer, Point(x2, bottom_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);


        sprintf(str_buffer, "Dist Meas: NA");
        //sprintf(str_buffer, "Dist Meas: %.1fin", avg_distance);
        putText(original_image, str_buffer, Point(x3, top_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);

        if (numAcceptedRect == 2)
        {

            int lock_limit = 4; // how many pixels off center is good
            Scalar circle_color = (abs(off_center) < lock_limit) ? Scalar(0, 255, 0) : Scalar(255, 0, 0);
            int circle_dia = (abs(off_center) < lock_limit) ? 15 : 10;
            int circle_thickness = (abs(off_center) < lock_limit) ? -1 : 2;

            circle(original_image, cv::Point(img_h_center + off_center, img_v_center), circle_dia, circle_color, circle_thickness);

            sprintf(str_buffer, "Dist Calc: %.1fin", avg_distance);
            putText(original_image, str_buffer, Point(x3, bottom_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);

            sprintf(str_buffer, "Angle: %.0f", angle_calc);
            putText(original_image, str_buffer, Point(x4, top_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);

            sprintf(str_buffer, "Off Center: %.0f", off_center);
            putText(original_image, str_buffer, Point(x4, bottom_row), 1, 1.0, color, 1, 8, bottomLeftOrigin);
        }

    }

    global_img = original_image.clone();

    /// Show in a window
    if (SHOW_PROCESSED_IMAGE)
        imshow(PROCESSED_IMAGE_WIN, image);
    if (SHOW_ORIG_IMAGE)
        imshow(ORIG_IMAGE_WIN, original_image);
    if (SHOW_CONTOUR_IMAGE)
        imshow(CONTOUR_WIN, drawing);

    cycles++;

}
