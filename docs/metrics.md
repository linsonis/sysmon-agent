# Sysmon Metrics Reference

Metrics ending in `_total` that hold cumulative kernel counters (network and disk) are exported with type `counter`; use `rate()` on them. Other metrics are gauges.

- `sysmon_cpu_usage_ratio`: CPU utilization ratio.
- `sysmon_memory_used_bytes`: Memory used in bytes.
- `sysmon_memory_total_bytes`: Total memory in bytes.
- `sysmon_process_count`: Total active processes.
- `sysmon_network_{receive,transmit}_{bytes,packets,errors,drop}_total` (counter, label `interface`): Cumulative network traffic.
- `sysmon_disk_{reads_completed,read_bytes,read_time_ms,writes_completed,write_bytes,write_time_ms,io_time_ms}_total` (counter, label `device`): Cumulative disk I/O.
- `sysmon_filesystem_{size,used,free,avail}_bytes`, `sysmon_filesystem_inodes_{total,free}` (gauge, labels `device`, `fstype`, `mount`): Filesystem usage.
- `sysmon_systemd_failed_units`: Number of failed systemd units.
- `sysmon_docker_up`: 1 if the Docker socket is reachable, 0 otherwise. Container metrics are only exported while this is 1.
- `sysmon_docker_containers_running`: Running docker containers count.
