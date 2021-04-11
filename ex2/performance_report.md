# Test System: Surface Pro 2

* OS: Arch Linux x86_64 (btw)
* Kernel: 5.3.12
* CPU: Intel i5-4300U (4) @ 2.900GHz
* Integrated "GPU": Intel Haswell-ULT
* Memory: 4 GiB

# Test conditions: No shader, moving light

Dolphin grid scene:
   442244 triangles
    immediate
      ~98ms on average per frame
      ~99ms when moving camera
    Array
      ~5ms on average per frame
      ~16ms when moving camera
    VBO
      ~3ms on average per frame
      ~15ms when moving camera

Dynamic scene
  1228140 triangles
    immediate
      ~355ms
    array
      ~85ms
    VBO
      ~77ms (9ms when not moving) on average per frame

# Test conditions: Shader 6, moving light

Dolphin grid scene:
   442244 triangles
    immediate
      ~96ms on average per frame
      ~97ms when moving camera
    Array
      ~5ms on average per frame
      ~16ms when moving camera
    VBO
      ~3ms on average per frame
      ~15ms when moving camera

Dynamic scene
  1228140 triangles
    immediate
      ~315ms
    array
      ~85ms
    VBO
      ~81ms (ms when not moving) on average per frame

(flushing the console with endl has a high impact on performance, too!)
