call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Auxiliary/Build/vcvars64.bat"
set PATH=C:/QtNew/5.10.1/msvc2017_64/bin;%PATH%;
cd /D D:/test1/VsHellowWord
qmake -r -tp vc "D:/test1/HellowWord/project_all.pro"
cmd
