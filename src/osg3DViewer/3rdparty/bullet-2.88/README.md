# Bullet Physics 2.88 (headers only)

Header-only extraction of [bullet3](https://github.com/bulletphysics/bullet3) at tag **2.88**,
subtree `src/**/**.h` (389 files). zlib license — see the upstream repository for details.

## Why headers only?

The custom `osgCollisionDetection` library (bundled at `../osgVirtualAssembly/`) wraps Bullet
Physics and is **prebuilt** — Bullet is already compiled into `osgCollisionDetection.dll`.
The application itself only needs the Bullet *headers* at compile time (for
`btBulletDynamicsCommon.h` / `btBulletCollisionCommon.h` pulled in via `CollisionInterface`),
it never links against Bullet directly.

## Layout

```
src/                         <- add this directory to the include path
  btBulletCollisionCommon.h
  btBulletDynamicsCommon.h
  LinearMath/  BulletCollision/  BulletDynamics/  BulletSoftBody/  ...
Demos/OpenGL/GLDebugDrawer.h <- compat shim taken from bullet3 tag 2.82
                                (the osgCollisionDetection DebugDraw header
                                 includes <Demos/OpenGL/GLDebugDrawer.h>,
                                 which only existed in pre-2.83 layouts)
```

The build adds **both** `src/` and this directory (the repo root of the layout
above) to the include path.
