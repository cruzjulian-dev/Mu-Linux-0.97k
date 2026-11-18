#!/usr/bin/env python3
"""Viewport de monstruos (0x13) contra IDA ReceiveCreateMonsterViewport (0x0042A230).

Compila el NetRecv_13 real con los servicios controlados y comprueba, por
entidad, lo que hace el binario:
- el bit 15 de la key (CreateFlag) llama a AppearMonster y no busca ruta;
- sin CreateFlag busca la ruta de (x, y) a (tx, ty) y, si la hay, Movement = 1;
- PositionX/Y y TargetX/Y quedan en la casilla actual (el destino sólo va a
  la ruta), el facing sale del nibble alto de e[10] y el buff de e[4..5].
También que el handler no pise lo que deja CreateCharacterPointer (Alpha,
MoveSpeed, Visible).

Windows: python tests/MonsterViewportPointer.py --architecture Win32
Windows x64: python tests/MonsterViewportPointer.py --architecture x64
"""
import argparse, re, tempfile
from pathlib import Path
from SceneObjectPointer import build_and_run, function, read_source


def harness():
    header = read_source("src/game/entity/EntityTypes.h")
    records = "\n".join(re.search(r"struct " + n + r"\s*\{.*?\n\};", header, re.S).group()
                        for n in ("OBJECT", "PART_t", "CHARACTER"))
    field = re.search(r"template <class B, class F> inline B FieldAddress\(B, F\* field\)\n\{.*?\n\}", header, re.S).group()
    handler = function(read_source("src/net/Recv/Recv_Viewport.cpp"), "NetRecv_13")
    return r'''
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#ifndef _MSC_VER
#define __cdecl
#endif
using BYTE=std::uint8_t;using WORD=std::uint16_t;using DWORD=std::uint32_t;
using vec3_t=float[3];using vec34_t=float[3][4];
void Require(bool v,const char* what){if(!v){std::fprintf(stderr,"0x13: %s\n",what);std::abort();}}
#pragma pack(push,1)
''' + records + r'''
#pragma pack(pop)
''' + field + r'''
CHARACTER* AsCharacter(void* p){return static_cast<CHARACTER*>(p);}
CHARACTER pool[16];std::uintptr_t CharactersClient=reinterpret_cast<std::uintptr_t>(pool);
void NetLog(const char*,...){}
namespace EntityView{constexpr int MonsterKind=2;}
struct HealthBars{void Set(WORD,BYTE,BYTE){}} gHealthBar;
int created,appeared,routed,buffed;bool routeOk;
int lastRoute[4];BYTE* lastPath;WORD lastBuff;
char* __cdecl CreateMonster(unsigned int type,int x,int y,int key,int){
    CHARACTER* c=pool+created++;std::memset(c,0,sizeof(*c));
    c->Object.Type=short(type);c->Key=WORD(key);c->Object.Alpha=1.0f;c->MoveSpeed=10;c->PositionX=x;c->PositionY=y;
    return reinterpret_cast<char*>(c);}
void __cdecl AppearMonster(std::intptr_t c){Require(c==reinterpret_cast<std::intptr_t>(pool+created-1),"AppearMonster con otro slot");++appeared;}
unsigned int __cdecl Path_FindRoute(int sx,int sy,int tx,int ty,unsigned char* path,float r){
    Require(r==0.0f,"radio de ruta");lastRoute[0]=sx;lastRoute[1]=sy;lastRoute[2]=tx;lastRoute[3]=ty;lastPath=path;++routed;return routeOk;}
void ApplyPersistentSkillEffect97k(BYTE*,WORD s,int enable){Require(enable==1,"buff");lastBuff=s;++buffed;}
''' + handler + r'''
int main(){
    unsigned cases=0;
    for(int flags=0;flags<8;++flags){
        const bool create=flags&1;routeOk=(flags&2)!=0;const bool c2=(flags&4)!=0;
        created=appeared=routed=buffed=0;
        BYTE msg[4+12*2]={0xC1,sizeof(msg),0x13,2};
        for(int i=0;i<2;++i){
            BYTE* e=msg+4+12*i;const bool cf=i?c2:create;
            e[0]=BYTE((cf?0x80:0)|0x12);e[1]=BYTE(0x34+i);e[2]=BYTE(30+i);e[4]=0x01;e[5]=BYTE(0x80|i);
            e[6]=BYTE(100+i);e[7]=BYTE(110+i);e[8]=BYTE(120+i);e[9]=BYTE(130+i);e[10]=BYTE(((3+i)<<4)|2);e[11]=77;
        }
        int expectRouted=0,expectAppeared=0;
        for(int i=0;i<2;++i){const bool cf=i?c2:create;if(cf)++expectAppeared;else ++expectRouted;}
        NetRecv_13(msg,sizeof(msg),0xC1,0,false);
        Require(created==2&&buffed==2,"dos entidades");
        Require(appeared==expectAppeared&&routed==expectRouted,"CreateFlag elige AppearMonster o ruta");
        for(int i=0;i<2;++i){
            const CHARACTER& c=pool[i];const bool cf=i?c2:create;
            Require(c.Key==WORD(0x1234+i),"key sin el bit 15");
            Require(c.PositionX==100+i&&c.PositionY==110+i,"posicion");
            Require(c.TargetX==100+i&&c.TargetY==110+i,"TargetX/Y = casilla actual");
            Require(c.Object.Angle[2]==float((3+i)-1)*45.0f,"facing");
            Require(c.Movement==((!cf&&routeOk)?1:0),"Movement sólo con ruta");
            Require(c.Object.Alpha==1.0f&&c.MoveSpeed==10&&c.Object.Visible==0,"no pisa CreateCharacterPointer");
            ++cases;
        }
        if(!c2){Require(lastRoute[0]==101&&lastRoute[1]==111&&lastRoute[2]==121&&lastRoute[3]==131,"ruta de (x,y) a (tx,ty)");
                Require(lastPath==reinterpret_cast<BYTE*>(&pool[1].PathCurrent),"buffer de ruta");}
        Require(lastBuff==WORD(0x8001|(1<<8)),"ViewSkillState");
    }
    std::printf("NetRecv_13: %u entidades con y sin CreateFlag, con y sin ruta OK\n",cases);
}
'''


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--architecture", choices=("Win32", "x64"), default="x64")
    p.add_argument("--generator")
    a = p.parse_args()
    with tempfile.TemporaryDirectory(prefix="mu-monster-viewport-") as t:
        print(build_and_run(Path(t) / "current", harness(), a), end="")


if __name__ == "__main__":
    main()
