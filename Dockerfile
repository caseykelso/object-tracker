FROM ubuntu:25.04

# Install common tools
RUN apt-get update && apt-get install -y \
    openssh-client \
    git \
    curl \
    wget \
    build-essential \
    libgtest-dev \
    exuberant-ctags \
    libopencv-dev \
    nlohmann-json3-dev \
    cmake \
    libdocopt-dev \
    libcaca-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create .ssh directory to mount into
RUN mkdir -p /root/.ssh && chmod 700 /root/.ssh

# Set working directory
WORKDIR /root

# Run bash shell
CMD ["/bin/bash"]

