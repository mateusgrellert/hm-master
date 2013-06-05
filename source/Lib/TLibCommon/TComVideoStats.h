/* 
 * File:   TComVideoStats.h
 * Author: grellert
 *
 * Created on May 22, 2013, 11:45 AM
 */

#ifndef TCOMVIDEOSTATS_H
#define	TCOMVIDEOSTATS_H
#include "MyTypedef.h"

class TComVideoStats {

public:
    
    static std::fstream rawYUV;
    static std::fstream statsYUV;
    
    static Char lumaPic[1600][2560];
    static Char chromaBPic[1600/2][2560/2];
    static Char chromaRPic[1600/2][2560/2];

    static UInt videoWidth, videoHeight;
    static std::string videoPath;
    
    static Bool firstFrame;
    
    TComVideoStats();
    static Void init();
    static Void setVideoStats(std::string path, UInt w, UInt h) {videoPath = path; videoWidth = w; videoHeight = h;};
    static Void openYUV();
    static Void loadPics();
    static Void writeStatsVideo();
    static Void printStatsInPics(UInt puX, UInt puY, UInt puW, UInt puH, UInt currMode,UInt mvHor,UInt mvVer);

    static Void closeYUV();
    
};
    

#endif	/* TCOMVIDEOSTATS_H */

