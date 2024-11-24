
export void saxpy_ispc(uniform int N,
                       uniform float scale,
                            uniform float X[],
                            uniform float Y[],
                            uniform float result[])
{
    foreach (i = 0 ... N) {           
        result[i] = scale * X[i] + Y[i];
    }
}

// 答：1.性能大幅提高是不太可能的，因为访存太过频繁。
// 2.为什么要*4？ 因为cpu的缓存机制。
// 每当我计算时，要从内存中读取两个数存一个数，这里乘数就是3了，然后还有考虑到取数是要加载缓存和换出缓存，就是4了。
// 换入和换出缓存一般不会同时发生。
// 什么是某地址所在的高速缓存行？
// 从0x0开始，按64字节对齐，该地址所处的行即为高速缓存行。


// 定义一个任务函数，可以分配给多个线程并行执行。ISPC负责管理线程的调度和同步。
task void saxpy_ispc_task(uniform int N,
                               uniform int span,
                               uniform float scale,
                               uniform float X[], 
                               uniform float Y[],
                               uniform float result[])
{
    // taskIndex 为内置参数，表示当前任务索引
    uniform int indexStart = taskIndex * span;
    uniform int indexEnd = min(N, indexStart + span);

    foreach (i = indexStart ... indexEnd) {
        result[i] = scale * X[i] + Y[i];
    }
}

export void saxpy_ispc_withtasks(uniform int N,
                               uniform float scale,
                               uniform float X[],
                               uniform float Y[],
                               uniform float result[])
{

    uniform int span = N / 64;  // 64 tasks

    //分配给多个核心或者线程执行
    launch[N/span] saxpy_ispc_task(N, span, scale, X, Y, result);
}
