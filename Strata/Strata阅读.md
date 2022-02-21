### 一、背景

#### 存储介质：

**NVM、SSD、HDD**，如下图所示，NVM的读写延迟最低，但目前的技术中容量较小，价格较高；SSD处于中间位置；HDD发展最久，读写延迟由于机械开销而最高，但能满足大容量，且价格最低。想要具有高性能且低成本，必须混合利用多种设备类型。

![image-20211203232923337](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20211203232923337.png)



#### 传统方法:

目前，传统的文件系统基本都是对单一存储介质进行设计，提供统一的设备接口。

但是，现有的基于单一介质的文件系统不可以同时满足容量、性能、高性价比的需求。

#### 本文方法：

所以本文提出了一种结合多种介质混合的文件系统，包含NVM、SSD、HDD三种介质的文件系统。

Strata，为了更好的利用不同存储介质的特性，对用户态和内核态的职责进行了重新划分。

### 二、Strata目标

#### 传统劣势：

1、对单一存储介质进行设计

2、存在写放大问题，对于NVM而言，写放大问题尤其明显。

#### 设计目标：

1、快速写-支持快速、随机、小数据量的写入。

2、在同步过程中减少性能损失。

3、管理写放大

3、支持多个线程并发。支持从单个进程中的多个线程进行并发日志记录。

4、为整个底层存储结构的所有设备提供统一的文件系统接口

### 三、Strata结构

#### 主要概念解释：

![image-20211204140647143](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20211204140647143.png)

1、Update log：对于每个进程的文件系统更新

2、Shared area：在NVM、SSD、HDD中保存文件系统数据，用户只读，写操作交给内核

3、File data cache：文件数据缓存 ，这是一个只读性的缓存，缓存SSD、HDD中的数据

4、Update log pointers： 更新日志指针。更新日志的索引结构，用于文件与日志块之间的映射

5、Strata transaction：一个持久性单元，用于处理系统调用产生的文件系统更改

6、Digest：将日志中的更改更新到共享区域

7、Lease：同步目录和文件的更新

![image-20211203234044740](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20211203234044740.png)

1、LibFS

Strata的库文件系统（LibFS）提供了应用程序级别机制来执行文件IO。 其目标是为整个底层存储层次结构提供快速、崩溃一致和同步的读写IO，与现有POSIX应用程序完全兼容的统一API，并通过与LibFS重新链接将其置于应用程序之下。

2、Shared Kernel FS

