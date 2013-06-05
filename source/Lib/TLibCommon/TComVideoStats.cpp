

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
    
    if(!firstFrame){
        writeStatsVideo();
    }

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

Void TComVideoStats::printStatsInPics(TComDataCU*& cu, UInt partIdx, UInt predMode, Int puIdx, Int mvHor, Int mvVer){
    Int puX, puY, puW, puH;
    Int n_PURow = 64/4;
    Int d = cu->getDepth(partIdx);
    
    PartSize partSize = cu->getPartitionSize(partIdx);
    puW = getPUWidth(d,puIdx, partSize);
    puH = getPUHeight(d,puIdx, partSize);
    
    partIdx = g_auiZscanToRaster[partIdx];
   
    puX = cu->getCUPelX() + (4*(partIdx % n_PURow));
    puX += getXOffSetInPU(d, puIdx, partSize);
    
    puY = cu->getCUPelY() + (4*floor(partIdx/n_PURow));
    puY += getYOffSetInPU(d, puIdx, partSize);
    
    for(int i = puY; i < puY + puH -1; i++)
        lumaPic[i][puX + puW -1] = 0;
    for(int j = puX; j < puX + puW; j++)
        lumaPic[puY + puH -1][j] = 0;
    
    if(predMode == MODE_INTER){
        //TODO printMv
    }
}

Void TComVideoStats::closeYUV(){
    statsYUV.close();
    rawYUV.close();
}

Int TComVideoStats::getPUWidth(Int d, Int PUIdx, PartSize partSize){
    Int base_w = 64 >> d;
    switch(partSize){
        case SIZE_Nx2N:
            return base_w >> 1;
        case SIZE_NxN:
            return base_w >> 1;
        case SIZE_nLx2N:
            return ((PUIdx == 0) ? (base_w >> 2) : (base_w - (base_w >> 2)));
        case SIZE_nRx2N:
            return ((PUIdx == 0) ? (base_w - (base_w >> 2)) : (base_w >> 2));
        default:
            return base_w;
    }
}

Int TComVideoStats::getPUHeight(Int d, Int PUIdx, PartSize partSize){
    Int base_h = 64 >> d;
    switch(partSize){
        case SIZE_2NxN:
            return base_h >> 1;
        case SIZE_NxN:
            return base_h >> 1;
        case SIZE_2NxnU:
            return ((PUIdx == 0) ? (base_h >> 2) : (base_h - (base_h >> 2)));
        case SIZE_2NxnD:
            return ((PUIdx == 0) ? (base_h - (base_h >> 2)) : (base_h >> 2));
        default:
            return base_h;
    }
}

Int TComVideoStats::getXOffSetInPU(Int d, Int PUIdx, PartSize partSize){
    Int base_w = 64 >> d;
    
    switch(partSize){
        case SIZE_Nx2N:
            return ((PUIdx == 0) ? 0 : (base_w >> 1));
        case SIZE_NxN:
            return ((PUIdx == 0 or PUIdx == 2) ? 0 : (base_w >> 1));
        case SIZE_nLx2N:
            return ((PUIdx == 0) ? 0 : (base_w >> 2 ));
        case SIZE_nRx2N:
            return ((PUIdx == 0) ? 0: (base_w - (base_w >> 2)));
        default:
            return 0;
    }
}

Int TComVideoStats::getYOffSetInPU(Int d, Int PUIdx, PartSize partSize){
    Int base_h = 64 >> d;
    
    switch(partSize){
        case SIZE_2NxN:
            return ((PUIdx == 0) ? 0 : (base_h >> 1));
        case SIZE_NxN:
            return ((PUIdx == 0 or PUIdx == 1) ? 0 : (base_h >> 1));
        case SIZE_2NxnU:
            return ((PUIdx == 0) ? 0 : (base_h >> 2 ));
        case SIZE_2NxnD:
            return ((PUIdx == 0) ? 0: (base_h - (base_h >> 2)));
        default:
            return 0;
    }
}