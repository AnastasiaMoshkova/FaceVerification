set OUTDIR1=..\facerecognition\VerifyService\VerifyService\Proxies
rd /s /q %OUTDIR1%
mkdir %OUTDIR1%
swig -c++ -csharp -namespace facerecognition -outdir %OUTDIR1% -o .\proxies_wrap_csh.cxx proxiesface.i