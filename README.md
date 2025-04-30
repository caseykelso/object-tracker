# Overview

# Setup Ubuntu 24.04 Build Environment
```bash
sudo apt update -y && sudo apt install nlohmann-json3-dev cmake git build-essential googletest exuberant-ctags -y
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

