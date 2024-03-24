# deb简介

deb是Unix系统(其实主要是Linux)下的安装包，基于 tar 包，因此本身会记录文件的权限(读/写/可执行)以及所有者/用户组。
由于 Unix 类系统对权限、所有者、组的严格要求，而 deb 格式安装包又经常会涉及到系统比较底层的操作，所以权限等的设置尤其重要。

## deb包使用方法

dpkg -c xxx.deb // 安装前根据deb文件查看
dpkg -L debname // 安装后根据包名查看
dpkg -i xxx.deb //安装deb包，如果提示错误，可以加参数—force-all强制安装，但不推荐这样做
dpkg -r debname or dpkg --purge to remove debname //移除deb包
dpkg -S filepath //查看某个文件属于哪个deb包
dpkg -X xxx.deb dirname //释放安装内容到dirname目录中
dpkg -e xxx.deb  //释放控制信息到当前目录下的DEBIAN子目录中
dpkg -l | grep debname  // 查找已经安装的deb包

## 控制文件 描述

control  用于记录软件标识，版本号，平台，依赖信息等数据
preinst  在解包data.tar.gz前运行的脚本
postinst 包含了软件在进行正常目录文件拷贝到系统后，所需要执行的配置工作。
prerm   卸载时，在删除文件之前运行的脚本
postrm   在删除文件之后运行的脚本
解释:
dpkg -i xxx.deb 执行时依次运行 preinst postinst
dpkg -r debname 执行时依次运行 prerm   postrm

control文件：描述软件包的名称（Package），版本（Version），描述（Description）等，是deb包必须剧本的描述性文件，以便于软件的安装管理和索引。
Package 包名Version 版本
Architecture：软件包结构，如基于i386, amd64,m68k, sparc, alpha, powerpc等
Priority：申明软件对于系统的重要程度，如required, standard, optional, extra等
Essential：申明是否是系统最基本的软件包（选项为yes/no），如果是的话，这就表明该软件是维持系统稳定和正常运行的软件包，不允许任何形式的卸载（除非进行强制性的卸载）
Section：申明软件的类别，常见的有utils, net, mail, text, devel 等Depends：软件所依赖的其他软件包和库文件。如果是依赖多个软件包和库文件，彼此之间采用逗号隔开
Pre-Depends：软件安装前必须安装、配置依赖性的软件包和库文件，它常常用于必须的预运行脚本需求
Recommends：这个字段表明推荐的安装的其他软件包和库文件
Suggests：建议安装的其他软件包和库文件
Description：对包的描述
Installed-Size：安装的包大小
Maintainer：包的制作者，联系方式等

## 目录结构如演示该文件 mydeb 中的一样

boot  目录下放需要安装的软件
DEBIAN  目录下是 deb 包的控制文件描述

## 打包命令

第一个参数为将要打包的目录名
第二个参数为生成包的名称
dpkg -b mydeb mydeb.deb
