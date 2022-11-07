#ifndef REGION_H
#define REGION_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <iterator>
#include <iomanip>
#include <algorithm>

class Region
{
private:
    unsigned int id;
    unsigned int idFusion;
    cv::Point seed;
    std::vector<cv::Point> border;
    std::vector<int> neighbors;
    size_t borderSize;
    size_t size;
    cv::Vec3b color;
    double b,g,r;
    bool expansion;
    bool fusion;

public:
    Region(const unsigned int &id,const unsigned int &x,const unsigned int &y, cv::Mat &img): id(id),idFusion(id), size(1), borderSize(1), seed(x, y),expansion(true),fusion(true), border(1,seed){
        color[0]= rand()%256;
        color[1]= rand()%256;
        color[2]= rand()%256;
        b=img.at<cv::Vec3b>(x,y)[0];
        g=img.at<cv::Vec3b>(x,y)[1];
        r=img.at<cv::Vec3b>(x,y)[2];
    };

    void updateBorder(std::vector<cv::Point> &new_border,const cv::Mat &matrix) {
        for(auto i : border){
            if((i.x-1 >= 0 && i.x+1 < matrix.size[0]) && (i.y-1 >= 0 && i.y+1 < matrix.size[1])){ //  x = line, height, y = column, width
                if(matrix.at<int>(i.x-1,i.y) != id || matrix.at<int>(i.x+1,i.y) != id || matrix.at<int>(i.x,i.y-1) != id || matrix.at<int>(i.x,i.y+1) != id) {
                    new_border.push_back(i);
                }
            }
            else{
                new_border.push_back(i);
            }
        }
        border.swap(new_border);
        new_border.clear();
    }

    void updateBGR(const cv::Vec3b &pixel){
        b+=pixel[0];
        g+=pixel[1];
        r+=pixel[2];
        this->size +=1;
    }



    void addNeighbor(const int &idRegion){
        //std::cout<<"neighbor added "<< idRegion << " in region " << id<<std::endl;
        neighbors.push_back(idRegion);
        //std::cout<<neighbors[0]<<std::endl;
    }

    bool isNeighbor(const int &id){
        if (std::find(neighbors.begin(), neighbors.end(), id) != neighbors.end()) {
            return true;
        }
        else {
            return false;
        }
    }


    void addPointToBorder(const cv::Point point){
        border.push_back(point);
    }

   
    //getters
    unsigned int getId() const   {return id;}
    unsigned int getIdFusion() const   {return idFusion;}
    double getB() const {return b;}
    double getG() const {return g;}
    double getR() const {return r;}
    cv::Point getSeed() const    {return seed;}
    size_t getSize() const      {return size;}
    std::vector<cv::Point> const getBorder() {return border;}
    size_t getBorderSize() const {return borderSize;}
    bool getExpansion() const    {return expansion;}
    bool getFusion() const    {return fusion;}
    std::vector<int> const getNeighbors() {return neighbors;}
    cv::Vec3b getColor() const {return color;}



    //setters
    void setExpansion(const bool exp){
        this->expansion = exp;
    }

    void setFusion(const bool fus){
        this->fusion = fus;
    }

    void setColor(const cv::Vec3b &newColor) {
        color = newColor;
    }

    void setIdFusion(const int idF){
        idFusion=idF;
    }

    void setNeighbors(std::vector<int> & newNeighbors){
        neighbors.swap(newNeighbors);
        newNeighbors.clear();
    }

};

#endif 