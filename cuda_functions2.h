#ifdef __CUDACC__
extern "C" void gpuAlloc(int , int );
extern "C" void gpuLoopAlloc(int);
extern "C" void gpuFree(void);
extern "C" void gpuMemLoad(int , int , float** , float** , int** );
extern "C" void gpuMemStore(int , int , float** , float** , int** , float** );
extern "C" void gpuLoopLoad(int , int , float** , int ** );
extern "C" void gpuLoopStore(int , float** );
extern "C" void gpuLoopStore2(int , float** );
extern "C" void gpuFindMax(int , int );
extern "C" void gpuConUp(int , int , int , int );
extern "C" void gpuBackSubs(int , int , int , int , float, float** );
extern "C" void gpuFwdSubs(int , int , int , int , float );
#else 
extern void gpuAlloc(int , int );
extern void gpuLoopAlloc(int);
extern void gpuFree(void);
extern void gpuMemLoad(int , int , float** , float** , int** );
extern void gpuMemStore(int , int , float** , float** , int** , float** );
extern void gpuLoopLoad(int , int , float** , int ** );
extern void gpuLoopStore(int , float** );
extern void gpuLoopStore2(int , float** );
extern void gpuFindMax(int , int );
extern void gpuConUp(int , int , int , int );
extern void gpuBackSubs(int , int , int , int , float , float**);
extern void gpuFwdSubs(int , int , int , int , float );
#endif
