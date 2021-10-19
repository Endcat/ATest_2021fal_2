# ATest_2021fal_2
stm32f103 experiment for xdu a-test

## status
经修改，完全能够实现实验所要求的功能。

~~（原来是pdf里代码的锅，屮）~~

实验代码见仓库main.c，连线图如下：

![image](https://user-images.githubusercontent.com/30371828/133403650-346d6de4-fc1a-4520-b294-1f81c019a052.png)

## how to do
- 首先要用到两个软件（Proteus 8.x/Keil uVision5)。通过搜索引擎或者官网下载安装
- 解压缩实验压缩包，直接可以打开`./ATest_2021fal_2.pdsprj`和`./ATest_2021fal_2/MDK-ARM/ATest.uvprojx`。
- 在Proteus里可以连线，在Keil里可以修改`main.c`中的代码。代码和连线图都在本仓库中（暂时有、问题）
- 记得需要修改Proteus工程配置中的hex文件路径，原工程配置的目录写成了`./ATest_2020fal_2/MDK-ARM/ATest/ATest.hex`，改成`./ATest_2021fal_2/MDK-ARM/ATest/ATest.hex`
- 还有Keil安装后有一个固件管理的程序，记得在其中找到STM32F103C6并下载安装
- 还有没想到的或者还没遇到的到时候补上

## license
别客气随便抄
