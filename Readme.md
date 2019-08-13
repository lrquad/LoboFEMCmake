# LoboFEM
## Introduction
Imgui OpenGL libigl c++14 for imfilebrowser.h
## Install

    cd external
    git clone https://github.com/eigenteam/eigen-git-mirror.git

    cd ..

    mkdir build
    cd build
    cmake ..
    make
    ../bin/LoboFEM
## Structure
- /LoboScene store surface mesh and volumetricmesh
- /LoboMesh surface mesh load from obj
- /LoboVolumetricMesh for volumetric mesh

## Note
    sudo nano /etc/modprobe.d/zz-nvidia-modeset.conf
    options nvidia_drm modeset=1
    sudo update-initramfs -u
This helps to remove tearing on my laptop. However, it locks FPS to 60fps.
My laptop: Razer Blade RTX 2070




