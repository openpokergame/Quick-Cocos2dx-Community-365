/****************************************************************************
 Copyright (c) 2013-2017 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "lua_cocos2dx_spine_manual.hpp"
#include "lua_cocos2dx_spine_auto.hpp"
#include "cocos2d.h"

#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#include "LuaScriptHandlerMgr.h"
#include "CCLuaValue.h"
#include "spine.h"
#include "spine-cocos2dx.h"
#include "LuaSkeletonAnimation.h"
#include "CCLuaEngine.h"

//add by my self
//**************************************************************************************************************************************************************************************
#include "Attachment.h"
#include "extension.h"
//**************************************************************************************************************************************************************************************

using namespace spine;

// setBlendFunc
template<class T>
static int tolua_cocos2dx_setBlendFunc(lua_State* tolua_S,const char* className)
{
    if (NULL == tolua_S || NULL == className || strlen(className) == 0)
        return 0;

    int argc = 0;
    T* self = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,className,0,&tolua_err)) goto tolua_lerror;
#endif

    self = static_cast<T*>(tolua_tousertype(tolua_S,1,0));

    argc = lua_gettop(tolua_S) - 1;
    if (2 == argc)
    {
        CCLOG("setBlendFunc of %s will deprecate two int parameter form,please pass a table like {src = xx, dst = xx} as a parameter", className);
        
        GLenum src, dst;
        if (!luaval_to_int32(tolua_S, 2, (int32_t*)&src, StringUtils::format("%s%s",className,":setBlendFunc").c_str()))
            return 0;

        if (!luaval_to_int32(tolua_S, 3, (int32_t*)&dst,StringUtils::format("%s%s",className,":setBlendFunc").c_str()))
            return 0;

        BlendFunc blendFunc = {src, dst};
        self->setBlendFunc(blendFunc);
        return 0;
    }


    luaL_error(tolua_S, "'setBlendFunc' has wrong number of arguments: %d, was expecting %d\n", argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'setBlendFunc'.",&tolua_err);
    return 0;
#endif
}


static int lua_cocos2dx_CCSkeletonAnimation_createWithFile(lua_State* L)
{
    if (nullptr == L)
        return 0 ;
    
    int argc = 0;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(L,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif
    
    argc = lua_gettop(L) - 1;
    
    if (2 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err)  ||
            !tolua_isstring(L, 3 ,0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        const char* skeletonDataFile = tolua_tostring(L, 2, "");
        const char* atlasFile = tolua_tostring(L, 3, "");
        
        auto tolua_ret = LuaSkeletonAnimation::createWithFile(skeletonDataFile, atlasFile);
        
        int nID = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_ccobject(L, nID, pLuaID, (void*)tolua_ret,"sp.SkeletonAnimation");
        return 1;
    } else if (3 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err)  ||
            !tolua_isstring(L, 3 ,0, &tolua_err)  ||
            !tolua_isnumber(L, 4 ,0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        const char* skeletonDataFile = tolua_tostring(L, 2, "");
        const char* atlasFile = tolua_tostring(L, 3, "");
        LUA_NUMBER scale = tolua_tonumber(L, 4, 1);

        auto tolua_ret = LuaSkeletonAnimation::createWithFile(skeletonDataFile, atlasFile, scale);

        int nID = (tolua_ret) ? (int)tolua_ret->_ID : -1;
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;
        toluafix_pushusertype_ccobject(L, nID, pLuaID, (void*)tolua_ret,"sp.SkeletonAnimation");
        return 1;
    }
    
    luaL_error(L, "'createWithFile' function of SkeletonAnimation has wrong number of arguments: %d, was expecting %d\n", argc, 2);
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'createWithFile'.",&tolua_err);
#endif
    return 0;
}

int executeSpineEvent(LuaSkeletonAnimation* skeletonAnimation, int handler, spEventType eventType, spTrackEntry* entry, spEvent* event = nullptr )
{
    if (nullptr == skeletonAnimation || 0 == handler)
        return 0;
    
    LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
    if (nullptr == stack)
        return 0;
    
    lua_State* L = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    if (nullptr == L)
        return 0;
    
    int ret = 0;
    
    std::string animationName = (entry && entry->animation) ? entry->animation->name : "";
    std::string eventTypeName = "";
    
    switch (eventType) {
        case spEventType::SP_ANIMATION_START:
            {
                eventTypeName = "start";
            }
            break;
        case spEventType::SP_ANIMATION_INTERRUPT:
            {
                eventTypeName = "interrupt";
            }
                break;
        case spEventType::SP_ANIMATION_END:
            {
                eventTypeName = "end";
            }
            break;
        case spEventType::SP_ANIMATION_DISPOSE:
            {
                eventTypeName = "dispose";
            }
            break;
        case spEventType::SP_ANIMATION_COMPLETE:
            {
                eventTypeName = "complete";
            }
            break;
        case spEventType::SP_ANIMATION_EVENT:
            {
                eventTypeName = "event";
            }
            break;
            
        default:
            break;
    }
    
    LuaValueDict spineEvent;
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("type", LuaValue::stringValue(eventTypeName)));
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("trackIndex", LuaValue::intValue(entry->trackIndex)));
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("animation", LuaValue::stringValue(animationName)));
    spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("loopCount", LuaValue::intValue(std::floor(entry->trackTime / entry->animationEnd))));
    
    if (nullptr != event)
    {
        LuaValueDict eventData;
        eventData.insert(eventData.end(), LuaValueDict::value_type("name", LuaValue::stringValue(event->data->name)));
        eventData.insert(eventData.end(), LuaValueDict::value_type("intValue", LuaValue::intValue(event->intValue)));
        eventData.insert(eventData.end(), LuaValueDict::value_type("floatValue", LuaValue::floatValue(event->floatValue)));
        eventData.insert(eventData.end(), LuaValueDict::value_type("stringValue", LuaValue::stringValue(event->stringValue)));
        spineEvent.insert(spineEvent.end(), LuaValueDict::value_type("eventData", LuaValue::dictValue(eventData)));
    }
    
    stack->pushLuaValueDict(spineEvent);
    ret = stack->executeFunctionByHandler(handler, 1);
    return ret;
}

int tolua_Cocos2d_CCSkeletonAnimation_registerSpineEventHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err) ||
        !toluafix_isfunction(tolua_S,2,"LUA_FUNCTION",0,&tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err)                  ||
        !tolua_isnoobj(tolua_S,4,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        LuaSkeletonAnimation* self    = (LuaSkeletonAnimation*)  tolua_tousertype(tolua_S,1,0);
        if (NULL != self ) {
            int handler = (  toluafix_ref_function(tolua_S,2,0));
            spEventType eventType = static_cast<spEventType>((int)tolua_tonumber(tolua_S, 3, 0));
            
            switch (eventType) {
                case spEventType::SP_ANIMATION_START:
                    {
                        self->setStartListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_START);
                    }
                    break;
                case spEventType::SP_ANIMATION_INTERRUPT:
                    {
                        self->setInterruptListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_INTERRUPT);
                    }
                    break;
                case spEventType::SP_ANIMATION_END:
                    {
                        self->setEndListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_END);
                    }
                    break;
                case spEventType::SP_ANIMATION_DISPOSE:
                {
                    self->setDisposeListener([=](spTrackEntry* entry){
                        executeSpineEvent(self, handler, eventType, entry);
                    });
                    ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_DISPOSE);
                }
                    break;
                case spEventType::SP_ANIMATION_COMPLETE:
                    {
                        self->setCompleteListener([=](spTrackEntry* entry){
                            executeSpineEvent(self, handler, eventType, entry);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_COMPLETE);
                    }
                    break;
                case spEventType::SP_ANIMATION_EVENT:
                    {
                        self->setEventListener([=](spTrackEntry* entry, spEvent* event){
                            executeSpineEvent(self, handler, eventType, entry, event);
                        });
                        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_EVENT);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'registerSpineEventHandler'.",&tolua_err);
    return 0;
#endif
}

int tolua_Cocos2d_CCSkeletonAnimation_unregisterSpineEventHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S,3,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        LuaSkeletonAnimation* self    = (LuaSkeletonAnimation*)  tolua_tousertype(tolua_S,1,0);
        if (NULL != self ) {
            spEventType eventType = static_cast<spEventType>((int)tolua_tonumber(tolua_S, 2, 0));
            ScriptHandlerMgr::HandlerType handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_START;
            switch (eventType) {
                case spEventType::SP_ANIMATION_START:
                    self->setStartListener(nullptr);
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_START;
                    break;
                case spEventType::SP_ANIMATION_INTERRUPT:
                    self->setInterruptListener(nullptr);
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_INTERRUPT;
                    break;
                case spEventType::SP_ANIMATION_END:
                    self->setEndListener(nullptr);
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_END;
                    break;
                case spEventType::SP_ANIMATION_DISPOSE:
                    self->setDisposeListener(nullptr);
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_DISPOSE;
                    break;
                case spEventType::SP_ANIMATION_COMPLETE:
                    self->setCompleteListener(nullptr);
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_COMPLETE;
                    break;
                case spEventType::SP_ANIMATION_EVENT:
                    self->setEventListener(nullptr);
                    handlerType = ScriptHandlerMgr::HandlerType::EVENT_SPINE_ANIMATION_EVENT;
                    break;
                    
                default:
                    break;
            }
            ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self, handlerType);
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'unregisterScriptHandler'.",&tolua_err);
    return 0;
#endif
}

extern int lua_cocos2dx_spine_SkeletonRenderer_setBlendFunc(lua_State* tolua_S);

CC_DEPRECATED_ATTRIBUTE static int tolua_spine_SkeletoneAnimation_setBlendFunc(lua_State* tolua_S)
{
    int argc = lua_gettop(tolua_S) - 1;
    if (argc == 2)
    {
        return tolua_cocos2dx_setBlendFunc<spine::SkeletonAnimation>(tolua_S,"sp.SkeletonAnimation");
    }
    
    return lua_cocos2dx_spine_SkeletonRenderer_setBlendFunc(tolua_S);
}

static int lua_cocos2dx_spine_SkeletonAnimation_addAnimation(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;
    bool ok  = true;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_addAnimation'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 3)
    {
        int arg0;
        const char* arg1;
        bool arg2;
        
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "sp.SkeletonAnimation:addAnimation");
        
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:addAnimation"); arg1 = arg1_tmp.c_str();
        
        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "sp.SkeletonAnimation:addAnimation");
        if(!ok)
            return 0;
        cobj->addAnimation(arg0, arg1, arg2);
        
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 4)
    {
        int arg0;
        const char* arg1;
        bool arg2;
        double arg3;
        
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "sp.SkeletonAnimation:addAnimation");
        
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:addAnimation"); arg1 = arg1_tmp.c_str();
        
        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "sp.SkeletonAnimation:addAnimation");
        
        ok &= luaval_to_number(tolua_S, 5,&arg3, "sp.SkeletonAnimation:addAnimation");
        if(!ok)
            return 0;
        
        cobj->addAnimation(arg0, arg1, arg2, arg3);

        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "addAnimation",argc, 3);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_addAnimation'.",&tolua_err);
#endif
    
    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_setAnimation(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;
    bool ok  = true;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"sp.SkeletonAnimation",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_setAnimation'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 3)
    {
        int arg0;
        const char* arg1;
        bool arg2;
        
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "sp.SkeletonAnimation:setAnimation");
        
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:setAnimation"); arg1 = arg1_tmp.c_str();
        
        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "sp.SkeletonAnimation:setAnimation");
        if(!ok)
            return 0;
        
        cobj->setAnimation(arg0, arg1, arg2);
        
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "setAnimation",argc, 3);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_setAnimation'.",&tolua_err);
#endif
    
    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_setAttachment(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_setAttachment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc > 0) {
        const char *arg0 = lua_tostring(tolua_S, 2);

        if (!arg0) {
            tolua_error(tolua_S, "sp.SkeletonAnimation:setAttachment arg 1 must string", nullptr);
            return 0;
        }

        const char* arg1 = lua_tostring(tolua_S, 3);
        if (arg1) {
            // set new Attachment
            cobj->setAttachment(arg0, arg1);
        }
        else {
            // remove Attachment
            cobj->setAttachment(arg0, NULL);
        }

        lua_settop(tolua_S, 1);
        return 1;
    }

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_setAttachment'.", &tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_replacementParts(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_replacementParts'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc > 0) {
        const char *arg0 = lua_tostring(tolua_S, 2);

        if (!arg0) {
            tolua_error(tolua_S, "sp.SkeletonAnimation:replacementParts arg 1 must string", nullptr);
            return 0;
        }

        const char* arg1 = lua_tostring(tolua_S, 3);
        if (!arg1) {
            tolua_error(tolua_S, "sp.SkeletonAnimation:replacementParts arg 2 must string", nullptr);
            return 0;
        }

        spSkin *skin = spSkeletonData_findSkin(cobj->getSkeleton()->data, arg0);
        if (!skin){
            tolua_error(tolua_S, "sp.SkeletonAnimation:replacementParts no this skin name", nullptr);
            return 0;
        }
        for (int i = 0; i < cobj->getSkeleton()->slotsCount; ++i)
        {
            spSlot* slot = cobj->getSkeleton()->slots[i];
            if (strcmp(slot->data->name, arg1) == 0)
            {
                spAttachment* attachment = spSkin_getAttachment(skin, i, slot->data->attachmentName);
                if (attachment) spSlot_setAttachment(slot, attachment);
            }
        }

        lua_settop(tolua_S, 1);
        return 1;
    }

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_setAttachment'.", &tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_findBone(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_findBone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        const char *arg0 = lua_tostring(tolua_S, 2);
        if (!arg0) {
            tolua_error(tolua_S, "sp.SkeletonAnimation:findBone arg 1 must string", nullptr);
            return 0;
        }

        // call spine func
        spBone *bone = cobj->findBone(arg0);
#if COCOS2D_DEBUG >= 1
        if (NULL == bone) {
            tolua_error(tolua_S, "sp.SkeletonAnimation:findBone can not find bone", nullptr);
            return 0;
        }
#endif
        // === return a table ===
        lua_newtable(tolua_S);
#if 0  // real need relative info ?  the world info is more useful
        lua_pushstring(tolua_S, "x");
        lua_pushnumber(tolua_S, bone->x);
        lua_rawset(tolua_S, -3);    /* bone.x */
        lua_pushstring(tolua_S, "y");
        lua_pushnumber(tolua_S, bone->y);
        lua_rawset(tolua_S, -3);    /* bone.y */
        lua_pushstring(tolua_S, "rotation");
        lua_pushnumber(tolua_S, bone->rotation);
        lua_rawset(tolua_S, -3);    /* bone.rotation */
        lua_pushstring(tolua_S, "rotationIK");
        lua_pushnumber(tolua_S, bone->rotationIK);
        lua_rawset(tolua_S, -3);    /* bone.rotationIK */
        lua_pushstring(tolua_S, "scaleX");
        lua_pushnumber(tolua_S, bone->scaleX);
        lua_rawset(tolua_S, -3);    /* bone.scaleX */
        lua_pushstring(tolua_S, "scaleY");
        lua_pushnumber(tolua_S, bone->scaleY);
        lua_rawset(tolua_S, -3);    /* bone.scaleY */
        lua_pushstring(tolua_S, "flipX");
        lua_pushboolean(tolua_S, bone->flipX);
        lua_rawset(tolua_S, -3);    /* bone.flipX */
        lua_pushstring(tolua_S, "flipY");
        lua_pushboolean(tolua_S, bone->flipY);
        lua_rawset(tolua_S, -3);    /* bone.flipY */
#endif
        lua_pushstring(tolua_S, "worldX");
        lua_pushnumber(tolua_S, bone->worldX);
        lua_rawset(tolua_S, -3);    /* bone.worldX */
        lua_pushstring(tolua_S, "worldY");
        lua_pushnumber(tolua_S, bone->worldY);
        lua_rawset(tolua_S, -3);    /* bone.worldY */
        lua_pushstring(tolua_S, "worldSignX");
        //lua_pushnumber(tolua_S, bone->worldSignX);
        lua_pushnumber(tolua_S, bone->worldX);
        lua_rawset(tolua_S, -3);    /* bone.worldScaleX */
        lua_pushstring(tolua_S, "worldSignY");
        //lua_pushnumber(tolua_S, bone->worldSignY);
        lua_pushnumber(tolua_S, bone->worldY);
        lua_rawset(tolua_S, -3);    /* bone.worldScaleY */
        // === end of table ===
        return 1;
    }
    luaL_error(tolua_S, "findBone has wrong number of arguments: %d, was expecting %d \n", argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_findBone'.", &tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_findSlot(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;
    bool ok = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_findSlot'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        const char* arg0;

        std::string arg0_tmp;
        ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "sp.SkeletonAnimation:findSlot");
        arg0 = arg0_tmp.c_str();

        if (!ok)
            return 0;

        // call spine func
        spSlot *slot = cobj->findSlot(arg0);
#if COCOS2D_DEBUG >= 1
        if (NULL == slot) {
            tolua_error(tolua_S, "sp.SkeletonAnimation:findSlot can not find slot", nullptr);
            return 0;
        }
#endif

        float scaleX = cobj->getScaleX(), scaleY = cobj->getScaleY();
        float width = 0;
        float height = 0;
        float* _worldVertices = new float[1000]; // Max number of vertices per mesh.
        int verticesCount;
        if (slot->attachment->type == SP_ATTACHMENT_REGION) {
            spRegionAttachment* attachment = (spRegionAttachment*)slot->attachment;
            spRegionAttachment_computeWorldVertices(attachment, slot->bone, _worldVertices);
            verticesCount = 8;
            width = attachment->width;
            height = attachment->height;
        }
        else if (slot->attachment->type == SP_ATTACHMENT_MESH) {
            spMeshAttachment* mesh = (spMeshAttachment*)slot->attachment;
            spMeshAttachment_computeWorldVertices(mesh, slot, _worldVertices);
            verticesCount = mesh->super.worldVerticesLength;
            width = mesh->width;
            height = mesh->height;
        }
        else {
            delete[] _worldVertices;
            luaL_error(tolua_S, "findSlot unsupport attachment type \n");
            return 0;
        }

        // return a table
        lua_newtable(tolua_S);
        lua_pushstring(tolua_S, "attachmentWidth");
        lua_pushnumber(tolua_S, width);
        lua_rawset(tolua_S, -3);    /* slot.attachmentWidth */
        lua_pushstring(tolua_S, "attachmentHeight");
        lua_pushnumber(tolua_S, height);
        lua_rawset(tolua_S, -3);    /* slot.attachmentHeight */

        // push points of slot
        lua_pushstring(tolua_S, "points");
        lua_newtable(tolua_S);
        for (int index = 0; index < verticesCount; index += 2) {
            lua_newtable(tolua_S);

            lua_pushstring(tolua_S, "x");
            lua_pushnumber(tolua_S, _worldVertices[index] * scaleX);
            lua_rawset(tolua_S, -3);

            lua_pushstring(tolua_S, "y");
            lua_pushnumber(tolua_S, _worldVertices[index + 1] * scaleY);
            lua_rawset(tolua_S, -3);

            lua_rawseti(tolua_S, -2, index / 2 + 1);
        }
        lua_rawset(tolua_S, -3);

        delete[] _worldVertices;
        return 1;
    }
    luaL_error(tolua_S, "findSlot has wrong number of arguments: %d, was expecting %d \n", argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_findSlot'.", &tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_setFlippedX(lua_State* tolua_S)
{
    spine::SkeletonAnimation* cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_setFlippedX'", nullptr);
        return 0;
    }
#endif

    int argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int arg0 = lua_toboolean(tolua_S, 2);
        cobj->setFlippedX(arg0);
        return 0;
    }
    luaL_error(tolua_S, "setFlippedX has wrong number of arguments: %d, was expecting %d \n", argc, 1);
    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_setFlippedY(lua_State* tolua_S)
{
    spine::SkeletonAnimation* cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_setFlippedY'", nullptr);
        return 0;
    }
#endif

    int argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        int arg0 = lua_toboolean(tolua_S, 2);
        cobj->setFlippedY(arg0);
        return 0;
    }
    luaL_error(tolua_S, "setFlippedY has wrong number of arguments: %d, was expecting %d \n", argc, 1);
    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_findAnimation(lua_State* tolua_S)
{
    spine::SkeletonAnimation* cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_findAnimation'", nullptr);
        return 0;
    }
#endif

    int argc = lua_gettop(tolua_S) - 1;
    if (argc == 1)
    {
        const char *arg0 = lua_tostring(tolua_S, 2);
        if (cobj->findAnimation(arg0)) {
            lua_pushboolean(tolua_S, 1);
        }
        else {
            lua_pushboolean(tolua_S, 0);
        }
        return 1;
    }
    luaL_error(tolua_S, "findAnimation has wrong number of arguments: %d, was expecting %d \n", argc, 1);
    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_getBoundingBox(lua_State* tolua_S)
{
    spine::SkeletonAnimation* cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_getBoundingBox'", nullptr);
        return 0;
    }
#endif

    Rect rect = cobj->getBoundingBox();
    // return a table
    lua_newtable(tolua_S);
    lua_pushstring(tolua_S, "x");
    lua_pushnumber(tolua_S, rect.origin.x);
    lua_rawset(tolua_S, -3);
    lua_pushstring(tolua_S, "y");
    lua_pushnumber(tolua_S, rect.origin.y);
    lua_rawset(tolua_S, -3);
    lua_pushstring(tolua_S, "width");
    lua_pushnumber(tolua_S, rect.size.width);
    lua_rawset(tolua_S, -3);
    lua_pushstring(tolua_S, "height");
    lua_pushnumber(tolua_S, rect.size.height);
    lua_rawset(tolua_S, -3);
    return 1;
}

//add by my self
//**************************************************************************************************************************************************************************************

static spAtlasPage* createNewPage(spine::SkeletonAnimation* skeletonAnim, const char* slotName, const char* attachmentName, cocos2d::Sprite* frame, spAttachment* oldAttachment)
{
    spAtlasPage *page = nullptr;
    spAtlasRegion* region = nullptr;
    spSlot* curSlot = skeletonAnim->findSlot(slotName);
    if (curSlot == NULL || curSlot->attachment == NULL)
    {
        return page;
    }
    oldAttachment = curSlot->attachment;
    switch (oldAttachment->type)
    {
        case SP_ATTACHMENT_REGION:
        {
            spRegionAttachment* attachment = SUB_CAST(spRegionAttachment, oldAttachment);
            //region = SUB_CAST(spAtlasRegion, attachment->rendererObject);
            region = (spAtlasRegion*)attachment->rendererObject;
            break;
        }
        case SP_ATTACHMENT_MESH:
        case SP_ATTACHMENT_LINKED_MESH:
        {
            spMeshAttachment* attachment = SUB_CAST(spMeshAttachment, oldAttachment);
            //region = SUB_CAST(spAtlasRegion, attachment->rendererObject);
            region = (spAtlasRegion*)attachment->rendererObject;
            break;
        }
        case SP_ATTACHMENT_BOUNDING_BOX:
        {

        }
        case SP_ATTACHMENT_PATH:
            break;
        default:
            break;
        
    }
    if (region == nullptr || region->page == nullptr) return page;

    Texture2D* texture = frame->getTexture();

    char pageName[20] = {0};
    //itoa((int)texture, pageName, 16);
    spAtlasPage *searchPage = skeletonAnim->getAtlas()->pages;
    //spAtlasPage *searchPage = region->page->atlas->pages;
    //spAtlasPage *searchPage = region->page;// ((spAtlasRegion*)(region->page->rendererObject))->page;
    spAtlasPage* lastPage = searchPage;
    while (searchPage != nullptr)
    {
        if (strcmp(searchPage->name, pageName) == 0)
        {
            page = searchPage;
            break;
        }
        searchPage = searchPage->next;
        if (searchPage != nullptr)
            lastPage = searchPage;
    }

    if (page != nullptr) return page;
    page = spAtlasPage_create(skeletonAnim->getAtlas(), pageName);
    //texture->retain();
    page->rendererObject = texture;
    page->width = texture->getPixelsWide();
    page->height = texture->getPixelsHigh();
    lastPage->next = page;

    page->format = region->page->format;
    page->minFilter = region->page->minFilter;
    page->magFilter = region->page->magFilter;
    page->uWrap = region->page->uWrap;
    page->vWrap = region->page->vWrap;
    page->name = region->page->name;

    return page;
}

static spAtlasRegion* createRegion(spAtlasPage *page, cocos2d::Sprite* sprite, spAttachment* oldAttachment)
{
    const char* name = oldAttachment->name;
    cocos2d::SpriteFrame* frame = sprite->getSpriteFrame();
    Texture2D* texture = frame->getTexture();
    spAtlasRegion *region = spAtlasRegion_create();
    region->page = page;
    MALLOC_STR(region->name, name);
    region->rotate = frame->isRotated();

    cocos2d::Rect rect = frame->getRectInPixels();
    region->x = rect.getMinX();
    region->y = rect.getMinY();
    region->width = rect.size.width;
    region->height = rect.size.height;

    cocos2d::Size originSize = frame->getOriginalSizeInPixels();
    region->originalWidth = originSize.width;
    region->originalHeight = originSize.height;

    region->u = region->x / (float)page->width;
    region->v = region->y / (float)page->height;
    if (region->rotate) {
        region->u2 = (region->x + region->height) / (float)page->width;
        region->v2 = (region->y + region->width) / (float)page->height;
    }
    else {
        region->u2 = (region->x + region->width) / (float)page->width;
        region->v2 = (region->y + region->height) / (float)page->height;
    }

    cocos2d::Vec2 offset = frame->getOffsetInPixels();
    region->offsetX = offset.x;
    region->offsetY = offset.y;
    region->index = -1;

    spAtlasRegion* lastRegion, *nextRegion;
    lastRegion = page->atlas->regions;
    nextRegion = region->next;

    while (nextRegion != nullptr)
    {
        lastRegion = nextRegion;
        nextRegion = nextRegion->next;
    }
    lastRegion->next = region;

    return region;
}

static  spAttachment* getAttachmentFromSlot(spine::SkeletonAnimation* skeletonAnim, const char* slotName, const char* attachmentName, const char* attachment2 /*= nullptr*/)
{
    spAttachment* attachment = nullptr;
    spSkeleton* _skeleton = skeletonAnim->getSkeleton();
    spSkin* skin = _skeleton->skin == nullptr ? _skeleton->data->defaultSkin : _skeleton->skin;
    for (auto i = 0; i < _skeleton->slotsCount; ++i)
    {
        spSlot* slot = _skeleton->slots[i];
        //if (slot->data->name != nullptr && strcmp(slot->data->name, slotName) == 0 && slot->data->attachmentName != nullptr && strcmp(slot->data->attachmentName, attachmentName) == 0)
        if (slot->data->attachmentName != nullptr && strcmp(slot->data->attachmentName, attachmentName) == 0)
        {
            attachment = spSkin_getAttachment(skin, i, attachment2 == nullptr ? attachmentName : attachment2);
            break;
        }
    }



    return attachment;
}

static spAtlasRegion*  changeAttachment(spAttachment* oldAttachment, spAtlasRegion* region)
{
    spAtlasRegion* oldRegion = nullptr;
    if (oldAttachment == nullptr || region == nullptr) return oldRegion;

    switch (oldAttachment->type)
    {
        case SP_ATTACHMENT_REGION:
        {
            spRegionAttachment* oldRegionAttachment = SUB_CAST(spRegionAttachment, oldAttachment);
            oldRegion = (spAtlasRegion*)oldRegionAttachment->rendererObject;
            oldRegionAttachment->rendererObject = region;
            spRegionAttachment_setUVs(oldRegionAttachment, region->u, region->v, region->u2, region->v2, region->rotate);
            oldRegionAttachment->regionOffsetX = region->offsetX;
            oldRegionAttachment->regionOffsetY = region->offsetY;
            oldRegionAttachment->regionWidth = region->width;
            oldRegionAttachment->regionHeight = region->height;
            oldRegionAttachment->regionOriginalWidth = region->originalWidth;
            oldRegionAttachment->regionOriginalHeight = region->originalHeight;

            spRegionAttachment_updateOffset(oldRegionAttachment);
            break;
        }
        case SP_ATTACHMENT_MESH:
        case SP_ATTACHMENT_LINKED_MESH:
        {
            spMeshAttachment* oldMesh = SUB_CAST(spMeshAttachment, oldAttachment);
            oldRegion = (spAtlasRegion*)oldMesh->rendererObject;
            oldMesh->rendererObject = region;
            oldMesh->regionU = region->u;
            oldMesh->regionV = region->v;
            oldMesh->regionU2 = region->u2;
            oldMesh->regionV2 = region->v2;
            oldMesh->regionRotate = region->rotate;
            oldMesh->regionOffsetX = region->offsetX;
            oldMesh->regionOffsetY = region->offsetY;
            oldMesh->regionWidth = region->width;
            oldMesh->regionHeight = region->height;
            oldMesh->regionOriginalWidth = region->originalWidth;
            oldMesh->regionOriginalHeight = region->originalHeight;

            spMeshAttachment_updateUVs(oldMesh);

            break;
        }
        case SP_ATTACHMENT_BOUNDING_BOX:
            break;
        case SP_ATTACHMENT_PATH:
            break;
        default:
            break;
    }
    return oldRegion;
}

static void replaceAttachmentByRegion(spine::SkeletonAnimation* skeletonAnim, const char* slotName, const char* attachmentName, void* region, const char* attachment2 /*= nullptr*/){
    changeAttachment(getAttachmentFromSlot(skeletonAnim, slotName, attachmentName, attachment2), (spAtlasRegion*)region);
}

static int lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentImage(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;
    bool ok = true;
    cocos2d::SpriteFrame* cSF = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentImage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        const char* arg0;
        const char* arg1;
        cocos2d::Sprite* arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "sp.SkeletonAnimation:replaceAttachmentImage"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:replaceAttachmentImage"); arg1 = arg1_tmp.c_str();

        ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 4, "cc.Sprite", &arg2);
        if (!ok)
            return 0;

        //cobj->setAnimation(arg0, arg1, arg2);

        spAttachment* oldAttachment = getAttachmentFromSlot(cobj, arg0, arg1, nullptr);
        if (oldAttachment == nullptr) 
            return 0;
        spAttachment* newAttchment = nullptr;
        const char* name = oldAttachment->name;
        spAtlasPage *page = createNewPage(cobj, arg0, arg1, arg2, oldAttachment);
        if (page == nullptr) 
            return 0;
        spAtlasRegion *region = createRegion(page, arg2, oldAttachment);
        replaceAttachmentByRegion(cobj, arg0, arg1, region, nullptr);

        lua_settop(tolua_S, 1);
        return 1;
    }
    else if (argc == 4)
    {
        const char* arg0;
        const char* arg1;
        cocos2d::Sprite* arg2;
        const char* arg3;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "sp.SkeletonAnimation:replaceAttachmentImage"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:replaceAttachmentImage"); arg1 = arg1_tmp.c_str();

        ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 4, "cc.Sprite", &arg2);
        if (!ok)
            return 0;
        std::string arg3_tmp; ok &= luaval_to_std_string(tolua_S, 5, &arg3_tmp, "sp.SkeletonAnimation:replaceAttachmentImage"); arg3 = arg3_tmp.c_str();

        //cobj->setAnimation(arg0, arg1, arg2);

        spAttachment* oldAttachment = getAttachmentFromSlot(cobj, arg0, arg1, arg3);
        if (oldAttachment == nullptr)
            return 0;
        spAttachment* newAttchment = nullptr;
        const char* name = oldAttachment->name;
        spAtlasPage *page = createNewPage(cobj, arg0, arg1, arg2, oldAttachment);
        if (page == nullptr)
            return 0;
        spAtlasRegion *region = createRegion(page, arg2, oldAttachment);
        replaceAttachmentByRegion(cobj, arg0, arg1, region, arg3);

        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "replaceAttachmentImage", argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentImage'.", &tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentByRegion(lua_State* tolua_S)
{
    int argc = 0;
    spine::SkeletonAnimation* cobj = nullptr;
    bool ok = true;
    cocos2d::SpriteFrame* cSF = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "sp.SkeletonAnimation", 0, &tolua_err)) goto tolua_lerror;
#endif

    cobj = (spine::SkeletonAnimation*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentByRegion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        const char* arg0;
        const char* arg1;
        cocos2d::Sprite* arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "sp.SkeletonAnimation:replaceAttachmentByRegion"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:replaceAttachmentByRegion"); arg1 = arg1_tmp.c_str();

        ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 4, "cc.Sprite", &arg2);
        if (!ok)
            return 0;

        //cobj->setAnimation(arg0, arg1, arg2);

        spAttachment* oldAttachment = getAttachmentFromSlot(cobj, arg0, arg1, nullptr);
        if (oldAttachment == nullptr)
            return 0;
        spAttachment* newAttchment = nullptr;
        const char* name = oldAttachment->name;
        spAtlasPage *page = createNewPage(cobj, arg0, arg1, arg2, oldAttachment);
        if (page == nullptr)
            return 0;
        spAtlasRegion *region = createRegion(page, arg2, oldAttachment);
        replaceAttachmentByRegion(cobj, arg0, arg1, region, nullptr);

        lua_settop(tolua_S, 1);
        return 1;
    }
    else if (argc == 4)
    {
        const char* arg0;
        const char* arg1;
        cocos2d::Sprite* arg2;
        const char* arg3;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "sp.SkeletonAnimation:replaceAttachmentByRegion"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "sp.SkeletonAnimation:replaceAttachmentByRegion"); arg1 = arg1_tmp.c_str();

        ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 4, "cc.Sprite", &arg2);
        if (!ok)
            return 0;
        std::string arg3_tmp; ok &= luaval_to_std_string(tolua_S, 5, &arg3_tmp, "sp.SkeletonAnimation:replaceAttachmentByRegion"); arg3 = arg3_tmp.c_str();

        //cobj->setAnimation(arg0, arg1, arg2);

        spAttachment* oldAttachment = getAttachmentFromSlot(cobj, arg0, arg1, arg3);
        if (oldAttachment == nullptr)
            return 0;
        spAttachment* newAttchment = nullptr;
        const char* name = oldAttachment->name;
        spAtlasPage *page = createNewPage(cobj, arg0, arg1, arg2, oldAttachment);
        if (page == nullptr)
            return 0;
        spAtlasRegion *region = createRegion(page, arg2, oldAttachment);
        replaceAttachmentByRegion(cobj, arg0, arg1, region, arg3);

        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "replaceAttachmentByRegion", argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentByRegion'.", &tolua_err);
#endif

    return 0;
}
//**************************************************************************************************************************************************************************************

static void extendCCSkeletonAnimation(lua_State* L)
{
    lua_pushstring(L, "sp.SkeletonAnimation");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "create", lua_cocos2dx_CCSkeletonAnimation_createWithFile);
        tolua_function(L, "registerSpineEventHandler", tolua_Cocos2d_CCSkeletonAnimation_registerSpineEventHandler00);
        tolua_function(L, "unregisterSpineEventHandler", tolua_Cocos2d_CCSkeletonAnimation_unregisterSpineEventHandler00);
        tolua_function(L, "setBlendFunc", tolua_spine_SkeletoneAnimation_setBlendFunc);
        tolua_function(L, "addAnimation", lua_cocos2dx_spine_SkeletonAnimation_addAnimation);
        tolua_function(L, "setAnimation", lua_cocos2dx_spine_SkeletonAnimation_setAnimation);

        tolua_function(L, "setAttachment", lua_cocos2dx_spine_SkeletonAnimation_setAttachment);
        tolua_function(L, "findBone", lua_cocos2dx_spine_SkeletonAnimation_findBone);
        tolua_function(L, "findSlot", lua_cocos2dx_spine_SkeletonAnimation_findSlot);
        tolua_function(L, "setFlippedX", lua_cocos2dx_spine_SkeletonAnimation_setFlippedX);
        tolua_function(L, "setFlippedY", lua_cocos2dx_spine_SkeletonAnimation_setFlippedY);
        tolua_function(L, "findAnimation", lua_cocos2dx_spine_SkeletonAnimation_findAnimation);
        tolua_function(L, "getBoundingBox", lua_cocos2dx_spine_SkeletonAnimation_getBoundingBox);
        //**************************************************************************************************************************************************************************************
        tolua_function(L, "replacementParts", lua_cocos2dx_spine_SkeletonAnimation_replacementParts);
        //tolua_function(L, "replaceAttachmentByRegion", lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentByRegion);
        //tolua_function(L, "replaceAttachmentImage", lua_cocos2dx_spine_SkeletonAnimation_replaceAttachmentImage);
        //**************************************************************************************************************************************************************************************
    }
    lua_pop(L, 1);
    
    /*Because sp.SkeletonAnimation:create creat a LuaSkeletonAnimation object,so we need use LuaSkeletonAnimation typename for g_luaType*/
    std::string typeName = typeid(LuaSkeletonAnimation).name();
    g_luaType[typeName] = "sp.SkeletonAnimation";
    g_typeCast["SkeletonAnimation"] = "sp.SkeletonAnimation";
}

int register_all_cocos2dx_spine_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendCCSkeletonAnimation(L);
    
    return 0;
}

int register_spine_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L,-1))//stack:...,_G,
    {
        register_all_cocos2dx_spine(L);
        register_all_cocos2dx_spine_manual(L);
    }
    lua_pop(L, 1);

    return 1;
}
