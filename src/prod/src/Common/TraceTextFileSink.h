// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#pragma once

namespace Common
{
    class TraceTextFileSink
    {
        DENY_COPY(TraceTextFileSink);

    public:
        static std::string GetPath()
        {
            return Singleton->path_;
        }

        static void SetPath(std::string const & path)
        {
            Singleton->PrivateSetPath(path);
        }

        static void SetOption(std::string const & option)
        {
            Singleton->PrivateSetOption(option);
        }

        static void Write(StringLiteral taskName, StringLiteral eventName, LogLevel::Enum level, std::string const & id, std::string const & data)
        {
            Singleton->PrivateWrite(taskName, eventName, level, id, data);
        }

        static bool IsEnabled()
        {
            return Singleton->enabled_;
        }

    private:
        // Helper class to release file handle without destructing TraceTextFileSink::Singleton
        class CleanupHelper
        {
        public:
            CleanupHelper();
            ~CleanupHelper();
        };

        static TraceTextFileSink* Singleton;

        static int64 const DefaultSizeCheckIntervalInMinutes;
        static int const DefaultMaxFilesToKeep;

        TraceTextFileSink();

        DateTime CalculateCheckTime(DateTime now, int64 minutes);
        void CloseFile();
        void OpenFile();
        void PrivateSetPath(std::string const & path);
        void PrivateSetOption(std::string const & option);
        void PrivateWrite(StringLiteral taskName, StringLiteral eventName, LogLevel::Enum level, std::string const & id, std::string const & data);
        void Disable();
        static std::string TimeToFileNameSuffix(DateTime const & now);

        File file_;
        RwLock lock_;
        std::vector<std::string> files_;
        std::string path_;
        std::string option_;
        DateTime segmentTime_;
        DateTime sizeCheckTime_;
        int64 maxSize_;
        int64 sizeCheckIntervalInMinutes_;
        bool enabled_;

        std::string const processNameOption_;
        std::string const processIdOption_;
        std::string const instanceIdOption_;
        std::string const moduleOption_;
        std::string const segmentHoursOption_;
        std::string const segmentMinutesOption_;
        std::string const fileCountOption_;
        std::string const segmentSizeOption_;
    };
}
