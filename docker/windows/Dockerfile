# Windows cross build image using Fedora's MinGW-w64 Qt 6 packages.
FROM fedora:42

RUN dnf install -y --setopt=install_weak_deps=False \
        cmake \
        ninja-build \
        zip \
        mingw64-gcc-c++ \
        mingw64-qt6-qtbase \
        mingw64-qt6-qtsvg \
    && dnf clean all

CMD ["/src/docker/windows/build.sh"]
