# ATest_2021fal_2
stm32f103 experiment for xdu a-test

## status
~~（原来是实验指南里代码的锅，屮）~~

实验代码见仓库main.c，连线图如下：

![image](https://user-images.githubusercontent.com/30371828/133403650-346d6de4-fc1a-4520-b294-1f81c019a052.png)

## tips
- 首先要用到两个软件（Proteus 8.x/Keil uVision5)。通过搜索引擎或者官网下载安装
- 解压缩实验压缩包，直接可以打开`./ATest_2021fal_2.pdsprj`和`./ATest_2021fal_2/MDK-ARM/ATest.uvprojx`。
- 在Proteus里可以连线，在Keil里可以修改`main.c`中的代码。代码和连线图都在本仓库中
- 记得需要修改Proteus工程配置中的hex文件路径，原工程配置的目录写成了`./ATest_2020fal_2/MDK-ARM/ATest/ATest.hex`，改成`./ATest_2021fal_2/MDK-ARM/ATest/ATest.hex`
- 还有Keil安装后有一个固件管理的程序，记得在其中找到STM32F103C6并下载安装
- 在Proteus里启动模拟的时候（就是按下左下角的那个开始按钮），可能会触发断点（原因不明，汇编代码里直接就有一行bkpt）。遇到这种情况多按几下开始按钮就行。
- 代码里标注了需要修改的地方，提出了一些调试过程中发现的小问题，可以看看修改一下并写在你的实验报告里。

## license
随便抄协议
