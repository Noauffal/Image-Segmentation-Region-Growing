#include <opencv2/opencv.hpp>
#include "Region.h"
#include "RegionGrowing.h"
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}


int main(int argc, char** argv)
{
  std::cout<<"program start.. " <<std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  cv::Mat img = cv::imread(argv[1], 1);
  unsigned int nbSeeds;
  unsigned int seuil;
  try
  {
    nbSeeds = stoi(getCmdOption(argv, argv + argc, "-nbs"));
    
  }
  catch(const std::exception& e)
  {
    nbSeeds = 1000;
  }
  try
  {
    seuil = stoi(getCmdOption(argv, argv + argc, "-s"));
    
  }
  catch(const std::exception& e)
  {
    seuil=20;
  }
  
  std::cout<<nbSeeds<<" "<<seuil<<std::endl;

  if (img.data)
  { if(cmdOptionExists(argv, argv + argc, "-bl2")){
      cv::Mat bluredImg2; 
      cv::blur(img, bluredImg2, cv::Size(3, 3));
      img = bluredImg2;
      cv::blur(img, bluredImg2, cv::Size(3, 3));
      img = bluredImg2;
    }
    if(cmdOptionExists(argv, argv + argc, "-bl")){
      cv::Mat bluredImg2; 
      cv::blur(img, bluredImg2, cv::Size(3, 3));
      img = bluredImg2;
    }
    RegionGrowing regionGrowing(img,nbSeeds,seuil);
    regionGrowing.segmentation();
    regionGrowing.completeSegmentation();
    if(cmdOptionExists(argv, argv + argc, "-cs") || cmdOptionExists(argv, argv + argc, "-all"))
      regionGrowing.coloration("../img/segmentation.jpg");
    if(cmdOptionExists(argv, argv + argc, "-r")){
      regionGrowing.fusion(stoi(getCmdOption(argv, argv + argc, "-r")));
    }
    else
      regionGrowing.fusion(0);
    if(cmdOptionExists(argv, argv + argc, "-cf") || cmdOptionExists(argv, argv + argc, "-all"))
      regionGrowing.coloration("../img/segmentation-fusion.jpg");
    if(cmdOptionExists(argv, argv + argc, "-cb") || cmdOptionExists(argv, argv + argc, "-all"))
      regionGrowing.showBorders("../img/segmentation-fusion-borders.jpg",false,false);
    if(cmdOptionExists(argv, argv + argc, "-cfbw") || cmdOptionExists(argv, argv + argc, "-all"))
      regionGrowing.showBorders("../img/segmentation-fusion-borders-c.jpg",true,false);
    else if(cmdOptionExists(argv, argv + argc, "-cfbb") || cmdOptionExists(argv, argv + argc, "-all"))
      regionGrowing.showBorders("../img/segmentation-fusion-borders-c.jpg",true,true);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout<<"program done and took "<<  duration.count() << " ms" <<std::endl;
   
  }

  // string filename2("perf.txt");
  // fstream outfile;
  // outfile.open(filename2, std::ios_base::out);
  // int cpt = 0;
  // for(nbSeeds = 1; nbSeeds<1000000;nbSeeds+=10000){

  //   std::cout<<"program start.. " <<std::endl;
  //   auto start = std::chrono::high_resolution_clock::now();

  //   RegionGrowing regionGrowing(img,nbSeeds,20);
  //   auto start1 = std::chrono::high_resolution_clock::now();
  //   regionGrowing.segmentation();
  //   regionGrowing.completeSegmentation();
  //   auto end1 = high_resolution_clock::now();
  //   auto duration1 = duration_cast<milliseconds>(end1 - start1);

  //   auto start2 = std::chrono::high_resolution_clock::now();
  //   regionGrowing.fusion(0);
  //   auto end2 = high_resolution_clock::now();
  //   auto duration2 = duration_cast<milliseconds>(end2 - start2);

  //   auto start3 = std::chrono::high_resolution_clock::now();
  //   regionGrowing.showBorders("../img/segmentation-fusion-borders-c.jpg",true,false);
  //   auto end3 = high_resolution_clock::now();
  //   auto duration3 = duration_cast<milliseconds>(end3 - start3);

  //   auto end = high_resolution_clock::now();
  //   auto duration = duration_cast<milliseconds>(end - start);
  //   std::cout<<"program done and took "<<  duration.count() << " ms" <<std::endl;



  //   if (!outfile.is_open()) {
  //     cout << "failed to open " << filename2 << '\n';
  //   } else {
  //       outfile << cpt <<" "<< nbSeeds << " " << 20 << " "<< duration1.count()<<" "<< duration2.count()<<" "<< duration3.count()<<" "<< duration.count() <<std::endl;
  //   }
  //   cpt++;

    
  // }
 
  
  return 0;
}