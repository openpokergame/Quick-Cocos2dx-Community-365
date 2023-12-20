if db==nil or db.DBCCFactory==nil then return nil end

local dblua = {}

local dbFactory = db.DBCCFactory:getInstance()

function dblua.new(params)
	local path = params.path
	local skeFile = params.skeleton
	local texFile = params.texture
	local armName = params.armatureName
	local aniName = params.animationName or ""
	local skeName = params.skeletonName or armName
	local texName = params.textureName or skeName
	local skinName = params.skinName or ""
	assert(armName and skeName, "armatureName and skeletonName are necessary!")
	if async then
		local newDBHandler = function()
			params.handler(
				dblua.new({
					armatureName=armName, 
					animationName=aniName, 
					skeletonName=skeName, 
					textureName = texName,
					skinName = skinName,
			}))
		end
		dblua.loadData(
			{
				path = path,
				skeleton = skeFile,
				texture = texFile,
				armatureName=armName, 
				animationName=aniName, 
				skeletonName=skeName, 
				textureName = texName,
				skinName = skinName,
				handler = newDBHandler,
			})
		return nil
	end
	if path then		
		return dblua.createByDir(path, armName, aniName, skeName, texName, skinName)
	end
	if skeFile and texFile then
		return dblua.createByFiles(skeFile, texFile, armName, aniName, skeName, texName,skinName)
	end
	return dblua.createByName(armName, aniName, 
		skeName, texName,
		skinName)
end

function dblua.loadData(params)
	local path = params.path
	local skeFile = params.skeleton
	local texFile = params.texture
	local armName = params.armatureName
	local aniName = params.animationName or ""
	local skeName = params.skeletonName or armName
	local texName = params.textureName or skeName
	local skinName = params.skinName or ""

	local async = params.handler and type(params.handler) == "function"
	if path then
		if async then
			dblua.loadDataByDirAsync(
				path, 
				skeName, texName, 
				params.handler)
		else
			dblua.loadDataByDir(path, skeName, texName)
		end
	else
		assert(skeFile and texFile, "Please provide skeleton and texture file!")
		if async then
			dblua.loadDataAsync(
				skeFile, texFile, 
				skeName, texName, 
				params.handler)
		else
			dblua.loadData(
				skeFile, texFile, 
				skeName, texName)
		end
	end
end

function dblua.loadDataList(fileList, handler)
	local total = #fileList
	local cur = total 
	local aHandler = nil
	if type(handler) == "function" then
		aHandler = function(evt)
			cur = cur - 1
			if cur <= 0 then
				handler(true, cur, total)
			else
				--handler(false, cur, total)
			end
		end
	end
	for __, adb in ipairs(fileList) do
		if aHandler then
			local param = clone(adb)
			param.handler = aHandler
			db.loadData(param)
		else
			db.loadData(adb)
		end
	end
end

function dblua.unloadData(params)
	local skeName = params.skeletonName
	local texName = params.textureName or skeName

	dbFactory:removeDragonBonesData(skeName)
	dbFactory:removeTextureAtlas(texName)
end

function dblua.unloadDataList(fileList)
	for __, v in ipairs(fileList) do
		dblua.unloadData(v.skeletonName, v.textureName)
	end
end

function dblua.loadDataByDirAsync(path, skeName, texName, callback)
	dblua.loadDataAsync(path .. "skeleton.xml", path .. "texture.xml", skeName, texName, callback)
end

function dblua.loadDataByDir(path, skeName, texName)
	dblua.loadData(path .. "skeleton.xml", path .. "texture.xml", skeName, texName)
end

function dblua.loadDataAsync(skeFile, texFile, skeName, texName, callback)
	--dbFactory:loadDragonBonesData(skeFile,skeName)
	--if dbFactory:getTextureAtlas(skeName) then
	--	dbFactory:loadTextureAtlas(texFile,texName)
	--	if callback then
	--		callback()
	--	end
	--else
	--	
	--end
	dblua.loadDataAsync(skeFile, texFile, skeName, texName)
	
end

function dblua.loadData(skeFile, texFile, skeName, texName)
	if skeFile ~= "" then
		dbFactory:loadDragonBonesData(skeFile,skeName)
	end
	if texFile ~= "" then
		dbFactory:loadTextureAtlas(texFile,texName)
	end
end

function dblua.createByDir(path, armName, aniName, skeName, texName, skinName)
	return dblua.createByFiles(path .. "skeleton.xml", path .. "texture.xml",armName, aniName, skeName, texName, skinName)
end

function dblua.createByFiles(skeFile, texFile, armName, aniName, skeName, texName,skinName)
	dblua.loadData(skeFile, texFile, skeName, texName)
	local armature = dbFactory:buildArmature(armName,skinName,aniName,skeName,texName)
	return db.DBCCArmatureNode:create(armature)
end

function dblua.createByName(armName, aniName, skeName, texName,skinName)
	local armature = dbFactory:buildArmature(armName,skinName,aniName,skeName,texName)
	return db.DBCCArmatureNode:create(armature)
end

return dblua