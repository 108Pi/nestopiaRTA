#ifndef NST_RUNTIMER_H
#define NST_RUNTIMER_H

#include "NstCpu.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>


#ifdef NST_PRAGMA_ONCE
#pragma once
#endif

namespace Nes
{
    namespace Core
    {

        struct TimerCondition
        {
            int address;
            char comparison;
            int value;
        };

        struct Token
        {
            int type;
            int value;
        };

        class RunTimer
        {
        public:
            RunTimer(Cpu *cpu);
            const double GetTime();
            const bool IsValid();
            void UpdateTimer();
            void Reset();

        private:
            int frameCount;
            int timerState; //0=stopped 1=running 2=vpause
            int startFrame;
            int vpauseTime;
            bool isValid;
            Cpu *_cpu;
            std::vector<std::vector<TimerCondition>> startConds;
            std::vector<std::vector<TimerCondition>> stopConds;
            std::vector<std::vector<TimerCondition>> vpauseConds;
            std::vector<std::vector<TimerCondition>> resetConds;
            std::vector<std::vector<TimerCondition>> endConds;
            bool Init();
            bool CheckConditions(std::vector<std::vector<TimerCondition>> & conds);
        };

    }
}

#endif
