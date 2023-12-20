--- init utils package
-- @author zrong(zengrong.net)
-- Creation 2014-01-02
local utils = cc.utils or {}

utils.ByteArray = import(".ByteArray")
utils.ByteArrayVarint = import(".ByteArrayVarint")
utils.Gettext = import(".Gettext")

return utils
