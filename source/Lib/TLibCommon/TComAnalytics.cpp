/* 
 * File:   TComAnalytics.cpp
 * Author: grellert
 * 
 * Created on november 13, 2011, 3:24 PM
 */

#include "TComAnalytics.h"
#include "TLibCommon/TypeDef.h"

TComDataCU *TComAnalytics::currCU;

Bool TComAnalytics::encodingStarted;

UInt TComAnalytics::currDepth;
UInt TComAnalytics::currPartIdx;
UInt TComAnalytics::currMode;

Int TComAnalytics::frameWidth;
Int TComAnalytics::frameHeight;
Int TComAnalytics::frameNum;
Int TComAnalytics::mvList[40][25][64][2]; // [0] x*y/(64*64), [1] partIdx/8, [2] mv_h, mv_v

Double TComAnalytics::sadCount[MAX_CU_DEPTH][2]; // [0] current frame, [1] total
Double TComAnalytics::sseCount[MAX_CU_DEPTH][2];
Double TComAnalytics::satdCount[MAX_CU_DEPTH][2];

Double TComAnalytics::addCount[MAX_CU_DEPTH][2];
Double TComAnalytics::subCount[MAX_CU_DEPTH][2];
Double TComAnalytics::mulCount[MAX_CU_DEPTH][2];

Double TComAnalytics::transfCount[MAX_CU_DEPTH][2];

Double TComAnalytics::puCount[MAX_CU_DEPTH][8];

Double TComAnalytics::modeCount[MAX_CU_DEPTH][3];

Double TComAnalytics::totalPUCount;

std::ofstream TComAnalytics::outFile;
std::ofstream TComAnalytics::hsvFile;
std::ofstream TComAnalytics::RDTimeFile;
std::ofstream TComAnalytics::avgRDTimeFile;

/*! \brief Analytics class initializer
 */
Void TComAnalytics::init() {

    currCU = NULL;
    currDepth = 0;
    currPartIdx = 0;
    currMode = 0;
    hsvFile.open("frame_stats.hsv",std::ofstream::out);
    encodingStarted = false; // true if best mv only
    
    totalPUCount = 0.0;
    
    for(int i = 0; i < MAX_CU_DEPTH; i++){
         for(int j = 0; j < 2; j++){
        
            sadCount[i][j] = 0.0;
            sseCount[i][j] = 0.0;
            satdCount[i][j] = 0.0;

            addCount[i][j] = 0.0;
            subCount[i][j] = 0.0;
            mulCount[i][j] = 0.0;
            transfCount[i][j] = 0.0;

         }
        
        for (int j = 0; j < 3; j++){
            modeCount[i][j] = 0.0;
        }
        
        for(int j = 0; j < 8; j++){
            puCount[i][j] = 0.0;
        }
    }
    
}

Void TComAnalytics::report(){
    string PUMap[] = {"2Nx2N", "2NxN", "Nx2N", "NxN", "2NxnU", "2NxnD", "nLx2N", "nRx2N"};
    outFile.open("analyticsReport.csv",ofstream::out);

    for(int i = 0; i < 8; i++)
        outFile <<  ";" << PUMap[i];
    outFile << ";" << endl;
    
    for(int i = 0; i < g_uiMaxCUDepth; i++){
        outFile << i << ";";
        for(int j = 0; j < 8; j++){
            outFile << puCount[i][j]/totalPUCount << ";"; 
        }
        outFile << endl;
    }
    
    outFile << endl << endl;
    
    outFile << ";INTER;INTRA;SKIP;" << endl;
    
    for(int i = 0; i < g_uiMaxCUDepth; i++){
        outFile << i << ";";
        for(int j = 0; j < 3; j++){
            outFile << modeCount[i][j]/totalPUCount << ";"; 
        }
        outFile << endl;
    }
    
    outFile << endl << endl;
    
    outFile << ";kSAD operations;kSSE operations;kSATD operations;kTransforms;";
    outFile << "mADD operations;mSUB operations;mMUL operations;" << endl;
    
    for(int i = 0; i < g_uiMaxCUDepth; i++){
        outFile << i << ";" << sadCount[i][1]*1000.0 << ";";
        outFile << sseCount[i][1]*1000.0 << ";";
        outFile << satdCount[i][1]*1000.0 << ";";
        outFile << transfCount[i][1]*1000.0 << ";";

        outFile << addCount[i][1] << ";";
        outFile << subCount[i][1] << ";";
        outFile << mulCount[i][1] << endl;
    }   
    
}

Void TComAnalytics::printRDStats(UInt bits, Double y, Double u, Double v, Double et){
    if(!RDTimeFile.is_open()){
        RDTimeFile.open("RDTime.csv", ofstream::out);
        RDTimeFile << "Bits;" << "Y;" << "U;" << "V;" << "ET" << endl;
    }
    RDTimeFile << bits << ";" << y  << ";" << u << ";" << v << ";" << et << endl;
    if (et < 0){
        if(!avgRDTimeFile.is_open())       
            avgRDTimeFile.open("averageRDTime.csv", ofstream::out);
        avgRDTimeFile << bits << ";" << y  << ";" << u << ";" << v << ";";
    }
   
}


Void TComAnalytics::setCU(TComDataCU*& cu, UInt partIdx){
    currCU = cu;
    currPartIdx = partIdx;
    
    if (cu->isSkipped(partIdx))
        currMode = 2;
    else
        currMode = cu->getPredictionMode(partIdx);
        
    currDepth = cu->getDepth(partIdx);
   
}

Void TComAnalytics::setMv(TComDataCU *&cu, UInt partIdx, Int mvH, Int mvV){
 
    if(!encodingStarted) return; // skip mvs during compressing stage
    
    Int x = cu->getCUPelX();
    Int y = cu->getCUPelY();
    mvList[x/64][y/64][partIdx/4][0] = mvH;
    mvList[x/64][y/64][partIdx/4][1] = mvV;
    

}

Void TComAnalytics::setCurrDepth(UInt d){
    currDepth = d;
}

Void TComAnalytics::analyze(){
    Int n_PU = calcPUNumber(currCU->getPartitionSize(currPartIdx));
    Int puX, puY, puW, puH;
    Int n_PURow = 64/4;
    Int d = currCU->getDepth(currPartIdx);
    string modeStr[] = {"INTER", "INTRA", "SKIP"};
    
    PartSize partSize = currCU->getPartitionSize(currPartIdx);
    
    for(int puIdx = 0; puIdx < n_PU; puIdx++){
        puW = getPUWidth(d,puIdx, partSize);
        puH = getPUHeight(d,puIdx, partSize);

        UInt partIdx = g_auiZscanToRaster[currPartIdx];

        puX = currCU->getCUPelX() + (4*(partIdx % n_PURow));
        puX += getXOffSetInPU(d, puIdx, partSize);

        puY = currCU->getCUPelY() + (4*floor(partIdx/n_PURow));
        puY += getYOffSetInPU(d, puIdx, partSize);
        hsvFile << puX << ";" << puY << ";" << puW << ";" << puH << ";" << modeStr[currMode] << endl;

    }
    
    incPUCount(currCU->getPartitionSize(currPartIdx), currDepth);
    incModeCount(currMode, currDepth);
}

Void TComAnalytics::incPUCount(PartSize part, UInt d){
    puCount[d][part] += (Double) ((2 << (5-d)) * (2 << (5-d))/4096.0);
    totalPUCount += (Double) ((2 << (5-d)) * (2 << (5-d))/4096.0);
}

Void TComAnalytics::incModeCount(UInt mode, UInt d){
    modeCount[d][mode] += (Double) ((2 << (5-d)) * (2 << (5-d))/4096.0);
}

Void TComAnalytics::incSadCount(UInt num){
    sadCount[currDepth][0] += (Double)  num/(4096*1000000.0);
    sadCount[currDepth][1] += (Double)  num/(4096*1000000.0);
}


Void TComAnalytics::incSseCount(UInt num){
    sseCount[currDepth][0] += (Double) num/(4096*1000000.0);
    sseCount[currDepth][1] += (Double) num/(4096*1000000.0);
}

Void TComAnalytics::incSatdCount(UInt num){
    satdCount[currDepth][0] += (Double) num/(4096*1000000.0);
    satdCount[currDepth][1] += (Double) num/(4096*1000000.0);
}


Void TComAnalytics::incTransformsCount(UInt w, UInt h){
    transfCount[currDepth][0] += (Double) (w*h)/(4096*1000000.0);
    transfCount[currDepth][1] += (Double) (w*h)/(4096*1000000.0);
}

Void TComAnalytics::incAdds(UInt num){
    addCount[currDepth][0] += (Double) num/(1000000.00);
    addCount[currDepth][1] += (Double) num/(1000000.00);
}

Void TComAnalytics::incSubs(UInt num){
    subCount[currDepth][0] += (Double) num/(1000000.00);
    subCount[currDepth][1] += (Double) num/(1000000.00);
}

Void TComAnalytics::incMults(UInt num){
    mulCount[currDepth][0] += (Double) num/(1000000.00);
    mulCount[currDepth][1] += (Double) num/(1000000.00);
}

Void TComAnalytics::resetStats(){
    
    for(int i = 0; i < MAX_CU_DEPTH; i++){       
        sadCount[i][0] = 0.0;
        sseCount[i][0] = 0.0;
        satdCount[i][0] = 0.0;
        
        addCount[i][0] = 0.0;
        subCount[i][0] = 0.0;
        mulCount[i][0] = 0.0;
        
        transfCount[i][0] = 0.0;
    }
}

Int TComAnalytics::calcPUNumber(PartSize s){
    switch (s){
        case SIZE_2Nx2N: return 1;
        case SIZE_2NxN:
        case SIZE_Nx2N:
        case SIZE_2NxnU:
        case SIZE_2NxnD:
        case SIZE_nRx2N:
        case SIZE_nLx2N: return 2;
        default: return 4;
    }
}
Int TComAnalytics::getPUWidth(Int d, Int PUIdx, PartSize partSize){
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

Int TComAnalytics::getPUHeight(Int d, Int PUIdx, PartSize partSize){
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

Int TComAnalytics::getXOffSetInPU(Int d, Int PUIdx, PartSize partSize){
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

Int TComAnalytics::getYOffSetInPU(Int d, Int PUIdx, PartSize partSize){
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

Void TComAnalytics::setPOC(UInt poc){
    if (poc > 0)
        hsvFile << "EOP" << endl;
    hsvFile << "POC;" << poc << endl;
}