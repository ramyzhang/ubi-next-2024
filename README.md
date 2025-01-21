# The Big Catch: C++ 3D Game

This is my submission for Ubisoft's NEXT 2024-2025 challenge this year!
I began working on the engine on December 14th, 2024, and completed the game submission on January 20th :)

![ezgif-5-3b7335a1fe](https://github.com/user-attachments/assets/9503bdea-e39a-4e8e-956a-085794c5c47f)

Video demo: https://youtu.be/JIPNhrzT12A?si=B-tTXGQSRBKkDaIk

Completed features:
- Flocking with boids :D
- Grid-based level generation
- Music and sound effects self-made on Beepbox!
- 3D renderer (projection, triangles, rasterizer, lighting, backface culling, transformations, triangle clipping)
- .obj file loading
- Free camera, fixed target camera, "parallax-clamp" camera
- Scene management for levels
- Tiny Vector3 and Matrix4x4 math library
- Core game loop
- Entity manager (game object creation), entity memory pool, custom entity iterator
- Components, component memory pools
- UI system
- Basic physics (mostly sphere and AABB collision w/ impulse, rigidbodies, basic dynamics, drag)

Features that I planned but shelved:
- Octree spatial partitioning to improve z-depth rendering, and also optimize collision detection - decided to prioritize other things
- Obstacle avoidance for the boids - required the above to be feasible performance-wise
- More audio features, like spatial panning and effects - the given SimpleSound interface we were allowed to use was sadly only limited to starting, looping, and stopping sounds
