# sysmon-agent
Custom deployable Linux system monitor

## Getting Started

### Prerequisites
- CMake 3.14+
- C++17 compiler (GCC / Clang)
- `libncurses-dev` / `libncursesw5-dev`

### Building from Source
```bash
# Clone and configure build
cmake -B build -DSYSMON_BUILD_TESTS=ON

# Compile binary and tests
cmake --build build -j$(nproc)
```

### Running the Agent
- **Interactive TUI Dashboard**:
  ```bash
  ./build/sysmon
  ```
- **Headless Service Mode (Prometheus Exporter)**:
  ```bash
  ./build/sysmon --service
  ```
- **Verify Metrics**:
  ```bash
  curl http://localhost:9100/metrics
  ```

### Running with Docker
To build and run `sysmon` inside a Docker container (with access to monitor Docker containers via the mounted host socket):
```bash
# Build the Docker image
docker build -f deploy/Dockerfile -t sysmon-agent .

# Run the container
docker run -d \
  --name sysmon \
  -p 9100:9100 \
  -v /var/run/docker.sock:/var/run/docker.sock \
  sysmon-agent
```


