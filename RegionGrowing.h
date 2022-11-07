#ifndef REGION_GROWING_H
#define REGION_GROWING_H
#include <iostream>
#include "opencv2/opencv.hpp"
#include "Region.h"
#include <algorithm> // for copy() and assign()
#include <iterator>
#include <stack>
#include <map>
#include <math.h>
#include <string.h>
using namespace std::chrono;

class RegionGrowing
{
private:
    unsigned int nbSeeds;
    const unsigned int seuil;
    std::map<int,Region> finalRegions;
    std::vector<Region> regions;
    cv::Mat matrix;
    cv::Mat image;

public:
    RegionGrowing(const cv::Mat &img, unsigned int &nbSeeds, const unsigned int &seuil):nbSeeds(nbSeeds), seuil(seuil){
        srand (time(NULL));
        this->image = img;
        this->matrix = cv::Mat::zeros(cv::Size(img.size[1], img.size[0]), CV_32SC1);
        std::cout<<"img("<<img.size[0]<< "," << img.size[1]<<") and size: "<<img.size[1]*img.size[0]<<std::endl;
        initSeedsAndRegions();
    }

    void initSeedsAndRegions(){
        std::cout<<"init seeds.."<<std::endl;
        srand (time(NULL));
        const int TAILLE = matrix.size[0]*matrix.size[1]; 
        int initInterval = (TAILLE)/(this->nbSeeds);
        int interval = initInterval;
        for(int cpt=0; cpt<nbSeeds ; cpt++){
            if(cpt==nbSeeds-1){
                interval = TAILLE-(cpt*initInterval);
            }
            int index = rand()%interval + cpt*initInterval;
            int l = index / (matrix.size[1]);
            int c = index % (matrix.size[1]);
            this->matrix.at<int>(l,c)=cpt+1;
            Region region(cpt+1,l,c,this->image);
            regions.push_back(region);
        }

    }



    bool distanceBGR(Region &region,cv::Vec3b &pixel) const{
        
        int b= region.getB()/region.getSize();
        int g= region.getG()/region.getSize();
        int r= region.getR()/region.getSize();

        if(abs(b - pixel[0]) <= seuil && abs(g - pixel[1]) <= seuil && abs(r - pixel[2]) <= seuil){
            return true;
        }

        return false;
    }

    bool distanceRegion(Region &region1,Region &region2, const unsigned int seuil_)const{
        int b1= region1.getB()/region1.getSize();
        int g1= region1.getG()/region1.getSize();
        int r1= region1.getR()/region1.getSize();
        int b2= region2.getB()/region2.getSize();
        int g2= region2.getG()/region2.getSize();
        int r2= region2.getR()/region2.getSize();
        if(abs(b1 - b2) <= seuil_ && abs(g1 - g2) <= seuil_ && abs(r1 - r2) <= seuil_){
            return true;
        }
        
        return false;
    }

    void updateMatrix(std::vector<cv::Point> &new_border,const cv::Point &borderPoint,Region &region){
        for (int line = borderPoint.x-1; line <= borderPoint.x+1; line++){
            for (int column = borderPoint.y-1; column <= borderPoint.y+1; column++){
                if(line>=0 && line<matrix.size[0] && column>=0 && column<matrix.size[1]){
                    int& idRegion = matrix.at<int>(line,column);
                    if(idRegion == 0 && distanceBGR(region,image.at<cv::Vec3b>(line,column))){
                        new_border.push_back(cv::Point(line,column));
                        idRegion = region.getId();
                        region.updateBGR(image.at<cv::Vec3b>(line,column));
                        
                    }
                }
            }
        }
    }


    void segmentation(){
        std::cout<<"starting segmentation... " << std:: endl;
        auto start = std::chrono::high_resolution_clock::now();
        bool expansion = true;
        int cpt =0;
        //int nbFalse=0;
        while (expansion){   
            int counter = 0;
            cpt++;
            for(auto &region : regions){
                if(region.getExpansion() == true){
                    counter++;
                    std::vector<cv::Point> new_border;
                    for(auto &borderPoint : region.getBorder())
                        updateMatrix(new_border, borderPoint, region);
                    if (new_border.empty()){

                        region.setExpansion(false); 
                    }
                    else
                        region.updateBorder(new_border,matrix);
                }
            }
            if (counter == 0)
                expansion = false;

        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        std::cout<<"segmentation done and took "<<  duration.count() << " ms" <<std::endl;

    }

    void completeSegmentation(){
        for(int line=0; line<image.size[0]; line++){
            for(int column=0; column<image.size[1];column++){
                if(matrix.at<int>(line,column) == 0){
                    Region region(regions.size()+1,line,column,this->image);
                    bool expansion = true;
                    while (expansion){   
                        int counter = 0;
                        if(region.getExpansion() == true){
                            counter++;
                            std::vector<cv::Point> new_border;
                            for(auto &borderPoint : region.getBorder())
                                updateMatrix(new_border, borderPoint, region);
                            if (new_border.empty()){

                                region.setExpansion(false); 
                            }
                            else
                                region.updateBorder(new_border,matrix);
                        }
                        if (counter == 0)
                            expansion = false;

                    }
                    regions.push_back(region);
                }
            }
        }
    }


    void fusion(const unsigned int &lissage){
        std::cout<<"starting fusion... " << std:: endl;
        std::stack<int> stackRegion;
        for(auto &region : regions){
            for(auto &borderPoint : region.getBorder()){
                for (int line = borderPoint.x-1; line <= borderPoint.x+1; line++){
                    for (int column = borderPoint.y-1; column <= borderPoint.y+1; column++){
                        if(line>=0 && line<matrix.size[0] && column>=0 && column<matrix.size[1]){
                            int& idRegion = matrix.at<int>(line,column);
                            if(idRegion != 0 && idRegion != region.getId()){
                                if (!region.isNeighbor(idRegion)){
                                    region.addNeighbor(idRegion);
                                    regions[idRegion-1].addNeighbor(region.getId());

                                }
                            }
                        }
                    }
                }
            }
            stackRegion.push(region.getId());
        }
        while(!stackRegion.empty()){
            int idR = stackRegion.top();
            stackRegion.pop();
            if(regions[idR-1].getFusion()==true){
                regions[idR-1].setFusion(false);
                for(auto id : regions[idR-1].getNeighbors()){
                    if(distanceRegion(regions[idR-1],regions[id-1],seuil) || (regions[id-1].getSize()<20 && distanceRegion(regions[idR-1],regions[id-1],lissage))){
                        stackRegion.push(id);
                        regions[id-1].setColor(regions[idR-1].getColor());
                        regions[id-1].setIdFusion(regions[idR-1].getIdFusion());

                    }
                }
            }
        }
        updateBorders();
        std::cout<<"fusion done. " << std:: endl;
    }
    std::vector<cv::Point> findNeighbors(const cv::Point & borderPoint){
        std::vector<cv::Point> neighbors_tmp;
        for (int line = borderPoint.x-1; line <= borderPoint.x+1; line++){
            for (int column = borderPoint.y-1; column <= borderPoint.y+1; column++){
                if(line>=0 && line<matrix.size[0] && column>=0 && column<matrix.size[1]){
                }
            }
        }
        
        return neighbors_tmp;
    }

    void updateBorders(){
    
        for(size_t i = 0; i<regions.size(); i++){  
            for(cv::Point borderPoint : regions[i].getBorder()){
                for (int line = borderPoint.x-1; line <= borderPoint.x+1; line++){
                    for (int column = borderPoint.y-1; column <= borderPoint.y+1; column++){
                        if(line>=0 && line<matrix.size[0] && column>=0 && column<matrix.size[1]){
                            int idRegion = matrix.at<int>(line,column);
                            if(idRegion == 0 || regions[idRegion-1].getIdFusion() != regions[i].getIdFusion()){
                                try
                                {
                                    finalRegions.at(regions[i].getIdFusion()); 
                                }
                                catch(const std::exception& e)
                                {
                                    finalRegions.insert (std::pair<int,Region>(regions[i].getIdFusion(),Region(regions[i].getIdFusion(),borderPoint.x,borderPoint.y,matrix)));
                                }

                                finalRegions.at(regions[i].getIdFusion()).addPointToBorder(borderPoint);

                            } 
                        } 
                    }
                }
            }   
        }
    }


    void showBorders(std::string path,bool background, bool black){
        unsigned char color;
        if(black)
            color = 0;
        else
            color = 255;
        cv::Mat outputImg2(image.size[0], image.size[1],
                      CV_8UC3, cv::Scalar(color, color, color));
        if(background){
            for(int line = 0 ; line<matrix.size[0]; line++){
                for(int column = 0; column<matrix.size[1];column++){
                    if(matrix.at<int>(line,column)!=0)
                        outputImg2.at<cv::Vec3b>(line,column) = regions[matrix.at<int>(line,column) -1].getColor();

                }
            }
        }
        else if(black) {
            for(int line = 0 ; line<matrix.size[0]; line++){
                for(int column = 0; column<matrix.size[1];column++){
                    outputImg2.at<cv::Vec3b>(line,column) = cv::Vec3b(255,255,255);
                }
            }
        }
        else{
            for(int line = 0 ; line<matrix.size[0]; line++){
                for(int column = 0; column<matrix.size[1];column++){
                    outputImg2.at<cv::Vec3b>(line,column) = cv::Vec3b(0,0,0);
                }
            }
        }


        for(auto &region: finalRegions){
            for(auto &borderPoint : region.second.getBorder()){
                outputImg2.at<cv::Vec3b>(borderPoint.x,borderPoint.y) = cv::Vec3b(color,color,color);
            }
        }
        cv::imwrite(path, outputImg2);
    }

    void coloration(std::string path){
        cv::Mat outputImg(image.size[0], image.size[1],
                      CV_8UC3, cv::Scalar(255, 255, 255));
        for(int line = 0 ; line<matrix.size[0]; line++){
            for(int column = 0; column<matrix.size[1];column++){
                if(matrix.at<int>(line,column)!=0)
                    outputImg.at<cv::Vec3b>(line,column) = regions[matrix.at<int>(line,column) -1].getColor();

            }
        }
        cv::imwrite(path, outputImg);
    }

};
#endif 