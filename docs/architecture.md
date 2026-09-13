# Sysmon Architecture

Sysmon is a lightweight Linux system monitor service and interactive TUI written in C++17.

## Components
- **Collectors**: Read directly from `/proc`, `/sys`, and Docker API socket.
- **Metric Registry**: Thread-safe centralized store for counters and gauges.
- **Prometheus Exporter**: HTTP server (`cpp-httplib`) exposing `/metrics` in Prometheus format.
- **Scheduler**: Manages collection intervals.
- **ncurses TUI**: Interactive terminal dashboard when run without `--service`.
