# osgVirtualAssembly SDK (prebuilt, Win32 x86)

This directory bundles the prebuilt custom SDK required to build **osg3DViewer / PipeAnimation**.
These libraries are **not** available in any public package repository — they are vendored here
so the project can build out of the box.

Original author of the libraries: **Liu zhiyu** (licensed under the OpenSceneGraph Public License, OSGPL).

## Contents

### `include/`

| Library | Description |
|---|---|
| `osgDragger/` | Drag/manipulation framework on top of `osgManipulator` (`DraggerManager`, `MyDragger`, `HudCamera`, ...) |
| `osgVirtualAssembly/` | Virtual assembly helpers (`CustomDraggerManager`, ...) |
| `osgCollisionDetection/` | Collision detection wrapper around **Bullet Physics** (`CollisionInterface`, `DebugDraw`, `ComputeTrianglesVisitor`) |
| `Utils/` | Shared utilities |

Note: the headers intentionally have **no file extension** (e.g. `#include <osgDragger/DraggerManager>`).

### `lib/` (Win32 / x86, release + debug)

- `osgDragger.lib/.dll`, `osgDraggerd.lib/.dll`
- `osgVirtualAssembly.lib/.dll`, `osgVirtualAssemblyd.lib/.dll`
- `osgCollisionDetection.lib/.dll`, `osgCollisionDetectiond.lib/.dll`

Debug variants carry the `d` postfix (same convention as OpenSceneGraph).
The CMake build copies the DLLs next to the executable automatically.

## Requirements

- The `osgCollisionDetection` headers include `<btBulletDynamicsCommon.h>` — the matching
  **Bullet 2.88 headers** are bundled at `../bullet-2.88/` (headers only, no linking needed:
  Bullet itself is already compiled into `osgCollisionDetection.dll`).
