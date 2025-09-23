.section .text.boot
.global _start
_start:
    // 清除中断
    li t0 , 0
    csrw mstatus , t0

    csrr tp, mhartid # 读取hartid暂存到tp中
    bnez tp, _wait_hart

    # 栈设置
    la t0 , __stack_start
    la t1 , __stack_end
    sub t0 , t1 , t0 # t0 = STACK_SIZE
    mul t1 , tp , t0 # t1 = hartid * STACK_SIZE 
    la t2, __stack_end
    sub sp,t2,t1 # 栈底 = _stack_end - hartid * STACK_SIZE

    mv a0, tp # 将hartid传递给init函数
    j init

_wait_hart:
    wfi


.end



