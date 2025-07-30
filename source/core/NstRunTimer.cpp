#include "NstRunTimer.hpp"

namespace Nes
{
	namespace Core
	{
#ifdef NST_MSVC_OPTIMIZE
#pragma optimize("s", on)
#endif
		RunTimer::RunTimer(Cpu* cpu)
		{
			_cpu = cpu;
			timerState = 0;
			vpauseTime = 0;
			isValid = Init();
		}

		bool RunTimer::Init()
		{
			std::ifstream file("timer.txt");
			if (!file.is_open())
			{
				return false;
			}
			char ch;
			std::vector<Token> tokens;
			while (file.get(ch)) {
				if (isspace(ch)) {
					continue;
				}
				if (ch == '{') {
					Token t = { 1, '{' };
					tokens.push_back(t);
				}
				else if (ch == '}') {
					Token t = { 2, '}' };
					tokens.push_back(t);
				}
				else if (ch == '>') {
					Token t = { 3, '>' };
					tokens.push_back(t);
				}
				else if (ch == '<') {
					Token t = { 3, '<' };
					tokens.push_back(t);
				}
				else if (ch == '=') {
					Token t = { 3, '=' };
					tokens.push_back(t);
				}
				else if (ch == '!') {
					if (!file.get(ch)) {
						return false;
					}
					if (ch != '=') {
						return false;
					}
					Token t = { 3, '!' };
					tokens.push_back(t);
				}
				else if (isalnum(ch)) {
					std::string word = "";
					while (std::isalnum(ch)) {
						word += ch;
						if (!file.get(ch)) {
							break;
						}
					}
					file.putback(ch);
					if (word == "start") {
						Token t = { 0, 't' };
						tokens.push_back(t);
					}
					else if (word == "stop") {
						Token t = { 0,'p' }; 
						tokens.push_back(t);
					}
					else if (word == "reset") {
						Token t = { 0,'r' };
						tokens.push_back(t);
					}
					else if (word == "vpause") {
						Token t = { 0,'v' };
						tokens.push_back(t);
					}
					else if (word == "end") {
						Token t = { 0, 'e' };
						tokens.push_back(t);
					}
					else if (word == "delay") {
						Token t = { 5,'d' }; 
						tokens.push_back(t);
					}
					else {
						int base = 10;
						if (word.size() > 2 && word[0] == '0' && (word[1] == 'x' || word[1] == 'X')) {
							base = 16;
						}
						try {
							size_t idx;
							int value = std::stoi(word, &idx, base);
							if (idx != word.size()) {
								return false;
							}
							Token t = { 4,value };
							tokens.push_back(t);
						}
						catch (const std::invalid_argument& e) {
							return false;
						}
						catch (const std::out_of_range& e) {
							return false;
						}
					}
				}
				else {
					return false;
				}
			}
			for (int i = 0; i < tokens.size(); ++i) {
				if (tokens[i].type == 5) {
					++i;
					if (tokens[i].type != 4) {
						return false;
					}
					startFrame = tokens[i].value * -1;
					continue;
				}
				if (tokens[i].type != 0) {
					return false;
				}
				char condType = tokens[i].value;
				++i;
				if (tokens[i].type != 1) {
					return false;
				}
				++i;
				std::vector<TimerCondition> conditions;
				while (tokens[i].type != 2) {
					if (tokens[i].type != 4) {
						return false;
					}
					int addr = tokens[i].value;
					++i;
					if (tokens[i].type != 3) {
						return false;
					}
					char op = tokens[i].value;
					++i;
					if (tokens[i].type != 4) {
						return false;
					}
					int value = tokens[i].value;
					++i;
					TimerCondition tc = { addr,op,value };
					conditions.push_back(tc);
				}
				if (condType == 't') {
					startConds.push_back(conditions);
				}
				else if (condType == 'p') {
					stopConds.push_back(conditions);
				}
				else if (condType == 'r') {
					resetConds.push_back(conditions);
				}
				else if (condType == 'v') {
					vpauseConds.push_back(conditions);
				} 
				else if (condType == 'e') {
					endConds.push_back(conditions);
				}
				else {
					return false;
				}
			}
			return true;
		}

		void RunTimer::UpdateTimer()
		{
			if (CheckConditions(resetConds)) {
				Reset();
				return;
			}
			if (timerState == 1) //running
			{
				++frameCount;
				if (CheckConditions(endConds)) {
					timerState = -1;
				}
				else if (CheckConditions(stopConds)) {
					timerState = 0;
				}
				else if (CheckConditions(vpauseConds)) {
					timerState = 2;
					vpauseTime = 60;
				}
			}
			else if (timerState == 0) //stopped
			{
				if (CheckConditions(startConds)) {
					timerState = 1;
				}
			}
			else if (timerState == 2) //vpause
			{
				if (CheckConditions(stopConds)) {
					timerState = 0;
					frameCount += 61 - vpauseTime;
				}
				else if (CheckConditions(endConds)) {
					timerState = -1;
					frameCount += 61 - vpauseTime;
				}
				else if (vpauseTime == 0) {
					timerState = 1;
					frameCount += 61;
				}
				else {
					vpauseTime--;
				}
			}
		}

		const double RunTimer::GetTime()
		{
			return frameCount;
		}

		const bool RunTimer::IsValid()
		{
			return isValid;
		}

		void RunTimer::Reset()
		{
			frameCount = startFrame;
			timerState = 0;
		}

		bool RunTimer::CheckConditions(std::vector<std::vector<TimerCondition>>& conds)
		{
			for (size_t i = 0; i < conds.size(); ++i) {
				const std::vector<TimerCondition>& test = conds[i];
				bool passes = true;
				for (size_t j = 0; j < test.size(); ++j) {
					const TimerCondition& cond = test[j];
					byte ramValue = _cpu->GetRam()[cond.address];
					if (cond.comparison == '=') {
						if (ramValue != cond.value) {
							passes = false;
							break;
						}
					}
					else if (cond.comparison == '<') {
						if (ramValue >= cond.value) {
							passes = false;
							break;
						}
					}
					else if (cond.comparison == '>') {
						if (ramValue <= cond.value) {
							passes = false;
							break;
						}
					}
					else if (cond.comparison == '!') {
						if (ramValue == cond.value) {
							passes = false;
							break;
						}
					}
					else {
						isValid = false;
						return false;
					}
				}
				if (passes) {
					return true;
				}
			}
			return false;
		}
	}
}