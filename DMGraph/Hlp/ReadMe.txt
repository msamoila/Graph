How to build the .CHM:

1. open DMGraph.t2c in Tlb2Html
- make sure "Import Library" is DMGraph.tlb (General and Colors tab)
- generate the output folder tree

2. Make sure you have built the utility .EXEs: ChmAppend.EXE, HhcAppend.EXE and Html2ChmFix.EXE.
(source code in on VSS, workspace AFTERHTML2CHM.dsw, use MS Visual C++ 6.0)

3. run 1_Html2ChmFix.bat
You can see what was changed in Html2ChmFix.txt.
The Html2ChmFix.exe is fixing some bugs found in Tlb2Html application. 
The fix is to modify the generated .HTML files by writing the correct syntax for COM methods and properties. 
Each change is logged in Html2ChmFix.txt.

4. run 2_Customize.bat
This will copy all *.htm files from html_customized to html folder.

5. run 3_BuildAll.bat
this will start the CHM compiler for html folder.

