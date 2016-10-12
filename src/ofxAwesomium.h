//
//	ofxAwesomium.h
//	basic
//
//	Created by jeff-cr on 11/6/13.
//	ofxAwesomiumPlus: created by razvanilin 15/10/15
//	ofxAwesomiumPlus changes merged back to ofxAwesomium by nneonneo on 2016/10/12
//
//

#pragma once
#include "ofMain.h"
#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <map>
#include "js_delegate.h"

using namespace Awesomium;

class ofxAwesomium : public ofBaseDraws, JSMethodHandler {
public:

	class Listener {
	public:
		virtual ~Listener() {}
		
		// Event is fired when app (and WebCore) have been loaded.
		virtual void OnLoaded() = 0;
		
		// Event is fired for each iteration of the Run loop.
		virtual void OnUpdate() = 0;
		
		// Event is fired when the app is shutting down.
		virtual void OnShutdown() = 0;
	};

	typedef std::pair<int, Awesomium::WebString> ObjectMethodKey;
	typedef std::map<ObjectMethodKey, JSDelegate> BoundMethodMap;
	typedef std::map<ObjectMethodKey, JSDelegateWithRetval> BoundMethodWithRetvalMap;

	//ofxAwesomium();
	~ofxAwesomium();
	void setup(int width, int height, string appName="ofxAwesomium");
	void loadURL(string url);
	bool update();
	string getTitle();
	bool getIsLoading();
	JSValue doJavaScript(string js);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(float x, float y);

	void windowResized(int w, int h);

	string getState() const { return state; }
	void setScrollSpeed(int modifier) { scrollModifier = modifier; }
	void setTransparent(bool flag) { web_view->SetTransparent(flag); }
	void setAutomaticResize(bool flag) { resizable = flag; }
	bool isAutomaticResize() const { return resizable; }

	// begin ofBaseDraws
	/* Compatibility with both 0.9.x and 0.8.x */
	void draw(float x, float y) {
		frame.draw(x, y);
	}
	void draw(float x, float y, float w, float h) {
		frame.draw(x, y, w, h);
	}
	void draw(float x, float y) const {
		const_cast<ofImage *>(&frame)->draw(x, y);
	}
	void draw(float x, float y, float w, float h) const {
		const_cast<ofImage *>(&frame)->draw(x, y, w, h);
	}
	float getHeight() {
		return frame.getHeight();
	}
	float getWidth() {
		return frame.getWidth();
	}
	float getHeight() const {
		return const_cast<ofImage *>(&frame)->getHeight();
	}
	float getWidth() const {
		return const_cast<ofImage *>(&frame)->getWidth();
	}
	// end ofBaseDraws
	
	ofImage frame;

	static void initCore(string logsPath, string sessionPath);
	static void updateCore();
	static void shutdownCore();

	// Method Binding
	void Bind(Awesomium::JSObject& object,
	          const Awesomium::WebString& name,
	          JSDelegate callback);

	void BindWithRetval(Awesomium::JSObject& object,
	                    const Awesomium::WebString& name,
	                    JSDelegateWithRetval callback);

	void OnMethodCall(Awesomium::WebView* caller,
	                  unsigned int remote_object_id,
	                  const Awesomium::WebString& method_name,
	                  const Awesomium::JSArray& args);

	Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView* caller,
	                                               unsigned int remote_object_id,
	                                               const Awesomium::WebString& method_name,
	                                               const Awesomium::JSArray& args);

	void set_listener(Listener* listener) { listener = listener; }

	// This is a custom event
	void BindState();

	void OnChangeState(WebView* caller, const JSArray& args) {
		// extract the string from the args
		state = Awesomium::ToString(args[0].ToString());
	}

	// Make web_view public so that users can "reach into" Awesomium if needed
	WebView* web_view;
protected:
	
	BoundMethodMap bound_methods_;
	BoundMethodWithRetvalMap bound_methods_with_retval_;

	Listener* listener;

	string appName;
	BitmapSurface* surface;

	static bool bCoreInited;
	static WebCore* core;
	static WebSession* session;

	string state;
	int scrollModifier;

	bool resizable;
};
