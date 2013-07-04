

#include "TComVideoStats.h"

std::fstream TComVideoStats::rawYUV;
std::fstream TComVideoStats::statsYUV;

Bool TComVideoStats::firstFrame;

Char TComVideoStats::lumaPic[1600][2560];
Char TComVideoStats::chromaBPic[1600/2][2560/2];
Char TComVideoStats::chromaRPic[1600/2][2560/2];

UInt TComVideoStats::videoWidth;
UInt TComVideoStats::videoHeight;
std::string TComVideoStats::videoPath;

Void TComVideoStats::init(){
    
    videoWidth = -1;
    videoHeight = -1;
    videoPath = "";   
}

Void TComVideoStats::openYUV(){
    rawYUV.open(videoPath.c_str(),ios::in|ios::binary);
    firstFrame = true;
    std::string outPath = "Borders.yuv";
    statsYUV.open(outPath.c_str(),ios::out|ios::binary);
}

Void TComVideoStats::loadPics(){
    
//    if(!firstFrame){
//        writeStatsVideo();
//    }

    for(int i = 0; i < videoHeight; i++){
        char *c = new char;
        for(int j = 0; j < videoWidth; j++){
            rawYUV.read(c,1);
            lumaPic[i][j] = *(c);
        }
    }
    
    for(int i = 0; i < videoHeight/2; i++){
        char *c = new char;
        for(int j = 0; j < videoWidth/2; j++){
           rawYUV.read(c,1);
           chromaBPic[i][j] = *(c);
        }
    }
    
    for(int i = 0; i < videoHeight/2; i++){
        char *c = new char;
        for(int j = 0; j < videoWidth/2; j++){
            rawYUV.read(c,1);
            chromaRPic[i][j] = *(c);
        }
    }
    
    firstFrame = false;

}

Void TComVideoStats::writeStatsVideo(){
    for(int i = 0; i < videoHeight; i++){
      char *c = new char[videoWidth];
      for(int j = 0; j < videoWidth; j++){
          
          c[j] = lumaPic[i][j];
      }
      statsYUV.write(c,videoWidth);
    }

    for(int i = 0; i < videoHeight/2; i++){
      char *c = new char[videoWidth/2];
      for(int j = 0; j < videoWidth/2; j++){
          
          c[j] = chromaBPic[i][j];
      }
      statsYUV.write(c,videoWidth/2);
    }

    for(int i = 0; i < videoHeight/2; i++){
      char *c = new char[videoWidth/2];
      for(int j = 0; j < videoWidth/2; j++){
          
          c[j] = chromaRPic[i][j];
      }
      statsYUV.write(c,videoWidth/2);
    }    
}

Void TComVideoStats::printStatsInPics(UInt puX, UInt puY, UInt puW, UInt puH, UInt predMode, Int mvHor, Int mvVer){
         // PINK: u = 190; v = 220; 
         // GRAY: u = 128; v = 128;
         // PURPLE: u = 200; v = 130; 
         // GREEN: u = 40; v = 40; 
         // ORANGE: u = 1; v = 230; 
         // RED: u = 120; v = 240; 
         //0 - INTER = ORANGE,  1 - INTRA = GREEN, 2-SKIP = GRAY        
    Char predModeMapB[] = {1,40,128};
    Char predModeMapR[] = {230,40,128};
    
    for(int i = puY; i < puY + puH -1; i++)
        lumaPic[i][puX + puW -1] = 0;
    for(int j = puX; j < puX + puW; j++)
        lumaPic[puY + puH -1][j] = 0;
    
    if(predMode == MODE_INTER){
        //TODO printMv
    }


#if EN_MODE_IN_CHROMA
    for(int i = puY/2; i < (puY + puH)/2; i++){
        for(int j = puX/2; j < (puX + puW)/2; j++){
            chromaBPic[i][j] = predModeMapB[predMode];
            chromaRPic[i][j] = predModeMapR[predMode];
        }
    }
#endif 
}

Void TComVideoStats::closeYUV(){
    statsYUV.close();
    rawYUV.close();
}

