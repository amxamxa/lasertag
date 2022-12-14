
#include "imageProjection.h"

//-----------------------------------------------------	
imageProjection::imageProjection(){
    
    width			= 0;
    height			= 0;
    
    scaleX			= 0;
    scaleY			= 0;
    
    toolX			= 0;
    toolY			= 0;
    
    brightness		= 100;
    bGreyscaleTexture   = false;
    
    whichToolSelected = 0;
    
    setProjectionColor(255, 255, 255);
    
}

//-----------------------------------------------------		
void imageProjection::setup(int _width, int _height){
    
    //this is the width and height of our  projected texture
    width  = _width;
    height = _height;
    
    //allocate our ofTextures - some are color - some are b&w
    
    colorTexture.allocate(width, height, GL_RGBA);
    greyscaleTexture.allocate(width, height, GL_LUMINANCE);
}

void imageProjection::setColorTexture(ofTexture & tex){
    colorTexture = tex;
    bGreyscaleTexture = false;
}
void imageProjection::setGrayTexture(ofTexture & tex){
    greyscaleTexture = tex;
    bGreyscaleTexture = true;
}

//if we have to dim the image
//projector is too bright close etc
//-----------------------------------------------------				
void imageProjection::setProjectionBrightness(float pBrightness){
    if(pBrightness > 100)pBrightness = 100;
    else if(pBrightness < 0) pBrightness = 0;
    brightness = pBrightness;
}

//for greyscale images - we can give em color!
//-----------------------------------------------------		
void imageProjection::setProjectionColor(int r, int g, int b){
    red 	= r;
    green 	= g;
    blue 	= b;
}

///////////////////////////////////////////////////////
//
//		UPDATE THE PROJECTED TEXTURE
//
///////////////////////////////////////////////////////

//------------------------------------------------------
void imageProjection::updateGreyscaleTexture(unsigned char * pixels){
    greyscaleTexture.loadData(pixels, width, height, GL_LUMINANCE);
    bGreyscaleTexture = true;
}

//------------------------------------------------------
void imageProjection::updateColorTexture(unsigned char * pixels){
    colorTexture.loadData(pixels, width, height, GL_RGB);
    bGreyscaleTexture = false;
}


///////////////////////////////////////////////////////
//
//		PROJECTION DISTORTION TOOLS
//
///////////////////////////////////////////////////////

//loads the projection quad settings from xml
//and updates the quad with the saved values
//----------------------------------------------------
void imageProjection::loadSettings(string filePath){
    //CAMERA QUAD SETTINGS;
    //SET DEFAULT VALUES HERE
    //0.0-1.0 range
    warpSrc[0].x = 0;
    warpSrc[0].y = 0;
    
    warpSrc[1].x = 0.9;
    warpSrc[1].y = 0;
    
    warpSrc[2].x = 0.9;
    warpSrc[2].y = 0.9;
    
    warpSrc[3].x = 0;
    warpSrc[3].y = 0.9;
    
    //Now we update out quad with the default
    //values and if then if the file is found
    //we overwrite with values from xml file
    QUAD.setup("QUAD_PROJ_");
    QUAD.setQuadPoints(warpSrc);
    QUAD.readFromFile(filePath);
    
    setToolDimensions(640, 360);
    
    applyQuad();
}


//sets the dimensions of the mini tool - as it is much smaller
//than the actual projected texture size
//-----------------------------------------------------		
void imageProjection::setToolDimensions(float desiredW, float desiredH){
    
    scaleX = desiredW / width;
    scaleY = desiredH / height;
    
    scaledWidth  = desiredW;
    scaledHeight = desiredH;
}

//this sets the the quad coords for our projection textures
//and does the calculations - we do it here so we don't
//have to do it in the draw command
//-----------------------------------------------------		
void imageProjection::applyQuad(){
    
    ofPoint * tmpPts = QUAD.getScaledQuadPoints(width, height);
    
    ofPoint p[4];
    p[0] = ofPoint(0, 0);
    p[1] = ofPoint(width, 0);
    p[2] = ofPoint(width, height);
    p[3] = ofPoint(0, height);
    matrix = QUAD.getHomography(p, tmpPts);
    
}

//-----------------------------------------------------
ofPoint *  imageProjection::getQuadPoints(){
    return QUAD.getQuadPoints();
}

//-----------------------------------------------------
ofPoint *  imageProjection::getQuadPointsScaled(){
    return QUAD.getScaledQuadPoints(width, height);
}

//this is for selecting the quad corners of the projection
//directly - should be hooked up to mouse down
//-----------------------------------------------------
int imageProjection::selectQuad(int x, int y, int xCheck, int yCheck, int w, int h, int hitArea){
    bool isSelected = QUAD.selectPoint(x, y, xCheck, yCheck, w, h, hitArea);
    if(isSelected)whichToolSelected = 1;
    
    return whichToolSelected;
}

//this is for updating te quad corners of the projection 
//directly - should be used on mouse drag
//-----------------------------------------------------		
bool imageProjection::updateQuad(int x, int y, int xCheck, int yCheck,  int w, int h){
    if(whichToolSelected != 1)return false;
    bool isSelected = QUAD.updatePoint(x, y, xCheck, yCheck,w, h);
    applyQuad();
    
    return isSelected;
}

//this is for manipulation the projection quad with the mini tool
//should be hooked up to mouse down 
//-----------------------------------------------------		
int imageProjection::selectMiniQuad(int x, int y, int hitArea){
    bool isSelected = QUAD.selectPoint(x, y, toolX, toolY, scaledWidth, scaledHeight, hitArea);
    if(isSelected)whichToolSelected = 2;
    
    return whichToolSelected;
}

//this is for moving one of the quad corners
//should be hooked up to mouse drag		
//-----------------------------------------------------		
bool imageProjection::updateMiniQuad(int x, int y){
    if(whichToolSelected != 2)return false;
    bool isSelected = QUAD.updatePoint(x, y, toolX, toolY, scaledWidth, scaledHeight);
    applyQuad();
    
    return isSelected;
}

//unselects the quad corner
//should be hooked up to the mouse up event
//-----------------------------------------------------		
void imageProjection::releaseAllQuads(){
    QUAD.releaseAllPoints();
    whichToolSelected = 0;
    
}


///////////////////////////////////////////////////////
//
//		THIS IS WHERE WE DRAW
//
///////////////////////////////////////////////////////

//Draws our mini quad tool with the projected texture at 
//the precomputed scale set by setToolDimensions
//-----------------------------------------------------
void imageProjection::drawMiniProjectionTool(float x, float y, bool showOutline, bool showTexture){
    
    toolX = x;
    toolY = y;
    
    ofPushMatrix();
    ofTranslate(x, y, 0);
    ofScale(scaleX, scaleY, 1.0);
    ofMultMatrix(matrix);
    ofSetColor(255, 0, 255);
    if(showOutline){
        ofNoFill();
        ofDrawRectangle(0,0,width, height);
        ofFill();
    }
    
    if(showTexture){
        ofEnableAlphaBlending();
        
        float dim = brightness *0.01;
        if(bGreyscaleTexture){
            ofSetColor((float)red, (float)green, (float)blue);
            greyscaleTexture.draw(0, 0, greyscaleTexture.getWidth(), greyscaleTexture.getHeight());
        }
        else{
            ofSetColor(255, 255, 255);
            colorTexture.draw(0, 0, colorTexture.getWidth(), colorTexture.getHeight());
        }
        ofDisableAlphaBlending();
    }
    
    ofPopMatrix();
    
    QUAD.draw(x, y, scaledWidth, scaledHeight);
}


//Draws the tool at any size
//-----------------------------------------------------
void imageProjection::drawProjectionToolHandles(float x, float y, float w, float h, bool showOutline, bool highlyVisible){
    
    float sX = w/width;
    float sY = h/height;
    
    ofPushMatrix();
    ofTranslate(x, y, 0);
    ofScale(sX, sY, 1.0);
    ofSetHexColor(0xFFFFFF);
    if(showOutline){
        ofNoFill();
        ofDrawRectangle(0,0,width, height);
        ofFill();
    }
    ofPopMatrix();
    
    if(highlyVisible){
        QUAD.draw(x, y, w, h, 255, 255, 255, 3);
    }else{
        QUAD.draw(x, y, w, h);
    }
}

//-----------------------------------------------------
void imageProjection::drawProjectionMask(float x, float y, float w, float h){
    
    ofPoint *pts = QUAD.getScaledQuadPoints(w, h);
    
    //we do a mask around the quad
    ofSetHexColor(0x000000);
    ofBeginShape();
    ofVertex(x, y);
    ofVertex(x + w, y);
    ofVertex(x + w, y+h);
    ofVertex(x , y+h);
    
    ofNextContour(true);
    ofVertex(x + pts[0].x, y + pts[0].y);
    ofVertex(x + pts[1].x, y + pts[1].y);
    ofVertex(x + pts[2].x, y + pts[2].y);
    ofVertex(x + pts[3].x, y + pts[3].y);
    ofEndShape();
}

//the texture that is projected
//-----------------------------------------------------
void imageProjection::drawProjectionTex(float x, float y, float w, float h){
    
    float sX = w/width;
    float sY = h/height;
    ofPushStyle();
    {
        ofEnableAlphaBlending();
        {
            ofPushMatrix();
            {
                ofTranslate(x,y,0);
                ofScale(sX, sY, 1);
                ofMultMatrix(matrix);
                
                float dim = brightness *0.01;
                
                if(bGreyscaleTexture){
                    ofSetColor((float)red * dim, (float)green * dim, (float)blue * dim);
                    greyscaleTexture.draw(0, 0, greyscaleTexture.getWidth(), greyscaleTexture.getHeight());
                }
                else{
                    ofSetColor(255.0 * dim, 255.0 * dim, 255.0 * dim);
                    colorTexture.draw(0, 0, colorTexture.getWidth(), colorTexture.getHeight());
                }
            }
            ofDisableAlphaBlending();
        }
        ofPopMatrix();
    }
    ofPopStyle();
}


//draws an uwarped texture of what is projected
//-----------------------------------------------------
void imageProjection::drawPreviewTex(float x, float y, float w, float h){
    ofEnableAlphaBlending();
    if(bGreyscaleTexture)greyscaleTexture.draw(x,y,w,h);
    else colorTexture.draw(x,y,w,h);
    ofDisableAlphaBlending();
}		

