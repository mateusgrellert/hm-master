#ifndef _TComAnalytics_H
#define	_TComAnalytics_H

#include "MyTypedef.h"

class TComAnalytics {

public:
    
    static TComDataCU *currCU;
    
    static Bool encodingStarted;
    
    static UInt currDepth;
    static UInt currPartIdx;
    static UInt currMode;
    
    static Int frameWidth;
    static Int frameHeight;
    static Int frameNum;
    
    static Double sadCount[MAX_CU_DEPTH][2];
    static Double sseCount[MAX_CU_DEPTH][2];
    static Double satdCount[MAX_CU_DEPTH][2];
    
    static Double addCount[MAX_CU_DEPTH][2];
    static Double subCount[MAX_CU_DEPTH][2];
    static Double mulCount[MAX_CU_DEPTH][2];
    static Double divCount[MAX_CU_DEPTH][2];
    
    static Double transfCount[MAX_CU_DEPTH][2];
    static Double transfCountTotal[MAX_CU_DEPTH][2];
    
    static Int mvList[40][25][64][2]; // [0] x*y/(64*64), [1] partIdx/8, [2] mv_h, mv_v
    static Double puCount[MAX_CU_DEPTH][8];
    static Double modeCount[MAX_CU_DEPTH][3];
    static Double totalPUCount;
    static std::ofstream outFile;



    
    TComAnalytics();
    static Void init();
    static Void setCU (TComDataCU *&cu, UInt partIdx);
    static Void setResolution(Int w, Int h) {frameWidth = w; frameHeight = h;};
    static Void setFrameCount(Int n) {frameNum = n;};
    static Void setCurrDepth(UInt d);
    static Void setMv(TComDataCU *&cu, UInt partIdx, Int mvH, Int mvV);
    static Void analyze();
    
    static Int calcPUNumber(PartSize s);
    static Int getPUWidth(Int d, Int PUIdx, PartSize partSize);
    static Int getPUHeight(Int d, Int PUIdx, PartSize partSize);
    static Int getXOffSetInPU(Int d, Int PUIdx, PartSize partSize);
    static Int getYOffSetInPU(Int d, Int PUIdx, PartSize partSize);
    
    static Void incModeCount(UInt mode, UInt d);
    static Void incPUCount(PartSize part, UInt d);
    
    static Void incSadCount(UInt num);
    static Void incSseCount(UInt num);
    static Void incSatdCount(UInt num);
    
    static Void incTransformsCount(UInt w, UInt h);

    static Void incAdds(UInt num);
    static Void incSubs(UInt num);
    static Void incMults(UInt num);
    static Void incDivs(UInt num);
    
    static Void report();
    static Void resetStats();
    static Void resetStatsTotal();

};

#endif	/* _TComAnalytics_H */

