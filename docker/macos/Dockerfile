# macOS cross build image based on osxcross and the official prebuilt Qt for macOS.
#
# The macOS SDK is not redistributable and is therefore passed in as the
# additional build context "macos-sdk" (see docker-compose.yml). It must point
# to an unpacked MacOSX<version>.sdk directory, e.g. from the Xcode command line
# tools of the host.
FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates curl git patch xz-utils bzip2 cpio \
        clang llvm-dev lld cmake ninja-build make python3 python3-venv \
        libssl-dev liblzma-dev libxml2-dev zlib1g-dev uuid-dev libbz2-dev \
        libglib2.0-0t64 \
    && rm -rf /var/lib/apt/lists/*

# --- osxcross -----------------------------------------------------------------
ARG MACOS_SDK_VERSION=15.4
ARG OSXCROSS_REVISION=master
RUN git clone https://github.com/tpoechtrager/osxcross.git /opt/osxcross-src \
    && git -C /opt/osxcross-src checkout "$OSXCROSS_REVISION"
COPY --from=macos-sdk . /tmp/MacOSX${MACOS_SDK_VERSION}.sdk
RUN cd /tmp && tar -cJf /opt/osxcross-src/tarballs/MacOSX${MACOS_SDK_VERSION}.sdk.tar.xz \
        MacOSX${MACOS_SDK_VERSION}.sdk \
    && rm -rf /tmp/MacOSX${MACOS_SDK_VERSION}.sdk \
    && cd /opt/osxcross-src \
    && TARGET_DIR=/opt/osxcross UNATTENDED=1 ./build.sh \
    && rm -rf /opt/osxcross-src
ENV PATH=/opt/osxcross/bin:$PATH

# --- Qt -----------------------------------------------------------------------
# Prebuilt Qt for macOS plus the matching Linux host tools (moc, uic, rcc).
ARG QT_VERSION=6.8.3
ARG TARGETARCH
RUN python3 -m venv /opt/aqt \
    && /opt/aqt/bin/pip install --no-cache-dir aqtinstall \
    && /opt/aqt/bin/aqt install-qt mac desktop "$QT_VERSION" clang_64 -m qtimageformats -O /opt/qt \
    && if [ "$TARGETARCH" = "arm64" ]; then \
           /opt/aqt/bin/aqt install-qt linux_arm64 desktop "$QT_VERSION" linux_gcc_arm64 -O /opt/qt; \
       else \
           /opt/aqt/bin/aqt install-qt linux desktop "$QT_VERSION" linux_gcc_64 -O /opt/qt; \
       fi \
    && ln -s "$(find /opt/qt/$QT_VERSION -maxdepth 1 -name 'gcc_*' | head -n1)" /opt/qt/host \
    && ln -s /opt/qt/$QT_VERSION/macos /opt/qt/macos \
    && rm -rf /opt/qt/*/*/doc /opt/qt/*/*/examples

# --- rcodesign (ad-hoc signing of the bundle, required on Apple Silicon) -------
ARG RCODESIGN_VERSION=0.29.0
RUN case "$TARGETARCH" in arm64) arch=aarch64 ;; *) arch=x86_64 ;; esac \
    && curl -fsSL "https://github.com/indygreg/apple-platform-rs/releases/download/apple-codesign%2F${RCODESIGN_VERSION}/apple-codesign-${RCODESIGN_VERSION}-${arch}-unknown-linux-musl.tar.gz" \
        | tar -xz -C /usr/local/bin --strip-components=1 --wildcards '*/rcodesign'

ENV MACOS_SDK_VERSION=${MACOS_SDK_VERSION} \
    LANG=C.UTF-8
CMD ["/src/docker/macos/build.sh"]
