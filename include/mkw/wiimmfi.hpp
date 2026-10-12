#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace mkw::wiimmfi {
// Wiimmfi is a Wii WFC replacement, not a 3DS matchmaking service.
// No substitute protocol: the actual NAS ticket/GPCM login is required.
struct Service {const char* host;unsigned port;};
inline constexpr Service Gpcm={"gpcm.gs.wiimmfi.de",29900};
inline constexpr Service Gpsp={"gpsp.gs.wiimmfi.de",29901};
inline constexpr Service Master={"mariokartwii.master.gs.wiimmfi.de",27900};
inline constexpr Service Match={"mariokartwii.ms19.gs.wiimmfi.de",28910};
inline constexpr Service Natneg={"mariokartwii.natneg1.gs.wiimmfi.de",27901};

// GameSpy GPCM records end at \\final\\. TCP can split or join them.
struct GpcmStream {
    char bytes[4096]{};
    unsigned used=0;
    void clear(){used=0;}
    bool feed(const void* input,size_t size){
        if(!input||size>sizeof(bytes)-used){clear();return false;}
        std::memcpy(bytes+used,input,size);used+=unsigned(size);return true;
    }
    bool next(char* output,size_t capacity){
        static constexpr char end[]="\\final\\";
        constexpr unsigned marker=sizeof(end)-1;
        for(unsigned i=0;i+marker<=used;++i){
            if(std::memcmp(bytes+i,end,marker))continue;
            const unsigned n=i+marker;
            if(!output||capacity<=n)return false;
            std::memcpy(output,bytes,n);output[n]=0;
            std::memmove(bytes,bytes+n,used-n);used-=n;
            return true;
        }
        return false;
    }
};

inline bool field(const char* record,const char* name,char* output,size_t capacity){
    if(!record||!name||!output||!capacity)return false;
    const size_t keylen=std::strlen(name);
    for(const char* p=record;*p;){
        if(*p!='\\')return false;
        const char* key=++p;
        const char* kEnd=std::strchr(key,'\\');if(!kEnd)return false;
        const char* value=kEnd+1;
        const char* vEnd=std::strchr(value,'\\');if(!vEnd)return false;
        const size_t n=size_t(vEnd-value);
        if(size_t(kEnd-key)==keylen&&!std::memcmp(key,name,keylen)){
            if(n>=capacity)return false;
            std::memcpy(output,value,n);output[n]=0;return true;
        }
        p=vEnd;
    }
    return false;
}
inline bool challenge(const char* record,char* output,size_t size){
    char state[4];
    return field(record,"lc",state,sizeof(state))&&!std::strcmp(state,"1")&&
        field(record,"challenge",output,size);
}
}
