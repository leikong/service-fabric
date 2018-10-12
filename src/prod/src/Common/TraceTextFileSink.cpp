// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace std;

namespace Common
{
    StringLiteral const Extension(".trace");

    int const TraceTextFileSink::DefaultMaxFilesToKeep = 3;
    int64 const TraceTextFileSink::DefaultSizeCheckIntervalInMinutes = 5;

    TraceTextFileSink* TraceTextFileSink::Singleton = new TraceTextFileSink();

    TraceTextFileSink::CleanupHelper::CleanupHelper()
    {
    }

    TraceTextFileSink::CleanupHelper::~CleanupHelper()
    {
        TraceTextFileSink::Singleton->Disable();
    }

    TraceTextFileSink::TraceTextFileSink()
        : enabled_(false),
        processNameOption_("e"),
        processIdOption_("p"),
        instanceIdOption_("i"),
        moduleOption_("m"),
        segmentHoursOption_("hr"),
        segmentMinutesOption_("min"),
        fileCountOption_("f"),
        segmentSizeOption_("sizemb"),
        file_(),
        lock_(),
        files_(),
        path_(),
        option_(),
        segmentTime_(0),
        sizeCheckTime_(0),
        maxSize_(0),
        sizeCheckIntervalInMinutes_(DefaultSizeCheckIntervalInMinutes)
    {
        static CleanupHelper cleanupHelper; // cleanupHelper destructor calls Disable().
    }

    void TraceTextFileSink::Disable()
    {
        AcquireWriteLock lock(lock_);
        this->enabled_ = false;
        this->CloseFile();
    }

    DateTime TraceTextFileSink::CalculateCheckTime(DateTime now, int64 minutes)
    {
        int64 ticks = (now + TimeSpan::FromMinutes(static_cast<double>(minutes))).Ticks;
        ticks -= (ticks % (TimeSpan::TicksPerMinute * minutes));

        return DateTime(ticks);
    }

    void TraceTextFileSink::PrivateSetPath(std::string const & path)
    {
        AcquireWriteLock lock(lock_);
        if (path_ != path)
        {
            path_ = path;
            CloseFile();
            
            enabled_ = (!path_.empty());
        }
    }

    void TraceTextFileSink::PrivateSetOption(std::string const & option)
    {
        AcquireWriteLock lock(lock_);
        if (option_ != option)
        {
            option_ = option;
            CloseFile();
        }
    }

    void TraceTextFileSink::CloseFile()
    {
        if (file_.IsValid())
        {
            file_.Close2();
        }
    }

    void TraceTextFileSink::OpenFile()
    {
        if (!enabled_)
        {
            return;
        }

        string fileName;
        if (StringUtility::EndsWithCaseInsensitive(path_, string(Extension.begin(), Extension.end())))
        {
            fileName = path_.substr(0, path_.size() - Extension.size());
        }
        else
        {
            fileName = path_;
        }

        StringCollection options;
        StringUtility::Split<string>(option_, options, ",");

        bool foundSegmentOption = false;
        DateTime now = DateTime::Now();
        segmentTime_ = DateTime::MaxValue;
        sizeCheckTime_ = DateTime::MaxValue;
        int maxFilesToKeep = DefaultMaxFilesToKeep;
        for (auto iter = options.begin(); iter != options.end(); iter++)
        {
            string const& option = *iter;
            if (option == processNameOption_)
            {
                string processPath;
                Environment::GetExecutableFileName(processPath);
                string processName = Path::GetFileName(processPath);
                StringUtility::Replace<string>(processName, ".", "_");

                fileName += formatString.L("-{0}", processName);
            }

            if (option == processIdOption_)
            {
                DWORD pid = GetCurrentProcessId();
                fileName += formatString.L("-{0}", pid);
            }

            if (option == instanceIdOption_)
            {
                int64 ticks = DateTime::Now().Ticks;
                fileName += formatString.L("-{0}", ticks);
            }

            if (option == moduleOption_)
            {
                std::string moduleFullName;
                Environment::GetCurrentModuleFileName(/*out*/moduleFullName);
                std::string moduleName = Path::GetFileNameWithoutExtension(moduleFullName);
                fileName.append("-");
                fileName.append(moduleName);
            }

            if (StringUtility::StartsWith(option, fileCountOption_))
            {
                if (option.size() > (fileCountOption_.size() + 1) && option[fileCountOption_.size()] == ':')
                {
                    string fileCountString = option.substr(fileCountOption_.size() + 1, option.size() - (fileCountOption_.size() + 1));
                    maxFilesToKeep = Int32_Parse(fileCountString);
                }
            }

            if (StringUtility::StartsWith(option, segmentHoursOption_))
            {
                ASSERT_IF(foundSegmentOption, "Trace file is already segemented on time or size.");
                if (option.size() > (segmentHoursOption_.size() + 1) && option[segmentHoursOption_.size()] == ':')
                {
                    string segmentTimeStringInHours = option.substr(segmentHoursOption_.size() + 1, option.size() - (segmentHoursOption_.size() + 1));
                    int segmentTimeInHours = Int32_Parse(segmentTimeStringInHours);
                    foundSegmentOption = true;
                    segmentTime_ = CalculateCheckTime(now, segmentTimeInHours * 60);
                }
            }

            if (StringUtility::StartsWith(option, segmentMinutesOption_))
            {
                ASSERT_IF(foundSegmentOption, "Trace file is already segemented on time or size.");
                if (option.size() > (segmentMinutesOption_.size() + 1) && option[segmentMinutesOption_.size()] == ':')
                {
                    string segmentTimeStringInMins = option.substr(segmentMinutesOption_.size() + 1, option.size() - (segmentMinutesOption_.size() + 1));
                    int segmentTimeInMins = Int32_Parse(segmentTimeStringInMins);
                    foundSegmentOption = true;
                    segmentTime_ = CalculateCheckTime(now, segmentTimeInMins);
                    fileName += TimeToFileNameSuffix(now);
                }
            }

            if (StringUtility::StartsWith(option, segmentSizeOption_))
            {
                ASSERT_IF(foundSegmentOption, "Trace file is already segemented on time or size.");
                if (option.size() > (segmentSizeOption_.size() + 1) && option[segmentSizeOption_.size()] == ':')
                {
                    Common::StringCollection sizeOptions;
                    string sizeOptionsString = option.substr(segmentSizeOption_.size() + 1, option.size() - (segmentSizeOption_.size() + 1));
                    StringUtility::Split<string>(sizeOptionsString, sizeOptions, ":");
                    
                    if (sizeOptions.size() > 0)
                    {
                        maxSize_ = Int64_Parse(sizeOptions[0]);
                        maxSize_ = maxSize_ * 1024 * 1024;
                        foundSegmentOption = true;
                    }

                    if (sizeOptions.size() > 1)
                    {
                        sizeCheckIntervalInMinutes_ = Int64_Parse(sizeOptions[0]);
                    }

                    sizeCheckTime_ = CalculateCheckTime(now, sizeCheckIntervalInMinutes_);
                    fileName += TimeToFileNameSuffix(now);
                }
            }
        } // foreach options

        fileName += formatString.L("{0}", Extension);

        auto error = file_.TryOpen(fileName, FileMode::Create, FileAccess::Write, FileShare::Read);
        if (!error.IsSuccess())
        {
            TraceConsoleSink::Write(LogLevel::Error, formatString.L("Unable to open text trace file '{0}': {1}", fileName, error));
            throw runtime_error(formatString.L("Unable to open text trace file '{0}': {1}", fileName, error));
        }

        files_.push_back(fileName);
        if (files_.size() > static_cast<size_t>(maxFilesToKeep))
        {
            File::Delete(files_[0], NOTHROW());
            files_.erase(files_.begin());
        }
    }

    string TraceTextFileSink::TimeToFileNameSuffix(DateTime const & now)
    {
        string result = formatString.L("-{0:local}", now);
        return result.substr(0, 11) + "-" + result.substr(12, 2) + "-" + result.substr(15, 2);  
    }

    void TraceTextFileSink::PrivateWrite(StringLiteral taskName, StringLiteral eventName, LogLevel::Enum level, std::string const & id, std::string const & data)
    {
        DateTime now = DateTime::Now();

        std::string text;
        text.reserve(data.size() + 128);
        StringWriterA w(text);

        w.Write(now);
        w.Write(',');
        w.Write(level);
        w.Write(',');
        w.Write(GetCurrentThreadId());
        w.Write(',');
        w.Write(taskName);

        if (eventName.size() > 0)
        {
            w.Write('.');
            w.Write(eventName);
        }

        if (id.size() > 0)
        {
            w.Write('@');
            w.Write(id);
        }

        w.Write(',');
        w.WriteUnicodeBuffer(data.c_str(), data.size());

        size_t newLineIndex = 0;
        while ((newLineIndex = text.find('\n', newLineIndex)) != std::string::npos)
        {
            text[newLineIndex] = '\t';
        }

        text.append("\r\n");

        AcquireWriteLock lock(lock_);
        {
            if (now > segmentTime_)
            {
                CloseFile();
            }

            if (now > sizeCheckTime_)
            {
                int64 size;
                if (file_.TryGetSize(size) && (size >= maxSize_))
                {
                    CloseFile();
                }
                else
                {
                    sizeCheckTime_ = CalculateCheckTime(now, sizeCheckIntervalInMinutes_);
                }
            }

            if (!file_.IsValid())
            {
                OpenFile();
            }

            if (file_.IsValid())
            {
                file_.TryWrite(text.c_str(), static_cast<int>(text.size()));
            }
        }
    }
}
