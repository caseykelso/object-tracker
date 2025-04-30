# Overview

# Setup Ubuntu 24.04 Build Environment
```bash
sudo apt update -y && sudo apt install libopencv-dev nlohmann-json3-dev cmake git build-essential libgtest-dev exuberant-ctags libdocopt-dev libcaca-dev -y
```
# Initial Build
```bash
make ci
```

# Build iteratively
```bash
make build
```
or
```bash
make
```

# Run
```bash
make run
```

# Tests
```bash
make tests
```

# Deploy
```bash
make deploy
```

# Ctag generation
```bash
make ctags
```

# Docker

## Build & Start & Run
```bash
docker-compose build
docker-compose up -d
docker-compose exec ubuntu bash
```
