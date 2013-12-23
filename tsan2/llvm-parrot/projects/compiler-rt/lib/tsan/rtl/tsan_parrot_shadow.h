#ifndef TSAN_PARROT_SHADOW_H
#define TSAN_PARROT_SHADOW_H
#include <cassert>
extern u32 *parrotTurnNum;
namespace __tsan{
#if 0
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
#endif
/*
 * The content in a ShadowValue
 * 32 bit of the read_epoch_start
 * 8 bit of the read_epoch_range
 * 8 bit of the write_epoch_start, offset to the read_epoch_start
 * 8 bit of the write_epoch_range
 * 3 bit, address offset to point to the real shadowValue data
 * 5 bit, reserved
 */
class ShadowValue {
	public:
		ShadowValue(u64 u){
			*(u64 *)this=u;
		}
		ShadowValue(){
			Clear();
		}
		void Clear(){
			*(u64 *)this=0ULL;
		}
		ShadowValue* get_real_pointer(){
			return this-offset;
		}
		u32 get_read_epoch_start(){
			return read_epoch_start;
		}
		u32 get_read_epoch_range(){
			return read_epoch_range;
		}
		u32 get_read_epoch_end(){
			return read_epoch_start+read_epoch_range;
		}
		u32 get_write_epoch_start(){
			return read_epoch_start+write_epoch_start_offset;
		}
		u32 get_write_epoch_end(){
			return read_epoch_start+write_epoch_start_offset+write_epoch_range;
		}
		u32 get_write_epoch_range(){
			return write_epoch_range;
		}
		u32 get_latest_epoch_start(){
			return read_epoch_start+write_epoch_start_offset*(write_epoch_start_offset>0);
		}
		u32 get_latest_epoch_next(){
			return write_epoch_start_offset>0?(read_epoch_start+write_epoch_start_offset+write_epoch_range):(read_epoch_start+read_epoch_range);
		}
		bool get_latest_IsWrite(){
			return write_epoch_start_offset>0;
		}
		bool same_read_epoch(const u32 &cur_epoch_start){
			return get_read_epoch_start() == cur_epoch_start;
		}
		bool same_write_epoch(const u32 &cur_epoch_start){
			return get_write_epoch_start() == cur_epoch_start;
		}
		//intersection for detection race
		bool intersect_read_epoch(const u32 &cur_epoch_start, const u32 &cur_epoch_end){
			return cur_epoch_start < get_read_epoch_end();
		}
		bool intersect_write_epoch(const u32 &cur_epoch_start,const u32 &cur_epoch_end){
			return cur_epoch_start < get_write_epoch_end();
		}

		//update
		void update_read_epoch(const u32 &cur_epoch_start,const u8 &cur_epoch_range){
			// BE VERY CAREFUL: the args are start, range instead of start, end
			if (cur_epoch_start <= read_epoch_start) return;
			s32 write_epoch_start_offset32=write_epoch_start_offset-cur_epoch_start+read_epoch_start;
			if(UNLIKELY(write_epoch_start_offset32<-128)){ write_epoch_start_offset = -128;}
			else{write_epoch_start_offset = write_epoch_start_offset32;}
			read_epoch_start = cur_epoch_start;
			read_epoch_range = cur_epoch_range;
			return;
		}
		void update_write_epoch(const u32 &cur_epoch_start,const u8 &cur_epoch_range){
			if (cur_epoch_start <= read_epoch_start + write_epoch_start_offset) return;
			s32 write_epoch_start_offset32=cur_epoch_start-read_epoch_start;
			if(UNLIKELY(write_epoch_start_offset32<-128)) write_epoch_start_offset32=-128;
			else if(UNLIKELY(write_epoch_start_offset32>127)) write_epoch_start_offset = 127;
			else
			{
				write_epoch_start_offset = (s8)write_epoch_start_offset32;
			}
			write_epoch_range = cur_epoch_range;
			return;
		}
		//determine data race
		// ret:[need_update|race]
		int race(const u32 &cur_epoch_start,const u32 &cur_epoch_end, const u8 &cur_epoch_range, bool is_write){
			int ret;
			if(is_write){
					if(same_write_epoch(cur_epoch_start)) return 0;
					if(same_read_epoch(cur_epoch_start)) return 2;
					ret=intersect_read_epoch(cur_epoch_start,cur_epoch_end)\
							||intersect_write_epoch(cur_epoch_start,cur_epoch_end);
					if(UNLIKELY(cur_epoch_start>get_write_epoch_start())){
						update_write_epoch(cur_epoch_start,cur_epoch_range);
						ret+=2;
					}
					return ret;
			}
			else{
					if(same_read_epoch(cur_epoch_start)) return 0;
					if(same_write_epoch(cur_epoch_start)) return 2;
					ret=intersect_write_epoch(cur_epoch_start,cur_epoch_end);
					if(UNLIKELY(cur_epoch_start>get_read_epoch_start())){
						update_read_epoch(cur_epoch_start,cur_epoch_range);
						ret+=2;
					}
					return ret;	
			}
			return 0;
		}
		//comparison
		bool operator == (const ShadowValue &sval) const{
			return *(u64 *)this == *(u64 *)&sval;
		}
		void set_offset(u8 ost){
			offset=ost;
		}
		int get_unused(){return unused;}
	private:
		// The offset to the real shadow value we are looking into
		u8 offset    :3;
		// Reserved
		u8 unused    :5;
		
		s8 write_epoch_start_offset;
		u8 read_epoch_range;
		u8 write_epoch_range;
		u32 read_epoch_start;
};
const ShadowValue SV_Zero=ShadowValue(0);
const ShadowValue SV_Rodata=ShadowValue(u64(-1));

}
#endif
