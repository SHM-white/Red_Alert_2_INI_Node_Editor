#pragma once

/***********************************************************************************************
 ***                                 ??????????����Դ����                                    ***
 ***********************************************************************************************
 *                                                                                             *
 *                       �ļ��� : global_timer.h                                               *
 *                                                                                             *
 *                     ��д���� : Old Sovok                                                    *
 *                                                                                             *
 *                     �½����� : 2022/3/23                                                    *
 *                                                                                             *
 *                     ����༭ : 2022/3/30 Old Sovok                                          *
 *                                                                                             *
 *                     ��Ԫ���� : 2022/3/30 Old Sovok                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * ���ݣ�                                                             
 *   namespace Timer -- WOTT�ļ�ʱ�����������ռ�       
 *     inttime_t (�� types.h) -- ʱ�������
 *     timer_t -- ʱ�������                            
 *     dura_t -- ʱ�������                                                 
 *     ClocksPerSec -- ���һ���ʱ��γ���                               
 *     ClocksPerMSec -- ���һ�����ʱ��γ���                               
 *     SecondToTick -- ����������ʱ���
 *     MilliToTick -- �ú���������ʱ���
 *     ToDuration -- ��ʱ�������ʱ���
 *     ToTimeInt -- ��ʱ�������ʱ���
 *     ClockNow -- ��ȡ��ǰʱ���
 *     SetSpeedRatio -- �����ٶȱ��ʣ�> 1 = ���� ��< 1 = ���٣�
 *     mode_t -- ��ʱ�����ࣨ����ʱ������ʱ��
 *     TimerClass -- ��ʱ����
 *       Set -- �������в���
 *       Reset -- ��ʱ����λ
 *       Pause -- ��ͣ��ʱ
 *       Resume -- ������ʱ
 *       GetClock -- ��ȡ��ʱ����ʱ��Σ�����ʱ = ��ʱʱ�� �� ����ʱ = ʣ��ʱ�䣩
 *       GetSecond -- ��ȡ��ʱ������������ʱ = ��ʱʱ�� �� ����ʱ = ʣ��ʱ�䣩
 *       GetMilli -- ��ȡ��ʱ��������������ʱ = ��ʱʱ�� �� ����ʱ = ʣ��ʱ�䣩
 *       TimeUp -- �Ƿ�ʱ�䵽��
 *       BeginTime -- ��ȡ��ʼʱ���
 *       CountLength -- ��ȡ������ʱ��ʱ��γ���
 *       PauseMode -- �Ƿ�������ͣ
 *     GlobalTimer -- ȫ�ּ�ʱ��
 *       RealTimer -- ��ȡ����ʵ������ʱ���ʱ��
 *       MapTimer -- ��ȡ���ֵ�ͼ������ʱ��
 *       Init -- ��ʼ��2����ʱ��
 *       ResetGlobalTimer -- ��λ��ͼ������ʱ��
 *       SaveGameTimer -- ���⸳ֵ2����ʱ�����ڱ���
 *       FrameFlow -- ֡����������
 *       SavePlayFrames -- ���⸳ֵ֡���������ڱ���
 *       GetPlayFrames -- ��ȡ֡������
 *       QuitGameReset -- �˳���Ϸʱ�ļ�ʱ����֡����������
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "types.h"
#include<ratio>
#include<chrono>

namespace ETimer
{
	typedef std::chrono::time_point<std::chrono::steady_clock> timer_t;
	typedef std::chrono::steady_clock::duration dura_t;
	const static dura_t __DurationZero = dura_t::zero();
	typedef int64_t inttime_t;
	typedef int64_t counter_t;
	const static inttime_t ClockTicksPerSec = std::chrono::steady_clock::duration::period::den / (std::chrono::steady_clock::duration::period::num);
	const static inttime_t ClockTicksPerMSec = ClockTicksPerSec / 1000;

    constexpr dura_t ClocksPerSec() { return dura_t((inttime_t)((double)ClockTicksPerSec)); }
    constexpr dura_t ClocksPerMSec() { return dura_t((inttime_t)((double)ClockTicksPerMSec)); }
    constexpr inttime_t SecondToTick(const inttime_t _clock) { return _clock * ClockTicksPerSec; }
    constexpr inttime_t MilliToTick(const inttime_t _clock) { return _clock * ClockTicksPerMSec; }
	constexpr dura_t ToDuration(const inttime_t _clock) { return dura_t(_clock); }
	constexpr inttime_t ToTimeInt(const dura_t& _clock) { return (inttime_t)_clock.count(); }
    constexpr dura_t SecondToDuration(const inttime_t _clock) { return ToDuration(SecondToTick(_clock)); }
    constexpr dura_t MilliToDuration(const inttime_t _clock) { return ToDuration(MilliToTick(_clock)); }
    static timer_t ClockNow() { return std::chrono::steady_clock::now(); }

	enum mode_t
	{
		T_COUNT,
		T_COUNTDOWN
	};
	class TimerClass
	{
		timer_t Begin, RBegin;
		timer_t PauseTime;
		dura_t Length;
		mode_t Mode;
		bool InPause;//sizeof TimerClass = 32
		void ReleasePause(timer_t To)
		{
			Begin += To - PauseTime;
			PauseTime = To;
		}
	public:
		//CONSTRUCTOR
		TimerClass(timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNT;
			Length = __DurationZero;
			PauseTime = ClockNow();
			InPause = false;
		}
		TimerClass(dura_t _Length, timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNTDOWN;
			Length = _Length;
			PauseTime = ClockNow();
			InPause = false;
		}
		//ACTION
		void Set(timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNT;
			Length = __DurationZero;
			PauseTime = ClockNow();
			InPause = false;
		}
		void Set(dura_t _Length, timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNTDOWN;
			Length = _Length;
			PauseTime = ClockNow();
			InPause = false;
		}
		void Reset()
		{
			Begin = RBegin = ClockNow();
			PauseTime = ClockNow();
			InPause = false;
		}
		void Pause()
		{
			if (InPause)return;
			PauseTime = ClockNow();
			InPause = true;
		}
		void Resume()
		{
			if (!InPause)return;
			timer_t Now = ClockNow();
			ReleasePause(Now);
			PauseTime = Now;
			InPause = false;
		}
		//QUERY
		dura_t GetClock()
		{
			timer_t Now = ClockNow();
			if (InPause)
			{
				ReleasePause(Now);
			}
			if (Mode == T_COUNT)
			{
				return (Now - Begin);
			}
			else if (Mode == T_COUNTDOWN)
			{
				return (Begin + Length) - Now;
			}
			else return __DurationZero;
		}
		inttime_t GetSecond()
		{
			return (inttime_t)((double)GetClock().count() / (double)ClockTicksPerSec);
		}
		inttime_t GetMilli()
		{
			return (inttime_t)(((double)GetClock().count() - (double)Length.count()) / (double)ClockTicksPerMSec);
		}
		bool TimeUp()
		{
			if (Mode == T_COUNTDOWN)
			{
				return ((inttime_t)((double)GetClock().count() - (double)Length.count()) <= 0);
			}
			else return false;
		}
		timer_t BeginTime() { return RBegin; }
		dura_t CountLength() { return Length; }
		bool PauseMode() { return InPause; }
	};

}