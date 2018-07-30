#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	browser.setup(ofGetWidth()-30, ofGetHeight()-30);
	browser.loadURL("https://m.naver.com");
	
}

//--------------------------------------------------------------
void ofApp::update() {
	ofxAwesomium::updateCore();
	browser.update();
	
	ofSetWindowTitle(browser.getTitle());
}

//--------------------------------------------------------------
void ofApp::draw() {
	if(browser.getIsLoading()) {
		ofSetColor(0);
		ofDrawBitmapString("Loading...", 10, 15);
	}
	ofSetColor(255);
	browser.draw(10, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	browser.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	browser.keyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {
	browser.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	browser.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	browser.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	browser.mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 

}
