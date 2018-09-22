#include "MovementController.h"

void MovementController::setup(int cableNum, int x1, int y1, int w, int h, int x_range, int y_range, int numCtrl){
    
    max_x_pos = x_range;
    max_y_pos = y_range;
    
    numOfControllers = numCtrl;
    
    curveControls.setup("CurveReadWrite", "settings.xml", x1+w+10, y1);
    for(int j=0; j< numOfControllers; j++){
        for(int i=0; i< 5; i++){
            
            ofParameter<int> a;
            if(i==0){
                a.set("Option",1,0,3); //lx,ly,rx,ry
            }
            else if(i==1){
                a.set("Width",10,0,400); //lx,ly,rx,ry
            }
            else if(i==2){
                a.set("HeightA",40,0,100); //lx,ly,rx,ry
            }
            else if(i==3){
                a.set("HeightB",max_x_pos/2,0,max_x_pos); //lx,ly,rx,ry
            }
            else if(i==4){
                a.set("Offest",100,0,1000);//lx,ly,rx,ry
            }
            
            else{
                a.set("--",100,0,1000);//lx,ly,rx,ry
            }
            curves.push_back(a);
            
            
        }
        ofPolyline p;
        trail.push_back(p);
        
        curveCoord.push_back(ofPoint(x1,y1 + j*h));
    }
    for(int j=0; j< numOfControllers*5; j++){
        curveControls.add(curves[j]);
    }
    
    
    int eW = 120;
    
    curveControls.setSize(eW, 100);
    curveControls.setWidthElements(eW);
    
    
    curveW = w;
    curveH = h;
    
    numOfCables = cableNum;
    
}

vector<ofPoint> MovementController::getPoints(){
    vector<ofPoint> p;
    p.clear();
    for(int i=0; i < numOfCables; i++){
        if(curves[0] == 1){
            p.push_back(trail[0].getPointAtPercent((float)i / numOfCables));
            p[i].y = p[i].y * curves[2] + (curves[3] - max_x_pos/2)*4;
            p[i].x = p[i].x;
        }else if(curves[0] == 2){
            if(i%2 == 0){
                p.push_back(trail[0].getPointAtPercent((float)i / numOfCables));
                p[i].y = p[i].y * curves[2] + (curves[3] - max_x_pos/2)*4;
                p[i].x = p[i].x;
            }else{
                p.push_back(trail[1].getPointAtPercent((float)i / numOfCables));
                p[i].y = p[i].y * curves[2+5] + (curves[3+5] - max_x_pos/2)*4;
                p[i].x = p[i].x;
            }
            
        }else if(curves[0] == 3){
            if(!(i%2 == 0)){
                p.push_back(trail[0].getPointAtPercent((float)i / numOfCables));
                p[i].y = p[i].y * curves[2] + (curves[3] - max_x_pos/2)*4;
                p[i].x = p[i].x;
            }else{
                p.push_back(trail[1].getPointAtPercent((float)i / numOfCables));
                p[i].y = p[i].y * curves[2+5] + (curves[3+5] - max_x_pos/2)*4;
                p[i].x = p[i].x;
            }
            
        }else if(curves[5] == 1){
            p.push_back(trail[1].getPointAtPercent((float)i / numOfCables));
            p[i].y = p[i].y * curves[2+5] + (curves[3+5] - max_x_pos/2)*4;
            p[i].x = p[i].x;
        }
        else{
            p.push_back(trail[0].getPointAtPercent((float)i / numOfCables));
            p[i].y = p[i].y * curves[2+5] + (curves[3+5] - max_x_pos/2)*4;
            p[i].x = p[i].x;
        }
    }
    return p;
}

void MovementController::setPoints(int whichCtrl, int op, int a, int b, int c,int d){
    if(whichCtrl < numOfControllers){
        curves[0+(whichCtrl*5)] = op;
        curves[1+(whichCtrl*5)] = a;
        curves[2+(whichCtrl*5)] = b;
        curves[3+(whichCtrl*5)] = c;
        curves[4+(whichCtrl*5)] = d;
    }
}

int MovementController::getOption(int op){
    if(op == 0){
        return curves[0];
    }
    return curves[5];
}

void MovementController::setOption(int whichCtrl, int op){
    if(whichCtrl == 0){
        curves[0] = op;
    } else if (whichCtrl == 1){
        curves[5] = op;
    }
    
}

void MovementController::update(){
    if(curves[0] == 2){
        curves[5] = 3;
    }else if (curves[0] == 3){
        curves[5] = 2;
    }
    
    for(int j=0; j< numOfControllers; j++){
        
        x=0;
        trail[j].clear();
        
        
        angle = 0;
        increment = (float)curves[1+(j*5)]/1000/TWO_PI;
        for(int i=0; i< curveW;i++){
            x++;
            angle+=increment;
            if (angle>=TWO_PI) { //if the angle is more than or equal to two PI (a full rotation measured in Radians) then make it zero.
                angle=0;
            }
            y=(curveH/2)+ (curveH/2)*sin(angle+ curves[4+(j*5)]);
            trail[j].addVertex(ofPoint(curveCoord[j].x+ x,curveCoord[j].y+ y));
        }
    }
};

void MovementController::draw(){
    // ofEnableSmoothing();
    
    ofSetColor(255,0,0);
    for(int j=0; j< numOfControllers; j++){
        ofNoFill();
        ofDrawRectangle(curveCoord[j].x, curveCoord[j].y, curveW, curveH);
        ofSetColor(20);
        trail[j].draw();
        for(int i=0; i< numOfCables; i++){
            ofSetColor(0, 140, 255);
            ofPoint p = trail[j].getPointAtPercent((float)i/numOfCables);
            ofDrawCircle(p,5);
            ofDrawCircle(ofGetMouseX(),ofGetMouseY(),5);
        }
    }
    // ofDisableSmoothing();
    curveControls.draw();
    
};



