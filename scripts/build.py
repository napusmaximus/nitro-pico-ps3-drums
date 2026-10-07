#!/usr/bin/env python3
"""Pinned source bootstrap, native tests, firmware build, binary audit. No third-party Python modules."""
import argparse, hashlib, json, os, pathlib, shutil, subprocess, sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
os.chdir(ROOT)
LOCK=json.loads((ROOT/'dependencies.lock.json').read_text())
def run(*args, cwd=ROOT):
    print('+', ' '.join(map(str,args)),flush=True)
    subprocess.run(list(map(str,args)),cwd=cwd,check=True)
def revision(path):
    return subprocess.check_output(['git','-C',str(path),'rev-parse','HEAD'],text=True).strip()
def dependency(name):
    p=ROOT/'.deps'/name; spec=LOCK[name]
    if not p.exists():
        p.mkdir(parents=True); run('git','init',p)
        run('git','-C',p,'fetch','--depth','1',spec['url'],spec['revision'])
        run('git','-C',p,'checkout','--detach','FETCH_HEAD')
    if revision(p)!=spec['revision']: raise RuntimeError(f'{name} revision mismatch; preserve local edits and restore the locked revision')
    if subprocess.check_output(['git','-C',str(p),'status','--porcelain','--untracked-files=no'],text=True).strip():
        raise RuntimeError(f'{name} has modified tracked files')
    return p

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--debug',action='store_true');ap.add_argument('--pro',action='store_true');ap.add_argument('--toolchain',help='Arm toolchain root (contains bin/)');a=ap.parse_args()
    sdk=dependency('pico-sdk');run('git','-C',sdk,'submodule','update','--init','lib/tinyusb')
    if revision(sdk/'lib/tinyusb')!=LOCK['tinyusb']['revision']: raise RuntimeError('TinyUSB revision mismatch')
    pt=dependency('picotool')
    toolchain=a.toolchain or os.getenv('PICO_TOOLCHAIN_PATH')
    if not toolchain:
        local=list((ROOT/'.deps').glob('arm-gnu-toolchain-14.2.rel1-*-arm-none-eabi'))
        if local:toolchain=str(local[0])
    cc=pathlib.Path(toolchain)/'bin'/('arm-none-eabi-gcc.exe' if os.name=='nt' else 'arm-none-eabi-gcc') if toolchain else 'arm-none-eabi-gcc'
    libc=subprocess.check_output([str(cc),'-print-file-name=libc.a'],text=True).strip()
    if libc=='libc.a' or not pathlib.Path(libc).exists():raise RuntimeError('Install the complete Arm GNU 14.2.Rel1 toolchain including newlib, then pass --toolchain ROOT')
    run('cmake','-S',pt,'-B','build-picotool','-G','Ninja',f'-DPICO_SDK_PATH={sdk}','-DPICOTOOL_NO_LIBUSB=1','-DPICOTOOL_FLAT_INSTALL=1',f'-DCMAKE_INSTALL_PREFIX={ROOT/".deps/picotool-install"}')
    run('cmake','--build','build-picotool','--parallel','4');run('cmake','--install','build-picotool')
    # Independent descriptor reference: remove only unrelated includes from the full upstream header.
    gen=ROOT/'build-tests/generated';gen.mkdir(parents=True,exist_ok=True)
    text=(ROOT/'third_party/santroller/hid_reports.h').read_text()
    (gen/'upstream_descriptor.h').write_text('\n'.join(x for x in text.splitlines() if not x.startswith('#include')))
    source=(ROOT/'third_party/santroller/ps3.hpp').read_text()
    end=source.index('} __attribute__((packed)) PS3RockBandDrums_Data_t;')+len('} __attribute__((packed)) PS3RockBandDrums_Data_t;')
    start=source.rfind('typedef struct',0,end)
    (gen/'upstream_ps3_drums.h').write_text('#include <stdint.h>\n'+source[start:end]+'\n')
    run('cmake','-S','tests','-B','build-tests','-G','Ninja');run('cmake','--build','build-tests');run('ctest','--test-dir','build-tests','--output-on-failure')
    variant=('pro' if a.pro else 'standard')+('-debug' if a.debug else '')
    b=ROOT/f'build-{variant}';dest=ROOT/'dist'/variant;dest.mkdir(parents=True,exist_ok=True)
    args=['cmake','-S',ROOT,'-B',b,'-G','Ninja','-DCMAKE_BUILD_TYPE=Release',f'-DNITRO_PRO_DRUMS={"ON" if a.pro else "OFF"}',f'-DNITRO_DEBUG={"ON" if a.debug else "OFF"}']
    if toolchain:args.append(f'-DPICO_TOOLCHAIN_PATH={pathlib.Path(toolchain).resolve()}')
    run(*args);run('cmake','--build',b,'--parallel','4')
    run(sys.executable,'scripts/verify.py',b,dest)
    for ext in ('elf','bin','uf2'):shutil.copy2(b/f'nitro_ps3.{ext}',dest/f'nitro_ps3.{ext}')
    run(ROOT/'.deps/picotool-install/picotool'/('picotool.exe' if os.name=='nt' else 'picotool'),'info','-a',dest/'nitro_ps3.uf2')
    manifest={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in dest.iterdir() if p.suffix in ('.elf','.bin','.uf2')}
    (dest/'SHA256SUMS').write_text(''.join(f'{h}  {n}\n' for n,h in sorted(manifest.items())))
    print(f'BUILD VERIFIED: {dest}. HARDWARE NOT VERIFIED.')
if __name__=='__main__':main()
