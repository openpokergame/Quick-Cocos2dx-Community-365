# Quick-Cocos2dx-Community 3.6.5

Quick-Cocos2dx-Community 发展自用户公认的最稳定版本 Quick-Cocos2d-x v3.3。由于官方疏于维护，并表示不在发布v3.5以后的版本。原作者因为公司战略调整和所属部门的变化，无法将精力投入到引擎上，导致引擎全部合并到c++仓库中。我们并不赞成Lua分支合并到Cocos2d-x主版本中，因为我们使用Lua作为主要开发语言，并不需要工程中包含js等部分。

Quick-Cocos2dx-Community 具有 Cocos2d-x 和 Quick-Cocos2d-x 的所有优点。并在 Quick-Cocos2d-x 基础上更新了最新的平台和第三方功能。让开发者可以用更低的成本、更高的开发效率、更省的人力成本创造高品质手游产品。

## 官方网站

[http://www.cocos2d-lua.org/](http://www.cocos2d-lua.org/)

telegram 群: [https://t.me/+yhTso4XiPPs4ODI9](https://t.me/+yhTso4XiPPs4ODI9)

## 相关开源仓库地址

* 引擎代码仓库: [Quick-Cocos2dx-Community](https://github.com/u0u0/Quick-Cocos2dx-Community)
* 第三方库编译框架: [Quick-3rd-party-libs](https://github.com/u0u0/Quick-3rd-party-libs)
* vim开发插件: [vim-quick-community](https://github.com/u0u0/vim-quick-community)
* sublime开发插件: [quick-comminuty-dev](https://github.com/u0u0/quick-comminuty-dev)

## 快速安装指南

1. 下载 Quick-Cocos2dx-Community 最新版，并解压到一个路径。
	> 解压的路径以及文件夹名称**不要有中文或空格**.
2. Mac系统：在终端中执行：setup_mac.sh
	> 命令前*不需要*加入sudo。当提示输入密码的时候，请输入当前用户的登陆密码。 如果遇到新建的工程mac_ios项目不能编译通过，多是Xcode环境变量设置失败，建议在终端手动输入下面的命令。

	```
    defaults write com.apple.dt.Xcode IDEApplicationwideBuildSettings -dict
	# 路径替换为自己的Quick root
	defaults write com.apple.dt.Xcode IDEApplicationwideBuildSettings -dict-add QUICK_V3_ROOT "/User/u0u0/Quick_cocos2dx_Community"
	defaults write com.apple.dt.Xcode IDESourceTreeDisplayNames -dict
	defaults write com.apple.dt.Xcode IDESourceTreeDisplayNames -dict-add QUICK_V3_ROOT QUICK_V3_ROOT
   ```

   Windows系统：双击setup_win.bat即可。
3. 启动 player
	> 运行步骤2以后，脚本会自动创建player的快捷方式。

	 Mac 系统在 Applications 文件夹(或系统应用列表)中 player.app 。
	 Windows 系统在桌面上的 player.exe 快捷方式。
	
	

# 这版本已加入官方的patch，修复了以下问题

1. Xcode 8.2之后，运行iOS模拟器崩溃。 补丁下载：https://pan.baidu.com/s/1pKIUf0J 替换 Quick-Cocos2dx-Community/external/lua/luajit/prebuilt/ios/libluajit.a
2. spine 事件注销的错误修正。 补丁下载：https://pan.baidu.com/s/1gePWa6R
3. 多摄像机渲染bugfix。 补丁下载：https://pan.baidu.com/s/1c1V1KJm
4. 矩形拆剪嵌套渲染混乱bugfix。 补丁下载：https://pan.baidu.com/s/1o7QxGSi
5. RenderTexture的渲染中有矩形拆剪的bugfix。 补丁下载：https://pan.baidu.com/s/1bpMXDUn
6. MacOS 10.15 系统TTF字体花屏, 参考commit:[ macos Catalina 系统ttf显示修正](https://github.com/u0u0/Quick-Cocos2dx-Community/commit/6145679dd278371e841231c1d6657eb943755647)。 下载最新的[cocos/platform/mac/CCDevice-mac.mm](https://github.com/u0u0/Quick-Cocos2dx-Community/blob/master/cocos/platform/mac/CCDevice-mac.mm),替换并重现编译Player.