# LoboFEM
## Todo

08/26/2019
- [x] Fullspace simulation
  - [x] Hyperelastic model
    - [x] K and f
  - [x] Optimization solver
- [ ] Eigen MKL test on ubuntu

08/25/2019
- [x] Fullspace simulation
  - [x] Fullspace sovler
  - [x] Dynamic model
  - [x] Constraints model
  - [x] Hyperelastic model
    - [x] multi thread stiffness and internal forces
  - [x] Time integration
  - [x] Kinetic model
  - [x] Precomptue tetmesh

## Introduction
- Imgui
- OpenGL 
- libigl 
- c++14 (for imfilebrowser)
- hyperelastic material based FEM simulator

![bunny](https://github.com/lrquad/LoboFEMCmake/blob/master/demo/default/Images/bunny.gif)
![scene](https://github.com/lrquad/LoboFEMCmake/blob/master/demo/default/Images/scene.png)

## Install
    
    sudo apt-get install libomp-dev
    sudo apt-get install cmake libx11-dev xorg-dev libglu1-mesa-dev libglm-dev


    cd external
    git clone https://github.com/eigenteam/eigen-git-mirror.git

    cd ..

    mkdir build
    cd build
    cmake ..
    make
    ../bin/LoboFEM

## Structure
![Structure](https://github.com/lrquad/LoboFEMCmake/blob/master/demo/default/Images/LoboFEM.jpeg)



## Note
    sudo nano /etc/modprobe.d/zz-nvidia-modeset.conf
    options nvidia_drm modeset=1
    sudo update-initramfs -u
This helps to remove tearing on my laptop. However, it locks FPS to 60fps.
My laptop: Razer Blade RTX 2070

Update vertices buffer
6.1 FPS

Only call glBufferSubData
20 FPS

OpenMP 12 threads
19 FPS


https://stackoverflow.com/questions/993352/when-should-i-make-explicit-use-of-the-this-pointer
https://stackoverflow.com/questions/2812470/why-does-gcc-need-extra-declarations-in-templates-when-vs-does-not/2812501#2812501


