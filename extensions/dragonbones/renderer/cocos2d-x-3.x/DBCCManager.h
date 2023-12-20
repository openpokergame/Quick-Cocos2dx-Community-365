#ifndef DBCC_MANAGER_H
#define DBCC_MANAGER_H

#include "DragonBonesHeaders.h"
#include "DBCCRenderHeaders.h"
#include "cocos2d.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class DBCCManager : public cocos2d::CCObject
{
private:
	DBCCManager();
	~DBCCManager();
public:
	static DBCCManager *getInstance();
	static void destroyInstance();
	static void parseXMLByDir(const std::string& path, std::string &skeletonXMLFile, std::string &textureXMLFile);

	inline Armature* createArmatureByDir(const std::string &path,
		const std::string &armatureName,
		const std::string &animationName = "",
		const std::string &skeletonName = "",
		const std::string &textureAtlasName = "",
		const std::string &skinName = "")
	{
		std::string skeletonFile(""), textureFile("");
		parseXMLByDir(path, skeletonFile, textureFile);
		return createArmatureByFiles(skeletonFile, textureFile,
			armatureName, animationName,
			skeletonName, textureAtlasName, skinName);
	};

	inline Armature* createArmatureByFiles(const std::string &skeletonXMLFile,
		const std::string &textureXMLFile,
		const std::string &armatureName,
		const std::string &animationName = "",
		const std::string &skeletonName = "",
		const std::string &textureAtlasName = "",
		const std::string &skinName = "")
	{
		loadData(skeletonXMLFile, textureXMLFile,
			skeletonName, textureAtlasName);
		return _factory->buildArmature(armatureName, animationName,
			skeletonName, textureAtlasName, skinName);
	};

	inline void loadDataByDir(const std::string &path,
		const std::string &skeletonName,
		const std::string &textureAtlasName)
	{
		std::string skeletonFile(""), textureFile("");
		parseXMLByDir(path, skeletonFile, textureFile);
		loadData(skeletonFile, textureFile, skeletonName, textureAtlasName);
	};
	void loadData(const std::string &skeletonFile, const std::string &textureAtlasFile,
		const std::string &skeletonName, const std::string &textureAtlasName);

	inline void loadDataByDirAsync(const std::string &path,
		const std::string &skeletonName,
		const std::string &textureAtlasName,
		cocos2d::CCObject* pObj,
		cocos2d::SEL_CallFuncO selector)
	{
		std::string skeletonFile(""), textureFile("");
		parseXMLByDir(path, skeletonFile, textureFile);
		loadDataAsync(skeletonFile, textureFile,
			skeletonName, textureAtlasName,
			pObj, selector);
	};

	inline void loadDataByDirAsync(const std::string &path,
		const std::string &skeletonName,
		const std::string &textureAtlasName,
		int scriptHandler)
	{
		std::string skeletonFile(""), textureFile("");
		parseXMLByDir(path, skeletonFile, textureFile);
		loadDataAsync(skeletonFile, textureFile,
			skeletonName, textureAtlasName,
			scriptHandler);
	};

	inline void loadDataAsync(const std::string &skeletonFile,
		const std::string &textureAtlasFile,
		const std::string &skeletonName,
		const std::string &textureAtlasName,
		cocos2d::CCObject* pObj,
		cocos2d::SEL_CallFuncO selector)
	{
		loadDataAsyncImpl(skeletonFile, textureAtlasFile,
			skeletonName, textureAtlasName,
			pObj, selector, 0);
	};

	inline void loadDataAsync(const std::string &skeletonFile,
		const std::string &textureAtlasFile,
		const std::string &skeletonName,
		const std::string &textureAtlasName,
		int scriptHandler)
	{
		loadDataAsyncImpl(skeletonFile, textureAtlasFile,
			skeletonName, textureAtlasName,
			nullptr, nullptr, scriptHandler);
	};

	void unloadData(const std::string &skeletonName, const std::string &textureAtlasName = "");
	DBCCFactory* getFactory();

private:
	struct AsyncStruct
	{
		const char* imagePath;
		TextureAtlasData* pData;
		cocos2d::CCObject* pObj;
		cocos2d::SEL_CallFuncO pSelector;
		int scriptHandler;
	};
	void loadDataAsyncImpl(const std::string &skeletonFile,
		const std::string &textureAtlasFile,
		const std::string &skeletonName,
		const std::string &textureAtlasName,
		cocos2d::CCObject* pObj,
		cocos2d::SEL_CallFuncO selector,
		int scriptHandler = 0);
	std::map<std::string, AsyncStruct*> _asyncList;
	DBCCFactory* _factory = DBCCFactory::getInstance();
	void loadTextureCallback(cocos2d::CCObject* pObj);
	void printTickCount();


};

NAME_SPACE_DRAGON_BONES_END
#endif // __CCDBFACTORY_H__