#include "AppDelegate.h"
#include "CCLuaEngine.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include "lua_module_register.h"

// extra lua module
#include "cocos2dx_extra.h"
#include "lua_extensions/lua_extensions_more.h"
#include "luabinding/lua_cocos2dx_extension_filter_auto.hpp"
#include "luabinding/lua_cocos2dx_extension_nanovg_auto.hpp"
#include "luabinding/lua_cocos2dx_extension_nanovg_manual.hpp"
#include "luabinding/cocos2dx_extra_luabinding.h"
#include "luabinding/HelperFunc_luabinding.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "luabinding/cocos2dx_extra_ios_iap_luabinding.h"
#endif

using namespace CocosDenshion;

USING_NS_CC;
using namespace std;

static void quick_module_register(lua_State *L)
{
    luaopen_lua_extensions_more(L);

    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))//stack:...,_G,
    {
        register_all_quick_manual(L);
        // extra
        luaopen_cocos2dx_extra_luabinding(L);
        register_all_cocos2dx_extension_filter(L);
        register_all_cocos2dx_extension_nanovg(L);
        register_all_cocos2dx_extension_nanovg_manual(L);
        luaopen_HelperFunc_luabinding(L);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        luaopen_cocos2dx_extra_ios_iap_luabinding(L);
#endif
    }
    lua_pop(L, 1);
}

//
AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
	director->setProjection(Director::Projection::_2D);
    
    auto glview = director->getOpenGLView();
    if(!glview) {
        string title = "__PROJECT_COCOS_NAME__";
        glview = cocos2d::GLViewImpl::create(title.c_str());
        director->setOpenGLView(glview);
        director->startAnimation();
    }
   
    auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    lua_State* L = engine->getLuaStack()->getLuaState();
    lua_module_register(L);

    // use Quick-Cocos2d-X
    quick_module_register(L);

    LuaStack* stack = engine->getLuaStack();

#if 1

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	// load framework
#ifdef CC_TARGET_OS_IPHONE
    if (sizeof(long) == 4) {
        stack->loadChunksFromZIP("res/framework_precompiled.zip");
    } else {
        stack->loadChunksFromZIP("res/framework_precompiled64.zip");
    }
#else
    stack->loadChunksFromZIP("res/framework_precompiled.zip");
#endif
#endif
    // use luajit bytecode package
    int x=0;
    char y[17];
    char *z = y;
    *(z++)=0x65;         //e  65
    *(z++)=y[x++]+0x06;  //k  6B
    *(z++)=y[x++]-0x18;  //S  53
    *(z++)=y[x++]+0x22;  //u  75
    *(z++)=y[x++]-0x05;  //p  70
    *(z++)=y[x++]-0x0B;  //e  65
    *(z++)=y[x++]+0x0D;  //r  72
    *(z++)=y[x++]-0x31;  //A  41
    *(z++)=y[x++]+0x2D;  //n  6E
    *(z++)=y[x++]+0x06;  //t  74
    *(z++)=y[x++]-0x34;  //@  40
    *(z++)=y[x++]-0x0E;  //2  32
    *(z++)=y[x++]-0x02;  //0  30
    *(z++)=y[x++]+0x01;  //1  31
    *(z++)=y[x++]+0x07;  //8  38
    *(z++)=y[x++]-0x14;  //$  24
    *(z++)=y[x]-0x24;    //0  0
    // stack->setXXTEAKeyAndSign("ekSuperAnt@2018$", "Mini");
    //stack->setXXTEAKeyAndSign(y, "__PROJECT_COCOS_NAME__");

#ifdef CC_TARGET_OS_IPHONE
    if (sizeof(long) == 4) {
        stack->loadChunksFromZIP("res/game.zip");
    } else {
        stack->loadChunksFromZIP("res/game64.zip");
    }
#else
    
    stack->loadChunksFromZIP("res/game.zip");
#endif
    stack->executeString("require 'main'");
#else // #if 0
    // use discrete files
    engine->executeScriptFile("src/main.lua");
#endif

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
    Director::getInstance()->pause();

    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();

    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("APP_ENTER_BACKGROUND_EVENT");
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->resume();
    Director::getInstance()->startAnimation();

    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();

    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("APP_ENTER_FOREGROUND_EVENT");
}
