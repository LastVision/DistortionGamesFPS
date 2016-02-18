rd /s /q temp
rd /s /q Solution\ipch

del Solution\FPS.sdf
del /A:H Solution\FPS.suo

rd /s /q Tools\LogReader\LogReader\LogReader\obj
rd /s /q Tools\ModelViewer\ModelViewer\obj
rd /s /q Tools\EntityEditor\EntityEditor\obj
rd /s /q Tools\CSharpUtilities\CSharpUtilities\obj
rd /s /q Tools\ParticleEditor\ParticleEditor\obj
rd /s /q Tools\ToolBuilder\ToolBuilder\obj
rd /s /q Libs