xcopy html_customized\*.htm html /S /Y
if not exist html\img md html\img
xcopy html_customized\img\*.png html\img /S /Y
xcopy html_customized\*.css html /S /Y
xcopy html_customized\TOC.hhc .\TOC.hhc /S /Y