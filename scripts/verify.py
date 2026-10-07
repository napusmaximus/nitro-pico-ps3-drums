#!/usr/bin/env python3
"""Audit actual ELF symbols, HID bit counts, and UF2 payload against BIN."""
import hashlib,json,pathlib,struct,subprocess,sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
def elf_symbols(path):
    b=path.read_bytes();assert b[:7]==b'\x7fELF\x01\x01\x01','Expected ELF32 little-endian'
    assert struct.unpack_from('<H',b,18)[0]==40,'Expected ARM machine'
    off=struct.unpack_from('<I',b,32)[0];size,count=struct.unpack_from('<HH',b,46)
    sections=[struct.unpack_from('<10I',b,off+i*size) for i in range(count)]
    out={}
    for s in sections:
        if s[1]!=2:continue
        strings=sections[s[6]];names=b[strings[4]:strings[4]+strings[5]]
        for k in range(s[4],s[4]+s[5],s[9]):
            name,value,length,info,other,index=struct.unpack_from('<IIIBBH',b,k)
            if not length or not 0<index<len(sections):continue
            text=names[name:names.find(b'\0',name)].decode();target=sections[index]
            start=target[4]+value-target[3];out[text]=b[start:start+length]
    return out

def main():
    build=pathlib.Path(sys.argv[1]);dest=pathlib.Path(sys.argv[2]);dest.mkdir(parents=True,exist_ok=True)
    samples={}
    for kind in ('local','upstream'):
        p=ROOT/'build-tests'/kind;p.mkdir(exist_ok=True)
        exe=ROOT/'build-tests'/('dump_'+kind+('.exe' if sys.platform=='win32' else ''))
        subprocess.run([str(exe)],cwd=p,check=True)
        samples[kind]={n:(p/(n+'.bin')).read_bytes() for n in ('device','configuration','report')}
    assert samples['local']['report']==samples['upstream']['report'],'Santroller report descriptor differs'
    symbols=elf_symbols(build/'nitro_ps3.elf')
    for n,b in samples['local'].items():assert symbols[n+'_descriptor']==b,f'{n} descriptor differs in ARM ELF'
    dev=samples['local']['device'];vid,pid,rev=struct.unpack_from('<HHH',dev,8)
    assert (vid,pid,rev)==(0x12ba,0x0210,0x0200)
    cfg=samples['local']['configuration'];assert struct.unpack_from('<H',cfg,2)[0]==len(cfg)
    eps=[];i=0
    while i<len(cfg):
        length,kind=cfg[i:i+2];assert length>0
        if kind==5:eps.append((cfg[i+2],cfg[i+3],struct.unpack_from('<H',cfg,i+4)[0],cfg[i+6]))
        if kind==0x21:assert struct.unpack_from('<H',cfg,i+7)[0]==len(samples['local']['report'])
        i+=length
    assert eps==[(1,3,64,1),(129,3,64,1)],eps
    report=samples['local']['report'];i=0;size=count=0;bits={8:0,9:0,11:0}
    while i<len(report):
        prefix=report[i];i+=1;n=(0,1,2,4)[prefix&3];v=int.from_bytes(report[i:i+n],'little');i+=n
        typ=(prefix>>2)&3;tag=prefix>>4
        if typ==1:
            if tag==7:size=v
            if tag==9:count=v
            assert tag!=8,'Unexpected report ID'
        if typ==0 and tag in bits:bits[tag]+=size*count
    assert bits=={8:216,9:64,11:256},bits
    binary=(build/'nitro_ps3.bin').read_bytes();uf2=(build/'nitro_ps3.uf2').read_bytes()
    assert len(uf2)%512==0
    payload=bytearray();blocks=len(uf2)//512
    for i in range(blocks):
        block=uf2[i*512:(i+1)*512];a,b,flags,addr,size,num,total,family=struct.unpack_from('<8I',block)
        assert (a,b)==(0x0a324655,0x9e5d5157)
        assert struct.unpack_from('<I',block,508)[0]==0x0ab16f30
        assert flags==0x2000 and family==0xe48bff56
        assert (num,total,size,addr)==(i,blocks,256,0x10000000+i*256)
        payload.extend(block[32:32+size])
    assert payload[:len(binary)]==binary and not any(payload[len(binary):])
    # SDK-generated boot2 checksum, non-reflected CRC32, initial value 0xffffffff.
    crc=0xffffffff
    for byte in binary[:252]:
        crc^=byte<<24
        for _ in range(8):crc=((crc<<1)^ (0x04c11db7 if crc&0x80000000 else 0))&0xffffffff
    assert struct.unpack_from('<I',binary,252)[0]==crc,'boot2 checksum'
    result={'status':'BUILD VERIFIED; HARDWARE NOT VERIFIED','vid':hex(vid),'pid':hex(pid),'bcdDevice':hex(rev),'report_descriptor_bytes':len(report),'report_sha256':hashlib.sha256(report).hexdigest(),'input_bytes':27,'output_bytes':8,'feature_declared_bytes':32,'feature_initialization_reply_bytes':8,'interrupt_endpoints':eps,'uf2_blocks':blocks,'bin_bytes':len(binary),'checks':['Santroller descriptor byte equality','descriptor equality in ARM ELF','HID report bit count','VID/PID/revision','endpoint type/size/interval','UF2 family/address/block structure','UF2 payload equals BIN','RP2040 boot2 checksum']}
    (dest/'verification.json').write_text(json.dumps(result,indent=2)+'\n')
    for n,b in samples['local'].items():(dest/(n+'-descriptor.bin')).write_bytes(b)
    print(json.dumps(result,indent=2))
if __name__=='__main__':main()
