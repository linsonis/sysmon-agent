#include "filesystem.hpp"
#include "../metrics/registry.hpp"
#include <sys/statvfs.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>

namespace sysmon {

void FilesystemCollector::collect() {
    std::ifstream file("/proc/mounts");
    if (!file.is_open()) {
        return;
    }

    // Set of target filesystem types to report on
    static const std::unordered_set<std::string> target_fstypes = {
        "ext4", "ext3", "ext2", "xfs", "btrfs", "zfs", "vfat", "exfat", "ntfs", "tmpfs"
    };

    std::unordered_set<std::string> visited_mounts;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string device, mount_point, fstype;
        if (ss >> device >> mount_point >> fstype) {
            // Skip non-target filesystems or duplicate mount points
            if (target_fstypes.find(fstype) == target_fstypes.end()) {
                continue;
            }
            if (visited_mounts.find(mount_point) != visited_mounts.end()) {
                continue;
            }
            visited_mounts.insert(mount_point);

            struct statvfs vfs;
            if (statvfs(mount_point.c_str(), &vfs) == 0) {
                double block_size = static_cast<double>(vfs.f_frsize);
                double total_bytes = static_cast<double>(vfs.f_blocks) * block_size;
                double free_bytes = static_cast<double>(vfs.f_bfree) * block_size;
                double avail_bytes = static_cast<double>(vfs.f_bavail) * block_size;
                double used_bytes = total_bytes - free_bytes;

                double total_inodes = static_cast<double>(vfs.f_files);
                double free_inodes = static_cast<double>(vfs.f_ffree);

                std::unordered_map<std::string, std::string> labels = {
                    {"mount", mount_point},
                    {"fstype", fstype},
                    {"device", device}
                };

                MetricRegistry::instance().update_metric("sysmon_filesystem_size_bytes", total_bytes, labels);
                MetricRegistry::instance().update_metric("sysmon_filesystem_free_bytes", free_bytes, labels);
                MetricRegistry::instance().update_metric("sysmon_filesystem_avail_bytes", avail_bytes, labels);
                MetricRegistry::instance().update_metric("sysmon_filesystem_used_bytes", used_bytes, labels);
                MetricRegistry::instance().update_metric("sysmon_filesystem_inodes_total", total_inodes, labels);
                MetricRegistry::instance().update_metric("sysmon_filesystem_inodes_free", free_inodes, labels);
            }
        }
    }
}

} // namespace sysmon
