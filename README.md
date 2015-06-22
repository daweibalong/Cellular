# Cellular
1、Cellular是一个基于lua5.1的类Actor模型的简单实现。

2、实现了在单个lua_State中加载多个独立的脚本沙箱环境。

3、因为没有对全局环境的写权限，各沙箱之间相互不影响。

4、因为各沙箱对全局环境有读权限，各沙箱可以共享全局环境，避免lua基础库和公共脚本的重复加载。

5、在调度的过程中通过lua hook的方式实现任务的抢占式调度。

## Build && Test
```
cd Cellular && make
./testcase
```
``````
