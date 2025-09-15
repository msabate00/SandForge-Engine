# SandForge Engine

Motor de simulación *falling-sand* en C++17 con OpenGL 3.3 Core. Render con shaders, materiales por celdas, entrada con GLFW y audio básico. Multiplataforma vía CMake. Soporte principal en Windows x64.

## Características
- Simulación por celdas con *chunks*.
- Render en GPU con paleta y GLSL.
- Entrada de teclado y ratón (GLFW).
- Carga de recursos desde `assets/` junto al ejecutable.
- Helpers de shaders con logs de compilación/enlace.
- Build reproducible con CMake.

## Requisitos

### Mínimos
- **SO:** Windows 10 64-bit  
- **CPU:** 2 núcleos x64  
- **GPU:** OpenGL 3.3 compatible (ej. Intel HD 4000)  
- **RAM:** 2 GB  
- **Disco:** 100 MB  
- **Pantalla:** 1280×720  

### Recomendados
- **SO:** Windows 10/11 64-bit  
- **CPU:** 4 núcleos x64  
- **GPU:** OpenGL 4.x o superior (ej. Intel UHD 620 / GTX 750 / RX 460)  
- **RAM:** 8 GB  
- **Disco:** 200 MB  
- **Pantalla:** 1920×1080  

> Actualiza drivers. En portátiles con iGPU/dGPU fuerza GPU dedicada.
