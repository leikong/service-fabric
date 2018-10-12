// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

namespace Common
{
    namespace PerformanceCounterType
    {
        void WriteToTextWriter(Common::TextWriter & w, Enum const & e)
        {
            switch (e)
            {
            case QueueLength : w << "QueueLength"; return;
            case LargeQueueLength : w << "LargeQueueLength"; return;
            case QueueLength100Ns : w << "QueueLength100Ns"; return;
            case RawData32 : w << "RawData32"; return;
            case RawData64 : w << "RawData64"; return;  
            case RawDataHex32 : w << "RawDataHex32"; return;  
            case RawDataHex64 : w << "RawDataHex64"; return;  
            case RateOfCountPerSecond32 : w << "RateOfCountPerSecond32"; return;  
            case RateOfCountPerSecond64 : w << "RateOfCountPerSecond64"; return;  
            case RawFraction32 : w << "RawFraction32"; return;  
            case RawFraction64 : w << "RawFraction64"; return;
            case RawBase32 : w << "RawBase32"; return;  
            case RawBase64 : w << "RawBase64"; return;  
            case SampleFraction : w << "SampleFraction"; return;  
            case SampleCounter : w << "SampleCounter"; return;  
            case SampleBase : w << "SampleBase"; return;
            case AverageTimer32 : w << "AverageTimer32"; return;  
            case AverageBase : w << "AverageBase"; return;  
            case AverageCount64 : w << "AverageCount64"; return;  
            case PercentageActive : w << "PercentageActive"; return;  
            case PercentageNotActive : w << "PercentageNotActive"; return;  
            case PercentageActive100Ns : w << "PercentageActive100Ns"; return;
            case PercentageNotActive100Ns : w << "PercentageNotActive100Ns"; return;  
            case ElapsedTime : w << "ElapsedTime"; return;  
            case MultiTimerPercentageActive : w << "MultiTimerPercentageActive"; return;  
            case MultiTimerPercentageNotActive : w << "MultiTimerPercentageNotActive"; return;  
            case MultiTimerPercentageActive100Ns : w << "MultiTimerPercentageActive100Ns"; return;  
            case MultiTimerPercentageNotActive100Ns : w << "MultiTimerPercentageNotActive100Ns"; return;  
            case MultiTimerBase : w << "MultiTimerBase"; return;
            case Delta32 : w << "Delta32"; return;  
            case Delta64 : w << "Delta64"; return;  
            case ObjectSpecificTimer : w << "ObjectSpecificTimer"; return; 
            case PrecisionSystemTimer : w << "PrecisionSystemTimer"; return;  
            case PrecisionTimer100Ns : w << "PrecisionTimer100Ns"; return;  
            case PrecisionObjectSpecificTimer : w << "PrecisionObjectSpecificTimer"; return;
            }

            w << "PerformanceCounterType(" << static_cast<int>(e) << ')';
        }
    }
}
