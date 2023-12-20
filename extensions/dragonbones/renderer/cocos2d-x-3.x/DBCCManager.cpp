#include "DBCCManager.h"

NAME_SPACE_DRAGON_BONES_BEGIN

static unsigned int _tickCount;

static void doAsyncCallBack(cocos2d::CCObject* target, cocos2d::SEL_CallFuncO selector, int handler)
{
	
}

static DBCCManager *msCCDBManager = nullptr;

DBCCManager * DBCCManager::getInstance()
{
	if (!msCCDBManager)
	{
		msCCDBManager = new DBCCManager();
	}
	return msCCDBManager;
}

/**
* Creates a new CCDBManager instance.
*/
DBCCManager::DBCCManager()
{
	printTickCount();
}
DBCCManager::~DBCCManager()
{
	CCLOG("~CCDBMAnager");
	for (auto iter = _asyncList.begin(); iter != _asyncList.end(); iter++)
	{
		delete iter->second;
	}
	_asyncList.clear();
	_factory->dispose(true);
}

void DBCCManager::destroyInstance()
{
	CCLOG("CCDBMAnager::destroyInstance");
	if (msCCDBManager)
	{
		CC_SAFE_DELETE(msCCDBManager);
	}
}

void DBCCManager::printTickCount()
{
	++_tickCount;
	//cocos2d::log("%s tickcount:%d thisptr:%d msCCDBManager:%d, sizeof(%d) %d, %d", __FUNCTION__, _tickCount, this, msCCDBManager, sizeof(CCDBManager), sizeof(BaseFactory), sizeof(CCObject));
}

void DBCCManager::parseXMLByDir(const std::string& path, std::string &skeletonXMLFile, std::string &textureXMLFile)
{
	std::string dataDir(path);
	size_t pos;
	while ((pos = dataDir.find_first_of("\\")) != std::string::npos)
	{
		dataDir.replace(pos, 1, "/");
	}
	size_t slash = dataDir.find_last_of("/");
	if (slash == std::string::npos || slash != dataDir.size() - 1)
	{
		dataDir.append("/");
	}
	skeletonXMLFile.append(dataDir + "skeleton.xml");
	textureXMLFile.append(dataDir + "texture.xml");
}

void DBCCManager::loadData(const std::string &skeletonFile,
	const std::string &textureAtlasFile,
	const std::string &skeletonName,
	const std::string &textureAtlasName)
{
	if (!skeletonFile.empty())
		_factory->loadDragonBonesData(skeletonFile, skeletonName);
	if (!textureAtlasFile.empty())
		_factory->loadTextureAtlas(textureAtlasFile, textureAtlasName);
}

void DBCCManager::loadDataAsyncImpl(const std::string &skeletonFile,
	const std::string &textureAtlasFile,
	const std::string &skeletonName,
	const std::string &textureAtlasName,
	cocos2d::CCObject* pObj,
	cocos2d::SEL_CallFuncO selector,
	int scriptHandler)
{
	/*
	CCLOG("%s skeletonFile:%s, textureAtlasFile:%s skeletonName:%s pObj:%d selector:%d, handler:%d",
	__FUNCTION__,
	skeletonFile.c_str(),
	textureAtlasFile.c_str(),
	skeletonName.c_str(),
	pObj,
	selector,
	scriptHandler);
	//*/
	_factory->loadDragonBonesData(skeletonFile, skeletonName);
	//if (!_asyncList)
	//	_asyncList = new std::map<std::string, AsyncStruct*>();
	if (_factory->hasDragonBones(skeletonName))
	{
		_factory->loadTextureAtlas(textureAtlasFile, textureAtlasName);
		doAsyncCallBack(pObj, selector, scriptHandler);
	}
	else
	{
		//ITextureAtlas *textureAtlas = _factory.loadTextureAtlas(textureAtlasFile);

		//CCDBManager::AsyncStruct* asyncObj = new CCDBManager::AsyncStruct();
		////auto asyncObj = new AsyncStruct();
		//asyncObj->pObj = pObj;
		//asyncObj->pData = textureAtlas->textureAtlasData;
		//asyncObj->pSelector = selector;
		//asyncObj->scriptHandler = scriptHandler;
		//std::string imgPath = cocos2d::CCFileUtils::sharedFileUtils()
		//	->fullPathForFilename(textureAtlas->textureAtlasData->imagePath.c_str());
		//(_asyncList)[imgPath] = asyncObj;
		//CCLOG("loadDataAsyncImpl,size:%d", _asyncList.size());
		//if (pObj) pObj->retain();
		//printTickCount();
		//cocos2d::TextureCache::getInstance()
		//	->addImageAsync(imgPath.c_str(),
		//	(cocos2d::CCObject*)msCCDBManager,
		//	cocos2d::SEL_CallFuncO(&CCDBManager::loadTextureCallback));
	}
}

void DBCCManager::loadTextureCallback(cocos2d::CCObject *pObj)
{
	//cocos2d::CCTexture2D* texture = static_cast<cocos2d::CCTexture2D*>(pObj);
	//const char* textureKey = cocos2d::TextureCache::getInstance()->keyForTexture(texture);
	////CCLOG("%s textureKey:%s object:%d", __FUNCTION__, textureKey, pObj);
	////for(auto kename : _asynclist)
	////{
	////    cclog("first:%s, second:", kename.first.c_str());
	////}
	//CCLOG("CCDBManager::loadTextureCallback size %d", _asyncList.size());
	//CCLOG("CCDBManager::loadTextureCallback empty %d", _asyncList.empty());
	//printTickCount();
	//if (_asyncList.empty())
	//	CCLOG("asyncList is empty");
	//if (textureKey)
	//{
	//	std::string keyName(textureKey);
	//	auto iter = _asyncList.find(keyName);
	//	if (iter != _asyncList.end())
	//	{
	//		auto asyncObj = iter->second;
	//		_factory.addTextureAtlas(new DBCCTextureAtlas(asyncObj->pData));
	//		doAsyncCallBack(asyncObj->pObj, asyncObj->pSelector, asyncObj->scriptHandler);
	//		delete asyncObj;
	//		_asyncList.erase(keyName);
	//	}
	//}
}

void DBCCManager::unloadData(const std::string &skeletonName, const std::string &textureAtlasName)
{
	_factory->removeDragonBonesData(skeletonName);
	std::string texName(textureAtlasName.empty() ? skeletonName : textureAtlasName);
	DBCCTextureAtlas *texture = static_cast<DBCCTextureAtlas*>(_factory->getTextureAtlas(textureAtlasName));
	if (texture)
	{
		cocos2d::Texture2D* text= texture->getTexture();
		cocos2d::TextureCache::getInstance()
			->removeTexture(text);
	}
	_factory->removeTextureAtlas(texName);
	//CCLOG("Dragon in skeleton cache:%d", existSkeletonDataInDic("Dragon"));
	//CCLOG("Dragon in texture cache:%d", existTextureDataInDic("Dragon"));
}

DBCCFactory* DBCCManager::getFactory()
{
	return _factory;
}

NAME_SPACE_DRAGON_BONES_END