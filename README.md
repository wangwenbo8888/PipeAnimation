# PipeAnimation（弯管机加工模拟系统）

基于 **Qt 4 + OpenSceneGraph** 的弯管机三维加工模拟软件。

构建弯管机三维数字模型（轮模、夹模、机头、小车、导架等），载入加工文件
（YBC 数据 / 弯弧流程 / 生产参数表 / G 代码），按加工文件驱动整机动画模拟弯管加工
全过程（直管/弯管逐段生成、左右弯切换、多机头加工），过程中**实时碰撞检测**
（基于 Bullet Physics 的碰撞接口，模具与管子均参与），发现碰撞后可调整加工参数并
重新模拟验证，最终可将加工成型的管子导出为 STL。

> English summary: a Qt4 + OpenSceneGraph based 3D simulation system for rotary-draw
> pipe bending machines. It builds the machine's digital model, loads machining files,
> replays the whole bending process with real-time collision detection (Bullet Physics),
> and lets the operator tune parameters and re-simulate. Based on the open-source
> [osGraphX](https://github.com/remi.regnault/osg3DViewer) viewer.

## 功能模块

| 模块 | 说明 |
|---|---|
| 三维数模 | STL 模型载入与分组、程序化生成轮模/夹模/直管/弯管几何、双机头模具排布 |
| 加工文件 | YBC 数据、弯弧流程（TWLC）、生产参数表、G 代码、循环工艺、公式引擎 |
| 过程模拟 | 各部件动画轨迹（OSG 回调驱动）、管子逐段生成（直/弯、左/右弯、多机头）、变速播放 |
| 碰撞检测 | 定时器驱动的实时检测、碰撞分组/掩码、碰撞对记录与导出 |
| 参数闭环 | 加工值/轮模表编辑、自动优化、重写加工文件、再模拟验证、管子 STL 导出 |
| 其他 | 普通用户/高级用户双模式、机器码授权、多语言、模型树/拾取/着色器等查看器功能 |

## 构建指南

### 前置条件

| 依赖 | 版本/说明 |
|---|---|
| CMake | ≥ 3.15 |
| Visual Studio | 2010 及以上，**Win32 (x86)** 平台（自定义 SDK 为 32 位） |
| Qt | 4.8.x（QtCore/QtGui/QtXml/QtOpenGL） |
| OpenSceneGraph | 3.6.x 预编译 SDK（Win32），需包含 **osgQt**（3.6 之后主线已移除 osgQt，请用 3.6.5） |
| 自定义 SDK | **已内置**：`src/osg3DViewer/3rdparty/osgVirtualAssembly/`（osgDragger / osgVirtualAssembly / osgCollisionDetection / Utils，Win32 预编译库） |
| Bullet | **已内置**：`src/osg3DViewer/3rdparty/bullet-2.88/`（仅头文件） |

### 编译步骤

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A Win32 ^
  -DQT_QMAKE_EXECUTABLE=C:/Qt/4.8.6/bin/qmake.exe ^
  -DOPENSCENEGRAPH_INCLUDE_DIR=C:/path/to/osg-3.6.5/include ^
  -DOPENSCENEGRAPH_LIB_DIR=C:/path/to/osg-3.6.5/lib
cmake --build . --config Release
```

- 老环境（VS2010 时代）亦可：`cmake .. -G "Visual Studio 10 2010"`。
- 构建完成后自定义 SDK 的 DLL 会自动拷贝到输出目录（`build/bin`）。
- 运行时需要 OSG / Qt 的 DLL 以及 `data/` 资源目录（见主 CMakeLists 的
  `EXECUTABLE_OUTPUT_PATH`）。

### 目录结构

```
CMakeLists.txt            # 根构建脚本
src/osg3DViewer/          # 全部源码 + 模块级 CMakeLists
  3rdparty/
    osgVirtualAssembly/   # 内置自定义 SDK（头文件 + Win32 库）
    bullet-2.88/          # 内置 Bullet 2.88 头文件
    osgWorks/             # 内置 osgWorks（源码编译）
    qtpropertybrowser/    # 内置 Qt Property Browser（源码编译）
ui/                       # Qt Designer 界面文件
res/                      # 图标、qrc、rc 资源
data/                     # 运行时数据（shader/贴图/枢轴模型）
```

## 许可

基于 osGraphX（GPL v2）二次开发，请遵循 GPL v2。
内置第三方库遵循各自许可（osgWorks: OSGPL；qtpropertybrowser: BSD；Bullet: zlib）。
