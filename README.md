**StarTalk Pc 2.0 客户端**
=====
StarTalk Pc 2.0 是基于StarTalk开源服务的Pc客户端, 支持Windows、Linux、Mac三个平台，除了部分平台性质的差别外，UI和功能都一致。
与1.0相比，除了支持更多平台外，UI、性能等各个方面都得到了有效提升。

Release版本下载地址: [StarTalk官网](https://im.qunar.com/#/download)

### 开发环境搭建 
___
依赖的第三方库

名称  | 版本要求  | 下载地址 | 备注
 ---- | ----- | ------ | ---
 Qt  | 5.9.6及以上 | https://download.qt.io/archive/qt/ | 建议保持新版本 (ubuntu可以使用apt-get方式下载、MacOS可以使用brew方式下载)
 protobuf  | 2.6.1 | | 建议使用此版本
 zlib | 1211 | | 
 openssl | 1.1.1 | | 
 curl | 7.61.0 | | 
 qzxing |  | https://github.com/ftylitak/qzxing | 项目中包含源码
 quazip |  | https://github.com/stachenov/quazip | 项目中包含源码
 EventBus |  | https://github.com/greenrobot/EventBus | 项目中包含源码
 cJson | | https://github.com/DaveGamble/cJSON | 项目中包含源码 
 
1. Windows 操作系统
 ① Visual Studio 2017/2019 
 ② Cmake
 ③ Clion(IDE建议使用Clion, 也可以根据cmakelist 写vs项目, 或者使用记事本)
 
2. Linux/MacOS 操作系统
 ① Cmake
 ② UUID
 ③ Clion 或者使用VIM
 
Tips: 

1> protobuf、zlib、openssl、curl几个库因为编译比较复杂，我们提供了已经编译成功的版本，位置 ${root_dir}/Depends/${name}，其他平台需要自己build

2> 安装Qt时请选择 QtWebEngine、QtMultimedia、QtMultimedia Widgets、QtQuick、QtQuick Controls2、QtSvg、QtWebChannel、QtPositioning、QtMacExtras(MacOS)、QtX11Extras(Linux)等模块 

### 源码编译

```
#进入源文件根目录
$ cd ${root_dir}
$ mkdir build_release
$ cd build_release
$ cmake -DCMAKE_BUILD_TYPE=Debug/Release -DSTARTALK=ON .. #根据自己需要修改build版本
$ make
```

可执行文件会生成到 ${root_dir}/build_release/bin 目录下

tips: 

1> 需要修改跟目录下的CMakeList.txt 中第三方的路径 根据自己的安装位置修改

2> Windows操作系统下需要设置Qt、Cmake的全局系统变量

3> Windows操作系统下需要使用Visual Studio的工具编译( VS 2017/2019 的开发人员命令提示符 )

### 关于更新

此版本不包含版本升级功能、我们会在功能完善之后放出。

### 联系我们
1> issue 

2> 我们的[官网](https://im.qunar.com/#/) 留言