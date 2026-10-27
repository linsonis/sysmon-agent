# Sysmon Metrics Reference

- `sysmon_cpu_usage_ratio`: CPU utilization ratio.
- `sysmon_memory_used_bytes`: Memory used in bytes.
- `sysmon_memory_total_bytes`: Total memory in bytes.
- `sysmon_process_count`: Total active processes.
- `sysmon_network_receive_bytes_total`: Network bytes received.
- `sysmon_disk_read_bytes_total`: Disk bytes read.
- `sysmon_filesystem_free_bytes`: Filesystem free space.
- `sysmon_systemd_failed_units`: Number of failed systemd units.
- `sysmon_docker_up`: 1 if the Docker socket is reachable, 0 otherwise. Container metrics are only exported while this is 1.
- `sysmon_docker_containers_running`: Running docker containers count.
