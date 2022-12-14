@set include=
del /q vm
mkdir vm
cd vm
set cc=C:\PRGRMMNG\SoF2SDK\bin\sof2lcc -A -DQ3_VM -DMISSIONPACK -S -Wf-target=bytecode -Wf-g -I..\..\..\gametype -I..\..\gt_inf -I..\..\..\game %1

%cc%  ../gt_main.c
@if errorlevel 1 goto quit

%cc%  ../../gt_syscalls.c
@if errorlevel 1 goto quit

%cc%  ../../../game/bg_lib.c
@if errorlevel 1 goto quit

%cc%  ../../../game/q_shared.c
@if errorlevel 1 goto quit

%cc%  ../../../game/q_math.c
@if errorlevel 1 goto quit

C:\PRGRMMNG\SoF2SDK\bin\sof2asm -f ../gt_inf
@if errorlevel 1 goto quit

mkdir "..\..\..\..\base\vm"
copy *.map "..\..\..\..\base\vm"
copy *.qvm "..\..\..\..\base\vm"

:quit
cd ..
