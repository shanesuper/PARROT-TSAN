#ifndef TSAN_PARROT_SHADOW_H
#define TSAN_PARROT_SHADOW_H
#include <cassert>
extern u32 *parrotTurnNum;
namespace __tsan{
class threadLocal{
	public:
		u32 *turnNum;
		u8 m_tid;
		u8 ignore        :1;
		u8 history_size  :3;
		u8 unused        :4;
	public:
		threadLocal(u64 tid):m_tid(tid),ignore(0),history_size(0),unused(0){
			if(parrotTurnNum!=0){
				set_turnNum(tid);
			}
		};
		void set_turnNum(u64 tid){
			turnNum=parrotTurnNum+2*tid;
			Printf("Setting thread %d's turnNum ptr at %0x\n",tid,turnNum);
		}
		u64 tid() const{ return m_tid;}
		u32 epoch() const {
			return turnNum[0];
		}
		u32 epoch_next() const{
			return turnNum[1];
		}
		u8 epoch_range() const{
			return turnNum[1]-turnNum[0];
		}
		void SetIgnoreBit(){
			ignore=1;
		}
		void ClearIgnoreBit(){
			ignore=0;
		}
		bool GetIgnoreBit() const {
			return ignore;
		}
		void SetHistorySize(int hs){
    		CHECK_GE(hs, 0);
   			CHECK_LE(hs, 7);
			history_size=hs;
		}
		int GetHistorySize() const{
			return history_size;
		}
		u64 GetTracePos() const{
		    const int hs = GetHistorySize();
		    // When hs == 0, the trace consists of 2 parts.
		    const u64 mask = (1ull << (__tsan::kTracePartSizeBits + hs + 1)) - 1;
		    return epoch() & mask;
		}
		int get_unused(){return unused;}
};
}
#endif
