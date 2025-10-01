/*
 * RBPak - Retroboi64's Package System
 * A compact yet powerful file packaging system for game engines
 * Copyright (c) 2025 Patrick Reese (Retroboi64)
 *
 * Licensed under MIT
 * See LICENSE file for full terms
 * GitHub: https://github.com/Retroboi64/RBPak
 */

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <string_view>
#include <span>
#include <functional>

namespace rbpak {
    using ByteArray = std::vector<uint8_t>;

    enum class CompressionLevel : uint8_t {
        None = 0,
        Fast = 1,
        Balanced = 6,
        Best = 9
    };

    enum class EncryptionMethod : uint8_t {
        None = 0,
        XOR = 1,
        AES = 2  // Future expansion
    };

    enum class PackageFlags : uint32_t {
        None = 0,
        Compressed = 1 << 0,
        Encrypted = 1 << 1,
        ObfuscatedNames = 1 << 2,
        ChecksumVerified = 1 << 3
    };

    enum class PackageError {
        None,
        FileNotFound,
        InvalidSignature,
        CorruptedData,
        DecryptionFailed,
        CompressionFailed,
        DecompressionFailed,
        ChecksumMismatch,
        IOError,
        InvalidParameter,
        OutOfMemory,
        AccessDenied
    };

    struct PackageResult {
        bool success;
        PackageError error;
        std::string message;

        static PackageResult Success() {
            return { true, PackageError::None, "" };
        }

        static PackageResult Failure(PackageError err, std::string_view msg) {
            return { false, err, std::string(msg) };
        }

        explicit operator bool() const { return success; }
    };

    struct PackageConfig {
        CompressionLevel compression{ CompressionLevel::Balanced };
        EncryptionMethod encryption{ EncryptionMethod::None };
        std::string encryption_key;
        bool obfuscate_filenames{ false };
        bool verify_checksums{ true };
        bool lazy_load{ true };
        size_t max_cache_size{ 100 * 1024 * 1024 }; // 100MB default cache

        static PackageConfig Default() {
            return PackageConfig{};
        }

        static PackageConfig Secure(std::string_view key) {
            PackageConfig cfg;
            cfg.encryption = EncryptionMethod::XOR;
            cfg.encryption_key = key;
            cfg.obfuscate_filenames = true;
            cfg.verify_checksums = true;
            return cfg;
        }

        static PackageConfig FastLoad() {
            PackageConfig cfg;
            cfg.compression = CompressionLevel::Fast;
            cfg.verify_checksums = false;
            cfg.lazy_load = false;
            return cfg;
        }

        [[nodiscard]] bool IsValid() const {
            if (encryption != EncryptionMethod::None && encryption_key.empty()) {
                return false;
            }
            return true;
        }
    };

    struct FileInfo {
        std::string name;
        std::string stored_name;
        uint32_t uncompressed_size;
        uint32_t compressed_size;
        uint32_t crc32;
        bool is_encrypted;
        bool is_loaded;

        [[nodiscard]] float GetCompressionRatio() const {
            if (uncompressed_size == 0) return 0.0f;
            return 1.0f - (static_cast<float>(compressed_size) / uncompressed_size);
        }
    };

    using ProgressCallback = std::function<void(size_t current, size_t total, std::string_view filename)>;

    class Package {
    public:
        explicit Package(const PackageConfig& config = PackageConfig::Default());
        ~Package();

        Package(const Package&) = delete;
        Package& operator=(const Package&) = delete;
        Package(Package&&) noexcept;
        Package& operator=(Package&&) noexcept;

        [[nodiscard]] PackageResult Add(std::string_view name, std::span<const uint8_t> data);
        [[nodiscard]] PackageResult Add(std::string_view name, const ByteArray& data);
        [[nodiscard]] PackageResult AddFromFile(std::string_view name, std::string_view filepath);

        [[nodiscard]] PackageResult AddDirectory(std::string_view directory, bool recursive = true,
            ProgressCallback callback = nullptr);
        [[nodiscard]] PackageResult AddMultiple(const std::vector<std::pair<std::string, ByteArray>>& files,
            ProgressCallback callback = nullptr);

        [[nodiscard]] std::optional<ByteArray> Get(std::string_view name);
        [[nodiscard]] PackageResult Extract(std::string_view name, std::string_view output_path);
        [[nodiscard]] PackageResult ExtractAll(std::string_view output_directory,
            ProgressCallback callback = nullptr);

        [[nodiscard]] bool Remove(std::string_view name);
        [[nodiscard]] bool Has(std::string_view name) const;
        [[nodiscard]] std::optional<FileInfo> GetFileInfo(std::string_view name) const;

        [[nodiscard]] PackageResult Save(std::string_view filepath, ProgressCallback callback = nullptr);
        [[nodiscard]] PackageResult Load(std::string_view filepath);
        void Clear() noexcept;

        [[nodiscard]] std::vector<std::string> List() const;
        [[nodiscard]] std::vector<FileInfo> ListDetailed() const;
        [[nodiscard]] size_t GetFileCount() const noexcept;
        [[nodiscard]] size_t GetTotalSize() const noexcept;
        [[nodiscard]] size_t GetCompressedSize() const noexcept;
        [[nodiscard]] float GetCompressionRatio() const noexcept;

        void ClearCache() noexcept;
        [[nodiscard]] size_t GetCacheSize() const noexcept;

        void PrintStatistics() const;
        [[nodiscard]] const PackageConfig& GetConfig() const noexcept;
        [[nodiscard]] PackageError GetLastError() const noexcept;

        [[nodiscard]] std::optional<ByteArray> operator[](std::string_view name);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };

    namespace pak_utils {
        [[nodiscard]] uint32_t CalculateCRC32(std::span<const uint8_t> data);
        [[nodiscard]] uint32_t CalculateCRC32(const uint8_t* data, size_t size);
        [[nodiscard]] std::string ObfuscateName(std::string_view name);
        [[nodiscard]] bool ValidatePackageFile(std::string_view filepath);
        [[nodiscard]] std::string FormatSize(size_t bytes);
        [[nodiscard]] std::string GetErrorMessage(PackageError error);

        [[nodiscard]] bool SecureCompare(uint32_t a, uint32_t b);
    }
} 