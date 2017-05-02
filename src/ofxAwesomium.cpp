//
//  ofxAwesomium.cpp
//  basic
//
//  Created by jeff-cr on 11/6/13.
//
//

#include "ofxAwesomium.h"

// ----------------------------------------------------------------
ofxAwesomium::~ofxAwesomium(){
	web_view->Destroy();
}

// ----------------------------------------------------------------
void ofxAwesomium::setup(int width, int height, string appName) {
	if(!bCoreInited) {
		initCore(ofToDataPath("Logs"), ofToDataPath("SessionData"));
	}
	web_view = core->CreateWebView(width, height, session);
	this->appName = appName;
	scrollModifier = 3;

	// Bind the events
	BindState();

	frame.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
	//texture.allocate(width, height, GL_RGBA);

	// the browser is resizable by default
	resizable = true;
}

// ----------------------------------------------------------------
void ofxAwesomium::loadURL(string url) {
	web_view->LoadURL(WebURL(WSLit(url.c_str())));
    web_view->Focus();
}

// ----------------------------------------------------------------
JSValue ofxAwesomium::doJavaScript(string js) {
	return web_view->ExecuteJavascriptWithResult(WSLit(js.c_str()), WSLit(""));
}

// ----------------------------------------------------------------
bool ofxAwesomium::update() {
    surface = (BitmapSurface*)web_view->surface();
    
    if(surface && surface->buffer() && surface->is_dirty()) {
		surface->CopyTo(frame.getPixels().getData(), frame.getWidth()*4, 4, true, false);
		frame.update();
		return true;
    }
	return false;
}

// ----------------------------------------------------------------
string ofxAwesomium::getTitle(){
	char buf[1024];
    web_view->title().ToUTF8( buf, 1024 );
	return string(buf);
}

// ----------------------------------------------------------------
bool ofxAwesomium::getIsLoading(){
	return web_view->IsLoading();
}

// ----------------------------------------------------------------
static int getVirtualKeyCode(int key) {
    switch (key)
    {
        case OF_KEY_LEFT:
            return KeyCodes::AK_LEFT;
        case OF_KEY_RIGHT:
            return KeyCodes::AK_RIGHT;
        case OF_KEY_UP:
            return KeyCodes::AK_UP;
        case OF_KEY_DOWN:
            return KeyCodes::AK_DOWN;
        case OF_KEY_DEL:
            return KeyCodes::AK_DELETE;
        case OF_KEY_BACKSPACE:
            return KeyCodes::AK_BACK;
            // this is the 'dot' key. For some reason I had to hard-code this in order to make it work
        case 46:
            return KeyCodes::AK_DECIMAL;
        case -1:
            return -1;
        case OF_KEY_TAB:
            // don't insert anything when TAB is pressed
            return -1;
        default:
            return key;
    }
}

void ofxAwesomium::keyPressed(int key) {
	web_view->Focus();
    
    int text = key;
    int vk = getVirtualKeyCode(key);
    if(vk == -1)
         return;

    Awesomium::WebKeyboardEvent keyDown;
    keyDown.type = Awesomium::WebKeyboardEvent::kTypeKeyDown;
    keyDown.virtual_key_code = (char)vk;
    keyDown.native_key_code = (char)key;
    keyDown.text[0] = (char)text;
    keyDown.unmodified_text[0] = (char)text;
    keyDown.modifiers = (char)OF_KEY_MODIFIER;
    web_view->InjectKeyboardEvent( keyDown );
    
    Awesomium::WebKeyboardEvent typeChar;
    typeChar.type = Awesomium::WebKeyboardEvent::kTypeChar;
    typeChar.virtual_key_code =  (char)vk;
    typeChar.native_key_code =  (char)key;
    typeChar.text[0] =  (char)text;
    typeChar.unmodified_text[0] =  (char)text;
    typeChar.modifiers = (char)OF_KEY_MODIFIER;
    web_view->InjectKeyboardEvent( typeChar );
}

// ----------------------------------------------------------------
void ofxAwesomium::keyReleased(int key) {
	web_view->Focus();
    
    int text = key;
    int vk = getVirtualKeyCode(key);
    if(vk == -1)
         return;

    Awesomium::WebKeyboardEvent evt;
    evt.type = Awesomium::WebKeyboardEvent::kTypeKeyUp;
    evt.virtual_key_code = (char)vk;
    evt.native_key_code = (char)key;
    evt.text[0] = (char)text;
    evt.unmodified_text[0] = (char)text;
    evt.modifiers = (char)OF_KEY_MODIFIER;
    web_view->InjectKeyboardEvent( evt );
}

//--------------------------------------------------------------
void ofxAwesomium::mouseMoved(int x, int y ){
    web_view->InjectMouseMove( x, y );
}

//--------------------------------------------------------------
void ofxAwesomium::mouseDragged(int x, int y, int button){
    web_view->InjectMouseMove( x, y );
}

//--------------------------------------------------------------
void ofxAwesomium::mousePressed(int x, int y, int button){
    if( button ==  OF_MOUSE_BUTTON_1)
        web_view->InjectMouseDown( Awesomium::kMouseButton_Left );
    else if( button == OF_MOUSE_BUTTON_2 )
        web_view->InjectMouseDown( Awesomium::kMouseButton_Middle );
    else if( button== OF_MOUSE_BUTTON_3 )
        web_view->InjectMouseDown( Awesomium::kMouseButton_Right );
}

//--------------------------------------------------------------
void ofxAwesomium::mouseReleased(int x, int y, int button){
    if( button ==  OF_MOUSE_BUTTON_1)
        web_view->InjectMouseUp( Awesomium::kMouseButton_Left );
    else if( button == OF_MOUSE_BUTTON_2 )
        web_view->InjectMouseUp( Awesomium::kMouseButton_Middle );
    else if( button== OF_MOUSE_BUTTON_3 )
        web_view->InjectMouseUp( Awesomium::kMouseButton_Right );
}

//--------------------------------------------------------------
void ofxAwesomium::mouseScrolled(float x, float y) {
    web_view->InjectMouseWheel(y*scrollModifier, x*scrollModifier);
}

//--------------------------------------------------------------
void ofxAwesomium::windowResized(int w, int h) {
	if (resizable) {
		frame.resize(w, h);
		web_view->Resize(frame.getWidth(), frame.getHeight());
	}
    
    // Might crash on windows - substract a few pixels from width and height to fix it (just in case)
}


//--------------------------------------------------------------
//
//	AWESOMIUM JS METHOD HANDLER METHODS
//
//--------------------------------------------------------------

void ofxAwesomium::BindState() {
    JSValue result = web_view->CreateGlobalJavascriptObject(WSLit(appName.c_str()));
    if (result.IsObject()) {
        // Bind our custom method to it.
        JSObject& app_object = result.ToObject();
        
        // Bind the js function 'changeStaet' to the cpp method 'OnChangeState'
        Bind(app_object,
             WSLit("changeState"),
             JSDelegate(this, &ofxAwesomium::OnChangeState));
    }
    
    // Bind our method dispatcher to the WebView
    web_view->set_js_method_handler(this);
}

void ofxAwesomium::Bind(Awesomium::JSObject& object,
                       const Awesomium::WebString& name,
                       JSDelegate callback) {
    // We can't bind methods to local JSObjects
    if (object.type() == Awesomium::kJSObjectType_Local)
        return;
    
    object.SetCustomMethod(name, false);
    
    ObjectMethodKey key(object.remote_id(), name);
    bound_methods_[key] = callback;
}

void ofxAwesomium::BindWithRetval(Awesomium::JSObject& object,
                                 const Awesomium::WebString& name,
                                 JSDelegateWithRetval callback) {
    // We can't bind methods to local JSObjects
    if (object.type() == Awesomium::kJSObjectType_Local)
        return;
    
    object.SetCustomMethod(name, true);
    
    ObjectMethodKey key(object.remote_id(), name);
    bound_methods_with_retval_[key] = callback;
}

void ofxAwesomium::OnMethodCall(Awesomium::WebView* caller,
                               unsigned int remote_object_id,
                               const Awesomium::WebString& method_name,
                               const Awesomium::JSArray& args) {
    ofLogNotice("On method call...");
    // Find the method that matches the object id + method name
    std::map<ObjectMethodKey, JSDelegate>::iterator i =
    bound_methods_.find(ObjectMethodKey(remote_object_id, method_name));
    
    // Call the method
    if (i != bound_methods_.end())
        i->second(caller, args);
}

Awesomium::JSValue ofxAwesomium::OnMethodCallWithReturnValue(Awesomium::WebView* caller,
                                                            unsigned int remote_object_id,
                                                            const Awesomium::WebString& method_name,
                                                            const Awesomium::JSArray& args) {
    // Find the method that matches the object id + method name
    std::map<ObjectMethodKey, JSDelegateWithRetval>::iterator i =
    bound_methods_with_retval_.find(ObjectMethodKey(remote_object_id, method_name));
    
    // Call the method
    if (i != bound_methods_with_retval_.end())
        return i->second(caller, args);
    
    return Awesomium::JSValue::Undefined();
}


// ----------------------------------------------------------------
//  _       __     __    ______
// | |     / /__  / /_  / ____/___  ________
// | | /| / / _ \/ __ \/ /   / __ \/ ___/ _ \
// | |/ |/ /  __/ /_/ / /___/ /_/ / /  /  __/
// |__/|__/\___/_.___/\____/\____/_/   \___/
// ----------------------------------------------------------------

bool ofxAwesomium::bCoreInited;
WebCore* ofxAwesomium::core;
WebSession* ofxAwesomium::session;

// ----------------------------------------------------------------
void ofxAwesomium::initCore(string logsPath, string sessionPath) {
	WebConfig config;
    config.log_path = WSLit(logsPath.c_str());
    config.log_level = kLogLevel_Verbose; //kLogLevel_Normal;
    
    core = WebCore::Initialize(config);
    
    WebPreferences prefs;
    prefs.enable_plugins = true;
    prefs.enable_smooth_scrolling = true;
    
    session = core->CreateWebSession(WSLit(sessionPath.c_str()), prefs);
	
	bCoreInited = true;
}

// ----------------------------------------------------------------
void ofxAwesomium::updateCore() {
	core->Update();
}

// ----------------------------------------------------------------
void ofxAwesomium::shutdownCore() {
	WebCore::Shutdown();
}

