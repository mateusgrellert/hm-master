#include "TComComplexityManagement.h"
#include "TComCABACTables.h"
#include <cmath>

using namespace std;

Int64 TComComplexityBudgeter::countBudget[4];

vector<vector <cuStats> > TComComplexityBudgeter::history;
vector<vector <config> > TComComplexityBudgeter::configMap;
vector<Double> TComComplexityBudgeter::weightsVector;
UInt TComComplexityBudgeter::origGop;
UInt TComComplexityBudgeter::picWidth;
UInt TComComplexityBudgeter::picHeight;
UInt TComComplexityBudgeter::intraPeriod;
UInt TComComplexityBudgeter::maxCUDepth;
UInt TComComplexityBudgeter::maxTUDepth;
UInt TComComplexityBudgeter::maxNumRefPics;
UInt TComComplexityBudgeter::nEncoded;
UInt TComComplexityBudgeter::currPoc;
Int TComComplexityBudgeter::searchRange;
Bool TComComplexityBudgeter::hadME;
Bool TComComplexityBudgeter::testAMP;
Bool TComComplexityBudgeter::restore_AMVPInfo;
Double TComComplexityBudgeter::frameBudget;
std::ofstream TComComplexityBudgeter::budgetFile;
AMVPInfo* TComComplexityBudgeter::keep_AMVP;

Void TComComplexityBudgeter::init(UInt w, UInt h, UInt gopSize, UInt intraP){
    vector<Double> directionVector;
    vector<cuStats> tempHistRow;
    vector<config> tempConfigRow;
    config conf;

    weightsVector.push_back(0); // first frame = I_Frame
    origGop = gopSize;
    intraPeriod = intraP;
    maxCUDepth = 4;
    maxTUDepth = 3;
    maxNumRefPics = 4;
    picWidth = w;
    picHeight = h;
    hadME = 1;
    testAMP = 1;
    searchRange = 64;
    restore_AMVPInfo = false;
    keep_AMVP = NULL;
    for(int i = 0; i < (w >> 6) + 1; i++){
        tempHistRow.clear();
        tempConfigRow.clear();
        for(int j = 0; j < (h >> 6) + 1; j++){
            directionVector.clear();
            conf.clear();
            // for history table 7 directions plus no motion position = 8
            for(int k = 0; k < 9; k++){
                directionVector.push_back(0.0);
            }
            tempHistRow.push_back(make_pair(-1, directionVector));
            
            // for config map
            for(int k = 0; k < 6; k++){
                conf.push_back(0);
            }
            tempConfigRow.push_back(conf);
        }
        history.push_back(tempHistRow);
        configMap.push_back(tempConfigRow);
    }
}

Double TComComplexityBudgeter::calcWeight(UInt t_layer){


    Double weights8[] = {1.5,0.5,0.83,0.5,1.16,0.5,0.83,0.5,1.5};
    Double weights4[] = {1.3,0.9,1.3,0.5};

   // cout << weights[currPoc % (origGop)] << endl;
    if(origGop == 8)
        return weights8[currPoc % (origGop)];
    else
        return weights4[currPoc % (origGop)];
    //return (a1 + d*(5-t_layer));
}

void TComComplexityBudgeter::setDepthHistory(TComDataCU *&pcCU, UInt pu_idx){
      
    //TODO: implement CU extrapolation
    UInt x = pcCU->getCUPelX();
    UInt y = pcCU->getCUPelY();
    UInt d = pcCU->getDepth(pu_idx);
    Int xP, yP, nPSW, nPSH;
    Double theta = -1;
    Int idx;
    pcCU->getPartPosition(pu_idx, xP, yP, nPSW, nPSH);
    
    Int vectorX = pcCU->getCUMvField(REF_PIC_LIST_0)->getMv(pu_idx).getHor() >> 2;
    Int vectorY = pcCU->getCUMvField(REF_PIC_LIST_0)->getMv(pu_idx).getVer() >> 2;

    history[x >> 6][y >> 6].first = d;

    if((pcCU->isIntra(pu_idx)) or (pcCU->isSkipped(pu_idx))){
        history[x >> 6][y >> 6].second[8] += ((nPSW*nPSH)/(64.0*64)); // no movement idx
    }
    else{
        // test if PU is inside LCU no motion
        if(vectorIsInCU(x,y,xP,yP,vectorX,vectorY))
            history[x >> 6][y >> 6].second[8] += ((nPSW*nPSH)/(64.0*64)); // no movement idx
        else{
            theta = calcMotionAngle(xP, yP, vectorX + xP, vectorY + yP);
            if(theta == 360) theta = 0;
            idx = (Int) (floor(theta/45.0));
            history[x >> 6][y >> 6].second[idx] += ((nPSW*nPSH)/(64.0*64));
        }
    }
    
}

void TComComplexityBudgeter::updateConfig(TComDataCU*& cu){
    Int x = cu->getCUPelX() >> 6;
    Int y = cu->getCUPelY() >> 6;

    maxCUDepth = configMap[x][y][0];
    maxTUDepth = configMap[x][y][1];
    searchRange = configMap[x][y][2];
    maxNumRefPics = configMap[x][y][3];

    testAMP = configMap[x][y][4];
    hadME = configMap[x][y][5];
}

void TComComplexityBudgeter::resetConfig(TComDataCU*& cu){
    maxCUDepth = 4;
    maxTUDepth = 3;
    searchRange = 64;
    maxNumRefPics = 4;
    testAMP = 1;
    hadME = 1;
}

Bool TComComplexityBudgeter::isConstrained(){

    if(nEncoded % MANAGE_GOP < NUM_RD_FRAMES)
        return false;  
  //  if(nEncoded > 15)
  //      return false;
    
    return true;
}

Void TComComplexityBudgeter::uniformBudget(){ 
    Int CUConfig;
        Double estCycleCount = 0.0;

    if (estimateCycleCount(3,0) <= frameBudget)
        CUConfig = 3;
    else if (estimateCycleCount(2,0) <= frameBudget)
        CUConfig = 2;
    else if (estimateCycleCount(1,0) <= frameBudget)
        CUConfig = 1;
    else
        CUConfig = 0;
    
    
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (history[i][j].first == -1)
                continue;
            setConfigMap(i,j,CUConfig);
            estCycleCount = updateEstimationAndStats(estCycleCount,-1,CUConfig);
        }
    }
}

Void TComComplexityBudgeter::topDownBudget(){
    Double estCycleCount = 0.0;
    
    Int depth = 3;
   
    
    do{
        for (Int demote_depth = 0; demote_depth <= 3; demote_depth++){
            for(int i = 0; i < history.size(); i++){
                for(int j = 0; j < history[0].size(); j++){
                    if (history[i][j].first == -1)
                        continue;
                    
                    
                    if (depth == 2 and history[i][j].first == demote_depth){
                        setConfigMap(i,j,2);
                        estCycleCount = updateEstimationAndStats(estCycleCount,3,2);
                    }
                    else if (depth == 1 and history[i][j].first == demote_depth){
                        setConfigMap(i,j,1);
                        estCycleCount = updateEstimationAndStats(estCycleCount,2,1);
                    }
                    else if (depth == 0 and history[i][j].first == demote_depth){
                        setConfigMap(i,j,0);
                        estCycleCount = updateEstimationAndStats(estCycleCount,1,0);
                    }
                    else if (depth == 3 and demote_depth == 0)
                    {
                        setConfigMap(i,j,3);
                        estCycleCount = updateEstimationAndStats(estCycleCount,-1,3);
                    }
                    if(estCycleCount <= frameBudget and depth < 3)
                        break;
                }           

            }
        }
        depth -= 1;

    }while (depth >= 0 and estCycleCount > frameBudget);
}

Void TComComplexityBudgeter::bottomUpBudget(){
    Double estCycleCount = 0.0;
    
    Int depth = 0;
   
    
    do{
        for (Int promote_depth = 3; promote_depth >= 0; promote_depth--){
            for(int i = 0; i < history.size(); i++){
                for(int j = 0; j < history[0].size(); j++){
                    if (history[i][j].first == -1)
                        continue;
                   
                    if (depth == 3 and history[i][j].first == promote_depth){
                        setConfigMap(i,j,3);
                        estCycleCount = updateEstimationAndStats(estCycleCount,2,3);

                    }
                    else if (depth == 2 and history[i][j].first == promote_depth){
                        setConfigMap(i,j,2);
                        estCycleCount = updateEstimationAndStats(estCycleCount,1,2);
                    }
                    else if (depth == 1 and history[i][j].first == promote_depth){
                        setConfigMap(i,j,1);
                        estCycleCount = updateEstimationAndStats(estCycleCount,0,1);
                    }
                    else if (depth == 0 and promote_depth == 3){
                        setConfigMap(i,j,0);
                        estCycleCount = updateEstimationAndStats(estCycleCount,-1,0);
                    }   
                    if(frameBudget <= estCycleCount and depth > 0)
                        break;
                }           
            }
        }
        //cout << estCycleCount << endl;
        depth += 1;

    }while (depth < 4 and frameBudget > estCycleCount);
}
        
Void TComComplexityBudgeter::knapSackBudget(){return;}

Void TComComplexityBudgeter::ICIPBudget(){
    Double estCycleCount = 0.0;

    // first step - set HIGH and LOW configs and estimate cycle count
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (history[i][j].first == -1)
                        continue;
                   
            if(history[i][j].first == 0 or (estCycleCount > frameBudget)){ //set LOW budget!
                setConfigMap(i,j,0);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,0);
                history[i][j].first = 0; // there should be some other way to account this without modifying the history
            }
            
            else if(history[i][j].first == 3){ //set HIGH budget!
                setConfigMap(i,j,3);
                estCycleCount = updateEstimationAndStats(estCycleCount,-1,3);
            }
        }
    }
            
    // second step - set MEDIUM budget according to computation available
    for(int i = 0; i < history.size(); i++){
        for(int j = 0; j < history[0].size(); j++){
            if (history[i][j].first == -1)
                   continue;
                   
            if(history[i][j].first == 1 or history[i][j].first == 2){
                if(estCycleCount < frameBudget){ // still available - spend some effort
                    setConfigMap(i,j,2);
                    estCycleCount = updateEstimationAndStats(estCycleCount,-1,2);
                }
                else{                    // no more available - use low effort
                    setConfigMap(i,j,1);
                    estCycleCount = updateEstimationAndStats(estCycleCount,-1,1);
                }                
            }   
        }
    }
        //  loop : reset LOW to HIGh If B > est
    for(int i = 0; i < history.size() and estCycleCount < frameBudget; i++){
        for(int j = 0; j < history[0].size() and estCycleCount < frameBudget; j++){
             if (history[i][j].first == -1)
                        continue;
                   
            // apply high config.to med-high budgeted CUs
            if(configMap[i][j][0] == 3){ 
                setConfigMap(i,j,3);
                estCycleCount = updateEstimationAndStats(estCycleCount,2,3);
            }            
            
            // apply med-high config.to med-low and low budgeted CUs
            if(configMap[i][j][2] == 16 or history[i][j].first == 0){ 
                if(configMap[i][j][2] == 16){ // means it was med-L
                    setConfigMap(i,j,2);
                    estCycleCount = updateEstimationAndStats(estCycleCount,1,2);

                }
                else if(history[i][j].first == 0){
                    setConfigMap(i,j,2);
                    estCycleCount = updateEstimationAndStats(estCycleCount,0,2);
                    
                }
            }
        }
    }
    
    // last loop : reset high to low if est > B
    for(int i = 0; i < history.size() and estCycleCount > frameBudget; i++){
        for(int j = 0; j < history[0].size() and estCycleCount > frameBudget; j++){
            if (history[i][j].first == -1)
                  continue;
                   
            // apply med-low config.to high budgeted CUs
            if(history[i][j].first == 3){ 
                    setConfigMap(i,j,1);
                    estCycleCount = updateEstimationAndStats(estCycleCount,3,1);
            }            
            
            // apply low config.to med-low and med-high budgeted CUs
            if(history[i][j].first == 2 or history[i][j].first == 1){ 
                if(configMap[i][j][0] == 3) // means it was med-H
                    estCycleCount = updateEstimationAndStats(estCycleCount,2,0);
                else
                    estCycleCount = updateEstimationAndStats(estCycleCount,1,0);

                    setConfigMap(i,j,0);

            }
            
            //reset history for future refs
            for(int k = 0; k < 9; k++)
                history[i][j].second[k] = 0.0;
        }
    }

    
}

// conf 0 = low, 1 = medL, 2 = medH, 3 = high

Double TComComplexityBudgeter::updateEstimationAndStats(Double est, UInt old, UInt neww){
    Double nCU = (picHeight*picWidth)/(64.0*64.0);
    UInt depth_map[] = {1,2,3,3}; //maximum depth allowed in each profile
   
    UInt d_old, d_new;
        
    if (old != -1)
        d_old = depth_map[old];
    d_new = depth_map[neww];

    if (old != -1)
        countBudget[old]--;
    countBudget[neww]++;
       
    if (old != -1)
        est -= estimateCycleCount(d_old,old)/nCU;
    est += estimateCycleCount(d_new,neww)/nCU;
    
    return est;
}

Void TComComplexityBudgeter::setConfigMap(UInt i, UInt j, UInt prof){
    switch (prof){
        case 3:                         
            configMap[i][j][0] = 4; // Max CU Depth
            configMap[i][j][1] = 3; // Max TU Depth
            configMap[i][j][2] = 64; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 1; // AMP
            configMap[i][j][5] = 1; // Had ME 
            break;
        case 2:
            configMap[i][j][0] = 4; // Max CU Depth
            configMap[i][j][1] = 3; // Max TU Depth
            configMap[i][j][2] = 32; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 0; // AMP
            configMap[i][j][5] = 1; // Had ME  
                        break;

        case 1:
            configMap[i][j][0] = 3; // Max CU Depth
            configMap[i][j][1] = 1; // Max TU Depth
            configMap[i][j][2] = 16; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 0; // AMP
            configMap[i][j][5] = 1; // Had ME
                        break;

        default:
            configMap[i][j][0] = 2; // Max CU Depth
            configMap[i][j][1] = 1; // Max TU Depth
            configMap[i][j][2] = 16; // SR
            configMap[i][j][3] = 4; // Max Num Ref Pics
            configMap[i][j][4] = 0; // AMP
            configMap[i][j][5] = 1; // Had ME  
                        break;

    }

}

Double TComComplexityBudgeter::estimateCycleCount(UInt d, UInt conf){
    Double count = 0;
    Double factor = 0.0;
    switch (conf){
        case 3: factor = 1.0; break;
        case 2: factor = 0.6; break;
        case 1: factor = 0.33; break;
        case 0: factor = 0.1; break;
        default: factor = 1.0; break;
    }
    
    for(int i = 0; i <= d; i++){
        count += (TComAnalytics::sadCount[i][0]) * CYCLES_SAD;
        count += (TComAnalytics::sseCount[i][0]) * CYCLES_SSE;
        count += (TComAnalytics::satdCount[i][0]) * CYCLES_SATD;
        count += (TComAnalytics::transfCount[i][0]) * CYCLES_TRANSF;
    }
    
    return (count*factor);
}

Void TComComplexityBudgeter::distributeBudget(){
    UInt alg = TComComplexityController::budgetAlg;
    resetBudgetStats();

    switch(alg){
        case 0: uniformBudget(); break;
        case 1: topDownBudget(); break;
        case 2: bottomUpBudget(); break;
        case 3: knapSackBudget(); break; 
        case 4: ICIPBudget(); break;
        default: uniformBudget(); break;
    }
    printBudgetStats();

}

Void TComComplexityBudgeter::resetBudgetStats(){

    for (int i = 0; i < 4; i++)
         countBudget[i] = 0;
}

Void TComComplexityBudgeter::printBudgetStats(){
    
    if(!budgetFile.is_open())
        budgetFile.open("budgetDist.csv",ofstream::out);

    Double total = 0.0;
    for (int i = 0; i < 4; i++)
        total += countBudget[i];
    
    for (int i = 0; i < 4; i++)
        budgetFile << (Double) countBudget[i]/total << ";";

    budgetFile << endl;
}

Void TComComplexityBudgeter::setFrameBudget(Double budget, UInt t_layer){
    frameBudget = budget*calcWeight(t_layer);
}

Void TComComplexityBudgeter::setEncodedCount(UInt n){
    nEncoded = n;
}

Double TComComplexityBudgeter::calcMotionAngle(Int x1, Int y1, Int x2, Int y2){
    Double theta;
    
    // calculate theta = arctan(co/ca)
    theta = atan(abs(y2-y1)/abs(x2-x1+0.01))*180/M_PI;

    if((x2 - x1) >= 0 and (y2 - y1) < 0){ // second quadrat
        theta = 180 - theta;
    }
    else if((x2 - x1) < 0 and (y2 - y1) <= 0){ // third quadrat
        theta = 270 - (theta - 90);
    }
    else if((x2 - x1) >= 0 and (y2 - y1) > 0){
        theta = 270 + (theta - 90);
    }
    
    return theta;
}

Void TComComplexityBudgeter::calcDominantDirection(){
    Double temp_max;
    Int domDir = 8;
    for(int i = 0; i < history.size() -1; i++){
        for(int j = 0; j < history[0].size() -1; j++){
            temp_max = 0;
            for(int k = 0; k < 9; k++){
                // 1 - calculate dominant direction
                if(temp_max < history[i][j].second[k]){
                    temp_max = history[i][j].second[k];
                    domDir = k;
                }
                // 2 - switch depths
                switch(domDir){
                    case 0:
                        if(i+1 < picWidth)
                            history[i+1][j].first = max(history[i][j].first, history[i+1][j].first);
                        break;
                    case 1:
                        if(i+1 < picWidth)
                            history[i+1][j].first = max(history[i][j].first, history[i+1][j].first);
                        if((i+1 < picWidth) and (j-1  >= 0))
                            history[i+1][j-1].first = max(history[i][j].first, history[i+1][j-1].first);
                        if(j-1 >= 0)
                            history[i][j-1].first = max(history[i][j].first, history[i][j-1].first);
                        break;
                    case 2:
                        if(j-1 >= 0)
                            history[i][j-1].first = max(history[i][j].first, history[i][j-1].first);
                        break;
                    case 3:
                        if(j-1 >= 0)
                            history[i][j-1].first = max(history[i][j].first, history[i][j-1].first);
                        if((i-1  >= 0) and (j-1  >= 0))
                            history[i-1][j-1].first = max(history[i][j].first, history[i-1][j-1].first);
                        if(i-1  >= 0)
                            history[i-1][j].first = max(history[i][j].first, history[i-1][j].first);
                        break;
                    case 4:
                        if(i-1 >= 0)
                            history[i-1][j].first = max(history[i][j].first, history[i-1][j].first);
                        break;
                    case 5:
                        if(i-1 >= 0)
                            history[i-1][j].first = max(history[i][j].first, history[i-1][j].first);
                        if((i-1  >= 0) and (j+1 < picHeight))
                            history[i-1][j+1].first = max(history[i][j].first, history[i-1][j+1].first);
                        if(j+1 < picHeight)
                            history[i][j+1].first = max(history[i][j].first, history[i][j+1].first);
                        break;
                    case 6:
                        if(j+1 < picHeight)
                            history[i][j+1].first = max(history[i][j].first, history[i][j+1].first);
                        break;
                    case 7:
                        if(j+1 < picHeight)
                            history[i][j+1].first = max(history[i][j].first, history[i][j+1].first);
                        if((i+1 < picWidth) and (j+1 < picHeight))
                            history[i+1][j+1].first = max(history[i][j].first, history[i+1][j+1].first);
                        if(i+1 < picWidth)
                            history[i+1][j].first = max(history[i][j].first, history[i+1][j].first);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

Bool TComComplexityBudgeter::vectorIsInCU(Int cuX, Int cuY, Int puX, Int puY, Int vecX, Int vecY){
    
    if(vecX == 0 and vecY == 0)
        return true;
    
    if(vecX <= 0){ // left
        if (vecY <= 0) // above left
            return (( vecX <= (puX - cuX)) and ( vecY <= (puY - cuY)));
        else // below left
            return (( vecX <= (puX - cuX)) and ( vecY <= (64 - puY + cuY)));
    }
    else{ // right
        if (vecY <= 0) // above right
            return (( vecX <= (64 - puX + cuX)) and ( vecY <= (puY - cuY)));
        else // below right
            return (( vecX <= (64 - puX + cuX)) and ( vecY <= (64 - puY + cuY)));
    }
}
