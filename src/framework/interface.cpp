#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "imageManager.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
using namespace cv;
using namespace std;
static void help(char** argv)
{
    cout << "\nThis program demonstrates the famous watershed segmentation algorithm in OpenCV: watershed()\n"
            "Usage:\n" << argv[0] <<" [image_name -- default is fruits.jpg]\n" << endl;
    cout << "Hot keys: \n"
            "\tESC - quit the program\n"
            "\tr - restore the original image\n"
            "\tw or SPACE - run watershed segmentation algorithm\n"
            "\t\t(before running it, *roughly* mark the areas to segment on the image)\n"
            "\t  (before that, roughly outline several markers on the image)\n";
}
Mat markerMask, img, overlay, img_show, img_GS, markerMask_prev,markerMask_sub;
Point prevPt(-1, -1);
int markerId = 1;
double alpha = 0.5;
imageManager* img_backend;int* color_tab;
bool action =false;


static void onMouse( int event, int x, int y, int flags, void* )
{
    if( x < 0 || x >= img.cols || y < 0 || y >= img.rows )
        return;
    if( event == EVENT_LBUTTONUP && (flags & EVENT_FLAG_LBUTTON) ) {
        action = true;
        prevPt = Point(-1, -1);
    }
    else if( event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON) )
        prevPt = Point(-1, -1);
    else if( event == EVENT_LBUTTONDOWN )
        prevPt = Point(x, y);
    else if( event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON) )
    {
        Point pt(x, y);
        if( prevPt.x < 0 )
            prevPt = pt;
        line( markerMask, prevPt, pt, Scalar::all(markerId), 5, 8, 0 );
        switch (markerId) {
            case 1:
                line( overlay, prevPt, pt, Scalar::Vec(0,255,0,0), 5, 8, 0 );
                break;
            case 2:
                line( overlay, prevPt, pt, Scalar::Vec(0,0,255,0), 5, 8, 0 );
                break;
            case 0:
                line( overlay, prevPt, pt, Scalar::all(0), 30, 8, 0 );
                line( markerMask, prevPt, pt, Scalar::all(markerId), 30, 8, 0 );
        }

        prevPt = pt;
        addWeighted(overlay,alpha,img,1-alpha,0,img_show);
        imshow("image", img_show);
    }
}
int main( int argc, char** argv )
{
    cv::CommandLineParser parser(argc, argv, "{help h | | }{ @input | fruits.jpg | }");
    if (parser.has("help"))
    {
        help(argv);
        return 0;
    }
    string filename = samples::findFile(parser.get<string>("@input"));
    Mat img0 = imread(filename, 1), imgGray;
    resize(img0, img0, Size(), 0.5, 0.5, 0);
    if( img0.empty() )
    {
        cout << "Couldn't open image ";
        help(argv);
        return 0;
    }
    help(argv);
    namedWindow( "image", 1 );
    img0.copyTo(img);
    img0.copyTo(overlay);

    cvtColor(img, markerMask, COLOR_BGR2GRAY);
    cvtColor(img, markerMask_prev, COLOR_BGR2GRAY);
    cvtColor(img, overlay, COLOR_BGR2RGB);
    cvtColor(markerMask, imgGray, COLOR_GRAY2BGR);

    markerMask = Scalar::all(0);
    overlay = Scalar::all(0);
    markerMask_prev = Scalar::all(0);
    imshow( "image", img );
    setMouseCallback( "image", onMouse, 0 );

    cout << "Create backend image manager" << endl;
    img_GS = imread(filename,IMREAD_GRAYSCALE);
    cout << "Image in H " << img_GS.size << endl;
    cout << "Image in I " << imgGray.size << endl;

    imageManager ptr_imgman = imageManager(filename,img_GS);
    img_backend = &ptr_imgman;
    img_backend->init();
    color_tab = (int*)malloc(sizeof(int)*img_backend->getGraph().getNbVertex());
    int nb_change = 0;
    for(;;)
    {
        char c = (char)waitKey(100);
        if( c == 27 )
            break;
        if( c == 'r' )
        {
            markerMask = Scalar::all(0);
            overlay = Scalar::all(0);
            img0.copyTo(img);
            imshow( "image", img );
        }
        if( c == '1'){
            markerId = 1;
        }
        if( c == '2'){
            markerId = 2;
        }
        if( c == 's'){
            markerId = 0;
        }
        if( c == 'w' || action == true)
        {
            Mat markerMask_rzs, markerMask_prev_rzs;
            resize(markerMask, markerMask_rzs, Size(), 2, 2, 0);
            resize(markerMask_prev, markerMask_prev_rzs, Size(), 2, 2, 0);
            bool add = false;
            Mat nonZeroCoordinates_markerMask, nonZeroCoordinates_markerMask_prev;
            //Check if we added or removed markers
            findNonZero(markerMask_rzs, nonZeroCoordinates_markerMask);
            findNonZero(markerMask_prev_rzs, nonZeroCoordinates_markerMask_prev);

            if(nonZeroCoordinates_markerMask_prev.total() < nonZeroCoordinates_markerMask.total()){
                add = true;
            }

            if(add == true){
                subtract(markerMask_rzs,markerMask_prev_rzs,markerMask_sub);
            }
            else{
                subtract(markerMask_prev_rzs,markerMask_rzs,markerMask_sub);
            }

            markerMask.copyTo(markerMask_prev);
            Mat flatMarker;
            markerMask_sub.copyTo(flatMarker);
            flatMarker = flatMarker.reshape(1,1);
            Mat nonZeroCoordinates_flat, nonZeroCoordinates_all;

            findNonZero(flatMarker, nonZeroCoordinates_flat);
            int nbmarkers = nonZeroCoordinates_flat.total();
            cout << nbmarkers << endl;
            int* markers_idx = (int*)malloc(nbmarkers*sizeof(int));
            for (int i = 0; i < nbmarkers; i++ ) {
                markers_idx[i] = nonZeroCoordinates_flat.at<Point>(i).x;
            }

            if(add == true) {
                double t = (double) getTickCount();
                img_backend->addMarkers(markers_idx, nbmarkers, false);
                t = (double) getTickCount() - t;
                printf("execution time for adding = %gms\n", t * 1000. / getTickFrequency());
            }
            else{
                double t = (double) getTickCount();
                img_backend->removeMarkers(markers_idx, nbmarkers, false);
                t = (double) getTickCount() - t;
                printf("execution time for removing = %gms\n", t * 1000. / getTickFrequency());
            }

            int cpt = 0,seed,color;
            for(int i = 0; i < markerMask_rzs.rows; i++ )
                for(int j = 0; j < markerMask_rzs.cols; j++ )
                {
                    color = (int)markerMask_rzs.at<unsigned char>(i,j);
                    if(color != 0) {
                        color_tab[img_backend->segments_[cpt]] = color;
                    }
                cpt++;
            }

            Mat wshed(markerMask_rzs.size(), CV_8UC3);
            cpt = 0;
            for(int y = 0; y < img_backend->getHeight(); y++){
                for(int x = 0; x < img_backend->getWidth(); x++){
                    seed = color_tab[img_backend->segments_[cpt]];
                    if(seed == 1){
                        wshed.at<Vec3b>(y, x) = cv::Vec3b(0, 255, 0);
                    }
                    else{
                        wshed.at<Vec3b>(y, x) = cv::Vec3b(0, 0,255);
                    }
                    cpt++;
                }
            }
            resize(wshed, wshed, Size(), 0.5, 0.5, 0);
            wshed = wshed*0.5 + imgGray*0.5;
            imshow( "watershed transform", wshed );
            moveWindow("watershed transform", getWindowImageRect("image").y,getWindowImageRect("image").x);

            std::string file_name = "";
            std::string file_name_png = "";

            if(add == true){
                file_name.append(filename + "data/step_" + std::to_string(nb_change) + "_add.txt");
            }
            else{
                file_name.append(filename + "data/step" + std::to_string(nb_change)  + "_remove.txt");
            }

            ofstream fw(file_name, std::ofstream::out);

            if(fw.is_open()) {
                for (int i = 0; i < nbmarkers; i++) {
                    fw << std::to_string(markers_idx[i]);
                    if (i != nbmarkers - 1) {
                        fw << ",";
                    }
                }
                fw.close();
            }
            else{
                cout << "can't write markers." << endl;
            }

            if(add == true){
                file_name_png.append(filename + "data/step_" + std::to_string(nb_change) + "_add.png");
            }
            else{
                file_name_png.append(filename + "data/step" + std::to_string(nb_change)  + "_remove.png");
            }

            imwrite(file_name_png,markerMask_rzs);

            free(markers_idx);
            nb_change++;

            imwrite(filename + "history/seg_step" + std::to_string(nb_change) + ".png",wshed);
            action = false;
        }
    }
    return 0;
}